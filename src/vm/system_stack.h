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
  int _index; // current top.

 public:
  int size() {
    return _index + 1;
  }

  SystemStack() {
    _stack = new SystemStackSlot[STACK_SIZE];
    _index = -1;
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
    _index++;
    _stack[_index].wstr = wstr;
  }

  inline std::wstring popString() {
    const std::wstring& wstr = _stack[_index].wstr;
    _index--;
    return wstr;
  }

  inline int popInteger() {
    int intValue = VMWstringUtils::stringTo<int>(_stack[_index].wstr);
    _index--;
    return intValue;
  }
};

#endif
