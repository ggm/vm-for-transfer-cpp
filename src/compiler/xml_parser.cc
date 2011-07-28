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
}

XmlParser::XmlParser(int fd) {
	reader = xmlReaderForFd(fd, NULL, NULL, 0);
}

XmlParser::XmlParser(char *fileName) {
	reader = xmlReaderForFile("test_macro_1.t1x", NULL, 0);
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
}

/**
 * Parse the contents of the xml file set in the constructor.
 */
void XmlParser::parse() {
	int ret;

	if (reader != NULL) {
		ret = xmlTextReaderRead(reader);
		while (ret == 1) {
			processNode();
			ret = xmlTextReaderRead(reader);
		}
		xmlFreeTextReader(reader);
		if (ret != 0) {
			throw CompilerException("An error occurred while parsing rules the file");
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
 * Create an event object, push it to the stack and call the appropriate
 * eventhandler's method.
 *
 * @param name the name of the element
 * @param lineNumber the line number of the event
 * @param attributes the parsed attributes of the event
 */
void XmlParser::handleStartElement(const wstring &name, int lineNumber,
		map<wstring, wstring> attributes) {
	Event event(lineNumber, name, attributes);
	callStack.push_back(event);

	if (name == L"transfer")
		eventHandler.handleTransferStart(event);
	else if (name == L"interchunk")
		eventHandler.handleInterchunkStart(event);
	else if (name == L"postchunk")
		eventHandler.handlePostchunkStart(event);
}

/**
 * Pop the last event from the stack and call the appropriate eventhandler's
 * method.
 *
 * @param the name of the event
 */
void XmlParser::handleEndElement(const wstring &name) {
	Event event;
	if (!callStack.empty()) {
		event = callStack.back();
		callStack.pop_back();
	}

	if (name == L"transfer")
		eventHandler.handleTransferEnd(event);
	else if (name == L"interchunk")
		eventHandler.handleInterchunkEnd(event);
	else if (name == L"postchunk")
		eventHandler.handlePostchunkEnd(event);
}
