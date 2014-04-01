PAXOS Project 
=============

Contributors
> Jimmy Lin (xl5224)
> Calvin Szeto 

USAGE
-----------------
To run the sample tests, replace test_name with the name of the test and execute the following command:

cat tests/[test_name].test | $(cat COMMAND)

To automatically check the output:

cat tests/[test_name].test | $(cat COMMAND) > temp_output && diff -q temp_output tests/[test_name].sol

If your output matches the solution, NOTHING will be printed. Otherwise the lines that differ will be shown. 
The output for the run of the test will also be stored in a file temp_output after running the second command.

RUN USAGE:
    ./RUN 1   - run the simpletest
    ./RUN 2   - run the harder test
