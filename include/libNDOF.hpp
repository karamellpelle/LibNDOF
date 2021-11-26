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
#ifndef _LIBNDOF_LIBNDOF_HPP_
#define _LIBNDOF_LIBNDOF_HPP_
#include "LibNDOF/Connexion.hpp"
#include <queue>
#include <forward_list>
#include <vector>
#include <chrono>
#include <mutex>
#include <thread>
#include <tuple>
#include <regex>
#include <sstream>
#include <cstdarg>
#include <iostream>

#ifdef LIBNDOF_BACKEND_HDIAPI 
#include "hidapi.h"
#endif


namespace ndof 
{

////////////////////////////////////////////////////////////////////////////////
// NDOF small types

// our number type  
using uint = unsigned int;

// word types
using uint8_t  = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using uint64_t = std::uint64_t;
using int8_t   = std::int8_t;
using int16_t  = std::int16_t;
using int32_t  = std::int32_t;
using int64_t  = std::int64_t;

// our time tag type
using Time = double; // FIXME

// exeption type
using Error = std::runtime_error;

class NDOF;
class Connection;
class DeviceEvent;


// device buttons
enum class Button
{
    EMPTY,
    A,
    // etc
};


// button pressed (DOWN) or neutral (UP)?
enum class ButtonState
{
    EMPTY,
    UP,
    DOWN
};


////////////////////////////////////////////////////////////////////////////////
// DeviceEvent data

// strict change of button state
class ButtonChange
{
public:
    ButtonChange()                                                  { }
    ButtonChange(Button b, ButtonState s) : button( b ), state( s ) { }
    operator bool () const                                          { return button != Button::EMPTY; }

    Button        button = Button::EMPTY;
    ButtonState   state  = ButtonState::EMPTY;

    Time     time = 0.0; // FIXME
};

// mouse movement
class Motion
{
public:
    operator bool () const { return false; } // FIXME

    // TODO x, y, z etc

    Time     time = 0.0; // FIXME
};



////////////////////////////////////////////////////////////////////////////////
// DeviceVariant

// descrining a product variant of a 3D mouse (VendorID, ProductID)
class DeviceVariant
{
public:
    constexpr DeviceVariant(uint16_t v, uint16_t p) : DeviceVariant( v, p, 0x0000, 0x0000 ) {  }
    constexpr DeviceVariant(uint16_t v, uint16_t p, uint16_t va, uint16_t pa) : vid( v ), pid( p ), vid_alt( va ), pid_alt( pa ) {  }

    uint16_t vid = 0x0000;
    uint16_t pid = 0x0000;

    // there may be an alternative(s?), typically connecting a wireless variant with cable
    uint16_t vid_alt = 0x0000;
    uint16_t pid_alt = 0x0000;
};


////////////////////////////////////////////////////////////////////////////////
// DeviceInfo

// specific information about a physically connected device
class DeviceInfo
{
public:
    operator bool() const { return false; } // FIXME
    
    // for example
    std::string name() const;
    std::string uuid() const;
    // etc

private:
    DeviceVariant m_variant;

    Time     time = 0.0; // FIXME
};


////////////////////////////////////////////////////////////////////////////////
// DeviceEvent

// different deviceevents from a NDOF device
enum class DeviceEventType
{
    EMPTY,              // ^ the empty deviceevent
    CONNECTED,          // ^ device connected
    DISCONNECTED,       // ^ device disconnected
    MOTION,             // ^ motion
    BUTTON_CHANGE       // ^ strict change of botton state
};


// device events from device
class DeviceEvent
{
friend class NDOF;
friend class Connection;

public:
    // DeviceEvent's are unique and can only be moved, not copied
    DeviceEvent(DeviceEvent&& ) = default;
    DeviceEvent& operator=(DeviceEvent&& ) = default;
    ~DeviceEvent();

    // is this a valid event?
    operator bool() const                    { return m_type != DeviceEventType::EMPTY; }
    // is this event of given type?
    bool operator()(DeviceEventType t) const { return m_type == t; }

    // client calls this to retrieve data
    // only call this if current event is of corresponding type
    DeviceInfo    device() const;
    Motion        motion() const;
    ButtonChange  buttonchange() const;

private:
    DeviceEvent()                 : m_type( DeviceEventType::EMPTY ) {  }
    DeviceEvent(DeviceEventType t): m_type( t ) {  }
    DeviceEvent(float ,float ,float ,float ,float ,float );
    DeviceEvent(Button, ButtonState );

    // no copy construct/assign (DeviceEvent's are unique objects, only construct, move and destroy)
    DeviceEvent(const DeviceEvent& ) = delete;
    DeviceEvent& operator=(const DeviceEvent& ) = delete;

    // type of event
    DeviceEventType m_type = DeviceEventType::EMPTY;

    // event data
    union
    {
        // movement
        struct
        {
            float m_tx, m_ty, m_tz;
            float m_rx, m_ry, m_rz;
            
        };
        // button change
        struct
        {
            Button      m_button;
            ButtonState m_buttonstate;
        };
        // device info
        struct
        {
           DeviceInfo* m_deviceinfo = nullptr;
        };
    };

    Time     time = 0.0; // FIXME
};



////////////////////////////////////////////////////////////////////////////////
// Connection

class ConnectionImpl;

// interface to a potentional 3D mouse
class Connection
{
friend class NDOF;

public:
    Connection();

    // multiple connections are allowed, however, they all work on the same
    // low level connection. this makes it possible to use Connection's as a 
    // members in a class that is copied
    Connection(const Connection& ) = default;
    Connection& operator=(const Connection& ) = default;
    Connection(Connection&& ) = default;
    Connection& operator=(Connection&& ) = default;
    ~Connection() = default;

    // pull a DeviceEvent from connection (the result may be empty)
    DeviceEvent pop();
    
    //bool connected() const; 
    // ^ no such functionlity since connection status may change during calls.
    
    // TODO
    Time time() const;

public:

    // stating the ideal device for a connection
    class Ideal
    {
    public:
        Ideal();                                        // ^ any device
        Ideal(std::initializer_list<DeviceVariant> );   // ^ any device of given product variants (VendorID, ProductID). 
        Ideal(const std::regex& name);                  // ^ any device matching product name 
        Ideal(uint uuid);                               // ^ the exact device
        
        enum class Reconnect
        {
            IDEAL,         // ^ reconnect to any available device matching ideal
            DEVICE,        // ^ reconnect to same device (using UUID (or serial number?))
            NONE,          // ^ do not reconnect to any device; let connection die
        };

        Ideal& operator()(Reconnect r) { reconnect = r; return *this; }


        std::regex regex; 
        std::vector<DeviceVariant> variants;
        uint uuid = 0;

        // how to handle reconnections
        Reconnect reconnect = Reconnect::IDEAL; 
    };

private:
    // NDOF owns all connections
    Connection(NDOF* );

    std::shared_ptr<ConnectionImpl> m_impl;

    Time m_time_begin = 0.0;
    Time m_time_prev = m_time_begin;

};



////////////////////////////////////////////////////////////////////////////////
// NDOF

// NDOF: manage NDOF devices
class NDOF
{
public:
    NDOF();
    NDOF(const NDOF& ) = delete;
    NDOF& operator=(const NDOF& ) = delete;

    // logging
    void logger(std::ostream* );

    // start/stop ndof. may throw exceptions
    virtual void begin() = 0;
    virtual void end() = 0;

    // all physically connected devices 
    //std::vector<DeviceInfo> devices() const;

    // create connection to a device (existing or pending) based on given ideal.
    // returned Connection will not connect to a device if there already is a 
    // Connection to that device. the connected device may change between 
    // DISCONNECTED/CONNECTED 
    Connection connect(const Connection::Ideal& );

    //// all open connections
    //std::vector<Connection> connections() const;


    // debug: more low level details than logging
#ifdef LIBNDOF_DEBUG
    static void debug(const std::string& s)         { std::cout << s << std::endl; }
    static void debug(const std::ostringstream& os) { NDOF::debug( os.str() ); }
    //static void debug(const char* fmt, ...)         { va_list args; va_start( args, fmt ); std::printf( fmt, args ); va_end( args  ); }
#define NDOF_DEBUG(arg) NDOF::debug( (arg) )
#else
    static void debug(const std::string& ) {  }
    static void debug(const std::ostringstream& ) {  }
    //static void debug(const char* , ...) {  }
#define NDOF_DEBUG(arg)
// ^FIXME: no definition OK?
#endif
    


protected:
    std::ostream* m_logger = nullptr;
    bool m_initialized = false;

    std::ostream& log();
    std::ostream& log(const std::string& );
    //std::ostream& log(const char* fmt, ...);


    std::forward_list<std::shared_ptr<Connection>> m_connections;

    // worker thread (HID)
    std::thread m_hid_thread;
    void run_hid();
   
    std::queue<DeviceEvent> m_eventqueue; // or std::priority_queue?
    mutable std::mutex m_mutex_eventqueue;
    
    bool push_hid_data(/* args */ );
    void push_deviceevent(const DeviceEvent& );

    // 3DConnexion driver input/output
    void connexion_handle_axis(const ConnexionTranslation&, const ConnexionRotation& );
    void connexion_handle_buttons(const ConnexionButtons& );
    void connexion_handle_app();
};


} // namespace ndof


#endif // _LIBNDOF_LIBNDOF_HPP_
