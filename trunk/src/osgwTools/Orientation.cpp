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
    : _yawAxis( 0., 0., 1. ),
      _pitchAxis( 1., 0., 0. ),
      _rollAxis( 0., 1., 0. ),
      _rightHanded( false )
{
}
Orientation::Orientation( const Orientation& rhs, const osg::CopyOp copyop )
    : _yawAxis( rhs._yawAxis ),
      _pitchAxis( rhs._pitchAxis ),
      _rollAxis( rhs._rollAxis ),
      _rightHanded( rhs._rightHanded )
{
}
Orientation::~Orientation()
{
}


void Orientation::setBasis( const osg::Vec3d& yawAxis, const osg::Vec3d& pitchAxis, const osg::Vec3d& rollAxis )
{
    _yawAxis = yawAxis;
    _yawAxis.normalize();

    _pitchAxis = pitchAxis;
    _pitchAxis.normalize();

    _rollAxis = rollAxis;
    _rollAxis.normalize();
}
void Orientation::getBasis( osg::Vec3d& yawAxis, osg::Vec3d& pitchAxis, osg::Vec3d& rollAxis ) const
{
    yawAxis = _yawAxis;
    pitchAxis = _pitchAxis;
    rollAxis = _rollAxis;
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

    // Rotate the base vectors by yaw, pitch, and roll.
    //   Rotate pitchAxis and rollAxis by yaw.
    osg::Vec3d yawAxis( _yawAxis );
    double angle = osg::DegreesToRadians( normalizeAngle( yaw ) );
    osg::Quat qHeading( angle, yawAxis );
    osg::Vec3 pitchAxis = qHeading * _pitchAxis;
    osg::Vec3 rollAxis = qHeading * _rollAxis;

    //   Rotate yawAxis  and rollAxis by pitch.
    angle = osg::DegreesToRadians( normalizeAngle( pitch ) );
    osg::Quat qPitch( angle, pitchAxis );
    rollAxis = qPitch * rollAxis;
    yawAxis = qPitch * yawAxis;

    //   Rotate yawAxis and pitchAxis by roll.
    angle = osg::DegreesToRadians( normalizeAngle( roll ) );
    osg::Quat qRoll( angle, rollAxis );
    pitchAxis = qRoll * pitchAxis;
    yawAxis = qRoll * yawAxis;

    // Use transformed base vectors to create an orientation matrix.
    result.set( pitchAxis[0], pitchAxis[1], pitchAxis[2], 0.,
                rollAxis[0], rollAxis[1], rollAxis[2], 0.,
                yawAxis[0], yawAxis[1], yawAxis[2], 0.,
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
    osg::Vec3d pitchAxisIn( m(0,0), m(0,1), m(0,2) );
    pitchAxisIn.normalize();
    osg::Vec3d rollAxisIn( m(1,0), m(1,1), m(1,2) );
    rollAxisIn.normalize();
    osg::Vec3d yawAxisIn( m(2,0), m(2,1), m(2,2) );
    yawAxisIn.normalize();


    // Roll

    // Compute pitchAxisIn projected onto plane defined by _yawAxis.
    // Then compute angle to rotate pitchAxisIn into that plane.
    // rollXyaw *is* the destination pitchAxisIn vector.
    osg::Vec3d rollXyaw( rollAxisIn ^ _yawAxis );
    rollXyaw.normalize();
    const double dotPitch = osg::clampBetween<double>( pitchAxisIn * rollXyaw, -1., 1. );
    double rollRad( acos( dotPitch ) );
    // Is pitchAxisIn below the plane defined by _yawAxis?
    const double pitchDotUp( pitchAxisIn * _yawAxis );
    if( pitchDotUp < 0. )
        rollRad = -rollRad;

    // Adjust the yawAxisIn and pitchAxisIn vectors accordingly.
    osg::Quat qRoll( rollRad, rollAxisIn );
    yawAxisIn = qRoll * yawAxisIn;
    pitchAxisIn = rollXyaw;

    roll = normalizeAngle( osg::RadiansToDegrees( rollRad ), !_rightHanded );


    // Pitch

    // Compute the angle between the yawAxisIn and _yawAxis vectors.
    const double dotYaw = osg::clampBetween<double>( yawAxisIn * _yawAxis, -1., 1. );
    double pitchRad( acos( dotYaw ) );
    // Adjust if we pitched backwards.
    const osg::Vec3d yawXYaw( _yawAxis ^ yawAxisIn );
    if( yawXYaw * pitchAxisIn > 0. )
        pitchRad = -pitchRad;

    // Adjust the rollAxisIn and yawAxisIn vectors accordingly.
    osg::Quat qPitch( pitchRad, pitchAxisIn );
    rollAxisIn = qPitch * rollAxisIn;

    pitch = normalizeAngle( osg::RadiansToDegrees( pitchRad ), !_rightHanded );


    // Yaw

    // Compute the angle between the rollAxisIn and _rollAxis vectors.
    const double dotRoll = osg::clampBetween<double>( rollAxisIn * _rollAxis, -1., 1. );
    double yawRad( acos( dotRoll ) );
    // Adjust if we yawed left.
    if( rollAxisIn * _pitchAxis < 0. )
        yawRad = -yawRad;
    yaw = normalizeAngle( osg::RadiansToDegrees( yawRad ), !_rightHanded );
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
