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

#ifndef CALL_STACK_H_
#define CALL_STACK_H_

#include <vector>

using namespace std;

class VM;

enum CODE_SECTION {
  MACROS_SECTION,
  RULES_SECTION
};

/// This stuct represents a call to a code unit.
struct TCALL {
  CODE_SECTION section;
  int number;
  vector<int> words;
  int PC;
};

/// This class represents a stack of calls to different code units.
class CallStack {

public:

  CallStack();
  CallStack(VM *);
  CallStack(const CallStack &);
  ~CallStack();
  CallStack& operator=(const CallStack &);
  void copy(const CallStack &);

  void pushCall(const TCALL &);
  void popCall();
  void saveCurrentPC(int);

private:

  /// The actual stack used to track the calls and returns.
  vector<TCALL> stack;

  /// Access to the data structures of the vm is needed.
  VM *vm;

};

#endif /* CALL_STACK_H_ */
