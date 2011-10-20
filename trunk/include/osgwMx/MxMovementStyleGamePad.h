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

#ifndef __OSGWMX_MX_MXMOVEMENTSTYLEGAMEPAD_H__
#define __OSGWMX_MX_MXMOVEMENTSTYLEGAMEPAD_H__ 1

#include <osgwMx/MxMovementStyle.h>

namespace osgwMx
{


/** \class MxMovementStyleGamePad MxMovementStyleGamePad.h <osgwMx/MxMovementStyleGamePad.h>
\brief A class to make matrix transformations based on a game pad input adapter.
*/
class OSGWMX_EXPORT MxMovementStyleGamePad : public MxMovementStyle
{
public:
    MxMovementStyleGamePad(MxInputAdapter *ia);

    void setDpadMoveAmount(double chg) {_dpadMoveAmount = chg;}
    double getDPadMoveAmount() const {return _dpadMoveAmount;}

    /** for reversing the movement directions specific to a game pad. */
    void setReverseMoveDpadX(bool state) {_reverseMoveDpadX = state;}
    bool getReverseMoveDpadX() const {return _reverseMoveDpadX;}
    void setReverseMoveDpadY(bool state) {_reverseMoveDpadY = state;}
    bool getReverseMoveDpadY() const {return _reverseMoveDpadY;}

protected:
    /** this is the function that is called from MxMovementStyle::updateData(),
    and modifies the matrix according to game pad input. */
    virtual void matrixTransform(double ts);

    void rotateHandler(double ts, double rate);
    void moveHandler(double ts, double rate);
    void moveDpadHandler(double ts, double rate);

private:
    double _dpadMoveAmount;       // amount of change to affect (at the specified rate) when holding the dpad down.
    double _lastMoveDpadTime;     // the last move timestamp when moving is active using the dpad.
    bool _movingDpad;             // true if moving with the dpad
    bool _reverseMoveDpadX;
    bool _reverseMoveDpadY;
};


// osgwMx
}


// __OSGWMX_MX_MXMOVEMENTSTYLEGAMEPAD_H__
#endif
