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

#ifndef LEXICAL_UNIT_H_
#define LEXICAL_UNIT_H_

#include <string>
#include <iostream>

using namespace std;

enum LU_PART {
  WHOLE, LEM, LEMH, LEMQ, TAGS, CHCONTENT
};

/**
 *  Represent a lexical unit and all its attributes for the transfer stage.
 *  The attributes will be parsed on demand, that is, when the lexical unit
 *  is created, only the attribute whole, with the entire content of the
 *  lexical unit, will be stored. Only if one of the other attributes its
 *  needed the whole content will be parsed and split between components.
 */
class LexicalUnit {

  friend wostream& operator<<(wostream &, const LexicalUnit &);

public:

  LexicalUnit();
  LexicalUnit(const wstring &);
  LexicalUnit(const LexicalUnit &);
  ~LexicalUnit();
  LexicalUnit& operator=(const LexicalUnit &);
  void copy(const LexicalUnit &);

  void parse();
  wstring getWhole() const;
  wstring getPart(LU_PART);
  void changePart(LU_PART, const wstring &);
  void modifyTag(const wstring &, const wstring &);

private:

  /// The whole content of the lexical unit.
  wstring whole;

  /// The lemma of the lexical unit.
  wstring lem;

  /// The lemma's head of the lexical unit.
  wstring lemh;

  /// The lemma's queue of the lexical unit.
  wstring lemq;

  /// The tags of the lexical unit.
  wstring tags;

  /// If the lexical unit is parsed, its individual components are filled.
  bool isParsed;

  /// Store if the queue is stored before or after the tags.
  bool lemqBeforeTags;

};

#endif /* LEXICAL_UNIT_H_ */
