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

#include <osgwMx/MxUtils.h>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osg/Plane>

#include <osg/Math>
#include <osg/io_utils>



namespace osgwMx
{


double computeInitialFOVYFromDistance( const osg::BoundingSphere& bs, const double distance )
{
    double halfFOVYTangent = bs.radius() / distance;
    double halfFOVYAngle = atan( halfFOVYTangent );
    return( 2. * halfFOVYAngle );
}
double computeInitialDistanceFromFOVY( const osg::BoundingSphere& bs, const double fovy )
{
    return( bs.radius() / tan( osg::DegreesToRadians( fovy/2. ) ) );
}


osg::Vec2d computeOptimalNearFar( const osg::Vec3d& position,
    const osg::BoundingSphere& bs, bool ortho )
{
    double zNear, zFar;

    osg::Vec3d eyeToCenter = bs.center() - position;
    if( ortho )
    {
        zNear = eyeToCenter.length() - bs._radius;
        zFar = eyeToCenter.length() + bs._radius;
    }
    else
    {
        zNear = eyeToCenter.length() - bs._radius;
        zFar = zNear + ( bs._radius * 2. );
        if( zNear < 0. )
            zNear = zFar / 2000.; // Default z ratio.
    }

    return( osg::Vec2d( zNear, zFar ) );
}


osg::Vec4d computePanPlane( osg::Node* scene, const osgwMx::MxCore* mxCore,
                 const double ndcX, const double ndcY )
{
    const osg::BoundingSphere& bs = scene->getBound();

    osg::Matrix proj = mxCore->computeProjection(
        osgwMx::computeOptimalNearFar( mxCore->getPosition(), bs, mxCore->getOrtho() ) );

    // Assume ortho, where ndc far plane == 1 and w always == 1.
    osg::Vec4d farPoint( ndcX, ndcY, 1., 1. );
    if( !( mxCore->getOrtho() ) )
    {
        // Not ortho, so w != 1.0. Multiply by the far plane distance.
        // This yields a value in clip coords.
        double fovy, aspect, zNear, zFar;
        proj.getPerspective( fovy, aspect, zNear, zFar );
        farPoint *= zFar;
    }

    // Get inverse view & proj matrices to back-transform the clip coord far point.
    osg::Matrixd v = mxCore->getMatrix(); // This is the inverse of the view matrix.
    osg::Matrix p = osg::Matrix::inverse( proj );

    osg::Vec4d wc = farPoint * p * v;
    osg::Vec3d wcFarPoint( wc.x(), wc.y(), wc.z() );

    // Define world coord plane orthogonal to view, which contains the picked point.
    osg::Vec3d pickPoint;
    if( !( intersect( pickPoint, wcFarPoint, scene, mxCore ) ) )
    {
        // Intersection failed, probably user clicked on background.
        // Use a point 10. units in front of the eye, better than nothing.
        pickPoint = mxCore->getPosition() + ( mxCore->getDir() * 10. );
        osg::notify( osg::DEBUG_FP ) << "MxUtil::setPanStart: Intersection failed. ";
    }

    const osg::Vec3d viewDir( mxCore->getDir() );
    osg::Vec4d panPlane( viewDir, -( pickPoint * viewDir ) );
    osg::notify( osg::DEBUG_FP ) << "Pick point " << pickPoint << std::endl;
    osg::notify( osg::DEBUG_FP ) << "  Plane " << panPlane << std::endl;

    return( panPlane );
}

bool intersect( osg::Vec3d& result, const osg::Vec3d& farPoint, osg::Node* scene, const osgwMx::MxCore* mxCore )
{
    if( ( scene == NULL ) )
    {
        osg::notify( osg::WARN ) << "MxUtil::intersect: _scene == NULL." << std::endl;
        return( false );
    }

    const osg::BoundingSphere& bs = scene->getBound();
    const double viewDistance = ( bs.center() - mxCore->getPosition() ).length();
    const double distance = viewDistance + bs._radius;

    osg::Vec3d startPoint = mxCore->getOrtho() ? farPoint - ( mxCore->getDir() * distance * 2. ) : mxCore->getPosition();
    osgUtil::LineSegmentIntersector* intersector = new osgUtil::LineSegmentIntersector(
        startPoint, farPoint );
    osgUtil::IntersectionVisitor intersectVisitor( intersector, NULL );
    scene->accept( intersectVisitor );

    osgUtil::LineSegmentIntersector::Intersections& intersections = intersector->getIntersections();
    if( intersections.empty() )
        return( false );

    const osgUtil::LineSegmentIntersector::Intersection& intersection = *( intersections.begin() );
    result = intersection.getWorldIntersectPoint();
    return( true );
}

osg::Vec3d pan( const osg::Node* scene, const osgwMx::MxCore* mxCore,
    const osg::Vec4d panPlane, const double ndcX, const double ndcY )
{
    const osg::BoundingSphere& bs = scene->getBound();

    // Get the view volume far plane value, and the distance from
    // the near to far plane.
    osg::Vec2d nearFar( osgwMx::computeOptimalNearFar( mxCore->getPosition(), bs, mxCore->getOrtho() ) );
    double zNear( nearFar[ 0 ] ), zFar( nearFar[ 1 ] );
    const double distance = zFar - zNear;

    // Create two points, both in NDC space, and lying on the far plane at the back
    // of the view volume. One is the xy origin, the other with the passed xy parameters.
    osg::Vec4d farPoint0 = osg::Vec4d( 0., 0., 1., 1. );
    osg::Vec4d farPoint1 = osg::Vec4d( ndcX, ndcY, 1., 1. );
    if( !( mxCore->getOrtho() ) )
    {
        // Not ortho, so w != 1.0. Multiply by the far plane distance.
        // This yields values in clip coordinates.
        farPoint0 *= zFar;
        farPoint1 *= zFar;
    }

    // Get inverse view & proj matrices to back-transform the
    // two clip coord far points into world space.
    osg::Matrixd v = mxCore->getMatrix(); // This is the inverse of the view matrix.
    osg::Matrix proj = mxCore->computeProjection( nearFar );
    osg::Matrix p = osg::Matrix::inverse( proj );

    osg::Vec4d wc0 = farPoint0 * p * v;
    osg::Vec4d wc1 = farPoint1 * p * v;

    const osg::Vec3d viewDir = mxCore->getDir();
    const osg::Vec3d& pos = mxCore->getPosition();

    // Intersect the two world coord points with the pan plane.
    osg::Vec3d result0, result1;
    osg::Vec3d p1( wc0.x(), wc0.y(), wc0.z() );
    osg::Vec3d p0 = mxCore->getOrtho() ? p1 - ( viewDir * distance ) : pos;
    intersectRayPlane( result0, panPlane, p0, p1 );
    p1 = osg::Vec3d( wc1.x(), wc1.y(), wc1.z() );
    p0 = mxCore->getOrtho() ? p1 - ( viewDir * distance ) : pos;
    intersectRayPlane( result1, panPlane, p0, p1 );

    // Subtract the two plane intersection points to get the delta world coord
    // motion and move the view center accordingly.
    osg::Vec3d delta = result1 - result0;
    osg::notify( osg::DEBUG_FP ) << "    delta " << delta << std::endl;

    return( delta );
}


osg::Vec3d pickCenter( osg::Node* scene, const osgwMx::MxCore* mxCore,
    const double ndcX, const double ndcY )
{
    const osg::BoundingSphere& bs = scene->getBound();
    osg::Vec2d nearFar( osgwMx::computeOptimalNearFar( mxCore->getPosition(), bs, mxCore->getOrtho() ) );

    osg::Vec4d ccFarPoint( ndcX, ndcY, 1., 1. );
    if( !( mxCore->getOrtho() ) )
    {
        // Not ortho, so w != 1.0. Multiply by the far plane distance.
        // This yields a value in clip coords.
        ccFarPoint *= nearFar[ 1 ];
    }

    // Get inverse view & proj matrices to back-transform the clip coord point.
    const osg::Matrixd v = mxCore->getMatrix();
    const osg::Matrix proj = mxCore->computeProjection( nearFar );
    osg::Matrix p = osg::Matrix::inverse( proj );

    osg::Vec4d wc = ccFarPoint * p * v;
    osg::Vec3d farPoint( wc.x(), wc.y(), wc.z() );

    osg::Vec3d pickResult;
    if( !( intersect( pickResult, farPoint, scene, mxCore ) ) )
        osg::notify( osg::WARN ) << "MxUtils::pickCenter: No intersections." << std::endl;

    return( pickResult );
}


bool intersectRayPlane( osg::Vec3d& result, const osg::Vec4d& plane, const osg::Vec3d& p0, const osg::Vec3d& p1 )
{
    osg::Vec3d planeNormal = osg::Vec3d( plane[ 0 ], plane[ 1 ], plane[ 2 ] );

    osg::notify( osg::DEBUG_FP ) << "    p0 " << p0 << std::endl;
    osg::notify( osg::DEBUG_FP ) << "    p1 " << p1 << std::endl;
    const osg::Vec3d vDir = p1 - p0;
    const double dotVd = vDir * planeNormal;
    osg::notify( osg::DEBUG_FP ) << "  dotVd " << dotVd << std::endl;
    if( dotVd == 0. )
    {
        osg::notify( osg::WARN ) << "MxCore::intersectPlaneRay: No plane intersection." << std::endl;
        return( false );
    }
    double length = -( planeNormal * p0 + plane[ 3 ] ) / dotVd;
    osg::notify( osg::DEBUG_FP ) << "  length " << length << std::endl;
    result = p0 + ( vDir * length );
    osg::notify( osg::DEBUG_FP ) << "    intersection point " << result << std::endl;
    return( true );
}



void computeTrackball( double& angle, osg::Vec3d& axis,
    const osg::Vec2d& start, const osg::Vec2d& delta,
    const osg::Matrix& orientMat, const double sensitivity )
{
    // Take the spin direction 'delta' and rotate it 90 degrees
    // to get our base axis (still in the window plane).
    // Simultaneously convert to current view space.
    osg::Vec2d screenAxis( -delta[ 1 ], delta[ 0 ] );
    const osg::Vec3d baseAxis = osg::Vec3d( screenAxis[ 0 ], screenAxis[ 1 ], 0. ) * orientMat;
    osg::Vec3d dir3 = osg::Vec3d( delta[ 0 ], delta[ 1 ], 0. ) * orientMat;
    dir3.normalize();

    // The distance from center, along with the roll sensitivity,
    // tells us how much to rotate the baseAxis (ballTouchAngle) to get
    // the actual ballAxis.
    const double distance = start.length();
    const double rotationDir( ( screenAxis * start > 0. ) ? -1. : 1. );
    const double ballTouchAngle = rotationDir * sensitivity * distance;
    osg::Vec3d ballAxis = baseAxis * osg::Matrixd::rotate( ballTouchAngle, dir3 );
    ballAxis.normalize();

    angle = -( delta.length() );
    axis = ballAxis;
}


// osgwMx
}
