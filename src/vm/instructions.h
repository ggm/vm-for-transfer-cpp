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

#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include <string>
#include <vector>

using namespace std;

/**
 * This enum serves as a change only once place and contains the internal
 * representation of the instructions used by the vm. All the others loaders and
 * interpreters should used this module to become immune to changes.
 */
enum OP_CODE {
  ADDTRIE = 0, AND, APPEND, BEGINS_WITH, BEGINS_WITH_IG, OR, CALL, CLIP, CLIPSL,
  CLIPTL, CMP_SUBSTR, CMPI_SUBSTR, CMP, CMPI, CONCAT, CHUNK, ENDS_WITH,
  ENDS_WITH_IG, GET_CASE_FROM, IN, INIG, JMP, JZ, JNZ, MLU, MODIFY_CASE,
  PUSHBL, PUSHSB, LU, LU_COUNT, NOT, OUT, RET, STORECL, STORESL, STORETL,
  STOREV, CASE_OF, PUSH_STR, PUSH_INT, PUSH_VAR
};

/// A struct representing a instruction as an opcode and an operand.
struct Instruction {
  OP_CODE opCode;
  wstring op1;
  int lineNumber = -1;
  int intOp1;
};

/// A code unit is a collection of instructions like the instructions of a rule.
struct CodeUnit {
  bool loaded;
  vector<Instruction> code;
};

/// A code section is a collection of code units like all rules of a file.
struct CodeSection {
  vector<CodeUnit> units;
};

#endif /* INSTRUCTIONS_H_ */
