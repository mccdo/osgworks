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

    /** Set the left stick position.

    Left stick controls movement. setLeftStick(float,float) modifies the MxCore position based on the
    specified values once per function call. For example, calling setLeftStick( -1., 0. )
    twice in a row will move the MxCore position -2.0 in x.

    Left stick \c x axis moves the position left/right. With no buttons depressed, \c y
    axis moves the position forward/backward. If the BottomButton is depressed ("RGB"
    buttons in front of right stick), then left stick \c y axis moves the position up
    and down. Negative values move left, backward, and down, while positive values move
    right, forward, and up.

    Movement is scaled by the right shoulder buttons. RightShoulderBottom decreases speed
    to 0.33x, and RightShoulderTop increases speed to 3x. Movement is further scaled
    by MxCore::setMoveScale(). While movement scaling is sufficient for small demos, it's
    often insufficient to move at a specified rate. Consider using setLeftStick(float,float,double)
    instead. See also setStickRate() and setStickRates().
    */
    virtual bool setLeftStick( const float x, const float y );
    void getLeftStick( float& x, float& y ) const
    {
        x = _leftStick[0]; y = _leftStick[1];
    }
    /** Move at a specified rate.

    Behavior is idential to that of setLeftStick(float,float), except that \c x and \c y
    values are taken as normalized percentages in the range -1.0 to 1.0 of a desired
    movement rate in units/second. The movement rate for the left stick is set with
    setStickRate() (see also setStickRates()).
    
    \param elapsedSeconds Specifies the elapsed time in seconds since the last call to
    setLeftStick(float,float,double). This function computes the delta motion based on
    \c _leftRate and \c elapsedSeconds, then scales that motion by \c x and \c y. */
    virtual bool setLeftStick( const float x, const float y, const double elapsedSeconds );

    /** Set the right stick position.

    Right stick controls rotation. The x value rotates around the current
    view up vector. The y value rotates around the "right" vector (the
    cross product of the view direction and view up vector).

    Positive values rotate counterclockwise. For example, if the MxCore
    class is being used to manage a view matrix, and setRightStick is called
    with a positive x value, this rotates the view counterclockwise around
    the up vector, resulting in "turning your head left". In the same situation,
    a positive y value "looks up".
    */
    virtual bool setRightStick( const float x, const float y );
    void getRightStick( float& x, float& y ) const
    {
        x = _rightStick[0]; y = _rightStick[1];
    }
    /** Rotate at a specified rate.

    Behavior is idential to that of setRightStick(float,float), except that \c x and \c y
    values are taken as normalized percentages in the range -1.0 to 1.0 of a desired
    rotateion rate in degrees/second. The rotation rate for the right stick is set with
    setStickRates().
    
    \param elapsedSeconds Specifies the elapsed time in seconds since the last call to
    setRightStick(float,float,double). This function computes the delta rotation based on
    \c _rightRate and \c elapsedSeconds, then scales that rotation by \c x and \c y. */
    virtual bool setRightStick( const float x, const float y, const double elapsedSeconds );

    /** Set the stick dead zone.

    Specify a non-zero dead zone to eliminate non-zero values from
    idle game pad sticks.

    Both setLeftStick() and setRightStick() check their input parameters
    against the specified dead zone value. If the absolute value of
    a parameter is less that \c dz, the value is zeroed. The default
    dead zone value is 0.0.

    Most game pad sticks produce non-zero values for sticks that are
    actually idle. Call setStickDeadZone() to eliminate this issue.

    As an example usage, where setLeftStick and setRightStick are called
    with \c x and \c y values in the range -1 to 1, a \c dz dead zone value
    of 0.05 would mean that 5% around the stick center position would be
    treated as a zero value. */
    void setStickDeadZone( const float dz ) { _deadZone = dz; }
    float getStickDeadZone() const { return( _deadZone ); }

    /** Set the left stick (movement) rate in units/second. See setLeftStick(float,float,double).
    The default is 1.0.
    */
    void setStickRate( const double leftRate )
    {
        _leftRate = leftRate;
    }
    /** Set both the left and right stick (movement and rotation) rates. See
    setLeftStick(float,float,double) and setRightStick(float,float,double).
    The default for \c leftRate movement is 1.0 units/second, and the default
    for \c rightRate rotation is 60.0 degrees/second. */
    void setStickRates( const double leftRate, const double rightRate )
    {
        _leftRate = leftRate; _rightRate = rightRate;
    }
    void getStickRates( double& leftRate, double& rightRate )
    {
        leftRate = _leftRate; rightRate = _rightRate;
    }

    /** Enumerant button values. Bitwise OR enumerants that correspond to
    pressed device buttons, and pass the result to setButtons(). */
    typedef enum {
        Button0  = ( 0x1 <<  0 ),     // Jump to world origin.
        Button1  = ( 0x1 <<  1 ),     // Level the view..
        Button2  = ( 0x1 <<  2 ),     // When held, left stick moves up/down.
        Button3  = ( 0x1 <<  3 ),     // Jump to home position.
        Button4  = ( 0x1 <<  4 ),     // Unused.
        Button5  = ( 0x1 <<  5 ),     // Unused.
        Button6  = ( 0x1 <<  6 ),     // When held, move speed scales by 0.3333x.
        Button7  = ( 0x1 <<  7 ),     // When held, move speed scales by 3x.
        Button8  = ( 0x1 <<  8 ),     // Move in world coordinate space.
        Button9  = ( 0x1 <<  9 ),     // Move with the osgwMx::MxCore::moveConstrained function.
        Button10 = ( 0x1 << 10 ),     // Rotate about a point with the right stick.
        Button11 = ( 0x1 << 11 ),     // Rotate about the direction vector and cross vector.
        Button12 = ( 0x1 << 12 ),     // Unused.
        Button13 = ( 0x1 << 13 ),     // Unused.
        Button14 = ( 0x1 << 14 ),     // Unused.
        Button15 = ( 0x1 << 15 ),     // Unused.
        Button16 = ( 0x1 << 16 ),     // Unused.
        Button17 = ( 0x1 << 17 ),     // Unused.
        Button18 = ( 0x1 << 18 ),     // Unused.
        Button19 = ( 0x1 << 19 ),     // Unused.
        Button20 = ( 0x1 << 20 ),     // Unused.
        Button21 = ( 0x1 << 21 ),     // Unused.
        Button22 = ( 0x1 << 22 ),     // Unused.
        Button23 = ( 0x1 << 23 ),     // Unused.
        Button24 = ( 0x1 << 24 ),     // Unused.
        Button25 = ( 0x1 << 25 ),     // Unused.
        Button26 = ( 0x1 << 26 ),     // Unused.
        Button27 = ( 0x1 << 27 ),     // Unused.
        Button28 = ( 0x1 << 28 ),     // Unused.
        Button29 = ( 0x1 << 29 ),     // Unused.
        Button30 = ( 0x1 << 30 ),     // Unused.
        Button31 = ( 0x1 << 31 )      // Unused.
    } Buttons;
    /** Set the current button state.

    \param buttons A bit mask composed of MxGamePad::Buttons.

    \li Top RGB Button: Jump to world origin.
    \li Right RGB Button: Level the view.
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

    /** Set the MxCore. This allows sharing an MxCore between multiple device
    handlers, such as MxEventHandler and MxGamePad. */
    void setMxCore( osgwMx::MxCore* mxCore ) { _mxCore = mxCore; }
    /** Get the MxCore for access to the managed matrix. */
    MxCore* getMxCore() { return( _mxCore.get() ); }
    const MxCore* getMxCore() const { return( _mxCore.get() ); }

    /** Set the point of rotation for the right stick rotations. This allows the
    user to press button 10 and rotate about an arbitrary point in space with 
    the right stick control.*/
    void setRotationPoint( const osg::Vec3d& point ) { _rotationPoint = point; }
    
protected:
    virtual ~MxGamePad();

    inline float deadZone( const float value )
    {
        return( ( osg::absolute< float >( value ) > _deadZone ) ? value : 0.f );
    }

    virtual void internalLeftStick( const float x, const float y );
    virtual void internalRightStick( const float x, const float y );

    osg::Vec2f _leftStick;
    osg::Vec2f _rightStick;
    unsigned int _buttons;

    float _deadZone;

    double _leftRate, _rightRate;

    osg::ref_ptr< MxCore > _mxCore;
    
    osg::Vec3d _rotationPoint;
};


// osgwMx
}


// __OSGWMX_MX_GAME_PAD_H__
#endif
