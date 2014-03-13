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

#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <unordered_map>
#include <string>

#include <symbol.h>

using namespace std;

/// Represents a collection of symbols.
class SymbolTable {

public:

  SymbolTable();
  SymbolTable(const SymbolTable&);
  ~SymbolTable();
  SymbolTable& operator=(const SymbolTable&);
  void copy(const SymbolTable&);

  void addMacro(wstring, int);
  bool macroExists(wstring) const;
  Symbol getMacro(wstring) const;

private:
  /// Store symbols in a unordered_map with their name as key.
  unordered_map<wstring, Symbol> symbols;

  /// Each symbol has a unique id (order by appearance on the rules files).
  int nextId;

  void addSymbol(wstring, int, SYMBOL_TYPE);
};

#endif /* SYMBOL_TABLE_H_ */
