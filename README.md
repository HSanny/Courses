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

	SENDER_TYPE 
	SENDER_INDEX
	RECEIVER_TYPE
	RECEIVER_INDEX
	TITLE
	CONTENT

TODO list
--------------
1. [DONE] naively implement joinServer and joinClient
    - [DONE] invoke process for server and client
    - [DONE] tell all existing server about the new server
    - [DONE] establish listener for server and client
    - [DONE] send ack back to master

2. [DONE] implement logging for server and client
    - [DONE] write new macro function
    - [DONE] use uncached file object to log the errors

3. [DONE] make system terminated by itself
    - [DONE] broadcast EXIT message to all servers and clients
    - [DONE] send EXIT message to master's listener thread

4. [DONE] implement put, get and delete
    - [DONE] request and ack sending and delivery
    - [DONE] use local data structure localLogs and localData for each server
    - [DONE] update the local structure on server that client connects to

5. implement pause and resume
    - [DONE] send message to all servers
    - [DONE] when received, send acks?
    - [DONE] use indicator to denote the working status
    - catogorize messages that are anti-entropy related 
    - disable these messages if switched off

6. [DONE] Connection Mechanism:
    - [DONE] servers initialize with knowledge all other servers, and no clients
    - [DONE] clients initialize with knowledge only one server
    - [DONE] breakConnection and restoreConnection updates above data structure

7. implement update propagation: 
    - anti-entropy routine (see pseudo-code at FUP Figure 2 or 3.) 
    - 

8. implement stabilization process: see FUP 3.1
    - update the stabilization 

9. implement creation protocol: see paper FUP 4.3
    - when node enters the system, should be brought up to date
    - 

10. implement retirement protocol: see paper FUP 4.3
    - initialize accept-stamp 
    - initialize local logs 
    - initialize version vector (use counter by util function)
    - 


Problems
---------------

Implementation Details
---------------



