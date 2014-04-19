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

import sys, socket, os, subprocess
from threading import Thread

from Util import *
from Protocol import *
from Logging import *

## TODO: add more static variable here..
'''Utitlity'''
localhost = socket.gethostname()
logHeader = MASTER_LOG_HEADER

'''Global States'''
allClients = set([])
allServers = set([])

'''Boolean Counter'''
restartAcks = None
pauseAcks = None

'''Semaphores'''
joinClientSema = initEmptySemaphore() 
joinServerSema = initEmptySemaphore()
restartSema = None 
pauseSema = None 
listenerSetUpSema = None

def MasterListener():
    '''
    Hold on server socket and listen to all incoming message by infinite loop
    '''
    s = socket.socket()         # Create a socket object
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((localhost, MASTER_PORT))        # Bind to the port
    listenerSetUpSema.release()

    s.listen(5)                 # Now wait for client connection.
    while True:
        conn, addr = s.accept()     # Establish connection with client.
        # c.send('Thank you for connecting')  # send message to client
        recvMsg = conn.recv(BUFFER_SIZE)      # receive message with BUFFER_SIZE
        printRecvMessage(recvMsg, logHeader)
        st, si, rt, ri, title, content = decode (recvMsg)
        # TODO: add the processor
        if title == JOIN_SERVER_ACK_TITLE:
            joinServerSema.release()
        elif title == JOIN_CLIENT_ACK_TITLE:
            joinClientSema.release()
        elif title == PAUSE_ACK_TITLE:
            if checkCounterAllTrue(pauseAcks):
                pauseSema.release()
        elif title == RESTART_ACK_TITLE:
            if checkCounterAllTrue(restartAcks):
                startSema.release()
        elif title == EXIT_TITLE:
            conn.close()
            s.close()
            return 
        else:
            pass

        conn.close()                # Close the connection
    s.close()

def MasterProcessor():
    for line in sys.stdin:
        line = line.strip()
        print "[INPUT]", line
        line = line.split()
        if line[0] ==  "joinServer":
            serverId = int(line[1])
            '''
            Start up a new server with this id and connect it to all servers
            '''
            assert (serverId not in allServers), \
                    "JOIN: server %d already exists." % serverIdx

            ## invoke new server
            cmd = "python -u src/Server.py "+ str(serverId)
            os.system(cmd + " &")
            print cmd

            ## block until receiving acks
            joinServerSema.acquire()
            ## confirm to add new server to its list
            allServers.update([serverId])
            print "joinServer ", serverId, "completes."

        if line[0] ==  "retireServer":
            serverId = int(line[1])
            '''
            Retire the server with the id specified. This should block until
            the server can tell another server of its retirement
            ''' 
            retireMsg = encode (MASTER_TYPE, 0, SERVER_TYPE, serverId, \
                               RETIRE_REQUEST_TITLE, EMPTY_CONTENT)
            port = getPortByMsg (retireMsg)
            send (localhost, port, retireMsg, logHeader)
            ## TODO: retirement Protocol

        if line[0] ==  "joinClient":
            clientId = int(line[1])
            serverId = int(line[2])
            """
            Start a new client with the id specified and connect it to 
            the server
            """
            assert (clientId not in allClients), \
                    "START: Client %d already exists" % clientId
            assert (serverId in allServers), \
                    "START: Server %d not exists." % serverId

            ## invoke new client
            cmd = "python -u src/Client.py "+ str(clientId) + " "+ str(serverId) 
            os.system(cmd+ " &")
            print cmd

            ## block until received ack
            joinClientSema.acquire()
            ## confirm to add new client to its list
            allServers.update([serverId])
            allClients.update ([clientId])

            print "startClient", clientId, serverId, "completes."

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
            pauseSema = initEmptySemaphore()
            sampleMsg = encode(MASTER_TYPE, 0,"xx",0, PAUSE_TITLE,"")
            broadcastServers (localhost, sampleMsg, logHeader, allServers, allClients)

            pauseSema.acquire()
        if line[0] ==  "start":
            """
            Resume the system and allow any Anti-Entropy messages to
            propagate through the system
            """
            restartAcks = initAllFalseCounter(allServers)
            restartSema = initEmptySemaphore()
            sampleMsg = encode(MASTER_TYPE, 0,"xx",0, RESTART_TITLE,"")
            broadcastServers (localhost, sampleMsg, logHeader, allServers, allClients)
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
    exitMsg = encode (MASTER_TYPE, 0, "xx", -1, EXIT_TITLE, \
                        EMPTY_CONTENT)
    broadcast (localhost, exitMsg, logHeader, allServers, allClients)

    exitMsg = encode (MASTER_TYPE, 0, MASTER_TYPE, 0, \
                          EXIT_TITLE, EMPTY_CONTENT)
    send (localhost, MASTER_PORT, exitMsg, logHeader)

''' Program entry '''
if __name__ == "__main__":
    # out logging file
    origin_out = sys.stdout
    origin_err = sys.stderr
    '''
    logFile = open(Master_LOG_FILENAME, 'w+')
    sys.stdout = logFile
    sys.stderr = logFile
    '''
    # create listener thread 
    listenerSetUpSema = initEmptySemaphore()
    listener = Thread(target=MasterListener, args=())
    listener.start()
    ## block until master's listener has setup
    listenerSetUpSema.acquire()

    processor = Thread(target=MasterProcessor, args=())
    processor.start()


    processor.join()
    listener.join()
    # sys.stdout.close()
    sys.stdout = origin_out
    sys.stderr = origin_err
    print "Exit."
