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
#ifndef _LIBNDOF_CONNEXION_HPP_
#define _LIBNDOF_CONNEXION_HPP_
#include <cstdint>
#include <tuple>


namespace ndof
{

////////////////////////////////////////////////////////////////////////////////
// ConnexionRotation

using ConnexionRotation    = std::tuple<int16_t, int16_t, int16_t>;


////////////////////////////////////////////////////////////////////////////////
// ConnexionTranslation

using ConnexionTranslation = std::tuple<int16_t, int16_t, int16_t>;


////////////////////////////////////////////////////////////////////////////////
// ConnexionButtons

class ConnexionButtons
{
public:
    explicit ConnexionButtons(uint32_t b) : m_bits( b ) {  }

private:
    uint32_t m_bits = 0x00000000;

    //Mapping m_map = nullptr;
    //static constexpr Mapping map_new;
};

} // namespace ndof

#endif
