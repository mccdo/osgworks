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

#include <windows.h>
#include <tchar.h>

#include <osgwMx/MxInputAdapterGamePadDirectInput.h>

using namespace std;

namespace osgwMx
{

// --------------------------------------------------------------------------

#define MIN_AXIS_RANGE -10000
#define MAX_AXIS_RANGE 10000

// --------------------------------------------------------------------------

static LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK EnumAxisCallback(const DIDEVICEOBJECTINSTANCE* pDIDOI, void *pContext);

// --------------------------------------------------------------------------

bool MxInputAdapterGamePadDirectInput::winClassRegistered = false;      // true if the window class has been registered

// **************************************************************************

MxInputAdapterGamePadDirectInput::MxInputAdapterGamePadDirectInput() : pDI(0), pDIDevice(0)

{
   // DirectInput requires a Window from the app, so try to create one now.
   // This window will be a top-level window (not a child window), but will be
   // invisible to the user.
   if ((hDIWindow = CreateInvisiWindow()) == 0)
      return;
   // try to open DirectInput 8 for use. This works as far back as Windows 95 (DirectX 8.0a sdk).
   if (OpenDirectInput())
      SelectFirstDevice();    // try to select the first gaming device on the system.
}

// **************************************************************************
// create an invisible, top-level window that DirectInput can accept.

HWND MxInputAdapterGamePadDirectInput::CreateInvisiWindow()

{
   HINSTANCE hMod = (HINSTANCE)GetModuleHandle(0);
   LPCTSTR className = _T("MxInputAdapterGamePadDirectInput Window");
   if (winClassRegistered == false)
      {
      winClassRegistered = true;
      WNDCLASSEX wc;
      memset(&wc, 0, sizeof(wc));
      wc.cbSize = sizeof(WNDCLASSEX);
      wc.lpfnWndProc = &WinProc;
      wc.hInstance = hMod;
      wc.lpszClassName = className;
      if (RegisterClassEx(&wc) == 0)
         return 0;
      }
   return ::CreateWindow(className, _T("DirectInput Window"), WS_POPUP, 0, 0, 1, 1, 0, 0, hMod, 0);
}

// **************************************************************************
// Our window procedure callback handles nothing, but passes it all down to
// the default handler. No work happens here.

static LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

{
   return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// **************************************************************************
// try to open DirectInput 8 for use.
// example error handling code provided in comments.

bool MxInputAdapterGamePadDirectInput::OpenDirectInput()

{
   if (pDI != 0)
      return true;
   HRESULT rc;
   if ((rc = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID **)&pDI, 0)) != DI_OK)
      {
      /*
      String s = "DirectInput8Create() failed with return code: ";
      s += NumToStr(rc, false);
      s += " (0x";
      s += NumToStrHex(rc);
      s += ')';
      switch (rc)
         {
         case DIERR_BETADIRECTINPUTVERSION:
            s += " [DIERR_BETADIRECTINPUTVERSION]";
            break;
         case DIERR_INVALIDPARAM:
            s += " [DIERR_INVALIDPARAM]";
            break;
         case DIERR_OLDDIRECTINPUTVERSION:
            s += " [DIERR_OLDDIRECTINPUTVERSION]";
            break;
         case DIERR_OUTOFMEMORY:
            s += " [DIERR_OUTOFMEMORY]";
            break;
         default:
            break;
         }
      SetErrorNoLookup(1, s);
      */
      return false;
      }
   return true;
}

// **************************************************************************

MxInputAdapterGamePadDirectInput::~MxInputAdapterGamePadDirectInput()

{
   FreeDirectInput();
   DestroyWindow();
}

// **************************************************************************

void MxInputAdapterGamePadDirectInput::FreeDirectInput()

{
   FreeDevice();
   if (pDI)
      {
      pDI->Release();
      pDI = 0;
      }
}

// **************************************************************************

void MxInputAdapterGamePadDirectInput::FreeDevice()

{
   if (pDIDevice)
      {
      pDIDevice->Unacquire();
      pDIDevice->Release();
      pDIDevice = 0;
      }
}

// **************************************************************************

void MxInputAdapterGamePadDirectInput::DestroyWindow()

{
   if (hDIWindow)
      {
      ::DestroyWindow(hDIWindow);
      hDIWindow = 0;
      }
}

// **************************************************************************
// Select a specific device for use. Any device previously acquired will be released.
// note: device name must match exactly (case dependent).

bool MxInputAdapterGamePadDirectInput::SelectDevice(const std::string &deviceName)

{
   // an empty string will try to select the first device DX can find.
   if (deviceName.size() == 0)
      return SelectFirstDevice();
   EnumDevices();
   // if no devices attached to the system, then return failure.
   if (devList.size() == 0)
      return false;
   std::list<DIDEVICEINSTANCE>::iterator it = devList.begin();
   while (it != devList.end())
      {
      // WARNING! Not unicode capable.
      if (deviceName == it->tszInstanceName)
         return SelectDevice(*it);
      it++;
      }
   return false;
}

// **************************************************************************

bool MxInputAdapterGamePadDirectInput::SelectFirstDevice()

{
   EnumDevices();
   // if no devices attached to the system, then return failure.
   if (devList.size() == 0)
      return false;
   std::list<DIDEVICEINSTANCE>::iterator it = devList.begin();
   return SelectDevice(*it);
}

// **************************************************************************
// Enumerate all gaming devices attached to the system.

bool MxInputAdapterGamePadDirectInput::EnumDevices()

{
   if (pDI == 0)
      return false;
   devList.clear();
   HRESULT rc;
   if ((rc = pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDevicesCallback, this, DIEDFL_ATTACHEDONLY)) != DI_OK)
      {
      /*
      String s = "EnumDevices() failed with return code: ";
      s += NumToStr(rc, false);
      s += " (0x";
      s += NumToStrHex(rc);
      s += ')';
      switch (rc)
         {
         case DIERR_INVALIDPARAM:
            s += " [DIERR_INVALIDPARAM]";
            break;
         case DIERR_NOTINITIALIZED:
            s += " [DIERR_NOTINITIALIZED]";
            break;
         default:
            break;
         }
      SetErrorNoLookup(2, s);
      */
      return false;
      }
   return true;
}

// **************************************************************************

BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE *pdidInstance, void *pUser)

{
   ((MxInputAdapterGamePadDirectInput *)pUser)->devList.push_back(*pdidInstance);
   return DIENUM_CONTINUE;                   // next please
}

// **************************************************************************
// select a device by its previously enumerated device instance.

bool MxInputAdapterGamePadDirectInput::SelectDevice(const DIDEVICEINSTANCE &device)

{
   // release any currently held device.
   FreeDevice();
   // create a device interface for the specified device.
   if (pDI->CreateDevice(device.guidInstance, &pDIDevice, 0) != DI_OK)
      {
      pDIDevice = 0;
      return false;
      }
   // set the type of data format want DirectInput to hand to us.
   if (pDIDevice->SetDataFormat(&c_dfDIJoystick2) != DI_OK)
      {
      FreeDevice();
      return false;
      }
   // use the device in a non-exclusive manner. No need to acquire it exclusively.
   // background access does not require that the window be the currently active window,
   // and will not automatically unacquire the device when the window is not the active window.
   if (pDIDevice->SetCooperativeLevel(hDIWindow, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
      {
      FreeDevice();
      return false;
      }
   devCaps.dwSize = sizeof(DIDEVCAPS);
   if (pDIDevice->GetCapabilities(&devCaps) != DI_OK)
      {
      FreeDevice();
      return false;
      }
   // set range of axis
   if (pDIDevice->EnumObjects(EnumAxisCallback, pDIDevice, DIDFT_AXIS) != DI_OK)
      {
      FreeDevice();
      return false;
      }
   HRESULT rc;
   // try to acquire the specified device for use.
   if (((rc = pDIDevice->Acquire()) != DI_OK) && (rc != S_FALSE))
      {
      FreeDevice();
      return false;
      }
   return true;
}

// **************************************************************************
// Enumerate the axis of the game controller in order to set the range to something predictable.

static BOOL CALLBACK EnumAxisCallback(const DIDEVICEOBJECTINSTANCE* pDIDOI, void *pContext)

{
   DIPROPRANGE pr;
   pr.diph.dwSize = sizeof(DIPROPRANGE);
   pr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
   pr.diph.dwHow = DIPH_BYID;
   pr.diph.dwObj = pDIDOI->dwType;           // the axis being enumerated
   // set min & max range values. this is the whole purpose of enumerating the axis.
   pr.lMin = MIN_AXIS_RANGE;
   pr.lMax = MAX_AXIS_RANGE;
   ((LPDIRECTINPUTDEVICE8)pContext)->SetProperty(DIPROP_RANGE, &pr.diph);
   return DIENUM_CONTINUE;                   // let's do 'em all, thank you
}

// **************************************************************************
// poll the device and populate the MxInputAdapter data members.

void MxInputAdapterGamePadDirectInput::updateData()

{
   if (pDIDevice == 0)
      return;
   // re-init all data values.
   clear();
   // poll the device for those devices that require it.
   HRESULT rc;
   if (((rc = pDIDevice->Poll()) != DI_OK) && (rc != DI_NOEFFECT))
      {
      pDIDevice->Acquire();            // try to reacquire device if lost it
      return;
      }
   // read the raw device state information in DirectInput format.
   DIJOYSTATE2 devState;
   if (pDIDevice->GetDeviceState(sizeof(DIJOYSTATE2), &devState) != DI_OK)
      return;
   // extract and convert DirectInput raw data into MxInputAdapter format.
   ExtractButtons(devState);
   ExtractPOV(devState);
   ExtractAxis(devState);
}

// **************************************************************************
// extract button information from DirectInput info.

void MxInputAdapterGamePadDirectInput::ExtractButtons(const DIJOYSTATE2 &devState)

{
   int cnt, maxBtns = BUTTON_COUNT;
   if (BUTTON_COUNT > 128)       // don't read more than is available in DIJOYSTATE2.
      maxBtns = 128;
   for (cnt = 0; cnt < maxBtns; cnt++)
      {
      if (devState.rgbButtons[cnt])
         buttons[cnt] = true;
      }
}

// **************************************************************************
// extract point-of-view / dpad information from DirectInput info.
// yes, this is strange, but these are the values that DirectInput uses.

void MxInputAdapterGamePadDirectInput::ExtractPOV(const DIJOYSTATE2 &devState)

{
   DWORD pov = devState.rgdwPOV[0] & 0xffff;          // get POV value
   if (pov != 0xffff)                                 // if current POV not centered
      {
      if ((pov >= 30000) || (pov <= 6000))            // forward
         {
         dpad.up = true;
         if ((pov >= 4500) && (pov <= 6000))
            dpad.right = true;
         else if ((pov >= 30000) && (pov <= 31500))
            dpad.left = true;
         }
      else if ((pov > 6000) && (pov < 12000))         // right
         dpad.right = true;
      else if ((pov >= 12000) && (pov <= 24000))      // backward
         {
         dpad.down = true;
         if (pov <= 13500)
            dpad.right = true;
         else if (pov >= 22500)
            dpad.left = true;
         }
      else if ((pov > 24000) && (pov < 30000))        // left
         dpad.left = true;
      }
}

// **************************************************************************
// extract axis information from DirectInput info.

void MxInputAdapterGamePadDirectInput::ExtractAxis(const DIJOYSTATE2 &devState)

{
   if (RANGE_VALUE_COUNT == 0)
      return;
   int idx = 0;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lX);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lY);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lZ);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lRx);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lRy);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lRz);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.rglSlider[0]);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.rglSlider[1]);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lVX);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lVY);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lVZ);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lVRx);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lVRy);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lVRz);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.rglVSlider[0]);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.rglVSlider[1]);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lAX);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lAY);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lAZ);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lARx);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lARy);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lARz);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.rglASlider[0]);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.rglASlider[1]);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lFX);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lFY);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lFZ);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lFRx);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lFRy);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.lFRz);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.rglFSlider[0]);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
   if (++idx >= RANGE_VALUE_COUNT)
      return;
   rangeValues[idx] = GetNormalizedAxisValue(devState.rglFSlider[1]);
   if (abs(rangeValues[idx]) < rangeValueAttenuate[idx])
      rangeValues[idx] = 0;
}

// **************************************************************************
// return a normalized value based on the maximum we previously handed to
// DirectX for all axis.

double MxInputAdapterGamePadDirectInput::GetNormalizedAxisValue(LONG av)

{
   static double maxAxis = (double)MAX_AXIS_RANGE;

   return (double)av / maxAxis;
}

// osgwMx
}
