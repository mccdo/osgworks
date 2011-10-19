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

#ifndef __OSGWMX_MX_MXINPUTADAPTER_H__
#define __OSGWMX_MX_MXINPUTADAPTER_H__ 1

#include <memory.h>

#include <osg/Referenced>

#include <osgwMx/Export.h>

namespace osgwMx
{


/** \class DPad MxInputAdapter.h <osgwMx/MxInputAdapter.h>
\brief a class to store dpad state rather than just lump it in with the buttons of MxInputAdapter.
*/
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
/** \class MxInputAdapter MxInputAdapter.h <osgwMx/MxInputAdapter.h>
\brief A class for holding device acquired input. Inheriting classes should implement updateData() in order to actually acquire input data / state from the device.
Note that this class is not pure virtual so that it can be used to copy and compare states.
*/

class OSGWMX_EXPORT MxInputAdapter : public osg::Referenced
{
public:
    MxInputAdapter() {init();}
    MxInputAdapter(const MxInputAdapter &o) {copy(o);}
    virtual ~MxInputAdapter() {}

    MxInputAdapter &operator=(const MxInputAdapter &rhs) {copy(rhs); return *this;}

    bool operator==(const MxInputAdapter &rhs) {if (memcmp(this, &rhs, sizeof(MxInputAdapter)) == 0) return true; return false;}

    /** reset all data / input values, but don't change attentuation values. */
    void clear();

    /** the inheriting class must implement this to read values from the actual device.
    NOTE: this function is not pure virtual so that MxInputAdapter may be instantiated
    by itself merely as a data holder. Could be used for things like comparing
    previous input for changes. */
    virtual void updateData() {}

    /** the number of each type of input. */
    enum {BUTTON_COUNT = 32};
    enum {RANGE_VALUE_COUNT = 8};
    enum {MOVEMENT_VALUE_COUNT = 8};

    /** the device value members. */
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


// __OSGWMX_MX_MXINPUTADAPTER_H__
#endif
