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
#include <ctime>

#include <thread>

#include "net/Socket.hpp"
#include "utils/CryptoUtils.hpp"
#include "utils/TextUtils.hpp"

#include "Server.hpp"

namespace server {

static const std::string DUMMY_USERNAME = "DUMMY";
static const std::string DUMMY_PASSWORD = "DUMMYPASSWORD";

static constexpr uint64_t CLIENT_MAX_IDLE_TIMEOUT = 60 * 5;

Server::Server(uint16_t port)
:   mServerSocket(net::Socket::Domain::IPv4, net::Socket::Type::STREAM, port)
{
}

void Server::run() {
    mRunning = true;
}

int64_t Server::now() {
    return std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now()
                                          .time_since_epoch())
                                          .count();
}

void Server::listenForConnections() {
    while(mRunning) {
        net::Connection connection = mServerSocket.Accept();
        mUnloggedConnections.push_back(connection);
    }
}

void Server::removeIdleClients() {
    for (auto client = mLoggedClients.begin(); client < mLoggedClients.end(); client++) {
        const int64_t idleTime = now() - client->lastActiveTime;
        if (idleTime >= CLIENT_MAX_IDLE_TIMEOUT) {
            mLoggedClients.erase(client);
        }
    }
}

bool Server::login(UserToken token) {
    return false;
}

}  // namespace server


int main(int argc, char* argv[]) {
    static constexpr uint16_t DEFAULT_SERVER_PORT = 3000;
    server::Server server(DEFAULT_SERVER_PORT);
    server.run();

    return 0;
}
