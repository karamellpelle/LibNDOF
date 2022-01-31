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
#ifndef _LIBNDOF_CONNEXIONMACOS_HPP_
#define _LIBNDOF_CONNEXIONMACOS_HPP_
#include <cstdint>
#include "helpers.hpp"



////////////////////////////////////////////////////////////////////////////////
// API function declaration

// callback types
using ConnexionAddedHandlerProc   = void (*)(unsigned int productID);
using ConnexionRemovedHandlerProc = void (*)(unsigned int productID);
using ConnexionMessageHandlerProc = void (*)(unsigned int productID, unsigned int messageT, void* messageArg );

// setup connexion driver (install callbacks)
using PFN_SetConnexionHandlers     = int16_t (*)(ConnexionMessageHandlerProc , ConnexionAddedHandlerProc , ConnexionRemovedHandlerProc , bool seperate_tread );
using PFN_CleanupConnexionHandlers = void (*)(void);
using PFN_InstallConnexionHandlers = int16_t (*)(ConnexionMessageHandlerProc , ConnexionAddedHandlerProc, ConnexionRemovedHandlerProc ); // legacy

// start stop receiving data
using PFN_RegisterConnexionClient = uint16_t (*)(uint32_t signature, uint8_t* name, uint16_t mode, uint32_t mask );
using PFN_UnregisterConnexionClient = void (*)(uint16_t clientID );
using PFN_SetConnexionClientMask = void (*)(uint16_t clientID, uint32_t mask);        // ^ defines which commands to send to us (clientID)
using PFN_SetConnexionClientButtonMask = void (*)(uint16_t clientID, uint32_t mask);  // ^ defines which buttons that are sent to us (clientID)

// send control messages to device
using PFN_ConnexionClientControl = int16_t (*)(uint16_t clientID, uint32_t message, int32_t param, int32_t* result );
using PFN_ConnexionControl = int16_t (*)(uint32_t message, int32_t param, int32_t* result ); // legacy

// get preferences of device (which one?)
class ConnexionDevicePrefs;
using PFN_ConnexionGetCurrentDevicePrefs = int16_t (*)(uint32_t deviceID, ConnexionDevicePrefs* prefs );

// set labels using 32 UTF-8 c strings (null terminated)
using PFN_ConnexionSetButtonLabels = int16_t (*)(uint8_t* labels, uint16_t size );


////////////////////////////////////////////////////////////////////////////////
// 3Dconnexion drivers
//
//    * see /Library/Frameworks/3DconnexionClient.framework/Headers/ConnexionClient.h|ConnexionClientAPI.h (MacOS)h

namespace ndof
{
namespace macos
{

////////////////////////////////////////////////////////////////////////////////
// ConnexionAPIVersion

enum class ConnexionAPIVersion
{
    EMPTY,
    LEGACY,   // there may be different legacy versions
    MODERN,   // this may become outdated, but this type is internal and not public exposed 
};

inline bool empty_ConnexionAPIVersion(ConnexionAPIVersion e)
{
    return e == ConnexionAPIVersion::EMPTY;
}

////////////////////////////////////////////////////////////////////////////////
// ConnexionMsg

enum class ConnexionMsg : unsigned int 
{
    EMPTY            = 0,
    DEVICE_STATE     = '3dSR',
    PREFS_CHANGED    = '3dPC',
    CALIBRATE_DEVICE = '3dSC',
};

inline unsigned int from_ConnexionMsg(ConnexionMsg e)
{
    return static_cast<uint32_t>( e );
}

inline ConnexionMsg to_ConnexionMsg(unsigned int e)
{
    switch (e)
    {
    case 3: return ConnexionMsg::DEVICE_STATE;
    case 2: return ConnexionMsg::PREFS_CHANGED;
    case 10:return ConnexionMsg::CALIBRATE_DEVICE;
    };

    return ConnexionMsg::EMPTY;
}


////////////////////////////////////////////////////////////////////////////////
// ConnexionCtl

enum class ConnexionCtl : uint32_t 
{
    EMPTY             = 0,
    SET_LED_STATE     = '3dsl',
    GET_DEVICE_ID     = '3did',
    CALIBRATE         = '3dca',
    UNCALIBRATE       = '3dde',
    OPEN_PREF_PANE    = '3dop',
    SET_SWITCHES      = '3dss',
    ACTIVATE_CLIENT   = '3dac',
    DEACTIVATE_CLIENT = '3ddc',
};

inline uint32_t from_ConnexionCtl(ConnexionCtl e)
{
    return static_cast<uint32_t>( e );
}

////////////////////////////////////////////////////////////////////////////////
// ConnexionCmd

enum class ConnexionCmd : uint16_t
{
    EMPTY           = 0,  // kConnexionCmdNone
    HANDLE_RAW_DATA = 1,  // kConnexionCmdHandleRawData
    HANDLE_BUTTONS  = 2,  // kConnexionCmdHandleButtons
    HANDLE_AXIS     = 3,  // kConnexionCmdHandleAxis
    APP_SPECIFIC    = 10, // kConnexionCmdAppSpecific
};

inline uint16_t from_ConnexionCmd(ConnexionCmd e)
{
    return static_cast<uint16_t>( e );
}

// TODO: constexpr for C++14 and above (allows if and switch)
inline ConnexionCmd to_ConnexionCmd(uint16_t e)
{
    switch (e)
    {
    case 3: return ConnexionCmd::HANDLE_AXIS;
    case 2: return ConnexionCmd::HANDLE_BUTTONS;
    case 10:return ConnexionCmd::APP_SPECIFIC;
    };

    return ConnexionCmd::EMPTY;
}


////////////////////////////////////////////////////////////////////////////////
// 
enum class ConnexionClientMode : uint16_t
{
    TAKE_OVER = 1,  // take over device completely, driver no longer executes assignments
    PLUGIN    = 2,  // receive plugin assignments only, let driver take care of its own
};

inline uint16_t from_ConnexionClientMode(ConnexionClientMode e)
{
    return static_cast<uint16_t>( e );
}

////////////////////////////////////////////////////////////////////////////////
// 

enum class ConnexionMask : uint32_t
{
    EMPTY      = 0x0000,

    // trans
    AXIS1      = 0x0100,
    AXIS2      = 0x0200,
    AXIS3      = 0x0400,
    AXIS_TRANS = 0x0700,
    // rot
    AXIS4      = 0x0800,
    AXIS5      = 0x1000,
    AXIS6      = 0x2000,
    AXIS_ROT   = 0x3800,

    AXIS       = 0x3F00,

    ALL        = 0x3FFF,

    // these are legacy; use SetConnexionClientButtonMask with the modern API:
    BUTTON1    = 0x0001,
    BUTTON2    = 0x0002,
    BUTTON3    = 0x0004,
    BUTTON4    = 0x0008,
    BUTTON5    = 0x0010,
    BUTTON6    = 0x0020,
    BUTTON7    = 0x0040,
    BUTTON8    = 0x0080,
    BUTTONS    = 0x00FF,

};

inline uint32_t from_ConnexionMask(ConnexionMask e)
{
    return static_cast<uint32_t>( e );
}


////////////////////////////////////////////////////////////////////////////////
// ConnexionMaskButton
enum class ConnexionMaskButton : uint32_t
{
    EMPTY    = 0x00000000,

    BUTTON1  = 0x00000001,
    BUTTON2  = 0x00000002,
    BUTTON3  = 0x00000004,
    BUTTON4  = 0x00000008,
    BUTTON5  = 0x00000010,
    BUTTON6  = 0x00000020,
    BUTTON7  = 0x00000040,
    BUTTON8  = 0x00000080,
    BUTTON9  = 0x00000100,
    BUTTON10 = 0x00000200,
    BUTTON11 = 0x00000400,
    BUTTON12 = 0x00000800,
    BUTTON13 = 0x00001000,
    BUTTON14 = 0x00002000,
    BUTTON15 = 0x00004000,
    BUTTON16 = 0x00008000,
    BUTTON17 = 0x00010000,
    BUTTON18 = 0x00020000,
    BUTTON19 = 0x00040000,
    BUTTON20 = 0x00080000,
    BUTTON21 = 0x00100000,
    BUTTON22 = 0x00200000,
    BUTTON23 = 0x00400000,
    BUTTON24 = 0x00800000,
    BUTTON25 = 0x01000000,
    BUTTON26 = 0x02000000,
    BUTTON27 = 0x04000000,
    BUTTON28 = 0x08000000,
    BUTTON29 = 0x10000000,
    BUTTON30 = 0x20000000,
    BUTTON31 = 0x40000000,
    BUTTON32 = 0x80000000,

    ALL      = 0xFFFFFFFF,
};

inline uint32_t from_ConnexionMaskButton(ConnexionMaskButton e)
{
    return static_cast<uint32_t>( e );
}


////////////////////////////////////////////////////////////////////////////////
// ConnexionDeviceState

// TODO: define
//#define kConnexionDeviceStateType 0x4D53		// 'MS' (Connexion State)
//#define kConnexionDeviceStateVers 0x6D33		// 'm3' (version 3 includes 32-bit button data in previously unused field, binary compatible with version 2)

// pack structure for raw read of 3DConnexion data
// #pragma pack supported by
//    * MVSC
//    * GCC (https://gcc.gnu.org/onlinedocs/gcc/Structure-Layout-Pragmas.html)
//    * Clang (tested)
#pragma pack( push, 1 )
struct ConnexionDeviceState
{
    uint16_t version;       //                          // 2
    uint16_t client;        //                          // 2
    uint16_t command;       //                          // 2
    int16_t  parameter;     //                          // 2
    int32_t  value;         //                          // 4
    uint64_t time;          //                          // 8
    uint8_t  report[8];     //                          // 8 == 8 * 1
    uint16_t buttons_old;   // old drivers compability  // 2
    int16_t  axis[6];       //                          // 12 == 6 * 2
    uint16_t address;       // device id                // 2                    
    uint32_t buttons_new;   //                          // 4
                                                        // => sum should be 48
};
#pragma pack( pop )

// create ConnexionButtons from macOS client API
template <typename T>
ConnexionButtons to_ConnexionButtons(const T& b)
{
    return ConnexionButtons( static_cast<uint32_t>( b ) );
}

} // namespace macos
} // namespace ndof


////////////////////////////////////////////////////////////////////////////////
// class template specializations must occur at global scope

// instance EnableBitMaskOperators ConnexionMask where
INSTANCE_BITMASK( ndof::macos::ConnexionMask );

// instance EnableBitMaskOperators ConnexionMaskButton where
INSTANCE_BITMASK( ndof::macos::ConnexionMaskButton );


#endif

