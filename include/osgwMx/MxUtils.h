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

/** \brief Computes an angle and axis of rotation for a simulated trackball.

The returned \c axis and \c angle values are intended to be passed to the MxCore::rotate() function.

\param start The xy start position in normalized (-1 to 1 ) windows coordinates.
\param delta The delta xy morion in normalized (-1 to 1 ) windows coordinates.
\param orientMat The current view orientation matrix.
\param sensitivity Controls simulated trackball sensitivity. Smaller values produce less
of a rool effect, while larger values produce a more pronounced roll effect. Valid range
is 0.0 to pi/2. Default is 1.3.
\return angle The computed rotation angle in radians.
\return axis The axis to rotate about.
*/
void OSGWMX_EXPORT computeTrackball( double& angle, osg::Vec3d& axis,
    const osg::Vec2d& start, const osg::Vec2d& delta,
    const osg::Matrix& orientMat, const double sensitivity=1.3 );


// osgwMx
}


// __OSGWMX_MX_UTILS_H__
#endif
