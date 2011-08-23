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

#include <compiler.h>

using namespace std;

void showHelp(char *progName) {
  cerr << "USAGE: " << basename(progName)
       << " [-d debug_file] [-i input_file] [-o output_file] [-h]" << endl;
  cerr << "Options:" << endl;
  cerr << "-d, --debug:\t\t show debug messages" << endl;
  cerr << "-i, --inputfile:\t input file (stdin by default)" << endl;
  cerr << "-o, --outputfile:\t output file (stdout by default)" << endl;
  cerr << "-h, --help:\t\t show this help" << endl;
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
 * The main program which initializes the compiler and executes it.
 */
int main(int argc, char *argv[]) {
  static struct option long_options[] =
    {
      {"debug", required_argument, 0, 'd' },
      {"inputfile", required_argument, 0, 'i' },
      {"outputfile", required_argument, 0, 'o' },
      { "help", no_argument, 0, 'h' },
      { 0, 0, 0, 0 }
    };

	// Set the C++ global locale as the one in current use by the user.
  locale loc = locale("");
  locale::global(loc);

  // Create the compiler to initialize it with the options passed as argument.
  Compiler compiler;

  while (true) {
    int option_index = 0;

    int c = getopt_long(argc, argv, "d:i:o:h", long_options, &option_index);

    // Detect the end of the options.
    if (c == -1)
      break;

    switch (c) {
    case 'h':
      showHelp(argv[0]);
      return EXIT_FAILURE;
      break;
    case 'd': {
      char *debugFile = optarg;
      if (!testFile(debugFile, ios::out)) {
        cerr << "Error: Can't open debug file '" << debugFile << "'" << endl;
        return EXIT_FAILURE;
      } else {
        compiler.setDebug(debugFile);
      }
      break;
    }
    case 'i': {
      char *inputFile = optarg;
      if (!testFile(inputFile, ios::in)) {
        cerr << "Error: Can't open input file '" << inputFile << "'" << endl;
        return EXIT_FAILURE;
      } else {
        compiler.setInputFile(inputFile);
      }
      break;
    }
    case 'o': {
      char *outputFile = optarg;
      if (!testFile(outputFile, ios::out)) {
        cerr << "Error: Can't open output file '" << outputFile << "'" << endl;
        return EXIT_FAILURE;
      } else {
        compiler.setOutputFile(outputFile);
      }
      break;
    }
    }
  }

  compiler.compile();

  loc.~locale();

  return EXIT_SUCCESS;
}
