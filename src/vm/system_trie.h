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

using namespace std;

/// Not a rule number: for the nodes without a rule number.
const static int NaRuleNumber = -1;

/// This struct represents a node of the trie data structure.
struct TrieNode {
  /// The rule's number to apply, if it's the end of a pattern.
  int ruleNumber;

  /// A collection of children with the next character as key.
  map<wchar_t, TrieNode *> children;
};

/**
 * This class stores the patterns, used to identify which rules to apply, as
 * a trie for easy retrieval.
 */
class SystemTrie {

public:

  SystemTrie();
  SystemTrie(const SystemTrie &);
  ~SystemTrie();
  SystemTrie& operator=(const SystemTrie &);
  void copy(const SystemTrie &);

  list<TrieNode *> getPatternNodes(const wstring &);
  list<TrieNode *> getPatternNodes(const wstring &, TrieNode*);
  int getRuleNumber(const wstring &);
  void addPattern(const vector<wstring> &, int);

private:

  /// The first TrieNode of the trie.
  TrieNode *root;

  bool canSkipChar(wchar_t) const;
  list<TrieNode *> getNextNodes(wchar_t, TrieNode *) const;
  TrieNode* setDefaultChild(TrieNode *, wchar_t);
  TrieNode* insertStar(TrieNode *);
  TrieNode* insertTagStar(TrieNode *);
  TrieNode* insertPattern(const wstring &, int ruleNumber, TrieNode *);

};

#endif /* SYSTEM_TRIE_H_ */
