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

#include <event_handler.h>

#include <iostream>
#include <sstream>

#include <compiler_exception.h>
#include <wstring_utils.h>

EventHandler::EventHandler() {
  codeGenerator = NULL;
}

EventHandler::EventHandler(const EventHandler &e) {
  copy(e);
}

EventHandler::~EventHandler() {
  codeGenerator = NULL;
}

EventHandler& EventHandler::operator=(const EventHandler &e) {
  if (this != &e) {
    this->~EventHandler();
    this->copy(e);
  }
  return *this;
}

void EventHandler::copy(const EventHandler &e) {
  transferStage = e.transferStage;
  transferDefault = e.transferDefault;
  codeGenerator = e.codeGenerator;
  defCats = e.defCats;
  currentDefCat = e.currentDefCat;
  defAttrs = e.defAttrs;
  currentDefAttr = e.currentDefAttr;
  defVars = e.defVars;
  defLists = e.defLists;
  currentDefList = e.currentDefList;
}

/**
 * Set one of the derived code generators to use.
 *
 * @param codeGenerator the code generator to use.
 */
void EventHandler::setCodeGenerator(CodeGenerator *codeGenerator) {
  this->codeGenerator = codeGenerator;
}

/**
 * Throw an error so the compiler can handle it.
 *
 * @param event the event which generated the error
 * @param msg the message to be shown
 */
void EventHandler::throwError(const Event &event, const wstring &msg) const {
  try {
    wstringstream exc;
    exc << L"line " << event.getLineNumber() << L", " << msg;
    throw CompilerException(exc.str());
  } catch (CompilerException &c) {
    throw c;
  }
}

/**
 * Check if an event has an attribute.
 *
 * @param event the event which has to have to attribute
 * @param attr the attribute to find
 */
void EventHandler::checkAttributeExists(const Event &event,
    wstring attr) const {
  if (!event.hasAttribute(attr)) {
    wstringstream msg;
    msg << event.getName() << L" needs attribute " << attr << L".";
    throwError(event, msg.str());
  }
}

/**
 * Check if a macro definition is correct. Check the name and the number of
 * parameters.
 *
 * @param event the event containing the macro
 */
void EventHandler::checkMacro(const Event &event) const {
  wstring name = event.getAttribute(L"n");

  if (!symbolTable.macroExists(name)) {
    wstringstream msg;
    msg << L"macro '" << name << L"' doesn't exist.";
    throwError(event, msg.str());
  }

  // Check if the number of parameters passed is correct.
  int numParams = event.getNumChildren();
  int numParamsSymb = symbolTable.getMacro(name).getNumParameters();
  if (numParams != numParamsSymb) {
    wstringstream msg;
    msg << L"macro '" << name << L"' needs " << numParamsSymb
        << L" parameters, passed " << numParams;
    throwError(event, msg.str());
  }

}

/**
 * Unescape values of the xml files (<, >) and leaves them on tag form.
 *
 * @param wstr the wide string containing &amp;lt; &amp;gt;
 *
 * @return the wide string with the correct symbols (<, >)
 */
wstring EventHandler::unEscape(wstring &wstr) const {
  wstr = WstringUtils::replace(wstr, L"&lt;", L"<");
  wstr = WstringUtils::replace(wstr, L"&gt;", L">");
  wstr = WstringUtils::replace(wstr, L"&amp;", L"");
  return wstr;
}

/**
 * Do all the necessary operations and the end of parsing. For now, check all
 * remaining macros.
 */
void EventHandler::handleEndOfParsing() {
  for (unsigned int i = 0; i < uncheckedMacros.size(); i++) {
    checkMacro(uncheckedMacros[i]);
  }
}

void EventHandler::handleTransferStart(const Event &event) {
  transferStage = TRANSFER;

  wstring value = event.getAttribute(L"default");
  if (value == L"chunk")
    transferDefault = CHUNK;
  else
    transferDefault = LU;

  codeGenerator->genTransferStart(event);
}

void EventHandler::handleTransferEnd(const Event &event) {
  handleEndOfParsing();
}

void EventHandler::handleInterchunkStart(const Event &event) {
  transferStage = INTERCHUNK;
  codeGenerator->genInterchunkStart(event);
}

void EventHandler::handleInterchunkEnd(const Event &event) {
  handleEndOfParsing();
}

void EventHandler::handlePostchunkStart(const Event &event) {
  transferStage = POSTCHUNK;
  codeGenerator->genPostchunkStart(event);
}

void EventHandler::handlePostchunkEnd(const Event &event) {
  handleEndOfParsing();
}

void EventHandler::handleDefCatStart(const Event & event) {
  checkAttributeExists(event, L"n");
  wstring defCatId = event.getAttribute(L"n");
  currentDefCat = &(defCats[defCatId]);
}

void EventHandler::handleDefCatEnd(const Event & event) {
  currentDefCat = NULL;
}

void EventHandler::handleCatItemStart(const Event & event) {
  wstring catItem = L"";

  if (transferStage == POSTCHUNK) {
    checkAttributeExists(event, L"name");
    catItem = event.getAttribute(L"name");
  } else {
    // lemma attribute is optional.
    if (event.hasAttribute(L"lemma")) {
      catItem = event.getAttribute(L"lemma");
    }

    if (event.hasAttribute(L"tags")) {
      wstring tagsAttr = event.getAttribute(L"tags");
      vector<wstring> tags = WstringUtils::wsplit(tagsAttr, L'.');
      for (unsigned int i = 0; i < tags.size(); i++) {
        catItem += L'<';
        catItem += tags[i];
        catItem += L'>';
      }
    }
  }

  currentDefCat->push_back(catItem);
}

void EventHandler::handleDefAttrStart(const Event &event) {
  checkAttributeExists(event, L"n");
  wstring defAttrId = event.getAttribute(L"n");
  currentDefAttr = &(defAttrs[defAttrId]);
}

void EventHandler::handleDefAttrEnd(const Event &event) {
  currentDefAttr = NULL;
}

void EventHandler::handleAttrItemStart(const Event &event) {
  checkAttributeExists(event, L"tags");
  wstring attrItem = L"";

  wstring tagsAttr = event.getAttribute(L"tags");
  vector<wstring> tags = WstringUtils::wsplit(tagsAttr, L'.');
  for (unsigned int i = 0; i < tags.size(); i++) {
    attrItem += L'<';
    attrItem += tags[i];
    attrItem += L'>';
  }

  currentDefAttr->push_back(attrItem);
}

void EventHandler::handleDefVarStart(const Event &event) {
  checkAttributeExists(event, L"n");
  wstring varName = event.getAttribute(L"n");

  wstring defaultValue = L"";
  if (event.hasAttribute(L"v")) {
    defaultValue = event.getAttribute(L"v");
    if (defaultValue.find(L';') != string::npos) {
      defaultValue = unEscape(defaultValue);
    }
    codeGenerator->genDefVarStart(event, defaultValue);
  }

  defVars[varName] = defaultValue;
}

void EventHandler::handleDefListStart(const Event &event) {
  checkAttributeExists(event, L"n");
  wstring defListId = event.getAttribute(L"n");
  currentDefList = &(defLists[defListId]);
}

void EventHandler::handleDefListEnd(const Event &event) {
  currentDefList = NULL;
}

void EventHandler::handleDefListItemStart(const Event &event) {
  checkAttributeExists(event, L"v");

  wstring listItem = event.getAttribute(L"v");
  currentDefList->push_back(listItem);
}

void EventHandler::handleSectionDefMacrosStart(const Event &event) {

}

void EventHandler::handleDefMacroStart(const Event &event) {
  checkAttributeExists(event, L"n");
  wstring name = event.getAttribute(L"n");

  checkAttributeExists(event, L"npar");
  int npar;
  wstringstream ws(event.getAttribute(L"npar"));
  ws >> npar;

  symbolTable.addMacro(name, npar);

  codeGenerator->genDefMacroStart(event);
}
void EventHandler::handleDefMacroEnd(const Event &event) {
  codeGenerator->genDefMacroEnd(event);
}

void EventHandler::handleCallMacroStart(const Event &event) {
  checkAttributeExists(event, L"n");
  codeGenerator->genCallMacroStart(event);
}

void EventHandler::handleCallMacroEnd(const Event &event) {
  wstring macroName = event.getAttribute(L"n");

  // In one pass we can only check for the macros already parsed, so we add
  // the other ones to check an the end.
  if (symbolTable.macroExists(macroName)) {
    checkMacro(event);
  } else {
    Event e = event;
    uncheckedMacros.push_back(e);
  }

  codeGenerator->genCallMacroEnd(event);
}
