// Copyright (c) 2011 Digital Transforms.

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
      rangeValueAttenuate[cnt] = 0.10;          // by default, throw away the lower 10%. Should allow all but the most terrible / broken hardware to be used.
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
