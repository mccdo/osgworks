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

#ifndef __OSGWQUERY_QUERY_COMPUTATION_H__
#define __OSGWQUERY_QUERY_COMPUTATION_H__ 1


#include <osgwQuery/Export.h>
#include <osgwQuery/QueryObject.h>
#include <osgwQuery/QueryStats.h>
#include <osg/Drawable>
#include <OpenThreads/Mutex>

#include <osg/Node>
#include <osg/Geode>
#include <osg/NodeVisitor>
#include <osg/buffered_value>

#include <map>


namespace osgwQuery
{


/** \class QueryComputation QueryComputation.h <osgwQuery/QueryComputation.h>
\brief A support struct for the Guthe occlusion query algorithm.

This class attempts to implement the algorithm described in "Near Optimal
Hierarchical Culling: Performance Driven Use of Hardware Occlusion Queries"
by Guthe, Balázs, and Klein, Eurographics 2006.
*/
class OSGWQUERY_EXPORT QueryComputation : public osg::Object
{
public:
    QueryComputation( osgwQuery::QueryStats* debugStats=NULL );
    QueryComputation( const QueryComputation& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    META_Object(osgwQuery,QueryComputation);

    /** \brief Implements Guthe algorithm and tells calling code whether to render children.
    NOTE: Guthe assumes a concurrent cull/draw with an active rendering context. The
    cullOperation() method requires an active context in order to retrieve the active query
    result. If you call this function without an active context, results are undefined.
    Many osgViewer threading models do *not* have an active rendering context during cull
    and will not work with this algorithm.

    If the algorithm determines that a query is reasonable, it will insert a Drawable into
    the render graph and return false. The Drawable represents the bounding box of the
    node's subgraph and uses a draw callback to issue the OpenGL query commands.

    \return true if the calling node should continue traversing its children and adding
    them to the render graph. False if the calling code should not add its children to
    the render graph. */
    virtual bool cullOperation( osg::NodeVisitor* nv, osg::RenderInfo& renderInfo, const osg::BoundingBox& bb );

    static double getCscrOi( const osg::Camera* cam, unsigned int contextID );
    static void setCscrOi( double c, const osg::Camera* cam, unsigned int contextID );

    void setNumVertices( unsigned int numVertices ) { _numVertices = numVertices; }
    unsigned int getNumVertices() const { return( _numVertices ); }

    class QueryStatus
    {
    public:
        QueryStatus();

        bool _queryActive;
        bool _wasOccluded;
        osg::ref_ptr< osgwQuery::QueryObject > _queryObject;
    };
    QueryStatus& getQueryStatus( unsigned int contextID ) { return( _queries[ contextID ] ); }


protected:
    void init( osg::BoundingBox bb, osg::NodeVisitor* nv );
    bool _initialized;

    OpenThreads::Mutex _lock;

    osg::ref_ptr< osg::Drawable > _queryDrawable;
    static osg::ref_ptr< osg::StateSet > s_queryStateSet;

    unsigned int _numVertices;
    osg::BoundingBox _worldBB;

    // Guthe algorithm constants and variables.
    // Note: Must hold _lock when writing these values during cull.
    //   Surface area of the bounding box divided by 6.
    double _AbbOiOver6;
    //   Ratio: actual object coverage / bounding box coverage.
    double _RcovOi;
    //   Last frame for which a query was issued.
    unsigned int _lastQueryFrame;
    // Used to determine if the node was frustum culled. This is a Guthe
    // constant (indirectly).
    unsigned int _lastCullFrame;

    // Guthe variable to track accumulated coverage during front-to-back
    // rendering. Must be set to 0. at the start of each frame.
    typedef std::pair< const osg::Camera*, unsigned int /*ctx*/ > CameraContext;
    typedef std::map< CameraContext, double > CscrOiMap;
    static CscrOiMap s_CscrOiMap;


    osg::buffered_object< QueryStatus > _queries;

    osg::ref_ptr< osgwQuery::QueryStats > _debugStats;
};



class OSGWQUERY_EXPORT QueryDrawCallback : public osg::Drawable::DrawCallback
{
public:
    QueryDrawCallback();
    QueryDrawCallback( const QueryDrawCallback& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    META_Object(osgwQuery,QueryDrawCallback);

    virtual void drawImplementation( osg::RenderInfo& renderInfo, const osg::Drawable* drawable ) const;

    void attach( osg::Drawable* drawable, osgwQuery::QueryComputation* nd );

protected:
    osg::Drawable* _drawable;
    osgwQuery::QueryComputation* _nd;
};


// osgwQuery
}

// __OSGWQUERY_QUERY_COMPUTATION_H__
#endif
