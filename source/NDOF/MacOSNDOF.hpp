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
#include "libNDOF.hpp"


namespace ndof 
{

////////////////////////////////////////////////////////////////////////////////
// MacOSNDOF

class MacOSNDOF
{
public:
    virtual void begin() override;
    virtual void end() override;

    // the canonical NDOF object used for API communication
    static MacOSNDOF* ndof                             = nullptr;
    
    ////////////////////////////////////////////////////////////////////////////

    // 3Dconnexion framework
    void* m_connexion_dyld                             = nullptr;
    macos::ConnexionAPIVersion m_connexion_api_version = macos::ConnexionAPIVersion::EMPTY;
    // 3Dconnexion clientID (connexion to 3Dconnexion driver)
    uint16_t m_connexion_client_id                     = 0;

private: // FIXME: can callbacks be private?
    // callbacks for 3Dconnexion API framework
    static void connexion_MessageHandler(unsigned int productID, unsigned int messageT , void* messageArg);
    static void connexion_AddedHandler(unsigned int productID );
    static void connexion_RemovedHandler(unsigned int productID );

};

} // namespace ndof


#endif // _LIBNDOF_MACOSNDOF_HPP_
