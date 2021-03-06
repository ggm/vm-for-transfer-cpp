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

#ifndef SYSTEM_TRIE_H_
#define SYSTEM_TRIE_H_

#include <map>
#include <vector>
#include <string>
#include <list>
#include <unordered_map>
#include <set>

/// Not a rule number: for the nodes without a rule number.
const static int NaRuleNumber = -1;

struct TrieNode {
  int ruleNumber;
  TrieNode *starTransition;
  TrieNode *starTagTransition;
  std::unordered_map<std::wstring, TrieNode*> links;

  TrieNode();
  TrieNode(int ruleNumber);

  TrieNode *getOrCreateStarTransition();
  TrieNode *getOrCreateStarTagTransition();
  bool containsTransitionBy(const std::wstring& wstr) const;
  TrieNode* _insertPattern(const wchar_t* pattern, int ruleNumber);
  TrieNode* insertPattern(const std::wstring& pattern, int ruleNumber);
  void pushNextNodes(const std::wstring&, std::list<TrieNode*>&) const;
};

class SystemTrie {
 private:
  TrieNode *root;

 public:
  SystemTrie();
  ~SystemTrie();

  std::list<TrieNode*> getPatternNodes(const std::wstring& pattern, TrieNode *startNode);
  std::list<TrieNode*> getPatternNodes(const std::wstring& pattern);
  void addPattern(const std::vector<std::wstring> &pattern, int ruleNumber);
  int getRuleNumber(const std::wstring &pattern);
};

#endif
