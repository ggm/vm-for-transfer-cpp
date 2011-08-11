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

#ifndef CHUNK_LEXICAL_UNIT_H_
#define CHUNK_LEXICAL_UNIT_H_

#include <vector>
#include <string>

#include "lexical_unit.h"

using namespace std;

/**
 *  Represent a chunk and all its attributes for the inter/postchunk stage.
 *  The attributes will be parsed on demand, that is, when the lexical unit
 *  is created, only the attribute whole, with the entire content of the
 *  lexical unit, will be stored. Only if one of the other attributes its
 *  needed the whole content will be parsed and split between components.
 */
class ChunkLexicalUnit {

  friend wostream& operator<<(wostream &, const ChunkLexicalUnit &);

public:

  ChunkLexicalUnit();
  ChunkLexicalUnit(const wstring &);
  ChunkLexicalUnit(const ChunkLexicalUnit &);
  ~ChunkLexicalUnit();
  ChunkLexicalUnit& operator=(const ChunkLexicalUnit &);
  void copy(const ChunkLexicalUnit &);

  void parse();
  wstring getWhole() const;
  wstring getPart(LU_PART);
  void changePart(LU_PART, const wstring &);
  void modifyTag(const wstring &, const wstring &);

private:
  /// The whole content of the lexical unit.
  wstring whole;

  /// The pseudolemma of the chunk.
  wstring pseudolemma;

  /// The tags of the lexical unit.
  wstring tags;

  /// The content of the chunk.
  wstring chcontent;

  /// If the lexical unit is parsed, its individual components are filled.
  bool isParsed;
};

#endif /* CHUNK_LEXICAL_UNIT_H_ */
