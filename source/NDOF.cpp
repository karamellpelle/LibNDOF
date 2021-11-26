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
#include "configure.hpp"

#include <iostream>
#include <array>
#include <stdio.h>


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
/// debug

#ifdef LIBNDOF_DEBUG
//void NDOF_DEBUG(const std::string& s)
//{
//    std::cout << s << std::endl;
//}
//
//void NDOF_DEBUG(const std::ostringstream& os)
//{
//    NDOF_DEBUG( os.str() );
//}
//
//void NDOF_DEBUG(const char* fstr, ...)
//{
//   std::printf( fstr, ... );
//}
//
#endif

////////////////////////////////////////////////////////////////////////////////
/// NDOF 


NDOF::NDOF()
{
    // log to std::cout as default
    m_logger = &std::cout;
}

// set logger output stream. only makes sense outside begin()-end().
// no ownership is taken
void NDOF::logger(std::ostream* l)
{
    if ( !m_initialized ) m_logger = l;
}

std::ostream& NDOF::log() 
{
    if ( m_logger ) return *m_logger;

    // TODO: return dummy std::ostream
    return std::cout;
}

// adds newline
std::ostream& NDOF::log(const std::string& str)
{
    return log() << str << std::endl;   
}

//std::ostream& NDOF::log(const char* fmt, ...)
//{
//    // TODO: write to m_logger!
//
//    //NDOF_DEBUG(  "Warning: NDOF::log(const char* ) not implemented" );
//    //  * https://docs.microsoft.com/en-us/archive/msdn-magazine/2015/march/windows-with-c-using-printf-with-modern-c
//    va_list args;
//    va_start( args, fmt );
//    std::printf( fmt, args );
//    va_end(args);
//
//    return log();
//}
//
Connection NDOF::connect(const Connection::Ideal& ideal)
{
    std::cout << "ndof::NDOF::connect()"  << std::endl;

    // look at available devices. if there is a match between a device and the Ideal, 
    // return a Connection to that device.

    return Connection();
}


////////////////////////////////////////////////////////////////////////////////
// connexion_handle_axis
void NDOF::connexion_handle_axis(const ConnexionTranslation& trans, const ConnexionRotation& rot)
{
    NDOF_DEBUG( "NDOF::connexion_handle_axis()" );
}


////////////////////////////////////////////////////////////////////////////////
// connexion_handle_buttons

void NDOF::connexion_handle_buttons(const ConnexionButtons& buttons)
{

    NDOF_DEBUG( "NDOF::connexion_handle_buttons()" );
}


////////////////////////////////////////////////////////////////////////////////
// connexion_handle_app
void NDOF::connexion_handle_app()
{
    NDOF_DEBUG( "NDOF::connexion_handle_app()" );

}


} // namespace ndof
