// Copyright (c) 2011 Digital Transforms.

#include <osgwMx/MxMovementStyleGamePad.h>

namespace osgwMx
{

#define ROTATE_X_AXIS_IDX 2      // the range value (axis) index to be used for X axis rotation.
#define ROTATE_Y_AXIS_IDX 5      // the range value index to be used for Y axis rotation.
#define MOVE_X_AXIS_IDX 0        // the range value index to be used for X axis movement.
#define MOVE_Y_AXIS_IDX 1        // the range value index to be used for Y axis movement.
#define ZOOM_BTN_IDX 1           // the index of the button that, when pressed, turns MOVE_Y_AXIS_IDX into a zoom.
#define DOLLY_BTN_IDX 0          // the index of the button that, when pressed, turns MOVE_Y_AXIS_IDX into a dolly.
#define SLOW_RATE_BTN_IDX 6      // the index of the button that, when pressed, slows the change rate by a factor of 4.
#define FAST_RATE_BTN_IDX 4      // the index of the button that, when pressed, speeds up the change rate by a factor of 4.
#define RESET_MATRIX_BTN_IDX 5   // the index of the button that, when pressed, will reset the view matrix to its original state.

// **************************************************************************

MxMovementStyleGamePad::MxMovementStyleGamePad(MxInputAdapter *ia) :
   MxMovementStyle(ia),
   _dpadMoveAmount(0.5),
   _lastMoveDpadTime(0),
   _movingDpad(false),
   _reverseMoveDpadX(false),
   _reverseMoveDpadY(false)

{
}

// **************************************************************************
// this is the function that is called from MxMovementStyle::updateData(),
// and modifies the matrix according to game pad input.

void MxMovementStyleGamePad::matrixTransform(double ts)

{
   if ((_mxCore.valid() == false) || (_inputAdapter.valid() == false))
      return;
   // check if need to reset the matrix.
   if (_inputAdapter->buttons[RESET_MATRIX_BTN_IDX])
      resetMatrix();
   // determine the rate of change, accounting for speed modifier buttons.
   double rate = _chgRate;
   if (_inputAdapter->buttons[FAST_RATE_BTN_IDX])
      rate *= 4;
   else if (_inputAdapter->buttons[SLOW_RATE_BTN_IDX])
      rate /= 4;
   // call the rotational, movement, and zoom functions to do their thing
   // depending on the input supplied by the input adapter.
   rotateHandler(ts, rate);
   moveHandler(ts, rate);
   moveDpadHandler(ts, rate);
//   zoomHandler(ts, rate);
   dollyHandler(ts, rate);
}

// **************************************************************************

void MxMovementStyleGamePad::rotateHandler(double ts, double rate)

{
   // check rotational axis.
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
         double x = _inputAdapter->rangeValues[ROTATE_X_AXIS_IDX] * rate;
         double y = -(_inputAdapter->rangeValues[ROTATE_Y_AXIS_IDX] * rate);
         // check for reversed direction.
         if (_reverseRotateX)
            x = -x;
         if (_reverseRotateY)
            y = -y;
         _mxCore->rotate(osg::Vec2d(0, 0),
            osg::Vec2d(x, y));
         }
      }
   // shut down the operation when the input stops.
   else if (_rotating)
      _rotating = false;
}

// **************************************************************************

void MxMovementStyleGamePad::moveHandler(double ts, double rate)

{
   if ((_inputAdapter->rangeValues[MOVE_X_AXIS_IDX] || _inputAdapter->rangeValues[MOVE_Y_AXIS_IDX]) &&
      (_inputAdapter->buttons[ZOOM_BTN_IDX] == false) && (_inputAdapter->buttons[DOLLY_BTN_IDX] == false))
      {
      // check if just starting the operation.
      if (_moving == false)
         {
         _moving = true;
         _lastMoveTime = ts;
         _mxCore->setPanStart(0, 0);
         }
      // make sure some time has ellapsed since the last call.
      else if ((ts - _lastMoveTime) > 0)
         {
         // modify the change rate by the time difference since last call.
         rate *= ts - _lastMoveTime;
         _lastMoveTime = ts;
         double x = _inputAdapter->rangeValues[MOVE_X_AXIS_IDX] * rate;
         double y = -(_inputAdapter->rangeValues[MOVE_Y_AXIS_IDX] * rate);
         // check for reversed direction.
         if (_reverseMoveX)
            x = -x;
         if (_reverseMoveY)
            y = -y;
         _mxCore->pan(x, y);
         }
      }
   // shut down the operation when the input stops.
   else if (_moving)
      _moving = false;
}

// **************************************************************************

void MxMovementStyleGamePad::moveDpadHandler(double ts, double rate)

{
   if (_inputAdapter->dpad.left || _inputAdapter->dpad.right || _inputAdapter->dpad.up || _inputAdapter->dpad.down)
      {
      // check if just starting the operation.
      if (_movingDpad == false)
         {
         _movingDpad = true;
         _lastMoveDpadTime = ts;
         _mxCore->setPanStart(0, 0);
         }
      // make sure some time has ellapsed since the last call.
      else if ((ts - _lastMoveDpadTime) > 0)
         {
         // modify the change rate by the time difference since last call.
         rate *= ts - _lastMoveDpadTime;
         _lastMoveDpadTime = ts;
         double x = 0, y = 0;
         if (_inputAdapter->dpad.left)
            x = -_dpadMoveAmount;
         else if (_inputAdapter->dpad.right)
            x = _dpadMoveAmount;
         if (_inputAdapter->dpad.up)
            y = _dpadMoveAmount;
         else if (_inputAdapter->dpad.down)
            y = -_dpadMoveAmount;
         x *= rate;
         y *= rate;
         // check for reversed direction.
         if (_reverseMoveDpadX)
            x = -x;
         if (_reverseMoveDpadY)
            y = -y;
         _mxCore->pan(x, y);
         }
      }
   // shut down the operation when the input stops.
   else if (_movingDpad)
      _movingDpad = false;
}

// **************************************************************************

void MxMovementStyleGamePad::zoomHandler(double ts, double rate)

{
   // if the zoom button is pressed, then use the Y movement axis as a zoom.
   if (_inputAdapter->buttons[ZOOM_BTN_IDX] && _inputAdapter->rangeValues[MOVE_Y_AXIS_IDX])
      {
      // check if just starting the operation.
      if (_zooming == false)
         {
         _zooming = true;
         _lastZoomTime = ts;
         }
      // make sure some time has ellapsed since the last call.
      else if ((ts - _lastZoomTime) > 0)
         {
         rate *= ts - _lastZoomTime;
         _lastZoomTime = ts;
         double chg = _inputAdapter->rangeValues[MOVE_Y_AXIS_IDX] * rate;
         // check for reversed direction.
         if (_reverseZoom)
            chg = -chg;
         _mxCore->setFovyScale(1.0 + abs(chg));
         if (chg > 0)
            _mxCore->fovyScaleUp();
         else
            _mxCore->fovyScaleDown();
         }
      }
   // shut down the operation when the input stops.
   else if (_zooming)
      _zooming = false;
}

// **************************************************************************

void MxMovementStyleGamePad::dollyHandler(double ts, double rate)

{
   // if the dolly button is pressed, then use the Y movement axis as a dolly.
   if (_inputAdapter->buttons[DOLLY_BTN_IDX] && _inputAdapter->rangeValues[MOVE_Y_AXIS_IDX])
      {
      // check if just starting the operation.
      if (_dollying == false)
         {
         _dollying = true;
         _lastDollyTime = ts;
         }
      // make sure some time has ellapsed since the last call.
      else if ((ts - _lastDollyTime) > 0)
         {
         rate *= ts - _lastDollyTime;
         _lastDollyTime = ts;
         double chg = _inputAdapter->rangeValues[MOVE_Y_AXIS_IDX] * rate;
         // check for reversed direction.
         if (_reverseDolly)
            chg = -chg;
         _mxCore->dolly(chg);
         }
      }
   // shut down the operation when the input stops.
   else if (_dollying)
      _dollying = false;
}

// osgwMx
}
