// Copyright (c) 2011 Digital Transforms.

#ifndef __OSGWMX_MX_MXINPUTADAPTER_H__
#define __OSGWMX_MX_MXINPUTADAPTER_H__

#include <memory.h>

#include <osg/Referenced>

#include <osgwMx/Export.h>

namespace osgwMx
{

// --------------------------------------------------------------------------
// a specific class to store dpad input rather than just lump it
// in with the buttons.

class OSGWMX_EXPORT DPad : public osg::Referenced
   {
   public:
      DPad() {init();}
      DPad(const DPad &o) {copy(o);}

      DPad &operator=(const DPad &rhs) {copy(rhs); return *this;}

      void clear() {init();}

      bool left;
      bool right;
      bool up;
      bool down;

   protected:
      void init() {left = false; right = false; up = false; down = false;}

   private:
      void copy(const DPad &o)
         {
         left = o.left;
         right = o.right;
         up = o.up;
         down = o.down;
         }
   };

// --------------------------------------------------------------------------

class OSGWMX_EXPORT MxInputAdapter : public osg::Referenced
   {
   public:
      MxInputAdapter() {init();}
      MxInputAdapter(const MxInputAdapter &o) {copy(o);}
      virtual ~MxInputAdapter() {}

      MxInputAdapter &operator=(const MxInputAdapter &rhs) {copy(rhs); return *this;}

      bool operator==(const MxInputAdapter &rhs) {if (memcmp(this, &rhs, sizeof(MxInputAdapter)) == 0) return true; return false;}

      // reset all data / input values, but don't change attentuation values.
      void clear();

      // the inheriting class must implement this to read values from the actual device.
      // NOTE: this function is not pure virtual so that MxInputAdapter may be instantiated
      // by itself merely as a data holder. Could be used for things like comparing
      // previous input for changes.
      virtual void updateData() {}

      // the number of each type of input.
      enum {BUTTON_COUNT = 32};
      enum {RANGE_VALUE_COUNT = 8};
      enum {MOVEMENT_VALUE_COUNT = 8};

      // the device value members.
      DPad dpad;
      bool buttons[BUTTON_COUNT];                     // true = button pressed.
      double rangeValues[RANGE_VALUE_COUNT];          // values are 0 to +/- 1.0. These are range values from input such as an analog stick on a game pad.
      double movementValues[MOVEMENT_VALUE_COUNT];    // values from 'movement' type inputs such as a roller ball, mouse, 3D glove, etc.
      double rangeValueAttenuate[RANGE_VALUE_COUNT];  // the corresponding absolute value must be > the attenuation value to take effect.
      double movementValueAttenuate[MOVEMENT_VALUE_COUNT];

   protected:
      void init();
      void copy(const MxInputAdapter &o);
   };

// osgwMx
}

#endif