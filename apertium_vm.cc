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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <libgen.h>
#include <locale>

#include <vm.h>
#include <vm_exceptions.h>

using namespace std;

void showHelp(char *progName) {
  cerr << "USAGE: " << basename(progName)
       << " -c code_file [-i input_file] [-o output_file] [-g] [-h]" << endl;
  cerr << "Options:" << endl;
  cerr << "  -c, --codefile:\t a [chunker|interchunk|postchunk] compiled "
       << "rules file" << endl;
  cerr << "  -i, --inputfile:\t input file (stdin by default)" << endl;
  cerr << "  -o, --outputfile:\t output file (stdout by default)" << endl;
  cerr << "  -g, --debug:\t\t debug interactively the program code" << endl;
  cerr << "  -h, --help:\t\t show this help" << endl;
}

/**
 * Test if a file exists and can be read.
 *
 * @param fileName the name of the file to test
 */
bool testFile(const char *fileName, ios_base::openmode mode) {
  wfstream file;
  file.open(fileName, mode);

  if (file.good()) {
    file.close();
    return true;
  } else {
    return false;
  }
}

/**
 * The main program which initializes the vm and executes it.
 */
int main(int argc, char *argv[] ) {
  bool codeFileSupplied = false;
	static struct option long_options[] =
		{
		  {"codefile", required_argument, 0, 'c' },
		  {"inputfile", required_argument, 0, 'i' },
		  {"outputfile", required_argument, 0, 'o' },
		  {"debug", no_argument, 0, 'g' },
		  {"help", no_argument, 0, 'h' },
		  { 0, 0, 0, 0 }
		};

	// Set the C++ global locale as the one in current use by the user.
  locale loc = locale("");
  locale::global(loc);

  // Create the vm to initialize it with the options passed as argument.
  VM vm;

  while (true) {
    int option_index = 0;

    int c = getopt_long(argc, argv, "c:i:o:gh", long_options, &option_index);

    // Detect the end of the options.
    if (c == -1)
      break;

    switch (c) {
    case 'h':
      showHelp(argv[0]);
      return EXIT_FAILURE;
      break;
    case 'c': {
      char *codeFile = optarg;
      if (!testFile(codeFile, ios::in)) {
        cerr << "Error: Can't open code file '" << codeFile << "'" << endl;
        return EXIT_FAILURE;
      } else {
        try {
          vm.setCodeFile(codeFile);
        } catch (VmException &e) {
          wcerr << L"Error: " << e.getMessage() << endl;
          return EXIT_FAILURE;
        }
      }
      codeFileSupplied = true;
      break;
    }
    case 'i': {
      char *inputFile = optarg;
      if (!testFile(inputFile, ios::in)) {
        cerr << "Error: Can't open input file '" << inputFile << "'" << endl;
        return EXIT_FAILURE;
      } else {
        vm.setInputFile(inputFile);
      }
      break;
    }
    case 'o': {
      char *outputFile = optarg;
      if (!testFile(outputFile, ios::out)) {
        cerr << "Error: Can't open output file '" << outputFile << "'" << endl;
        return EXIT_FAILURE;
      } else {
        vm.setOutputFile(outputFile);
      }
      break;
    }
    case 'g':
      vm.setDebugMode();
      break;
    }
  }

  // If a code file isn't supplied, there won't be any loader so we end.
  if (!codeFileSupplied) {
    showHelp(argv[0]);
    return EXIT_FAILURE;
  }

  vm.run();

  loc.~locale();

  return EXIT_SUCCESS;
}
