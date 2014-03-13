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

#ifndef SCOPE_H_
#define SCOPE_H_

#include <unordered_map>
#include <vector>
#include <string>

#include "instructions.h"

using namespace std;

/// This class represents a scope in the assembly and vm.
class Scope {

public:
  /// Next executable address, used to set the labels to an address.
  unsigned int nextAddress;

  Scope();
  Scope(const Scope&);
  ~Scope();
  Scope& operator=(const Scope&);
  void copy(const Scope&);

  void backPatchLabels(CodeUnit &) const;
  wstring createNewLabelAddress(const wstring &);
  wstring getReferenceToLabel(const wstring &, const CodeUnit&);

private:

  /// Each label is converted to an internal address of the vm.
  unordered_map<wstring, wstring> labelAddress;

  /// Store the labels in need of backpatching.
  unordered_map<wstring, vector<unsigned int> > patchNeeded;

  void addLabelToPatch(wstring, unsigned int);
};

#endif /* SCOPE_H_ */
