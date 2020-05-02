#!/bin/bash

source common.sh

specs=(

"acpp veitch ABC+AB'C"
"acpp veitch -n 4 --show-all ABC'+ACD'+B'CD+A'BD"
"acpp veitch 0,1,2,3,7,9,11,16,23,25,26,29,31 -d 4,10,13,15 -m -s 2"

)

for (( i = 0; i < ${#specs[*]}; i++ )); do
	echo "Command $i"
	${specs[$i]}
done
