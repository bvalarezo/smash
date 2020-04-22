#!/bin/bash

echo Test 3
out_d=$(pwd)/tests/rdump
prog=$(find "$(pwd)" -name smash)
script_file=$(find "$(pwd)" -name three.sh) 
echo $prog -d $script_file
$prog -d $script_file 2> $out_d/3.log
if [ $? = 0 ] ; then
    echo "Return Code 0; Success"
else
    echo "Return Code $?; Failure!"
fi