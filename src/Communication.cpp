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

#include <cstring>

#include <numeric>

#include "Communication.hpp"
#include "debug.hpp"

static __attribute_used__ const char* LOG_TAG = "Communication";

namespace server {
namespace comm {

Message::Message(const uint8_t* const buffer, const uint16_t bufferSize) {
    Debug::Log::v(LOG_TAG, "%s()", __func__);

    static constexpr uint8_t typeOffset = 0;
    static constexpr uint8_t checksumOffset = typeOffset + sizeof(header.type);
    static constexpr uint8_t sizeOffset = checksumOffset + sizeof(header.checksum);
    static constexpr uint8_t payloadOffset = sizeof(header);

    header.type = *(reinterpret_cast<const MessageType* const>(buffer + typeOffset));
    header.size = *(reinterpret_cast<const MessageType* const>(buffer + sizeOffset));
    header.checksum = *(reinterpret_cast<const MessageType* const>(buffer + checksumOffset));
    payload = reinterpret_cast<const uint8_t* const>(buffer + payloadOffset);

    if (header.size > bufferSize) {
        header.size = 0;
        validFlag = false;
        Debug::Log::w(LOG_TAG, "%s(): declared message size bigger than buffer", __func__);
    } else {
        validFlag = isCheckSumOk();
    }

    Debug::Log::v(LOG_TAG, "%s(): type=%d, size=%d, valid=%d",
        __func__, header.type, header.size, validFlag);
}

Message::Message(MessageType type, const uint8_t *const buffer, const uint16_t size) {
    header.type =  type;
    header.size = size;
    payload = buffer;

    if (header.size > (size - sizeof(header))) {
        header.size = 0;
        validFlag = false;
        Debug::Log::w(LOG_TAG, "%s(): declared message size bigger than buffer", __func__);
    } else {
        header.checksum = calculateChecksum();
        validFlag = isCheckSumOk();
    }

    Debug::Log::v(LOG_TAG, "%s(): type=%d, size=%d, valid=%d",
        __func__, header.type, header.size, validFlag);

}

uint8_t Message::calculateChecksum() const {
    const uint8_t sum = std::accumulate(payload, payload + header.size, header.type + header.size);
    return (0xFF - sum);
}

bool Message::isValid() const {
    return validFlag;
}

bool Message::isCheckSumOk() const {
    return calculateChecksum() == header.checksum;
}

bool Message::serialize(uint8_t* buffer, uint16_t bufferSize) const {
    if (bufferSize < getLength()) {
        return false;
    }

    memcpy(buffer, &header, sizeof(header));
    memcpy(buffer + sizeof(header), payload, header.size);
    return true;
}

}  // namespace comm
}  // namespace server
