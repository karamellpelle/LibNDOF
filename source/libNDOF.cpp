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

class ConnectionImpl
{
private:
    ConnectionImpl(NDOF& n) : m_ndof( n ) {  }

    // TODO: handle destruction, i.e. all references from client are gone

    // NDOF manager
    NDOF& m_ndof;

    DeviceEvent pop_event();

    std::queue<DeviceEvent> m_queue;
    mutable std::mutex m_mutex_queue;

};


Connection::Connection(NDOF& n) : 
    m_impl( std::make_shared<ConnectionImpl>( n ) ) 
{
     
}



Connection NDOF::begin()
{
    std::cout << "ndof::Context::begin():"  << std::endl
              << "  libNDOF:" << std::endl 
              << "    version major: " << LIBNDOF_VERSION_MAJOR << std::endl
              << "    version minor: " << LIBNDOF_VERSION_MINOR << std::endl
              << std::endl;

    // TODO: setup hidapi, launch thread

    return Connection( *this );

}


////////////////////////////////////////////////////////////////////////////////
// DeviceEvent

DeviceInfo DeviceEvent::device() const
{
    return DeviceInfo();  // FIXME
}

Motion DeviceEvent::motion() const
{
    return Motion();  // FIXME
}

ButtonChange DeviceEvent::buttonchange() const
{
    return ButtonChange(); 
}

std::string DeviceInfo::name() const
{
    return m_name;
}


////////////////////////////////////////////////////////////////////////////////
// Connection

DeviceEvent Connection::pop_event()
{
    ConnectionImpl& imp = *m_impl;
}


////////////////////////////////////////////////////////////////////////////////
// DeviceInfo


} // namespace ndof
