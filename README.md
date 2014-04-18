PAXOS Project 
=============

##### Contributors

 	**Jimmy Lin** (UTEID: xl5224)  Login: jimmylin 
 	**Calvin Szeto** (UTEID: CS37888) Login: calvins

Slip days used (this project): __0 day__ 
Slip days used (total): __0 day__

USAGE:
---------------

To better test our implementation, we design a series of new tests based on given system requirement and **pass all of them in our own machine**. The series of tests are 

    TODO: add test

To run the sample tests, replace test_name with the name of the test and execute the following command:

    TODO: add command
	
An alternative is that you can use the RUN script designed by us, the usage is as follows:


or you can run all tests by simply typing:


If your output matches the solution, NOTHING will be printed. Otherwise the lines that differ will be shown. 
The output for the run of the test will also be stored in a file temp_output after running the second command.

CodeBase Decription
------------------
Here we provide brief introduction to the java files in the src/ directory.

	Master 

Protocol Design
----------------

####Message Format: 

	SENDER_TYPE 
	SENDER_INDEX
	RECEIVER_TYPE
	RECEIVER_INDEX
	TITLE
	CONTENT

TODO list
--------------
1. implement joinServer and joinClient
    - invoke process for server and client
    - establish listener for server and client
    - send ack back to master

2. implement logging for server and client
    - write new macro function

3. implement put, get and delete
    - local data structure for client and server

Problems
---------------

Implementation Details
---------------

