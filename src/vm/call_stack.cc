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

#include "call_stack.h"

#include "vm.h"

CallStack::CallStack() {
  vm = NULL;
}

CallStack::CallStack(VM *vm) {
  this->vm = vm;
}

CallStack::CallStack(const CallStack &c) {
  copy(c);
}

CallStack::~CallStack() {
  vm = NULL;
}

CallStack& CallStack::operator=(const CallStack &c) {
  if (this != &c) {
    this->~CallStack();
    this->copy(c);
  }
  return *this;
}

void CallStack::copy(const CallStack &c) {
  stack = c.stack;
  vm = c.vm;
}

/**
 * Push a code unit call to the stack. This is needed because a rule can call
 * a macro and a macro can also call a macro, so storing the last PC isn't
 * enough, we also need the code section.
 *
 * @param call the call to push to the stack
 */
void CallStack::pushCall(const TCALL &call) {
  stack.push_back(call);
  vm->setCurrentCodeUnit(call);
}

/**
 * Pop the current code unit and set the last one as the current one. This is
 * done when a macro ends, it restores its caller and its PC.
 */
void CallStack::popCall() {
  stack.pop_back();
  vm->setCurrentCodeUnit(stack.back());
}

/**
 * Save the current PC so we can return to it at the end of a call.
 *
 * @param PC the current PC to save
 */
void CallStack::saveCurrentPC(int PC) {
  stack.back().PC = PC;
}

