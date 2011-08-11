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

#include "lexical_unit.h"

LexicalUnit::LexicalUnit() {
  whole = L"";
  lem = L"";
  lemh = L"";
  lemq = L"";
  tags = L"";
  isParsed = false;
  lemqBeforeTags = false;
}

LexicalUnit::LexicalUnit(const wstring &whole) {
  lem = L"";
  lemh = L"";
  lemq = L"";
  tags = L"";
  isParsed = false;
  lemqBeforeTags = false;

  this->whole = whole;
}

LexicalUnit::LexicalUnit(const LexicalUnit &lu) {
  copy(lu);
}

LexicalUnit::~LexicalUnit() {

}

LexicalUnit& LexicalUnit::operator=(const LexicalUnit &lu) {
  if (this != &lu) {
    this->~LexicalUnit();
    this->copy(lu);
  }
  return *this;
}

void LexicalUnit::copy(const LexicalUnit &lu) {
  whole = lu.whole;
  lem = lu.lem;
  lemh = lu.lemh;
  lemq = lu.lemq;
  tags = lu.tags;
  isParsed = lu.isParsed;
  lemqBeforeTags = lu.lemqBeforeTags;
}

/**
 * Parse the content of the whole attribute and fill all the individual
 * components.
 */
void LexicalUnit::parse() {
  // Get the positions of the key characters.
  size_t tag = whole.find(L'<');
  size_t head = whole.find(L'#');
  size_t size = whole.size();

  if (tag != wstring::npos) {
    // Set tags depending on if the '#' is before or after tags.
    if (head == wstring::npos || head < tag) {
      lemqBeforeTags = true;
      lem = whole.substr(0, tag);
      tags = whole.substr(tag, size);
    } else {
      lemqBeforeTags = false;
      lem = whole.substr(0, tag) + whole.substr(head, size);
      tags = whole.substr(tag, head - tag);
    }
  } else {
    // If there isn't any tag, the lemma is everything.
    lem = whole;
    tags = L"";
  }

  if (head != wstring::npos) {
    // Set lemh, lemq depending on if the '#' is before or after tags.
    if (tag == wstring::npos || head < tag) {
      lemh = whole.substr(0, head);
      lemq = whole.substr(head, tag - head);
    } else {
      lemh = whole.substr(0, tag);
      lemq = whole.substr(head, size);

    }
  } else {
    // If it isn't a multiword, then the lemh is the lemma.
    lemh = lem;
    lemq = L"";
  }

  isParsed = true;
}

/**
 * Get the whole attribute of the lexical unit. If the lexical unit is not
 * parsed, just return the unparsed whole to save processing time. If the
 * unit is parsed, return the combination of the individual components.
 *
 * @return the whole lexical unit
 */
wstring LexicalUnit::getWhole() const {
  if (!isParsed) {
    return whole;
  } else {
    if (lemqBeforeTags) {
      return lemh + lemq + tags;
    } else {
      return lemh + tags + lemq;
    }
  }
}

/**
 * Get an attribute of the lexical unit.
 *
 * @param part the part to get
 *
 * @return the part
 */
wstring LexicalUnit::getPart(LU_PART part) {
  // If we want a part but the lu is not parsed, we need to parse it.
  if (part != WHOLE && !isParsed) {
    parse();
  }

  switch (part) {
  case WHOLE:
    return getWhole();
  case LEM:
    return lem;
  case LEMH:
    return lemh;
  case LEMQ:
    return lemq;
  case TAGS:
    return tags;
  default:
    return whole;
  }
}

/**
 * Change the value of a component of the lexical unit.
 *
 * @param part the part to change
 * @param value the new value to set
 */
void LexicalUnit::changePart(LU_PART part, const wstring &value) {
  // If we want to change a part but the lu is not parsed, we need to parse it.
  if (part != WHOLE && !isParsed) {
    parse();
  }

  switch (part) {
  case WHOLE:
    // If we change the whole we set the lexical unit as unparsed.
    whole = value;
    isParsed = false;
    break;
  case LEM:
    lem = value;
    break;
  case LEMH:
    lemh = value;
    break;
  case LEMQ:
    lemq = value;
    break;
  case TAGS:
    tags = value;
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
void LexicalUnit::modifyTag(const wstring &tag, const wstring &value) {
  // If we want to change a tag but the lu is not parsed, we need to parse it.
  if (!isParsed) {
    parse();
  }

  tags.replace(tags.find(tag), tag.size(), value);
}

wostream& operator<<(wostream &wos, const LexicalUnit &lu) {
  wos << L"[" << L"'lem': '" << lu.lem << L"', 'lemh': '" << lu.lemh;
  wos << L"', 'lemq': '" << lu.lemq << L"', 'tags': '" << lu.tags << L"'}";
  return wos;
}
