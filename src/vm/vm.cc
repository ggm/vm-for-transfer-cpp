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

#include "vm.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "vm_exceptions.h"
#include "assembly_loader.h"

using namespace std;

VM::VM() {
  debugMode = false;
  callStack = new CallStack(this);
  interpreter = new Interpreter(this);
  nextPattern = 0;
  lastSuperblank = -1;
  loader = NULL;
}

VM::VM(const VM &vm) {
  copy(vm);
}

VM::~VM() {
  if (loader != NULL) {
    delete loader;
    loader = NULL;
  }

  for (unsigned int i = 0; i < words.size(); i++) {
    if (words[i] != NULL) {
      delete words[i];
      words[i] = NULL;
    }
  }

  if (callStack != NULL) {
    delete callStack;
    callStack = NULL;
  }

  if (interpreter != NULL) {
    delete interpreter;
    interpreter = NULL;
  }

  if (outputFile.is_open()) {
    outputFile.close();
  }
}

VM& VM::operator=(const VM &vm) {
  if (this != &vm) {
    this->~VM();
    this->copy(vm);
  }
  return *this;
}

void VM::copy(const VM &vm) {
  transferStage = vm.transferStage;
  transferDefault = vm.transferDefault;
  inputFileName = vm.inputFileName;
  debugMode = vm.debugMode;
}

/**
 * Set the code file to read the instructions, initialize the appropriate
 * loader depending on the file's type and set the transfer stage.
 *
 * @param fileName code file's name
 */
void VM::setCodeFile(char *fileName) {
  wfstream file;
  file.open(fileName, ios::in);

  wstring header;
  getline(file, header, L'\n');
  setLoader(header, fileName);

  wstring transferHeader;
  getline(file, transferHeader, L'\n');
  setTransferStage(transferHeader);

  file.close();
}

/**
 * Set the loader to use depending on the header of the code file.
 *
 * @param header the first line of the code file indicates the type of file
 * @param fileName the file name of the code file
 */
void VM::setLoader(const wstring &header, char *fileName) {
  if (header == L"#<assembly>") {
    loader = new AssemblyLoader(fileName);
  } else {
    wstringstream msg;
    msg << L"The header of the file " << fileName << " is not recognized: "
        << endl;
    msg << header << endl;
    throw VmException(msg.str());
  }
}

/**
 * Set the transfer stage to process by the vm.
 *
 * @param transferHeader the part of the code file with the stage header
 */
void VM::setTransferStage(const wstring &transferHeader) {
  wstring transfer = L"transfer";
  wstring interchunk = L"interchunk";
  wstring postchunk = L"postchunk";
  wstring chunk = L"chunk";

  if (!transferHeader.compare(2, transfer.size(), transfer)) {
    transferStage = TRANSFER;
    // Set chunker mode, by default 'lu'.
    if (transferHeader.size() >= 24 && !transferHeader.compare(20, 5, chunk)) {
      transferDefault = TD_CHUNK;
    } else {
      transferDefault = TD_LU;
    }
  } else if (!transferHeader.compare(2, interchunk.size(), interchunk)) {
    transferStage = INTERCHUNK;
  } else if (!transferHeader.compare(2, postchunk.size(), postchunk)) {
    transferStage = POSTCHUNK;
  }
}

/**
 * Set the input file to read the input of the vm.
 *
 * @param fileName input file's name
 */
void VM::setInputFile(char *fileName) {
  inputFileName = fileName;
}

/**
 * Set the output file to save the vm's output.
 *
 * @param fileName output file's name
 */
void VM::setOutputFile(char *fileName) {
  outputFile.open(fileName);
}

/**
 * Set the debug mode, creating a debugger an setting it up as a proxy.
 */
void VM::setDebugMode() {
  debugMode = true;
  // TODO: Create the debugger proxy and its components.
}

/**
 * Set the current code unit as the one passed as parameter.
 *
 * @param call the call containing the code unit to set as the current one
 */
void VM::setCurrentCodeUnit(const TCALL &call) {
  currentWords = call.words;
  setPC(call.PC);

  int numCodeUnit = call.number;

  switch (call.section) {
  case RULES_SECTION:
    currentCodeUnit = &(rulesCode.units[numCodeUnit]);
    break;
  case MACROS_SECTION:
    currentCodeUnit = &(macrosCode.units[numCodeUnit]);
    break;
  }

  // If the code unit hasn't been fully loaded, we need to process it now.
  if (!currentCodeUnit->loaded) {
    loader->loadCodeUnit(*currentCodeUnit);
  }

  endAddress = currentCodeUnit->code.size();
}

/**
 * Set the program counter to one passed as parameter.
 *
 * @param PC the program counter to set as the current one
 */
void VM::setPC(int PC) {
  this->PC = PC;
}

/**
 * Write a wide string to the output already set or stdout by default.
 *
 * @param wstr the wide string to output
 */
void VM::writeOutput(wstring wstr) {
  if (outputFile.is_open()) {
    outputFile << wstr;
  } else {
    wcout << wstr;
  }
}

/**
 * Load, preprocess and execute the contents of the files.
 */
void VM::run() {
  try {
    loader->load(preproprocessCode, code, rulesCode, macrosCode, endAddress);
    interpreter->preprocess();
    initializeVM();
    tokenizeInput();

    // Select the first rule. If there isn't one, the vm work has ended.
    if (status == RUNNING) {
      selectNextRule();
    }

    while(status == RUNNING) {
      // Execute the rule selected until it ends.
      while (status == RUNNING and PC < endAddress) {
        interpreter->execute(currentCodeUnit->code[PC]);
      }

      // Process rule ending and select the next one to execute.
      processRuleEnd();
      if (status == RUNNING) {
        selectNextRule();
      }

    }
  } catch (LoaderException &le) {
    wcerr << L"Loader error: " << le.getMessage() << endl;
  } catch (InterpreterException &ie) {
    wcerr << L"Interpreter error: " << ie.getMessage() << endl;
  }
}

/**
 * Print all the code sections for information or debugging purposes.
 */
void VM::printCodeSection() const {
  loader->printCodeUnit(code, L" Code section ");
  loader->printCodeUnit(preproprocessCode, L" Preprocess section ");
  loader->printCodeSection(rulesCode, L" Rules code section ", L"Rule");
  loader->printCodeSection(macrosCode, L" Macros code section ", L"Macro");
}

/**
 * Divide the input in words using the appropriate word type depending on the
 * transfer stage.
 */
void VM::tokenizeInput() {
  wfstream input;
  input.open(inputFileName, ios::in);

  if (input.is_open()) {
    if (transferStage == TRANSFER) {
      BilingualWord::tokenizeInput(input, words, superblanks);
    } else if (transferStage == INTERCHUNK) {
      ChunkWord::tokenizeInput(input, words, superblanks, false, false);
    } else if (transferStage == POSTCHUNK) {
      ChunkWord::tokenizeInput(input, words, superblanks, true, true);
    }
  } else {
    if (transferStage == TRANSFER) {
      BilingualWord::tokenizeInput(wcin, words, superblanks);
    } else if (transferStage == INTERCHUNK) {
      ChunkWord::tokenizeInput(wcin, words, superblanks, false, false);
    } else if (transferStage == POSTCHUNK) {
      ChunkWord::tokenizeInput(wcin, words, superblanks, true, true);
    }
  }

  input.close();
}

/**
 * Execute code to initialize the VM, e.g. default values for vars.
 */
void VM::initializeVM() {
  PC = 0;
  status = RUNNING;

  while(status == RUNNING && PC < code.code.size()) {
    interpreter->execute(code.code[PC]);
  }
}

/**
 * Get the part of a source word needed for pattern matching, depending on the
 * transfer stage.
 *
 * @param pos the position of the word to use
 *
 * @return the part needed or "" if pos is past the end. Parts needed would be:
 *  - For transfer: whole (lemma+tags)
 *  - For interchunk: lemma+tags
 *  - For the postchunk: just the lemma
 */
wstring VM::getSourceWord(unsigned int pos) {
  if (pos >= words.size()) {
    return L"";
  }

  if (transferStage == TRANSFER) {
    return ((BilingualWord *) words[pos])->getSource()->getWhole();
  } else if (transferStage == INTERCHUNK) {
    ChunkLexicalUnit *chunk = ((ChunkWord *) words[pos])->getChunk();
    return chunk->getPart(LEM) + chunk->getPart(TAGS);
  } else {
    return ((ChunkWord *) words[pos])->getChunk()->getPart(LEM);
  }
}

/**
 * Get the next input pattern to analyze, lowering the lemma first.
 *
 * @return next pattern with lemma lowered
 */
wstring VM::getNextInputPattern() {
  wstring pattern = VMWstringUtils::lemmaToLower(getSourceWord(nextPattern));
  nextPattern++;

  return pattern;
}

/**
 * Get the superblank at the supplied position avoiding duplicates.
 *
 * @param pos the position of the superblank
 *
 * @return the superblank at pos or "" if that one was already used
 */
wstring VM::getUniqueSuperblank(int pos) {
  int numBlanks = superblanks.size();

  if (pos < numBlanks) {
    if (pos != lastSuperblank) {
      lastSuperblank = pos;
      return superblanks[pos];
    }
  }

  return L"";
}

/**
 * Select the next rule to execute depending on the transfer stage.
 */
void VM::selectNextRule() {
  if(transferStage == POSTCHUNK) {
    selectNextRulePostchunk();
  } else {
    selectNextRuleLRLM();
  }
}

/**
 * Select the next rule trying to match patterns one by one.
 */
void VM::selectNextRulePostchunk() {
  // Go through all the patterns until one matches a rule.
  while (nextPattern < words.size()) {
    unsigned int startPatternPos = nextPattern;
    wstring pattern = getNextInputPattern();
    int ruleNumber = systemTrie.getRuleNumber(pattern);

    if (ruleNumber != NaRuleNumber) {
      setRuleSelected(ruleNumber, startPatternPos, pattern);
      return;
    } else {
      processUnmatchedPattern(words[startPatternPos]);
    }
  }

  // if there isn't any rule at all to execute, stop the vm.
  status = HALTED;
}

/**
 * Select the next rule to execute matching the LRLM pattern.
 */
void VM::selectNextRuleLRLM() {
  int longestMatch = NaRuleNumber;
  unsigned int nextPatternToProcess = nextPattern;

  // Go through all the patterns until one matches a rule.
  while(nextPattern < words.size()) {
    unsigned int startPatternPos = nextPattern;
    // Get the next pattern to process.
    wstring pattern = getNextInputPattern();
    vector<TrieNode *> curNodes = systemTrie.getPatternNodes(pattern);
    nextPatternToProcess++;

    // Get the longest match, left to right
    wstring fullPattern = pattern;
    while (curNodes.size() > 0) {
      // Update the longest match if needed.
      int ruleNumber = systemTrie.getRuleNumber(fullPattern);
      if (ruleNumber != NaRuleNumber) {
        longestMatch = ruleNumber;
        nextPatternToProcess = nextPattern;
      }

      // Continue trying to match current pattern + the next one.
      pattern = getNextInputPattern();
      fullPattern += pattern;
      vector<TrieNode*> nextNodes;
      vector<TrieNode*> auxNodes;
      // For each current node, add every possible transition to the nextNodes.
      for (unsigned int i = 0; i < curNodes.size(); i++) {
        auxNodes = systemTrie.getPatternNodes(pattern, curNodes[i]);
        nextNodes.insert(nextNodes.end(), auxNodes.begin(), auxNodes.end());
      }
      curNodes = nextNodes;
    }

    // If the pattern doesn't match, we will continue with the next one.
    // If there is a match of a group of patterns, we will continue with
    // the last unmatched pattern.
    nextPattern = nextPatternToProcess;

    // Get the full pattern matched by the rule.
    if (nextPattern < words.size()) {
      size_t end = fullPattern.find(getSourceWord(nextPattern));
      if (end != wstring::npos) {
        fullPattern = fullPattern.substr(0, end);
      }
    }

    if (longestMatch != NaRuleNumber) {
      // If there is a longest match, set the rule to process
      setRuleSelected(longestMatch, startPatternPos, fullPattern);
      return;
    } else {
      // Otherwise, process the unmatched pattern.
      processUnmatchedPattern(words[nextPattern - 1]);
    }

    longestMatch = NaRuleNumber;
  }

  // if there isn't any rule at all to execute, stop the vm.
  status = HALTED;
}

/**
 * Set a rule and its words as current ones.
 *
 * @param ruleNumber the number of the rule selected
 * @param startPos the index of the first word of the pattern selected
 * @param pattern the pattern which matched the rule
 */
void VM::setRuleSelected(int ruleNumber, unsigned int startPos, const wstring &pattern) {
  // Output the leading superblank of the matched pattern.
  writeOutput(getUniqueSuperblank(startPos));

  // Add only a reference to the index pos of words, to avoid copying them.
  vector<int> wordsIndex;
  while (startPos != nextPattern) {
    wordsIndex.push_back(startPos);
    startPos++;
  }

  // Create an entry in the call stack with the rule to execute.
  TCALL rule = {RULES_SECTION, ruleNumber, wordsIndex, 0};

  callStack->pushCall(rule);
}

/**
 * Do all the processing needed when rule ends.
 */
void VM::processRuleEnd() {
  // Output the trailing superblank of the matched pattern.
  writeOutput(getUniqueSuperblank(nextPattern));
}

/**
 * Output unmatched patterns as the default form depending on the transfer
 * stage.
 */
void VM::processUnmatchedPattern(TransferWord *word) {
  wstring defaultOutput = L"";

  // Output the leading superblank of the unmatched pattern.
  writeOutput(getUniqueSuperblank(nextPattern - 1));

  switch(transferStage) {
  //For the chunker, output the default version of the unmatched pattern.
  case TRANSFER: {
    wstring whole = ((BilingualWord *) word)->getTarget()->getWhole();

    // If the target word is empty, we don't need to output anything.
    if (whole != L"") {
      wstring wordTL = L"^" + whole + L"$";

      if (transferDefault == TD_CHUNK) {
        if (wordTL[1] == L'*') {
          defaultOutput += L"^unknown<unknown>{" + wordTL + L"}$";
        } else {
          defaultOutput += L"^default<default>{" + wordTL + L"}$";
        }
      } else {
        defaultOutput += wordTL;
      }
    }
    break;
  }
  // For the interchunk stage only need to output the complete chunk.
  case INTERCHUNK: {
    wstring whole = ((ChunkWord *) word)->getChunk()->getWhole();
    defaultOutput += L"^" + whole + L"$";
    break;
  }
  // Lastly, for the postchunk stage output the lexical units inside chunks
  // with the case of the chunk pseudolemma, without the { and }.
  case POSTCHUNK: {
    wstring chcontent = ((ChunkWord *) word)->getChunk()->getPart(CHCONTENT);
    defaultOutput += chcontent.substr(1, chcontent.size() - 2);
    break;
  }
  }

  // Output the trailing superblank of the matched pattern.
  defaultOutput += getUniqueSuperblank(nextPattern);

  writeOutput(defaultOutput);
}

