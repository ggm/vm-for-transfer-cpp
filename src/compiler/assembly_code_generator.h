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

#ifndef ASSEMBLY_CODE_GENERATOR_H_
#define ASSEMBLY_CODE_GENERATOR_H_

#include <string>
#include <sstream>
#include <vector>

#include <event.h>
#include <code_generator.h>

using namespace std;

//Define the instruction set a set of constants specific to this code generator.
static const wstring INSTR_SEP = L" ";

static const wstring ADDTRIE_OP = L"addtrie";
static const wstring AND_OP = L"and";
static const wstring APPEND_OP = L"append";
static const wstring BEGINS_WITH_OP = L"begins-with";
static const wstring BEGINS_WITH_IG_OP = L"begins-with-ig";
static const wstring OR_OP = L"or";
static const wstring CALL_OP = L"call";
static const wstring CASE_OF_OP = L"case-of";
static const wstring CMP_SUBSTR_OP = L"cmp-substr";
static const wstring CMPI_SUBSTR_OP = L"cmpi-substr";
static const wstring CLIP_OP = L"clip";
static const wstring CLIPSL_OP = L"clipsl";
static const wstring CLIPTL_OP = L"cliptl";
static const wstring CMP_OP = L"cmp";
static const wstring CMPI_OP = L"cmpi";
static const wstring CONCAT_OP = L"concat";
static const wstring CHUNK_OP = L"chunk";
static const wstring ENDS_WITH_OP = L"ends-with";
static const wstring ENDS_WITH_IG_OP = L"ends-with-ig";
static const wstring GET_CASE_FROM_OP = L"get-case-from";
static const wstring IN_OP = L"in";
static const wstring INIG_OP = L"inig";
static const wstring JMP_OP = L"jmp";
static const wstring JZ_OP = L"jz";
static const wstring MLU_OP = L"mlu";
static const wstring MODIFY_CASE_OP = L"modify-case";
static const wstring PUSH_OP = L"push";
static const wstring PUSHBL_OP = L"pushbl";
static const wstring PUSHSB_OP = L"pushsb";
static const wstring LU_OP = L"lu";
static const wstring LU_COUNT_OP = L"lu-count";
static const wstring NOT_OP = L"not";
static const wstring OUT_OP = L"out";
static const wstring RET_OP = L"ret";
static const wstring STORECL_OP = L"storecl";
static const wstring STORESL_OP = L"storesl";
static const wstring STORETL_OP = L"storetl";
static const wstring STOREV_OP = L"storev";

/// This class generates code as a predefined pseudo-assembly.
class AssemblyCodeGenerator: public CodeGenerator {

public:

  AssemblyCodeGenerator();
  AssemblyCodeGenerator(const AssemblyCodeGenerator&);
  virtual ~AssemblyCodeGenerator();
  AssemblyCodeGenerator& operator=(const AssemblyCodeGenerator&);
  void copy(const AssemblyCodeGenerator&);

  void addCode(const wstring &);
  wstring getWritableCode() const;
  void genHeader(const Event &);

  void genTransferStart(const Event &);
  void genInterchunkStart(const Event &);
  void genPostchunkStart(const Event &);
  void genDefVarStart(const Event &, const wstring &);
  void genSectionDefMacrosStart(const Event &);
  void genDefMacroStart(const Event &);
  void genDefMacroEnd(const Event &);
  void genCallMacroStart(const Event &);
  void genCallMacroEnd(const Event &);

private:
  /// Used to get the next address of an instruction if needed.
  int nextAddress;

  /// The assembly code generated.
  vector<wstring> code;
};

#endif /* ASSEMBLY_CODE_GENERATOR_H_ */
