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
#ifndef _LIBNDOF_WINNDOF_HPP_
#define _LIBNDOF_WINNDOF_HPP_
#include "libNDOF.hpp"


namespace ndof 
{

////////////////////////////////////////////////////////////////////////////////
// WinNDOF

class WinNDOF
{
public:
    virtual void begin() override;
    virtual void end() override;

    // drive NDOF manually by application HID messages on Win32
    // TODO: see if we can create a dummy windowclass in 'begin()' with custom WinProc
    bool inject(UINT , WPARAM, LPARAM );

private:

};


} // namespace ndof


#endif // _LIBNDOF_WINNDOF_HPP_
