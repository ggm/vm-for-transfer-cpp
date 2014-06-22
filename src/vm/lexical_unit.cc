#include "lexical_unit.h"

#include <vector>
#include <string>
#include <iostream>

#include "string_pool.h"
#include "vm_wstring_utils.h"

void LexicalUnit::preprocessLemmaAndTagsHashes(
    const std::wstring& lemma,
    const std::wstring& tags) {
  _lemmaAndTagsHashes.clear();

  // Get the code for lemma and push it into the hash cache.
  int lemmaToLowerCode = StringPool::getCode(VMWstringUtils::wtolower(lemma));
  _lemmaAndTagsHashes.push_back(lemmaToLowerCode);

  if (tags.size() > 0) {
    int lastPos = -1;
    while(true) {
      // Find next occurence.
      std::wstring::size_type currentPos = tags.find(L'>', lastPos + 1);
      if(currentPos == std::wstring::npos) {
        break;
      }

      // Process current tag (add to string pool, record code).
      std::wstring currentTag(tags.substr(lastPos + 1, currentPos - lastPos));
      int currentTagCode = StringPool::getCode(currentTag);
      _lemmaAndTagsHashes.push_back(currentTagCode);

      // Prepare next iteration.
      lastPos = currentPos;
    }
  }
}

const std::vector<int>& LexicalUnit::lemmaAndTagsHashes() const {
  return _lemmaAndTagsHashes;
}
