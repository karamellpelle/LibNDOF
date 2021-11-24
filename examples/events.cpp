#include <iostream>
#include "libNDOF.hpp"
#include "libNDOF/devices.hpp"

using uint = ndof::uint;


// quit if 'q' or EOF on std::cin 
bool is_quit()
{
    // only read characters if such is available, to prevent hang. 
    // http://www.cplusplus.com/reference/streambuf/streambuf/in_avail/
    // TODO: can this be done smoother?
    // FIXME: this function does not work right now :(

    std::streambuf* buf = std::cin.rdbuf();
    auto avail = buf->in_avail();
    
    // look for "quit" character, if characters are available at all
    if ( 0 <= avail )
    {
        for ( uint i = 0; i != avail; ++i ) 
        {
            int c = buf->sbumpc();  
            if ( c == 'q' ) return true;
        }

        return false;
    }
    else
    {
        // EOF or failbit
        return true;
    }
    
}


int main(int argc, char** argv)
{
    using namespace ndof;

    // create NDOF manager
    NDOF ndof;
    ndof.begin();

    // connect (and reconnect) to any available device.
    // this is a pretty normal line, if you only will use one connected device at the time
    Connection connection = ndof.connect( Connection::Ideal() );
    // connect to any available device, but only accept reconnections to the same device
    //Connection connection = ndof.connect( Connection::Ideal()( Connection::Ideal::Reconnect::SAME_DEVICE ) );
    // only connect to "SpaceMouse" devices (connected or wireless), acc
    //Connection connection = ndof.connect( Connection::Ideal( { device::spacemouse_compact, device::spacemouse_wireless } )( Connection::Ideal::Reconnect::SAME_VARIANT ) );

    while ( !is_quit() )
    {
        // poll events until no more left
        while ( auto event = connection.pop() )
        {
            if ( event( ndof::DeviceEventType::CONNECTED ) )
            {
                ndof::DeviceInfo info = event.device();
                std::cout << "NDOF: device connected"  << std::endl
                          << "    name: " << info.name() << std::endl
                          << std::endl;
                continue;
            }
            if ( event( ndof::DeviceEventType::DISCONNECTED ) )
            {
                std::cout << "NDOF: device disconnected" << std::endl;
                continue;
            }
            if ( event( ndof::DeviceEventType::MOTION ) )
            {
                ndof::Motion motion = event.motion();

                std::cout << "NDOF: movement "  << "(time: " << motion.time << ")" << std::endl
                          << "    translate: " << std::endl
                          << "    rotate:    " << std::endl
                          << "    rotate:    " << std::endl
                          << std::endl;
                continue;
            }
            if ( event( ndof::DeviceEventType::BUTTON_CHANGE ) )
            {
                ndof::ButtonChange change = event.buttonchange();

                std::cout << "NDOF: button change "  << "(time: " << change.time << ")" << std::endl
                          << "    button:    " /*<< change.button*/ << std::endl
                          << "    state:     " /*<< change.state*/ << std::endl
                          << std::endl;
                continue;
            }
            
            // (continue popping events until no more left)
        }
        
    }

    return 0;
}
