#include "libNDOF.hpp"

#include <iostream>

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
