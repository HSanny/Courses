############################################################
##    FILENAME:   Master.py    
##    VERSION:    1.0
##    SINCE:      2014-04-15
##    AUTHOR: 
##        Jimmy Lin (xl5224) - JimmyLin@utexas.edu  
##
############################################################
##    Edited by MacVim
##    Documentation auto-generated by Snippet 
############################################################

import sys, socket
from threading import Thread

import Util as U
import Protocol as P
import Logging as L

## TODO: add more static variable here..
'''Utitlity'''
localhost = socket.gethostname()
logHeader = L.MASTER_LOG_HEADER

'''Global States'''
allClients = set([])
allServers = set([])

'''Boolean Counter'''
restartAcks = None
pauseAcks = None

'''Semaphores'''
restartSema = None 
pauseSema = None 


def MasterListener(log):
    '''
    Hold on server socket and listen to all incoming message by infinite loop
    '''
    s = socket.socket()         # Create a socket object
    host = socket.gethostname() # Get local machine name
    port = P.MASTER_PORT               # Reserve a port for your service.
    s.bind((host, port))        # Bind to the port

    s.listen(5)                 # Now wait for client connection.
    while True:
        conn, addr = s.accept()     # Establish connection with client.
        # c.send('Thank you for connecting')  # send message to client
        recvMsg = conn.recv(P.BUFFER_SIZE)      # receive message with BUFFER_SIZE
        printRecvMessage(recvMsg, logHeader)
        st, si, rt, ri, title, content = decode (recvMsg)
        # TODO: add the processor
        if title == 'startupAck':
            pass
        elif title == PAUSE_ACK_TITLE:
            if U.checkCounterAllTrue(pauseAcks):
                pauseSema.release()
        elif title == RESTART_ACK_TITLE:
            if U.checkCounterAllTrue(restartAcks):
                startSema.release()
        else:
            pass

        conn.close()                # Close the connection
    s.close()


def MasterProcessor(log):
    for line in fileinput.input():
        line = line.split()
        if line[0] ==  "joinServer":
            serverId = int(line[1])
            '''
            Start up a new server with this id and connect it to all servers
            '''
            assert (serverIdx not in allServers), \
                    "JOIN: server %d already exists." % serverIdx
            allServers.update([serverIdx])

            print "join ", serverIdx, "completes."

        if line[0] ==  "retireServer":
            serverId = int(line[1])
            '''
            Retire the server with the id specified. This should block until
            the server can tell another server of its retirement
            ''' 
            retireMsg = encode (MASTER_TYPE, 0, SERVER_TYPE, serverId, \
                               RETIRE_REQUEST_TITLE, EMPTY_CONTENT)
            port = U.getPortByMsg (retireMsg)
            send (localhost, port, retireMsg, logHeader)
            ## TODO: retirement

        if line[0] ==  "joinClient":
            clientId = int(line[1])
            serverId = int(line[2])
            """
            Start a new client with the id specified and connect it to 
            the server
            """
            clientIdx = int (args[1])
            serverIdx = int (args[2])
            assert (clientIdx not in allClients), \
                    "START: Client %d already exists" % clientIdx
            assert (serverIdx in allServers), \
                    "START: Server %d not exists." % serverIdx
            allClients.update ([clientIdx])
            # call (["python src/Client.py", str(clientIdx)])

            print "startClient", clientIdx, serverIdx, "completes."

        if line[0] ==  "breakConnection":
            id1 = int(line[1])
            id2 = int(line[2])
            """
            Break the connection between a client and a server or between
            two servers
            """
        if line[0] ==  "restoreConnection":
            id1 = int(line[1])
            id2 = int(line[2])
            """
            Restore the connection between a client and a server or between
            two servers
            """
        if line[0] ==  "pause":
            """
            Pause the system and don't allow any Anti-Entropy messages to
            propagate through the system
            """
            pauseAcks = initAllFalseCounter(allServers)
            pauseSema = threading.Semaphore(0)
            sampleMsg = encode(MASTER_TYPE, 0,"xx",0, PAUSE_TITLE,"")
            U.broadcast(localhost, sampleMsg, logHeader, allServers, allClients)
            pauseSema.acquire()
        if line[0] ==  "start":
            """
            Resume the system and allow any Anti-Entropy messages to
            propagate through the system
            """
            restartAcks = initAllFalseCounter(allServers)
            restartSema = threading.Semaphore(0)
            sampleMsg = encode(MASTER_TYPE, 0,"xx",0, RESTART_TITLE,"")
            U.broadcast (localhost, sampleMsg, logHeader, allServers, allClients)
            restartSema.acquire()
        if line[0] ==  "stabilize":
            """
            Block until there are enough Anti-Entropy messages for all values to 
            propagate through the currently connected servers. In general, the 
            time that this function blocks for should increase linearly with the 
            number of servers in the system.
            """
        if line[0] ==  "printLog":
            serverId = int(line[1])
            """
            Print out a server's operation log in the format specified in the
            handout.
            """
        if line[0] ==  "put":
            clientId = int(line[1])
            songName = line[2]
            URL = line[3]
            """
            Instruct the client specified to associate the given URL with the given
            songName. This command should block until the client communicates with
            one server.
            """ 
        if line[0] ==  "get":
            clientId = int(line[1])
            songName = line[2]
            """
            Instruct the client specified to attempt to get the URL associated with
            the given songName. The value should then be printed to standard out of 
            the master script in the format specified in the handout. This command 
            should block until the client communicates with one server.
            """ 
        if line[0] ==  "delete":
            clientId = int(line[1])
            songName = line[2]
            """
            Instruct the client to delete the given songName from the playlist. 
            This command should block until the client communicates with one server.
            """ 

    # Command processing completes
    # TODO: send messages to terminate the whole system
    for clientIdx in allClients:
        exitMsg = U.encode (P.MASTER_TYPE, 0, P.CLIENT_TYPE, clientIdx, \
                          P.EXIT_TITLE, P.EMPTY_CONTENT)
        port = P.CLIENT_PORT_BASE + clientIdx;
        U.send (localhost, port, exitMsg, logHeader)

    for serverIdx in allServers:
        exitMsg = U.encode (P.MASTER_TYPE, 0, P.SERVER_TYPE, serverIdx, \
                          P.EXIT_TITLE, P.EMPTY_CONTENT)
        port = P.SERVER_PORT_BASE + serverIdx;
        U.send (localhost, port, exitMsg, logHeader)

    exitMsg = U.encode (P.MASTER_TYPE, 0, P.MASTER_TYPE, 0, \
                          P.EXIT_TITLE, P.EMPTY_CONTENT)
    port = P.MASTER_PORT;
    U.send (localhost, port, exitMsg, logHeader)

''' Program entry '''
if __name__ == "__main__":
    # out logging file
    log = open(L.Master_LOG_FILENAME, 'w+')
    # create listener thread 
    listener = Thread(target=MasterListener, args=(log,))
    processor = Thread(target=MasterProcessor, args=(log,))

    listener.start()
    processor.start()

    processor.join()
    listener.join()
    log.close()
