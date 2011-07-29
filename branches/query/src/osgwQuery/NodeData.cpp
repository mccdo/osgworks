/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2009-2011 by Kenneth Mark Bryden
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *************** <auto-copyright.pl END do not edit this line> ***************/

#include <osgwQuery/NodeData.h>
#include <osgwQuery/QueryAPI.h>
#include <osgwQuery/QueryBenchmarks.h>
#include <osgwTools/Shapes.h>
#include <osgwTools/CountsVisitor.h>
#include <osgwTools/Transform.h>
#include <OpenThreads/ScopedLock>
#include <osgUtil/CullVisitor>
#include <osg/ComputeBoundsVisitor>

#include <osg/io_utils>


namespace osgwQuery
{


double AddQueries::s_CscrOi( 0. );


NodeData::NodeData( osgwQuery::QueryStats* debugStats )
  : _initialized( false ),
    _numVertices( 0 ),
    _lastQueryFrame( 0 ),
    _lastCullFrame( 0 ),
    _debugStats( debugStats )
{
}
NodeData::NodeData( const NodeData& rhs, const osg::CopyOp& copyop )
  : _initialized( false ),
    _numVertices( rhs._numVertices ),
    _lastQueryFrame( rhs._lastQueryFrame ),
    _lastCullFrame( rhs._lastCullFrame )
{
}


bool NodeData::cullOperation( osg::NodeVisitor* nv, osg::RenderInfo& renderInfo, const osg::BoundingBox& bb )
{
    if( !_initialized )
    {
        OpenThreads::ScopedLock< OpenThreads::Mutex > lock( _lock );

        if( !( _queryDrawable.valid() ) )
            init( bb, nv );
        _initialized = true;
    }


    osgUtil::CullVisitor* cv = static_cast< osgUtil::CullVisitor* >( nv );
    osg::RefMatrix* view = cv->getModelViewMatrix();
    const osg::Camera* cam = cv->getCurrentCamera();

    // Obtain cull-time constants (possibly different per cull in multidisplay).
    //     Distance from the Drawble to the viewpoint.
    const double dOi = cv->getDistanceFromEyePoint( _worldBB.center(), false );
    //     Width and height of the viewport.
    const osg::Viewport* vp = cam->getViewport();
    const double width = vp->width();
    const double height = vp->height();
    //     Half of the vertical field of view.
    osg::Matrix proj = cam->getProjectionMatrix();
    double fovy, aspect, zNear, zFar;
    proj.getPerspective( fovy, aspect, zNear, zFar );
    const double thetaOver2 = osg::DegreesToRadians( fovy * .5 );


    // We should only be here if the cull visitor visits our associated Node.
    // That should only happen if our Node isn't frustum culled. Determine
    // if the current frame# is significantly different from the last time
    // we were here, then record the current frame#.
    const unsigned int currentFrame = nv->getFrameStamp()->getFrameNumber();
    bool wasFrustumCulled( _lastCullFrame < currentFrame - 2 );
    _lastCullFrame = currentFrame;
    // Note that we will explicitly set wasFrustumCulled=false
    // if it turns out we have an active query.

    unsigned int contextID = renderInfo.getState()->getContextID();
    osgwQuery::QueryBenchmarks* qb = osgwQuery::getQueryBenchmarks( contextID, &renderInfo );
    osgwQuery::QueryAPI* qapi = osgwQuery::getQueryAPI( contextID );
    QueryStatus& qs = _queries[ contextID ];

    // Retrieve any pending query result.
    if( qs._queryActive )
    {
        GLuint result;
        const GLuint id = qs._queryObject->getID( contextID );
        qapi->glGetQueryObjectuiv( id, GL_QUERY_RESULT, &result );
        osg::notify( osg::INFO ) << " ID: " << id << " Result: " << result << ", numV " << _numVertices << std::endl;
        qs._wasOccluded = ( result < 25 ); // TBD need configurable threshold.
        qs._queryActive = false;

        // If query was active, we were _not_ frustum culled.
        wasFrustumCulled = false;
    }

    // Report if frustum culled
    if( _debugStats.valid() && wasFrustumCulled )
        _debugStats->incFrustum();

    const double cbbOiSqRt = 1. / ( dOi * sqrt( width / height ) * 2. * tan( thetaOver2 ) );
    const double cbbOi = cbbOiSqRt * cbbOiSqRt * _AbbOiOver6;

    // Compute pcovOi, the probability that this Node is covered.
    double pcovOi;
    const double cscrOi = AddQueries::getCscrOi();
    if( cbbOi < cscrOi )
    {
        double temp = sqrt( cscrOi ) - sqrt( cbbOi );
        pcovOi = temp * temp;
    }
    else
        pcovOi = 0.;

    // Compute pocclOi, the probability that the object is occluded,
    // accounting for temporal coherence. "p sub occl ( O sub i )" is
    // referred to as "p sub o ( O sub i )" after section 3.1, and
    // (in reference to a hierarchy) also as "p sub o ( H sub i )".
    // This is from personal communication with author Michael Guthke.
    double pocclOi;
    if( !qs._wasOccluded )
        // previously visible
        pocclOi = 0.5 * pcovOi * pcovOi;
    else if( wasFrustumCulled )
        pocclOi = pcovOi;
    else /* previously occluded */
        pocclOi = 1.;
    if( _debugStats.valid() )
        _debugStats->setPoccl( (float)pocclOi );



    /** Implements the Guthke paper'a "QueryReasonable" function, pseudocode
    in Guthke Figure 5, described in Guthke section 4. */
    bool queryReasonable( false );


    double cOi = cbbOi / _RcovOi;
    // TBD need max of triangle cost or fragment cost.
    const double pixels = width * height * cOi;
    const double fragRenderTime = pixels * qb->_trFragment;
    // TBD QueryBenchmark should time per-vertex cost.
    // TBD vertRenderTime is a constant.
    const double vertRenderTime = _numVertices * qb->_trTriangle;
    const double renderTime = osg::maximum< double >( fragRenderTime, vertRenderTime ) + qb->_trSetup;
    const double queryTime = pixels * qb->_toFragment;// + qb->_toSetup;

    // TBD hardcoded literanl should be set by application or detected automatically with
    // a node visitor.
    const double sceneDepth( 3. );
    const double nodeCost = queryTime;
    const double nodeBenefit = pocclOi * ( renderTime - queryTime ) / sceneDepth;

    unsigned int framesSinceLastQuery = currentFrame - _lastQueryFrame;

    if( ( renderTime < queryTime ) /* ||
            ( sum of child costs < node cost ) */ )
    {
        queryReasonable = false;
        osg::notify( osg::INFO ) << "Case 1: False. renderTime < queryTime" << std::endl;

        if( _debugStats.valid() )
            _debugStats->incRtLessQt();
    }

    else if( qs._wasOccluded )
    {
        queryReasonable = true;
        osg::notify( osg::INFO ) << "Case 2: True. Was occluded" << std::endl;

        if( _debugStats.valid() )
            _debugStats->incOccluded();
    }

    else if( nodeCost > ( framesSinceLastQuery * nodeBenefit ) )
    {
        queryReasonable = false;
        osg::notify( osg::INFO ) << "Case 3: False. cost > benefit" << std::endl;

        if( _debugStats.valid() )
            _debugStats->incCGreaterB();
    }

    else
    {
        queryReasonable = true;
        osg::notify( osg::INFO ) << "Case 4: True. default" << std::endl;
    }
    /*
    for each child
        if child render time < child query time
            return true;
    if node benefit > sum of child benefits &&
            ( query overhead time * # of node leaves ) > node query time
        return true;
    */


    if( queryReasonable )
    {
        cv->updateCalculatedNearFar( *view, *_queryDrawable, false );

        // Issue query. Add the query drawable to the render graph just as
        // if CullVisitor had encountered it while processing a Geode.
        const float depth = _worldBB.valid() ? cv->getDistanceFromEyePoint( _worldBB.center(), false ) : 0.f;
        if( osg::isNaN( depth ) )
        {
            osg::notify(osg::NOTICE)<<"NodeData: detected NaN,"<<std::endl
                                    <<"    depth="<<depth<<", center=("<<_worldBB.center()<<"),"<<std::endl;
        }
        else
        {
            osg::notify( osg::INFO ) << "Issued query" << std::endl;
            cv->addDrawableAndDepth( _queryDrawable.get(), view, depth );
        }

        _lastQueryFrame = currentFrame;

        if( _debugStats.valid() )
            _debugStats->incQueries();
    }


    // Increment CscrOi for next Drawable.
    AddQueries::setCscrOi( cscrOi + ( ( 1. - cscrOi ) * cbbOi ) );

    osg::notify( osg::INFO ) << "  Was occluded? " << std::boolalpha << qs._wasOccluded << " numV " << _numVertices << std::endl;
    return( !qs._wasOccluded );
}


void NodeData::init( osg::BoundingBox bb, osg::NodeVisitor* nv )
{
    _worldBB = osgwTools::transform( osg::computeLocalToWorld( nv->getNodePath() ), bb );

    osg::Vec3 extents = bb._max - bb._min;


    //
    // Create box geometry for use as query geometry.

    osg::Vec3 halfExtents = extents / 2.;
    osg::Geometry* geom = osgwTools::makeBox( halfExtents );

    QueryDrawCallback* qdc = new QueryDrawCallback();
    qdc->attach( geom, this );
    geom->setDrawCallback( qdc );

    // TBD osgwTools Shapes really does need to support non-origin centers.
    // Offset the query geometry box by the bounding box center.
    osg::Vec3Array* v = static_cast< osg::Vec3Array* >( geom->getVertexArray() );
    const osg::Vec3 center = bb.center();
    unsigned int idx;
    for( idx=0; idx<v->size(); idx++ )
        (*v)[ idx ] += center;

    // Optimize.
    // TBD Possibly display lists are better? Need to try on higher-end hardware.
    geom->setUseDisplayList( false );
    geom->setUseVertexBufferObjects( true );
    geom->setTexCoordArray( 0, NULL );
    geom->setNormalArray( NULL );
    geom->setNormalBinding( osg::Geometry::BIND_OFF );

    _queryDrawable = geom;


    //
    // Compute and store constants defined in Guthke, section 3.1.

    // 1/6th of the bounding box surface area.
    const double abbOi = ( 2. * extents[ 0 ] * extents[ 1 ] ) +
        ( 2. * extents[ 1 ] * extents[ 2 ] ) +
        ( 2. * extents[ 2 ] * extents[ 0 ] );
    _AbbOiOver6 = abbOi / 6.;

    // Ratio of actual object screen area to boulding box screen area.
    const double aOi = 4. * osg::PI * bb.radius() * bb.radius();
    _RcovOi = ( 3. / 2. ) * ( aOi / abbOi );
}



NodeData::QueryStatus::QueryStatus()
  : _queryActive( false ),
    _wasOccluded( false ),
    _queryObject( new QueryObject() )
{
}



QueryCullCallback::QueryCullCallback()
  : osg::NodeCallback(),
    _node( NULL ),
    _nd( NULL )
{
}
QueryCullCallback::QueryCullCallback( const QueryCullCallback& rhs, const osg::CopyOp& copyop )
  : osg::NodeCallback( rhs ),
    _node( rhs._node ),
    _nd( rhs._nd )
{
}

void QueryCullCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if( ( _node == NULL ) || ( _nd == NULL ) )
        return;

    osgUtil::CullVisitor* cv = static_cast< osgUtil::CullVisitor* >( nv );
    osg::RenderInfo& renderInfo = cv->getRenderInfo();

    bool traverseChildren = _nd->cullOperation( nv, renderInfo, _bb );
    if( !traverseChildren )
        return;

    traverse( node, nv );
}

void QueryCullCallback::attach( osg::Node* node, osgwQuery::NodeData* nd, osg::BoundingBox bb )
{
    _node = node;
    _nd = nd;
    _bb = bb;
}



QueryDrawCallback::QueryDrawCallback()
  : osg::Drawable::DrawCallback(),
    _drawable( NULL ),
    _nd( NULL )
{
}
QueryDrawCallback::QueryDrawCallback( const QueryDrawCallback& rhs, const osg::CopyOp& copyop )
  : osg::Drawable::DrawCallback( rhs ),
    _drawable( rhs._drawable ),
    _nd( rhs._nd )
{
}

void QueryDrawCallback::drawImplementation( osg::RenderInfo& renderInfo, const osg::Drawable* drawable ) const
{
    if( ( _drawable == NULL ) || ( _nd == NULL ) )
        return;


    const unsigned int contextID = renderInfo.getState()->getContextID();
    osgwQuery::QueryAPI* qapi = osgwQuery::getQueryAPI( contextID );

    NodeData::QueryStatus& qs = _nd->getQueryStatus( contextID );
    const GLuint id = qs._queryObject->getID( contextID );
    osg::notify( osg::INFO ) << " ID: " << id << std::endl;
    qapi->glBeginQuery( GL_SAMPLES_PASSED, id );

    glPolygonOffset( -1., -1. );
    glEnable( GL_POLYGON_OFFSET_FILL );
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    glDepthMask( GL_FALSE );
    _drawable->drawImplementation( renderInfo );
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDepthMask( GL_TRUE );
    glDisable( GL_POLYGON_OFFSET_FILL );

    qapi->glEndQuery( GL_SAMPLES_PASSED );
    qs._queryActive = true;
}

void QueryDrawCallback::attach( osg::Drawable* drawable, osgwQuery::NodeData* nd )
{
    _drawable = drawable;
    _nd = nd;
}



void AddQueries::apply( osg::Group& node )
{
    if( node.getName() == std::string( "__QueryStats" ) )
        // This is the QueryStats subtree. Don't instrument it with any OQ stuff.
        return;

    if( node.getCullCallback() != NULL )
    {
        traverse( node );
        return;
    }

    // Create NodeData for this node.
    // Add q QueryStats only if a) we have one and
    // b) the node addresses match.
    osgwQuery::QueryStats* debugStats( NULL );
    if( ( _qs != NULL ) && ( &node == _qs->getNode() ) )
        debugStats = _qs;
    NodeData* nd = new NodeData( debugStats );

    osgwTools::CountsVisitor cv;
    node.accept( cv );
    nd->setNumVertices( cv.getVertices() );

    osg::ComputeBoundsVisitor cbv;
    node.accept( cbv );
    osg::BoundingBox bb = cbv.getBoundingBox();
    // We do not want a transformed bounding box, because the bb is used to create the
    // query geometry. If this node is a transform, this would result in the transform
    // being applied twice, rendering the geometry in the wrong location.
    osg::Transform* transform = node.asTransform();
    if( transform != NULL )
    {
        // Remove the transform from the bounding box.
        osg::Matrix m;
        transform->computeLocalToWorldMatrix( m, NULL );
        m.invert( m );
        bb = osgwTools::transform( m, bb );
    }

    QueryCullCallback* qcc = new QueryCullCallback();
    qcc->attach( &node, nd, bb );
    node.setCullCallback( qcc );

    traverse( node );
}
void AddQueries::apply( osg::Geode& node )
{
    traverse( node );
}


// osgwQuery
}
