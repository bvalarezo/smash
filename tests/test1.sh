#!/bin/bash

echo Test 1
out_d=$(pwd)/tests/rdump
prog=$(find "$(pwd)" -name smash)
script_file=$(find "$(pwd)" -name one.sh) 
echo $prog -d $script_file
$prog -d $script_file 2> $out_d/one.log
if [ $? = 0 ] ; then
    echo "Return Code 0; Success"
else
    echo "Return Code $?; Failure!"
fi