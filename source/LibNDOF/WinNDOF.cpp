// Copyright (C) 2021  karamellpelle@hotmail.com
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
// USA
#include "libNDOF.hpp"

//  # TODO
//  * dummy window class for custom WinProc ?
//  * log windows error: https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
//  * "The application must call DefWindowProc so the system can perform cleanup." - https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-input 

bool NDOF::begin()
{
    //  
    // "RegisterRawInputDevices should not be used from a library" 
    //
    //        - Microsoft (https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerrawinputdevices)
    //
    //


    // about RawInput:  https://docs.microsoft.com/en-us/windows/win32/inputdev/about-raw-input
    // example:         https://docs.microsoft.com/en-us/windows/win32/inputdev/using-raw-input

    // declare input devices to look for
    // RAWINPUTDEVICE:         https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawinputdevice
    // HID usage and pages:    https://www.usb.org/sites/default/files/hut1_22.pdf
    // HID clients supported:  https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/hid-architecture#hid-clients-supported-in-windows
    RAWINPUTDEVICE rawinputs[ 1 ];
    
    // register 3Dconnexion HID mouse
    rawinputs[0].usUsagePage = 0x01;
    rawinputs[0].usUsage     = 0x08;
    rawinputs[0].dwFlags     = RIDEV_DEVNOTIFY | RIDEV_NOLEGACY; // FIXME: nolegacy necessary?
    rawinputs[0].hwndTarget  = 0; // 0 => follow keyboard focus

    // RegisterRawInputDevices(): https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerrawinputdevices
    // a user of this library is probably not using this UsagePage+Usage.
    if ( ! ::RegisterRawInputDevices( rawinputs, 2, sizeof( RAWINPUTDEVICE ) ) )
    {
        //registration failed. Call GetLastError for the cause of the error
        std::cerr << "registering RawInput device failed!\n";
        return false;
    }
    
    // TODO

    return true;
}


// TODO: catch allocation error, etc
// question: can we create a dummy window class in `NDOF::begin()` with custom WindowProc 
//           so the user don't need to inject these events?
bool NDOF::win32_inject(UINT msg, WPARAM wp, LPARAM lp)
{
    // example: https://docs.microsoft.com/en-us/windows/win32/inputdev/using-raw-input

    if ( msg == WM_INPUT ) 
    {
        // WM_INPUT: https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-input
        auto code = GET_RAWINPUT_CODE_WPARAM( wp );   // RIM_INPUT (normal) or RIM_INPUTSINK (buffered)
        auto rawinput_h = static_cast<HRAWINPUT>( lp );
        if ( code == RIM_INPUT )
        {
            // retrieve RawInput data
            // RAWINPUT:          https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawinput
            // GetRawInputData(): https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getrawinputdata
            RAWINPUT rawinput;
            PUINT rawinput_size = sizeof( rawinput );
            if ( ::GetRawInputData( rawinput_h, RID_INPUT, &rawinput, &rawinput_size, sizeof( RAWINPUTHEADER ) ) == rawinput_size )
            {
                // inspect header
                // RAWINPUTHEADER: https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawinputheader
                auto type = rawinput.header.dwType;
                HANDLE device = rawinput.header.hDevice;

                if ( type == RIM_TYPEHID )
                {
                    // RAWHID: https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawhid
                    uint8_t[] buf    = static_cast<uint8_t[]>( raw.data.hid.bRawData );
                    std::size_t size = raw.data.hid.dwSizeHid;
                    
                    // TODO: let NDOF handle 'buf' and 'size' for 'device'

                    return true;
                }
            }
            else
            {
                std::err << "GetRawInputData does not return correct size !\n"; // FIXME: better error report
            }

        }
    }

    if ( msg == WM_INPUT_DEVICE_CHANGE )
    {
        // WM_INPUT_DEVICE_CHANGE: https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-input-device-change
        auto change = wp; // GIDC_ARRIVAL | GIDC_REMOVAL 
        HANDLE device = static_cast<HANDLE>( wp );

        // TODO: handle device add/remove 
    }

    return false;
}

