#Simple Makefile to compile the compiler and vm. Proper autotools support will
#be added later, following the model already used by the rest of the Apertium
#project.

CC=g++
# OPTIONS= -mtune=nocona -fomit-frame-pointer -funroll-loops -O3 -Wall --std=c++11
OPTIONS= -g -Wall --std=c++11
PREFIX=/usr/local

#Compiler variables
COMPILER_DIR=./src/compiler
COMP_CFLAGS=`xml2-config --cflags` -I$(PREFIX)/include/lttoolbox-3.3
COMP_LIBS=`xml2-config --libs` -L$(PREFIX)/lib -llttoolbox3
_COMP_OBJ= compiler.o xml_parser.o wstring_utils.o event.o event_handler.o assembly_code_generator.o symbol.o symbol_table.o
COMP_OBJ = $(patsubst %,$(COMPILER_DIR)/%,$(_COMP_OBJ))

#VM variables
VM_DIR=./src/vm
VM_CFLAGS=
VM_LIBS=
_VM_OBJ= vm.o scope.o assembly_loader.o bilingual_lexical_unit.o bilingual_word.o chunk_lexical_unit.o chunk_word.o vm_wstring_utils.o system_trie.o call_stack.o interpreter.o
VM_OBJ = $(patsubst %,$(VM_DIR)/%,$(_VM_OBJ))

.PHONY: all clean doc test

all: compiler vm

compiler: apertium_compiler.cc $(COMP_OBJ)
	$(CC) $(COMP_CFLAGS) -I $(COMPILER_DIR) $(OPTIONS) apertium_compiler.cc $(COMP_OBJ) -o apertium-compile-transfer $(COMP_LIBS)

$(COMPILER_DIR)/%.o : $(COMPILER_DIR)/%.cc $(COMPILER_DIR)/%.h
	$(CC) $(COMP_CFLAGS) -I $(COMPILER_DIR) $(OPTIONS) -c -o $@ $< $(COMP_LIBS)

vm: apertium_vm.cc $(VM_OBJ)
	$(CC) $(VM_CFLAGS) -I $(VM_DIR) $(OPTIONS) apertium_vm.cc $(VM_OBJ) -o apertium-xfervm $(VM_LIBS)

$(VM_DIR)/%.o : $(VM_DIR)/%.cc $(VM_DIR)/%.h
	$(CC) $(VM_CFLAGS) -I $(VM_DIR) $(OPTIONS) -c -o $@ $< $(VM_LIBS)

install:
	cp apertium-compile-transfer $(PREFIX)/bin
	cp apertium-xfervm $(PREFIX)/bin

doc:
	doxygen

test: all
	./run-tests-compiler.sh
	./run-tests-vm.sh

clean:
	rm -f $(OBJ) apertium-compile-transfer apertium-xfervm ./src/*~ ./src/*/*.o doxygen.log
	rm -rf doc html
