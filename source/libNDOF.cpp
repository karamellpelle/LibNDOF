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
#include <sstream>
#include <cstdio>

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


} // namespace ndof
