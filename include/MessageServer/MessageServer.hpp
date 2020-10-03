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

#ifndef _INCLUDE_MESSAGE_SERVER_MESSAGE_SERVER_HPP_
#define _INCLUDE_MESSAGE_SERVER_MESSAGE_SERVER_HPP_

#include <string>

#include "Server.hpp"

using Message = server::comm::Message;

enum MessageTypes : server::comm::MessageType {
    USER_LOGGED_IN = 0x10,
    POST_MSG = 0x11,
};

class MessageServer : public server::Server {
public:
    MessageServer(const uint16_t port);

private:
    void onLogin(Client& client) override;
    void onMessageReceived(Client& client, const Message& message) override;

    void handlePostMessage(const char* rcv_msg, Client& client);
    void sendMsgToOthers(Message& msg, Client& client);
};

#endif  // _INCLUDE_MESSAGE_SERVER_MESSAGE_SERVER_HPP_
