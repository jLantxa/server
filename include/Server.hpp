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

#include <chrono>
#include <string>
#include <vector>

#include <Communication.hpp>
#include "Database.hpp"
#include "net/Socket.hpp"

/** Server classes */
namespace server {

static constexpr uint16_t BUFFER_SIZE = 1024;

/**
 * \brief Basic server functionality like handling login requests, automatic logout of
 *        idle users and dispatching incoming messages from connected clients.
 */
class Server {
public:
    Server(const char* serverName, const uint16_t port);
    virtual ~Server() = default;

    void run();
    const char* getName() const;

protected:
    using BufferSize = uint16_t;

    struct User;
    struct Client;

    struct Client final {
        net::Connection connection;
        int64_t lastActiveTime;
        User* user = nullptr;

        Client(const net::Connection connection, User* user)
        :   connection(connection), user(user)
        {
            refreshTime();
        }

        Client(const net::Connection connection) : Client(connection, nullptr) { }

        void refreshTime() {
            lastActiveTime = getCurrentTime();
        }

        bool isLogged() {
            return (user != nullptr);
        }
    };

    struct User final {
        UserToken token;
        std::vector<Client> clients;

        User(const UserToken token) : token(token) { }
    };

    /**
     * \brief Authenticates a user token.
     * \param userToken User token.
     * \return true if this user token is registered in this server, false otherwise.
     */
    bool authenticate(const UserToken userToken);

    /**
     * \brief Called when a user logs in.
     * \param client A reference to the client.
     */
    virtual void onLogin(Client& client) = 0;

    /**
     * \brief Called when a message is received.
     * \param client The client that sent the message.
     * \param message The received message.
     */
    virtual void onMessageReceived(Client& client, const comm::Message& message) = 0;

    virtual void sendMessage(const comm::Message& message, const Client& client);

private:
    static constexpr auto REMOVE_IDLE_PERIOD = std::chrono::seconds(30);
    static constexpr int32_t CLIENT_MAX_IDLE_TIMEOUT = 300;
    static constexpr auto HANDLE_MESSAGES_PERIOD = std::chrono::milliseconds(100);

    const char* mServerName;
    volatile bool mRunning = false;

    net::ServerSocket mServerSocket;
    std::vector<User> mUsers;
    std::vector<Client> mUnloggedConnections;

    uint8_t mMessageBuffer[BUFFER_SIZE];

    /**
     * \brief Removes idle clients from the server.
     *        An client is considered idle when no messages are received from it in a
     *        predetermined amount of time.
     */
    void removeIdleClients();

    /**
     * \brief Read incoming messages from logged and unlogged clients and handle them.
     *        Messages coming from logged clients are relayed to the server implementation using
     *        the onMessageReceived() interface.
     *        Messages coming from unlogged clients are checked for login only.
     */
    void pollMessages();

    /**
     * \brief Processes a received message as a login request. If the message is a
     *        login request, attempt to log in.
     * \param buffer The buffer received.
     * \param size Size of the buffer in bytes.
     */
    void handleLogin(Client& client, const comm::Message& message, bool logged);

    /**
     * \brief Try to log in a user token. If the user token is sucessfully authenticated, the
     *        client that sent the login request is added to the list of users.
     * \param token User token.
     * \param client The client that sent the login request.
     * \return true if the token was authenticated, false otherwise.
     */
    bool tryToLogin(const UserToken token, Client& client);

    /**
     * \brief Returns the current time.
     * \return Current timestamp in seconds from epoch.
     */
    static int64_t getCurrentTime();

    unsigned int getNumUnloggedConnections() const;

    unsigned int getNumLoggedConnections() const;

    void printNumClients() const ;

    class LoginResponse : public comm::Message {
    public:
        enum Response: uint8_t {
            LOGIN_OK = 0,
            LOGIN_FAILED = 1,
            ALREADY_LOGGED_IN = 2,
        };

        LoginResponse(Response response);
    };
};

}  //namespace server

#endif  // _INCLUDE_SERVER_HPP_
