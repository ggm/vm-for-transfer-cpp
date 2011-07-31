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

#ifndef EVENT_H_
#define EVENT_H_

#include <string>
#include <map>
#include <vector>

using namespace std;

/// A class which encapsulates an event generated by the XML parser.
class Event {

public:

  Event();
  Event(int, const wstring &, map<wstring, wstring>);
  Event(const Event&);
  ~Event();
  Event& operator=(const Event&);
  void copy(const Event&);

  int getLineNumber() const;
  wstring getName() const;
  map<wstring, wstring> getAttributes() const;
  wstring getAttribute(const wstring&) const;
  bool hasAttribute(const wstring &) const;
  int getNumChildren() const;

  const Event *getParent() const;
  void setParent(const Event *);
  void addChild(const Event *);
  const Event* getChild(unsigned int) const;

private:
  /// The line number of the event in the XML original file.
  int lineNumber;

  /// The name of the element originator of the event.
  wstring name;

  /// The attributes of the element.
  map<wstring, wstring> attributes;

  /// Store a reference to the parent of the event.
  const Event *parent;

  /// Storing references to the event's children gives us a lot of flexibility.
  vector<const Event*> children;

};

#endif /* EVENT_H_ */
