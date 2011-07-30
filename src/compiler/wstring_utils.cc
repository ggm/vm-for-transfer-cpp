/*Copyright (C) 2011  Gabriel Gregori Manzano

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <wstring_utils.h>

#include <lttoolbox/xml_parse_util.h>

/**
 * Convert a character sequence used by xml parser to a wide string.
 *
 * @param input the character sequence to convert
 *
 * @return the resulting wide string
 */
wstring WstringUtils::towstring(const xmlChar *input) {
  return XMLParseUtil::towstring(input);
}

/**
 * Convert a string to a wide string.
 *
 * @param the string to convert
 *
 * @return the resulting wide string
 */
wstring WstringUtils::stows(const string &str) {
  return XMLParseUtil::stows(str);
}
