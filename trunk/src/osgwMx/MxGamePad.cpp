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


#include <osgwMx/MxGamePad.h>
#include <osg/io_utils>
#include <osg/Notify>

#include <osg/Math>


namespace osgwMx {


MxGamePad::MxGamePad()
  : osg::Object(),
    _leftStick( osg::Vec2f( 0.f, 0.f ) ),
    _rightStick( osg::Vec2f( 0.f, 0.f ) ),
    _buttons( 0 ),
    _deadZone( 0.f ),
    _leftRate( 1. ),
    _rightRate( 60. ),
    _rotationPoint( osg::Vec3d( 0.f, 0.f, 0.f ) )
{
    _mxCore = new osgwMx::MxCore;
}
MxGamePad::MxGamePad( const MxGamePad& rhs, const osg::CopyOp& copyop )
  : osg::Object( rhs, copyop ),
    _mxCore( rhs._mxCore ),
    _leftStick( rhs._leftStick ),
    _rightStick( rhs._rightStick ),
    _buttons( rhs._buttons ),
    _deadZone( rhs._deadZone ),
    _leftRate( rhs._leftRate ),
    _rightRate( rhs._rightRate ),
    _rotationPoint( rhs._rotationPoint )
{
    if( !( _mxCore.valid() ) )
        _mxCore = new osgwMx::MxCore;
}
MxGamePad::~MxGamePad()
{
}

bool MxGamePad::setLeftStick( const float x, const float y )
{
    _leftStick.set( x, y );

    // Zero the values if they fall within the dead zone.
    float myX( deadZone( x ) );
    float myY( deadZone( y ) );

    if( myX == 0 && myY == 0 )
    {
        return( false );
    }
    
    internalLeftStick( myX, myY );
    
    return( true );
}
bool MxGamePad::setLeftStick( const float x, const float y, const double elapsedSeconds )
{
    _leftStick.set( x, y );

    // Zero the values if they fall within the dead zone.
    const float myX( deadZone( x ) );
    const float myY( deadZone( y ) );

    if( myX == 0 && myY == 0 )
    {
        return( false );
    }
    
    // How far do we go at 100% movement?
    const float maxDistance = (float)( _leftRate * elapsedSeconds );

    internalLeftStick( myX * maxDistance, myY * maxDistance );
    
    return( true );
}
void MxGamePad::internalLeftStick( const float x, const float y )
{
    // Check Button2 for forward/backward or up/down.
    osg::Vec3d movement;
    if( ( _buttons & Button2 ) != 0 )
        // Move left/right and up/down.
        // Positive values move up, so negate y.
        movement.set( x, -y, 0. );
    else
        // Move left/right and forwards/backwards.
        movement.set( x, 0., y );

    if( _buttons & Button8 )
    {
        _mxCore->moveWorldCoords( movement );
        return;
    }
    
    if( _buttons & Button9 )
    {
        _mxCore->moveConstrained( movement );
        return;
    }
    
    ///By default we will move in local coordinate space
    _mxCore->move( movement );
}

bool MxGamePad::setRightStick( const float x, const float y )
{
    _rightStick.set( x, y );

    // Zero the values if they fall within the dead zone.
    float myX( deadZone( x ) );
    float myY( deadZone( y ) );

    if( myX == 0 && myY == 0 )
    {
        return( false );
    }
    
    internalRightStick( myX, myY );
    
    return( true );
}
bool MxGamePad::setRightStick( const float x, const float y, const double elapsedSeconds )
{
    _rightStick.set( x, y );

    // Zero the values if they fall within the dead zone.
    float myX( deadZone( x ) );
    float myY( deadZone( y ) );

    if( myX == 0 && myY == 0 )
    {
        return( false );
    }

    // How far do we turn at 100% rotation?
    const float maxDegrees = (float)( _rightRate * elapsedSeconds );
    
    internalRightStick( myX * maxDegrees, myY * maxDegrees );
    
    return( true );
}
void MxGamePad::internalRightStick( const float x, const float y )
{
    // Input is degrees, but MxCore wants radians.
    const double myX = osg::DegreesToRadians( x );
    const double myY = osg::DegreesToRadians( y );

    if( _buttons & Button10 )
    {
        _mxCore->rotate( myX, _mxCore->getUp(), _rotationPoint );
        _mxCore->rotate( myY, _mxCore->getCross(), _rotationPoint );
        return;
    }

    _mxCore->rotate( myX, _mxCore->getUp() );
    _mxCore->rotate( myY, _mxCore->getCross() );
}
void MxGamePad::setButtons( const unsigned int buttons )
{
    // Determine which buttons just entered a pressed or released state.
    const unsigned int deltaPressed = ( buttons ^ _buttons ) & buttons;
    const unsigned int deltaReleased = ( buttons ^ _buttons ) & _buttons;

    // Scale movement based on right shoulder button state.
    if( ( deltaPressed & Button6 ) != 0 )
        _mxCore->setMoveScale( osg::Vec3d( .33, .33, .33 ) );
    else if( ( deltaPressed & Button7 ) != 0 )
        _mxCore->setMoveScale( osg::Vec3d( 3., 3., 3. ) );
    if( ( ( deltaReleased & Button6 ) != 0 ) ||
        ( ( deltaReleased & Button7 ) != 0 ) )
        _mxCore->setMoveScale( osg::Vec3d( 1., 1., 1. ) );

    if( ( deltaPressed & Button3 ) != 0 )
        _mxCore->reset();
    if( ( deltaPressed & Button0 ) != 0 )
        _mxCore->setPosition( osg::Vec3( 0., 0., 0. ) );
    if( ( deltaPressed & Button1 ) != 0 )
        _mxCore->level();

    _buttons = buttons;
}
// osgwMx
}
