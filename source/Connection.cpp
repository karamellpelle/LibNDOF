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

} // namespace ndof
