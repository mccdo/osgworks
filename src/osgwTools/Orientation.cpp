/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2009-2012 by Kenneth Mark Bryden
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

#include "osgwTools/Orientation.h"
#include <osg/Quat>
#include <osg/Matrix>
#include <osg/Vec3>

#include <osg/Notify>


namespace osgwTools
{


Orientation::Orientation()
    : _baseDir( 0., 1., 0. ),
      _baseUp( 0., 0., 1. ),
      _baseCross( 1., 0., 0. ),
      _rightHanded( false )
{
}
Orientation::Orientation( const Orientation& rhs, const osg::CopyOp copyop )
    : _baseDir( rhs._baseDir ),
      _baseUp( rhs._baseUp ),
      _baseCross( rhs._baseCross ),
      _rightHanded( rhs._rightHanded )
{
}
Orientation::~Orientation()
{
}


void Orientation::setBasis( const osg::Vec3d& baseDir, const osg::Vec3d& baseUp )
{
    _baseDir = baseDir;
    _baseDir.normalize();

    _baseCross = _baseDir ^ baseUp;
    _baseCross.normalize();

    _baseUp = _baseCross ^ _baseDir;
    _baseUp.normalize();
}
void Orientation::getBasis( osg::Vec3d& baseDir, osg::Vec3d& baseUp ) const
{
    baseDir = _baseDir;
    baseUp = _baseUp;
}
void Orientation::setRightHanded( const bool rightHanded )
{
    _rightHanded = rightHanded;
}
bool Orientation::getRightHanded() const
{
    return( _rightHanded );
}


osg::Quat Orientation::getQuat( const osg::Vec3d& ypr )
{
    osg::Quat result;
    result.set( getMatrix( ypr ) );
    return( result );
}
osg::Quat Orientation::getQuat( const double yaw, const double pitch, const double roll )
{
    osg::Quat result;
    result.set( getMatrix( yaw, pitch, roll ) );
    return( result );
}
void Orientation::makeQuat( osg::Quat& result, const osg::Vec3d& ypr )
{
    result.set( getMatrix( ypr ) );
}
void Orientation::makeQuat( osg::Quat& result, const double yaw, const double pitch, const double roll )
{
    result.set( getMatrix( yaw, pitch, roll ) );
}
osg::Matrix Orientation::getMatrix( const osg::Vec3d& ypr )
{
    return( getMatrix( ypr[0], ypr[1], ypr[2] ) );
}
osg::Matrix Orientation::getMatrix( const double yaw, const double pitch, const double roll )
{
    osg::Matrix result;
    makeMatrix( result, yaw, pitch, roll );
    return( result );
}
void Orientation::makeMatrix( osg::Matrix& result, const osg::Vec3d& ypr )
{
    makeMatrix( result, ypr[0], ypr[1], ypr[2] );
}
void Orientation::makeMatrix( osg::Matrix& result, const double yaw, const double pitch, const double roll )
{
    // Given yaw, pitch, and roll angles in degrees, build a Quat to affect these rotatiions.
    // We do this by setting the Matrix with correctly-oriented x, y, and z axes.

    // First, create x, y, and z axes that represent the yaw, pitch, and roll angles.
    //   Rotate x and y axes by yaw.
    osg::Vec3d z( _baseUp );
    double angle = normalizeAngle( yaw );
    osg::Quat qHeading( angle, z );
    osg::Vec3 x = qHeading * _baseCross;
    osg::Vec3 y = qHeading * _baseDir;

    //   Rotate z and y axes by the pitch.
    angle = normalizeAngle( pitch );
    osg::Quat qPitch( angle, x );
    y = qPitch * y;
    z = qPitch * z;

    //   Rotate x and z axes by the roll.
    angle = normalizeAngle( roll );
    osg::Quat qRoll( angle, y );
    x = qRoll * x;
    z = qRoll * z;

    // Use x, y, and z axes to create an orientation matrix.
    result.set( x[0], x[1], x[2], 0.,
                y[0], y[1], y[2], 0.,
                z[0], z[1], z[2], 0.,
                0., 0., 0., 1. );
}


osg::Vec3d Orientation::getYPR( const osg::Quat& q ) const
{
    osg::Matrix m;
    m.set( q );
    return( getYPR( m ) );
}
void Orientation::getYPR( const osg::Quat& q, double& yaw, double& pitch, double& roll ) const
{
    osg::Matrix m;
    m.set( q );
    getYPR( m, yaw, pitch, roll );
}
osg::Vec3d Orientation::getYPR( const osg::Matrix& m ) const
{
    osg::Vec3d ypr;
    getYPR( m, ypr[0], ypr[1], ypr[2] );
    return( ypr );
}
void Orientation::getYPR( const osg::Matrix& m, double& yaw, double& pitch, double& roll ) const
{
    const osg::Vec3d up( m(1,0), m(1,1), m(1,2) );
    const osg::Vec3d dir( -m(2,0), -m(2,1), -m(2,2) );

    // Temp var for cross products.
    osg::Vec3d right;

    const osg::Vec3d viewDirXBaseUp( dir ^ _baseUp );
    const double twoPi( 2. * osg::PI );


    // Yaw

    // Compute view direction, projected into plane defined by base up.
    // TBD what if _viewDir and _initialUp are coincident?
    osg::Vec3d projectedDir = _baseUp ^ viewDirXBaseUp;
    projectedDir.normalize();
    // Is the vector pointing to the left of north, or to the right?
    right = _baseDir ^ _baseUp;
    const double dotDirRight = projectedDir * right;
    // Dot product of two unit vectors is the cosine of the angle between them.
    const double dotDirNorth = osg::clampBetween<double>( projectedDir * _baseDir, -1., 1. );
    double yawRad = acos( dotDirNorth );
    if( dotDirRight > 0. )
        yawRad = osg::PI + ( osg::PI - yawRad );
    if( !_rightHanded )
        yawRad = twoPi - yawRad;
    if( yawRad == twoPi )
        yawRad = 0.;
    yaw = osg::RadiansToDegrees( yawRad );


    // Pitch

    const double dotDirUp = dir * _baseUp;
    const double dotUpUp = osg::clampBetween<double>( up * _baseUp, -1., 1. );
    double pitchRad = acos( osg::absolute< double >( dotUpUp ) );
    if( dotDirUp < 0. )
        pitchRad *= -1.;
    pitch = osg::RadiansToDegrees( pitchRad );


    // Roll

    // Compute base up projected onto plane defined by view direction.
    // TBD what if _viewDir and _initialUp are coincident?
    osg::Vec3d projectedBaseUp = viewDirXBaseUp ^ dir;
    projectedBaseUp.normalize();
    // Is the view up vector pointing to the left of the projected base up, or to the right?
    right = dir ^ projectedBaseUp;
    const double dotUpRight = up * right;
    // Dot product of two unit vectors is the cosine of the angle between them.
    const double dotUp = osg::clampBetween<double>( projectedBaseUp * up, -1., 1. );
    double rollRad = acos( dotUp );
    if( dotUpRight > 0. )
        rollRad = osg::PI + ( osg::PI - rollRad );
    if( !_rightHanded )
        rollRad = twoPi - rollRad;
    if( rollRad == twoPi )
        rollRad = 0.;
    roll = osg::RadiansToDegrees( rollRad );
}


double Orientation::normalizeAngle( const double degreesIn )
{
    double result = osg::DegreesToRadians( degreesIn );
    while( result < 0. )
        result += 360.;
    while( result > 360. )
        result -= 360.;

    if( _rightHanded )
        result = 360. - result;

    return( result );
}


// namespace osgwTools
}
