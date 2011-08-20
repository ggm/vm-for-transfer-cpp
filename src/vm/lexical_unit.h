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

using namespace std;

enum LU_PART {
  WHOLE, LEM, LEMH, LEMQ, TAGS, CHCONTENT
};

/// Define an interface for a lexical unit.
class LexicalUnit {

public:

  virtual ~LexicalUnit() { };

  virtual void parse() = 0;
  virtual wstring getWhole() const = 0;
  virtual wstring getPart(LU_PART) = 0;
  virtual void changePart(LU_PART, const wstring &) = 0;
  virtual void modifyTag(const wstring &, const wstring &) = 0;

private:

};

#endif /* LEXICAL_UNIT_H_ */
