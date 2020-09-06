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

/** Server classes */
namespace server {

/**
 * \brief Basic server functionality like handling login requests, automatic logout of
 *        idle users and dispatching incoming messages from connected clients.
 */
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
        int64_t lastActiveTime = getCurrentTime();
        User* user = nullptr;

        Client(const net::Connection connection, User* user)
        :   connection(connection), user(user) { }

        Client(const net::Connection connection)
        :   connection(connection) { }
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
    virtual bool authenticate(const UserToken userToken) = 0;

    /**
     * \brief Called when a user logs in.
     * \param client A reference to the client.
     */
    virtual void onLogin(Client& client) = 0;

    /**
     * \brief Called when a message is received.
     * \param client The client that sent the message.
     * \param buffer A buffer that contains the message.
     */
    virtual void onMessageReceived(Client& client, const uint8_t *const buffer) = 0;

private:
    volatile bool mRunning = false;

    net::ServerSocket mServerSocket;
    std::vector<User> mUsers;
    std::deque<Client> mUnloggedConnections;

    /**
     * \brief Listen for incoming connections and accept clients. It adds new clients to
     *        the mUnloggedConnections deque, waiting until they successfully log in to the server.
     */
    void listenForConnections();

    /**
     * \brief Removes idle clients from the server.
     *        An client is considered idle when no messages are received from it in a
     *        predetermined amount of time.
     */
    void removeIdleClients();

    /**
     * \brief Calls removeIdleClients periodically. This method is called in a separate thread.
     */
    void loopRemoveIdleClients();

    /**
     * \brief Try to log in a user token. If the user token is sucessfully authenticated, the
     *        client that sent the login request is added to the list of users.
     * \param token User token.
     * \param client The client that sent the login request.
     * \return true if the token was authenticated, false otherwise.
     */
    bool login(const UserToken token, Client& client);

    /**
     * \brief Returns the current time.
     * \return Current timestamp in seconds from epoch.
     */
    static int64_t getCurrentTime();
};

}  //namespace server

#endif  // _INCLUDE_SERVER_HPP_
