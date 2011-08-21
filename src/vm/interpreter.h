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

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <string>

#include "vm_exceptions.h"
#include "instructions.h"
#include "lexical_unit.h"
#include "chunk_lexical_unit.h"
#include "vm_wstring_utils.h"

using namespace std;

class VM;

/// Interprets an op code and executes the appropriate instruction.
class Interpreter {

public:

  /// Define a constant to represent false as a return value of an instruction.
  static const wstring FALSE_WSTR; // = L"0" defined in the .cc

  /// Define a constant to represent true as a return value of an instruction.
  static const wstring TRUE_WSTR; // = L"1" defined in the .cc

  Interpreter();
  Interpreter(VM *);
  Interpreter(const Interpreter &);
  ~Interpreter();
  Interpreter& operator=(const Interpreter &);
  void copy(const Interpreter &);

  void preprocess();
  void execute(const Instruction&);

private:

  /// Access to the vm data structures is needed.
  VM *vm;

  /// Track if the last executed instruction modified the PC.
  bool modifiedPC;

  void throwError(const wstring &);
  void modifyPC(int);
  LexicalUnit* getSourceLexicalUnit(int);
  LexicalUnit* getTargetLexicalUnit(int);
  vector<wstring> getOperands(const Instruction &);
  vector<int> getNOperands(const Instruction &, const wstring &);
  wstring popSystemStack();
  int popSystemStackInteger();
  void pushCaseToStack(CASE);

  void executeAddtrie(const Instruction&);
  void executeAnd(const Instruction&);
  void executeOr(const Instruction&);
  void executeNot(const Instruction&);
  void executeAppend(const Instruction&);
  bool beginsWith(const wstring &, const wstring &) const;
  void executeBeginsWith(const Instruction&);
  void executeBeginsWithIg(const Instruction&);
  void executeCall(const Instruction&);
  void executeRet(const Instruction&);
  void executeClip(const Instruction&);
  void executeClipsl(const Instruction&);
  void executeCliptl(const Instruction&);
  void handleClipInstruction(const wstring &, LexicalUnit*, const wstring &,
      const wstring &);
  void executeCmp(const Instruction&);
  void executeCmpi(const Instruction&);
  void executeCmpSubstr(const Instruction&);
  void executeCmpiSubstr(const Instruction&);
  void executeIn(const Instruction&);
  void executeInig(const Instruction&);
  void searchValueInList(const wstring &, const wstring &);
  void executeConcat(const Instruction&);
  void executeChunk(const Instruction&);
  bool endsWith(const wstring &, const wstring &) const;
  void executeEndsWith(const Instruction&);
  void executeEndsWithIg(const Instruction&);
  void executeJmp(const Instruction&);
  void executeJz(const Instruction&);
  void executeJnz(const Instruction&);
  void executeLu(const Instruction&);
  void executeLuCount(const Instruction&);
  void executeMlu(const Instruction&);
  void executeCaseOf(const Instruction&);
  void executeGetCaseFrom(const Instruction&);
  void executeModifyCase(const Instruction&);
  void executeOut(const Instruction&);
  void executePush(const Instruction&);
  void executePushbl(const Instruction&);
  void executePushsb(const Instruction&);
  void executeStorecl(const Instruction&);
  void executeStoresl(const Instruction&);
  void executeStoretl(const Instruction&);
  void handleStoreClipInstruction(const wstring &, LexicalUnit*,
      const wstring &, const wstring &);
  void executeStorev(const Instruction&);

};

#endif /* INTERPRETER_H_ */
