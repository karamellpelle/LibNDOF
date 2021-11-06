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

#include "libNDOF.hpp"
#include "configure.hpp"



namespace ndof
{


////////////////////////////////////////////////////////////////////////////////
// DeviceInfo

std::string DeviceInfo::name() const
{
    return "";
}


////////////////////////////////////////////////////////////////////////////////
// DeviceEvent

DeviceEvent::DeviceEvent(float tx, float ty, float tz, float rx, float ry, float rz) 
    : m_type( DeviceEventType::MOTION  ), 
      m_tx( tx ), m_ty( ty ), m_tz( tz ), m_rx( rx ), m_ry( ry ), m_rz( rz )
{

}

DeviceEvent::DeviceEvent(Button b, ButtonState s)
    : m_type( DeviceEventType::BUTTON_CHANGE ),
      m_button( b ), m_buttonstate( s )
{

}

DeviceEvent::~DeviceEvent()
{
    delete m_deviceinfo; 
}

// undefined result if type is not correct
DeviceInfo DeviceEvent::device() const
{
    return *m_deviceinfo;
}

// undefined result if type is not correct
Motion DeviceEvent::motion() const
{
    return Motion();  // FIXME
}

// undefined result if type is not correct
ButtonChange DeviceEvent::buttonchange() const
{
    return ButtonChange(); // FIXME
}


////////////////////////////////////////////////////////////////////////////////
// Connection

class ConnectionImpl
{
public:
    ConnectionImpl(NDOF* n) : m_ndof( n ) {  }
private:

    // TODO: handle destruction, i.e. all references from client are gone

    // NDOF manager
    NDOF* m_ndof = nullptr;

    DeviceEvent pop();

    std::queue<DeviceEvent> m_queue;
    mutable std::mutex m_mutex_queue;

};


Connection::Connection() : Connection( nullptr )
{
    
}

Connection::Connection(NDOF* n) : 
    m_impl( std::make_shared<ConnectionImpl>( n ) ) 
{
     
}

DeviceEvent Connection::pop()
{
    if ( m_impl )
    {
        ConnectionImpl& impl = *m_impl;
    
        return DeviceEvent(); // tmp
    }

    // return empty event if this object is empty
    return DeviceEvent();
}


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

Connection NDOF::connect()
{
    std::cout << "ndof::NDOF::connect()"  << std::endl;
    return Connection();
}



} // namespace ndof
