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

#include "interpreter.h"

#include <sstream>

#include "vm.h"
#include "vm_wstring_utils.h"

const wstring Interpreter::FALSE_WSTR = L"0";

const wstring Interpreter::TRUE_WSTR = L"1";

Interpreter::Interpreter() {
  modifiedPC = false;
}

Interpreter::Interpreter(VM *vm) {
  modifiedPC = false;
  this->vm = vm;
}

Interpreter::Interpreter(const Interpreter &c) {
  copy(c);
}

Interpreter::~Interpreter() {

}

Interpreter& Interpreter::operator=(const Interpreter &c) {
  if (this != &c) {
    this->~Interpreter();
    this->copy(c);
  }
  return *this;
}

void Interpreter::copy(const Interpreter &c) {

}

/**
 * Throw an interpreter specific error.
 *
 * @param msg the message describing the problem
 */
void Interpreter::throwError(const wstring &msg) {
  vm->status = FAILED;
  throw InterpreterException(msg);
}

/**
 * Modify the vm's PC and set it as modified for the interpreter.
 *
 * @param newPC the new PC to set in the vm
 */
void Interpreter::modifyPC(int newPC) {
  vm->PC = newPC;
  modifiedPC = true;
}

/**
 * Get a word from the source side for every transfer stage.
 *
 * @param relativePos the position of the word in the current context
 *
 * @return a reference to the lexical unit
 */
LexicalUnit* Interpreter::getSourceLexicalUnit(int relativePos) {
  if (vm->transferStage == TRANSFER) {
    int realPos = vm->currentWords[relativePos - 1];
    return ((BilingualWord *) vm->words[realPos])->getSource();
  } else if (vm->transferStage == INTERCHUNK) {
    int realPos = vm->currentWords[relativePos - 1];
    return ((ChunkWord *) vm->words[realPos])->getChunk();
  } else {
    // Get the only word available in the postchunk.
    ChunkWord *word = (ChunkWord *) vm->words[vm->currentWords[0]];

    int realPos;
    // If it's a macro, get the position passed as a parameter.
    if (vm->currentWords.size() > 1) {
      realPos = vm->currentWords[relativePos];
    } else {
      realPos = relativePos;
    }

    if (realPos == 0) {
      return word->getChunk();
    } else {
      return word->getContentLexicalUnit(realPos - 1);
    }
  }
}

/**
 * Get a word from the target side for chunker stage.
 *
 * @param relativePos the position of the word in the current context
 *
 * @return a reference to the lexical unit
 */
LexicalUnit* Interpreter::getTargetLexicalUnit(int relativePos) {
  int realPos = vm->currentWords[relativePos - 1];
  return ((BilingualWord *) vm->words[realPos])->getTarget();
}

/**
 * Execute all the code inside the preprocessing code section.
 */
void Interpreter::preprocess() {
  for (vector<Instruction>::iterator it = vm->preproprocessCode.code.begin();
      it != vm->preproprocessCode.code.end(); it++) {
    execute(*it);
  }
}

/**
 * Execute a instruction, modifying the vm accordingly.
 */
void Interpreter::execute(const Instruction &instr) {

  // Execute the appropriate instruction depending on the opcode.
  // Cases are ordered by frequency (calculated using the en-ca pair) just
  // in case the compiler doesn't optimize it.
  switch (instr.opCode) {
  case PUSH: executePush(instr); break;
  case CLIPTL: executeCliptl(instr); break;
  case CLIP: executeClip(instr); break;
  case LU: executeLu(instr); break;
  case CALL: executeCall(instr); break;
  case CMP: executeCmp(instr); break;
  case JZ: executeJz(instr); break;
  case JMP: executeJmp(instr); break;
  case PUSHSB: executePushsb(instr); break;
  case CHUNK: executeChunk(instr); break;
  case STOREV: executeStorev(instr); break;
  case OUT: executeOut(instr); break;
  case STORECL: executeStorecl(instr); break;
  case MODIFY_CASE: executeModifyCase(instr); break;
  case ADDTRIE: executeAddtrie(instr); break;
  case CONCAT: executeConcat(instr); break;
  case CASE_OF: executeCaseOf(instr); break;
  case CLIPSL: executeClipsl(instr); break;
  case CMPI: executeCmpi(instr); break;
  case NOT: executeNot(instr); break;
  case STORETL: executeStoretl(instr); break;
  case PUSHBL: executePushbl(instr); break;
  case OR: executeOr(instr); break;
  case GET_CASE_FROM: executeGetCaseFrom(instr); break;
  case AND: executeAnd(instr); break;
  case INIG: executeInig(instr); break;
  case MLU: executeMlu(instr); break;
  case APPEND: executeAppend(instr); break;
  case RET: executeRet(instr); break;
  case LU_COUNT: executeLuCount(instr); break;
  case STORESL: executeStoresl(instr); break;
  case IN: executeIn(instr); break;
  case JNZ: executeJnz(instr); break;
  case BEGINS_WITH: executeBeginsWith(instr); break;
  case BEGINS_WITH_IG: executeBeginsWithIg(instr); break;
  case ENDS_WITH: executeEndsWith(instr); break;
  case ENDS_WITH_IG: executeEndsWithIg(instr); break;
  case CMP_SUBSTR: executeCmpSubstr(instr); break;
  case CMPI_SUBSTR: executeCmpiSubstr(instr); break;
  }

  // If the last instruction didn't modify the PC, point it to the next
  // instruction. Otherwise, keep the modified PC.
  if (!modifiedPC) {
    vm->PC++;
  } else {
    modifiedPC = false;
  }
}

/**
 * Get the operands of instr from the stack and return them reversed.
 *
 * @param instr the instruction to get the operands for
 *
 * @return the operands in reversed stack order
 */
vector<wstring> Interpreter::getOperands(const Instruction &instr) {
  vector<wstring> operands;

  wstringstream ws(instr.op1);
  int numOperands;
  ws >> numOperands;

  while (numOperands > 0) {
    operands.insert(operands.begin(), popSystemStack());
    numOperands--;
  }

  return operands;
}

/**
 * Get n operands from the stack and return them reversed.
 *
 * @param instr the instruction to get the operands for
 * @param n the number of operands
 *
 * @return the operands in reversed stack order
 */
vector<int> Interpreter::getNOperands(const Instruction &instr,
    const wstring &n) {
  vector<int> operands;

  wstringstream ws(n);
  int numOperands;
  ws >> numOperands;

  int operand;
  while (numOperands > 0) {
    operand = popSystemStackInteger();
    operands.insert(operands.begin(), operand);
    numOperands--;
  }

  return operands;
}

/**
 * Pop the top of the stack and return its value.
 *
 * @return the top of the stack
 */
wstring Interpreter::popSystemStack() {
  wstring top = vm->systemStack.back();
  vm->systemStack.pop_back();

  return top;
}

/**
 * Pop the top of the stack and return its value as an integer.
 *
 * @return the top of the stack
 */
int Interpreter::popSystemStackInteger() {
  wstringstream top(vm->systemStack.back());
  vm->systemStack.pop_back();

  int intValue;
  top >> intValue;

  return intValue;
}

/**
 * Push a CASE value to the stack as a wide string.
 *
 * @param valueCase the case enum value to push
 */
void Interpreter::pushCaseToStack(CASE valueCase) {
  switch (valueCase) {
  case aa:
    vm->systemStack.push_back(L"aa");
    break;
  case Aa:
    vm->systemStack.push_back(L"Aa");
    break;
  case AA:
    vm->systemStack.push_back(L"AA");
    break;
  }
}

void Interpreter::executeAddtrie(const Instruction &instr) {
  int numPatterns = popSystemStackInteger();

  vector<wstring> pattern;
  while (numPatterns > 0) {
    wstring part = popSystemStack();
    pattern.insert(pattern.begin(),
        VMWstringUtils::replace(part, L"\"", L""));
    numPatterns--;
  }

  // Add the pattern with the rule number to the trie.
  int ruleNumber;
  wstringstream ws(instr.op1);
  ws >> ruleNumber;
  vm->systemTrie.addPattern(pattern, ruleNumber);
}

void Interpreter::executeAnd(const Instruction &instr) {
  vector<wstring> operands = getOperands(instr);

  // Return false (0) if one operand if false.
  for (unsigned int i = 0; i < operands.size(); i++) {
    if (operands[i] == FALSE_WSTR) {
      vm->systemStack.push_back(FALSE_WSTR);
      return;
    }
  }

  // Else, return true (1).
  vm->systemStack.push_back(TRUE_WSTR);
}

void Interpreter::executeOr(const Instruction &instr) {
  vector<wstring> operands = getOperands(instr);

  // Return true (1) if one operand if true.
  for (unsigned int i = 0; i < operands.size(); i++) {
    if (operands[i] == TRUE_WSTR) {
      vm->systemStack.push_back(TRUE_WSTR);
      return;
    }
  }

  // Else, return false (0).
  vm->systemStack.push_back(FALSE_WSTR);
}

void Interpreter::executeNot(const Instruction &instr) {
  wstring op1 = popSystemStack();

  if (op1 == FALSE_WSTR) {
    vm->systemStack.push_back(TRUE_WSTR);
  } else {
    vm->systemStack.push_back(FALSE_WSTR);
  }
}

void Interpreter::executeAppend(const Instruction &instr) {
  vector<wstring> operands = getOperands(instr);
  wstring ws = L"";

  for (unsigned int i = 0; i < operands.size(); i++) {
    ws += operands[i];
  }

  wstring varName = popSystemStack();
  vm->variables[varName] += ws;
}

bool
Interpreter::beginsWith(const wstring &word, const wstring &preffixes) const {
  wstring preffix = L"";
  wchar_t ch;

  for (unsigned int i = 0; i < preffixes.size(); i++) {
    ch = preffixes[i];

    if (ch == L'|' || i == preffixes.size() - 1) {
      bool begins = true;
      for (unsigned int j = 0; j < preffix.size(); j++) {
        if (j >= word.size() || preffix[j] != word[j]) {
          preffix = L"";
          begins = false;
          break;
        }
      }
      if (begins) return true;
    } else {
      preffix += ch;
    }
  }

  return false;
}

void Interpreter::executeBeginsWith(const Instruction &instr) {
  wstring preffixes = popSystemStack();
  wstring word = popSystemStack();

  if (beginsWith(word, preffixes)) {
    vm->systemStack.push_back(TRUE_WSTR);
  } else {
    vm->systemStack.push_back(FALSE_WSTR);
  }
}

void Interpreter::executeBeginsWithIg(const Instruction &instr) {
  wstring preffixes = VMWstringUtils::wtolower(popSystemStack());
  wstring word = VMWstringUtils::wtolower(popSystemStack());

  if (beginsWith(word, preffixes)) {
    vm->systemStack.push_back(TRUE_WSTR);
  } else {
    vm->systemStack.push_back(FALSE_WSTR);
  }
}

void Interpreter::executeCall(const Instruction &instr) {
  // Save current PC to return later when the macro ends.
  vm->callStack->saveCurrentPC(vm->PC);

  // Get the words passed as argument to the macro.
  vector<int> operands = getNOperands(instr, popSystemStack());

  vector<int> words;

  if (vm->transferStage == POSTCHUNK) {
    // For the postchunk append the index of the only current word and then
    // append all the parameters.
    words.push_back(vm->currentWords[0]);
    words.insert(words.end(), operands.begin(), operands.end());
  } else {
    // For the rest, just append the index of the current words.
    for (unsigned int i = 0; i < operands.size(); i++) {
      words.push_back(vm->currentWords[operands[i] - 1]);
    }
  }

  // Create an entry in the call stack with the macro called.
  wstringstream ws(instr.op1);
  int macroNumber;
  ws >> macroNumber;

  TCALL call;
  call.PC = 0;
  call.number = macroNumber;
  call.section = MACROS_SECTION;
  call.words = words;
  vm->callStack->pushCall(call);

  // Tell the interpreter that the PC has been modified, so it does not.
  modifyPC(vm->PC);
}

void Interpreter::executeRet(const Instruction &instr) {
  // Restore the last code section and its PC.
  vm->callStack->popCall();
}

void Interpreter::executeClip(const Instruction &instr) {
  wstring parts = popSystemStack();
  int pos = popSystemStackInteger();
  LexicalUnit *lu = getSourceLexicalUnit(pos);

  wstring linkTo = instr.op1;
  if (linkTo != L"") {
    VMWstringUtils::replace(linkTo, L"\"", L"");
  }

  wstring lemmaAndTags = lu->getPart(LEM) + lu->getPart(TAGS);
  handleClipInstruction(parts, lu, lemmaAndTags, linkTo);
}

void Interpreter::executeClipsl(const Instruction &instr) {
  wstring parts = popSystemStack();
  int pos = popSystemStackInteger();
  LexicalUnit *lu = getSourceLexicalUnit(pos);

  wstring linkTo = instr.op1;
  if (linkTo != L"") {
    VMWstringUtils::replace(linkTo, L"\"", L"");
  }

  handleClipInstruction(parts, lu, lu->getWhole(), linkTo);
}

void Interpreter::executeCliptl(const Instruction &instr) {
  wstring parts = popSystemStack();
  int pos = popSystemStackInteger();
  LexicalUnit *lu = getTargetLexicalUnit(pos);

  wstring linkTo = instr.op1;
  if (linkTo != L"") {
    VMWstringUtils::replace(linkTo, L"\"", L"");
  }

  handleClipInstruction(parts, lu, lu->getWhole(), linkTo);
}

void Interpreter::handleClipInstruction(const wstring &parts, LexicalUnit *lu,
    const wstring &lemmaAndTags, const wstring &linkTo) {
  bool notLinkTo = (linkTo == L"");

  if (notLinkTo && parts == L"whole") {
    vm->systemStack.push_back(lu->getWhole());
    return;
  } else if (notLinkTo && parts == L"lem") {
    vm->systemStack.push_back(lu->getPart(LEM));
    return;
  } else if (notLinkTo && parts == L"lemh") {
    vm->systemStack.push_back(lu->getPart(LEMH));
    return;
  } else if (notLinkTo && parts == L"lemq") {
    vm->systemStack.push_back(lu->getPart(LEMQ));
    return;
  } else if (notLinkTo && parts == L"tags") {
    vm->systemStack.push_back(lu->getPart(TAGS));
    return;
  } else if (notLinkTo && parts == L"chcontent") {
    vm->systemStack.push_back(lu->getPart(CHCONTENT));
    return;
  } else {
    // Check if one of the parts divided by | matches the lemma or tags.
    wstring longestMatch = L"";
    wstring part = L"";
    wchar_t ch;

    for (unsigned int i = 0; i < parts.size(); i++) {
      ch = parts[i];

      if (i == parts.size() - 1) {
        part += ch;
      }

      if (ch == L'|' || i == parts.size() - 1) {
        if (lemmaAndTags.find(part) != wstring::npos) {
          if (notLinkTo) {
            if (part.size() > longestMatch.size()) {
              longestMatch = part;
            }
          } else {
            vm->systemStack.push_back(linkTo);
            return;
          }
        }
        part = L"";
      } else {
        part += ch;
      }
    }

    if (longestMatch != L"") {
      vm->systemStack.push_back(longestMatch);
      return;
    }
  }

  // If the lu doesn't have the part needed, return "".
  vm->systemStack.push_back(L"");
}

void Interpreter::executeCmp(const Instruction &instr) {
  wstring op1 = popSystemStack();
  wstring op2 = popSystemStack();

  if (op1 == op2) {
    vm->systemStack.push_back(TRUE_WSTR);
  } else {
    vm->systemStack.push_back(FALSE_WSTR);
  }
}

void Interpreter::executeCmpi(const Instruction &instr) {
  wstring op1 = VMWstringUtils::wtolower(popSystemStack());
  wstring op2 = VMWstringUtils::wtolower(popSystemStack());

  if (op1 == op2) {
    vm->systemStack.push_back(TRUE_WSTR);
  } else {
    vm->systemStack.push_back(FALSE_WSTR);
  }
}

void Interpreter::executeCmpSubstr(const Instruction &instr) {
  wstring op1 = popSystemStack();
  wstring op2 = popSystemStack();

  if (op2.find(op1) != wstring::npos) {
    vm->systemStack.push_back(TRUE_WSTR);
  } else {
    vm->systemStack.push_back(FALSE_WSTR);
  }
}

void Interpreter::executeCmpiSubstr(const Instruction &instr) {
  wstring op1 = VMWstringUtils::wtolower(popSystemStack());
  wstring op2 = VMWstringUtils::wtolower(popSystemStack());

  if (op2.find(op1) != wstring::npos) {
    vm->systemStack.push_back(TRUE_WSTR);
  } else {
    vm->systemStack.push_back(FALSE_WSTR);
  }
}

void Interpreter::executeIn(const Instruction &instr) {
  wstring list = popSystemStack();
  wstring value = popSystemStack();

  searchValueInList(value, list);
}

void Interpreter::executeInig(const Instruction &instr) {
  wstring list = VMWstringUtils::wtolower(popSystemStack());
  wstring value = VMWstringUtils::wtolower(popSystemStack());

  searchValueInList(value, list);
}

void Interpreter::searchValueInList(const wstring &value, const wstring &list) {
  wstring part = L"";
  wchar_t ch;
  size_t listSize = list.size();

  for (unsigned int i = 0; i < listSize; i++) {
    ch = list[i];

    if (i == listSize - 1) {
      part += ch;
    }

    if (ch == L'|' || i == listSize - 1) {
      if (part == value) {
        vm->systemStack.push_back(TRUE_WSTR);
        return;
      }
      part = L"";
    } else {
      part += ch;
    }
  }

  vm->systemStack.push_back(FALSE_WSTR);
}

void Interpreter::executeConcat(const Instruction &instr) {
  vector<wstring> operands = getOperands(instr);

  wstring concat = L"";
  for (unsigned int i = 0; i < operands.size(); i++) {
    concat += operands[i];
  }

  vm->systemStack.push_back(concat);
}

void Interpreter::executeChunk(const Instruction &instr) {
  vector<wstring> operands = getOperands(instr);
  unsigned int numOperands = operands.size();

  wstring chunk = L"";
  // If there is only one operand it's the full content of the chunk.
  if (numOperands == 1) {
    chunk = L'^' + operands[0] + L'$';
  } else {
    wstring name = operands[0];
    wstring tags = operands[1];
    chunk += L'^' + name + tags;

    if (numOperands > 2) {
      // Only output enclosing {} in the chunker, in the interchunk the
      // 'chcontent' will already have the {}.
      if (vm->transferStage == TRANSFER) chunk += L'{';
      for (unsigned int i = 2; i < numOperands; i++) {
        chunk += operands[i];
      }
      if (vm->transferStage == TRANSFER) chunk += L'}';
    }

    chunk += L'$';
  }

  vm->systemStack.push_back(chunk);
}

bool
Interpreter::endsWith(const wstring &word, const wstring &suffixes) const {
  wstring suffix = L"";
  wchar_t ch;

  for (unsigned int i = 0; i < suffixes.size(); i++) {
    ch = suffixes[i];

    if (i == suffixes.size() - 1) {
      suffix += ch;
    }

    if (ch == L'|' || i == suffixes.size() - 1) {
      unsigned int suf = suffix.size() - 1;
      unsigned int suffixSize = word.size() - suffix.size();
      bool ends = true;

      for (unsigned int j = word.size() - 1; j >= suffixSize; j--) {
        if (suf < 0 || suffix[suf] != word[j]) {
          suffix = L"";
          ends = false;
          break;
        }
        suf--;
      }
      if (ends) return true;
    } else {
      suffix += ch;
    }
  }

  return false;
}

void Interpreter::executeEndsWith(const Instruction &instr) {
  wstring suffixes = popSystemStack();
  wstring word = popSystemStack();

  if (endsWith(word, suffixes)) {
    vm->systemStack.push_back(TRUE_WSTR);
  } else {
    vm->systemStack.push_back(FALSE_WSTR);
  }
}

void Interpreter::executeEndsWithIg(const Instruction &instr) {
  wstring preffixes = VMWstringUtils::wtolower(popSystemStack());
  wstring word = VMWstringUtils::wtolower(popSystemStack());

  if (endsWith(word, preffixes)) {
    vm->systemStack.push_back(TRUE_WSTR);
  } else {
    vm->systemStack.push_back(FALSE_WSTR);
  }
}

void Interpreter::executeJmp(const Instruction &instr) {
  wstringstream ws(instr.op1);
  int jmpTo;
  ws >> jmpTo;
  modifyPC(jmpTo);
}

void Interpreter::executeJz(const Instruction &instr) {
  wstring condition = popSystemStack();
  if (condition == FALSE_WSTR) {
    wstringstream ws(instr.op1);
    int jmpTo;
    ws >> jmpTo;
    modifyPC(jmpTo);
  }
}

void Interpreter::executeJnz(const Instruction &instr) {
  wstring condition = popSystemStack();
  if (condition != FALSE_WSTR) {
    wstringstream ws(instr.op1);
    int jmpTo;
    ws >> jmpTo;
    modifyPC(jmpTo);
  }
}

void Interpreter::executeLu(const Instruction &instr) {
  vector<wstring> operands = getOperands(instr);

  wstring lu = L"";
  lu += L'^';
  for (unsigned int i = 0; i < operands.size(); i++) {
    lu += operands[i];
  }
  lu += L'$';

  // If the lu is empty, only the ^$, then push an empty string.
  if (lu.size() == 2) {
    vm->systemStack.push_back(L"");
  } else {
    vm->systemStack.push_back(lu);
  }
}

void Interpreter::executeLuCount(const Instruction &instr) {
  ChunkWord *word = (ChunkWord *) vm->words[vm->currentWords[0]];

  int luCount = word->getLuCount();
  wstringstream ws;
  ws << luCount;

  vm->systemStack.push_back(ws.str());
}

void Interpreter::executeMlu(const Instruction &instr) {
  vector<wstring> operands = getOperands(instr);
  if (operands.size() == 0) {
    vm->systemStack.push_back(L"");
    return;
  }

  // Append the lexical units, removing its ^...$
  wstring mlu = L"";
  mlu += L'^';
  mlu += operands[0].substr(1, operands[0].size() - 2);
  for (unsigned int i = 1; i < operands.size(); i++) {
    mlu += L"+" + operands[i].substr(1, operands[i].size() - 2);
  }
  mlu += L'$';

  vm->systemStack.push_back(mlu);
}

void Interpreter::executeCaseOf(const Instruction &instr) {
  wstring value = popSystemStack();
  CASE valueCase = VMWstringUtils::getCase(value);
  pushCaseToStack(valueCase);
}

void Interpreter::executeGetCaseFrom(const Instruction &instr) {
  int pos = popSystemStackInteger();
  LexicalUnit *lu = getSourceLexicalUnit(pos);
  wstring lem = lu->getPart(LEM);

  pushCaseToStack(VMWstringUtils::getCase(lem));
}

void Interpreter::executeModifyCase(const Instruction &instr) {
  wstring newCase = popSystemStack();
  wstring container = popSystemStack();

  container = VMWstringUtils::changeCase(container, newCase);

  vm->systemStack.push_back(container);
}

void Interpreter::executeOut(const Instruction &instr) {
  vector<wstring> operands = getOperands(instr);
  wstring out = L"";
  for (unsigned int i = 0; i < operands.size(); i++) {
    out += operands[i];
  }
  vm->writeOutput(out);
}

void Interpreter::executePush(const Instruction &instr) {
  wstring op1 = instr.op1;

  if (op1.find(L'"') != wstring::npos) {
    // If it's a string, push it without quotes.
    vm->systemStack.push_back(VMWstringUtils::replace(op1, L"\"", L""));
  } else if (VMWstringUtils::iswnumeric(op1)) {
    vm->systemStack.push_back(op1);
  } else {
    // If it's a variable reference, eval it and push its value.
    if (vm->variables.find(op1) != vm->variables.end()) {
      vm->systemStack.push_back(vm->variables[op1]);
    } else {
      throwError(L"Variable " + op1 + L" is not defined.");
    }
  }
}

void Interpreter::executePushbl(const Instruction &instr) {
  vm->systemStack.push_back(L" ");
}

void Interpreter::executePushsb(const Instruction &instr) {
  // The position is relative to the current word(s), so we have to get the
  // actual one. For the postchunk, the relative is the actual one because
  // each chunk stores the blanks in their content.
  wstringstream ws(instr.op1);
  unsigned int relativePos;
  ws >> relativePos;

  if (vm->transferStage == POSTCHUNK) {
    ChunkWord *word = (ChunkWord *) vm->words[vm->currentWords[0]];
    vm->systemStack.push_back(word->getBlank(relativePos));
  } else {
    unsigned int actualPos = relativePos + vm->currentWords[0];
    if (actualPos < vm->superblanks.size()) {
      vm->systemStack.push_back(vm->superblanks[actualPos]);
    } else {
      vm->systemStack.push_back(L"");
    }
  }
}

void Interpreter::executeStorecl(const Instruction &instr) {
  wstring value = popSystemStack();
  wstring parts = popSystemStack();
  int pos = popSystemStackInteger();
  LexicalUnit *lu = getSourceLexicalUnit(pos);

  wstring lemmaAndTags = lu->getPart(LEM) + lu->getPart(TAGS);
  handleStoreClipInstruction(parts, lu, lemmaAndTags, value);
}

void Interpreter::executeStoresl(const Instruction &instr) {
  wstring value = popSystemStack();
  wstring parts = popSystemStack();
  int pos = popSystemStackInteger();
  LexicalUnit *lu = getSourceLexicalUnit(pos);

  handleStoreClipInstruction(parts, lu, lu->getWhole(), value);
}

void Interpreter::executeStoretl(const Instruction &instr) {
  wstring value = popSystemStack();
  wstring parts = popSystemStack();
  int pos = popSystemStackInteger();
  LexicalUnit *lu = getTargetLexicalUnit(pos);

  handleStoreClipInstruction(parts, lu, lu->getWhole(), value);
}

void Interpreter::handleStoreClipInstruction(const wstring &parts,
    LexicalUnit *lu, const wstring &lemmaAndTags, const wstring &value) {
  wstring oldWhole = lu->getWhole();
  bool change = false;

  if (parts == L"whole") {
    lu->changePart(WHOLE, value);
    change = true;
  } else if (parts == L"lem") {
    lu->changePart(LEM, value);
    change = true;
  } else if (parts == L"lemh") {
    lu->changePart(LEMH, value);
    change = true;
  } else if (parts == L"lemq") {
    lu->changePart(LEMQ, value);
    change = true;
  } else if (parts == L"tags") {
    lu->changePart(TAGS, value);
    change = true;
  } else if (parts == L"chcontent") {
    lu->changePart(CHCONTENT, value);
    // If we are in the postchunk stage and change the chunk content
    // we need to parse it again, so we can use it as lexical units.
    if (vm->transferStage == POSTCHUNK) {
      ChunkWord *word = (ChunkWord *) vm->words[vm->currentWords[0]];
      word->parseChunkContent();
    }
  } else {
    // Check if one of the parts divided by | matches the lemma or tags.
    wstring longestMatch = L"";
    wstring part = L"";
    wchar_t ch;

    for (unsigned int i = 0; i < parts.size(); i++) {
      ch = parts[i];

      if (i == parts.size() - 1) {
        part += ch;
      }

      if (ch == L'|' || i == parts.size() - 1) {
        if (lemmaAndTags.find(part) != wstring::npos) {
          if (part.size() > longestMatch.size()) {
            longestMatch = part;
          }
        }
        part = L"";
      } else {
        part += ch;
      }
    }

    if (longestMatch != L"") {
      lu->modifyTag(longestMatch, value);
      change = true;
    }
  }

  if (change && vm->transferStage == POSTCHUNK) {
    // Update the chunk content when changing a lu inside the chunk.
    ChunkWord *word = (ChunkWord *) vm->words[vm->currentWords[0]];
    word->updateChunkContent(oldWhole, lu->getWhole());
  }
}

void Interpreter::executeStorev(const Instruction &instr) {
  wstring value = popSystemStack();
  wstring varName = popSystemStack();
  vm->variables[varName] = value;
}
