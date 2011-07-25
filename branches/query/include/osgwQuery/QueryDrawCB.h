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

#ifndef __OSGWQUERY_QUERY_DRAW_CB_H__
#define __OSGWQUERY_QUERY_DRAW_CB_H__ 1


#include <osgwQuery/Export.h>
#include <osgwQuery/QueryObject.h>
#include <osg/Drawable>
#include <OpenThreads/Mutex>

#include <osg/Node>
#include <osg/Geode>
#include <osg/NodeVisitor>
#include <osg/buffered_value>


namespace osgwQuery
{


/** \class QueryDrawCB QueryDrawCB.h <osgwQuery/QueryDrawCB.h>
\brief A multi-inherited draw/cull callback that supports occlusion query.

This class attempts to implement the algorithm described in "Near Optimal
Hierarchical Culling: Performance Driven Use of Hardware Occlusion Queries"
by Guthke, Balázs, and Klein, Eurographics 2006.
*/
class OSGWQUERY_EXPORT QueryDrawCB : public osg::Drawable::DrawCallback,
    public osg::Drawable::CullCallback
{
public:
    QueryDrawCB();
    QueryDrawCB( const QueryDrawCB& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    META_Object(osgwQuery,QueryDrawCB);

    virtual void drawImplementation( osg::RenderInfo& renderInfo, const osg::Drawable* drawable ) const;

    virtual bool cull( osg::NodeVisitor* nv, osg::Drawable* drawable, osg::RenderInfo* renderInfo ) const;

    void setNumVertices( unsigned int numVertices ) { _numVertices = numVertices; }
    unsigned int getNumVertices() const { return( _numVertices ); }

protected:
    mutable osg::ref_ptr< osg::Drawable > _queryDrawable;

    mutable OpenThreads::Mutex _lock;

    mutable bool _initialized;
    void init( osg::BoundingBox bb ) const;

    unsigned int _numVertices;

    // Guthke algorithm constants and variables
    //   Surface area of the bounding box divided by 6.
    mutable double _AbbOiOver6;
    //   Ratio: actual object coverage / bounding box coverage.
    mutable double _RcovOi;
    //   Taken from CullVisitor:
    //     Distance from the Drawble to the viewpoint.
    mutable double _dOi;
    //     Width and height of the viewport.
    mutable double _w, _h;
    //     Half of the vertical field of view.
    mutable double _thetaOver2;


    class QueryStatus
    {
    public:
        QueryStatus();

        bool _queryActive;
        bool _wasOccluded;
        osg::ref_ptr< osgwQuery::QueryObject > _queryObject;
    };
    mutable osg::buffered_object< QueryStatus > _queries;
};



class OSGWQUERY_EXPORT AddDrawCB : public osg::NodeVisitor
{
public:
    AddDrawCB( osg::NodeVisitor::TraversalMode mode=osg::NodeVisitor::TRAVERSE_ALL_CHILDREN )
      : osg::NodeVisitor( mode )
    {}

    virtual void apply( osg::Geode& node );

    static double getCscrOi() { return( s_CscrOi ); }
    static void setCscrOi( double c ) { s_CscrOi = c; }

protected:
    static double s_CscrOi;
};


// osgwQuery
}

// __OSGWQUERY_QUERY_DRAW_CB_H__
#endif
