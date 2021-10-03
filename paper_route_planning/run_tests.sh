#!/bin/bash

### Options ###
files="berlin52"					# list of input files (thesis1,berlin52)
initial_slns="NEARESTNEIGHBOR RANDOM"			# list of initial solution methods (NEARESTNEIGHBOR RANDOM)
operators="TWOOPT"					# list of neighborhood operators (TWOOPT RELOCATE TWOOPT_AND_RELOCATE)
repititions="20"					# number of python calls per set of settings
iterations="1000"					# iterations while simulated annealing
debug="OFF"						# amount of command line output

### Script ###
for file in $files
do
	for initial_sln in $initial_slns
	do
		for operator in $operators
		do
			for ((z=0; z<$repititions;z++))
			do	
				#echo "$z"
				command="python3 solver.py --file $file --initial $initial_sln --operator $operator --debug $debug --iterations $iterations"
				echo $command
				$command
			done
		done 
	done 
done 
new_name="results_$(date +%F-%T).csv"
echo "Renamed output to $new_name"
mv results.csv $new_name
