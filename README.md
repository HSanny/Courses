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


Protocol Design
----------------

Message Format: 

SENDER_TYPE, SENDER_INDEX, RECEIVER_TYPE, RECEIVER_INDEX, TITLE, CONTENT



Problems
---------------




Implementation Details
---------------
0. At very beginning, the master establish its server socket to listen to all
   requests.

1. To start up all clients and servers, we use runtime.exec to run clients and
   servers as new processes. The arguments provided to them is clientIndex or
   serverIndex. After creating all processes, the master program waits for 
   arrivals of setup acknowledgement of created processe (servers and
   clients). Master proceeds to read the next instruction only when it
   collects all acknowledgements. 
