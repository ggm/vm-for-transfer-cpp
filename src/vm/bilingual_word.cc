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

LexicalUnit & BilingualWord::getSource() {
  return source;
}

LexicalUnit& BilingualWord::getTarget() {
  return target;
}

/**
 * Create a set of bilingual words from an input stream.
 *
 * @param input the input stream to parse
 * @param words a collection of words to be filled.
 * @param blanks a collection of blanks to be filled.
 */
void BilingualWord::tokenizeInput(wfstream &input,
    vector<TransferWord *> &words, vector<wstring> &blanks) {
  wstring token = L"";
  wstring sb = L"";
  bool insideSb = false;
  bool escapeNextChar = false;
  bool ignoreMultipleTargets = false;
  bool sourceSet = false;

  wchar_t ch;
  BilingualWord *word;

  while (input.get(ch)) {
    if (ignoreMultipleTargets && ch != L'$') {
      continue;
    } else if (ch == L'\\') {
      escapeNextChar = true;
    } else if (escapeNextChar) {
      token += ch;
      escapeNextChar = false;
    } else if (ch == L'^') {
      word = new BilingualWord();

      if (words.size() == 0 && blanks.size() == 0) {
        // If there aren't any blanks at the beginning, append an empty.
        blanks.push_back(token);
      } else if (words.size() == blanks.size()) {
        // Any character between lus is treated like a superblank.
        blanks.push_back(token);
      } else if (token != L"") {
        // If there are characters after the superblank, append them.
        wstring bl = blanks.back() + token;
        blanks.pop_back();
        blanks.push_back(bl);
      }
      token = L"";
    } else if (ch == L'$') {
      word->target = LexicalUnit(token);
      words.push_back(word);
      token = L"";
      ignoreMultipleTargets = false;
      sourceSet = false;
    } else if (ch == L'/') {
      if (!sourceSet) {
        word->source = LexicalUnit(token);
        token = L"";
        sourceSet = true;
      } else {
        ignoreMultipleTargets = true;
      }
    } else if (ch == L'[') {
      // If there are characters before the superblank, append them.
      if (token != L"") {
        sb += token;
        token = L"";
      }
      insideSb = true;
    } else if (ch == L']') {
      if (sb != L"") {
        blanks.push_back(sb);
      }
      insideSb = false;
      sb = L"";
    } else if (insideSb) {
      sb += ch;
    } else {
      token += ch;
    }
  }
}

wostream& operator<<(wostream &wos, const BilingualWord &bw) {
  wstring sourceWhole = bw.source.getWhole();
  wstring targetWhole = bw.target.getWhole();

  wos << L"^" << sourceWhole << L"/" << targetWhole << L"$: "
      << bw.source << L"/" << bw.target;

  return wos;
}
