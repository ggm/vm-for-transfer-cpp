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

#ifndef ASSEMBLY_LOADER_H_
#define ASSEMBLY_LOADER_H_

#include <string>
#include <unordered_map>

#include "loader.h"
#include "instructions.h"
#include "scope.h"

using namespace std;

/**
 * Reads and loads assembly instruction from a file and converts them to
 * the format used by the VM. This conversion is done directly for the code
 * and preprocess sections but rules and macros are only preloaded. This means
 * that the first time, and not until then, a rule or a macro is called, the
 * loader will convert them to the internal vm representation and process them
 * properly.
 */
class AssemblyLoader: public Loader {

public:

  AssemblyLoader();
  AssemblyLoader(char *);
  AssemblyLoader(const AssemblyLoader&);
  virtual ~AssemblyLoader();
  AssemblyLoader& operator=(const AssemblyLoader&);
  void copy(const AssemblyLoader&);

  void load(CodeUnit &, CodeUnit &, CodeSection &, CodeSection &,
      unsigned int &);
  void loadCodeUnit(CodeUnit &);

  void printCodeSection(const CodeSection &, const wstring &, const wstring &);
  void printCodeUnit(const CodeUnit &, const wstring &);
  void printInstruction(const Instruction &, unsigned int);

private:
  /// Name of the code file to use.
  char *codeFileName;

  /// Map with the assembly representation as key and the vm opcode as value.
  unordered_map<wstring, OP_CODE> opCodes;

  /// Reversed opcodes unordered_map, only used when debugging is activated.
  unordered_map<OP_CODE, wstring, hash<int>> reversedOpCodes;

  /// The current line number is used in the error messages.
  unsigned int currentLineNumber;

  /// Labels have different scopes: code, rules, macros...
  vector<Scope> scopes;

  /// Store the scope of the current code as it is being loaded.
  Scope *currentScope;

  /// Macros' names are converted to a unique identifier.
  unordered_map<wstring, wstring> macroNumber;

  /// Reversed macro numbers unordered_map only used for debugging purposes.
  unordered_map<wstring, wstring> reversedMacroNumber;

  /// Each macro name needs a unique number.
  unsigned int nextMacroNumber;

  void loadCodeSection(wfstream &, CodeUnit &);
  void addInstructionToCodeUnit(Instruction, CodeUnit&);
  void createNewScope();
  void deleteCurrentScope();
  wstring getRuleNumber(const wstring &) const;
  wstring getNextMacroNumber();
  wstring getMacroName(const wstring &) const;
  bool getInternalRepresentation(const wstring &, CodeUnit &, Instruction &);
  void throwError(const wstring &) const;

  bool startsWith(const wstring &, const wstring &) const;
  bool endsWith(const wstring &, const wstring &) const;
  void createReversedOpCodesMap();
  wstring getMacroNameFromNumber(const wstring &);
};

#endif /* ASSEMBLY_LOADER_H_ */
