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

#include <vector>

#include <sqlite3.h>

#include "debug.hpp"

#include "NotificationServer/NotificationServer.hpp"

static __attribute_used__ const char* LOG_TAG = "NotificationDatabase";

void NotificationDatabase::init() {
    createNotificationTable();
}

void NotificationDatabase::createNotificationTable() {
    Debug::Log::d(LOG_TAG, "%s()", __func__);

    static const char* SQL_CREATE_USER_TABLE =
    "CREATE TABLE IF NOT EXISTS Notifications ("
        "id INT PRIMARY KEY NOT NULL, "
        "user TEXT NOT NULL, "
        "active INT NOT NULL, "
        "title TEXT NOT NULL, "
        "description TEXT, "
        "schedule TEXT NOT NULL"
    ");";

    char *zErrMsg = 0;
    const int result = sqlite3_exec(mDb, SQL_CREATE_USER_TABLE, nullptr, nullptr, &zErrMsg);

    if(result != SQLITE_OK){
        Debug::Log::e(LOG_TAG, "%s(): SQL error: %s", __func__, zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

std::vector<Notification> NotificationDatabase::getNotificationsFromUser(std::string userToken)
{
    Debug::Log::d(LOG_TAG, "%s()", __func__);
    std::vector<Notification> notifications;

    char sql[256];
    static const char* SQL_SELECT_NOTIFICATIONS =
        "SELECT * FROM Notifications "
        "WHERE user = '%s' AND active = '1';";

    sprintf(sql, SQL_SELECT_NOTIFICATIONS, userToken.c_str());

    const auto callback = [](void* notifications, int argc, char** argv, char** azColName) -> int {
        (void) argc;
        (void) azColName;

        struct Notification notification {
            atoi(argv[0]),                      // id
            static_cast<bool>(atoi(argv[2])),   // active
            std::string(argv[3]),               // title
            std::string(argv[4]),               // description
            std::string(argv[5])                // schedule
        };

        static_cast<std::vector<Notification>*>(notifications)->emplace_back(notification);

        return 0;
    };

    char *zErrMsg = 0;
    const int rc = sqlite3_exec(mDb, sql, callback, &notifications, &zErrMsg);
    if (rc != SQLITE_OK) {
        Debug::Log::e(LOG_TAG, "%s():%d SQL error: %s", __func__, __LINE__, zErrMsg);
        sqlite3_free(zErrMsg);
        return notifications;
    }

    return notifications;
}
