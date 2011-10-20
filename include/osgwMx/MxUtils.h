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

#ifndef __OSGWMX_MX_UTILS_H__
#define __OSGWMX_MX_UTILS_H__ 1


#include <osgwMx/Export.h>
#include <osgwMx/MxCore.h>
#include <osg/BoundingSphere>
#include <osg/Vec2d>


namespace osgwMx
{


double OSGWMX_EXPORT computeInitialFOVYFromDistance( const osg::BoundingSphere& bs, const double distance );
double OSGWMX_EXPORT computeInitialDistanceFromFOVY( const osg::BoundingSphere& bs, const double distance );

osg::Vec2d OSGWMX_EXPORT computeOptimalNearFar( const osg::Vec3d& position,
    const osg::BoundingSphere& bs, bool ortho );

osg::Vec4d OSGWMX_EXPORT computePanPlane( osg::Node* scene, const osgwMx::MxCore* mxCore,
    const double ndcX, const double ndcY );
osg::Vec3d OSGWMX_EXPORT pan( const osg::Node* scene, const osgwMx::MxCore* mxCore,
    const osg::Vec4d panPlane, const double ndcX, const double ndcY );

bool OSGWMX_EXPORT intersect( osg::Vec3d& result, const osg::Vec3d& farPoint,
    osg::Node* scene, const osgwMx::MxCore* mxCore );

osg::Vec3d OSGWMX_EXPORT pickCenter( osg::Node* scene, const osgwMx::MxCore* mxCore,
    const double ndcX, const double ndcY );

/** Intersect a ray with a plane. OSG doesn't appear to have this utility. */
bool OSGWMX_EXPORT intersectRayPlane( osg::Vec3d& result, const osg::Vec4d& plane, const osg::Vec3d& p0, const osg::Vec3d& p1 );


// osgwMx
}


// __OSGWMX_MX_UTILS_H__
#endif
