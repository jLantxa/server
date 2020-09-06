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

#include <chrono>
#include <deque>
#include <string>
#include <thread>
#include <vector>

#include "Database.hpp"
#include "debug.hpp"
#include "net/Socket.hpp"
#include "utils/CryptoUtils.hpp"
#include "utils/TextUtils.hpp"

#include "Server.hpp"

LOG_TAG("Server");

namespace server {

static constexpr auto REMOVE_IDLE_RATE = std::chrono::seconds(300);
static constexpr uint64_t CLIENT_MAX_IDLE_TIMEOUT = 10;

Server::Server(uint16_t port)
:   mServerSocket(net::Socket::Domain::IPv4, net::Socket::Type::STREAM, port)
{
    Debug::Log::i(LOG_TAG, "Created server at port %d", port);
}

void Server::run() {
    mRunning = true;
    Debug::Log::i(LOG_TAG, "Running server");

    std::thread acceptConnectionsThread(&Server::listenForConnections, this);
    acceptConnectionsThread.detach();

    std::thread removeIdleClientsThread(&Server::loopRemoveIdleClients, this);
    removeIdleClientsThread.detach();
}

int64_t Server::now() {
    return std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now()
                                          .time_since_epoch())
                                          .count();
}

void Server::listenForConnections() {
    Debug::Log::d(LOG_TAG, "%s()", __func__);
    mServerSocket.Listen();

    while (mRunning) {
        net::Connection connection = mServerSocket.Accept();
        mUnloggedConnections.emplace_back(connection);
        Debug::Log::i(LOG_TAG, "New unlogged connection");
    }
}

void Server::removeIdleClients() {
    for (auto user = mUsers.begin(); user != mUsers.end(); user++) {
        std::vector<Client>& userClients = user->clients;
        for (auto client = userClients.begin(); client < userClients.end(); client++) {
            const int64_t idleTime = now() - client->lastActiveTime;
            if (idleTime >= CLIENT_MAX_IDLE_TIMEOUT) {
                userClients.erase(client);
                client--;
                Debug::Log::i(LOG_TAG, "Client from user %d timed out", user->token);
            }
        }

        if (userClients.size() == 0) {
            mUsers.erase(user);
            user--;
            Debug::Log::i(LOG_TAG, "User %d timed out", user->token);
        }
    }

    for (auto client = mUnloggedConnections.begin(); client != mUnloggedConnections.end(); client++) {
        const int64_t idleTime = now() - client->lastActiveTime;
        if (idleTime >= CLIENT_MAX_IDLE_TIMEOUT) {
            mUnloggedConnections.erase(client);
            client--;
            Debug::Log::i(LOG_TAG, "Unlogged client timed out");
        }
    }
}

void Server::loopRemoveIdleClients() {
    while (mRunning) {
        std::this_thread::sleep_for(REMOVE_IDLE_RATE);
        removeIdleClients();
    }
}

bool Server::login(const UserToken token, Client& client) {
    Debug::Log::d(LOG_TAG, "Login attempt with token %d", token);
    Database& database = Database::getInstance();

    if (!database.userTokenExists(token)) {
        Debug::Log::d(LOG_TAG, "Token %d does not exist", token);
        return false;
    }

    for (User user : mUsers) {
        if (user.token == token) {
            user.clients.emplace_back(client);
            client.user = &user;
            Debug::Log::d(LOG_TAG, "User %d logged in with new client", token);
            return true;
        }
    }

    User newUser(token);
    newUser.clients.emplace_back(client);
    client.user = &newUser;
    Debug::Log::d(LOG_TAG, "New user %d logged in", token);
    return true;
}

}  // namespace server
