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

#include <osgwQuery/QueryUtils.h>
#include <osgwQuery/QueryAPI.h>
#include <osgwQuery/QueryBenchmarks.h>
#include <osgwTools/Shapes.h>
#include <osgwTools/CountsVisitor.h>
#include <osgwTools/Transform.h>
#include <OpenThreads/ScopedLock>
#include <osgUtil/CullVisitor>
#include <osg/ComputeBoundsVisitor>
#include <osg/PolygonOffset>
#include <osg/ColorMask>
#include <osg/Depth>

#include <osg/io_utils>


namespace osgwQuery
{


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

void QueryCullCallback::attach( osg::Node* node, osgwQuery::QueryComputation* nd, osg::BoundingBox bb )
{
    _node = node;
    _nd = nd;
    _bb = bb;
}



CameraResetCallback::CameraResetCallback()
{
}
void CameraResetCallback::operator()( osg::RenderInfo& renderInfo ) const
{
    osg::Camera* cam = renderInfo.getCurrentCamera();
    unsigned int contextID = renderInfo.getState()->getContextID();
    QueryComputation::setCscrOi( 0., cam, contextID );
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

    // Do not add callbacks to redundant Groups because the parent Group's
    // bounding volume (and query geometry) would also be redundant.
    bool redundantGroup( true );
    unsigned int idx;
    for( idx=0; idx < node.getNumParents(); idx++ )
    {
        if( node.getParent( idx )->getNumChildren() > 1 )
        {
            redundantGroup = false;
            break;
        }
    }
    if( redundantGroup )
    {
        traverse( node );
        return;
    }

    // Create QueryComputation for this node.
    // Add q QueryStats only if a) we have one and
    // b) the node addresses match.
    osgwQuery::QueryStats* debugStats( NULL );
    if( ( _qs != NULL ) && ( &node == _qs->getNode() ) )
        debugStats = _qs;
    QueryComputation* nd = new QueryComputation( debugStats );

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

    _queryCount++;

    traverse( node );
}
void AddQueries::apply( osg::Geode& node )
{
    traverse( node );
}
void AddQueries::apply( osg::Camera& node )
{
    if( node.getCullCallback() != NULL )
    {
        traverse( node );
        return;
    }

    CameraResetCallback* crc = new CameraResetCallback();
    // TBD use the osgWorks composite post-draw callback.
    node.setPostDrawCallback( crc );

    traverse( node );
}


// osgwQuery
}
