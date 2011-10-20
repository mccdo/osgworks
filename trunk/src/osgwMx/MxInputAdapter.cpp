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

#include <osgwMx/MxInputAdapter.h>

namespace osgwMx
{

// **************************************************************************

void MxInputAdapter::init()

{
   int cnt;
   for (cnt = 0; cnt < BUTTON_COUNT; cnt++)
      buttons[cnt] = false;
   for (cnt = 0; cnt < RANGE_VALUE_COUNT; cnt++)
      {
      rangeValues[cnt] = 0;
      // create the dead zone for range values such as analog sticks on a game pad.
      rangeValueAttenuate[cnt] = 0.10;          // by default, throw away the lower +/-10%. Should allow all but the most terrible / broken hardware to be used effectively.
      }
   for (cnt = 0; cnt < MOVEMENT_VALUE_COUNT; cnt++)
      {
      movementValues[cnt] = 0;
      movementValueAttenuate[cnt] = 0;
      }
}

// **************************************************************************

void MxInputAdapter::copy(const MxInputAdapter &o)

{
   int cnt;
   dpad = o.dpad;
   for (cnt = 0; cnt < BUTTON_COUNT; cnt++)
      buttons[cnt] = o.buttons[cnt];
   for (cnt = 0; cnt < RANGE_VALUE_COUNT; cnt++)
      {
      rangeValues[cnt] = o.rangeValues[cnt];
      rangeValueAttenuate[cnt] = o.rangeValueAttenuate[cnt];
      }
   for (cnt = 0; cnt < MOVEMENT_VALUE_COUNT; cnt++)
      {
      movementValues[cnt] = o.movementValues[cnt];
      movementValueAttenuate[cnt] = o.movementValueAttenuate[cnt];
      }
}

// **************************************************************************

void MxInputAdapter::clear()

{
   dpad.clear();
   int cnt;
   for (cnt = 0; cnt < BUTTON_COUNT; cnt++)
      buttons[cnt] = false;
   for (cnt = 0; cnt < RANGE_VALUE_COUNT; cnt++)
      rangeValues[cnt] = 0;
   for (cnt = 0; cnt < MOVEMENT_VALUE_COUNT; cnt++)
      movementValues[cnt] = 0;
}

// osgwMx
}
