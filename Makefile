#Simple Makefile to compile the compiler and vm. Proper autotools support will
#be added later, following the model already used by the rest of the Apertium 
#project.

CC=g++
OPTIONS= -g -Wall
CFLAGS=`xml2-config --cflags` -I/usr/local/include/lttoolbox-3.2 -I/usr/local/lib/lttoolbox-3.2/include
LIBS=`xml2-config --libs` -L/usr/local/lib -llttoolbox3
COMPILER_DIR=./src/compiler
_OBJ= compiler.o xml_parser.o wstring_utils.o event.o event_handler.o assembly_code_generator.o
OBJ = $(patsubst %,$(COMPILER_DIR)/%,$(_OBJ))

.PHONY: all clean doc

all: compiler

compiler: apertium_compiler.cc $(OBJ)
	$(CC) $(CFLAGS) -I $(COMPILER_DIR) $(OPTIONS) apertium_compiler.cc $(OBJ) -o apertium-compiler $(LIBS)

$(COMPILER_DIR)/%.o : $(COMPILER_DIR)/%.cc $(COMPILER_DIR)/%.h
	$(CC) $(CFLAGS) -I $(COMPILER_DIR) $(OPTIONS) -c -o $@ $< $(LIBS)

doc:
	doxygen

clean:
	rm -f $(OBJ) apertium-compiler ./src/*~ ./src/*/*.o doxygen.log
	rm -rf doc html 
