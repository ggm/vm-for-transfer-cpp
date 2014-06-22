#ifndef STRING_POOL_
#define STRING_POOL_

#include <string>
#include <unordered_map>
#include <vector>

class StringPool {
 private:
  static std::unordered_map<std::wstring, int> _stringIndex;
  static std::vector<std::wstring> _stringPool;
 public:
  static int getCode(const std::wstring& wstr) {
    auto it = _stringIndex.find(wstr);
    if(it != _stringIndex.end()) {
      return it->second;
    } else {
      int index = static_cast<int>(_stringPool.size());
      _stringPool.push_back(wstr);
      _stringIndex[wstr] = index;
      return index;
    }
  }

  static size_t size() {
    return _stringPool.size();
  }
};

#endif
