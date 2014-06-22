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
#include <unordered_map>

#include "vm_wstring_utils.h"
#include "string_pool.h"

static size_t getFirstTokenLength(const wchar_t* pattern) {
  if (pattern[0] == L'\0') return 0;

  wchar_t lookFor = L'<';
  if(pattern[0] == L'<') {
    lookFor = L'>';
  }
  size_t cnt = 1;
  while(pattern[cnt] != lookFor && pattern[cnt] != L'\0') {
    ++cnt;
  }

  if(lookFor == L'>') ++cnt;
  return cnt;
}

TrieNode::TrieNode(int ruleNumber) {
  this->ruleNumber = ruleNumber;
  this->starTransition = NULL;
  this->starTagTransition = NULL;
}

TrieNode::TrieNode() {
  this->ruleNumber = NaRuleNumber;
  this->starTransition = NULL;
  this->starTagTransition = NULL;
}

TrieNode *TrieNode::getOrCreateStarTransition() {
  if(starTransition == NULL) {
    starTransition = new TrieNode;
    starTransition->starTransition = starTransition;
  }
  return starTransition;
}

TrieNode *TrieNode::getOrCreateStarTagTransition() {
  if(starTagTransition == NULL) {
    starTagTransition = new TrieNode;
    starTagTransition->starTagTransition = starTagTransition;
  }
  return starTagTransition;
}

bool TrieNode::containsTransitionBy(const wstring& wstr) const {
  return links.find(wstr) != links.end();
}

TrieNode* TrieNode::_insertPattern(const wchar_t* pattern, int ruleNumber) {
  size_t tokenLength = getFirstTokenLength(pattern);
  wstring patternToken(pattern, tokenLength);

  if(tokenLength == 0) {
    int curRuleNumber = this->ruleNumber;
    if (ruleNumber != NaRuleNumber) {
      if (curRuleNumber == NaRuleNumber) {
        this->ruleNumber = ruleNumber;
      } else {
        if (curRuleNumber != ruleNumber) {
          wcerr << L"MY: Paths to rule " << ruleNumber << L" blocked by rule "
                << curRuleNumber << L"." << endl;
        }
        this->ruleNumber = min(curRuleNumber, ruleNumber);
      }
    }
    return this;
  } else {
    TrieNode *nextNode = NULL;
    if(patternToken == L"<*>") {
      nextNode = getOrCreateStarTagTransition();
    } else {
      int patternTokenCode = StringPool::getCode(patternToken);
      if(!containsTransitionBy(patternToken)) {
        TrieNode* newNode = new TrieNode;
        links[patternToken] = newNode;
        intLinks[patternTokenCode] = newNode;
      }
      nextNode = intLinks[patternTokenCode];
    }
    return nextNode->_insertPattern(pattern + tokenLength, ruleNumber);
  }
}

TrieNode* TrieNode::insertPattern(const wstring& pattern, int ruleNumber) {
  wstring patternLowered = VMWstringUtils::lemmaToLower(pattern);
  return _insertPattern(patternLowered.c_str(), ruleNumber);
}

void TrieNode::pushNextNodes(const wstring& wstr, list<TrieNode*>& nodes) const {
  if(wstr[0] == L'*') return;

  // TODO optimize.
  const auto& it = links.find(wstr);
  if(it != links.end()) {
    nodes.push_back(it->second);
  }

  TrieNode* whichStarTransition = starTransition;
  if(wstr[0] == L'<') whichStarTransition = starTagTransition;
  if(whichStarTransition) {
    nodes.push_back(whichStarTransition);
  }
}

SystemTrie::SystemTrie() {
  root = new TrieNode;
}

SystemTrie::~SystemTrie() {
  queue<TrieNode *> q;
  unordered_set<TrieNode *> allNodes;
  q.push(root);
  allNodes.insert(root);

  while (!q.empty()) {
    TrieNode *node = q.front();
    q.pop();
    TrieNode *starTransition = node->starTransition;
    if (starTransition != NULL && allNodes.find(starTransition) == allNodes.end()) {
      q.push(starTransition);
      allNodes.insert(starTransition);
    }
    TrieNode *starTagTransition = node->starTagTransition;
    if (starTagTransition != NULL && allNodes.find(starTagTransition) == allNodes.end()) {
      q.push(starTagTransition);
      allNodes.insert(starTagTransition);
    }
    for (const auto& link : node->links) {
      TrieNode *linkedNode = link.second;
      if (linkedNode != NULL && allNodes.find(linkedNode) == allNodes.end()) {
        q.push(linkedNode);
        allNodes.insert(linkedNode);
      }
    }
  }
  for (TrieNode *node : allNodes) {
    delete node;
  }
}

void SystemTrie::addPattern(const vector<wstring> &pattern, int ruleNumber) {
  // Only the last part of the pattern matches to the ruleNumber.
  int rule = NaRuleNumber;
  unsigned int numPatterns = pattern.size();

  vector<TrieNode *> curNodes;
  curNodes.push_back(root);

  for (unsigned int i = 0; i < numPatterns; i++) {
    const wstring& part = pattern[i];

    vector<TrieNode *> lastNodes;
    // If it's the last part of the pattern, insert it with the rule number.
    if (i == numPatterns - 1) {
      rule = ruleNumber;
    }

    wstring option = L"";

    for (TrieNode* node : curNodes) {
      for (wchar_t ch : part) {
        // If the a part of a pattern has different options (op1|op2|op3).
        if (ch == L'|') {
          // Add each one, inserting a star if it starts with '<'.
          if (option[0] == L'<') {
            TrieNode *starNode = node->getOrCreateStarTransition();
            lastNodes.push_back(starNode->insertPattern(option, rule));
          } else {
            lastNodes.push_back(node->insertPattern(option, rule));
          }
          option = L"";
        } else {
          option += ch;
        }
      }

      // Add the single part or the last of its options.
      if (option[0] == L'<') {
        TrieNode *starNode = node->getOrCreateStarTransition();
        lastNodes.push_back(starNode->insertPattern(option, rule));
      } else {
        lastNodes.push_back(node->insertPattern(option, rule));
      }

      option = L"";
    }

    // Continue adding from where we ended.
    curNodes = lastNodes;
  }
}

list<TrieNode*> SystemTrie::getPatternNodes(const wstring &pattern, TrieNode *startNode) {
  list<TrieNode *> curNodes;

  if (pattern.size() == 0) {
    return curNodes;
  }

  wstring patternLowered = VMWstringUtils::lemmaToLower(pattern);

  const wchar_t *p = patternLowered.c_str();
  curNodes.push_back(startNode);

  while(*p != L'\0') {
    size_t tokenLength = getFirstTokenLength(p);
    const wstring currentToken(p, tokenLength);
    p += tokenLength;

    list<TrieNode*> nextNodes;
    for(TrieNode* node : curNodes) {
      node->pushNextNodes(currentToken, nextNodes);
    }

    curNodes = std::move(nextNodes);

    if (curNodes.size() == 0) {
      return curNodes;
    }
  }

  return curNodes;
}

list<TrieNode*> SystemTrie::getPatternNodes(const wstring& pattern) {
  return getPatternNodes(pattern, root);
}

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
