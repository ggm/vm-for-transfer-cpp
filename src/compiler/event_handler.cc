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
 * Check if the event passed as parameter acts as a container.
 *
 * @param event the event to be checked
 *
 * @return true if acts as a container, false in other case.
 */
bool EventHandler::isContainer(const Event &event) const {
  const Event *parent = event.getParent();

  if (parent->getName() == L"let") {
    // If it's the first child, it's on the left, so it's a container.
    if (parent->getNumChildren() == 1) {
      return true;
    }
  }

  return false;
}

/**
 * Get the part attribute from an event. If the part is one of the predefined
 * (lem, lemq, ...) return that. If it's the name of one attr defined in a
 * def-attr, return its content. In other case, throw an exception.
 *
 * @param event the event to extract the part attribute
 *
 * @return a vector with the part or the def-attr content
 */
vector<wstring> EventHandler::getPartAttribute(const Event &event) {
  vector<wstring> partAttrs;
  wstring part = event.getAttribute(L"part");

  if (part == L"lem" || part == L"lemh" || part == L"lemq" || part == L"whole"
      || part == L"tags" || part == L"chcontent") {
    partAttrs.push_back(part);
  } else if (defAttrs.find(part) != defAttrs.end()) {
    partAttrs = defAttrs[part];
  } else {
    wstringstream msg;
    msg << L"attr '" << part << L"' doesn't exist.";
    throwError(event, msg.str());
  }

  return partAttrs;
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

void EventHandler::handleSectionRulesStart(const Event &event) {
  codeGenerator->genSectionRulesStart(event);
}

void EventHandler::handleSectionRulesEnd(const Event &event) {
  codeGenerator->genSectionRulesEnd(event);
}

void EventHandler::handleRuleStart(const Event &event) {
  codeGenerator->genRuleStart(event);
}

void EventHandler::handlePatternStart(const Event &event) {
  codeGenerator->genPatternStart(event);
}

void EventHandler::handlePatternEnd(const Event &event) {
  codeGenerator->genPatternEnd(event);
}

void EventHandler::handlePatternItemStart(const Event &event) {
  checkAttributeExists(event, L"n");
  wstring catName = event.getAttribute(L"n");

  if (defCats.find(catName) == defCats.end()) {
    wstringstream msg;
    msg << L"cat '" << catName << L"' doesn't exist.";
    throwError(event, msg.str());
  } else {
    vector<wstring> cats = defCats[catName];
    codeGenerator->genPatternItemStart(event, cats);
  }
}

void EventHandler::handleActionStart(const Event &event) {
  codeGenerator->genActionStart(event);
}

void EventHandler::handleActionEnd(const Event &event) {
  codeGenerator->genActionEnd(event);
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

void EventHandler::handleWithParamStart(const Event &event) {
  codeGenerator->genWithParamStart(event);
}

void EventHandler::handleChooseStart(const Event &event) {
  codeGenerator->genChooseStart(event);
}

void EventHandler::handleChooseEnd(const Event &event) {
  codeGenerator->genChooseEnd(event);
}

void EventHandler::handleWhenStart(const Event &event) {
  codeGenerator->genWhenStart(event);
}

void EventHandler::handleWhenEnd(const Event &event) {
  codeGenerator->genWhenEnd(event);
}

void EventHandler::handleOtherwiseStart(const Event &event) {
  codeGenerator->genOtherwiseStart(event);
}

void EventHandler::handleTestEnd(const Event &event) {
  codeGenerator->genTestEnd(event);
}

void EventHandler::handleBStart(const Event &event) {
  codeGenerator->genBStart(event);
}

void EventHandler::handleLitStart(const Event &event) {
  codeGenerator->genLitStart(event);
}

void EventHandler::handleLitTagStart(const Event &event) {
  codeGenerator->genLitTagStart(event);
}

void EventHandler::handleTagsEnd(const Event &event) {
  codeGenerator->genTagsEnd(event);
}

void EventHandler::handleLuEnd(const Event &event) {
  codeGenerator->genLuEnd(event);
}

void EventHandler::handleMluEnd(const Event &event) {
  codeGenerator->genMluEnd(event);
}

void EventHandler::handleLuCountStart(const Event &event) {
  codeGenerator->genLuCountStart(event);
}

void EventHandler::handleChunkStart(const Event &event) {
  if (transferStage == TRANSFER && transferDefault != CHUNK) {
    throwError(event,
        L"Unexpected '<chunk>' element in a non '<transfer default=chunk>'");
  }
  codeGenerator->genChunkStart(event);
}

void EventHandler::handleChunkEnd(const Event &event) {
  codeGenerator->genChunkEnd(event);
}

void EventHandler::handleEqualEnd(const Event &event) {
  codeGenerator->genEqualEnd(event);
}

void EventHandler::handleAndEnd(const Event &event) {
  codeGenerator->genAndEnd(event);
}

void EventHandler::handleOrEnd(const Event &event) {
  codeGenerator->genOrEnd(event);
}

void EventHandler::handleNotEnd(const Event &event) {
  codeGenerator->genNotEnd(event);
}

void EventHandler::handleOutEnd(const Event &event) {
  codeGenerator->genOutEnd(event);
}

void EventHandler::handleVarStart(const Event &event) {
  checkAttributeExists(event, L"n");
  wstring varName = event.getAttribute(L"n");

  if (defVars.find(varName) != defVars.end()) {
    // Check if this var acts as a container.
    bool isContainer = this->isContainer(event);
    codeGenerator->genVarStart(event, isContainer);
  } else {
    wstringstream msg;
    msg << L"var '" << varName << L"' doesn't exist.";
    throwError(event, msg.str());
  }
}

void EventHandler::handleInEnd(const Event &event) {
  codeGenerator->genInEnd(event);
}

void EventHandler::handleClipStart(const Event &event) {
  bool linkTo = event.hasAttribute(L"link-to");

  checkAttributeExists(event, L"part");
  wstring part = event.getAttribute(L"part");
  vector<wstring> partAttrs = getPartAttribute(event);

  // Check if this clip acts as a container.
  bool isContainer = this->isContainer(event);

  codeGenerator->genClipStart(event, partAttrs, isContainer, linkTo);
}

void EventHandler::handleListStart(const Event &event) {
  checkAttributeExists(event, L"n");
  wstring listName = event.getAttribute(L"n");

  if (defLists.find(listName) != defAttrs.end()) {
    vector<wstring> list = defLists[listName];
    codeGenerator->genListStart(event, list);
  } else {
    wstringstream msg;
    msg << L"list '" << listName << L"' doesn't exist.";
    throwError(event, msg.str());
  }
}

void EventHandler::handleLetEnd(const Event &event) {
  const Event *container = event.getChild(0);
  codeGenerator->genLetEnd(event, container);
}

void EventHandler::handleConcatEnd(const Event &event) {
  codeGenerator->genConcatEnd(event);
}

void EventHandler::handleAppendStart(const Event &event) {
  codeGenerator->genAppendStart(event);
}

void EventHandler::handleAppendEnd(const Event &event) {
  codeGenerator->genAppendEnd(event);
}

void EventHandler::handleGetCaseFromStart(const Event &event) {
  codeGenerator->genGetCaseFromStart(event);
}

void EventHandler::handleGetCaseFromEnd(const Event &event) {
  codeGenerator->genGetCaseFromEnd(event);
}

void EventHandler::handleCaseOfStart(const Event &event) {
  checkAttributeExists(event, L"part");
  wstring part = event.getAttribute(L"part");
  vector<wstring> partAttrs = getPartAttribute(event);

  codeGenerator->genCaseOfStart(event, partAttrs);
}

void EventHandler::handleModifyCaseEnd(const Event &event) {
  const Event *container = event.getChild(0);
  codeGenerator->genModifyCaseEnd(event, container);
}

void EventHandler::handleBeginsWithEnd(const Event &event) {
  codeGenerator->genBeginsWithEnd(event);
}

void EventHandler::handleBeginsWithListEnd(const Event &event) {
  codeGenerator->genBeginsWithListEnd(event);
}

void EventHandler::handleEndsWithEnd(const Event &event) {
  codeGenerator->genEndsWithEnd(event);
}

void EventHandler::handleEndsWithListEnd(const Event &event) {
  codeGenerator->genEndsWithListEnd(event);
}

void EventHandler::handleContainsSubstringEnd(const Event &event) {
  codeGenerator->genContainsSubstringEnd(event);
}
