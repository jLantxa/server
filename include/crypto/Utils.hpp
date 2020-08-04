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

#ifndef _INCLUDE_CRYPTO_UTILS_HPP_
#define _INCLUDE_CRYPTO_UTILS_HPP_

#include <string>

#include "openssl/sha.h"

namespace server::crypto {

using sha256Hash = unsigned char[SHA256_DIGEST_LENGTH];

/** \brief Create a SHA256 from a std::string
* \param str The source std::string.
* \param hash An unsigned char[32] to write the hash.
*/
void sha256(std::string str, sha256Hash hash);

}  // namespace server::crypto

#endif  // _INCLUDE_CRYPTO_UTILS_HPP_
