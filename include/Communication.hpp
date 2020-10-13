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
class Message {
public:
    Message(uint8_t* buffer, const uint16_t maxSize);
    Message(MessageType type, uint8_t*  buffer, const uint16_t size);
    Message(MessageType type);

    struct __attribute__((packed)) Header {
        MessageType type;
        uint8_t checksum;
        uint16_t size = 0;
    };

    inline MessageType getType() const {
        return header.type;
    }

    inline uint8_t getPayloadSize() const{
        return header.size;
    }

    inline uint8_t getLength() const {
        return header.size + sizeof(header);
    }

    inline const uint8_t* getPayload() const {
        return payload;
    }

    bool isValid() const;

    bool serialize(uint8_t* buffer, uint16_t bufferSize) const;

private:
    Header header;
    const uint8_t* payload = nullptr;
    bool validFlag = false;

    uint8_t calculateChecksum() const;
    bool isCheckSumOk() const;
};


/**
 * \brief Reserved server messages types
 *
*/
namespace ServerMsgTypes {

enum : MessageType {
    LOGIN  = 0x0000,
    LOGOUT = 0x0001,
    OK     = 0x0002,
    ERROR  = 0x0003,
};

}  // namespace ServerMsgTypes

}  // namespace comm
}  // namespace server

#endif  // _INCLUDE_COMMUNICATION_HPP_
