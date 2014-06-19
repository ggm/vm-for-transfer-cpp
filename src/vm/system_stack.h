#ifndef SYSTEM_STACK_H_
#define SYSTEM_STACK_H_

#include <vector>
#include <string>

#include "vm_wstring_utils.h"

#define STACK_SIZE 1000000

struct SystemStackSlot {
  std::wstring wstr;
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

  inline std::wstring popString() {
    _index--;
    return _stack[_index].wstr;
  }

  inline int popInteger() {
    _index--;
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
