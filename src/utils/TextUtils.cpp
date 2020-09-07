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

#include "utils/TextUtils.hpp"

#include <string>
#include <vector>

namespace TextUtils {

/**
 * \brief Checks if two std::strings are equal in value.
 * \param str1 First string.
 * \param str2 Second string.
 * \return true if str1 and str2 are equal, false otherwise.
 */
bool Equals(const std::string& str1, const std::string& str2) {
    return (str1.compare(str2) == 0);
}

/**
 * \brief Split a string by a delimiter substring
 * \param str String
 * \param delimiter Delimiter
 * \return A vector with the split substrings.
 */
std::vector<std::string> Split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> str_vec;
    const std::size_t str_size = str.size();
    const std::size_t sep_size = delimiter.size();

    if (str_size > 0 && sep_size > 0) {
        std::size_t pos = 0;
        std::size_t found;
        while (pos < str_size) {
            found = str.find(delimiter, pos);
            if (found == std::string::npos) {
                // Not found -> add remaining substring
                str_vec.push_back(str.substr(pos));
                break;
            } else {
                // Found -> add substring
                str_vec.push_back(str.substr(pos, found - pos));
                pos = found + sep_size;
            }
        }

        // Add an empty string if the last character is a separator
        if (Equals(str.substr(str_size - sep_size), delimiter)) {
            str_vec.push_back("");
        }
    }

    return str_vec;
}

}  // namespace TextUtils
