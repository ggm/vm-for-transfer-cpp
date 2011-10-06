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

#ifndef COMPILER_H_
#define COMPILER_H_

#include <iostream>
#include <fstream>
#include <string>

#include <xml_parser.h>
#include <code_generator.h>

using namespace std;

/**
 * This class encapsulates all the compiling process.
 */
class Compiler {

public:

  Compiler();
  Compiler(const Compiler&);
  ~Compiler();
  Compiler& operator=(const Compiler&);
  void copy(const Compiler&);

  void setDebug(char *);
  void setInputFile(char *);
  void setOutputFile(char *);

  bool compile();
  void debugMessage(const wstring &);
  void writeOutput(const wstring &) const;

private:
  /// Name of the input file to use.
  char *inputFileName;

  /// Name of the output file to use.
  char *outputFileName;

  /// The debug file if debug mode is activated.
  wofstream debugFile;

  // Compiler's components.
  XmlParser parser;
  CodeGenerator *codeGenerator;

  void createParser();
};

#endif /* COMPILER_H_ */
