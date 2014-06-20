#ifndef SYSTEM_STACK_H_
#define SYSTEM_STACK_H_

#include <vector>
#include <string>
#include <iostream>

#include "vm_wstring_utils.h"

#define STACK_SIZE 1000000

struct SystemStackSlot {
  std::wstring wstr;
  int intVal;
  bool pushedByInteger;
};

class SystemStack {
 private:
  SystemStackSlot* _stack;
  int _index; // next free.

 public:
  int size() {
    return _index;
  }

  SystemStack() {
    _stack = new SystemStackSlot[STACK_SIZE];
    _index = 0;
    for(int i = 0; i < STACK_SIZE; ++i) {
      _stack[i].pushedByInteger = false;
    }
  }

  ~SystemStack() {
    delete[] _stack;
  }

  inline void pop(int N) {
    _index -= N;
  }

  inline void pop() {
    _index--;
  }

  inline void push(const std::wstring& wstr) {
    _stack[_index].wstr = wstr;
    _index++;
  }

  inline void pushInteger(const std::wstring& wstr) {
    wcerr << "pushInteger" << endl;
    _stack[_index].pushedByInteger = true;
    push(wstr);
  }

  inline void pushTrueInteger(int value) {
    _stack[_index].intVal = value;
    _index++;
  }

  inline int popTrueInteger() {
    _index--;
    return _stack[_index].intVal;
  }

  inline std::wstring popString() {
    _index--;
    if(_stack[_index].pushedByInteger) {
      wcerr << "popString on integer" << endl;
    }
    _stack[_index].pushedByInteger = false;
    return _stack[_index].wstr;
  }

  inline int popInteger() {
    wcerr << "popInteger" << endl;
    _index--;
    if(!_stack[_index].pushedByInteger) {
      wcerr << "popInteger on non integer" << endl;
    }
    _stack[_index].pushedByInteger = false;
    return VMWstringUtils::stringTo<int>(_stack[_index].wstr);
  }

  inline SystemStackSlot* relative(int relativePos) {
    return _stack + _index - relativePos;
  }

  inline SystemStackSlot* end() {
    return _stack + _index;
  }
};

#endif
