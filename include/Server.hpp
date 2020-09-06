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

#ifndef _INCLUDE_SERVER_HPP_
#define _INCLUDE_SERVER_HPP_

#include <cstdint>

#include <deque>
#include <string>
#include <vector>

#include "Database.hpp"
#include "net/Socket.hpp"

namespace server {

class Server {
public:
    Server(const uint16_t port);
    virtual ~Server() = default;

    void run();

protected:
    struct User;
    struct Client;

    struct Client final {
        net::Connection connection;
        int64_t lastActiveTime = now();
        User* user = nullptr;

        Client(const net::Connection connection, User* user)
        :   connection(connection), user(user) { }

        Client(const net::Connection connection)
        :   connection(connection) { }
    };

    struct User final {
        UserToken token;
        std::vector<Client> clients;

        User(const UserToken) : token(token) { }
    };

    virtual void onLogin(Client& client) = 0;
    virtual void onMessageReceived(Client& client, const uint8_t *const buffer) = 0;

private:
    volatile bool mRunning = false;

    net::ServerSocket mServerSocket;
    std::vector<User> mUsers;
    std::deque<Client> mUnloggedConnections;

    void listenForConnections();
    void removeIdleClients();
    void loopRemoveIdleClients();

    bool login(const UserToken token, Client& client);

    static int64_t now();
};

}  //namespace server

#endif  // _INCLUDE_SERVER_HPP_
