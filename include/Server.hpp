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

#include "net/Socket.hpp"

namespace server {

using UserToken = uint64_t;

class Server {
public:
    Server(uint16_t port);
    ~Server() = default;

    void run();

private:
    struct Client {
        uint64_t userToken;
        int64_t lastActiveTime;
        net::Connection connection;
    };

    bool mRunning = false;
    net::ServerSocket mServerSocket;
    std::deque<Client> mLoggedClients;
    std::deque<net::Connection> mUnloggedConnections;

    bool login(UserToken token);

    void listenForConnections();
    void removeIdleClients();

    int64_t now();
};

}  //namespace server

#endif  // _INCLUDE_SERVER_HPP_
