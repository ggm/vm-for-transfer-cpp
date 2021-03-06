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

#ifndef WSTRING_UTILS_H_
#define WSTRING_UTILS_H_

#include <string>
#include <vector>

#include <libxml/xmlreader.h>

using namespace std;

/// Abstraction of xml_utils found in lttoolbox and some utils added.
class WstringUtils {

public:
  static wstring replace(wstring &, const wstring &, const wstring &);
  static wstring towstring(const xmlChar *);
  static wstring stows(const string &);
  static vector<wstring> wsplit(const wstring &, const wchar_t&);
};

#endif /* WSTRING_UTILS_H_ */
