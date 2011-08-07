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

#include "vm.h"

#include <fstream>
#include <sstream>

#include <vm_exceptions.h>

VM::VM() {
  debugMode = false;
}

VM::VM(const VM &vm) {
  copy(vm);
}

VM::~VM() {

}

VM& VM::operator=(const VM &vm) {
  if (this != &vm) {
    this->~VM();
    this->copy(vm);
  }
  return *this;
}

void VM::copy(const VM &vm) {
  transferStage = vm.transferStage;
  transferDefault = vm.transferDefault;
  codeFileName = vm.codeFileName;
  inputFileName = vm.inputFileName;
  outputFileName = vm.outputFileName;
  debugMode = vm.debugMode;
}

/**
 * Set the code file to read the instructions, initialize the appropriate
 * loader depending on the file's type and set the transfer stage.
 *
 * @param fileName code file's name
 */
void VM::setCodeFile(char *fileName) {
  codeFileName = fileName;

  wfstream file;
  file.open(fileName, ios::in);

  wstring header;
  getline(file, header, L'\n');
  setLoader(header, fileName);

  wstring transferHeader;
  getline(file, transferHeader, L'\n');
  setTransferStage(transferHeader);

  file.close();
}

/*
 * Set the loader to use depending on the header of the code file.
 *
 * @param header the first line of the code file indicates the type of file
 * @param fileName the file name of the code file
 */
void VM::setLoader(wstring header, char *fileName) {
  if (header == L"#<assembly>") {
    // TODO: Add loader instantiation here.
  } else {
    wstringstream msg;
    msg << L"The header of the file " << fileName << " is not recognized: "
        << endl;
    msg << header << endl;
    throw VmException(msg.str());
  }
}

/*
 * Set the transfer stage to process by the vm.
 *
 * @param transferHeader the part of the code file with the stage header
 */
void VM::setTransferStage(wstring transferHeader) {
  wstring transfer = L"transfer";
  wstring interchunk = L"interchunk";
  wstring postchunk = L"postchunk";

  if (!transferHeader.compare(2, transfer.size(), transfer)) {
    transferStage = TRANSFER;
    // Set chunker mode, by default 'lu'.
    if (!transferHeader.compare(20, 5, L"chunk")) {
      transferDefault = CHUNK;
    } else {
      transferDefault = LU;
    }
  } else if (!transferHeader.compare(2, interchunk.size(), interchunk)) {
    transferStage = INTERCHUNK;
  } else if (!transferHeader.compare(2, postchunk.size(), postchunk)) {
    transferStage = POSTCHUNK;
  }
}

/**
 * Set the input file to read the input of the vm.
 *
 * @param fileName input file's name
 */
void VM::setInputFile(char *fileName) {
  inputFileName = fileName;
}

/**
 * Set the output file to save the vm's output.
 *
 * @param fileName output file's name
 */
void VM::setOutputFile(char *fileName) {
  outputFileName = fileName;
}

/**
 * Set the debug mode, creating a debugger an setting it up as a proxy.
 */
void VM::setDebugMode() {
  debugMode = true;
  // TODO: Create the debugger proxy and its components.
}

/**
 * Load, preprocess and execute the contents of the files.
 */
void VM::run() {
}
