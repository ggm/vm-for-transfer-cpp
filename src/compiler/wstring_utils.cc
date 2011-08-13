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
 * Replace a wide substring with another wide substring.
 *
 * @param wstr the wide string with the content
 * @param replaced the wide string to be replaced
 * @param replacement the wide string to put in the replaced place
 *
 * @return the wide string with the replacement done
 */
wstring WstringUtils::replace(wstring &wstr, const wstring &replaced,
    const wstring &replacement) {
  size_t pos = wstr.find(replaced);

  while (pos != string::npos) {
    wstr.replace(pos, replaced.size(), replacement);
    pos = wstr.find(replaced);
  }

  return wstr;
}

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
 * @param str the string to convert
 *
 * @return the resulting wide string
 */
wstring WstringUtils::stows(const string &str) {
  return XMLParseUtil::stows(str);
}

/**
 * Split a wide string in several words using a delimiter.
 *
 * @param wstr the wide string to split
 * @param delimiter the wide char to use as delimiter
 *
 * @return a vector with a word in each position
 */
vector<wstring> WstringUtils::wsplit(const wstring &wstr,
    const wchar_t &delimiter) {
  vector<wstring> tokens;
  wstring token = L"";
  wchar_t ch;

  for (unsigned int i = 0; i < wstr.length(); i++) {
    ch = wstr[i];
    if (ch == delimiter) {
      tokens.push_back(token);
      token = L"";
    } else {
      token += ch;
    }
  }

  tokens.push_back(token);

  return tokens;
}

