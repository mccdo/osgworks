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
    double angle = osg::DegreesToRadians( normalizeAngle( yaw ) );
    osg::Quat qHeading( angle, z );
    osg::Vec3 x = qHeading * _baseCross;
    osg::Vec3 y = qHeading * _baseDir;

    //   Rotate z and y axes by the pitch.
    angle = osg::DegreesToRadians( normalizeAngle( pitch ) );
    osg::Quat qPitch( angle, x );
    y = qPitch * y;
    z = qPitch * z;

    //   Rotate x and z axes by the roll.
    angle = osg::DegreesToRadians( normalizeAngle( roll ) );
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
    osg::Vec3d cross( m(0,0), m(0,1), m(0,2) );
    cross.normalize();
    osg::Vec3d dir( m(1,0), m(1,1), m(1,2) );
    dir.normalize();
    osg::Vec3d up( m(2,0), m(2,1), m(2,2) );
    up.normalize();


    // Roll

    // Compute cross vector projected onto plane defined by _baseUp.
    // Then compute angle to rotate the cross vector into that plane.
    // viewDirXBaseUp *is* the destination cross vector.
    osg::Vec3d viewDirXBaseUp( dir ^ _baseUp );
    viewDirXBaseUp.normalize();
    const double dotProjectedCross = osg::clampBetween<double>( cross * viewDirXBaseUp, -1., 1. );
    double rollRad( acos( dotProjectedCross ) );
    // Is cross below the plane defined by _baseUp?
    const double dotCrossBaseUp( cross * _baseUp );
    if( dotCrossBaseUp < 0. )
        rollRad = -rollRad;
    //if( rollRad != 0. )
    {
        // Adjust the up and cross vectors accordingly.
        osg::Quat qRoll( rollRad, dir );
        up = qRoll * up;
        cross = viewDirXBaseUp;

        roll = normalizeAngle( osg::RadiansToDegrees( rollRad ), true );
    }
    //else
      //  roll = 0.;


    // Pitch

    // Compute the angle between the up and _baseUp vectors.
    const double dotUpUp = osg::clampBetween<double>( up * _baseUp, -1., 1. );
    double pitchRad( acos( dotUpUp ) );
    // Adjust if we pitched backwards.
    const osg::Vec3d baseUpCrossUp( _baseUp ^ up );
    if( baseUpCrossUp * cross > 0. )
        pitchRad = -pitchRad;
    //if( pitchRad != 0. )
    {
        osg::Quat qPitch( pitchRad, cross );
        dir = qPitch * dir;
        up = _baseUp;

        pitch = normalizeAngle( osg::RadiansToDegrees( pitchRad ), true );
    }
    //else
      //  pitch = 0.;


    // Yaw

    // Compute the angle between the dir and _baseDir vectors.
    const double dotDirDir = osg::clampBetween<double>( dir * _baseDir, -1., 1. );
    double yawRad( acos( dotDirDir ) );
    // Adjust if we yawed left.
    const osg::Vec3d baseCross( _baseDir ^ _baseUp );
    if( dir * baseCross < 0. )
        yawRad = -yawRad;
    yaw = normalizeAngle( osg::RadiansToDegrees( yawRad ), true );
}


double Orientation::normalizeAngle( const double degreesIn, const bool convertHanded )
{
    double result( degreesIn );

    // Check for epsilon near zero.
    const double eps( 0.000005 );
    if( ( result < eps ) && ( result > -eps ) )
        return( 0. );

    while( result < 0. )
        result += 360.;
    while( result > 360. )
        result -= 360.;

    if( convertHanded && ( result > 0. ) )
        result = 360. - result;

    return( result );
}


// namespace osgwTools
}
