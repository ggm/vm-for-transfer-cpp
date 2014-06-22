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

#include "chunk_word.h"

#include <locale>
#include <sstream>

#include "vm_wstring_utils.h"

ChunkWord::ChunkWord() {
  chunk = NULL;
}

ChunkWord::ChunkWord(const ChunkWord &c) {
  copy(c);
}

ChunkWord::~ChunkWord() {
  for (unsigned int i = 0; i < content.size(); i++) {
    delete content[i];
    content[i] = NULL;
  }

  if (chunk != NULL) {
    delete chunk;
    chunk = NULL;
  }
}

ChunkWord& ChunkWord::operator=(const ChunkWord &c) {
  if (this != &c) {
    this->~ChunkWord();
    this->copy(c);
  }
  return *this;
}

void ChunkWord::copy(const ChunkWord &c) {
  chunk = new ChunkLexicalUnit(*c.chunk);
  content = c.content;
  blanks = c.blanks;
}

/**
 * Get the chunk lexical unit of the word.
 *
 * @return the lexical unit
 */
ChunkLexicalUnit* ChunkWord::getChunk() {
  return chunk;
}

/**
 * Solve the references, inside the chcontent, to the chunk tags.
 */
void ChunkWord::solveReferences() {
  vector<wstring> tagsValues;

  wstring tags = chunk->getPart(TAGS);

  wstring::size_type startPos = 0;
  wstring::size_type delimPos = tags.find(L'>', startPos);
  while (delimPos != wstring::npos) {
    wstring token = tags.substr(startPos, delimPos - startPos + 1);
    tagsValues.push_back(token);
    startPos = delimPos + 1;
    delimPos = tags.find(L'>', startPos);
  }

  wstring chcontent = chunk->getPart(CHCONTENT);
  wstring newChcontent = chcontent;
  wstring newWhole = chunk->getPart(WHOLE);

  for (unsigned int i = 1; i < chcontent.size() - 1; i++) {
    wchar_t ch = chcontent[i];
    if (chcontent[i - 1] == L'<' && chcontent[i + 1] == L'>') {
      if (VMWstringUtils::isdigit(ch)) {
        unsigned int pos = VMWstringUtils::charTo<unsigned int>(ch);
        wstring tag;
        if(pos - 1 < tagsValues.size()) {
          tag = tagsValues[pos - 1];
        } else {
          wcerr << "WARNING: tag linked from position " << pos
                << " not found in tag string " <<  tags << "." << endl;
          tag = L"";
        }
        newWhole = replaceReference(newWhole, ch, tag);
        newChcontent = replaceReference(newChcontent, ch, tag);
      }
    }
  }

  chunk->changePart(WHOLE, newWhole);
  chunk->changePart(CHCONTENT, newChcontent);
}

/**
 * Replace references to a tag with the actual tag.
 *
 * @param container the wide string containing the references
 * @param pos position of the reference to replace, as found in the container
 * @param tag the actual tag to put in place
 *
 * @return the container with the references replaced
 */
wstring ChunkWord::replaceReference(const wstring &container, wchar_t pos,
    const wstring &tag) {
  wstring newContainer = container;
  wstring reference;
  reference += L'<';
  reference += pos;
  reference += L'>';

  newContainer = VMWstringUtils::replace(newContainer, reference, tag);

  return newContainer;
}

/**
 * Set the content of the chunk word as a list of lexical units and apply the
 * postchunk rule of setting the case of the lexical units as the one of the
 * chunk pseudolemma.
 */
void ChunkWord::parseChunkContent() {
  // Depending on the case, change all cases or just the first lexical unit.
  CASE pseudoLemmaCase = VMWstringUtils::getCase(chunk->getPart(LEM));
  bool upperCaseAll = false;
  bool firstUpper = false;
  if (pseudoLemmaCase == AA) {
    upperCaseAll = true;
  } else if (pseudoLemmaCase == Aa) {
    firstUpper = true;
  }

  // The first blank is the one before the chunk name.
  blanks.push_back(L"");
  bool firstLu = true;

  wstring token = L"";
  wstring chcontent = chunk->getPart(CHCONTENT);
  wchar_t ch;
  BilingualLexicalUnit *lu;
  bool escapeNextChar = false;

  // Ignore first and last chars '{' and '}'.
  for (unsigned int i = 1; i < chcontent.size() - 1; i++) {
    ch = chcontent[i];
    if (escapeNextChar) {
      token += ch;
      escapeNextChar = false;
    } else if (ch == L'\\') {
      token += ch;
      escapeNextChar = true;
    } else if (ch == L'^') {
      // After the first blank, append the blanks between lexical units.
      if (firstLu) {
        firstLu = false;
      } else {
        blanks.push_back(token);
      }
      token = L"";
    } else if (ch == L'$') {
      lu = new BilingualLexicalUnit(token);

      if (upperCaseAll) {
        changeLemmaCase(*lu, pseudoLemmaCase);
      } else if (firstUpper) {
        changeLemmaCase(*lu, pseudoLemmaCase);
        firstUpper = false;
      }

      content.push_back(lu);

      token = L"";
    } else {
      token += ch;
    }
  }
}

/**
 * Update the chcontent when a lu inside the chunk changes.
 *
 * @param oldLu the lexical unit being replaced
 * @param newLu the new lexical unit
 */
void ChunkWord::updateChunkContent(const wstring & oldLu,
    const wstring & newLu) {
  wstring chcontent = chunk->getPart(CHCONTENT);

  size_t pos = chcontent.find(oldLu);
  if (pos != wstring::npos) {
    wstring ch = chcontent.replace(pos, oldLu.size(), newLu);
    chunk->changePart(CHCONTENT, ch);
  }
}

/**
 * Get a reference to one of the lexical units of the chunk content.
 *
 * @param pos the position of the lexical unit
 *
 * @return the reference to the lexical unit inside the chunk content.
 */
BilingualLexicalUnit* ChunkWord::getContentLexicalUnit(int pos) {
  if (content.size() == 0) {
    parseChunkContent();
  }

  return content[pos];
}

/**
 * Get the number of lexical units inside the chunk.
 *
 * @return the number of lexical units inside the chunk.
 */
int ChunkWord::getLuCount() {
  return content.size();
}

/**
 * Get the blank in the specified position. These blanks are the ones between
 * lexical units inside the chunk and are used in the postchunk stage.
 *
 * @param pos the position of the blank
 *
 * @return the blank in position pos or "" if the position is past the end
 */
wstring ChunkWord::getBlank(unsigned int pos) {
  if (pos < blanks.size()) {
    return blanks[pos];
  } else {
    return L"";
  }
}

/**
 * Tokenize the input in \verbatim ^name<tags>{^...$} \endverbatim tokens and
 * create corresponding ChunkWords.
 *
 * @param input the input stream to parse
 * @param words a collection of words to be filled.
 * @param blanks a collection of blanks to be filled.
 * @param solveRefs if references to chunk tags should be solved or not
 * @param parseContent if chunk content should be parsed and lus created or not
 */
void ChunkWord::tokenizeInput(wistream &input, vector<TransferWord*> &words,
    vector<wstring> &blanks, bool solveRefs, bool parseContent) {
  wstring token = L"";
  bool chunkStart = true;
  bool escapeNextChar = false;

  wchar_t ch;
  ChunkWord *word = new ChunkWord();

  wstring buffer(1024, L'\0');

  while (!input.eof()) {
    input.read(&buffer[0], buffer.size());
    int size = input.gcount();
    int startToken = 0;
    for (int i = 0; i < size; ++i) {
      ch = buffer[i];
      if (escapeNextChar) {
        escapeNextChar = false;
      }  else if (ch == L'\\') {
        escapeNextChar = true;
      } else if (ch == L'^') {
        // Read the ^ and $ of the lexical units but not of the chunks.
        if (chunkStart) {
          // Characters between chunks are treated like superblanks.
          token += buffer.substr(startToken, i - startToken);
          blanks.push_back(token);
          token = L"";
          startToken = i + 1;
          chunkStart = false;
        }
      } else if (ch == L'$') {
        if (chunkStart) {
          token += buffer.substr(startToken, i - startToken);
          startToken = i + 1;
        }
      } else if (ch == L'}') {
        token += buffer.substr(startToken, i - startToken + 1);
        startToken = i + 1;
        word->chunk = new ChunkLexicalUnit(token);

        if (solveRefs) {
          word->solveReferences();
        }
        if (parseContent) {
          word->parseChunkContent();
        }

        words.push_back(word);

        chunkStart = true;
        token = L"";
        word = new ChunkWord();
      }
    }
    token += buffer.substr(startToken, size - startToken);
  }

  // Append the last superblank of the input, usually the '\n'.
  blanks.push_back(token);

  if (word != NULL) {
    delete word;
  }
}

/**
 * Change the case of the lemma to the passed as parameter.
 *
 * @param lu the lexical unit to change the case from
 * @param luCase the new case to apply.
 */
void ChunkWord::changeLemmaCase(BilingualLexicalUnit &lu, CASE luCase) {
  wstring oldLem = lu.getPart(LEM);
  wstring newLem = VMWstringUtils::changeCase(oldLem, luCase);

  lu.changePart(LEM, newLem);

  // Also, update the chcontent attribute of the chunk.
  updateChunkContent(oldLem, newLem);
}

wostream& operator<<(wostream &wos, const ChunkWord &cw) {
  wstring chunkWhole = cw.chunk->getWhole();

  wos << L"^" << chunkWhole << L"$: " << *cw.chunk << L", content = [";

  for (unsigned int i = 0; i < cw.content.size(); i++) {
    wos << L"^" << cw.content[i]->getWhole() << L"$: " << *cw.content[i]
        << L" ";
  }

  wos << L"]";

  return wos;
}
