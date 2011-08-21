#!/bin/bash
input=test/input/chunker/
output=test/expected_output/vm/
code=test/expected_output/compiler

echo "============================================"
echo "* Compiling..."
make vm
echo "============================================"

echo "============================================"
echo "* Running tests..."
echo "* input = $input"
echo "* expected_output = $output"
echo "* code = $code"
echo "============================================"

for arg in `ls $output` ; do
  inputarg=$input$arg.txt
  cat $inputarg | ./apertium-vm -c $code/apertium-en-ca.en-ca.v1x | ./apertium-vm -c $code/apertium-en-ca.en-ca.v2x | ./apertium-vm -c $code/apertium-en-ca.en-ca.v3x > vm.out
  if diff vm.out $output$arg > test_results.log ; then
    echo $arg "-- OK"
  else
    echo $arg "-- Error"
    #cat test_results.log
    fi
done

rm -f vm.out test_results.log
make clean

