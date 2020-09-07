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

static __attribute_used__ const char* LOG_TAG = "Server";

namespace server {

static constexpr auto REMOVE_IDLE_PERIOD = std::chrono::seconds(30);
static constexpr int32_t CLIENT_MAX_IDLE_TIMEOUT = 300;

static constexpr auto HANDLE_MESSAGES_PERIOD = std::chrono::milliseconds(100);

Server::Server(const char* serverName, const uint16_t port)
:   mServerName(serverName),
    mServerSocket(net::Socket::Domain::IPv4, net::Socket::Type::STREAM, port)
{
    Debug::Log::i(LOG_TAG, "Created server at port %d", port);
}

int64_t Server::getCurrentTime() {
    return std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now()
                                          .time_since_epoch())
                                          .count();
}

const char* Server::getName() const {
    return mServerName;
}

void Server::run() {
    mRunning = true;
    Debug::Log::i(LOG_TAG, "Running server");

    mServerSocket.Listen();

    std::thread listenForConnectionsThread([&]() {
        Debug::Log::d(LOG_TAG, "Listening for new connections");
        while (mRunning) {
            // Accept is a blocking call so no need for a sleep
            net::Connection connection = mServerSocket.Accept();
            mUnloggedConnections.emplace_back(connection);
            Debug::Log::i(LOG_TAG, "New unlogged connection");
        }
    });

    std::thread removeIdleClientsThread([&]() {
        while (mRunning) {
            removeIdleClients();
            std::this_thread::sleep_for(REMOVE_IDLE_PERIOD);
        }
    });
    std::thread handleMessagesThread([&]() {
        while (mRunning) {
            handleMessages();
            std::this_thread::sleep_for(HANDLE_MESSAGES_PERIOD);
        }

    });

    listenForConnectionsThread.detach();
    removeIdleClientsThread.detach();
    handleMessagesThread.join();

    Debug::Log::i(LOG_TAG, "Exit %s()", __func__);
}

void Server::removeIdleClients() {
    Debug::Log::v(LOG_TAG, "Enter %s()", __func__);

    const int64_t now = getCurrentTime();
    for (auto user = mUsers.begin(); user != mUsers.end(); user++) {
        std::vector<Client>& userClients = user->clients;
        for (auto client = userClients.begin(); client < userClients.end(); client++) {
            const int64_t idleTime = now - client->lastActiveTime;
            if (idleTime >= CLIENT_MAX_IDLE_TIMEOUT) {
                userClients.erase(client);
                client--;
                Debug::Log::i(LOG_TAG,
                    "Client from user %d timed out (%d s)", user->token, idleTime);
            }
        }

        if (userClients.size() == 0) {
            mUsers.erase(user);
            user--;
            Debug::Log::i(LOG_TAG,
                "User %d logged out (no logged in clients)", user->token);
        }
    }

    for (auto client = mUnloggedConnections.begin(); client != mUnloggedConnections.end(); client++) {
        const int64_t idleTime = now - client->lastActiveTime;
        if (idleTime >= CLIENT_MAX_IDLE_TIMEOUT) {
            mUnloggedConnections.erase(client);
            client--;
            Debug::Log::i(LOG_TAG, "Unlogged client timed out (%d s)", idleTime);
        }
    }
}

void Server::handleMessages() {
    Debug::Log::v(LOG_TAG, "Enter %s()", __func__);
}

bool Server::authenticate(const UserToken token) {
    const Database& database = Database::getInstance();
    const bool success = database.authenticateUserToken(token, mServerName);
    if (success) {
        Debug::Log::d(LOG_TAG,
            "%s(): User %d successfully authenticated in server %s",
            __func__, token, mServerName);
    } else {
        Debug::Log::d(LOG_TAG,
            "%s(): Could not authenticate user %d in server %s",
            __func__, token, mServerName);
    }

    return success;
}

bool Server::login(const UserToken token, Client& client) {
    Debug::Log::i(LOG_TAG, "Login attempt with token %d", token);

    if (!authenticate(token)) {
        Debug::Log::i(LOG_TAG, "User token %d not registered in this server", token);
        return false;
    }

    for (User user : mUsers) {
        if (user.token == token) {
            user.clients.emplace_back(client);
            client.user = &user;
            Debug::Log::i(LOG_TAG, "User %d logged in with new client", token);
            return true;
        }
    }

    User newUser(token);
    newUser.clients.emplace_back(client);
    client.user = &newUser;
    Debug::Log::i(LOG_TAG, "New user %d logged in", token);
    return true;
}

}  // namespace server
