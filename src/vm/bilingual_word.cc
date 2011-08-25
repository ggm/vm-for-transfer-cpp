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

}

BilingualWord::BilingualWord(const BilingualWord &bw) {
  copy(bw);
}

BilingualWord::~BilingualWord() {

}

BilingualWord& BilingualWord::operator=(const BilingualWord &bw) {
  if (this != &bw) {
    this->~BilingualWord();
    this->copy(bw);
  }
  return *this;
}

void BilingualWord::copy(const BilingualWord &bw) {
  source = bw.source;
  target = bw.target;
}

BilingualLexicalUnit* BilingualWord::getSource() {
  return &source;
}

BilingualLexicalUnit* BilingualWord::getTarget() {
  return &target;
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

  while (input.get(ch)) {
    if (ignoreMultipleTargets && ch != L'$') {
      continue;
    } else if (escapeNextChar) {
      token += ch;
      escapeNextChar = false;
    } else if (ch == L'\\') {
      token += ch;
      escapeNextChar = true;
    } else if (ch == L'^') {
      word = new BilingualWord();

      blanks.push_back(token);
      token = L"";
    } else if (ch == L'$') {
      word->target = BilingualLexicalUnit(token);
      words.push_back(word);
      token = L"";
      ignoreMultipleTargets = false;
      sourceSet = false;
    } else if (ch == L'/') {
      if (!sourceSet) {
        word->source = BilingualLexicalUnit(token);
        token = L"";
        sourceSet = true;
      } else {
        ignoreMultipleTargets = true;
      }
    } else {
      token += ch;
    }
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
  wstring sourceWhole = bw.source.getWhole();
  wstring targetWhole = bw.target.getWhole();

  wos << L"^" << sourceWhole << L"/" << targetWhole << L"$: "
      << bw.source << L"/" << bw.target;

  return wos;
}
