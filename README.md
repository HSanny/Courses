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
    ./RUN 3 times  - run the simple test for $times iterations
    ./RUN 4 tiems  - run the threenode test for $times iterations

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

TODO
----------------
1. Crash instruction: kill a server and then re-elect a new leader if that server carries current leader.
    
    Leader creates heartbeat thread that sends timed heartbeat to every other replica
    Replica holds global variable for lastHeartbeatReceived
    Replica creates thread for counting timeout
    On receipt of a heartbeat message, Replica updates lastHeartbeatReceived with currentTime(or attached time?)
    What happens when counter reaches timeout?
        Interrupt replica
        End counter
        replica enters leader election
        if receive message not related to election:
            cache message in queue

    Idea for Leader Election:
        Use Heartbeats for detecting failure.
        Non-leader servers use the following algorithm:
        <pre><code>
            if detect_time_out:
                if self_has_not_proposed:
                    propose self as leader
            if receive_leader_proposal:
                if sender_idx_is_lower:
                    accept proposal
                    if self_has_proposed:
                        cancel proposal
                else:
                    ignore proposal
                    if self_has_not_proposed:
                        propose
        </pre></code>
        Election-related messages:
            leader_proposal
            leader_proposal_ack
        New variables for the replica:
            hasProposed

    After election-New Leader (when you receive all leader_proposal_acks):
        start leader thread
        execute all proposals which have not been decided
        execute all queued messages
    
    After election-Other Replicas
        On receipt of new heartbeat, start a new counter

2. Write tests for recover and crash
3. FIXME: should commander care about skip slot instruction. see code.

Problems
---------------

Sometimes there are Connection problems:

1. How to crash a server in practice? Should we use "kill" process by external system call or in the manner of message passing. In latter case, the server may not crash immediately. 
    
    Answered by TA: In theory, nothing is to be stored to stable storage. So, when we crash a server, it should lose everything. The easiest way to do this is to just kill the process.
    
2. How do we recover from crased server? What information should we make use of?

    Answered by TA: When a server is restarted by the Master, it needs to ask every other server about the state of the system. When the server is caught up on the state of the system, it will resume normal leader/acceptor actions.

3. In the timeBombLeader function, what is the precise definition paxos related messages? 

4. There should ideally be only one leader at a time. Every replica should have a leader, but only one is actively doing leader activities. Use heartbeats to determine when a leader fails, and activate the next leader. If there are more than one leaders up at a time, there should be a way to narrow it back down to one (e.g. if a server who is acting as leader receives a leader heartbeat from a server whose index is lower than his, then he will deactivate his leader).

5. Paxos sequence number == slot number. Since the spec needs it to be zero-indexed, just make the slot number zero-indexed, unlike the pseudocode. The function of cid is so that if a replica receives two "hello" messages from a single client, it knows whether that is a repeat message or actually two different messages. 

6. Ballot numbers don't have to necessarily be globally unique; the problem we want to avoid is where two replicas each have a ballot with the same number but different proposals. We can solve this problem by saying, from the point of view of the acceptor, we accept the ballot which came from the server (or client) with the higher index. 

7. Why does the pseudocode show <r', L'> instead of b? For commander.

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
