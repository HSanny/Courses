PAXOS Project 
=============

##### Contributors

 	Jimmy Lin (xl5224)
 	Calvin Szeto 

USAGE:
---------------
TO run the provide test, you can use

    ./RUN 1   - run the simple test
    ./RUN 2   - run the threenode test

To run the sample tests, replace test_name with the name of the test and execute the following command:

	cat tests/[test_name].test | $(cat COMMAND)

To automatically check the output:

	cat tests/[test_name].test | $(cat COMMAND) > temp_output && diff -q temp_output tests/[test_name].sol

If your output matches the solution, NOTHING will be printed. Otherwise the lines that differ will be shown. 
The output for the run of the test will also be stored in a file temp_output after running the second command.

CodeBase Decription
------------------
Here we provide brief introduction to the java files in the src/ directory.

	Master  Provided master file as program driver
	Server
	Client
	
	Protocol
	Logging
	Util

	Leader
	Acceptor

Protocol Design
----------------

####Message Format: 

	SENDER_TYPE 
	SENDER_INDEX
	RECEIVER_TYPE
	RECEIVER_INDEX
	TITLE
	CONTENT


Problems
---------------

Sometimes there are Connection problems:

1. How to crash a server in practice? Should we use "kill" process by external system call or in the manner of message passing. In latter case, the server may not crash immediately. 
2. How do we recover from crased server? What information should we make use of?
3. In the timeBombLeader function, what is the precise definition paxos related messages? 



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
