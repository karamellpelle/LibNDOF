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

#include <iostream>
#include <array>

#include "libNDOF.hpp"
#include "values.hpp"
#include "configure.hpp"


namespace ndof
{


////////////////////////////////////////////////////////////////////////////////
// USB vendor and product IDs

// 3dconnexion Vendor IDs (Logitec, Inc has acquired 3Dconnexion)
// see http://www.linux-usb.org/usb.ids
static constexpr std::array<uint16_t, 3> usb_vids =
    {
        0x046d, // Logitech, Inc.
        0x256f, // 3dconnexion
    };

// 3dconnexion Product IDs
// see http://www.linux-usb.org/usb.ids
static constexpr std::array<uint16_t, 18> usb_pids =
    {
        // Logitech, Inc.
        0xc603,  // 3Dconnexion Spacemouse Plus XT
        0xc605,  // 3Dconnexion CADman
        0xc606,  // 3Dconnexion Spacemouse Classic
        0xc621,  // 3Dconnexion Spaceball 5000
        0xc623,  // 3Dconnexion Space Traveller 3D Mouse
        0xc625,  // 3Dconnexion Space Pilot 3D Mouse
        0xc626,  // 3Dconnexion Space Navigator 3D Mouse
        0xc627,  // 3Dconnexion Space Explorer 3D Mouse
        0xc628,  // 3Dconnexion Space Navigator for Notebooks
        0xc629,  // 3Dconnexion SpacePilot Pro 3D Mouse
        0xc62b,  // 3Dconnexion Space Mouse Pro
        // 3Dconnexion
        0xc62e,  // SpaceMouse Wireless (cabled)
        0xc62f,  // SpaceMouse Wireless Receiver
        0xc631,  // SpaceMouse Pro Wireless (cabled)
        0xc632,  // SpaceMouse Pro Wireless Receiver
        0xc633,  // SpaceMouse Enterprise
        0xc635,  // SpaceMouse Compact
        //0xc652,  // Universal Receiver

    };


////////////////////////////////////////////////////////////////////////////////
/// NDOF

void NDOF::begin()
{
    std::cout << "ndof::NDOF::begin()"  << std::endl
              << "  version major: " << LIBNDOF_VERSION_MAJOR << std::endl
              << "  version minor: " << LIBNDOF_VERSION_MINOR << std::endl
              << std::endl;

    // TODO: setup hidapi, launch thread
    // m_eventqueue.emplace<DeviceEvent>( args );
}


Connection NDOF::connect(const Connection::Ideal& ideal)
{
    std::cout << "ndof::NDOF::connect()"  << std::endl;

    // look at available devices. if there is a match between a device and the Ideal, 
    // return a Connection to that device.

    return Connection();

}


} // namespace ndof
