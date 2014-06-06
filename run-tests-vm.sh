#!/bin/bash
input=test/input/chunker/
output=test/expected_output/vm/
code=test/expected_output/compiler
indoutput=test/expected_output/individual_tests

echo "============================================"
echo "* Running tests..."
echo "* input = $input"
echo "* expected_output = $output"
echo "* code = $code"
echo ""

#Full tests.
for arg in `ls $output` ; do
  inputarg=$input$arg.txt
  cat $inputarg |\
    ./apertium-xfervm -c $code/apertium-en-ca.en-ca.v1x 2> test_warnings.log |\
    ./apertium-xfervm -c $code/apertium-en-ca.en-ca.v2x 2> test_warnings.log |\
    ./apertium-xfervm -c $code/apertium-en-ca.en-ca.v3x > vm.out 2> test_warnings.log
  if diff vm.out $output$arg > test_results.log ; then
    echo "+" $arg "-- OK"
  else
    echo "-" $arg "-- Error"
    # cat test_results.log
  fi
done

echo "============================================"
echo ""

#Individual tests:
echo "============================================"
echo "* Running individual tests..."
echo "* expected_output = $indoutput"
echo ""

#Test escaping characters inside the chunk content.
name=escape-sme-nob
./apertium-xfervm -c $code/apertium-sme-nob.sme-nob.v4x -i test/input/postchunk/$name 2> test_warnings.log > vm.out
  if diff vm.out $indoutput/$name > test_results.log ; then
    echo "+" $name "-- OK"
  else
    echo "-" $name "-- Error"
    #cat test_results.log
    fi

echo "============================================"
echo ""

rm -f vm.out test_results.log test_warnings.log
