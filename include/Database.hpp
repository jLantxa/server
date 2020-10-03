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

#include <string>

#include <sqlite3.h>

namespace server {

class DatabaseManager;

/**
 * \brief Base class for a database handler.
 */
class Database {
public:
    /**
     * \brief Check if a user token exists in the user database.
     * \
     * \return true if the token is registered in the database, false otherwise.
     */
    bool authenticateUserToken(std::string token, std::string serverName) const;

protected:
    sqlite3* mDb;

    /**
     * \brief Initialise database.
     */
    virtual void init();

private:
    /**
     * \brief Create table of registered users
     */
    void createUserTable();

    /**
     * \brief Set the sqlite* database.
     * \param db Initialised sqlite*.
     */
    void setDb(sqlite3* db);

    friend DatabaseManager;
};

class DatabaseManager final {
public:
    ~DatabaseManager();

    /**
     * \brief Get a instance of the DatabaseManager.
     * \return Reference to the DatabaseManager singleton.
     **/
    static DatabaseManager& getInstance();

    /**
     * \brief Initialise a database
     * \param database Reference to an uninitialised database.
     */
    void initDatabase(Database& database);

private:
    DatabaseManager();

    sqlite3* mDb;
};

}  // namespace server

#endif  // _INCLUDE_DATABASE_HPP_
