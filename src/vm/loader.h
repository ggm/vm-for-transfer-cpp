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

#ifndef LOADER_H_
#define LOADER_H_

#include <vector>

#include "instructions.h"

/// Interface for a code loader.
class Loader {

public:

  virtual ~Loader() { }

  virtual void load(CodeUnit &, CodeUnit &, CodeSection &, CodeSection &,
      unsigned int &) = 0;
  virtual void loadCodeUnit(CodeUnit &) = 0;
  virtual void printCodeSection(const CodeSection &, const wstring &,
      const wstring &) = 0;
  virtual void printCodeUnit(const CodeUnit &, const wstring &) = 0;
  virtual void printInstruction(const Instruction &, unsigned int) = 0;

private:

};

#endif /* LOADER_H_ */
