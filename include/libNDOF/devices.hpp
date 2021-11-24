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
#ifndef _LIBNDOF_LIBNDOF_DEVICES_HPP_
#define _LIBNDOF_LIBNDOF_DEVICES_HPP_

namespace ndof 
{

namespace device
{

// gallery & info: http://spacemice.org/index.php?title=Gallery


////////////////////////////////////////////////////////////////////////////////
// modern

// https://3dconnexion.com/us/product/spacemouse-enterprise/
constexpr DeviceVariant spacemouse_enterprise   { 0x256f, 0xc633, 0x0000, 0x0000, }; // cable

// https://3dconnexion.com/us/product/spacemouse-pro/
// https://3dconnexion.com/us/product/spacemouse-pro-wireless/
constexpr DeviceVariant spacemouse_pro          { 0x046d, 0xc62b, 0x0000, 0x0000, }; // cable FIXME: only Logitec?
constexpr DeviceVariant spacemouse_pro_wireless { 0x256f, 0xc632, 0x0000, 0xc631, }; // wireless + cable

// https://3dconnexion.com/us/product/spacemouse-compact/
// https://3dconnexion.com/us/product/spacemouse-wireless/
constexpr DeviceVariant spacemouse_compact      { 0x256f, 0xc635, 0x256f, 0x0000, }; // cable
constexpr DeviceVariant spacemouse_wireless     { 0x256, 0xc62f, 0x256f, 0xc62e, };  // wireless + cable


////////////////////////////////////////////////////////////////////////////////
// older

// http://spacemice.org/index.php?title=SpacePilot
// http://spacemice.org/index.php?title=SpacePilot_Pro
constexpr DeviceVariant spacepilot              { 0x046d, 0xc625, 0x0000, 0x0000, }; // cable
constexpr DeviceVariant spacepilot_pro          { 0x046d, 0xc629, 0x0000, 0x0000, }; // cable

// http://spacemice.org/index.php?title=SpaceExplorer
constexpr DeviceVariant spaceexplorer           { 0x046d, 0xc627, 0x0000, 0x0000, }; // cable

// http://spacemice.org/index.php?title=SpaceNavigator
// http://spacemice.org/index.php?title=SpaceNavigator_Notebooks
constexpr DeviceVariant spacenavigator          { 0x046d, 0xc626, 0x0000, 0x0000, }; // cable
constexpr DeviceVariant spacenavigator_notebook { 0x046d, 0xc628, 0x0000, 0x0000, }; // wireless + cable

// http://spacemice.org/index.php?title=SpaceNavigator_Keyboard (rare!)
//constexpr DeviceVariant spacenavigator_keyboard { 0x046d, 0x0000, 0x0000, 0x0000, }; // cable

// http://spacemice.org/index.php?title=SpaceTraveler
constexpr DeviceVariant spacetraveler           { 0x046d, 0xc623, 0x0000, 0x0000, }; // cable

// http://spacemice.org/index.php?title=Spaceball_5000. FIXME: rebranded by IBM, HP, etc - does the VID change?
constexpr DeviceVariant spaceball_5000          { 0x046d, 0xc629, 0x0000, 0x0000, }; // cable

// http://spacemice.org/index.php?title=Cadman
constexpr DeviceVariant cadman                  { 0x046d, 0xc605, 0x0000, 0x0000, }; // cable

// http://spacemice.org/index.php?title=Spacemouse_Classic
// http://spacemice.org/index.php?title=Spacemouse_PlusXT
constexpr DeviceVariant spacemouse_classic      { 0x046d, 0xc606, 0x0000, 0x0000, }; // cable
constexpr DeviceVariant spacemouse_plus_xt      { 0x046d, 0xc603, 0x0000, 0x0000, }; // cable


} // namespace ndof::device

std::string to_string(const DeviceVariant& );
DeviceVariant from_string(const std::string& );

} // namespace ndof


#endif // _LIBNDOF_LIBNDOF_DEVICES_HPP_
