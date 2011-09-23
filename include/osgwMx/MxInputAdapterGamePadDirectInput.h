// Copyright (c) 2011 Digital Transforms.

#ifndef __OSGWMX_MX_MXINPUTADAPTERGAMEPADDIRECTINPUT_H__
#define __OSGWMX_MX_MXINPUTADAPTERGAMEPADDIRECTINPUT_H__

#include <dinput.h>
#include <string>
#include <list>

#include <osgwMx/MxInputAdapter.h>

namespace osgwMx
{

// Note: this class is not meant to be copied. An instance will create
// and make use of DirectInput references / handles, etc.

class OSGWMX_EXPORT MxInputAdapterGamePadDirectInput : public MxInputAdapter
   {
   public:
      MxInputAdapterGamePadDirectInput();
      ~MxInputAdapterGamePadDirectInput();

      // by default, instantiating this object will try to make use of the
      // first gaming device it finds on the system. A specific device can be
      // selected with the following call, which will deallocate any device
      // it decided to use, and return 'true' if it successfully allocated
      // the specified device.
      // note: device name must match exactly.
      bool SelectDevice(const std::string &deviceName);

      // inherited from MxInputAdapter, and called to poll device data.
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

// osgwMx
}

#endif