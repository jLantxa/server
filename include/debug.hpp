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

#ifndef _INCLUDE_DEBUG_HPP_
#define _INCLUDE_DEBUG_HPP_

#include <cstdio>

namespace Debug
{

#ifdef DEBUG_LEVEL
    static constexpr int defined_level = DEBUG_LEVEL;
#else
    static constexpr int defined_level = 0;
#endif

    template <typename... Args>
    static void log(const char* tag, const char* levelTag, const char* fmt, Args... args) {
        printf("[%s] %s: ", levelTag, tag);
        printf(fmt, args...);
        printf("\n");
    }

    template <typename... Args>
    static void log(const char* tag, const char* levelTag, const char* str) {
        log(tag, levelTag, "%s", str);
    }

#define LOG_FUNCTION(name, level, level_tag) \
        template <typename... Args> \
            static inline void name(const char* tag, Args... args) { \
                if constexpr (defined_level >= level) { \
                    log(tag, level_tag, args...); \
                } \
            }

    // Visible debug functions
    class Log {
    public:
        LOG_FUNCTION(e, -2, "ERROR");
        LOG_FUNCTION(w, -1, "WARNING");
        LOG_FUNCTION(i,  0, "INFO");
        LOG_FUNCTION(d,  1, "DEBUG");
        LOG_FUNCTION(v,  2, "VERBOSE");
    };
}

#endif  // _INCLUDE_DEBUG_HPP_
