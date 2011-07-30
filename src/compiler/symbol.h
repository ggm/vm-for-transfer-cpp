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

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <string>

using namespace std;

enum SYMBOL_TYPE {
  MACRO
};

/// Stores all the necessary information about symbols.
class Symbol {

public:

  Symbol();
  Symbol(int, wstring, int, SYMBOL_TYPE);
  Symbol(const Symbol&);
  ~Symbol();
  Symbol& operator=(const Symbol&);
  void copy(const Symbol&);

  int getId() const;
  wstring getName() const;
  int getNumParameters() const;
  SYMBOL_TYPE getSymbolType() const;
  wstring getTypeAsWstring() const;

private:
  /// Each symbol has a unique id.
  int id;

  /// Name of the symbol.
  wstring name;

  /// Number of parameters if applies.
  int numParams;

  /// The type of the symbol.
  SYMBOL_TYPE type;
};

#endif /* SYMBOL_H_ */
