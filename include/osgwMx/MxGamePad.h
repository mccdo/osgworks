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

#ifndef __OSGWMX_MX_GAME_PAD_H__
#define __OSGWMX_MX_GAME_PAD_H__ 1


#include <osgwMx/Export.h>
#include <osgwMx/MxCore.h>
#include <osg/Object>
#include <osg/ref_ptr>
#include <osg/Vec2f>

namespace osgwMx {


/** \class MxGamePad MxGamePad.h <osgwx/MxGamePad.h>
\brief A base class to support various GamePad SDKs.

To support a new SDK, such as VR Juggler, derive a class from MxGamePad.
Implement a function in your class to receive events (either handle events
sent from the SDK, or a function to poll the SDK device once per frame).
Call that function anything you want -- it's your class's specific function,
and the MxGamePad base class imposes no policy on its interface or how you
implement it.

In that event handling function, call into this base class as appropriate
in response to events or device state. Functions are provided for left stick
x and y axes, right stick x and y axes, and an unsigned int button mask.
The "DPad" (above the left stick on many game pads) is not currently supported.

The base class "has a" instance of an MxCore object. Functions to set stick
positions and button state result in immediate calls into MxCore. The
MxCore object is fully exposed. An application can retrieve the MxCore
managed matrix as desired. Your MxGamePad- derived class can also call
directly into the \c _mxCore member variable, if necessary, to support
functionality unavailable in the MxGamePad base class.
*/
class OSGWMX_EXPORT MxGamePad : public osg::Object
{
public:
    MxGamePad();
    MxGamePad( const MxGamePad& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    META_Object(osgwMx,MxGamePad);

    /* \brief Set the left stick position.

    Left stick controls movement. setLeftStick modifies the MxCore position based on the
    specified values once per function call. For example, calling setLeftStick( -1., 0. )
    twice in a row will move the MxCore position -2.0 in x.

    With no buttons depressed, left stick x axis moves the position left/right, and y
    axis moves the position forward/backward. Negative values move left and backward,
    while positive values move right and forward. If the BottomButton is depressed
    ("RGB" buttons in front of right stick), then left stick y axis moves the position
    up and down. Positive values move up.

    If RightShoulderBottom is depressed, move speed scales by 1/3 (slows down). If
    RightShoulderTop is depressed, move speed increases 3x. (This is useful for short-
    duration speed changes; but apps will probably want to call MxCore::setMoveScale()
    as appropriate depending on their world coordinate units.)
    */
    virtual void setLeftStick( const float x, const float y );
    void getLeftStick( float& x, float& y ) const
    {
        x = _leftStick[0];
        y = _leftStick[1];
    }

    /* \brief Set the right stick position.

    Right stick controls rotation. The x value rotates around the current
    view up vector. The y value rotates around the "right" vector (the
    cross product of the view direction and view up vector).

    Positive values rotate counterclockwise. For example, if the MxCore
    class is being used to manage a view matrix, and setRightStick is called
    with a positive x value, this rotates the view counterclockwise around
    the up vector, resulting in "turning your head left". In the same situation,
    a positive y value "looks up".
    */
    virtual void setRightStick( const float x, const float y );
    void getRightStick( float& x, float& y ) const
    {
        x = _rightStick[0];
        y = _rightStick[1];
    }

    /* \brief Set the stick dead zone.

    Specify a non-zero dead zone to eliminate non-zero values from
    idle game pad sticks.

    Both setLeftStick and setRightStick check their input parameters
    against the specified dead zone value. If the absolute value of
    a parameter is less that \c dz, the value is zeroed. The default
    dead zone value is 0.0.

    Most game pad sticks produce non-zero values for sticks that are
    actually idle. Call setStickDeadZone() to eliminate this issue.

    As an example usage, where setLeftStick and setRightStick are called
    with \c x and \c y values in the range -1 to 1, a \c dz dead zone value
    of 0.05 would mean that 5% around the stick center position would be
    trated as a zero value. */
    void setStickDeadZone( const float dz ) { _deadZone = dz; }
    float getStickDeadZone() const { return( _deadZone ); }

    typedef enum {
        TopButton = ( 0x1 << 0 ),             // Jump to world origin.
        RightButton = ( 0x1 << 1 ),           // Unused.
        BottomButton = ( 0x1 << 2 ),          // When held, left stick moves up/down.
        LeftButton = ( 0x1 << 3 ),            // Jump to home position.
        LeftShoulderBottom = ( 0x1 << 4 ),    // Unused.
        LeftShoulderTop = ( 0x1 << 5 ),       // Unused.
        RightShoulderBottom = ( 0x1 << 6 ),   // When held, move speed scales by 0.3333x.
        RightShoulderTop = ( 0x1 << 7 )       // When held, move speed scales by 3x.
    } ButtonValues;
    /* \brief Set the current button state.

    \param buttons A bit mask of ButtonValues.

    \li Top RGB Button: Jump to world origin.
    \li Right RGB Button: Unused.
    \li Bottom RGB Button: When held, left stick moves up/down.
    \li Left RGB Button: Jump to home position.
    \li Left Shoulder Bottom: Unused.
    \li Left Shoulder Top: Unused.
    \li Right Shoulder Bottom: When held, move speed scales by 0.3333x.
    \li Right Shoulder Top: When held, move speed scales by 3x.

    Note that derived classes should call setButtons even when all buttons are zero.
    */
    virtual void setButtons( const unsigned int buttons );
    unsigned int getButtons() const { return( _buttons ); }

    /* \brief Get the MxCore for access to the managed matrix.
    */
    MxCore* getMxCore() { return( _mxCore.get() ); }
    const MxCore* getMxCore() const { return( _mxCore.get() ); }

protected:
    virtual ~MxGamePad();

    virtual float deadZone( const float value )
    {
        return( ( osg::absolute< float >( value ) > _deadZone ) ? value : 0.f );
    }

    osg::Vec2f _leftStick;
    osg::Vec2f _rightStick;
    unsigned int _buttons;

    float _deadZone;

    osg::ref_ptr< MxCore > _mxCore;
};


// osgwMx
}


// __OSGWMX_MX_GAME_PAD_H__
#endif
