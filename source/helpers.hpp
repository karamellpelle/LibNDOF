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
#ifndef _LIBNDOF_HELPERS_HPP_
#define _LIBNDOF_HELPERS_HPP_
#include <type_traits>


////////////////////////////////////////////////////////////////////////////////
// https://wiggling-bits.net/using-enum-classes-as-type-safe-bitmasks/


////////////////////////////////////////////////////////////////////////////////
// class Bitmask a where

template<typename A>
struct ClassBitmask
{
    static const bool enable = false;
};

// operator|
template<typename A>
typename std::enable_if<ClassBitmask<A>::enable, A>::type
operator |(A lhs, A rhs)
{
    using UnderlyingType = typename std::underlying_type<A>::type;
    return static_cast<A> ( static_cast<UnderlyingType>( lhs ) | static_cast<UnderlyingType>( rhs ) );
}

#define INSTANCE_BITMASK(x)  \
template<>                           \
struct ClassBitmask<x>      \
{                                    \
    static const bool enable = true; \
};


#endif // _LIBNDOF_HELPERS_HPP_

