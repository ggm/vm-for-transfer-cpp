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

#include "assembly_loader.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include "vm_exceptions.h"
#include "vm_wstring_utils.h"

using namespace std;

AssemblyLoader::AssemblyLoader() {
  currentLineNumber = 0;
  nextMacroNumber = 0;
}

AssemblyLoader::AssemblyLoader(char *fileName) {
  currentLineNumber = 0;
  nextMacroNumber = 0;
  codeFileName = fileName;

  opCodes[L"addtrie"] = ADDTRIE;      opCodes[L"and"] = AND;
  opCodes[L"append"] = APPEND;        opCodes[L"begins-with"] = BEGINS_WITH;
  opCodes[L"call"] = CALL;            opCodes[L"case-of"] = CASE_OF;
  opCodes[L"cmp"] = CMP;              opCodes[L"cmpi"] = CMPI;
  opCodes[L"clip"] = CLIP;            opCodes[L"clipsl"] = CLIPSL;
  opCodes[L"cliptl"] = CLIPTL;        opCodes[L"concat"] = CONCAT;
  opCodes[L"chunk"] = CHUNK;          opCodes[L"ends-with"] = ENDS_WITH;
  opCodes[L"in"] = IN;                opCodes[L"inig"] = INIG;
  opCodes[L"jmp"] = JMP;              opCodes[L"jz"] = JZ;
  opCodes[L"jnz"] = JNZ;              opCodes[L"mlu"] = MLU;
  opCodes[L"push"] = PUSH;            opCodes[L"pushbl"] = PUSHBL;
  opCodes[L"pushsb"] = PUSHSB;        opCodes[L"lu"] = LU;
  opCodes[L"lu-count"] = LU_COUNT;    opCodes[L"not"] = NOT;
  opCodes[L"out"] = OUT;              opCodes[L"or"] = OR;
  opCodes[L"ret"] = RET;              opCodes[L"storecl"] = STORECL;
  opCodes[L"storesl"] = STORESL;      opCodes[L"storetl"] = STORETL;
  opCodes[L"storev"] = STOREV;

  opCodes[L"begins-with-ig"] = BEGINS_WITH_IG;
  opCodes[L"cmp-substr"] = CMP_SUBSTR;
  opCodes[L"cmpi-substr"] = CMPI_SUBSTR;
  opCodes[L"ends-with-ig"] = ENDS_WITH_IG;
  opCodes[L"get-case-from"] = GET_CASE_FROM;
  opCodes[L"modify-case"] = MODIFY_CASE;

  opCodes[L"push-str"] = PUSH_STR;
  opCodes[L"push-int"] = PUSH_INT;
  opCodes[L"push-var"] = PUSH_VAR;
}

AssemblyLoader::AssemblyLoader(const AssemblyLoader &c) {
  copy(c);
}

AssemblyLoader::~AssemblyLoader() {

}

AssemblyLoader& AssemblyLoader::operator=(const AssemblyLoader &c) {
  if (this != &c) {
    this->~AssemblyLoader();
    this->copy(c);
  }
  return *this;
}

void AssemblyLoader::copy(const AssemblyLoader &c) {
  codeFileName = c.codeFileName;
  opCodes = c.opCodes;
  reversedOpCodes = c.reversedOpCodes;
  currentLineNumber = c.currentLineNumber;
  scopes = c.scopes;
  macroNumber = c.macroNumber;
  reversedMacroNumber = c.reversedMacroNumber;
  nextMacroNumber = c.nextMacroNumber;
}

/**
 * Load an assembly file and transform the instructions to the vm
 * representation, substituting macro or rules names for addresses. As stated
 * in the class description, rules and macros are just preloaded and will be
 * properly loaded the first time they are called.
 *
 * @param preprocessCode the preprocess code section
 * @param code the main code section
 * @param rulesCode the code section containing rules
 * @param macrosCode the code section containing macros
 * @param finalAddress the final address of the main code section of the vm
 */
void AssemblyLoader::load(CodeUnit &preprocessCode, CodeUnit &code,
    CodeSection &rulesCode, CodeSection &macrosCode,
    unsigned int &finalAddress) {
  const wstring patterns = L"patterns";
  const wstring action = L"action";
  const wstring macro = L"macro";
  const wstring start = L"start:";
  const wstring end = L"end:";

  // The default section is the vm "code section" with a default scope.
  createNewScope();
  CodeUnit codeUnit;
  codeUnit.loaded = false;
  bool justPreload = false;

  wfstream file;
  file.open(codeFileName, ios::in);

  loadCodeSection(file, code);

  wstring line = L"";
  unsigned int macroAddr;
  while(getline(file, line, L'\n')) {
    currentLineNumber++;
    // Ignore comments.
    if (line[0] == L'#') {
      continue;
    }

    // Handle the patterns and addtries.
    else if (startsWith(line, patterns)) {
      justPreload = false;

      // At the start, create a code unit for the patterns' code.
      if (endsWith(line, start)) {
        codeUnit.code.clear();
      // At the end, add all the patterns' code to the preprocess section.
      } else if (endsWith(line, end)) {
        codeUnit.loaded = true;
        preprocessCode = codeUnit;
        codeUnit.code.clear();
        justPreload = false;
      }

    // Handle the contents of each rule.
    } else if (startsWith(line, action)) {
      justPreload = true;

      // At the start, clear the codeUnit which will contain the rule's code.
      if (endsWith(line, start)) {
        codeUnit.code.clear();
      // At the end, create an entry on the rules section with the code.
      } else if (endsWith(line, end)) {
        unsigned int ruleNumber = VMWstringUtils::stringTo<unsigned int>(
            getRuleNumber(line));
        codeUnit.loaded = false;
        rulesCode.units.insert(rulesCode.units.begin() + ruleNumber, codeUnit);

        codeUnit.code.clear();
        justPreload = false;
      }

    // Handle the contents of each macro.
    } else if (startsWith(line, macro)) {
      justPreload = true;

      // At the start get an macro number for this macro and clear the codeUnit.
      if (endsWith(line, start)) {
        wstring macroName = getMacroName(line);
        wstring addr = getNextMacroNumber();
        macroAddr = VMWstringUtils::stringTo<unsigned int>(addr);
        macroNumber[macroName] = addr;

        codeUnit.code.clear();
      // At the end create an entry on the macros section with the code and
      // substitute the macro end label with a ret instruction.
      } else if (line.find(L"end:") != wstring::npos) {
        Instruction ret;
        ret.op1 = L"ret";
        ret.lineNumber = currentLineNumber;
        codeUnit.code.push_back(ret);

        codeUnit.loaded = false;
        macrosCode.units.insert(macrosCode.units.begin() + macroAddr, codeUnit);

        codeUnit.code.clear();
        justPreload = false;
      }

    // Handle all the simple instructions.
    } else {
      Instruction instr;
      instr.lineNumber = currentLineNumber;
      if (justPreload) {
        // Just preload means that we don't get the internal representation of
        // instr, just keep the line as the first operand to later process it.
        instr.op1 = line;
        codeUnit.code.push_back(instr);
      } else if (getInternalRepresentation(line, codeUnit, instr)) {
        addInstructionToCodeUnit(instr, codeUnit);
      }
    }
  }

  // Finally we backpatch the root scope if needed and delete it.
  currentScope->backPatchLabels(code);
  deleteCurrentScope();

  // Set the final address of the code loaded.
  finalAddress = code.code.size();

  file.close();
}

/**
 * Load the first code section of the vm (which is called code section) until
 * we reach the end of the section.
 *
 * @param file the opened file with the assembly instructions
 * @param code the main code unit of the vm
 */
void AssemblyLoader::loadCodeSection(wfstream &file, CodeUnit &code) {
  wstring line = L"";

  while(getline(file, line, L'\n')) {
    currentLineNumber++;
    // Ignore comments.
    if (line[0] == L'#') {
      continue;
    }

    Instruction instr;
    instr.lineNumber = currentLineNumber;
    if (getInternalRepresentation(line, code, instr)) {
      addInstructionToCodeUnit(instr, code);
    }
    if (line == L"jmp section_rules_start") {
      return;
    }
  }
  code.loaded = true;
}

/**
 * Process a code unit already preloaded converting all instructions to the
 * internal vm representation and translating labels into addresses etc.
 *
 * @param unit the code unit containing the code (a rule or macro)
 */
void AssemblyLoader::loadCodeUnit(CodeUnit &unit) {
  CodeUnit preloadedUnit = unit;
  unit.code.clear();
  createNewScope();

  for (unsigned int i = 0; i < preloadedUnit.code.size(); i++) {
    Instruction instr;
    instr.lineNumber = preloadedUnit.code[i].lineNumber;
    if (getInternalRepresentation(preloadedUnit.code[i].op1, unit, instr)) {
      addInstructionToCodeUnit(instr, unit);
    }
  }

  currentScope->backPatchLabels(unit);
  deleteCurrentScope();
  unit.loaded = true;
}

/**
 * Add a instruction to a code unit, incrementing the appropriate address.
 *
 * @param instruction the instruction to add
 * @param codeUnit the code unit to add the instruction to
 */
void AssemblyLoader::addInstructionToCodeUnit(Instruction instruction,
    CodeUnit &codeUnit) {
  codeUnit.code.push_back(instruction);
  currentScope->nextAddress++;
}

/**
 * Create a new scope and set it as the current one.
 */
void AssemblyLoader::createNewScope() {
  scopes.push_back(Scope());
  currentScope = &(scopes.back());
}

/**
 * Delete the current scope and update currentScope.
 */
void AssemblyLoader::deleteCurrentScope() {
  scopes.pop_back();
  if (scopes.size() > 0) {
    currentScope = &(scopes.back());
  } else {
    currentScope = NULL;
  }
}

/**
 * Get the number assigned to a rule by the compiler.
 *
 * @param ruleLabel the label containing the rule number
 *
 * @return the rule number as a wide string
 */
wstring AssemblyLoader::getRuleNumber(const wstring &ruleLabel) const {
  wstring ruleNumber = L"";
  bool start = false;

  for (unsigned int i = 0; i < ruleLabel.size(); i++) {
    wchar_t ch = ruleLabel[i];
    if (ch == L'_') {
      if (start) {
        return ruleNumber;
      }
      start = true;
    } else if (start) {
      ruleNumber += ch;
    }
  }

  return L"";
}

/**
 * Get a new unique address for a macro.
 *
 * @return a new macro number as a wide string
 */
wstring AssemblyLoader::getNextMacroNumber() {
  wstringstream ws;

  ws << nextMacroNumber;
  nextMacroNumber++;

  return ws.str();
}

/**
 * Get the name of a macro inside a label.
 *
 * @param macroLabel the label containing the macro name
 *
 * @return the macro name as a wide string
 */
wstring AssemblyLoader::getMacroName(const wstring &macroLabel) const {
  size_t startPos = macroLabel.find(L'_') + 1;
  size_t endPos = macroLabel.rfind(L'_');

  return macroLabel.substr(startPos, endPos - startPos);
}

/**
 * Get the vm representation of an assembly instruction.
 *
 * @param line the line as read from the code file
 * @param codeUnit the current code unit where the instruction belongs
 * @param instr the instruction structure to fill in with the vm representation
 *
 * @return true if the instruction was filled, false if the instruction
 * structure can be ignored, for example if it's a label
 */
bool AssemblyLoader::getInternalRepresentation(const wstring &line,
    CodeUnit &codeUnit, Instruction &instr) {
  // First, we get the name of the instruction or process it if it's a label.
  wstring instrName = L"";
  unsigned int pos;
  for (pos = 0; pos < line.size(); pos++) {
    wchar_t ch = line[pos];
    if (ch == L' ' || ch == L'\t' || ch == '\r') {
      pos++;
      break;
    } else if (ch == L':') {
      // If it's a label, just create a new address for it and end.
      currentScope->createNewLabelAddress(instrName);
      return false;
    } else {
      instrName += ch;
    }
  }

  // Then, we use the name to get the opCode of the instruction.
  map<wstring, OP_CODE>::const_iterator it;
  it = opCodes.find(instrName);
  if (it != opCodes.end()) {
    instr.opCode = it->second;
  } else {
    throwError(L"Unrecognized instruction: " + line);
  }

  // Finally, we handle the operand.
  wstring operand = L"";
  for (; pos < line.size(); pos++) {
      wchar_t ch = line[pos];
      if (ch == L'\n' || ch == '\r') {
        break;
      } else {
        operand += ch;
      }
    }

  // FIXME rewrite this ugly code.

  bool hasOperand = line.find(L' ') != wstring::npos;
  if (hasOperand) {
    // If the instruction needs a label.
    switch (instr.opCode) {
    case ADDTRIE:
      instr.op1 = getRuleNumber(operand);
      break;
    case CALL:
      instr.op1 = macroNumber.find(operand)->second;
      break;
    case JMP: /* falls through */
    case JZ: /* falls through */
    case JNZ:
      instr.op1 = currentScope->getReferenceToLabel(operand, codeUnit);
      break;
    default:
      instr.op1 = operand;
      break;
    }
  }

  return true;
}

/**
 * Throw a loader specific error with the current line of the loading process.
 *
 * @param msg the message describing the problem
 */
void AssemblyLoader::throwError(const wstring &msg) const {
  wstringstream ws;
  ws << L"line " << currentLineNumber << L", " << msg;
  throw LoaderException(ws.str());
}

/**
 * Check if a wstring starts with a wstring.
 *
 * @param str the compared string
 * @param preffix the comparing string
 *
 * @return true if str starts with preffix, otherwise, false
 */
bool
AssemblyLoader::startsWith(const wstring &str, const wstring &preffix) const {
  return str.compare(0, preffix.size(), preffix) == 0;
}

/**
 * Check if a wstring ends with a wstring.
 *
 * @param str the compared string
 * @param suffix the comparing string
 *
 * @return true if str ends with preffix, otherwise, false
 */
bool
AssemblyLoader::endsWith(const wstring &str, const wstring &suffix) const {
  return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

/**
 * Print every code unit of a code section translation all instruction to their
 * assembly representation.
 *
 * @param section the section with the code units to print
 * @param header a header to show, usually the name of the section
 * @param unitHeader a header for each of the code units, e.g. "Rule" or "Macro"
 */
void AssemblyLoader::printCodeSection(const CodeSection &section,
    const wstring &header, const wstring &unitHeader) {
  unsigned int maxW = 60;
  unsigned int w = maxW - 20 - header.size();

  wcout << setfill(L'=') << setw(20) << L"=";
  wcout << header;
  wcout << setfill(L'=') << setw(w) << L"=" << endl;

  for (unsigned int i = 0; i < section.units.size(); i++) {
    CodeUnit unit = section.units[i];
    if (unit.loaded) {
      wcout << endl << unitHeader << L" " << i << L":" << endl;
      for (unsigned int j = 0; j < unit.code.size(); j++) {
        printInstruction(unit.code[j], j);
      }
    } else {
      wcout << endl << unitHeader << L" " << i << L" (not loaded):" << endl;
      for (unsigned int j = 0; j < unit.code.size(); j++) {
        wcout << unit.code[j].op1 << endl;
      }
    }
  }

  wcout << setfill(L'=') << setw(maxW) << L"=" << endl;
  wcout << endl;
}

/**
 * Print a code unit translating every instruction to its assembly
 * representation.
 *
 * @param codeUnit the unit with the instructions to print
 * @param header a header to show, usually the name of the code unit
 */
void AssemblyLoader::printCodeUnit(const CodeUnit &codeUnit,
    const wstring &header) {
  unsigned int maxW = 60;
  unsigned int w = maxW - 20 - header.size();

  wcout << setfill(L'=') << setw(20) << L"=";
  wcout << header;
  wcout << setfill(L'=') << setw(w) << L"=" << endl;

  if (codeUnit.loaded) {
    for (unsigned int i = 0; i < codeUnit.code.size(); i++) {
      printInstruction(codeUnit.code[i], i);
    }
  } else {
    wcout << L"(not loaded)" << endl;
    for (unsigned int i = 0; i < codeUnit.code.size(); i++) {
      wcout << codeUnit.code[i].op1 << endl;
    }
  }

  wcout << setfill(L'=') << setw(maxW) << L"=" << endl;
  wcout << endl;
}

/**
 * Convert an internal representation of an instruction to an assembly one
 * and print it for debugging purposes.
 *
 * @param instr the instruction to print
 * @param PC the program counter
 */
void AssemblyLoader::printInstruction(const Instruction & instr,
    unsigned int PC) {
  if (reversedOpCodes.size() == 0) {
    createReversedOpCodesMap();
  }

  wstring opCode = reversedOpCodes.find(instr.opCode)->second;
  wstring operand = L"";
  if (instr.opCode == CALL) {
    operand = getMacroNameFromNumber(instr.op1);
  } else {
    operand = instr.op1;
  }

  if (operand != L"") {
    wcout << PC << L"\t" << opCode << L" " << operand << endl;
  } else {
    wcout << PC << L"\t" << opCode << endl;
  }
}

/**
 * Create a reverse opcodes map only used for debugging purposes.
 */
void AssemblyLoader::createReversedOpCodesMap() {
  map<wstring, OP_CODE>::const_iterator it;
  for (it = opCodes.begin(); it != opCodes.end(); ++it) {
    reversedOpCodes[it->second] = it->first;
  }
}

/**
 * Get a macro name from the internal number representation. This method
 * creates the reversedMacroNumber map if it's not already created.
 *
 * @param number the number to get the macro name asociated with
 *
 * @return the name of the macro associated to the number passed as parameter
 */
wstring AssemblyLoader::getMacroNameFromNumber(const wstring &number) {
  if (reversedMacroNumber.size() == 0) {
    map<wstring, wstring>::const_iterator it;
      for (it = macroNumber.begin(); it != macroNumber.end(); ++it) {
        reversedMacroNumber[it->second] = it->first;
      }
  }

  return reversedMacroNumber[number];
}
