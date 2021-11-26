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
#include "LibNDOF/MacOSNDOF.hpp"
#include "LibNDOF/ConnexionMacOS.hpp"
#include <iostream>
#include <string>
#include <cstdint>
#include <dlfcn.h>
#include <unistd.h>
#include <libproc.h>


////////////////////////////////////////////////////////////////////////////////
// MacOSNDOF_Private: private data so we don't expose everything, especially ConnexionMacOS.hpp

namespace ndof
{

class MacOSNDOF_Private
{
public:
    // 3Dconnexion framework
    void* m_connexion_dyld                             = nullptr;
    macos::ConnexionAPIVersion m_connexion_api_version = macos::ConnexionAPIVersion::EMPTY;
    // 3Dconnexion clientID (connexion to 3Dconnexion driver)
    uint16_t m_connexion_client_id                     = 0;

    // callbacks for 3Dconnexion API framework
    static void connexion_MessageHandler(unsigned int productID, unsigned int messageT , void* messageArg);
    static void connexion_AddedHandler(unsigned int productID );
    static void connexion_RemovedHandler(unsigned int productID );
};

} // namespace NDOF


////////////////////////////////////////////////////////////////////////////////
// framework API functions
// FIXME: private symbol exposure?
// FIXME: extern C?
// FIXME: into namespace ndof::macos

// https://stackoverflow.com/a/36964600/753850
template <typename PFN>
PFN connexion_getProcAddress(const char* fname)
{
    PFN ret = reinterpret_cast<PFN>(  ::dlsym( ndof::MacOSNDOF::ndof->m_private->m_connexion_dyld, fname ) );
#ifdef LIBNDOF_DEBUG 
    //ndof::NDOF_DEBUG( "    loaded 3Dconnexion framework function %32s(): %08x\n", fname, ret ); // FIXME
    ndof::NDOF_DEBUG( std::ostringstream() << "    loaded 3Dconnexion framework function " << (void*)( ret ) << " (" << fname << ")" );
#endif

    return ret;
}

// create pointer to API function
#define CONNEXION_FUNCTION_PTR(name) PFN_##name name = nullptr

// load api function into function pointer
#define CONNEXION_LOAD_FUNCTION(name) ( name = connexion_getProcAddress< PFN_##name >( #name ) )

// create function pointers to 3Dconnexion framework functions
CONNEXION_FUNCTION_PTR( SetConnexionHandlers );         // modern
CONNEXION_FUNCTION_PTR( InstallConnexionHandlers );     // legacy
CONNEXION_FUNCTION_PTR( CleanupConnexionHandlers );
CONNEXION_FUNCTION_PTR( RegisterConnexionClient );
CONNEXION_FUNCTION_PTR( UnregisterConnexionClient );
CONNEXION_FUNCTION_PTR( SetConnexionClientMask );
CONNEXION_FUNCTION_PTR( SetConnexionClientButtonMask ); // modern
CONNEXION_FUNCTION_PTR( ConnexionClientControl );
CONNEXION_FUNCTION_PTR( ConnexionControl );             // legacy
CONNEXION_FUNCTION_PTR( ConnexionGetCurrentDevicePrefs );
CONNEXION_FUNCTION_PTR( ConnexionSetButtonLabels );


////////////////////////////////////////////////////////////////////////////////
// MacOSNDOF
// 
//    * https://www.martinmajewski.net/how2use3dconnexionwithswift/

namespace ndof
{


////////////////////////////////////////////////////////////////////////////////
// helpers

// return name of current process (executable name) as a pascal string
//
//    *            https://developer.apple.com/forums/thread/30522
//    * pip_t:     https://opensource.apple.com/source/xnu/xnu-201/bsd/sys/types.h.auto.html 
//    * libproc.h: https://opensource.apple.com/source/xnu/xnu-2422.1.72/libsyscall/wrappers/libproc/libproc.h.auto.html 
// 
std::vector<uint8_t> get_proc_pstr()
{
    // since return data is a pascal string (first byte is length), 
    // the executable name cannot be longer than 256 (in fact maximumn 255)
    std::vector<uint8_t> ret( 256 );

    // retrieve name of current process
    int pid = static_cast<int>( ::getpid() ); // pid_t (int32 according to sys/types.h) -> int
    std::uint8_t* buf = ret.data() + 1;
    std::size_t size  = sizeof(uint8_t) * ( 256 - 1 ); // should be 255
    int len = proc_name( pid, buf, size );
    if ( len < -1 )   throw Error( "could not retrieve executable name using 'proc_name()'" );
    if ( 256 <= len ) throw Error( "executable name stricly larger than 255 returned from 'proc_name()'" ); // I guess this should not happen

    // write length to first element
    ret[0] = static_cast<uint8_t>( len );

    return ret;
}


////////////////////////////////////////////////////////////////////////////////
// MacOSNDOF::begin()

MacOSNDOF::MacOSNDOF() : m_private( new MacOSNDOF_Private() )
{
    
}

MacOSNDOF::~MacOSNDOF() = default;


void MacOSNDOF::begin()
{

    MacOSNDOF::ndof = this;
    
    if ( !m_initialized )
    {
        log( "initializing MacOSNDOF:" );

        // retrieve vid pid
        // set button mask

        static constexpr const char* connexion_dyld_path = "/Library/Frameworks/3DconnexionClient.framework/3DconnexionClient";
       
        log() << "    opening framework '" << connexion_dyld_path << "'" << std::endl;
        m_private->m_connexion_dyld = ::dlopen( connexion_dyld_path, RTLD_LAZY | RTLD_LOCAL );
        if ( !m_private->m_connexion_dyld ) throw Error( "could not load 3Dconnexion client (are system drivers installed?)" );

        // load API function: SetConnexionHandlers (modern) or InstallConnexionHandlers (legacy)
        // set ConnexionAPIVersion based on availability
        CONNEXION_LOAD_FUNCTION( SetConnexionHandlers );
        if ( SetConnexionHandlers )
        {
            m_private->m_connexion_api_version = macos::ConnexionAPIVersion::MODERN; 
            log( "    3Dconnexion client API available" );
        }
        else
        {
            log( "    no modern 3Dconnexion client API available; trying legacy client API" );

            // try legacy API function
            if ( CONNEXION_LOAD_FUNCTION( InstallConnexionHandlers ) )
            {
                m_private->m_connexion_api_version = macos::ConnexionAPIVersion::LEGACY;
            }
        }
        if ( macos::empty_ConnexionAPIVersion( m_private->m_connexion_api_version ) ) throw Error( "could not load 3Dconnexion client API (are system drivers installed?)" );
        
        // load the rest of API functions
        CONNEXION_LOAD_FUNCTION( CleanupConnexionHandlers );
        CONNEXION_LOAD_FUNCTION( RegisterConnexionClient );
        CONNEXION_LOAD_FUNCTION( UnregisterConnexionClient );
        //CONNEXION_LOAD_FUNCTION( SetConnexionClientMask ); 
        CONNEXION_LOAD_FUNCTION( SetConnexionClientButtonMask ); // modern
        CONNEXION_LOAD_FUNCTION( ConnexionClientControl );
        //CONNEXION_LOAD_FUNCTION( ConnexionControl ); // legacy
        //CONNEXION_LOAD_FUNCTION( ConnexionGetCurrentDevicePrefs ); 
        //CONNEXION_LOAD_FUNCTION( ConnexionSetButtonLabels ); 

        uint16_t err = 0;
        if ( m_private->m_connexion_api_version == macos::ConnexionAPIVersion::MODERN )
        {
            constexpr bool seperate_thread = false; // FIXME!
            if ( uint16_t err = SetConnexionHandlers( MacOSNDOF_Private::connexion_MessageHandler, MacOSNDOF_Private::connexion_AddedHandler, MacOSNDOF_Private::connexion_RemovedHandler, seperate_thread ) )
            {
                throw Error( "could not setup client API callbacks (error code: " + std::to_string( err ) + ")" );
            }
        }
        if ( m_private->m_connexion_api_version == macos::ConnexionAPIVersion::LEGACY )
        {
            if ( uint16_t err = InstallConnexionHandlers( MacOSNDOF_Private::connexion_MessageHandler, MacOSNDOF_Private::connexion_AddedHandler, MacOSNDOF_Private::connexion_RemovedHandler ) ) 
            {
                throw Error( "could not setup legacy client API callbacks (error code: " + std::to_string( err ) + ")" );
            }
        }

        // register LibNDOF to the 3Dconnexion system driver.
        // 'RegisterConnexionClient()' needs either a CFBundleSignature or the process name as a pascal string.
        //
        // FIXME: can we instead use 'kConnexionClientManual'? see ConnexionClient.h in framework
        // 
        //    * https://stackoverflow.com/questions/1875912/naming-convention-for-cfbundlesignature-and-cfbundleidentifier
        // 
        auto name_pstr = get_proc_pstr(); // ^ current process name using pid
        m_private->m_connexion_client_id = RegisterConnexionClient( 0, name_pstr.data(), macos::from_ConnexionClientMode( macos::ConnexionClientMode::TAKE_OVER ), macos::from_ConnexionMask( macos::ConnexionMask::ALL ) );

        // FIXME: is ClientID 0 a failure?
        if ( m_private->m_connexion_client_id == 0 )
        {
            throw Error( "API could not register client" ); // TODO: print pascal string
        }

        // modern API needs specific call to set button mask
        if ( m_private->m_connexion_api_version == macos::ConnexionAPIVersion::MODERN )
        {
            SetConnexionClientButtonMask( m_private->m_connexion_client_id, macos::from_ConnexionMaskButton( macos::ConnexionMaskButton::ALL ) );
        }

        log( "    initialized." );

        m_initialized = true;
    }

}


////////////////////////////////////////////////////////////////////////////////
// MacOSNDOF::end()

void MacOSNDOF::end()
{
    if ( m_initialized )
    {
        // unregister client
        UnregisterConnexionClient( m_private->m_connexion_client_id );
        m_private->m_connexion_client_id = 0;

        // remove callbacks
        CleanupConnexionHandlers();

        // close framework
        ::dlclose( m_private->m_connexion_dyld );
        m_private->m_connexion_dyld = nullptr;


        m_initialized = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// connexion handlers

void MacOSNDOF_Private::connexion_MessageHandler(unsigned int productID, unsigned int msg, void* msg_arg)
{
    MacOSNDOF* ndof                 = MacOSNDOF::ndof;

    switch ( macos::to_ConnexionMsg( msg ) )
    {
    // device state changed
    case macos::ConnexionMsg::DEVICE_STATE:
    {
        auto* state = static_cast<macos::ConnexionDeviceState*>( msg_arg );

        switch ( macos::to_ConnexionCmd( state->command ) )
        {
            case macos::ConnexionCmd::HANDLE_AXIS:
            {
                ConnexionTranslation translate( state->axis[0], state->axis[1], state->axis[2] );
                ConnexionRotation rotate( state->axis[3], state->axis[4], state->axis[5] );

                ndof->connexion_handle_axis( translate, rotate );
            }
            break;

            case macos::ConnexionCmd::HANDLE_BUTTONS:
            {
                if ( ndof->m_private->m_connexion_api_version == macos::ConnexionAPIVersion::LEGACY ) 
                {
                    MacOSNDOF::ndof->connexion_handle_buttons( macos::to_ConnexionButtons( state->buttons_old ) ); // buttons_old :: uint8_t
                }
                if ( ndof->m_private->m_connexion_api_version == macos::ConnexionAPIVersion::MODERN )
                {
                    MacOSNDOF::ndof->connexion_handle_buttons( macos::to_ConnexionButtons( state->buttons_new ) );          // buttons_new :: uint32_t
                }
            }
            break;

            case macos::ConnexionCmd::APP_SPECIFIC:
            {
                NDOF_DEBUG( std::ostringstream() << "ConnexionMessageHandler: ConnexionCmd::APP_SPECIFIC" );
            }
            break;
            
            default:
            {
                NDOF_DEBUG( std::ostringstream() << "ConnexionMessageHandler: unknown ConnexionCmd: " << state->command );
            }
            break;
        }
    
    }
    break;

    case macos::ConnexionMsg::PREFS_CHANGED:
    {
        NDOF_DEBUG( "ConnexionMessageHandler: ConnexionMsg::PREFS_CHANGED" );
    }
    break;

    case macos::ConnexionMsg::CALIBRATE_DEVICE:
    {
        NDOF_DEBUG( "ConnexionMessageHandler: ConnexionMsg::CALIBRATE_DEVICE" );
    }
    break;

    default:
        NDOF_DEBUG( std::ostringstream() << "ConnexionMessageHandler: unknown ConnexionMsg: " << msg );
    }
};

void MacOSNDOF_Private::connexion_AddedHandler(unsigned int productID)
{
    NDOF_DEBUG( "MacOSNDOF::connexion_AddedHandler()");
}

void MacOSNDOF_Private::connexion_RemovedHandler(unsigned int productID)
{
    NDOF_DEBUG( "MacOSNDOF::connexion_RemovedHandler()");
}


////////////////////////////////////////////////////////////////////////////////
// 

MacOSNDOF* MacOSNDOF::ndof = nullptr;


} // namespace ndof
