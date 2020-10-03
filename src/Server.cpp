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

#include <cstdint>
#include <cstring>
#include <ctime>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "Communication.hpp"
#include "debug.hpp"
#include "Database.hpp"
#include "net/Socket.hpp"
#include "util/TextUtils.hpp"

#include "Server.hpp"

static __attribute_used__ const char* LOG_TAG = "Server";

namespace server {

using comm::ServerMessageTypes::LOGIN;
using comm::ServerMessageTypes::LOGOUT;

Server::Server(std::string serverName, const uint16_t port, bool requireAuth)
:
    mRequireAuthentication(requireAuth),
    mServerName(serverName),
    mServerSocket(net::Socket::Domain::IPv4, net::Socket::Type::STREAM, port)
{
    DatabaseManager& dbManager = DatabaseManager::getInstance();
    dbManager.initDatabase(mDatabase);

    Debug::Log::i(LOG_TAG, "Created server at port %d", port);
}

int64_t Server::getCurrentTime() {
    return std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now()
                                          .time_since_epoch())
                                          .count();
}

std::string Server::getName() const {
    return mServerName;
}

void Server::run() {
    mRunning = true;
    Debug::Log::i(LOG_TAG, "Running server");

    try {
        mServerSocket.Listen();
    }
    catch (server::net::SocketException& exception) {
        Debug::Log::e(LOG_TAG, exception.what());
        return;
    }

    std::thread listenForConnectionsThread([&]() {
        Debug::Log::d(LOG_TAG, "Listening for new connections");
        while (mRunning) {
            if (getNumUnloggedConnections() > MAX_UNLOGGED_CONNECTIONS) {
                Debug::Log::w(LOG_TAG,
                    "Maximum number of unlogged clients reached. "
                    "Refusing new connections");
                removeIdleClients();
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }

            try {
                net::Connection connection = mServerSocket.Accept();
                Client newClient(connection);
                mUnloggedConnections.push_back(newClient);
                Debug::Log::i(LOG_TAG, "New unlogged connection");
                printNumClients();
            }
            catch (net::SocketException& exception) {
                Debug::Log::e(LOG_TAG,
                    "listenForConnections: Could not accept incoming connection",
                    __func__);
                Debug::Log::e(LOG_TAG, exception.what());
            }
        }
    });

    std::thread removeIdleClientsThread([&]() {
        while (mRunning) {
            removeIdleClients();
            std::this_thread::sleep_for(mRemoveIdlePeriod_sec);
        }
    });

    std::thread handleMessagesThread([&]() {
        while (mRunning) {
            pollMessages();
            std::this_thread::sleep_for(mHandleMessagesPeriod_ms);
        }
    });

    listenForConnectionsThread.detach();
    removeIdleClientsThread.detach();
    handleMessagesThread.join();

    Debug::Log::i(LOG_TAG, "Exit %s()", __func__);
}

bool Server::removeUserIfUnlogged(std::vector<User>::iterator user) {
    if (user->clients.size() == 0) {
        Debug::Log::i(LOG_TAG, "User %s unlogged (no clients connected)", user->token.c_str());
        mUsers.erase(user);
        user--;
        return true;
    }

    return false;
}

void Server::removeIdleClients() {
    Debug::Log::v(LOG_TAG, "Enter %s()", __func__);

    const int64_t now = getCurrentTime();
    for (auto user = mUsers.begin(); user != mUsers.end(); user++) {
        std::vector<Client>& userClients = user->clients;
        for (auto client = userClients.begin(); client < userClients.end(); client++) {
            const int64_t idleTime = now - client->lastActiveTime;
            if (idleTime >= mLoggedClientMaxIdleTimeout_sec.count()) {
                userClients.erase(client);
                client--;
                Debug::Log::i(LOG_TAG,
                    "Client from user %s timed out (%d s)", user->token.c_str(), idleTime);
            }
        }

        removeUserIfUnlogged(user);
    }

    for (auto client = mUnloggedConnections.begin(); client != mUnloggedConnections.end(); client++) {
        const int64_t idleTime = now - client->lastActiveTime;
        if (idleTime >= mUnloggedClientMaxIdleTimeout_sec.count()) {
            Debug::Log::i(LOG_TAG, "Unlogged client timed out (%d s)", idleTime);
            mUnloggedConnections.erase(client);
            client--;
        }
    }

    printNumClients();
}

bool Server::handleLogin(Client& client, const comm::Message& loginMsg) {
    Debug::Log::v(LOG_TAG, "Enter %s()", __func__);

    const uint16_t size =  loginMsg.getPayloadSize();

    if (size == 0) {
        Debug::Log::e(LOG_TAG, "%s(): LOGIN message has size 0", __func__);
        return false;
    }

    std::string token = std::string((const char*) loginMsg.getPayload());
    Debug::Log::d(LOG_TAG, "%s(): token = %s", __func__, token.c_str());
    return tryToLogin(token, client);
}

void Server::pollUnlogged() {
    for (auto client_it = mUnloggedConnections.begin(); client_it != mUnloggedConnections.end(); client_it++) {
        const ssize_t numBytes = client_it->connection.Read(mMessageBuffer, BUFFER_SIZE);
        if (numBytes < 0) {
            continue;
        }
        else if (numBytes == 0) {
            mUnloggedConnections.erase(client_it);
            client_it--;
            printNumClients();
            continue;
        }

        client_it->refreshTime();
        comm::Message msg(mMessageBuffer, numBytes);
        if (msg.isValid()) {
            const comm::MessageType type = msg.getType();
            switch (type)
            {
            case LOGIN:
                Debug::Log::v(LOG_TAG, "%s(): Unlogged client message LOGIN", __func__);
                if (handleLogin(*client_it, msg) == true) {
                    mUnloggedConnections.erase(client_it);
                    client_it--;
                    printNumClients();
                }
                break;

            default:
                Debug::Log::v(LOG_TAG, "%s(): Unlogged client message not login", __func__);
                break;
            }
        }
    }
}

void Server::pollUsers() {
    for (auto user = mUsers.begin(); user < mUsers.end(); user++) {
        for (auto client_it = user->clients.begin(); client_it != user->clients.end(); client_it++) {
            const ssize_t numBytes = client_it->connection.Read(mMessageBuffer, BUFFER_SIZE);
            if (numBytes < 0) {
                continue;
            }
            else if (numBytes == 0) {
                user->clients.erase(client_it);
                client_it--;

                printNumClients();
                continue;
            }

            // client_it->refreshTime();
            comm::Message msg(mMessageBuffer, numBytes);
            if (msg.isValid()) {
                const comm::MessageType type = msg.getType();

                switch(type) {
                    case LOGIN: {
                        Debug::Log::v(LOG_TAG, "%s(): Logged client (user %s) message LOGIN",
                            __func__, client_it->user->token.c_str());
                        break;
                    }

                    case LOGOUT: {
                        Debug::Log::v(LOG_TAG, "%s(): Logged client (user %s) message LOGOUT",
                            __func__, client_it->user->token.c_str());
                        client_it->connection.Close();
                        user->clients.erase(client_it);
                        client_it--;
                        continue;
                    }

                    default: {
                        onMessageReceived(*client_it, msg);
                        continue;
                    }
                }
            }
        }

        if (removeUserIfUnlogged(user)) {
            printNumClients();
            break;
        }
    }
}

void Server::pollMessages() {
    pollUsers();
    pollUnlogged();
}

bool Server::authenticate(std::string token) {
    const bool success = mDatabase.authenticateUserToken(token, mServerName);
    if (success) {
        Debug::Log::d(LOG_TAG,
            "%s(): User %s successfully authenticated in server %s",
            __func__, token.c_str(), mServerName.c_str());
    } else {
        Debug::Log::d(LOG_TAG,
            "%s(): Could not authenticate user %s in server %s",
            __func__, token.c_str(), mServerName.c_str());
    }

    return success;
}

bool Server::tryToLogin(std::string token, Client& client) {
    Debug::Log::i(LOG_TAG, "Login attempt with token %s", token.c_str());

    if (mRequireAuthentication && !authenticate(token)) {
        Debug::Log::i(LOG_TAG, "User token %s not registered in this server", token.c_str());
        LoginResponse failedResponse(LoginResponse::LOGIN_FAILED);
        sendMessage(failedResponse, client);
        return false;
    }

    LoginResponse okResponse(LoginResponse::LOGIN_OK);

    for (auto& user : mUsers) {
        if (TextUtils::Equals(user.token, token)) {
            sendMessage(okResponse, client);

            client.user = &user;
            user.clients.emplace_back(client.connection, &user);
            Debug::Log::i(LOG_TAG, "User %s logged in with new client", user.token.c_str());
            onLogin(client);
            return true;
        }
    }

    mUsers.emplace_back(token, client);

    Debug::Log::i(LOG_TAG, "New user %s logged in", token.c_str());
    sendMessage(okResponse, client);
    onLogin(client);
    return true;
}

void Server::sendMessage(const comm::Message& message, const Client& client) {
    const bool serializeOk = message.serialize(mMessageBuffer, BUFFER_SIZE);
    const uint16_t msgSize =  message.getLength();
    if (serializeOk) {
        client.connection.Send(mMessageBuffer, msgSize);
        Debug::Log::v(LOG_TAG, "Sent message of size %d", msgSize);
    } else {
        Debug::Log::v(LOG_TAG,
            "Could not send message because the buffer "
            "was too short to serialize it (%d bytes into %d)", msgSize, BUFFER_SIZE);
    }
}

std::size_t Server::getNumUnloggedConnections() const {
    return mUnloggedConnections.size();
}

std::size_t Server::getNumLoggedConnections() const {
    unsigned int count = 0;
    for (auto& user : mUsers) {
        count += user.clients.size();
    }
    return count;
}

void Server::printNumClients() const {
    const std::size_t numLogged = getNumLoggedConnections();
    const std::size_t numUnlogged = getNumUnloggedConnections();
    Debug::Log::i(LOG_TAG,
        "%u connections (%u logged, %u unlogged), %u users",
        numUnlogged + numLogged,
        numLogged,
        numUnlogged,
        mUsers.size());
}

Server::LoginResponse::LoginResponse(Response response)
:   comm::Message(comm::ServerMessageTypes::LOGIN, (uint8_t[]) {response}, sizeof(Response))
{
}

}  // namespace server
