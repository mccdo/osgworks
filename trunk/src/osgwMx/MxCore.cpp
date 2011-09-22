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

#include <osgwMx/MxCore.h>

#include <osg/Matrixd>
#include <osg/Math>
#include <osg/Notify>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osg/Plane>

#include <osg/io_utils>
#include <iostream>


namespace osgwMx
{


MxCore::MxCore()
  : _trackballRollSensitivity( 1.3 ),
    _baseUp( osg::Vec3d( 0., 0., 1. ) ),
    _baseNorth( osg::Vec3d( 0., 1., 0. ) ),
    _viewUp( osg::Vec3d( 0., 0., 1. ) ),
    _lastCross( osg::Vec3d( 1., 0., 0. ) ),
    _viewDir( osg::Vec3d( 0., 1., 0. ) ),
    _viewCenter( osg::Vec3d( 0., 0., 0. ) ),
    _viewDistance( 1. ),
    _panPlane( osg::Vec4d( _viewDir, 0. ) ),
    _mode( THIRD_PERSON ),
    _ortho( false ),
    _scene( NULL ),
    _aspect( 1.0 ),
    _fovy( 30.0 ),
    _fovyScale( 1.1 ),
    _clampFovyScale( true ),
    _clampFovyRange( osg::Vec2d( 5.0, 160.0 ) ),
    _orthoBottom( 0.0 ),
    _orthoTop( 0.0 )
{
}
MxCore::MxCore( const MxCore& rhs, const osg::CopyOp& copyop )
  : Object( rhs, copyop ),
    _trackballRollSensitivity( rhs._trackballRollSensitivity ),
    _baseUp( rhs._baseUp ),
    _baseNorth( rhs._baseNorth ),
    _viewUp( rhs._viewUp ),
    _lastCross( rhs._lastCross ),
    _viewDir( rhs._viewDir ),
    _viewCenter( rhs._viewCenter ),
    _viewDistance( rhs._viewDistance ),
    _panPlane( rhs._panPlane ),
    _mode( rhs._mode ),
    _ortho( rhs._ortho ),
    _scene( rhs._scene ),
    _aspect( rhs._aspect ),
    _fovy( rhs._fovy ),
    _fovyScale( rhs._fovyScale ),
    _clampFovyScale( rhs._clampFovyScale ),
    _clampFovyRange( rhs._clampFovyRange ),
    _orthoBottom( rhs._orthoBottom ),
    _orthoTop( rhs._orthoTop )
{
}
MxCore::~MxCore()
{
}



void MxCore::setSceneData( osg::Node* scene )
{
    _scene = scene;

    const osg::BoundingSphere& bs = _scene->getBound();
    _viewCenter = bs._center;
}

void MxCore::computeInitialView()
{
    _viewUp = _baseUp;
    _viewDir = _baseNorth;

    _viewCenter = _scene->getBound().center();

    // tan( fovy/2. ) = bs.radius / distance
    // Solve for distance:
    // distance = bs.radius / tan( fovy/2. )
    float distance = _scene->getBound().radius() /
        tan( osg::DegreesToRadians( _fovy/2. ) );

    _viewDistance = distance;
}


//
// View matrix support
//

osg::Matrixd MxCore::getMatrix() const
{
    const osg::Vec3d& d = _viewDir;
    const osg::Vec3d& u = _viewUp;
    osg::Vec3d r = d ^ u;
    const osg::Vec3d p = getEyePosition();

    osg::Matrixd m = osg::Matrixd(
        r[0], r[1], r[2], 0.0,
        u[0], u[1], u[2], 0.0,
        -d[0], -d[1], -d[2], 0.0,
        p[0], p[1], p[2], 1.0 );
    return( m );
}
osg::Matrixd MxCore::getOrientationMatrix() const
{
    const osg::Vec3d& d = _viewDir;
    const osg::Vec3d& u = _viewUp;
    osg::Vec3d r = d ^ u;

    osg::Matrixd m = osg::Matrixd(
        r[0], r[1], r[2], 0.0,
        u[0], u[1], u[2], 0.0,
        -d[0], -d[1], -d[2], 0.0,
        0.0, 0.0, 0.0, 1.0 );
    return( m );
}
osg::Matrixd MxCore::getInverseMatrix() const
{
    osg::Matrixd m;
    m.invert( getMatrix() );
    return( m );
}

void MxCore::rotate( osg::Vec2d start, osg::Vec2d dir )
{
    if( dir.length2() == 0. )
        // No motion
        return;

    if( _mode == FIRST_PERSON )
    {
        // Position is constant in 1st person view. Obtain it (for later use)
        // *before* we alter the _viewDir.
        const osg::Vec3d position = getEyePosition();

        // Compute rotation matrix.
        osg::Vec3d cross = _viewDir ^ _viewUp;
        osg::Matrix m = osg::Matrix::rotate( dir[ 0 ], _viewUp ) *
            osg::Matrix::rotate( -dir[ 1 ], cross );

        // Re-orient the basis.
        _viewDir = _viewDir * m;
        _viewUp = _viewUp * m;
        // Orthonormalize.
        cross = _viewDir ^ _viewUp;
        _viewUp = cross ^ _viewDir;
        _viewDir.normalize();
        _viewUp.normalize();

        // Compute the new view center.
        _viewCenter = position + ( _viewDir * _viewDistance );
    }

    else // THIRD_PERSON
    {
        const osg::Matrixd orientMat = getOrientationMatrix();

        // Take the spin direction 'dir' and rotate it 90 degrees
        // to get our base axis (still in the window plane).
        // Simultaneously convert to current view space.
        osg::Vec2d screenAxis( -dir[ 1 ], dir[ 0 ] );
        const osg::Vec3d baseAxis = osg::Vec3d( screenAxis[ 0 ], screenAxis[ 1 ], 0. ) * orientMat;
        osg::Vec3d dir3 = osg::Vec3d( dir[ 0 ], dir[ 1 ], 0. ) * orientMat;
        dir3.normalize();

        // The distance from center, along with the roll sensitivity,
        // tells us how much to rotate the baseAxis (ballTouchAngle) to get
        // the actual ballAxis.
        const double distance = start.length();
        const double rotationDir( ( screenAxis * start > 0. ) ? -1. : 1. );
        const double ballTouchAngle = rotationDir * _trackballRollSensitivity * distance;
        osg::Vec3d ballAxis = baseAxis * osg::Matrixd::rotate( ballTouchAngle, dir3 );
        ballAxis.normalize();

        osg::Matrixd m = osg::Matrixd::rotate( -( dir.length() ), ballAxis );

        // Re-orient the basis.
        _viewDir = _viewDir * m;
        _viewUp = _viewUp * m;
        // Orthonormalize.
        osg::Vec3d cross = _viewDir ^ _viewUp;
        _viewUp = cross ^ _viewDir;
        _viewDir.normalize();
        _viewUp.normalize();
    }
}


void MxCore::pan( const double ndcX, const double ndcY )
{
    // Get the view volume far plane value, and the distance from
    // the near to far plane.
    double zNear, zFar;
    osg::Matrixd p = computeProjection();
    if( getOrtho() )
    {
        double l, r, b, t;
        p.getOrtho( l, r, b, t, zNear, zFar );
    }
    else
    {
        double fovy, aspect;
        p.getPerspective( fovy, aspect, zNear, zFar );
    }
    const double distance = zFar - zNear;

    // Create two points, both in NDC space, and lying on the far plane at the back
    // of the view volume. One is the xy origin, the other with the passed xy parameters.
    osg::Vec4d farPoint0 = osg::Vec4d( 0., 0., 1., 1. );
    osg::Vec4d farPoint1 = osg::Vec4d( ndcX, ndcY, 1., 1. );
    if( !getOrtho() )
    {
        // Not ortho, so w != 1.0. Multiply by the far plane distance.
        // This yields values in clip coordinates.
        farPoint0 *= zFar;
        farPoint1 *= zFar;
    }

    // Get inverse view & proj matrices to back-transform the
    // two clip coord far points into world space.
    osg::Matrixd v = getMatrix();
    p.invert( p );
    osg::Vec4d wc0 = farPoint0 * p * v;
    osg::Vec4d wc1 = farPoint1 * p * v;

    // Intersect the two world coord points with the pan plane.
    osg::Vec3d result0, result1;
    osg::Vec3d p1( wc0.x(), wc0.y(), wc0.z() );
    osg::Vec3d p0 = getOrtho() ? p1 - ( _viewDir * distance ) : getEyePosition();
    intersectPlaneRay( result0, _panPlane, p0, p1 );
    p1 = osg::Vec3d( wc1.x(), wc1.y(), wc1.z() );
    p0 = getOrtho() ? p1 - ( _viewDir * distance ) : getEyePosition();
    intersectPlaneRay( result1, _panPlane, p0, p1 );

    // Subtract the two plane intersection points to get the delta world coord
    // motion and move the view center accordingly.
    osg::Vec3d delta = result1 - result0;
    osg::notify( osg::DEBUG_FP ) << "    delta " << delta << std::endl;
    _viewCenter -= delta;
}

void MxCore::dolly( const double deltaMovement )
{
    if( getOrtho() )
        // No dolly in ortho mode
        return;

    double scale( 1.0 );
    if( !( _scene.valid() ) )
    {
        osg::notify( osg::WARN ) << "MxCore::dolly: _scene == NULL." << std::endl;
    }
    else
    {
        // Scale based on model size. TBD this should be under
        // app control so that it can be disabled if desired.
        const osg::BoundingSphere& bs = _scene->getBound();
        scale = bs._radius * .5;
        if( _viewDistance > bs._radius )
             scale *= ( _viewDistance / bs._radius );
    }

    _viewDistance += ( deltaMovement * scale );
    if( _viewDistance < 0. )
        _viewDistance = 0.;
}


void MxCore::setPanStart( const double ndcX, const double ndcY )
{
    osg::Matrixd p = computeProjection();

    // Assume ortho, where ndc far plane == 1 and w always == 1.
    osg::Vec4d farPoint = osg::Vec4d( ndcX, ndcY, 1., 1. );
    if( !getOrtho() )
    {
        // Not ortho, so w != 1.0. Multiply by the far plane distance.
        // This yields a value in clip coords.
        double fovy, aspect, zNear, zFar;
        p.getPerspective( fovy, aspect, zNear, zFar );
        farPoint *= zFar;
    }

    // Get inverse view & proj matrices to back-transform the clip coord far point.
    osg::Matrixd v = getMatrix();
    p.invert( p );

    osg::Vec4d wc = farPoint * p * v;
    osg::Vec3d wcFarPoint( wc.x(), wc.y(), wc.z() );

    // Define world coord plane orthogonal to view, which contains the picked point.
    osg::Vec3d pickPoint;
    if( !( intersect( pickPoint, wcFarPoint ) ) )
    {
        // Intersection failed, probably user clicked on background.
        // Use _viewCenter to compute plane distance value.
        pickPoint = _viewCenter;
        osg::notify( osg::DEBUG_FP ) << "Intersection failed. ";
    }

    _panPlane = osg::Vec4d( _viewDir, -( pickPoint * _viewDir ) );
    osg::notify( osg::DEBUG_FP ) << "Pick point " << pickPoint << std::endl;
    osg::notify( osg::DEBUG_FP ) << "  Plane " << _panPlane << std::endl;
}

void MxCore::setMxCoreMode( MxCoreMode mode )
{
    if( _mode == mode )
        return;
    _mode = mode;
}

void MxCore::pickCenter( const double ndcX, const double ndcY )
{
    // Preserve the view direction.
    const osg::Vec3d lastPosition = getEyePosition();

    osg::Matrixd p = computeProjection();

    osg::Vec4d ccFarPoint( ndcX, ndcY, 1., 1. );
    if( !getOrtho() )
    {
        // Not ortho, so w != 1.0. Multiply by the far plane distance.
        // This yields a value in clip coords.
        double fovy, aspect, zNear, zFar;
        p.getPerspective( fovy, aspect, zNear, zFar );
        ccFarPoint *= zFar;
    }

    // Get inverse view & proj matrices to back-transform the clip coord point.
    osg::Matrixd v = getMatrix();
    p.invert( p );

    osg::Vec4d wc = ccFarPoint * p * v;
    osg::Vec3d farPoint( wc.x(), wc.y(), wc.z() );

    if( !( intersect( _viewCenter, farPoint ) ) )
        osg::notify( osg::WARN ) << "MxCore::pickCenter: No intersections." << std::endl;

    _viewDistance = ( lastPosition - _viewCenter ).length();
}


void MxCore::setTrackballRollSensitivity( double sollSensitivity )
{
    _trackballRollSensitivity = sollSensitivity;
    osg::clampBetween< double >( _trackballRollSensitivity, 0., osg::PI_2 );
}

void MxCore::setBaseUpNorth( const osg::Vec3d& up, const osg::Vec3d& north )
{
    _baseUp = up;
    _baseNorth = north;
    _baseUp.normalize();
    _baseNorth.normalize();

    if( osg::absolute< double >( _baseUp * _baseNorth ) > 0.01 )
        osg::notify( osg::WARN ) << "MxCore::setBaseUpNorth: Vectors are not orthogonal. Results are undefined." << std::endl;
}
void MxCore::getYawPitchRoll( double& yaw, double& pitch, double& roll, bool rightHanded ) const
{
    // Temp var for cross products.
    osg::Vec3d right;

    const osg::Vec3d viewDirXBaseUp( _viewDir ^ _baseUp );
    const double twoPi( 2. * osg::PI );


    // Yaw

    // Compute view direction, projected into plane defined by base up.
    // TBD what if _viewDir and _baseUp are coincident?
    osg::Vec3d projectedDir = _baseUp ^ viewDirXBaseUp;
    projectedDir.normalize();
    // Is the vector pointing to the left of north, or to the right?
    right = _baseNorth ^ _baseUp;
    const double dotDirRight = projectedDir * right;
    // Dot product of two unit vectors is the cosine of the angle between them.
    const double dotDirNorth = projectedDir * _baseNorth;
    double yawRad = acos( dotDirNorth );
    if( dotDirRight > 0. )
        yawRad = osg::PI + ( osg::PI - yawRad );
    if( !rightHanded )
        yawRad = twoPi - yawRad;
    if( yawRad == twoPi )
        yawRad = 0.;
    yaw = osg::RadiansToDegrees( yawRad );


    // Pitch

    const double dotDirUp = _viewDir * _baseUp;
    const double dotUpUp = _viewUp * _baseUp;
    double pitchRad = acos( osg::absolute< double >( dotUpUp ) );
    if( dotDirUp < 0. )
        pitchRad *= -1.;
    pitch = osg::RadiansToDegrees( pitchRad );


    // Roll

    // Compute base up projected onto plane defined by view direction.
    // TBD what if _viewDir and _baseUp are coincident?
    osg::Vec3d projectedBaseUp = viewDirXBaseUp ^ _viewDir;
    projectedBaseUp.normalize();
    // Is the view up vector pointing to the left of the projected base up, or to the right?
    right = _viewDir ^ projectedBaseUp;
    const double dotUpRight = _viewUp * right;
    // Dot product of two unit vectors is the cosine of the angle between them.
    const double dotUp = projectedBaseUp * _viewUp;
    double rollRad = acos( dotUp );
    if( dotUpRight > 0. )
        rollRad = osg::PI + ( osg::PI - rollRad );
    if( !rightHanded )
        rollRad = twoPi - rollRad;
    if( rollRad == twoPi )
        rollRad = 0.;
    roll = osg::RadiansToDegrees( rollRad );
}


//
// Projection / FOB support
//

void MxCore::setOrtho( bool ortho )
{
    _ortho = ortho;
    if( _ortho )
    {
        if( !( _scene.valid() ) )
        {
            osg::notify( osg::WARN ) << "MxCore::setOrtho: _scene == NULL." << std::endl;
            _orthoTop = 1.0;
            _orthoBottom = -_orthoTop;
            return;
        }

        // tan (fovy/2) = a / e2c.len
        _orthoTop = tan( getFovyRadians() * .5 ) * _viewDistance;
        _orthoBottom = -_orthoTop;
    }
}

void MxCore::updateFovy( osg::Matrixd& proj ) const
{
    if( _ortho )
    {
        osg::notify( osg::WARN ) << "MxCore::updateFovy: Ortho is not yet implemented. TBD." << std::endl;
    }
    else
    {
        double left, right, bottom, top, near, far;
        proj.getFrustum( left, right, bottom, top, near, far );

        const double fovLeft = atan( left / near );
        const double fovRight = atan( right / near );
        const double fovBottom = atan( bottom / near );
        const double fovTop = atan( top / near );

        const double fovyRatio = getFovyRadians() /
            ( osg::absolute< double >( fovBottom ) + osg::absolute< double >( fovTop ) );

        const double newBottom = tan( fovBottom * fovyRatio ) * near;
        const double newTop = tan( fovTop * fovyRatio ) * near;
        const double xScale = newTop / top;
        left *= xScale;
        right *= xScale;
        proj = osg::Matrixd::frustum( left, right, newBottom, newTop, near, far );
    }
}
osg::Matrixd MxCore::computeProjection() const
{
    if( !( _scene.valid() ) )
    {
        osg::notify( osg::WARN ) << "MxCore::computeProjection: _scene == NULL." << std::endl;
        return( osg::Matrixd::identity() );
    }

    // TBD do we really want eyeToCenter to be a vector
    // to the *bound* center, or to the *view* center?
    const osg::BoundingSphere& bs = _scene->getBound();
    const osg::Vec3d eyeToCenter( bs._center - getEyePosition() );
    if( _ortho )
    {
        double zNear = eyeToCenter.length() - bs._radius;
        double zFar = eyeToCenter.length() + bs._radius;

        const double xRange = _aspect * ( _orthoTop - _orthoBottom );
        const double right = xRange * .5;

        return( osg::Matrixd::ortho( -right, right, _orthoBottom, _orthoTop, zNear, zFar ) );
    }
    else
    {
        double zNear = eyeToCenter.length() - bs._radius;
        double zFar = zNear + ( bs._radius * 2. );
        if( zNear < 0. )
        {
            zNear = zFar / 2000.; // Default z ratio.
        }
        return( osg::Matrixd::perspective( _fovy, _aspect, zNear, zFar ) );
    }
}

void MxCore::setFovy( double fovy )
{
    const double ratio = fovy / _fovy;
    _orthoBottom *= ratio;
    _orthoTop *= ratio;
    _fovy = fovy;
}
double MxCore::getFovyRadians() const
{
    return( osg::DegreesToRadians( _fovy ) );
}
void MxCore::fovyScaleUp()
{
    _fovy *= _fovyScale;
    if( _clampFovyScale )
    {
        _fovy = osg::clampBelow< double >( _fovy, _clampFovyRange.y() );
    }

    _orthoBottom *= _fovyScale;
    _orthoTop *= _fovyScale;
}
void MxCore::fovyScaleDown()
{
    const double factor( 1.0 / _fovyScale );
    _fovy *= factor;
    if( _clampFovyScale )
    {
        _fovy = osg::clampAbove< double >( _fovy, _clampFovyRange.x() );
    }

    _orthoBottom *= factor;
    _orthoTop *= factor;
}
void MxCore::setClampFovyScale( bool clamp, osg::Vec2d clampFovyRange )
{
    _clampFovyScale = clamp;
    _clampFovyRange = clampFovyRange;
    if( _clampFovyScale )
    {
        _fovy = osg::clampBetween< double >( _fovy, _clampFovyRange.x(), _clampFovyRange.y() );
    }
}


bool MxCore::intersect( osg::Vec3d& result, const osg::Vec3d& farPoint )
{
    if( !( _scene.valid() ) )
    {
        osg::notify( osg::WARN ) << "MxCore::intersect: _scene == NULL." << std::endl;
        return( false );
    }

    const osg::BoundingSphere& bs = _scene->getBound();
    const double distance = _viewDistance + bs._radius;

    osg::Vec3d startPoint = getOrtho() ? farPoint - ( _viewDir * distance * 2. ) : getEyePosition();
    osgUtil::LineSegmentIntersector* intersector = new osgUtil::LineSegmentIntersector(
        startPoint, farPoint );
    osgUtil::IntersectionVisitor intersectVisitor( intersector, NULL );
    _scene->accept( intersectVisitor );

    osgUtil::LineSegmentIntersector::Intersections& intersections = intersector->getIntersections();
    if( intersections.empty() )
        return( false );

    const osgUtil::LineSegmentIntersector::Intersection& intersection = *( intersections.begin() );
    result = intersection.getWorldIntersectPoint();
    return( true );
}

bool MxCore::intersectPlaneRay( osg::Vec3d& result, const osg::Vec4d& plane, const osg::Vec3d& p0, const osg::Vec3d& p1 )
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

void MxCore::viewTop()
{
    _viewUp = osg::Vec3d(0., 1., 0.);
    _viewDir = osg::Vec3d(0., 0., -1.);
}

void MxCore::viewBottom()
{
    _viewUp = osg::Vec3d(0., -1., 0.);
    _viewDir = osg::Vec3d(0., 0., 1.);
}

void MxCore::viewRight()
{
    _viewUp = osg::Vec3d(0., 0., 1.);
    _viewDir = osg::Vec3d(0., 1., 0.);
}

void MxCore::viewLeft()
{
    _viewUp = osg::Vec3d(0., 0., 1.);
    _viewDir = osg::Vec3d(0., -1., 0.);
}

void MxCore::viewFront()
{
    _viewUp = osg::Vec3d(0., 0., 1.);
    _viewDir = osg::Vec3d(-1., 0., 0.);
}

void MxCore::viewBack()
{
    _viewUp = osg::Vec3d(0., 0., 1.);
    _viewDir = osg::Vec3d(1., 0., 0.);
}


// osgwMx
}
