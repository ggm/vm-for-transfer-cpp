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

#include <assembly_code_generator.h>

AssemblyCodeGenerator::AssemblyCodeGenerator() {
  nextAddress = 0;
}

AssemblyCodeGenerator::AssemblyCodeGenerator(const AssemblyCodeGenerator &c) {
  copy(c);
}

AssemblyCodeGenerator::~AssemblyCodeGenerator() {

}

AssemblyCodeGenerator&
AssemblyCodeGenerator::operator=(const AssemblyCodeGenerator &c) {
  if (this != &c) {
    this->~AssemblyCodeGenerator();
    this->copy(c);
  }
  return *this;
}

void AssemblyCodeGenerator::copy(const AssemblyCodeGenerator &c) {
  this->nextAddress = c.nextAddress;
  this->code = c.code;
}

void AssemblyCodeGenerator::addCode(const wstring &code) {
  this->code.push_back(code);
  nextAddress++;
}

/**
 * Returns a writable representation of the assembly code.
 *
 * @return the writable representation as a wide string.
 */
wstring AssemblyCodeGenerator::getWritableCode() const {
  wstring writableCode = L"";

  for (unsigned int i = 0; i < code.size(); i++) {
    writableCode += code[i] + L"\n";
  }
  writableCode += L"\n";

  return writableCode;
}

/*
 * Generate the header of the assembly file.
 *
 * @param event the first event of the xml rules file.
 */
void AssemblyCodeGenerator::genHeader(const Event &event) {
  // Add the type of file so the vm can read it accordingly.
  addCode(L"#<assembly>");

  // Add the rest of the header using the first element of the xml.
  map<wstring, wstring> attributes = event.getAttributes();
  map<wstring, wstring>::iterator it;

  wstringstream header;
  header << L"#<" << event.getName();
  for (it = attributes.begin(); it != attributes.end(); ++it) {
    header << L" " << it->first << L"=\"" << it->second << L"\"";
  }
  header << L">";

  addCode(header.str());
}

void AssemblyCodeGenerator::genTransferStart(const Event &event) {
  genHeader(event);
}

void AssemblyCodeGenerator::genInterchunkStart(const Event &event) {
  genHeader(event);
}

void AssemblyCodeGenerator::genPostchunkStart(const Event &event) {
  genHeader(event);
}

void AssemblyCodeGenerator::genDefVarStart(const Event &event,
    const wstring &defaultValue) {

}

void AssemblyCodeGenerator::genSectionDefMacrosStart(const Event &event) {

}

void AssemblyCodeGenerator::genDefMacroStart(const Event &) {

}

void AssemblyCodeGenerator::genDefMacroEnd(const Event &) {

}

void AssemblyCodeGenerator::genSectionRulesStart(const Event & event) {
}

void AssemblyCodeGenerator::genSectionRulesEnd(const Event & event) {
}

void AssemblyCodeGenerator::genRuleStart(const Event & event) {
}

void AssemblyCodeGenerator::genPatternStart(const Event & event) {
}

void AssemblyCodeGenerator::genPatternEnd(const Event & event) {
}

void AssemblyCodeGenerator::genPatternItemStart(const Event & event,
    const vector<wstring> &cats) {
}

void AssemblyCodeGenerator::genActionStart(const Event & event) {
}

void AssemblyCodeGenerator::genActionEnd(const Event & event) {
}

void AssemblyCodeGenerator::genCallMacroStart(const Event & event) {
}

void AssemblyCodeGenerator::genCallMacroEnd(const Event & event) {
}

void AssemblyCodeGenerator::genWithParamStart(const Event & event) {
}

void AssemblyCodeGenerator::genChooseStart(const Event & event) {
}

void AssemblyCodeGenerator::genChooseEnd(const Event & event) {
}

void AssemblyCodeGenerator::genWhenStart(const Event & event) {
}

void AssemblyCodeGenerator::genWhenEnd(const Event & event) {
}

void AssemblyCodeGenerator::genOtherwiseStart(const Event & event) {
}

void AssemblyCodeGenerator::genTestEnd(const Event & event) {
}

void AssemblyCodeGenerator::genBStart(const Event & event) {
}

void AssemblyCodeGenerator::genLitStart(const Event & event) {
}

void AssemblyCodeGenerator::genLitTagStart(const Event & event) {
}

void AssemblyCodeGenerator::genTagsEnd(const Event & event) {
}

void AssemblyCodeGenerator::genLuEnd(const Event & event) {
}

void AssemblyCodeGenerator::genMluEnd(const Event & event) {
}

void AssemblyCodeGenerator::genLuCountStart(const Event & event) {
}

void AssemblyCodeGenerator::genChunkStart(const Event & event) {
}

void AssemblyCodeGenerator::genChunkEnd(const Event & event) {
}

void AssemblyCodeGenerator::genEqualEnd(const Event & event) {
}

void AssemblyCodeGenerator::genAndEnd(const Event & event) {
}

void AssemblyCodeGenerator::genOrEnd(const Event & event) {
}

void AssemblyCodeGenerator::genNotEnd(const Event & event) {
}

void AssemblyCodeGenerator::genOutEnd(const Event & event) {
}

void AssemblyCodeGenerator::genVarStart(const Event & event, bool) {
}

void AssemblyCodeGenerator::genInEnd(const Event & event) {
}

void AssemblyCodeGenerator::genClipStart(const Event & event,
    const vector<wstring> &, bool, bool) {
}

void AssemblyCodeGenerator::genListStart(const Event & event,
    const vector<wstring> &) {
}

void AssemblyCodeGenerator::genLetEnd(const Event & event,
    const Event *container) {
}

void AssemblyCodeGenerator::genConcatEnd(const Event & event) {
}

void AssemblyCodeGenerator::genAppendStart(const Event & event) {
}

void AssemblyCodeGenerator::genAppendEnd(const Event & event) {
}

void AssemblyCodeGenerator::genGetCaseFromStart(const Event & event) {
}

void AssemblyCodeGenerator::genGetCaseFromEnd(const Event & event) {
}

void AssemblyCodeGenerator::genCaseOfStart(const Event & event,
    const vector<wstring> &partAttrs) {
}

void AssemblyCodeGenerator::genModifyCaseEnd(const Event & event,
    const Event *container) {
}

void AssemblyCodeGenerator::genBeginsWithEnd(const Event & event) {
}

void AssemblyCodeGenerator::genBeginsWithListEnd(const Event & event) {
}

void AssemblyCodeGenerator::genEndsWithEnd(const Event & event) {
}

void AssemblyCodeGenerator::genEndsWithListEnd(const Event & event) {
}

void AssemblyCodeGenerator::genContainsSubstringEnd(const Event & event) {
}

