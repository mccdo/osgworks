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

#include <osgwMx/MxMovementStyleGamePad.h>

#include <stdio.h>
#include <ostream>
#include <iostream>

using std::abs;

namespace osgwMx
{


#define ROTATE_X_AXIS_IDX 2      // the range value (axis) index to be used for X axis rotation.
#define ROTATE_Y_AXIS_IDX 5      // the range value index to be used for Y axis rotation.
#define MOVE_X_AXIS_IDX 0        // the range value index to be used for X axis movement.
#define MOVE_Y_AXIS_IDX 1        // the range value index to be used for Y axis movement.
#define UPDOWN_BTN_IDX 1         // when pressed, this button makes the MOVE_Y_AXIS_IDX stick move up-down.
#define SLOW_RATE_BTN_IDX 6      // the index of the button that, when pressed, slows the change rate by a factor of 4.
#define FAST_RATE_BTN_IDX 7      // the index of the button that, when pressed, speeds up the change rate by a factor of 4.
#define RESET_MATRIX_BTN_IDX 5   // the index of the button that, when pressed, will reset the view matrix to its original state.
#define JUMP_TO_ORIGIN_BTN_IDX 4 // This butten sets the view position to the origin.

// **************************************************************************

MxMovementStyleGamePad::MxMovementStyleGamePad(MxInputAdapter *ia)
  : MxMovementStyle(ia),
    _dpadMoveAmount(0.5),
    _lastMoveDpadTime(0),
    _movingDpad(false),
    _reverseMoveDpadX(false),
    _reverseMoveDpadY(false)
{
    _mxCore->setMoveScale( osg::Vec3d( 8., 8., 8. ) );
}

// **************************************************************************
// this is the function that is called from MxMovementStyle::updateData(),
// and modifies the matrix according to game pad input.

void MxMovementStyleGamePad::matrixTransform(double ts)

{
    if( !( _mxCore.valid() ) || !( _inputAdapter.valid() ) )
        return;

    // check if need to reset the matrix.
    if( _inputAdapter->buttons[ RESET_MATRIX_BTN_IDX ] )
        _mxCore->reset();
    if( _inputAdapter->buttons[ JUMP_TO_ORIGIN_BTN_IDX ] )
        _mxCore->setPosition( osg::Vec3d( 0., 0., 0. ) );

    // determine the rate of change, accounting for speed modifier buttons.
    if( ( _inputAdapter->buttons[FAST_RATE_BTN_IDX] ) ||
        ( _inputAdapter->buttons[SLOW_RATE_BTN_IDX] ) )
    {
        osg::Vec3d moveScale = _mxCore->getMoveScale();
        if( _inputAdapter->buttons[ FAST_RATE_BTN_IDX ] )
            moveScale *= 1.01;
        else if( _inputAdapter->buttons[ SLOW_RATE_BTN_IDX ] )
            moveScale /= 1.01;
        _mxCore->setMoveScale( moveScale );
    }

    // call the rotational, movement, and zoom functions to do their thing
    // depending on the input supplied by the input adapter.
    rotateHandler(ts, 1.);
    moveHandler(ts, 1.);
    //moveDpadHandler(ts, 1.);
}

// **************************************************************************
// this takes the range values indexed at ROTATE_X_AXIS_IDX and ROTATE_Y_AXIS_IDX
// (see #define's at the top of this file), modified by the change rate, and
// feeds them as X & Y values to MxCore::rotate().

void MxMovementStyleGamePad::rotateHandler(double ts, double rate)

{
    // check rotational axis. If an X and/or Y axis is not centered...
    if (_inputAdapter->rangeValues[ROTATE_X_AXIS_IDX] || _inputAdapter->rangeValues[ROTATE_Y_AXIS_IDX])
    {
        // check if just starting the operation.
        if (_rotating == false)
        {
            _rotating = true;
            _lastRotateTime = ts;
        }
        // make sure some time has ellapsed since the last call.
        else if ((ts - _lastRotateTime) > 0)
        {
            // modify the change rate by the time difference since last call.
            rate *= ts - _lastRotateTime;
            _lastRotateTime = ts;
            double x = -(_inputAdapter->rangeValues[ROTATE_Y_AXIS_IDX] * rate);
            double y = _inputAdapter->rangeValues[ROTATE_X_AXIS_IDX] * rate;
            // check for reversed direction.
            if (_reverseRotateX)
                x = -x;
            if (_reverseRotateY)
                y = -y;
            double angleScale( 0.4 );
            _mxCore->rotate( x * angleScale, _mxCore->getUp() );
            _mxCore->rotate( y * angleScale, _mxCore->getCross() );
        }
    }
    // shut down the operation when the input stops.
    else if (_rotating)
        _rotating = false;
}

// **************************************************************************
// this takes the range values indexed at MOVE_X_AXIS_IDX and MOVE_Y_AXIS_IDX
// (see #define's at the top of this file), modified by the change rate, and
// feeds them as X & Y values to MxCore::pan(). This happens if neither the
// zoom button or dolly button are held down.

void MxMovementStyleGamePad::moveHandler(double ts, double rate)

{
    if( (_inputAdapter->rangeValues[MOVE_X_AXIS_IDX] || _inputAdapter->rangeValues[MOVE_Y_AXIS_IDX]) )
    {
        // check if just starting the operation.
        if (_moving == false)
        {
            _moving = true;
            _lastMoveTime = ts;
        }
        // make sure some time has ellapsed since the last call.
        else if ((ts - _lastMoveTime) > 0)
        {
            // modify the change rate by the time difference since last call.
            rate *= ts - _lastMoveTime;
            _lastMoveTime = ts;
            double x = _inputAdapter->rangeValues[MOVE_X_AXIS_IDX] * rate;
            double y = _inputAdapter->rangeValues[MOVE_Y_AXIS_IDX] * rate;
            // check for reversed direction.
            if (_reverseMoveX)
                x = -x;
            if (_reverseMoveY)
                y = -y;

            osg::Vec3d movement;
            if( _inputAdapter->buttons[UPDOWN_BTN_IDX] )
                movement.set( 0., -y, 0. );
            else
                movement.set( x, 0., y );
            _mxCore->move( movement );
        }
    }
    // shut down the operation when the input stops.
    else if (_moving)
        _moving = false;
}

// **************************************************************************
// given any dpad 'true' (boolean) input, feed the time-modified dpad movement
// as x and y values to MxCore::pan().

void MxMovementStyleGamePad::moveDpadHandler(double ts, double rate)

{
    if (_inputAdapter->dpad.left || _inputAdapter->dpad.right || _inputAdapter->dpad.up || _inputAdapter->dpad.down)
    {
        // check if just starting the operation.
        if (_movingDpad == false)
        {
            _movingDpad = true;
            _lastMoveDpadTime = ts;
        }
        // make sure some time has ellapsed since the last call.
        else if ((ts - _lastMoveDpadTime) > 0)
        {
            double x = 0, y = 0;
            if (_inputAdapter->dpad.left)
                x = _dpadMoveAmount;
            else if (_inputAdapter->dpad.right)
                x = -_dpadMoveAmount;
            if (_inputAdapter->dpad.up)
                y = -_dpadMoveAmount;
            else if (_inputAdapter->dpad.down)
                y = _dpadMoveAmount;

            // modify the change rate by the time difference since last call.
            x *= ( ts - _lastMoveDpadTime );
            y *= ( ts - _lastMoveDpadTime );
            _lastMoveDpadTime = ts;

            // check for reversed direction.
            if (_reverseMoveDpadX)
                x = -x;
            if (_reverseMoveDpadY)
                y = -y;

            osg::Vec3d up, dir, pos;
            double fovy;
            _mxCore->getInitialValues( up, dir, pos, fovy );

            //_mxCore->rotate( x, up, _pickCenter );
            //_mxCore->rotate( y, _mxCore->getCross(), _pickCenter );
        }
    }
    // shut down the operation when the input stops.
    else if (_movingDpad)
        _movingDpad = false;
}


// osgwMx
}
