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

#ifndef __OSGWMX_MX_MXINPUTADAPTERGAMEPADDIRECTINPUT_H__
#define __OSGWMX_MX_MXINPUTADAPTERGAMEPADDIRECTINPUT_H__ 1

#include <dinput.h>
#include <string>
#include <list>

#include <osgwMx/MxInputAdapter.h>

namespace osgwMx
{
/** \cond */


/** \class MxInputAdapterGamePadDirectInput MxInputAdapterGamePadDirectInput.h <osgwMx/MxInputAdapterGamePadDirectInput.h>
\brief A class derived from MxInputAdapter to receive game pad input from DirectInput (part of DirectX).
DirectX usage is completely transparent and self-contained.
Note: unlike the MxInputAdapter base class, this class is not meant to be copied.
An instance will create and make use of DirectInput references / handles, etc. */
class OSGWMX_EXPORT MxInputAdapterGamePadDirectInput : public MxInputAdapter
{
public:
    MxInputAdapterGamePadDirectInput();
    ~MxInputAdapterGamePadDirectInput();

    /** by default, instantiating this object will try to make use of the
    first gaming device it finds on the system. A specific device can be
    selected with the following call, which will deallocate any device
    it decided to use, and return 'true' if it successfully allocated
    the specified device.
    note: device name is case dependent and must match exactly. */
    bool SelectDevice(const std::string &deviceName);

    /** inherited from MxInputAdapter, and called to poll device data. */
    virtual void updateData();

private:
    HWND CreateInvisiWindow();
    void DestroyWindow();
    bool OpenDirectInput();
    void FreeDirectInput();
    void FreeDevice();
    bool SelectFirstDevice();
    bool EnumDevices();
    bool SelectDevice(const DIDEVICEINSTANCE &device);
    void ExtractButtons(const DIJOYSTATE2 &devState);
    void ExtractPOV(const DIJOYSTATE2 &devState);
    void ExtractAxis(const DIJOYSTATE2 &devState);
    double GetNormalizedAxisValue(LONG av);

    static bool winClassRegistered;        // true if the window class has been registered

    friend static BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE *pdidInstance, void *pContext);

    HWND hDIWindow;                        // handle to the top-level window that DirectInput needs.
    // DirectInput stuff
    LPDIRECTINPUT8 pDI;                    // a pointer to the DirectInput v8 COM interface.
    LPDIRECTINPUTDEVICE8 pDIDevice;        // pointer to the current device being used by this.
    DIDEVCAPS devCaps;
    std::list<DIDEVICEINSTANCE> devList;   // list of gaming devices on the system.
};


/* \endcond */
// osgwMx
}


// __OSGWMX_MX_MXINPUTADAPTERGAMEPADDIRECTINPUT_H__
#endif
