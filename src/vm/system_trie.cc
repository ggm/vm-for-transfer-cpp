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

#include "system_trie.h"

#include <iostream>
#include <unordered_set>
#include <queue>
#include <list>
#include <vector>
#include <utility>

#include "vm_wstring_utils.h"

SystemTrie::SystemTrie() {
  root = new TrieNode;
  root->ruleNumber = NaRuleNumber;
}

SystemTrie::SystemTrie(const SystemTrie &c) {
  copy(c);
}

/**
 * Destructor of SystemTrie. Because of the circular references, we employ a BFS
 * search in the TrieNode space, marking and accumulating all visited nodes.
 */
SystemTrie::~SystemTrie() {
  std::unordered_set<TrieNode*> allNodes;
  std::queue<TrieNode*> bfsQueue;

  bfsQueue.push(root);
  allNodes.insert(root);

  while(!bfsQueue.empty()) {
    TrieNode* currentNode = bfsQueue.front();
    bfsQueue.pop();
    for(const auto& it : currentNode->children) {
      TrieNode* neighbor = it.second;
      if(allNodes.find(neighbor) == allNodes.end()) {
        bfsQueue.push(neighbor);
        allNodes.insert(neighbor);
      }
    }
  }

  for(TrieNode* node : allNodes) {
    delete node;
  }
}

SystemTrie& SystemTrie::operator=(const SystemTrie &c) {
  if (this != &c) {
    this->~SystemTrie();
    this->copy(c);
  }
  return *this;
}

void SystemTrie::copy(const SystemTrie &c) {

}

/**
 * Get the last nodes of the sequence representing the pattern.
 *
 * @param pattern the pattern to match
 *
 * @return if the pattern is matched, the last nodes of the match, otherwise
 * an empty list.
 */
list<TrieNode*> SystemTrie::getPatternNodes(const wstring &pattern) {
  return getPatternNodes(pattern, root);
}

/**
 * Get the last nodes of the sequence representing the pattern.
 *
 * @param pattern the pattern to match
 * @param startNode the start of the matching process
 *
 * @return if the pattern is matched, the last nodes of the match, otherwise
 * an empty list.
 */
list<TrieNode*> SystemTrie::getPatternNodes(const wstring &pattern,
    TrieNode *startNode) {
  list<TrieNode *> curNodes;

  if (pattern == L"") {
    return curNodes;
  }

  wstring patternLowered = VMWstringUtils::lemmaToLower(pattern);

  curNodes.push_back(startNode);

  for (wchar_t ch : pattern) {
    list<TrieNode *> nextNodes;

    for(TrieNode* node : curNodes) {
      // Get the next nodes for every current node and append them to the next.
      nextNodes.splice(nextNodes.end(), getNextNodes(ch, node));
    }

    curNodes = std::move(nextNodes);
    if (curNodes.size() == 0) {
      return curNodes;
    }
  }

  return curNodes;
}

/**
 * Get the rule number for a given pattern.
 *
 * @param pattern the pattern to match
 *
 * @return if the pattern is matched, its rule number, otherwise NaRuleNumber.
 */
int SystemTrie::getRuleNumber(const wstring &pattern) {
  list<TrieNode *> curNodes = getPatternNodes(pattern, root);

  int ruleNumber = NaRuleNumber;

  // If there are several possible rules, return the first which appears on the
  // rules files.
  if (curNodes.size() > 0) {
    for (TrieNode* node : curNodes) {
      int itRuleNumber = node->ruleNumber;

      if (ruleNumber == NaRuleNumber) {
        ruleNumber = itRuleNumber;
      } else if (itRuleNumber != NaRuleNumber && itRuleNumber < ruleNumber) {
        ruleNumber = itRuleNumber;
      }
    }
  }

  return ruleNumber;
}

/**
 * Add a pattern, as a list of string(s), to the trie.
 *
 * Options are stored in a list of curNodes and the next part is inserted after
 * each option, for example, given the input:
 *
 * \code
 * Pattern: ["<det><nom>", "<adj>|<adj><sint>|<adj><comp>", "<adv_pp>"]
 * Rule number: 12
 *
 * The output trie would be:
 *                                   /~~<adv_pp> -> 12
 *                                  |
 *               <det><nom>~~<adj>~~|~~<sint>~~<adv_pp> -> 12
 *                                  |
 *                                   \~~<comp>~~<adv_pp> -> 12
 * \endcode
 *
 * Note: if a pattern starts directly with tags, we need to add a node that
 * matches every alphabetic char (*), so it can accept patterns with lemmas.
 *
 * @param pattern the pattern to add, a vector with one or more strings to add
 * @param ruleNumber the rule number to associate to this pattern.
 */
void SystemTrie::addPattern(const vector<wstring> &pattern, int ruleNumber) {
  wstring part = L"";

  // Only the last part of the pattern matches to the ruleNumber.
  int rule = NaRuleNumber;
  unsigned int numPatterns = pattern.size();

  vector<TrieNode *> curNodes;
  curNodes.push_back(root);

  for (unsigned int i = 0; i < numPatterns; i++) {
    vector<TrieNode *> lastNodes;
    // If it's the last part of the pattern, insert it with the rule number.
    if (i == numPatterns - 1) {
      rule = ruleNumber;
    }

    part = pattern[i];
    wstring option = L"";

    for (vector<TrieNode *>::iterator it = curNodes.begin();
         it != curNodes.end(); it++) {
      TrieNode *node = *it;

      wchar_t ch;
      for (unsigned int j = 0; j < part.size(); j++) {
        ch = part[j];

        // If the a part of a pattern has different options (op1|op2|op3).
        if (ch == L'|') {
          // Add each one, inserting a star if it starts with '<'.
          if (option[0] == L'<') {
            TrieNode *starNode = insertStar(node);
            lastNodes.push_back(insertPattern(option, rule, starNode));
          } else {
            lastNodes.push_back(insertPattern(option, rule, node));
          }

          option = L"";
        } else {
          option += part[j];
        }
      }

      // Add the single part or the last of its options.
      if (option[0] == L'<') {
        TrieNode *starNode = insertStar(node);
        lastNodes.push_back(insertPattern(option, rule, starNode));
      } else {
        lastNodes.push_back(insertPattern(option, rule, node));
      }

      option = L"";
    }

    // Continue adding from where we ended.
    curNodes = lastNodes;
  }
}

/**
 * Check if a char can be skip in a '*' node.
 *
 * @param ch the char to check
 *
 * @return true if char is skippable, otherwise, false
 */
bool SystemTrie::canSkipChar(wchar_t ch) const {
  return ch != L'<' && ch != L'>';
}

/**
 * Get a list of next nodes given a char and a start node.
 *
 * @param ch the wide character to use for the transition
 * @param startNode the node from which to start
 *
 * @return a collection of nodes to continue matching a pattern
 */
list<TrieNode*> SystemTrie::getNextNodes(wchar_t ch,
    TrieNode *startNode) const {
  list<TrieNode *> nextNodes;

  // If a word is unknown (*lemma) it shouldn't match with anything.
  if (ch == L'*') {
    return nextNodes;
  }

  map<wchar_t, TrieNode*>::const_iterator it;

  // If the char is skippable, add the transition with a star if there is one.
  if (canSkipChar(ch)) {
    it = startNode->children.find(L'*');
    if (it != startNode->children.end()) {
      nextNodes.push_back(it->second);
    }
  }

  it = startNode->children.find(ch);
  if (it != startNode->children.end()) {
    nextNodes.push_back(it->second);
  }

  return nextNodes;
}

/**
 * Return the child with character ch transition or create a new default child
 * with a ch transition and return it. This method is similar to Python's
 * setdefault() of dicts.
 *
 * @param node the node to get the child from
 * @param ch the character to use for the transition
 *
 * @return the child if the ch transition existed or the new one created for it
 */
TrieNode *SystemTrie::setDefaultChild(TrieNode *node, wchar_t ch) {
  map<wchar_t, TrieNode*>::const_iterator it;

  it = node->children.find(ch);
  if (it != node->children.end()) {
    return it->second;
  } else {
    TrieNode *child = new TrieNode;
    child->ruleNumber = NaRuleNumber;
    node->children[ch] = child;
    return child;
  }
}

/**
 * Insert a star node which can accept any character except <  and >.
 *
 * @param node the node to insert the star at
 *
 * @return the last node after inserting the star
 */
TrieNode *SystemTrie::insertStar(TrieNode *node) {
  // Create a new child node with the '*' special symbol.
  TrieNode *starNode = setDefaultChild(node, L'*');

  // Add a reference to itself to accept every alphabet char.
  starNode->children[L'*'] = starNode;

  return starNode;
}

/**
 * Insert a <*> to match a series of tags.
 *
 * @param node the node to insert the star at
 *
 * @return the node containing the *
 */
TrieNode *SystemTrie::insertTagStar(TrieNode *node) {
  TrieNode *auxNode = node;

  // Insert the star node following the last one.
  auxNode = setDefaultChild(auxNode, L'*');

  // Add a reference to itself to accept every alphabet char.
  auxNode->children[L'*'] = auxNode;
  TrieNode *starNode = auxNode;

  // Add the end and the start of a possible next tag.
  auxNode = setDefaultChild(auxNode, L'>');
  auxNode = setDefaultChild(auxNode, L'<');

  // Add the reference to the star node and the cycle is complete.
  auxNode->children[L'*'] = starNode;

  return starNode;
}

/**
 * Internal method used only by this class to insert part of a pattern, starting
 * in a node passed as parameter.
 *
 * @param pattern the pattern to insert
 * @param ruleNumber the rule number associated to a number if exists
 * @param node the node from where to start inserting the pattern
 *
 * @return the last node after inserting the pattern
 */
TrieNode *SystemTrie::insertPattern(const wstring &pattern, int ruleNumber,
    TrieNode *node) {
  wstring patternLowered = VMWstringUtils::lemmaToLower(pattern);

  TrieNode *curNode = node;

  wchar_t ch;
  for (unsigned int i = 0; i < patternLowered.size(); i++) {
    ch = patternLowered[i];
    if (ch == L'*') {
      curNode = insertTagStar(curNode);
    } else {
      curNode = setDefaultChild(curNode, ch);
    }
  }

  // If a pattern already exists, keep the first rule on the rule's file.
  int curRuleNumber = curNode->ruleNumber;
  if (ruleNumber != NaRuleNumber) {
    if (curRuleNumber == NaRuleNumber) {
      curNode->ruleNumber = ruleNumber;
    } else {
      if (curRuleNumber != ruleNumber) {
        wcerr << L"Paths to rule " << ruleNumber << L" blocked by rule "
              << curRuleNumber << L"." << endl;
      }
      curNode->ruleNumber = min(curRuleNumber, ruleNumber);
    }
  }

  return curNode;
}
