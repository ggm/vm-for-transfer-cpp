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

#ifndef VM_H_
#define VM_H_

#include <string>
#include <vector>

#include "loader.h"
#include "transfer_word.h"
#include "bilingual_word.h"
#include "chunk_word.h"
#include "call_stack.h"
#include "system_trie.h"
#include "interpreter.h"
#include "system_stack.h"

using namespace std;

enum TRANSFER_STAGE {
  TRANSFER, INTERCHUNK, POSTCHUNK
};

enum TRANSFER_DEFAULT {
  TD_CHUNK, TD_LU
};

enum VM_STATUS {
  RUNNING,
  HALTED,
  FAILED
};

/// This class encapsulates all the VM processing.
class VM {

  friend class Interpreter;

public:

  VM();
  VM(const VM&);
  ~VM();
  VM& operator=(const VM&);
  void copy(const VM&);

  void setCodeFile(char *);
  void setInputFile(char *);
  void setOutputFile(char *);
  void setDebugMode();

  void setCurrentCodeUnit(const TCALL &);
  void setPC(int);

  void writeOutput(wstring);

  bool run();

  void printCodeSection() const;

private:
  /// Store the current transfer stage.
  TRANSFER_STAGE transferStage;

  /// Store the default unit in the transfer.
  TRANSFER_DEFAULT transferDefault;

  /// Name of the input file to use.
  string inputFileName;

  /// Output file to use in case there is one, otherwise stdout will be used.
  wofstream outputFile;

  /// Store if the debug mode is active or not.
  bool debugMode;

  /// Program counter: position of the next instruction to execute.
  unsigned int PC;

  /** As a stack-based vm, the operands and results of every instruction are
   * stored in the system stack. */
  SystemStack systemStack;

  /// The vm also has a trie where all patterns are stored.
  SystemTrie systemTrie;

  /// The interpreter is the component responsible of instructions execution.
  Interpreter *interpreter;

  /// Represent the current status of the vm.
  VM_STATUS status;

  /// End address of the executing code.
  unsigned int endAddress;

  /// The preprocess code section stores the patterns processing code.
  CodeUnit preproprocessCode;

  /// Where the main code is stored, e.g. variables initialization.
  CodeUnit code;

  /// Stores all the rules action code.
  CodeSection rulesCode;

  /// This section stores all macros and their code.
  CodeSection macrosCode;

  /// Current code unit in execution (preprocessCode, a macro, a rule...).
  CodeUnit *currentCodeUnit;

  /// The loader is set dynamically, depending on the code file's type.
  Loader *loader;

  /// A call stack to track calls to macros and returns from them.
  CallStack *callStack;

  /// Input will be divided in words with their patterns information.
  vector<TransferWord *> words;

  /// Superblanks between words are stored in this vector.
  vector<wstring> superblanks;

  /// Store the index of the last superblank used to avoid outputting duplicates.
  int lastSuperblank;

  /** The current words is a vector of indices of the words vector ordered by
   * position in the current code unit.
   * \code
   {Position in this vector and codeUnit: index of the word in the words vector}
   {0: Word2, 1: Word1, 2: Word5}
   * \endcode
   */
  vector<int> currentWords;

  /// The index of the next input pattern to process.
  unsigned int nextPattern;

  /// Store variables definitions and their default values.
  map<wstring, wstring> variables;

  void setLoader(const wstring &, char*);
  void setTransferStage(const wstring &);
  void tokenizeInput();
  void initializeVM();
  wstring getSourceWord(unsigned int);
  wstring getNextInputPattern();
  wstring getUniqueSuperblank(int);
  void selectNextRule();
  void selectNextRulePostchunk();
  void selectNextRuleLRLM();
  void setRuleSelected(int, unsigned int, const wstring &);
  void processRuleEnd();
  void processUnmatchedPattern(TransferWord *);

};

#endif /* VM_H_ */
