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

// TODO: into ConnexionMacOS.hpp
// TODO: all Connexion loading into ConnexionMacOS.cpp? do this if there are other 3D mouse vendors than Connexion
#define kConnexionClientModeTakeOver 1
#define kConnexionMaskAxis 0x3f00
#define kConnexionMaskAll 0x3fff
#define kConnexionMaskAllButtons 0xffffffff
#define kConnexionCmdHandleButtons 2
#define kConnexionCmdHandleAxis 3
#define kConnexionCmdAppSpecific 10
#define kConnexionMsgDeviceState '3dSR'
#define kConnexionCtlGetDeviceID '3did'

// https://forum.3dconnexion.com/viewtopic.php?t=34846
#define kConnexionClientManual        0x2B2B2B2B
#define kConnexionCtlActivateClient   '3dac'
#define kConnexionCtlDeactivateClient '3ddc'


//    -> use kConnexionXXX instead of custom enums, KISS

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
// TODO: into ConnexionMacOS.mm?

// https://stackoverflow.com/a/36964600/753850
template <typename PFN>
PFN connexion_getProcAddress(const char* fname)
{
    PFN ret = reinterpret_cast<PFN>(  ::dlsym( ndof::MacOSNDOF::ndof->m_private->m_connexion_dyld, fname ) );

    ndof::NDOF_DEBUG( std::ostringstream() << "    loaded 3Dconnexion framework function " << (void*)( ret ) << " (" << fname << ")" );

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
// it looks like these can't be implemeted as static C++ methods. maybe it has
// something to do with Objective-C
static void device_added(uint32_t unused)
{
    std::cout << "device added." << std::endl;
}

static void device_removed(uint32_t unused)
{
    std::cout << "device removed." << std::endl;
}

static void device_message(uint32_t unused, uint32_t msg_type, void *msg_arg)
{
    std::cout << "device message, type " << msg_type << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
// MacOSNDOF
// 
//    * https://www.martinmajewski.net/how2use3dconnexionwithswift/

namespace ndof
{


////////////////////////////////////////////////////////////////////////////////
// helpers


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
        NDOF_DEBUG( std::ostringstream() << "    3Dconnexion framework opened (" << (void*)( m_private->m_connexion_dyld ) << ")" );

        // load API function: SetConnexionHandlers (modern) or InstallConnexionHandlers (legacy)
        // set ConnexionAPIVersion based on availability
        CONNEXION_LOAD_FUNCTION( SetConnexionHandlers );
        if ( SetConnexionHandlers )
        {
            m_private->m_connexion_api_version = macos::ConnexionAPIVersion::MODERN; 
            log( "    3Dconnexion modern API is available" );
        }
        else
        {
            log( "    no modern 3Dconnexion client API available; trying legacy client API" );

            // try legacy API function
            if ( CONNEXION_LOAD_FUNCTION( InstallConnexionHandlers ) )
            {
                m_private->m_connexion_api_version = macos::ConnexionAPIVersion::LEGACY;
                log( "    3Dconnexion legacy API is available" );
            }
        }
        if ( macos::empty_ConnexionAPIVersion( m_private->m_connexion_api_version ) )
        {
            std::ostringstream os;
            os << "could not load 3Dconnexion client API: " << ::dlerror() << " (are system drivers installed?)";
            throw Error( os.str() );
        }
        
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

        if ( m_private->m_connexion_api_version == macos::ConnexionAPIVersion::MODERN )
        {
            constexpr bool seperate_thread = false; // FIXME: use enum!!
            //if ( uint16_t err = SetConnexionHandlers( MacOSNDOF_Private::connexion_MessageHandler, MacOSNDOF_Private::connexion_AddedHandler, MacOSNDOF_Private::connexion_RemovedHandler, seperate_thread ) )
            if ( uint16_t err = SetConnexionHandlers( device_message, device_added, device_removed, true ) )
            {
                throw Error( "could not setup client API callbacks (error code: " + std::to_string( err ) + ")" );
            }
            NDOF_DEBUG( "    3Dconnexion callbacks registered");
        }
        if ( m_private->m_connexion_api_version == macos::ConnexionAPIVersion::LEGACY )
        {
            //if ( uint16_t err = InstallConnexionHandlers( MacOSNDOF_Private::connexion_MessageHandler, MacOSNDOF_Private::connexion_AddedHandler, MacOSNDOF_Private::connexion_RemovedHandler ) ) 
            if ( uint16_t err = InstallConnexionHandlers( device_message, device_added, device_removed ) ) 
            {
                throw Error( "could not setup legacy client API callbacks (error code: " + std::to_string( err ) + ")" );
            }
            NDOF_DEBUG( "    3Dconnexion callbacks registered (legacy)");
        }

        // (manually) register LibNDOF to the 3Dconnexion system driver. then we have to activate us using 'ConnexionClientControl'
        // FIXME: can we use 'kConnexionClientManual' on legacy drivers? I guess nobody is using legacy anyway :)
        //        if not, look at previous versions of this file in repo to see an implemetation using current process name
        m_private->m_connexion_client_id = RegisterConnexionClient( kConnexionClientManual, 0, kConnexionClientModeTakeOver, kConnexionMaskAxis );
        
        // FIXME: is ClientID 0 a failure?
        if ( m_private->m_connexion_client_id == 0 )
        {
            throw Error( "API could not register client" );
        }
        NDOF_DEBUG( std::ostringstream() << "    3DConnexion client registered; ClientID is " << m_private->m_connexion_client_id  );

        // modern API needs specific call to set button mask
        if ( m_private->m_connexion_api_version == macos::ConnexionAPIVersion::MODERN )
        {
            int32_t result;
            if ( uint16_t err = ConnexionClientControl( m_private->m_connexion_client_id, kConnexionCtlActivateClient, 0, &result ) )
            {
                std::ostringstream os;
                os << "activating 3DConnexion client failed: " << err;
                throw Error( os.str() );
            }
            NDOF_DEBUG( std::ostringstream() << "    activating 3DConnexion client: " << result );

            // enable all buttons 
            SetConnexionClientButtonMask( m_private->m_connexion_client_id, macos::from_ConnexionMaskButton( macos::ConnexionMaskButton::ALL ) );

        }
        if ( m_private->m_connexion_api_version == macos::ConnexionAPIVersion::LEGACY )
        {
            int32_t result;
            if ( uint16_t err = ConnexionControl( kConnexionCtlActivateClient, 0, &result ) )
            {
                std::ostringstream os;
                os << "activating legacy 3DConnexion client failed: " << err ;
                throw Error( os.str() );
            }
            NDOF_DEBUG( std::ostringstream() << "    activating 3DConnexion client: " << result );

            // (no need for button mask on legacy driver)
        }


        log( "    3DConnexion is initialized :)" );

        m_initialized = true;
    }

}


////////////////////////////////////////////////////////////////////////////////
// MacOSNDOF::end()

void MacOSNDOF::end()
{
    if ( m_initialized )
    {
        int32_t result;
        if ( uint16_t err = ConnexionClientControl( m_private->m_connexion_client_id, kConnexionCtlActivateClient, 0, &result ) )
        {
            NDOF_DEBUG( std::ostringstream() << "    deactivate 3DConnexion client failed: " << err );
        }
        NDOF_DEBUG( std::ostringstream() << "    deactivate 3DConnexion client: " << result );


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
    MacOSNDOF* ndof = MacOSNDOF::ndof;

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
