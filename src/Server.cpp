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

#include "crypto/Utils.hpp"
#include "net/Socket.hpp"

#include "Server.hpp"

namespace server {

bool login(std::string username, crypto::sha256Hash key_hash) {
    return false;
}

bool signUp(std::string username, crypto::sha256Hash key_hash) {
    return false;
}

}  // namespace server

int main(int argc, char* argv[]) {
    return 0;
}
