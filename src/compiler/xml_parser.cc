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

#include <xml_parser.h>

#include <wstring_utils.h>

XmlParser::XmlParser() {
  reader = NULL;
  lastElementWasEmpty = false;
}

XmlParser::XmlParser(int fd) {
  reader = xmlReaderForFd(fd, NULL, NULL, 0);
  lastElementWasEmpty = false;
}

XmlParser::XmlParser(char *fileName) {
  reader = xmlReaderForFile(fileName, NULL, 0);
  lastElementWasEmpty = false;
}

XmlParser::XmlParser(const XmlParser &p) {
  copy(p);
}

XmlParser::~XmlParser() {
  xmlCleanupCharEncodingHandlers();
  xmlCleanupParser();
  reader = NULL;
}

XmlParser& XmlParser::operator=(const XmlParser &c) {
  if (this != &c) {
    this->~XmlParser();
    this->copy(c);
  }
  return *this;
}

void XmlParser::copy(const XmlParser &c) {
  reader = c.reader;
  eventHandler = c.eventHandler;
  callStack = c.callStack;
  lastElementWasEmpty = c.lastElementWasEmpty;
}

/**
 * Parse the contents of the xml file set in the constructor.
 */
void XmlParser::parse() {
  int ret;

  if (reader != NULL) {
    ret = xmlTextReaderRead(reader);
    while (ret == 1) {
      int t = xmlTextReaderNodeType(reader);

      // Only need to process node of type element (start and end).
      if (t == XML_READER_TYPE_ELEMENT || t == XML_READER_TYPE_END_ELEMENT) {
        processNode();
      }
      ret = xmlTextReaderRead(reader);
    }
    xmlFreeTextReader(reader);
    if (ret != 0) {
      throw CompilerException(
          L"An error occurred while parsing rules the file");
    }
  }
}

/**
 * Set the code generator to use by the event handler.
 *
 * @param codeGenerator the code generator to use
 */
void XmlParser::setCodeGenerator(CodeGenerator *codeGenerator) {
  eventHandler.setCodeGenerator(codeGenerator);
}

/**
 * Process an xml node calling the appropriate method depending on the type.
 */
void XmlParser::processNode() {
  xmlChar * xName = xmlTextReaderName(reader);
  wstring name = WstringUtils::towstring(xName);
  free(xName);
  int lineNumber = xmlTextReaderGetParserLineNumber(reader);
  map<wstring, wstring> attributes = parseAttributes();

  switch (xmlTextReaderNodeType(reader)) {
  case XML_READER_TYPE_ELEMENT:
    handleStartElement(name, lineNumber, attributes);
    break;
  case XML_READER_TYPE_END_ELEMENT:
    handleEndElement(name);
    break;
  default:
    break;
  }
}

/**
 * Parse the atributes of an xml node and convert them to a map of wide strings.
 *
 * @return a map containing attributes pairs of key, value
 */
map<wstring, wstring> XmlParser::parseAttributes() const {
  map<wstring, wstring> attributes;

  if (xmlTextReaderHasAttributes(reader) == 1) {
    xmlNodePtr node = xmlTextReaderCurrentNode(reader);

    for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
      // Get the xml attribute name and value.
      const xmlChar *xName = attr->name;
      xmlChar *xValue = xmlTextReaderGetAttribute(reader, xName);

      // Convert them to wstring and store them in the map.
      wstring name = WstringUtils::towstring(xName);
      wstring value = WstringUtils::towstring(xValue);
      attributes[name] = value;

      free(xValue);
    }
  }
  return attributes;
}

/**
 * Detect a self-closing element and act accordingly. libxml2 doesn't create an
 * end event for a self-closing element, so we detect them using the isEmpty
 * function. Then we need to pop its event from the callstack as if it was the
 * handleEndElement method.
 */
void XmlParser::detectSelfClosingElements() {
  if (lastElementWasEmpty) {
    Event * event = callStack.back();
    callStack.pop_back();
    delete event;
  }
  lastElementWasEmpty =
      (xmlTextReaderIsEmptyElement(reader) == 1) ? true : false;
}

/**
 * Create an event object, push it to the stack and call the appropriate
 * eventhandler's method.
 *
 * @param name the name of the element
 * @param lineNumber the line number of the event
 * @param attributes the parsed attributes of the event
 */
void XmlParser::handleStartElement(const wstring &name, int lineNumber,
    map<wstring, wstring> attributes) {
  Event *event = new Event(lineNumber, name, attributes);

  // Add the event as a child of the current top.
  if (!callStack.empty()) {
    detectSelfClosingElements();
    const Event *top = callStack.back();
    callStack.back()->addChild(*event);
    event->setParent(top);
  }

  if (name == L"transfer") eventHandler.handleTransferStart(*event);
  else if (name == L"interchunk") eventHandler.handleInterchunkStart(*event);
  else if (name == L"postchunk") eventHandler.handlePostchunkStart(*event);
  else if (name == L"def-cat") eventHandler.handleDefCatStart(*event);
  else if (name == L"cat-item") eventHandler.handleCatItemStart(*event);
  else if (name == L"def-attr") eventHandler.handleDefAttrStart(*event);
  else if (name == L"attr-item") eventHandler.handleAttrItemStart(*event);
  else if (name == L"def-var") eventHandler.handleDefVarStart(*event);
  else if (name == L"def-list") eventHandler.handleDefListStart(*event);
  else if (name == L"list-item") eventHandler.handleDefListItemStart(*event);
  else if (name == L"section-def-macros") eventHandler.handleSectionDefMacrosStart(*event);
  else if (name == L"def-macro") eventHandler.handleDefMacroStart(*event);
  else if (name == L"section-rules") eventHandler.handleSectionRulesStart(*event);
  else if (name == L"rule") eventHandler.handleRuleStart(*event);
  else if (name == L"pattern") eventHandler.handlePatternStart(*event);
  else if (name == L"pattern-item") eventHandler.handlePatternItemStart(*event);
  else if (name == L"action") eventHandler.handleActionStart(*event);
  else if (name == L"call-macro") eventHandler.handleCallMacroStart(*event);
  else if (name == L"with-param") eventHandler.handleWithParamStart(*event);
  else if (name == L"choose") eventHandler.handleChooseStart(*event);
  else if (name == L"when") eventHandler.handleWhenStart(*event);
  else if (name == L"otherwise") eventHandler.handleOtherwiseStart(*event);
  else if (name == L"b") eventHandler.handleBStart(*event);
  else if (name == L"lit") eventHandler.handleLitStart(*event);
  else if (name == L"lit-tag") eventHandler.handleLitTagStart(*event);
  else if (name == L"lu-count") eventHandler.handleLuCountStart(*event);
  else if (name == L"chunk") eventHandler.handleChunkStart(*event);
  else if (name == L"var") eventHandler.handleVarStart(*event);
  else if (name == L"clip") eventHandler.handleClipStart(*event);
  else if (name == L"list") eventHandler.handleListStart(*event);
  else if (name == L"append") eventHandler.handleAppendStart(*event);
  else if (name == L"get-case-from") eventHandler.handleGetCaseFromStart(*event);
  else if (name == L"case-of") eventHandler.handleCaseOfStart(*event);

  callStack.push_back(event);
}

/**
 * Pop the last event from the stack and call the appropriate eventhandler's
 * method.
 *
 * @param name the name of the event
 */
void XmlParser::handleEndElement(const wstring &name) {
  Event *event;
  if (!callStack.empty()) {
    detectSelfClosingElements();
    event = callStack.back();
    callStack.pop_back();
  }

  if (name == L"transfer") eventHandler.handleTransferEnd(*event);
  else if (name == L"interchunk") eventHandler.handleInterchunkEnd(*event);
  else if (name == L"postchunk") eventHandler.handlePostchunkEnd(*event);
  else if (name == L"def-cat") eventHandler.handleDefCatEnd(*event);
  else if (name == L"def-attr") eventHandler.handleDefAttrEnd(*event);
  else if (name == L"def-list") eventHandler.handleDefListEnd(*event);
  else if (name == L"def-macro") eventHandler.handleDefMacroEnd(*event);
  else if (name == L"section-rules") eventHandler.handleSectionRulesEnd(*event);
  else if (name == L"pattern") eventHandler.handlePatternEnd(*event);
  else if (name == L"action") eventHandler.handleActionEnd(*event);
  else if (name == L"call-macro") eventHandler.handleCallMacroEnd(*event);
  else if (name == L"choose") eventHandler.handleChooseEnd(*event);
  else if (name == L"when") eventHandler.handleWhenEnd(*event);
  else if (name == L"test") eventHandler.handleTestEnd(*event);
  else if (name == L"tags") eventHandler.handleTagsEnd(*event);
  else if (name == L"lu") eventHandler.handleLuEnd(*event);
  else if (name == L"mlu") eventHandler.handleMluEnd(*event);
  else if (name == L"chunk") eventHandler.handleChunkEnd(*event);
  else if (name == L"equal") eventHandler.handleEqualEnd(*event);
  else if (name == L"and") eventHandler.handleAndEnd(*event);
  else if (name == L"or") eventHandler.handleOrEnd(*event);
  else if (name == L"not") eventHandler.handleNotEnd(*event);
  else if (name == L"out") eventHandler.handleOutEnd(*event);
  else if (name == L"in") eventHandler.handleInEnd(*event);
  else if (name == L"let") eventHandler.handleLetEnd(*event);
  else if (name == L"concat") eventHandler.handleConcatEnd(*event);
  else if (name == L"append") eventHandler.handleAppendEnd(*event);
  else if (name == L"get-case-from") eventHandler.handleGetCaseFromEnd(*event);
  else if (name == L"modify-case") eventHandler.handleModifyCaseEnd(*event);
  else if (name == L"begins-with") eventHandler.handleBeginsWithEnd(*event);
  else if (name == L"begins-with-list") eventHandler.handleBeginsWithListEnd(*event);
  else if (name == L"ends-with") eventHandler.handleEndsWithEnd(*event);
  else if (name == L"ends-with-list") eventHandler.handleEndsWithListEnd(*event);
  else if (name == L"contains-substring") eventHandler.handleContainsSubstringEnd(*event);

  delete event;
}
