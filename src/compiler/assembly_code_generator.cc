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
  jumpToRulesSection = false;
  listPoolNextIndex = 0;
}

/**
 * Set the output of debug messages on or off.
 *
 * @param mode true if debug is activated, false in other case.
 */
void AssemblyCodeGenerator::setDebug(bool mode) {
  debug = mode;
}

/**
 * Add the generated assembly code, modifying the next available address.
 *
 * @param code the assembly code to add
 */
void AssemblyCodeGenerator::addCode(const wstring &code) {
  this->code.push_back(code);
  nextAddress++;
}

/**
 * Add the generated assembly code for the patterns section. As we can't know
 * which is the last rule, we need to pop the footer and  append it at the end
 * for every new pattern instruction.
 *
 * @param code the assembly code to add
 */
void AssemblyCodeGenerator::addPatternsCode(const wstring &code) {
  // TODO duplicate code below.
  if (patternsCode.size() == 0) {
    patternsCode.push_back(L"patterns_start:");
    patternsCode.push_back(L"patterns_end:");
  }

  wstring footer = patternsCode.back();
  patternsCode.pop_back();
  patternsCode.push_back(code);
  patternsCode.push_back(footer);
}

void AssemblyCodeGenerator::genCodePushListOnStack(const vector<wstring>& list) {
  // Push list on stack in the preprocessing section.
  genPatternsCodePushListOnStack(list);

  // Generate instruction so the list gets stored in the list pool.
  wstringstream ws;
  ws << listPoolNextIndex;
  addPatternsCode(STORE_LIST_POOL_OP + INSTR_SEP + ws.str());

  // Instead of the list, push the index in the list pool.
  addCode(PUSH_INT_OP + INSTR_SEP + ws.str());
//
//  wstring strList(L"");
//  if(list.size() > 0) {
//    strList = list[0];
//    for(size_t i = 1; i < list.size(); ++i) {
//      strList += L"|";
//      strList += list[i];
//    }
//  }
//
//  addCode(PUSH_STR_OP + INSTR_SEP + strList);

  ++listPoolNextIndex;
}

void AssemblyCodeGenerator::genPatternsCodePushListOnStack(const vector<wstring>& list) {
  wstring strList(L"");
  if(list.size() > 0) {
    strList = list[0];
    for(size_t i = 1; i < list.size(); ++i) {
      strList += L"|";
      strList += list[i];
    }
  }
  addPatternsCode(PUSH_STR_OP + INSTR_SEP + strList);
}

/**
 * Returns a writable representation of the assembly code.
 *
 * @return the writable representation as a wide string.
 */
wstring AssemblyCodeGenerator::getWritableCode() const {
  wstring writableCode = L"";

  for (unsigned int i = 0; i < patternSection; i++) {
    writableCode += code[i] + L"\n";
  }

  for (unsigned int i = 0; i < patternsCode.size(); i++) {
    writableCode += patternsCode[i] + L"\n";
  }

  for (unsigned int i = patternSection; i < code.size(); i++) {
    writableCode += code[i] + L"\n";
  }

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

/**
 * Add, if it wasn't already added, the jump to the start of the rules section
 * to ignore macros until they are called.
 */
void AssemblyCodeGenerator::addJumpToRulesSection() {
  if (!jumpToRulesSection) {
    addCode(JMP_OP + INSTR_SEP + L"section_rules_start");
  }

  jumpToRulesSection = true;
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
  genDebugCode(event);

  // Push the default value and store it in var.
  wstring varName = event.getAttribute(L"n");
  addCode(PUSH_STR_OP + INSTR_SEP + varName);
  addCode(PUSH_STR_OP + INSTR_SEP + defaultValue);
  addCode(STOREV_OP);
}

void AssemblyCodeGenerator::genSectionDefMacrosStart(const Event &event) {
  addJumpToRulesSection();
}

void AssemblyCodeGenerator::genDefMacroStart(const Event &event) {
  genDebugCode(event);
  addCode(L"macro_" + event.getAttribute(L"n") + L"_start:");
}

void AssemblyCodeGenerator::genDefMacroEnd(const Event &event) {
  wstring macroEndLabel = L"macro_" + event.getAttribute(L"n") + L"_end:";
  addCode(macroEndLabel + INSTR_SEP + RET_OP);
}

void AssemblyCodeGenerator::genSectionRulesStart(const Event & event) {
  genDebugCode(event);
  addJumpToRulesSection();
  addCode(L"section_rules_start:");
  patternSection = nextAddress;
}

void AssemblyCodeGenerator::genSectionRulesEnd(const Event & event) {
  addCode(L"section_rules_end:");
}

void AssemblyCodeGenerator::genRuleStart(Event & event) {
  event.setVariable(L"label", getNextLabel(RULE));
}

void AssemblyCodeGenerator::genPatternStart(const Event & event) {
  // First time, add the header and footer of the patterns section.
  if (patternsCode.size() == 0) {
    patternsCode.push_back(L"patterns_start:");
    patternsCode.push_back(L"patterns_end:");
  }
}

void AssemblyCodeGenerator::genPatternEnd(const Event & event) {
  // Push the number of patterns to add to the trie.
  wstringstream ws;
  ws << event.getNumChildren();
  addPatternsCode(PUSH_INT_OP + INSTR_SEP + ws.str());

  // Push the trie instruction with destination address as operand.
  wstring numLabel = event.getParent()->getVariable(L"label");
  addPatternsCode(ADDTRIE_OP + INSTR_SEP + L"action_" + numLabel + L"_start");
}

void AssemblyCodeGenerator::genPatternItemStart(const Event & event,
    const vector<wstring> &cats) {
  // Push the contents of the category.
  genPatternsCodePushListOnStack(cats);
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
  genDebugCode(event);
}

void AssemblyCodeGenerator::genCallMacroEnd(const Event & event) {
  wstringstream ws;
  ws << event.getNumChildren();
  addCode(PUSH_INT_OP + INSTR_SEP + ws.str());
  addCode(CALL_OP + INSTR_SEP + event.getAttribute(L"n"));
}

void AssemblyCodeGenerator::genWithParamStart(const Event & event) {
  addCode(PUSH_INT_OP + INSTR_SEP + event.getAttribute(L"pos"));
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
  genDebugCode(event);
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
  genDebugCode(event);

  wstring value = event.getAttribute(L"v");
  addCode(PUSH_STR_OP + INSTR_SEP + value);
}

void AssemblyCodeGenerator::genLitTagStart(const Event & event) {
  genDebugCode(event);

  // Convert <det.ind> to <det><ind> format.
  wstring litTag = L"<" + event.getAttribute(L"v") + L">";
  litTag = WstringUtils::replace(litTag, L".", L"><");
  addCode(PUSH_STR_OP + INSTR_SEP + litTag);
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
  genDebugCode(event);

  wstring chunkName = L""; // Name is optional.

  bool isVariableReference;
  // If there is a fromname, we push the var name.
  if (event.hasAttribute(L"namefrom")) {
    chunkName = event.getAttribute(L"namefrom");
    isVariableReference = true;
  } else if (event.hasAttribute(L"name")) {
    chunkName = event.getAttribute(L"name");
    isVariableReference = false;
  }

  if (chunkName != L"") {
    if(isVariableReference) {
      addCode(PUSH_VAR_OP + INSTR_SEP + chunkName);
    } else {
      addCode(PUSH_STR_OP + INSTR_SEP + chunkName);
    }
    event.setVariable(L"name", L"true");
  }

  if (event.hasAttribute(L"case")) {
    // Push the var name, get its case and modify the case of the name.
    addCode(PUSH_VAR_OP + INSTR_SEP + event.getAttribute(L"case"));
    addCode(CASE_OF_OP);
    addCode(MODIFY_CASE_OP);
  }
}

void AssemblyCodeGenerator::genChunkEnd(const Event & event) {
  int numOps = event.getNumChildren();

  if (event.getVariable(L"name") != L"") {
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
  genDebugCode(event);

  wstring varName = event.getAttribute(L"n");

  // If it's a container push its name as a quoted string.
  if (isContainer) {
    addCode(PUSH_STR_OP + INSTR_SEP + event.getAttribute(L"n"));

    // If it's a container of a modify-case, we also need its content.
    if (event.getParent()->getName() == L"modify-case") {
      addCode(PUSH_VAR_OP + INSTR_SEP + varName);
    }
  } else {
    // Otherwise, push it as a symbol (without quotes).
    addCode(PUSH_VAR_OP + INSTR_SEP + varName);
  }

}

void AssemblyCodeGenerator::genInEnd(const Event & event) {
  addCode(getIgnoreCaseInstr(event, IN_OP, INIG_OP));
}

void AssemblyCodeGenerator::genClipCode(const Event &event,
    const vector<wstring> &partAttrs) {

  // Push the position to the stack.
  wstring pos = event.getAttribute(L"pos");
  addCode(PUSH_INT_OP + INSTR_SEP + pos);

  // Push the contents of the part attribute.
  genCodePushListOnStack(partAttrs);
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
  genDebugCode(event);

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
  genDebugCode(event);

  // Push the contents of the list to the stack.
  genCodePushListOnStack(list);
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
  genDebugCode(event);

  wstring varName = event.getAttribute(L"n");
  addCode(PUSH_STR_OP + INSTR_SEP + varName);
}

void AssemblyCodeGenerator::genAppendEnd(const Event & event) {
  wstringstream ws;
  ws << event.getNumChildren();
  addCode(APPEND_OP + INSTR_SEP + ws.str());
}

void AssemblyCodeGenerator::genGetCaseFromStart(const Event & event) {
  genDebugCode(event);
}

void AssemblyCodeGenerator::genGetCaseFromEnd(const Event & event) {
  wstring pos = event.getAttribute(L"pos");

  addCode(PUSH_INT_OP + INSTR_SEP + pos);
  addCode(GET_CASE_FROM_OP);

  // After getting the case we call modify-case to modify the container.
  addCode(MODIFY_CASE_OP);
}

void AssemblyCodeGenerator::genCaseOfStart(const Event & event,
    const vector<wstring> &partAttrs) {
  genDebugCode(event);

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

/**
 * Generate a debug message for a given event.
 *
 * @param event the event to generated the debug message from
 */
void AssemblyCodeGenerator::genDebugCode(const Event &event) {
  if (!debug)
    return;

  wstring debugMsg = L"#<" + event.getName();
  wstringstream ws;

  map<wstring, wstring>::iterator it;
  map<wstring, wstring> attributes = event.getAttributes();

  for ( it=attributes.begin() ; it != attributes.end(); it++ ) {
    ws <<  L" " << it->first << L"=\"" << it->second << L"\"";
  }
  debugMsg += ws.str();

  debugMsg += L">";
  addCode(debugMsg);
}
