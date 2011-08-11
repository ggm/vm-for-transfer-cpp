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

#ifndef CHUNK_WORD_H_
#define CHUNK_WORD_H_

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "transfer_word.h"
#include "lexical_unit.h"
#include "chunk_lexical_unit.h"
#include "vm_wstring_utils.h"

/// Represent a word as a chunk for the interchunk and postchunk stages.
class ChunkWord: TransferWord {

  friend wostream& operator<<(wostream &, const ChunkWord &);

public:

  ChunkWord();
  ChunkWord(const ChunkWord &);
  ~ChunkWord();
  ChunkWord& operator=(const ChunkWord &);
  void copy(const ChunkWord &);

  ChunkLexicalUnit& getChunk();
  void solveReferences();
  wstring replaceReference(const wstring &, wchar_t, const wstring &);
  void parseChunkContent();
  void updateChunkContent(const wstring &, const wstring &);

  static void tokenizeInput(wfstream &, vector<TransferWord *> &,
      vector<wstring> &, bool, bool);

private:

  /// The chunk lexical unit.
  ChunkLexicalUnit chunk;

  /// Content represented as a collection of lexical units.
  vector<LexicalUnit *> content;

  /// Blanks inside the chunk content (between lexical units) have to be stored.
  vector<wstring> blanks;

  void changeLemmaCase(LexicalUnit &, CASE);
};

#endif /* CHUNK_WORD_H_ */
