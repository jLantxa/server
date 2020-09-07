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

#include <array>
#include <string>

#include <sqlite3.h>

#include "Database.hpp"
#include "debug.hpp"

static __attribute_used__ const char* LOG_TAG = "Database";

namespace server {

static const char* SQL_CREATE_USER_TABLE =
    "CREATE TABLE IF NOT EXISTS Users"
    "("
        "UserToken INT PRIMARY KEY NOT NULL,"
        "Notification BOOL NOT NULL"
    ");";

Database::Database() {
   const int result = sqlite3_open("server.db", &mDb);

   if(result) {
      Debug::Log::e(LOG_TAG, "%s(): Can't open database: %s", __func__, sqlite3_errmsg(mDb));
      return;
   } else {
      Debug::Log::e(LOG_TAG, "%s(): Opened database", __func__);
   }

    createUserTable();
}

Database::~Database() {
    sqlite3_close(mDb);
}

void Database::createUserTable() {
    Debug::Log::d(LOG_TAG, "%s()", __func__);

    char *zErrMsg = 0;
    const int result = sqlite3_exec(mDb, SQL_CREATE_USER_TABLE, nullptr, nullptr, &zErrMsg);

    if(result != SQLITE_OK){
        Debug::Log::e(LOG_TAG, "%s(): SQL error: %s", __func__, zErrMsg);
        sqlite3_free(zErrMsg);
   }
}

void Database::addUser(const UserToken token) {
    // TODO
    (void) token;
}

void Database::deleteUser(const UserToken token) {
    // TODO
    (void) token;
}

bool Database::authenticateUserToken(const UserToken token) {
    // TODO
    (void) token;
    return false;
}

}  // namespace server
