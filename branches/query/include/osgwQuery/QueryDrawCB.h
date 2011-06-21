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
#include <osg/Drawable>
#include <OpenThreads/Mutex>

#include <osg/Node>
#include <osg/Geode>
#include <osg/NodeVisitor>


namespace osgwQuery
{


/**
*/
class OSGWQUERY_EXPORT QueryDrawCB : public osg::Drawable::DrawCallback
{
public:
    QueryDrawCB();
    QueryDrawCB( const QueryDrawCB& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    META_Object(osgwQuery,QueryDrawCB);

    virtual void drawImplementation( osg::RenderInfo& renderInfo, const osg::Drawable* drawable ) const;

protected:
    mutable osg::ref_ptr< osg::Drawable > _queryDrawable;

    mutable bool _initialized;
    mutable OpenThreads::Mutex _initLock;
    void init( osg::BoundingBox bb ) const;
};



class OSGWQUERY_EXPORT AddDrawCB : public osg::NodeVisitor
{
public:
    AddDrawCB() {}

    virtual void apply( osg::Geode& node )
    {
        unsigned int idx;
        for( idx=0; idx< node.getNumDrawables(); idx++ )
        {
            node.getDrawable( idx )->setDrawCallback( new QueryDrawCB );
        }

        traverse( node );
    }
};

// osgwQuery
}

// __OSGWQUERY_QUERY_DRAW_CB_H__
#endif
