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

using MessageType = uint16_t;

/**
 * \brief A message that can be sent to and from the server.
 * This is the standard message format that must be used when communicating
 * with the server, including logging in and sending messages to users in the
 * chat.
*/
template <uint16_t length>
class Message {
public:
    struct Header {
        MessageType type;
        uint16_t size;
    };

    Header header;
    uint8_t payload[length - sizeof(Header)];

    inline static constexpr std::size_t maxSize() {
        return length;
    }

    inline static constexpr std::size_t maxPayloadSize() {
        return (length - sizeof(Header));
    }

    inline uint16_t payloadSize() const {
        return header.size;
    }
};

template <uint16_t length> union MessageBuffer {
    uint8_t buffer[length];
    Message<length> message;
};

namespace ServerMessageTypes {

enum : MessageType {
    LOGIN = 0,
};

}  // namespace ServerMessageTypes

}  // namespace comm
}  // namespace server

#endif  // _INCLUDE_COMMUNICATION_HPP_
