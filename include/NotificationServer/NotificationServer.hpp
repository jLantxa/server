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

#ifndef _INCLUDE_NOTIFICATION_SERVER_NOTIFICATION_SERVER_HPP_
#define _INCLUDE_NOTIFICATION_SERVER_NOTIFICATION_SERVER_HPP_

#include "NotificationServer/NotificationDatabase.hpp"

#include "Server.hpp"

/**
 * \brief Notification server.
 *        This server sends scheduled notifications to registered clients.
*/
class NotificationServer final : public server::Server {
public:
    NotificationServer(const uint16_t port);
    virtual ~NotificationServer() = default;

    enum MessageTypes : server::comm::MessageType {
        REQUEST_TASKS  = 0x10,
        RESPONSE_TASKS = 0x11,
    };

private:
    void onLogin(Client& client) override;
    void onMessageReceived(Client& client, const server::comm::Message& message) override;

    NotificationDatabase mNotificationDb;

    void sendNotification(const Notification& notification, Client& client);
};

#endif  // _INCLUDE_NOTIFICATION_SERVER_NOTIFICATION_SERVER_HPP_
