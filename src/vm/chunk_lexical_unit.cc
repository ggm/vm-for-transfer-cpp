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

#include "chunk_lexical_unit.h"

ChunkLexicalUnit::ChunkLexicalUnit() {
  whole = L"";
  pseudolemma = L"";
  chcontent = L"";
  tags = L"";
  isParsed = false;
}

ChunkLexicalUnit::ChunkLexicalUnit(const wstring &whole) {
  pseudolemma = L"";
  chcontent = L"";
  tags = L"";
  isParsed = false;

  this->whole = whole;
}

ChunkLexicalUnit::ChunkLexicalUnit(const ChunkLexicalUnit &c) {
  copy(c);
}

ChunkLexicalUnit::~ChunkLexicalUnit() {

}

ChunkLexicalUnit& ChunkLexicalUnit::operator=(const ChunkLexicalUnit &c) {
  if (this != &c) {
    this->~ChunkLexicalUnit();
    this->copy(c);
  }
  return *this;
}

void ChunkLexicalUnit::copy(const ChunkLexicalUnit &c) {
  whole = c.whole;
  pseudolemma = c.pseudolemma;
  tags = c.tags;
  chcontent = c.chcontent;
  isParsed = c.isParsed;
}

/**
 * Parse the content of the whole attribute and fill all the individual
 * components.
 */
void ChunkLexicalUnit::parse() {
  // Get the positions of the key characters.
  size_t tag = whole.find(L'<');
  size_t contentStart = whole.find(L'{');

  if (tag != wstring::npos and tag < contentStart) {
    // The lemma is everything until the first tag.
    pseudolemma = whole.substr(0, tag);
    tags = whole.substr(tag, contentStart - tag);
  } else {
    // If there isn't any tag, the lemma is everything until the '{'.
    pseudolemma = whole.substr(0, contentStart);
    tags = L"";
  }

  // Store the chunk contents with the '{' and '}'.
  chcontent = whole.substr(contentStart, whole.size());

  isParsed = true;
}

/**
 * Get the whole attribute of the lexical unit. If the lexical unit is not
 * parsed, just return the unparsed whole to save processing time. If the
 * unit is parsed, return the combination of the individual components.
 *
 * @return the whole lexical unit
 */
wstring ChunkLexicalUnit::getWhole() const {
  if (!isParsed) {
    return whole;
  } else {
    return pseudolemma + tags + chcontent;
  }
}

/**
 * Get an attribute of the lexical unit.
 *
 * @param part the part to get
 *
 * @return the part
 */
wstring ChunkLexicalUnit::getPart(LU_PART part) {
  // If we want a part but the lu is not parsed, we need to parse it.
  if (part != WHOLE && !isParsed) {
    parse();
  }

  switch(part) {
  case WHOLE: return getWhole();
  case LEM: /*FALL THROUGH*/
  case LEMH: return pseudolemma;
  case LEMQ: return L"";
  case TAGS: return tags;
  case CHCONTENT: return chcontent;
  /*jacob's new 'part' (from apertium's interchunk.cc:248) */
  case CONTENT: return chcontent.substr(1, chcontent.size() - 2);
  default: return whole;
  }
}

/**
 * Change the value of a component of the lexical unit.
 *
 * @param part the part to change
 * @param value the new value to set
 */
void ChunkLexicalUnit::changePart(LU_PART part, const wstring &value) {
  // If we want to change a part but the lu is not parsed, we need to parse it.
  if (part != WHOLE && !isParsed) {
    parse();
  }

  switch(part) {
  case WHOLE:
    // If we change the whole we set the lexical unit as unparsed.
    whole = value;
    isParsed = false;
    break;
  case LEM: /*FALL THROUGH*/
  case LEMH:
    pseudolemma = value;
    break;
  case TAGS:
    tags = value;
    break;
  case CHCONTENT:
    chcontent = value;
    break;
  default:
    break;
  }
}

/**
 * Replace an existing tag or combination of tags with a new value
 *
 * @param tag the tag or combination of tags to replace
 * @param value the new value to store in its place
 */
void ChunkLexicalUnit::modifyTag(const wstring &tag, const wstring &value) {
  // If we want to change a tag but the lu is not parsed, we need to parse it.
  if (!isParsed) {
    parse();
  }

  tags.replace(tags.find(tag), tag.size(), value);
}

wostream& operator<<(wostream &wos, const ChunkLexicalUnit &clu) {
  wos << L"{" << L"'lem': '" << clu.pseudolemma << L"', 'tags': '" << clu.tags;
  wos << L"', 'chcontent': '" << clu.chcontent << L"'}" ;
  return wos;
}
