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

#include "scope.h"

#include <sstream>

#include "vm_wstring_utils.h"

Scope::Scope() {
  nextAddress = 0;
}

Scope::Scope(const Scope &s) {
  copy(s);
}

Scope::~Scope() {

}

Scope& Scope::operator=(const Scope &s) {
  if (this != &s) {
    this->~Scope();
    this->copy(s);
  }
  return *this;
}

void Scope::copy(const Scope &s) {
  nextAddress = s.nextAddress;
  labelAddress = s.labelAddress;
  patchNeeded = s.patchNeeded;
}

/**
 * Backpatch all the labels which require it.
 *
 * @param codeUnit code unit which has the labels to patch
 */
void Scope::backPatchLabels(CodeUnit &codeUnit) const {
  map<wstring, vector<unsigned int> >::const_iterator itPatch;

  for (itPatch = patchNeeded.begin(); itPatch != patchNeeded.end(); ++itPatch) {
    wstring address = labelAddress.find(itPatch->first)->second;

    vector<unsigned int> positions = itPatch->second;
    for (unsigned int pos = 0; pos < positions.size(); ++pos) {
      Instruction& instr = codeUnit.code[positions[pos]];
      instr.op1 = address;
      instr.intOp1 = VMWstringUtils::stringTo<int>(address);
    }
  }
}

/**
 * Create a new unique internal address for a label.
 *
 * @param label the label which needs a new internal address
 *
 * @return the new internal address asociated to the label
 */
wstring Scope::createNewLabelAddress(const wstring &label) {
  unsigned int labelAddr = nextAddress;
  wstringstream ws;
  ws << labelAddr;

  labelAddress[label] = ws.str();
  return ws.str();
}

/**
 * Get the label address if it's already processed or mark it as patch
 * needed if the label address is unknown yet.
 *
 * @param label the label which we need the reference of
 * @param codeUnit the code unit where the label belongs
 *
 * @return the reference of the label if it's already processed, otherwise
 * return the mark ("#0#") representing it as in need of backpatching
 */
wstring Scope::getReferenceToLabel(const wstring &label,
    const CodeUnit &codeUnit) {
  map<wstring, wstring>::iterator it;

  it = labelAddress.find(label);
  if (it != labelAddress.end()) {
    return it->second;
  } else {
    addLabelToPatch(label, codeUnit.code.size());
    return L"#0#";
  }
}

/**
 * Add a reference to an unknown label, to later patch it.
 *
 * @param label the label to set for backpatching
 * @param pos the position in its code unit
 */
void Scope::addLabelToPatch(wstring label, unsigned int pos) {
  map<wstring, vector<unsigned int> >::iterator it;
  it = patchNeeded.find(label);

  if (it != patchNeeded.end()) {
    it->second.push_back(pos);
  } else {
    vector<unsigned int> positions;
    positions.push_back(pos);
    patchNeeded[label] = positions;
  }
}
