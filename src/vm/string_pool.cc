#include "string_pool.h"

std::unordered_map<std::wstring, int> StringPool::_stringIndex;
std::vector<std::wstring> StringPool::_stringPool;
