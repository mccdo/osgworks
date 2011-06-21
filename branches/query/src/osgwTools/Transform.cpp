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

#include "osgwTools/Transform.h"
#include <osg/BoundingSphere>
#include <osg/BoundingBox>
#include <osg/Matrix>


namespace osgwTools {


osg::BoundingSphere
transform( const osg::Matrix& m, const osg::BoundingSphere& sphere )
{
    osg::BoundingSphere::vec_type xdash = sphere._center;
    xdash.x() += sphere._radius;
    xdash = xdash * m;

    osg::BoundingSphere::vec_type ydash = sphere._center;
    ydash.y() += sphere._radius;
    ydash = ydash * m;

    osg::BoundingSphere::vec_type zdash = sphere._center;
    zdash.z() += sphere._radius;
    zdash = zdash * m;

    osg::BoundingSphere newSphere;
    newSphere._center = sphere._center * m;

    xdash -= newSphere._center;
    osg::BoundingSphere::value_type len_xdash = xdash.length();

    ydash -= newSphere._center;
    osg::BoundingSphere::value_type len_ydash = ydash.length();

    zdash -= newSphere._center;
    osg::BoundingSphere::value_type len_zdash = zdash.length();

    newSphere._radius = len_xdash;
    if( newSphere._radius < len_ydash )
        newSphere._radius = len_ydash;
    if( newSphere._radius < len_zdash )
        newSphere._radius = len_zdash;

    return( newSphere );
}
osg::BoundingBox
transform( const osg::Matrix& m, const osg::BoundingBox& box )
{
    osg::Vec3 p0( box._min );
    osg::Vec3 p1( box._max.x(), box._min.y(), box._min.z() );
    osg::Vec3 p2( box._max.x(), box._min.y(), box._max.z() );
    osg::Vec3 p3( box._min.x(), box._min.y(), box._max.z() );
    osg::Vec3 p4( box._max );
    osg::Vec3 p5( box._min.x(), box._max.y(), box._max.z() );
    osg::Vec3 p6( box._min.x(), box._max.y(), box._min.z() );
    osg::Vec3 p7( box._max.x(), box._max.y(), box._min.z() );

    osg::BoundingBox newBox( p0 * m, p1 * m );
    newBox.expandBy( p2 * m );
    newBox.expandBy( p3 * m );
    newBox.expandBy( p4 * m );
    newBox.expandBy( p5 * m );
    newBox.expandBy( p6 * m );
    newBox.expandBy( p7 * m );

    return( newBox );
}


// osgwTools
}
