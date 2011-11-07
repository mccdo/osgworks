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

#include <osg/io_utils>
#include <iostream>


namespace osgwMx
{


MxCore::MxCore()
  : _viewUp( osg::Vec3d( 0., 0., 1. ) ),
    _viewDir( osg::Vec3d( 0., 1., 0. ) ),
    _position( osg::Vec3d( 0., 0., 0. ) ),
    _initialUp( osg::Vec3d( 0., 0., 1. ) ),
    _initialDir( osg::Vec3d( 0., 1., 0. ) ),
    _initialPosition( osg::Vec3d( 0., 0., 0. ) ),
    _rotateScale( 1. ),
    _moveScale( osg::Vec3d( 1., 1., 1. ) ),
    _ortho( false ),
    _aspect( 1.0 ),
    _fovy( 30.0 ),
    _initialFovy( 30.0 ),
    _fovyScale( 1.1 ),
    _clampFovyScale( true ),
    _clampFovyRange( osg::Vec2d( 5.0, 160.0 ) ),
    _orthoBottom( 0.0 ),
    _orthoTop( 0.0 )
{
}
MxCore::MxCore( const MxCore& rhs, const osg::CopyOp& copyop )
  : Object( rhs, copyop ),
    _viewUp( rhs._viewUp ),
    _viewDir( rhs._viewDir ),
    _position( rhs._position ),
    _initialUp( rhs._initialUp ),
    _initialDir( rhs._initialDir ),
    _initialPosition( rhs._initialPosition ),
    _rotateScale( rhs._rotateScale ),
    _moveScale( rhs._moveScale ),
    _ortho( rhs._ortho ),
    _aspect( rhs._aspect ),
    _fovy( rhs._fovy ),
    _initialFovy( rhs._initialFovy ),
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



//
// View matrix support
//

osg::Matrixd MxCore::getMatrix() const
{
    osg::Vec3d c = getCross();
    const osg::Vec3d& u = _viewUp;
    const osg::Vec3d& d = _viewDir;
    const osg::Vec3d p = _position;

    osg::Matrixd m = osg::Matrixd(
        c[0], c[1], c[2], 0.0,
        u[0], u[1], u[2], 0.0,
        -d[0], -d[1], -d[2], 0.0,
        p[0], p[1], p[2], 1.0 );
    return( m );
}
osg::Matrixd MxCore::getOrientationMatrix() const
{
    osg::Vec3d c = getCross();
    const osg::Vec3d& u = _viewUp;
    const osg::Vec3d& d = _viewDir;

    osg::Matrixd m = osg::Matrixd(
        c[0], c[1], c[2], 0.0,
        u[0], u[1], u[2], 0.0,
        -d[0], -d[1], -d[2], 0.0,
        0.0, 0.0, 0.0, 1.0 );
    return( m );
}
osg::Matrixd MxCore::getInverseMatrix() const
{
    // TBD for efficiency, reimplement without calling invert().
    osg::Matrixd m;
    m.invert( getMatrix() );
    return( m );
}

void MxCore::setInitialValues( const osg::Vec3d& up, const osg::Vec3d& dir,
    const osg::Vec3d& pos, double fovy )
{
    _initialUp = up;
    _initialDir = dir;
    _initialPosition = pos;
    _initialFovy = fovy;

    // Error check.
    _initialUp.normalize();
    _initialDir.normalize();
    if( osg::absolute< double >( _initialUp * _initialDir ) > 0.99 )
        osg::notify( osg::WARN ) << "MxCore::setInitialValues: Up and dir vectors are nearly coincident. Results are undefined." << std::endl;

    // orthonormalize
    osg::Vec3d c = _initialDir ^ _initialUp;
    _initialUp = c ^ _initialDir;
    _initialUp.normalize();
    _initialDir.normalize();
}
void MxCore::getInitialValues( osg::Vec3d& up, osg::Vec3d& dir,
    osg::Vec3d& pos, double& fovy )
{
    up = _initialUp;
    dir = _initialDir;
    pos = _initialPosition;
    fovy = _initialFovy;
}


void MxCore::reset()
{
    _viewUp = _initialUp;
    _viewDir = _initialDir;
    orthonormalize();

    _position = _initialPosition;
    _fovy = _initialFovy;
    setOrtho( false );
}

void MxCore::level()
{
    _viewUp = _initialUp;

    // Check for vurrent view dir coincident with initial up vector. If so,
    // we can't preserve the current view dir and need to set it to the
    // initial view dir.
    if( osg::absolute< double >( _initialUp * _initialDir ) > 0.99 )
        _viewDir = _initialDir;
    else
        _viewDir = _viewUp ^ getCross();
}


void MxCore::rotate( double angle, const osg::Vec3d& axis )
{
    osg::Matrix r = osg::Matrix::rotate( angle * _rotateScale, axis );
    _viewDir = _viewDir * r;
    _viewUp = _viewUp * r;
    orthonormalize();
}

void MxCore::rotate( double angle, const osg::Vec3d& axis, const osg::Vec3d& point )
{
    osg::Matrix r = osg::Matrix::rotate( angle * _rotateScale, axis );

    _position = ( _position - point ) * r + point;
    _viewDir = _viewDir * r;
    _viewUp = _viewUp * r;
    orthonormalize();
}

void MxCore::moveWorldCoords( osg::Vec3d delta )
{
    osg::Vec3d scaledDelta( delta[0] * _moveScale[0],
        delta[1] * _moveScale[1], delta[2] * _moveScale[2] );
    _position += scaledDelta;
}
void MxCore::move( osg::Vec3d delta )
{
    osg::Vec3d scaledDelta( delta[0] * _moveScale[0],
        delta[1] * _moveScale[1], delta[2] * _moveScale[2] );
    _position += ( scaledDelta * getOrientationMatrix() );
}


void MxCore::getYawPitchRoll( double& yaw, double& pitch, double& roll, bool rightHanded ) const
{
    // Temp var for cross products.
    osg::Vec3d right;

    const osg::Vec3d viewDirXBaseUp( _viewDir ^ _initialUp );
    const double twoPi( 2. * osg::PI );


    // Yaw

    // Compute view direction, projected into plane defined by base up.
    // TBD what if _viewDir and _initialUp are coincident?
    osg::Vec3d projectedDir = _initialUp ^ viewDirXBaseUp;
    projectedDir.normalize();
    // Is the vector pointing to the left of north, or to the right?
    right = _initialDir ^ _initialUp;
    const double dotDirRight = projectedDir * right;
    // Dot product of two unit vectors is the cosine of the angle between them.
    const double dotDirNorth = projectedDir * _initialDir;
    double yawRad = acos( dotDirNorth );
    if( dotDirRight > 0. )
        yawRad = osg::PI + ( osg::PI - yawRad );
    if( !rightHanded )
        yawRad = twoPi - yawRad;
    if( yawRad == twoPi )
        yawRad = 0.;
    yaw = osg::RadiansToDegrees( yawRad );


    // Pitch

    const double dotDirUp = _viewDir * _initialUp;
    const double dotUpUp = _viewUp * _initialUp;
    double pitchRad = acos( osg::absolute< double >( dotUpUp ) );
    if( dotDirUp < 0. )
        pitchRad *= -1.;
    pitch = osg::RadiansToDegrees( pitchRad );


    // Roll

    // Compute base up projected onto plane defined by view direction.
    // TBD what if _viewDir and _initialUp are coincident?
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
// Projection / FOV support
//

void MxCore::setOrtho( bool ortho, const double viewDistance )
{
    _ortho = ortho;

    // tan (fovy/2) = a / e2c.len
    _orthoTop = tan( getFovyRadians() * .5 ) * viewDistance;
    _orthoBottom = -_orthoTop;
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
osg::Matrixd MxCore::computeProjection( const osg::Vec2d& nearFar ) const
{
    const double zNear = nearFar[ 0 ];
    const double zFar = nearFar[ 1 ];
    if( _ortho )
    {
        const double xRange = _aspect * ( _orthoTop - _orthoBottom );
        const double right = xRange * .5;

        return( osg::Matrixd::ortho( -right, right, _orthoBottom, _orthoTop, zNear, zFar ) );
    }
    else
    {
        double myNear( zNear );
        if( zNear < 0. )
            myNear = zFar / 2000.; // Default OSG z plane ratio.
        return( osg::Matrixd::perspective( _fovy, _aspect, myNear, zFar ) );
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



void MxCore::orthonormalize()
{
    osg::Vec3d c = getCross();
    _viewUp = c ^ _viewDir;
    _viewDir.normalize();
    _viewUp.normalize();
}


// osgwMx
}
