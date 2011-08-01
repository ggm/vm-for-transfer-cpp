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

#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include <iostream>
#include <list>
#include <libxml/xmlreader.h>

#include <compiler_exception.h>
#include <code_generator.h>
#include <event_handler.h>
#include <event.h>

using namespace std;

/// The XML parser which goes through the transfer files
class XmlParser {

public:

  XmlParser();
  XmlParser(int);
  XmlParser(char *);
  XmlParser(const XmlParser&);
  ~XmlParser();
  XmlParser& operator=(const XmlParser&);
  void copy(const XmlParser&);

  void parse();
  void setCodeGenerator(CodeGenerator*);

private:
  /// The libxml2's XML reader
  xmlTextReaderPtr reader;

  /// The event handler used to handle the xml elements.
  EventHandler eventHandler;

  /// The call stack with the parsed events.
  list<Event*> callStack;

  /// Store if the last element was empty (self-closing) to pop it from stack.
  bool lastElementWasEmpty;

  void detectSelfClosingElements();
  void processNode();
  map<wstring, wstring> parseAttributes() const;
  void handleStartElement(const wstring &, int, map<wstring, wstring>);
  void handleEndElement(const wstring &);
};

#endif /* XMLPARSER_H_ */
