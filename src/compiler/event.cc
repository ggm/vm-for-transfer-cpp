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

Event::Event() {
	this->lineNumber = 0;
	name = L"";
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

}

Event& Event::operator=(const Event &e) {
  if(this != &e)
  {
    this->~Event();
    this->copy(e);
  }
  return *this;
}

void Event::copy(const Event &e) {
	lineNumber = e.lineNumber;
	name = e.name;
	attributes = e.attributes;
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
