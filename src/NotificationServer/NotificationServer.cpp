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

using server::comm::Message;

static __attribute_used__ const char* LOG_TAG = "NotificationServer";
static const char* SERVER_NAME = "Notification";

NotificationServer::NotificationServer(const uint16_t port) : Server(SERVER_NAME, port, true) {

}

void NotificationServer::onLogin(Client& client) {
    // TODO
    (void) client;
}

void NotificationServer::onMessageReceived(Client& client, const Message& message) {
    Debug::Log::e(LOG_TAG, "Message from user %s", client.user->token.c_str());

    switch(message.getType()) {
    case REQUEST_TASKS:
        break;

    default:
        break;
    }
}

int main(int argc, char* argv[]) {
    // Port numbers up to 1024 are reserved
    uint16_t port = (argc <= 1)? 3000 : std::max(atoi(argv[1]), 1024 + 1);

    NotificationServer server(port);
    server.run();

    Debug::Log::i(LOG_TAG, "Server shut down");
    return 0;
}
