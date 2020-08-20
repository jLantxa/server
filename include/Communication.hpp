/*
 * Copyright (C) 2020  Javier Lancha Vázquez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _INCLUDE_COMMUNICATION_HPP_
#define _INCLUDE_COMMUNICATION_HPP_

#include <cstdint>

namespace server {
namespace comm {

enum Destination : uint32_t {
    SERVER = 0,
};

enum ServerActions : uint32_t {
    LOGIN = 0,
};

struct Message {
    uint32_t destination;
    uint32_t action;
    uint16_t size;
    uint8_t* payload;
};

}  // namespace comm
}  // namespace server

#endif  // _INCLUDE_COMMUNICATION_HPP_
