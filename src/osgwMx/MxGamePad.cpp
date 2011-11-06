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
    _deadZone( 0.f )
{
    _mxCore = new osgwMx::MxCore;
}
MxGamePad::MxGamePad( const MxGamePad& rhs, const osg::CopyOp& copyop )
  : osg::Object( rhs, copyop ),
    _mxCore( rhs._mxCore ),
    _leftStick( rhs._leftStick ),
    _rightStick( rhs._rightStick ),
    _buttons( rhs._buttons ),
    _deadZone( rhs._deadZone )
{
    if( !( _mxCore.valid() ) )
        _mxCore = new osgwMx::MxCore;
}
MxGamePad::~MxGamePad()
{
}


void MxGamePad::setLeftStick( const float x, const float y )
{
    _leftStick.set( x, y );

    // Zero the values if they fall within the dead zone.
    float myX( deadZone( x ) );
    float myY( deadZone( y ) );

    // Scale movement based on depressed buttons.
    float scale( 1.f );
    if( ( _buttons & RightShoulderBottom ) != 0 )
        scale = 0.3333;
    else if( ( _buttons & RightShoulderTop ) != 0 )
        scale = 3.;

    // Either move left/right/forward/backward, or move up/down.
    osg::Vec3d movement;
    if( ( _buttons & BottomButton ) != 0 )
        // Move up/down. Positive values move up, so negate myY.
        movement.set( 0., -myY, 0. );
    else
        // Move left/right and forwards/backwards.
        movement.set( myX, 0., myY );

    // Don't bother changing the position unless the movement vector has non-zero length.
    if( movement.length2() > 0. )
        _mxCore->move( movement * scale );
}

void MxGamePad::setRightStick( const float x, const float y )
{
    _rightStick.set( x, y );

    // Zero the values if they fall within the dead zone.
    float myX( deadZone( x ) );
    float myY( deadZone( y ) );
    if( ( myX == 0.f ) && ( myY == 0.f ) )
        return;

    // Input is degrees, but MxCore wants radians.
    myX = osg::DegreesToRadians( myX );
    myY = osg::DegreesToRadians( myY );

    _mxCore->rotate( myX, _mxCore->getUp() );
    _mxCore->rotate( myY, _mxCore->getCross() );
}

void MxGamePad::setButtons( const unsigned int buttons )
{
    // Determine which buttons just entered a pressed state.
    unsigned int delta = ( buttons ^ _buttons ) & buttons;

    if( ( delta & LeftButton ) != 0 )
        _mxCore->reset();
    if( ( delta & TopButton ) != 0 )
        _mxCore->setPosition( osg::Vec3( 0., 0., 0. ) );

    _buttons = buttons;
}


// osgwMx
}
