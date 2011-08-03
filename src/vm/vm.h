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

#ifndef VM_H_
#define VM_H_

#include <string>

using namespace std;

enum TRANSFER_STAGE {
  TRANSFER, INTERCHUNK, POSTCHUNK
};

enum TRANSFER_DEFAULT {
  CHUNK, LU
};

/// This class encapsulates all the VM processing.
class VM {

public:

  VM();
  VM(const VM&);
  ~VM();
  VM& operator=(const VM&);
  void copy(const VM&);

  void setCodeFile(char *);
  void setInputFile(char *);
  void setOutputFile(char *);
  void setDebugMode();

  void run();

private:
  /// Store the current transfer stage.
  TRANSFER_STAGE transferStage;

  /// Store the default unit in the transfer.
  TRANSFER_DEFAULT transferDefault;

  /// Name of the code file to use.
  char *codeFileName;

  /// Name of the input file to use.
  char *inputFileName;

  /// Name of the output file to use.
  char *outputFileName;

  /// Store if the debug mode is active or not.
  bool debugMode;

  void setLoader(wstring, char*);
  void setTransferStage(wstring);
};

#endif /* VM_H_ */
