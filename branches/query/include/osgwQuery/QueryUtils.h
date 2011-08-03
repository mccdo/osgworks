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

#ifndef __OSGWQUERY_QUERY_UTILS_H__
#define __OSGWQUERY_QUERY_UTILS_H__ 1


#include <osgwQuery/Export.h>
#include <osgwQuery/QueryComputation.h>
#include <osgwQuery/QueryObject.h>
#include <osgwQuery/QueryStats.h>
#include <osg/Drawable>
#include <OpenThreads/Mutex>

#include <osg/Node>
#include <osg/Geode>
#include <osg/NodeVisitor>
#include <osg/buffered_value>


namespace osgwQuery
{


/** \class QueryCullCallback QueryUtils.h <osgwQuery/QueryUtils.h>
\brief Make a cull-time traverse/no-traverse decision based on Guthe
algorithm criteria in the QueryComputation class.
*/
class OSGWQUERY_EXPORT QueryCullCallback : public osg::NodeCallback
{
public:
    QueryCullCallback();
    QueryCullCallback( const QueryCullCallback& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    META_Object(osgwQuery,QueryCullCallback);

    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

    void attach( osg::Node* node, osgwQuery::QueryComputation* nd, osg::BoundingBox bb );

protected:
    osg::Node* _node;
    osgwQuery::QueryComputation* _nd;
    osg::BoundingBox _bb;
};
/** \class CameraResetCallback QueryUtils.h <osgwQuery/QueryUtils.h>
\brief Resets the Guthe screen coverage CscrOi value for each
Camera as a post-draw callback.
*/
struct OSGWQUERY_EXPORT CameraResetCallback : public osg::Camera::DrawCallback
{
public:
    CameraResetCallback();
    virtual void operator()( osg::RenderInfo& renderInfo ) const;
};


/** \class AddQueries QueryUtils.h <osgwQuery/QueryUtils.h>
\brief Add cull-time callbacks to a scene graph to support the Guthe algorithm.
*/
class OSGWQUERY_EXPORT AddQueries : public osg::NodeVisitor
{
public:
    AddQueries( osg::NodeVisitor::TraversalMode mode=osg::NodeVisitor::TRAVERSE_ALL_CHILDREN )
      : osg::NodeVisitor( mode )
    {}

    virtual void apply( osg::Group& node );
    virtual void apply( osg::Geode& node );
    virtual void apply( osg::Camera& node );

    void setQueryStats( osgwQuery::QueryStats* qs ) { _qs = qs; }

protected:
    osgwQuery::QueryStats* _qs;
};


// osgwQuery
}

// __OSGWQUERY_QUERY_UTILS_H__
#endif
