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
