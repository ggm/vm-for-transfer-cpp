#!/bin/bash
input=test/data/
output=test/expected_output/

echo "============================================"
echo "* Running tests..."
echo "* input = $input"
echo "* expected_output = $output"
echo "============================================"

for arg in `ls $output` ; do
  inputarg=$input$arg
  inputarg=${inputarg/.v1x/.t1x}
  inputarg=${inputarg/.v2x/.t2x}
  inputarg=${inputarg/.v3x/.t3x}
  python3 src/compiler/ -i $inputarg -d compiler.log > compiler.out
  if diff compiler.out $output$arg > test_results.log ; then
    echo $arg "-- OK"
  else
    echo $arg "-- Error"
    #cat test_results.log
    fi
done

rm -f compiler.log compiler.out test_results.log
