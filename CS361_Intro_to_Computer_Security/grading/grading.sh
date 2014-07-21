#!/bin/sh

# Autograding bash script
# Input arguments:
#   1. homework number
#   2. 

students=$(ls ./cs361_hw${1})
threshold=100

for stud in $students
do
    lines=$(cat ./cs361_hw${1}/${stud}/* | wc -l)
    if [ ${lines} -lt ${threshold} ]
    then 
        grade='-'
        echo ${stud}, ${lines}, ${grade}
    else
        grade='+'
    fi

done
