###################################################
## FILENAME:    BookFreq.sh
## AUTHOR:      Jimmy Lin (u5223173) 
## DATE:        2013-05-19
## DESCRIPTION: 
##     Homework 11 (part 2) in COMP2100
##
###################################################
## Edited by MacVim
## Documentation autogenerated by snippet
###################################################

## Deal with the bookID counting by using one-line bash command

## Process as follows:
##   1. cut the sorted field
##   2. sort the .tsv file based on its bookID field
##   3. count the number of pieces for each unique bookID
cut -f 4 ./LoansList.tsv | sort -k 4 | uniq -c

## Note that the following command is also available 
## but the efficiency may not be satisfying when 
## the objective data file is enormous.

# sort -k 4 ./LoansList.tsv | cut -f 4 | uniq -c
