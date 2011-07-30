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

#ifndef CODE_GENERATOR_H_
#define CODE_GENERATOR_H_

#include <string>

#include <event.h>

//! Interface for a code generator.
class CodeGenerator {

public:

  virtual ~CodeGenerator() {}
  virtual wstring getWritableCode() const = 0;

  virtual void genTransferStart(const Event &) = 0;
  virtual void genInterchunkStart(const Event &) = 0;
  virtual void genPostchunkStart(const Event &) = 0;
  virtual void genDefMacroStart(const Event &) = 0;
  virtual void genDefMacroEnd(const Event &) = 0;
  virtual void genCallMacroStart(const Event &) = 0;
  virtual void genCallMacroEnd(const Event &) = 0;

private:

};

#endif /* CODE_GENERATOR_H_ */
