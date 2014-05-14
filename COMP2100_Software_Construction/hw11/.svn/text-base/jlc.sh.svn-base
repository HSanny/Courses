#!/bin/bash
#############################################
## FILENAME:    jlc.sh
## AUTHOR:      Jimmy Lin (u5223173)
## DATE:        2013-05-19
## DESCRIPTION: 
##    Homework 11 (part 1) of COMP2100
## TEST:
##    ./jlc.sh ./testFiles/*.java
#############################################

## initialise the used local variable
nLines=0  ## line counter
nFiles=0  ## file number counter
## loop through all input file
for javaFile in ${*}
do
    ## redirect content of one input file to script jncnb.sh
    ## and then pipe it into word count program with line conting option
    ## assign the result to variable tempLines
    tempLines=$(./jncnb.sh < ${javaFile} | wc -l)
    ## report the result with file names
    echo "   $tempLines ${javaFile}"
    ## increment the line counter and file number counter
    let nLines=nLines+tempLines
    let nFiles=nFiles+1
done
## report the summary information
echo $nFiles files, $nLines lines.
