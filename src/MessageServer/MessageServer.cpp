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

#include "debug.hpp"

#include "MessageServer/MessageServer.hpp"

static __attribute_used__ const char* LOG_TAG = "MessageServer";
static const char* SERVER_NAME = "Message";

MessageServer::MessageServer(const uint16_t port) : Server(SERVER_NAME, port, false) {
}

void MessageServer::onLogin(Client& client) {
    Message msg(
        MessageTypes::USER_LOGGED_IN,
        (uint8_t*) client.user->token,
        strlen(client.user->token) + 1
    );

    sendMsgToOthers(msg, client);
}

void MessageServer::onMessageReceived(Client& client, const Message& message) {
    (void) client;

    switch (message.getType()) {
    case  MessageTypes::POST_MSG:
        handlePostMessage((const char*) message.getPayload(), client);
    }
}

void MessageServer::handlePostMessage(const char* rcv_msg, Client& client) {

    Message msg(MessageTypes::POST_MSG, nullptr, 0);
    sendMsgToOthers(msg, client);
}

void MessageServer::sendMsgToOthers(Message& msg, Client& client) {
    for (auto& user : mUsers) {
        for (auto& snd_client : user.clients) {
            if (&client != &snd_client) {
                sendMessage(msg, snd_client);
            }
        }
    }
}

int main(int argc, char const *argv[]) {
    // Port numbers up to 1024 are reserved
    uint16_t port = (argc <= 1)? 3001 : std::max(atoi(argv[1]), 1024 + 1);

    MessageServer server(port);
    server.run();

    Debug::Log::i(LOG_TAG, "Server shut down");

    return 0;
}
