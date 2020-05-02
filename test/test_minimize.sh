#!/bin/bash

source common.sh

specs=(

"acpp minimize ABC+ABCD"					"ABC"
"acpp minimize --dont-care AB'C' ABC+AB'C+A'BC+A'B'C"		"C"
"acpp minimize -n 4 ABC'+ACD'+B'CD+A'BD -e"			"AB'C+ABD'+A'CD+BC'D
								 A'BD+ABC'+ACD'+B'CD"
"acpp minimize --minterms    -d	 0,2,3 0,1,5,10,13,14"		"A'B'+ACD'+BC'D"

)

colored=0
if [ "$1" != '--nocolor' ] && [ "$1" != "-n" ]; then 
	colored=1
fi

red='\e[1;31m'
def='\e[0m'
grn='\e[1;32m'
blu='\e[1;34m'

# Set color for output
c() {
	if [ $colored == 1 ]; then
		echo -n -e "${!1}"
	fi
}

# Compare two DNFs
compare() {
	dnf1=( $(echo $1 | awk  '{gsub("+", " "); print}') )
	dnf2=( $(echo $2 | awk  '{gsub("+", " "); print}') )
	if (( ${#dnf1[*]} < ${#dnf2[*]} )); then
		dnf="$dnf1"
		dnf1="$dnf2"
		dnf2="$dnf1"
	fi
	for elmt1 in ${dnf1[*]}; do
		found=0
		for elmt2 in ${dnf2[*]}; do
			if [ $elmt2 == $elmt1 ]; then
				found=1
				break
			fi
		done
		if [ $found == 0 ]; then
			return 1
		fi
	done
}

# Usage: test command {expected_output excluding first line}
test() {
	output="$($1 2>/dev/null)"

	# Header
	echo -e "$(c blu)- Command $n -$(c def)\n"
	echo -e "$output\n";
	echo -e "Expected:\n$2" | tr -d '[[:blank:]]'

	# Make an array of all DNFs in the command output
	dnfs=( $(echo "$output" | tail -n +2) )

	# Test if the two DNFs have the same number of elements	
	if [ ${#dnfs[*]} != $(echo "$2" | wc -w)  ]; then
		echo -e "$(c red)Error in command $n$(c def)\n" >&2
		return 1
	fi
	
	# Compare each output DNF with the corresponding expected DNF
	for ((  i = 0; i < ${#dnfs[*]}; i++ )); do 
		compare "${dnfs[$i]}" "$2" || { echo -e "$(c red)Error in command $n$(c def)\n" >&2; return 1; }
	done
	echo -e "$(c grn)Command $n successful$(c def)\n"
}

size=${#specs[*]}

if (( $size % 2)); then
	echo "Invalid command/result specification"; exit 1
fi
for i in $( seq 0 2 $(( $size - 1 )) ); do
	n=$(($i / 2))
	test "${specs[$i]}" "${specs[ $(($i+1)) ]}"
done
