Bayou Project
=============

##### Contributors
 	**Jimmy Lin** (UTEID: xl5224)  Login: jimmylin
 	**Calvin Szeto** (UTEID: CS37888) Login: calvins

Slip days used (this project): __0 day__ Slip days used (total): __0 day__

USAGE:
---------------

To better test our implementation, we design a series of new tests based on given system requirement and **pass all of them in our own machine**. The series of tests are

    propagate.test
    conflict.test

    simple.test
    cover.test
    delete.test
    logging.test
    newborn.test
    retirePrimary.test
    retireNonPrimary.test

To run the sample tests, replace test_name with the name of the test and execute the following command:

    cat tests/scripts/[test_name].test | $(cat COMMAND)

An alternative is that you can use the RUN script designed by us, the usage is as follows:

    ./run -t tests/scripts/[test_name].test

or you can run all tests by simply typing:

    ./run

If your output matches the solution, NOTHING will be printed. Otherwise the lines that differ will be shown.
The output for the run of the test will also be stored in a file temp_output after running the second command.

CodeBase Decription
------------------
Here we provide brief introduction to the python files in the src/ directory.

	Master.py
    Server.py
    Client.py
    Protocol.py
    Logging.py
    Util.py

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
    - [DONE] catogorize messages that are anti-entropy related
    - [DONE] cached these messages if switched off
    - [DONE] prioritize the cached message if system is non-paused

6. [DONE] Connection Mechanism:
    - [DONE] servers initialize with knowledge all other servers, and no clients
    - [DONE] clients initialize with knowledge only one server
    - [DONE] breakConnection and restoreConnection updates above data structure

7. implement update propagation:
    - [DONE] anti-entropy routine (see pseudo-code at FUP Figure 2 or 3.)

    - [DONE] how to trigger anti-entropy routine in practice? Paper says that the
      trigger is chosen at random. Current implementation is to propagate the
      update on demand; that is, exchagne information between S and R when S
      detect R's ignorance of S's update.
    - See FUP 5.3: We use a combination of periodic reconciliation and system triggered
        reconciliation:
           On new server connection, do anti-entropy.
           Also, periodically do anti-entropy with a different server.

8. implement stabilization evaluation: see FUP 3.1
    - [DONE] evaluate if system is stable
    - [DONE] Master send stabilization checking request to every clients with
      round number
    - [DONE] Clients deliver checking request to servers it connects to
    - [DONE] Servers, when receive request, deliver checking request to server it connects to
    - [DONE] Servers, once receive request, start asking version vector (VV) of
      connected servers
    - [DONE] When one server receives one VV of its connected server, check if that VV
      up to its own current accept_stamp.
    - [DONE] When one server collects VV of all its connected servers, decide
      if it is stable and send decision back to Master.
    - [DONE] Once Master collects all decisions from un-isolated servers, it
      will make decision for stabilization. If stable, unblock reader thread.
      Otherwise, initiate a new round of stabilization checking. 
    - [DONE] initiate a new round of checking with a time interval? not
      necessarily for now.
    - [DONE] extend the naive implementation to committed write.
    - ??? what is ERR\_DEP?  See piazza's answer.

9. [DONE]implement creation protocol: see paper FUP 4.3

10. [DONE]implement retirement protocol: see paper FUP 4.3

11. implement conflict resolution: see the test and project specification
    - [DONE] for primary server, there is no conflict because primary is the
      one who determines the stable write log. All write issued by primary or
      sent to primary will be commited to be stable. 
    - [DONE] for non-primary server, once it receive a commited write from
      other server, it will check if that songName exists. If exists, it will
      apply the write with highest commit sequence number (csn) to the local
      data. (that is, to resolve the conflict between the previous committed
      write and currently committed write)

Implementation Details
---------------

## Creation and Retirement

Creation and retirement follow the Bayou protocol, meaning that servers other than the first server
are given a globally unique identifier of the form (T,S). In implementation, servers are also given
an integer identifier by the Master program, which is also used for communication purposes among
servers. Thus, each server holds a mapping between the Bayou IDs and "Master" IDS of all other servers;
as far as the Bayou logic is concerned, the Master IDs do not exist.

Other than the above caveat, creation and retirement follow the Bayou protocol, summarized below:

1. Creation process
    1:  joinServer creates a new Server process S_i, giving it a list of all servers.
    2:  If S_i is deemed primary, 
    3:       then it takes the Master ID as its Bayou ID.
    4:  Else,
    5:       the S_i sends a CREATE to a connected Server S_k 
    6:       S_k adds the CREATE to its write log
    7:       S_k adds the <T,S> to its version vector
    8:       S_k sends the Bayou ID <T,S> to S_i
    9:  S_i initializes its version vector as {0: 0, <T,S>: T+1} # Paper says to do T+1.. why?
    10: S_i sends an ack to Master with its new ID      
    11: Master sends a new server message to all other servers with the B->M mapping
    12: On receipt of new server message, a server adds the B->M mapping to its dict
2. Log write process
    When a Server reaches a log which is type CREATE or RETIRE, it needs to add or delete the entry from its
        version vector instead of doing anything to local data. Otherwise, this is treated as a regular action.
3. Anti-entropy process
    Let S_i be the sender, S_k be the receiver
        1: S_i requests and receives version vector from S_k
        2: For every server S_m = <T, S_n> in S_i.V, calculate S_k.CV(S_m) as follows:
        3:      If S_k.V(S_m) exists, then S_k.CV(S_m) = S_k.V(S_m)
        4:      Else If S_m == 0, the first Server, then S_k.CV(S_m) = +infinity
        5:      Else If S_k.CV(S_n) >= T, then S_k.CV(S_m) = +infinity
        6:      Else If S_k.CV(S_n) < T, then S_k.CV(S_m) = -infinity
        7: If S_k.CV(S_m) < S_i.V(S_m), then send all missing updates.
4. Retirement process
    1: Master sends retire to Server S_i
    2: S_i sends a RETIRE to itself
    3: S_i processes the RETIRE as a client message
    4: S_i chooses a Server to run anti-entropy
    5: When anti-entropy is complete, S_i sends an ack to Master
    6: S_i terminates
    If S_i is primary, then the server it runs anti-entropy with becomes the new primary
5. Print log
    Print log needs to ignore CREATE or RETIRE logs

## Session Guarantees

To ensure the Read Your Writes session guarantee, Clients hold a version vector of all servers, although
they themselves do not hold entries in the vector. The client version vector only updates on the client's
own writes - so, after a PUT request, the server will return an ack to the client telling what the accept
stamp was for its write.
On every PUT and GET request, the connected server will receive the version vector from the client and 
compare with their own. If the server knows about all the client's writes, then it proceeds as normal.
Otherwise, it ignores the PUT request or returns ERR_DEP on the GET request.
Note that since the Client only knows Master-given IDs, the Server will send its Bayou ID along with the
accept stamp so the Client version vectors match the Server ones.

References
---------------
[1] K. Petersen, M. J. Spreitzer, D. B. Terry, M. M. Theimer and A. J. Demers.
Flexible Update Propagation for Weakly Consistent Replication

[2] D. B. Terry, M. M. Theimer, K. Petersen, A. J. Demers, M. J. Spreitzer and
C. H. Hauser. Managing Update Conflicts in Bayou, a Weakly Connected Replicated Storage
Syste

[3] A. Demers, D. Greene, C. Hauser, W. Irish,
J. Larson, S. Shenker, H. Sturgis, D. Swinehart, and D. Terry. Epidemic
algorithms for replicated database maintenance.
