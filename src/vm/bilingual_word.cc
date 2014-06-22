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

#include "bilingual_word.h"

BilingualWord::BilingualWord() {
  source = NULL;
  target = NULL;
}

BilingualWord::BilingualWord(const BilingualWord &bw) {
  copy(bw);
}

BilingualWord::~BilingualWord() {
  if (source != NULL) {
    delete source;
    source = NULL;
  }

  if (target != NULL) {
    delete target;
    target = NULL;
  }
}

BilingualWord& BilingualWord::operator=(const BilingualWord &bw) {
  if (this != &bw) {
    this->~BilingualWord();
    this->copy(bw);
  }
  return *this;
}

void BilingualWord::copy(const BilingualWord &bw) {
  source = new BilingualLexicalUnit(*bw.source);
  target = new BilingualLexicalUnit(*bw.target);
}

BilingualLexicalUnit* BilingualWord::getSource() {
  return source;
}

BilingualLexicalUnit* BilingualWord::getTarget() {
  return target;
}

/**
 * Create a set of bilingual words from an input stream.
 *
 * @param input the input stream to parse
 * @param words a collection of words to be filled.
 * @param blanks a collection of blanks to be filled.
 */
void BilingualWord::tokenizeInput(wistream &input,
    vector<TransferWord *> &words, vector<wstring> &blanks) {
  wstring token = L"";
  bool escapeNextChar = false;
  bool ignoreMultipleTargets = false;
  bool sourceSet = false;

  wchar_t ch;
  BilingualWord *word;

  wstring buffer(1024, L'\0');

  while (!input.eof()) {
    input.read(&buffer[0], buffer.size());
    int size = input.gcount();
    int startToken = 0;
    for (int i = 0; i < size; ++i) {
      ch = buffer[i];
      if (ignoreMultipleTargets && ch != L'$') {
          token += buffer.substr(startToken, i - startToken);
          startToken = i + 1;
      } else if (escapeNextChar) {
        escapeNextChar = false;
      } else if (ch == L'\\') {
        escapeNextChar = true;
      } else if (ch == L'^') {
        word = new BilingualWord();

        token += buffer.substr(startToken, i - startToken);
        blanks.push_back(token);
        startToken = i + 1;
        token = L"";
      } else if (ch == L'$') {
        token += buffer.substr(startToken, i - startToken);
        word->target = new BilingualLexicalUnit(token);

        words.push_back(word);
        startToken = i + 1;
        token = L"";
        ignoreMultipleTargets = false;
        sourceSet = false;
      } else if (ch == L'/') {
        token += buffer.substr(startToken, i - startToken);
        startToken = i + 1;
        if (!sourceSet) {
          word->source = new BilingualLexicalUnit(token);
          token = L"";
          sourceSet = true;
        } else {
          ignoreMultipleTargets = true;
        }
      }
    }
    token += buffer.substr(startToken, size - startToken);
  }

  // Add everything at the end until the last ']' as a superblank.
  size_t pos = token.rfind(L']');
  if (pos != wstring::npos) {
    blanks.push_back(token.substr(0, 1 + pos));
  } else {
    blanks.push_back(token);
  }
}

wostream& operator<<(wostream &wos, const BilingualWord &bw) {
  wstring sourceWhole = bw.source->getWhole();
  wstring targetWhole = bw.target->getWhole();

  wos << L"^" << sourceWhole << L"/" << targetWhole << L"$: "
      << *bw.source << L"/" << *bw.target;

  return wos;
}
