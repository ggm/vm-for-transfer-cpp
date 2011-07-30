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

#include <symbol_table.h>

#include <sstream>
#include <iostream>

#include <compiler_exception.h>

SymbolTable::SymbolTable() {
  nextId = 0;
}

SymbolTable::SymbolTable(const SymbolTable &s) {
  copy(s);
}

SymbolTable::~SymbolTable() {

}

SymbolTable& SymbolTable::operator=(const SymbolTable &s) {
  if (this != &s) {
    this->~SymbolTable();
    this->copy(s);
  }
  return *this;
}

void SymbolTable::copy(const SymbolTable &s) {
  nextId = s.nextId;
  symbols = s.symbols;
}

/**
 * Add a macro to the symbol table.
 *
 * @param name the name of the macro
 * @param numParams the number of parameters
 */
void SymbolTable::addMacro(wstring name, int numParams) {
  if (macroExists(name)) {
    wstringstream msg;
    msg << L"Macro '" << name << "' already defined";
    throw CompilerException(msg.str());
  }
  addSymbol(name, numParams, MACRO);
}

/**
 * Check if a macro exists in the symbol table.
 *
 * @name the name of the macro to check
 *
 * @return true if exists, otherwise false
 */
bool SymbolTable::macroExists(wstring name) const {
  map<wstring, Symbol>::const_iterator it;

  it = symbols.find(name);
  if (it != symbols.end()) {
    return true;
  } else {
    return false;
  }
}

/**
 * Get a macro from the symbol table.
 *
 * @name the name of the macro to get
 *
 * @return the symbol from the symbol table or an empty one if it isn't found
 */
Symbol SymbolTable::getMacro(wstring name) const {
  map<wstring, Symbol>::const_iterator it;

  it = symbols.find(name);
  if (it != symbols.end()) {
    return it->second;
  } else {
    Symbol s;
    return s;
  }
}

/**
 * Add a symbol to the symbol table, it will assign a unique id.
 *
 * @name the name of the symbol
 * @numParams the number of parameters of the symbol
 * @type the symbol type
 */
void SymbolTable::addSymbol(wstring name, int numParams, SYMBOL_TYPE type) {
  Symbol s(nextId, name, numParams, type);
  nextId++;
  symbols[name] = s;
}
