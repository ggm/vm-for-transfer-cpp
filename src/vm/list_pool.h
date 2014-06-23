#ifndef LIST_POOL_
#define LIST_POOL_

#include <string>
#include <vector>

#include "vm_wstring_utils.h"

class ListPool {
 private:
  static std::vector<std::vector<std::wstring>> _loweredPool;
  static std::vector<std::vector<std::wstring>> _unchangedPool;

  static std::vector<std::wstring> _lowerList(const std::vector<std::wstring>& vList) {
    std::vector<std::wstring> loList;
    for(const std::wstring& wstr : vList) {
      loList.push_back(VMWstringUtils::wtolower(wstr));
    }
    return loList;
  }

 public:
  static inline void store(int index, const std::vector<std::wstring>& vList) {
    if(index >= static_cast<int>(_unchangedPool.size())) {
      _unchangedPool.resize(index + 1);
      _loweredPool.resize(index + 1);
    }

    _unchangedPool[index] = vList;
    _loweredPool[index] = _lowerList(vList);
  }

  static inline const std::vector<std::wstring>& ref(int index) {
    return _unchangedPool[index];
  }

  static inline const std::vector<std::wstring>& refLowered(int index) {
    return _loweredPool[index];
  }
};

#endif
