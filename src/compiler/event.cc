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

#include <event.h>

#include <sstream>

#include <compiler_exception.h>

Event::Event() {
  this->lineNumber = 0;
  name = L"";
  parent = NULL;
}

Event::Event(int lineNumber, const wstring &name,
    map<wstring, wstring> attributes) {
  this->lineNumber = lineNumber;
  this->name = name;
  this->attributes = attributes;
}

Event::Event(const Event &e) {
  copy(e);
}

Event::~Event() {
  parent = NULL;
}

Event& Event::operator=(const Event &e) {
  if (this != &e) {
    this->~Event();
    this->copy(e);
  }
  return *this;
}

void Event::copy(const Event &e) {
  lineNumber = e.lineNumber;
  name = e.name;
  attributes = e.attributes;
  parent = e.parent;
  children = e.children;
  variables = e.variables;
}

/**
 * Get the line number (original xml) of the element which generated the event.
 *
 * @return the line number
 */
int Event::getLineNumber() const {
  return lineNumber;
}

/**
 * Get the name of the element which generated the event.
 *
 * @return the name of the element
 */
wstring Event::getName() const {
  return name;
}

/**
 * Get the collection of attributes (key, value).
 *
 * @return the map of key, value pairs.
 */
map<wstring, wstring> Event::getAttributes() const {
  return attributes;
}

/**
 * Get the value of an attribute, given an attribute name.
 *
 * @param name the name of the attribute
 *
 * @return the value if the attribute is found, in other case, an empty string.
 */
wstring Event::getAttribute(const wstring &name) const {
  map<wstring, wstring>::const_iterator it;
  it = attributes.find(name);
  if (it != attributes.end()) {
    return it->second;
  } else {
    return L"";
  }
}

/**
 * Check if the event has an attribute with the name passed as parameter.
 *
 * @param name the name of the attribute
 *
 * @return true if the attribute is found, in other case, false.
 */
bool Event::hasAttribute(const wstring &attrName) const {
  map<wstring, wstring>::const_iterator it;
  it = attributes.find(attrName);
  if (it != attributes.end()) {
    return true;
  } else {
    return false;
  }
}

/**
 * Get the number of events which are children of this one.
 *
 * @return the number of children
 */
int Event::getNumChildren() const {
  return children.size();
}

/**
 * Return the parent of the node.
 *
 * @return the parent of the node
 */
const Event* Event::getParent() const {
  return parent;
}

/**
 * Set a reference to the parent of the event.
 *
 * @param event the parent of the event
 */
void Event::setParent(const Event *event) {
  parent = event;
}

/**
 * Add a reference to a child so we can inspect them in the future.
 *
 * @param event the child of the event
 */
void Event::addChild(Event event) {
  children.push_back(event);
}

/**
 * Get a child of the event.
 *
 * @param pos the index of the child to get
 *
 * @return the reference to the child Event
 */
Event Event::getChild(unsigned int pos) const {
  if (pos < children.size()) {
    return children[pos];
  } else {
    wstringstream msg;
    msg << L"Event '" << name << L"' doesn't have a child in pos " << pos;
    throw CompilerException(msg.str());
  }
}

/**
 * Get the value of an event's variable.
 *
 * @param varName the name of the variable
 *
 * @return the value of the variable with name varName, or an empty string
 */
wstring Event::getVariable(const wstring &varName) const {
  map<wstring, wstring>::const_iterator it;
    it = variables.find(varName);
    if (it != variables.end()) {
      return it->second;
    } else {
      return L"";
    }
}

/**
 * Set the value of an event's variable.
 *
 * @param varName the name of the variable
 * @param value the new value of the variable
 */
void Event::setVariable(const wstring &varName, const wstring &value) {
  variables[varName] = value;
}

