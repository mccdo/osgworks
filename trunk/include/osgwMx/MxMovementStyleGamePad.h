// Copyright (c) 2011 Digital Transforms.

#ifndef __OSGWMX_MX_MXMOVEMENTSTYLEGAMEPAD_H__
#define __OSGWMX_MX_MXMOVEMENTSTYLEGAMEPAD_H__

#include <osgwMx/MxMovementStyle.h>

namespace osgwMx
{

class OSGWMX_EXPORT MxMovementStyleGamePad : public MxMovementStyle
   {
   public:
      MxMovementStyleGamePad(MxInputAdapter *ia);

      void setDpadMoveAmount(double chg) {_dpadMoveAmount = chg;}
      double getDPadMoveAmount() const {return _dpadMoveAmount;}

      // for reversing the movement directions specific to a game pad.
      void setReverseMoveDpadX(bool state) {_reverseMoveDpadX = state;}
      bool getReverseMoveDpadX() const {return _reverseMoveDpadX;}
      void setReverseMoveDpadY(bool state) {_reverseMoveDpadY = state;}
      bool getReverseMoveDpadY() const {return _reverseMoveDpadY;}

   protected:
      // this is the function that is called from MxMovementStyle::updateData(),
      // and modifies the matrix according to game pad input.
      virtual void matrixTransform(double ts);

      void rotateHandler(double ts, double rate);
      void moveHandler(double ts, double rate);
      void moveDpadHandler(double ts, double rate);
      void zoomHandler(double ts, double rate);
      void dollyHandler(double ts, double rate);

   private:
      double _dpadMoveAmount;       // amount of change to affect (at the specified rate) when holding the dpad down.
      double _lastMoveDpadTime;     // the last move timestamp when moving is active using the dpad.
      bool _movingDpad;             // true if moving with the dpad
      bool _reverseMoveDpadX;
      bool _reverseMoveDpadY;
   };

// osgwMx
}

#endif