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

#include <symbol.h>

Symbol::Symbol() {
  id = -1;
  name = L"";
  numParams = -1;
}

Symbol::Symbol(int id, wstring name, int numParams, SYMBOL_TYPE type) {
  this->id = id;
  this->name = name;
  this->numParams = numParams;
  this->type = type;
}

Symbol::Symbol(const Symbol &s) {
  copy(s);
}

Symbol::~Symbol() {

}

Symbol& Symbol::operator=(const Symbol &s) {
  if (this != &s) {
    this->~Symbol();
    this->copy(s);
  }
  return *this;
}

void Symbol::copy(const Symbol &s) {
  id = s.id;
  name = s.name;
  numParams = s.numParams;
  type = s.type;
}

/**
 * Get the symbol's id.
 *
 * @return id the unique id of the symbol
 */
int Symbol::getId() const {
  return id;
}

/**
 * Get the symbol's name.
 *
 * @return the name as a wide string
 */
wstring Symbol::getName() const {
  return name;
}

/**
 * Get the number of parameters of the symbol.
 *
 * @return the number of parameters
 */
int Symbol::getNumParameters() const {
  return numParams;
}

/**
 * Get the symbol's type.
 *
 * @return the type as an enum value
 */
SYMBOL_TYPE Symbol::getSymbolType() const {
  return type;
}

/**
 * Get the symbol's type as a wide string.
 *
 * @return a wide string representing the symbol type
 */
wstring Symbol::getTypeAsWstring() const {
  if (type == MACRO) {
    return L"Macro";
  } else {
    return L"Unrecognized type";
  }
}
