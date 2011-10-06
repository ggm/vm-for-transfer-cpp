#!/bin/bash
input=test/input/chunker/
output=test/expected_output/vm/
code=test/expected_output/compiler

echo "============================================"
echo "* Building the vm..."
echo ""
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
  cat $inputarg | ./apertium-transfervm -c $code/apertium-en-ca.en-ca.v1x 2> test_warnings.log | ./apertium-transfervm -c $code/apertium-en-ca.en-ca.v2x 2> test_warnings.log | ./apertium-transfervm -c $code/apertium-en-ca.en-ca.v3x > vm.out 2> test_warnings.log
  if diff vm.out $output$arg > test_results.log ; then
    echo $arg "-- OK"
  else
    echo $arg "-- Error"
    #cat test_results.log
    fi
done

rm -f vm.out test_results.log test_warnings.log
make clean

