#ifndef _LIBNDOF_LIBNDOF_HPP_
#define _LIBNDOF_LIBNDOF_HPP_
#include <queue>
#include <list>
#include <vector>
#include <chrono>
#include <mutex>
#include <thread>

#ifdef LIBNDOF_BACKEND_HDIAPI 
#include "hidapi.h"
#endif


namespace ndof 
{

////////////////////////////////////////////////////////////////////////////////
// NDOF types

// our number type  
using uint = unsigned int;

#ifdef LIBNDOF_TIMETAG
// our time tag type
using Time = double; // FIXME
#endif

class NDOF;
class Connection;
class DeviceEvent;


// device buttons
enum class Button
{
    // used internally, never sent
    EMPTY,
    A,
    // etc
};


// button press (DOWN) or neutral (UP)?
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


#ifdef LIBNDOF_TIMETAG
    Time     time = 0.0; // FIXME
#endif
};

// mouse movement
class Motion
{
public:
    operator bool () const { return false; } // FIXME

    // TODO x, y, z etc

#ifdef LIBNDOF_TIMETAG
    Time     time = 0.0; // FIXME
#endif
};

// device information
class DeviceInfo
{
public:
    operator bool() const { return ; }
    
    // for example
    std::string name() const;
    std::string id() const;
    // etc

private:
    std::string m_name;
    std::string m_id;

#ifdef LIBNDOF_TIMETAG
    Time     time = 0.0; // FIXME
#endif
};


////////////////////////////////////////////////////////////////////////////////
// DeviceEvent

// different deviceevents from a NDOF device
enum class DeviceEventType
{
    EMPTY,              // ^ the empty deviceevent
    CONNECT,            // ^ device connected
    DISCONNECT,         // ^ device disconnected
    MOTION,             // ^ motion
    BUTTON_CHANGE       // ^ strict change of botton state
};


// device events from device
class DeviceEvent
{
friend class NDOF;
friend class Connection;

public:
    DeviceEvent(const DeviceEvent& ) = delete;
    DeviceEvent& operator=(const DeviceEvent& ) = delete;
    DeviceEvent(DeviceEvent&& ) = default;
    DeviceEvent& operator=(DeviceEvent&& ) = default;

    bool operator()(DeviceEventType t) const { return m_type == t; }

    // client calls this to retrieve data
    // only call this if current event is of corresponding type
    DeviceInfo    device() const;
    Motion        motion() const;
    ButtonChange  buttonchange() const;

private:
    DeviceEvent(DeviceEventType t) : m_type( t ) {  }

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
            ButtonState m_state;
        };
        //// device info
        //struct
        //{
        //    std::unique_ptr<DeviceInfo> m_deviceinfo;
        //};
    };
    std::unique_ptr<DeviceInfo> m_deviceinfo; 
    // ^ TODO: into union to save memory, or more general:
    //  * std::unique_ptr<void> m_data; 
    //  * std::unique_ptr<DeviceEventData> m_data; // DeviceEventData is virtual subclass

#ifdef LIBNDOF_TIMETAG
    Time     time = 0.0; // FIXME
#endif
};


////////////////////////////////////////////////////////////////////////////////
// Connection

class ConnectionImpl;

// Device: a connection to a 3D mouse 
class Connection
{
friend class NDOF;

public:
    // multiple connections are allowed, however, they all work on the same
    // low level connection. this makes it possible to use Connection's as a 
    // members in a class that is copied
    Connection(const Connection& ) = default;
    Connection& operator=(const Connection& ) = default;
    Connection(Connection&& ) = default;
    Connection& operator=(Connection&& ) = default;
    ~Connection() = default;

    // pull DeviceEvent from connection, if any
    DeviceEvent pop_event();
    
    //bool connected() const; 
    // ^ no such functionlity since connection status may change during calls.

private:
    // NDOF owns devices
    Connection(NDOF& );
    std::shared_ptr<ConnectionImpl> m_impl;
};



////////////////////////////////////////////////////////////////////////////////
// NDOF

// NDOF: manage NDOF devices
class NDOF
{
public:
    NDOF(const NDOF& ) = delete;
    NDOF& operator=(const NDOF& ) = delete;

    // start ndof
    void begin();
    void end();

    // list of connected devices. 
    std::vector<DeviceInfo> devices() const;

    // create connection to a device (pending or existing).
    // returned Connection will not connect to a device if there
    // already is a Connection to that device. this way we can 
    // connect to multiple devices. the connected device may however 
    // change between DEVICE_DISCONNECTED/DEVICE_CONNECTED unless
    // care has been taken (like UUID, etc, which is not implemented atm)
    Connection connect();                     // ^ automatically connect to any available device. 
    Connection connect(uint16_t , uint16_t ); // ^ connect to device with specified VID, PID
    Connection connect(const std::string );   // ^ connect to device having name (?)

private:
    std::list<std::shared_ptr<Connection>> m_connections;

    // worker thread (HID)
    std::thread m_hid_thread;
    void run_hid();
    
    bool push_hid_data(/* args */ );
    void push_deviceevent(const DeviceEvent& );

};


} // namespace ndof


#endif // _LIBNDOF_LIBNDOF_HPP_
