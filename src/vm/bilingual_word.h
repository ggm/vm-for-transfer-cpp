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

#ifndef BILINGUAL_WORD_H_
#define BILINGUAL_WORD_H_

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "transfer_word.h"
#include "bilingual_lexical_unit.h"

using namespace std;

/// Represent a word as a source/target language pair.
class BilingualWord: public TransferWord {

  friend wostream& operator<<(wostream &, const BilingualWord &);

public:

  BilingualWord();
  BilingualWord(const BilingualWord &);
  virtual ~BilingualWord();
  BilingualWord& operator=(const BilingualWord &);
  void copy(const BilingualWord &);

  BilingualLexicalUnit* getSource();
  BilingualLexicalUnit* getTarget();

  static void tokenizeInput(wfstream &, vector<TransferWord *> &,
      vector<wstring> &);

private:

  /// The source language lexical unit.
  BilingualLexicalUnit source;

  /// The target language lexical unit.
  BilingualLexicalUnit target;
};

#endif /* BILINGUAL_WORD_H_ */
