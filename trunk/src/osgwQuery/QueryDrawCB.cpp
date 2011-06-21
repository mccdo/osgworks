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

#include <osgwQuery/QueryDrawCB.h>
#include <osgwTools/Shapes.h>
#include <OpenThreads/ScopedLock>


namespace osgwQuery
{


QueryDrawCB::QueryDrawCB()
  : osg::Object(),
    _initialized( false )
{
}
QueryDrawCB::QueryDrawCB( const QueryDrawCB& rhs, const osg::CopyOp& copyop )
  : osg::Object( rhs, copyop ),
    _initialized( false )
{
}

void QueryDrawCB::drawImplementation( osg::RenderInfo& renderInfo, const osg::Drawable* drawable ) const
{
    if( !_initialized )
    {
        OpenThreads::ScopedLock< OpenThreads::Mutex > lock( _initLock );

        if( !( _queryDrawable.valid() ) )
            init( drawable->getBound() );
        _initialized = true;
    }

    _queryDrawable->drawImplementation( renderInfo );
    //drawable->drawImplementation( renderInfo );
}

void QueryDrawCB::init( osg::BoundingBox bb ) const
{
    // Create box geometry for use as query geometry.
    osg::Vec3 halfExtents = ( bb._max - bb._min ) / 2.;
    osg::Geometry* geom = osgwTools::makeBox( halfExtents );

    // Offset the query geometry box by the bounding box center.
    osg::Vec3Array* v = static_cast< osg::Vec3Array* >( geom->getVertexArray() );
    const osg::Vec3 center = bb.center();
    unsigned int idx;
    for( idx=0; idx<v->size(); idx++ )
        (*v)[ idx ] += center;

    // Optimize.
    geom->setUseDisplayList( false );
    geom->setUseVertexBufferObjects( true );
    geom->setTexCoordArray( 0, NULL );
    geom->setNormalArray( NULL );
    geom->setNormalBinding( osg::Geometry::BIND_OFF );

    _queryDrawable = geom;
}


// osgwQuery
}
