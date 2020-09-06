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

#include <cstdlib>
#include <cstdint>

#include "debug.hpp"
#include "NotificationServer/NotificationServer.hpp"

LOG_TAG("NotificationServer");

namespace server {

NotificationServer::NotificationServer(const uint16_t port) : Server(port) {

}

void NotificationServer::onLogin(Client& client) {

}

void NotificationServer::onMessageReceived(Client& client, const uint8_t *const buffer) {

}

}  // namespace server

int main(int argc, char* argv[]) {
    const uint16_t port = (argc <= 1)? 3000 : atoi(argv[1]);

    server::NotificationServer server(port);
    server.run();

    return 0;
}
