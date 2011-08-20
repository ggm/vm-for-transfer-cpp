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

#ifndef VM_WSTRING_UTILS_H_
#define VM_WSTRING_UTILS_H_

#include <string>

using namespace std;

/// Defines the different string cases needed by the vm instructions.
enum CASE {
  AA, Aa, aa
};

/// Some wide string utilities needed by some components of the vm.
class VMWstringUtils {

public:

  static bool iswlower(const wstring &);
  static bool iswnumeric(const wstring &);
  static wstring wtolower(const wstring &);
  static wstring wtoupper(const wstring &);
  static CASE getCase(const wstring &);
  static wstring changeCase(const wstring &, CASE);
  static wstring changeCase(const wstring &, const wstring &);
  static wstring replace(wstring &, const wstring &, const wstring &);

private:

};

#endif /* VM_WSTRING_UTILS_H_ */
