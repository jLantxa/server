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

#ifndef _INCLUDE_DATABASE_HPP_
#define _INCLUDE_DATABASE_HPP_

#include <cstdint>

#include <sqlite3.h>

namespace server {

/**
 * \brief Base class for a database handler.
 */
class Database {
public:
    virtual ~Database();

    static Database& getInstance();

    /**
     * \brief Check if a user token exists in the user database.
     * \
     * \return true if the token is registered in the database, false otherwise.
     */
    bool authenticateUserToken(const char* token, const char* serverName) const;

protected:
    sqlite3* mDb;

private:
    Database();

    /**
     * \brief Create table of registered users
     */
    void createUserTable();
};

}  // namespace server

#endif  // _INCLUDE_DATABASE_HPP_
