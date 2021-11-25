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
#ifndef _LIBNDOF_MACOSNDOF_HPP_
#define _LIBNDOF_MACOSNDOF_HPP_
#include <memory>
#include "libNDOF.hpp"

namespace ndof 
{

////////////////////////////////////////////////////////////////////////////////
// MacOSNDOF

class MacOSNDOF_Private;

class MacOSNDOF : public NDOF
{
friend class MacOSNDOF_Private;

public:
    MacOSNDOF();
    ~MacOSNDOF();

    virtual void begin() override;
    virtual void end() override;

    // the canonical NDOF object used for API communication
    static MacOSNDOF* ndof;
    
    ////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<MacOSNDOF_Private> m_private;
};

} // namespace ndof


#endif // _LIBNDOF_MACOSNDOF_HPP_
