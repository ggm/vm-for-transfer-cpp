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

#ifndef CODE_GENERATOR_H_
#define CODE_GENERATOR_H_

#include <vector>
#include <string>

#include <event.h>

//! Interface for a code generator.
class CodeGenerator {

public:

  virtual ~CodeGenerator() {}
  virtual wstring getWritableCode() const = 0;

  virtual void genTransferStart(const Event &) = 0;
  virtual void genInterchunkStart(const Event &) = 0;
  virtual void genPostchunkStart(const Event &) = 0;
  virtual void genDefVarStart(const Event &, const wstring &) = 0;
  virtual void genSectionDefMacrosStart(const Event &) = 0;
  virtual void genDefMacroStart(const Event &) = 0;
  virtual void genDefMacroEnd(const Event &) = 0;
  virtual void genSectionRulesStart(const Event &) = 0;
  virtual void genSectionRulesEnd(const Event &) = 0;
  virtual void genRuleStart(Event &) = 0;
  virtual void genPatternStart(const Event &) = 0;
  virtual void genPatternEnd(const Event &) = 0;
  virtual void genPatternItemStart(const Event &, const vector<wstring>&) = 0;
  virtual void genActionStart(const Event &) = 0;
  virtual void genActionEnd(const Event &) = 0;
  virtual void genCallMacroStart(const Event &) = 0;
  virtual void genCallMacroEnd(const Event &) = 0;
  virtual void genWithParamStart(const Event &) = 0;
  virtual void genChooseStart(Event &) = 0;
  virtual void genChooseEnd(const Event &) = 0;
  virtual void genWhenStart(Event &) = 0;
  virtual void genWhenEnd(const Event &) = 0;
  virtual void genOtherwiseStart(const Event &) = 0;
  virtual void genTestEnd(const Event &) = 0;
  virtual void genBStart(const Event &) = 0;
  virtual void genLitStart(const Event &) = 0;
  virtual void genLitTagStart(const Event &) = 0;
  virtual void genTagsEnd(const Event &) = 0;
  virtual void genLuEnd(const Event &) = 0;
  virtual void genMluEnd(const Event &) = 0;
  virtual void genLuCountStart(const Event &) = 0;
  virtual void genChunkStart(Event &) = 0;
  virtual void genChunkEnd(const Event &) = 0;
  virtual void genEqualEnd(const Event &) = 0;
  virtual void genAndEnd(const Event &) = 0;
  virtual void genOrEnd(const Event &) = 0;
  virtual void genNotEnd(const Event &) = 0;
  virtual void genOutEnd(const Event &) = 0;
  virtual void genVarStart(const Event &, bool) = 0;
  virtual void genInEnd(const Event &) = 0;
  virtual
    void genClipStart(const Event &, const vector<wstring> &, bool, bool) = 0;
  virtual void genListStart(const Event &, const vector<wstring> &) = 0;
  virtual void genLetEnd(const Event &, const Event &) = 0;
  virtual void genConcatEnd(const Event &) = 0;
  virtual void genAppendStart(const Event &) = 0;
  virtual void genAppendEnd(const Event &) = 0;
  virtual void genGetCaseFromStart(const Event &) = 0;
  virtual void genGetCaseFromEnd(const Event &) = 0;
  virtual void genCaseOfStart(const Event &, const vector<wstring> &) = 0;
  virtual void genModifyCaseEnd(const Event &, const Event &) = 0;
  virtual void genBeginsWithEnd(const Event &) = 0;
  virtual void genEndsWithEnd(const Event &) = 0;
  virtual void genContainsSubstringEnd(const Event &) = 0;

private:

};

#endif /* CODE_GENERATOR_H_ */
