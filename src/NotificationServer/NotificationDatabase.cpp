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
        "json BLOB NOT NULL"
    ");";

    char *zErrMsg = 0;
    const int result = sqlite3_exec(mDb, SQL_CREATE_USER_TABLE, nullptr, nullptr, &zErrMsg);

    if(result != SQLITE_OK){
        Debug::Log::e(LOG_TAG, "%s(): SQL error: %s", __func__, zErrMsg);
        sqlite3_free(zErrMsg);
    }
}
