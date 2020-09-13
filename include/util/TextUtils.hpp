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

#ifndef _INCLUDE_TEXT_UTILS_HPP_
#define _INCLUDE_TEXT_UTILS_HPP_

#include <string>
#include <vector>

/** Utility functions for manipulating text. */
namespace TextUtils {

/** Compare two strings and check whether they are the same.
 *
 * \param str1 First string
 * \param str2 Second string
 * \return true if the contents of both strings are the same
*/
bool Equals(const std::string& str1, const std::string& str2);

/** \brief Split a string into substrings using another string as a delimiter.
 *
 * The substrings are returned in a std:vector<std::string>.
 * <br>
 * Example: Split("127.0.0.1", ".") -> {"127", "0", "0", "1"}
 *
 * \param str String
 * \param str Delimiter string
 * \return std::vector containing the substrings
*/
std::vector<std::string> Split(const std::string& str, const std::string& delimiter);

}  // namespace TextUtils

#endif  // _INCLUDE_TEXT_UTILS_HPP_
