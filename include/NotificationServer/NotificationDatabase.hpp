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

#ifndef _INCLUDE_NOTIFICATION_DATABASE_HPP_
#define _INCLUDE_NOTIFICATION_DATABASE_HPP_

#include <cstdint>

#include <sqlite3.h>

#include "Database.hpp"

class NotificationDatabase : public server::Database {
public:
    void init() override;

private:
    /**
     * \brief Create table of registered users
     */
    void createNotificationTable();
};

#endif  // _INCLUDE_NOTIFICATION_DATABASE_HPP_
