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

#include <sstream>

#include <compiler_exception.h>
#include <wstring_utils.h>

AssemblyCodeGenerator::AssemblyCodeGenerator() {
  nextAddress = 0;
  nextLabel[RULE] = 0;
  nextLabel[WHEN] = 0;
  nextLabel[CHOOSE] = 0;
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
 * Get the next label depending on the type element.
 *
 * @param element the constant number representing the element's type
 *
 * @return the next number of the label as a wide string
 */
wstring AssemblyCodeGenerator::getNextLabel(unsigned int element) {
  unsigned int label = nextLabel[element];
  nextLabel[element]++;

  wstringstream ws;
  ws << label;
  return ws.str();
}

/**
 * Choose the appropriate store instruction depending on the container.
 *
 * @param container the container of the store instruction
 *
 * @return the opcode of the store instruction to use
 */
wstring AssemblyCodeGenerator::genStoreInstr(const Event &container) const {
  wstring name = container.getName();

  if (name == L"var") {
    return STOREV_OP;
  } else if (name == L"clip") {
    if (!container.hasAttribute(L"side")) {
      return STORECL_OP;
    } else {
      wstring side = container.getAttribute(L"side");
      if (side == L"sl") {
        return STORESL_OP;
      } else if (side == L"tl") {
        return STORETL_OP;
      }
    }
  }

  wstringstream msg(L"Can't find an appropriate store instruction for event: ");
  msg << name;
  throw CompilerException(msg.str());
}

/**
 * Select the appropriate instruction depending on the case options.
 *
 * @param event the event to get the case options from
 * @param instrNotIgnoreCase the instruction opcode without the ignore case
 * @param instrIgnoreCase the instruction opcode with the ignore case
 *
 * @return the appropriate instruction opcode
 */
wstring AssemblyCodeGenerator::getIgnoreCaseInstr(const Event &event,
    const wstring &instrNotIgnoreCase, const wstring &instrIgnoreCase) {

  if (!event.hasAttribute(L"caseless")) {
    return instrNotIgnoreCase;
  } else {
    wstring caseless = event.getAttribute(L"caseless");
    if (caseless == L"no")
      return instrNotIgnoreCase;
    else if (caseless == L"yes")
      return instrIgnoreCase;
  }

  wstringstream msg(
      L"Can't select the appropriate case instruction for event: ");
  msg << event.getName();
  throw CompilerException(msg.str());
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
  // Push the default value and store it in var.
  wstring varName = event.getAttribute(L"n");
  addCode(PUSH_OP + INSTR_SEP + varName);
  addCode(PUSH_OP + INSTR_SEP + L"\"" + defaultValue + L"\"");
  addCode(STOREV_OP);
}

void AssemblyCodeGenerator::genSectionDefMacrosStart(const Event &event) {
  // Jump to the start of the rules, ignoring the macros until called.
  addCode(JMP_OP + INSTR_SEP + L"section_rules_start");
}

void AssemblyCodeGenerator::genDefMacroStart(const Event &event) {
  addCode(L"macro_" + event.getAttribute(L"n") + L"_start:");
}

void AssemblyCodeGenerator::genDefMacroEnd(const Event &event) {
  wstring macroEndLabel = L"macro_" + event.getAttribute(L"n") + L"_end:";
  addCode(macroEndLabel + INSTR_SEP + RET_OP);
}

void AssemblyCodeGenerator::genSectionRulesStart(const Event & event) {
  addCode(L"section_rules_start:");
}

void AssemblyCodeGenerator::genSectionRulesEnd(const Event & event) {
  addCode(L"section_rules_end:");
}

void AssemblyCodeGenerator::genRuleStart(Event & event) {
  event.setVariable(L"label", getNextLabel(RULE));
}

void AssemblyCodeGenerator::genPatternStart(const Event & event) {
}

void AssemblyCodeGenerator::genPatternEnd(const Event & event) {
}

void AssemblyCodeGenerator::genPatternItemStart(const Event & event,
    const vector<wstring> &cats) {
}

void AssemblyCodeGenerator::genActionStart(const Event & event) {
  wstring label = event.getVariable(L"label");
  addCode(L"action_" + label + L"_start:");
}

void AssemblyCodeGenerator::genActionEnd(const Event & event) {
  wstring label = event.getVariable(L"label");
  addCode(L"action_" + label + L"_end:");
}

void AssemblyCodeGenerator::genCallMacroStart(const Event & event) {
}

void AssemblyCodeGenerator::genCallMacroEnd(const Event & event) {
  wstringstream ws;
  ws << event.getNumChildren();
  addCode(PUSH_OP + INSTR_SEP + ws.str());
  addCode(CALL_OP + INSTR_SEP + event.getAttribute(L"n"));
}

void AssemblyCodeGenerator::genWithParamStart(const Event & event) {
  addCode(PUSH_OP + INSTR_SEP + event.getAttribute(L"pos"));
}

void AssemblyCodeGenerator::genChooseStart(Event & event) {
  event.setVariable(L"label", getNextLabel(CHOOSE));
}

void AssemblyCodeGenerator::genChooseEnd(const Event & event) {
  addCode(L"choose_" + event.getVariable(L"label") + L"_end:");
}

void AssemblyCodeGenerator::genWhenStart(Event & event) {
  event.setVariable(L"label", getNextLabel(WHEN));
}

void AssemblyCodeGenerator::genWhenEnd(const Event & event) {
  // Add a jump to the end of the choose element, if the when is successful.
  wstring chooseNumber = event.getParent()->getVariable(L"label");
  addCode(JMP_OP + INSTR_SEP + L"choose_" + chooseNumber + L"_end");

  // Add the when end's label.
  addCode(L"when_" + event.getVariable(L"label") + L"_end:");
}

void AssemblyCodeGenerator::genOtherwiseStart(const Event & event) {
}

void AssemblyCodeGenerator::genTestEnd(const Event & event) {
  wstring label = event.getParent()->getVariable(L"label");
  addCode(JZ_OP + INSTR_SEP + L"when_" + label + L"_end");
}

void AssemblyCodeGenerator::genBStart(const Event & event) {
  if (event.hasAttribute(L"pos")) {
    addCode(PUSHSB_OP + INSTR_SEP + event.getAttribute(L"pos"));
  } else {
    addCode(PUSHBL_OP);
  }
}

void AssemblyCodeGenerator::genLitStart(const Event & event) {
  wstring value = event.getAttribute(L"v");
  wstringstream ws(value);
  int numericValue;

  if (ws >> numericValue) {
    addCode(PUSH_OP + INSTR_SEP + value);
  } else {
    addCode(PUSH_OP + INSTR_SEP + L"\"" + value + L"\"");
  }
}

void AssemblyCodeGenerator::genLitTagStart(const Event & event) {
  // Convert <det.ind> to <det><ind> format.
  wstring litTag = L"<" + event.getAttribute(L"v") + L">";
  litTag = WstringUtils::replace(litTag, L".", L"><");
  addCode(PUSH_OP + INSTR_SEP + litTag);
}

void AssemblyCodeGenerator::genTagsEnd(const Event & event) {
  int numChildren = event.getNumChildren();
  if (numChildren > 1) {
    wstringstream ws;
    ws << numChildren;
    addCode(CONCAT_OP + INSTR_SEP + ws.str());
  }
}

void AssemblyCodeGenerator::genLuEnd(const Event & event) {
  wstringstream ws;
  ws << event.getNumChildren();
  addCode(LU_OP + INSTR_SEP + ws.str());
}

void AssemblyCodeGenerator::genMluEnd(const Event & event) {
  wstringstream ws;
  ws << event.getNumChildren();
  addCode(MLU_OP + INSTR_SEP + ws.str());
}

void AssemblyCodeGenerator::genLuCountStart(const Event & event) {
  addCode(LU_COUNT_OP);
}

void AssemblyCodeGenerator::genChunkStart(Event & event) {
  wstring chunkName = L""; // Name is optional.

  // If there is a fromname, we push the var name.
  if (event.hasAttribute(L"namefrom")) {
    chunkName = event.getAttribute(L"namefrom");
  } else if (event.hasAttribute(L"name")) {
    chunkName = L"\"" + event.getAttribute(L"name") + L"\"";
  }

  if (chunkName != L"") {
    addCode(PUSH_OP + INSTR_SEP + chunkName);
    event.setVariable(L"name", L"true");
  }

  if (event.hasAttribute(L"case")) {
    // Push the var name, get its case and modify the case of the name.
    addCode(PUSH_OP + INSTR_SEP + event.getAttribute(L"case"));
    addCode(CASE_OF_OP);
    addCode(MODIFY_CASE_OP);
  }
}

void AssemblyCodeGenerator::genChunkEnd(const Event & event) {
  int numOps = event.getNumChildren();

  if (event.hasAttribute(L"name")) {
    numOps++;
  }

  wstringstream ws;
  ws << numOps;
  addCode(CHUNK_OP + INSTR_SEP + ws.str());
}

void AssemblyCodeGenerator::genEqualEnd(const Event & event) {
  addCode(getIgnoreCaseInstr(event, CMP_OP, CMPI_OP));
}

void AssemblyCodeGenerator::genAndEnd(const Event & event) {
  wstringstream ws;
  ws << event.getNumChildren();
  addCode(AND_OP + INSTR_SEP + ws.str());
}

void AssemblyCodeGenerator::genOrEnd(const Event & event) {
  wstringstream ws;
  ws << event.getNumChildren();
  addCode(OR_OP + INSTR_SEP + ws.str());
}

void AssemblyCodeGenerator::genNotEnd(const Event & event) {
  addCode(NOT_OP);
}

void AssemblyCodeGenerator::genOutEnd(const Event & event) {
  wstringstream ws;
  ws << event.getNumChildren();
  addCode(OUT_OP + INSTR_SEP + ws.str());
}

void AssemblyCodeGenerator::genVarStart(const Event & event, bool isContainer) {
  wstring varName = event.getAttribute(L"n");

  // If it's a container push its name as a quoted string.
  if (isContainer) {
    addCode(PUSH_OP + INSTR_SEP + L"\"" + event.getAttribute(L"n") + L"\"");

    // If it's a container of a modify-case, we also need its content.
    if (event.getParent()->getName() == L"modify-case") {
      addCode(PUSH_OP + INSTR_SEP + varName);
    }
  } else {
    // Otherwise, push it as a symbol (without quotes).
    addCode(PUSH_OP + INSTR_SEP + varName);
  }

}

void AssemblyCodeGenerator::genInEnd(const Event & event) {
  addCode(getIgnoreCaseInstr(event, IN_OP, INIG_OP));
}

void AssemblyCodeGenerator::genClipCode(const Event &event,
    const vector<wstring> &partAttrs) {

  // Push the position to the stack.
  wstring pos = event.getAttribute(L"pos");
  addCode(PUSH_OP + INSTR_SEP + pos);

  // Push the contents of the part attribute.
  wstring partAttrStr = L"";
  if (partAttrs.size() > 0) {
    partAttrStr = L"\"" + partAttrs[0];
    for (unsigned int i = 1; i < partAttrs.size(); i++) {
      partAttrStr += L"|";
      partAttrStr += partAttrs[i];
    }
    partAttrStr += L"\"";
  }

  addCode(PUSH_OP + INSTR_SEP + partAttrStr);
}

void AssemblyCodeGenerator::genClipInstr(const Event &event, bool linkTo) {
  wstring link = L"";
  if (linkTo) {
    link = INSTR_SEP + L"\"<" + event.getAttribute(L"link-to") + L">\"";
  }

  // Choose the appropriate instr depending on the side of the clip element.
  if (!event.hasAttribute(L"side")) {
    addCode(CLIP_OP + link);
  } else {
    wstring side = event.getAttribute(L"side");
    if (side == L"sl") {
      addCode(CLIPSL_OP + link);
    } else if (side == L"tl") {
      addCode(CLIPTL_OP + link);
    }
  }
}

void AssemblyCodeGenerator::genClipStart(const Event & event,
    const vector<wstring> &partAttrs, bool isContainer, bool linkTo) {

  genClipCode(event, partAttrs);

  if (isContainer && event.getParent()->getName() == L"modify-case") {
    // If it's a container of a modify-case, we need to generate another
    //clip instruction to get the clip value needed by the modify-case.
    genClipCode(event, partAttrs);
    genClipInstr(event, false);
  } else if (!isContainer) {
    // If this clip doesn't work as a container (left-side) we need a CLIP(SL|TL).
    genClipInstr(event, linkTo);
  }

}

void AssemblyCodeGenerator::genListStart(const Event & event,
    const vector<wstring> &list) {
  wstring processedList = L"";

  // Push the contents of the list to the stack.
  if (list.size() > 0) {
    processedList = L"\"" + list[0];
    for (unsigned int i = 1; i < list.size(); i++) {
      processedList += L"|";
      processedList += list[i];
    }
    processedList += L"\"";
  }

  addCode(PUSH_OP + INSTR_SEP + processedList);
}

void AssemblyCodeGenerator::genLetEnd(const Event & event,
    const Event &container) {
  addCode(genStoreInstr(container));
}

void AssemblyCodeGenerator::genConcatEnd(const Event & event) {
  wstringstream ws;
  ws << event.getNumChildren();
  addCode(CONCAT_OP + INSTR_SEP + ws.str());
}

void AssemblyCodeGenerator::genAppendStart(const Event & event) {
  wstring varName = L"\"" + event.getAttribute(L"n") + L"\"";
  addCode(PUSH_OP + INSTR_SEP + varName);
}

void AssemblyCodeGenerator::genAppendEnd(const Event & event) {
  wstringstream ws;
  ws << event.getNumChildren();
  addCode(APPEND_OP + INSTR_SEP + ws.str());
}

void AssemblyCodeGenerator::genGetCaseFromStart(const Event & event) {
}

void AssemblyCodeGenerator::genGetCaseFromEnd(const Event & event) {
  wstring pos = event.getAttribute(L"pos");

  addCode(PUSH_OP + INSTR_SEP + pos);
  addCode(GET_CASE_FROM_OP);

  // After getting the case we call modify-case to modify the container.
  addCode(MODIFY_CASE_OP);
}

void AssemblyCodeGenerator::genCaseOfStart(const Event & event,
    const vector<wstring> &partAttrs) {

  // Generate the code of the clip we are going to get the case from.
  genClipCode(event, partAttrs);
  // Add the clip instruction depending on the side attribute.
  genClipInstr(event, false);

  // Finally, use case-of to get the case of the clip on the stack.
  addCode(CASE_OF_OP);
}

void AssemblyCodeGenerator::genModifyCaseEnd(const Event & event,
    const Event &container) {
  addCode(MODIFY_CASE_OP);
  addCode(genStoreInstr(container));
}

void AssemblyCodeGenerator::genBeginsWithEnd(const Event & event) {
  addCode(getIgnoreCaseInstr(event, BEGINS_WITH_OP, BEGINS_WITH_IG_OP));
}

void AssemblyCodeGenerator::genEndsWithEnd(const Event & event) {
  addCode(getIgnoreCaseInstr(event, ENDS_WITH_OP, ENDS_WITH_IG_OP));
}

void AssemblyCodeGenerator::genContainsSubstringEnd(const Event & event) {
  addCode(getIgnoreCaseInstr(event, CMP_SUBSTR_OP, CMPI_SUBSTR_OP));
}

