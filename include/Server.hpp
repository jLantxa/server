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

constexpr uint16_t NOTIFICATION_SERVER_PORT = 3000;

class Server {
public:
    Server(uint16_t port);
    virtual ~Server() = default;

    void start();

protected:
    struct Client final {
        int64_t lastActiveTime;
        net::Connection connection;
    };

    struct User final {
        UserToken token;
        std::vector<Client> clients;
    };

    bool mRunning = false;
    net::ServerSocket mServerSocket;
    std::vector<User> mUsers;
    std::deque<Client> mUnloggedConnections;

    virtual void run() = 0;

    bool login(UserToken token, Client& client);
    void listenForConnections();
    void removeIdleClients();

    int64_t now();

private:
    void loopRemoveIdleClients();
};

}  //namespace server

#endif  // _INCLUDE_SERVER_HPP_
