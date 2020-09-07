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

using UserToken = uint64_t;

/**
 * \brief Base class for a database handler.
 */
class Database {
public:
    Database();
    ~Database();

    /**
     * \brief Check if a user token exists in the user database.
     * \return true if the token is registered in the database, false otherwise.
     */
    virtual bool authenticateUserToken(const UserToken token);

    /**
     * \brief Add a user to the database. Ignore if the user already exists.
     * \param token User token to be added.
     */
    virtual void addUser(const UserToken token);

    /**
     * \brief Delete user from the database. Ignore if the user does not exist in the database.
     * \param token User token to be deleted.
     */
    virtual void deleteUser(const UserToken token);

protected:
    sqlite3* mDb;

private:
    /**
     * \brief Create table of registered users
     */
    void createUserTable();
};

}  // namespace server

#endif  // _INCLUDE_DATABASE_HPP_
