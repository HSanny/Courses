############################################################
##    FILENAME:   Server.py    
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
from Protocol import *
from Logging import *
from Util import *

## TODO: static variable here
logHeader = None
localhost = socket.gethostname() 

def main(argv):
    '''
    Main function of server
    '''
    assert len(argv) >= 2, "SERVER: too less arguments"

    ## initialize static variables
    pause = False
    serverID = int(argv[0])
    allServers = str2set(argv[1])
    logHeader = SERVER_LOG_HEADER % serverID
    writeLogs = initWriteLogs()
    versionVector = initVersionVector()
    accept_stamp = 0
    

    ## construct server socket
    s = socket.socket()         
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    port = SERVER_PORT_BASE + serverID
    s.bind((localhost, port))        
    ## send back acknowledgement
    ackMsg = encode (SERVER_TYPE, serverID, MASTER_TYPE, 0,\
                      JOIN_SERVER_ACK_TITLE, EMPTY_CONTENT)
    send (localhost, MASTER_PORT, ackMsg, logHeader)

    s.listen(5)
    while True:
        conn, addr = s.accept()     
        recvMsg = conn.recv(BUFFER_SIZE)

        '''Incoming message preprocessing'''
        st, si, rt, ri, title, content = decode(recvMsg)
        printRecvMessage(recvMsg, logHeader)

        '''Processing incoming messages'''
        if title == JOIN_SERVER_TITLE:
            newServerId = int(content)
            allServers.add(newServerId)
            ackMsg = encode(SERVER_TYPE, serverID, MASTER_TYPE, 0, \
                           JOIN_SERVER_ACK_TITLE, str(newServerId))
            send(localhost, MASTER_PORT, ackMsg, logHeader)

        elif title == JOIN_CLIENT_ACK_TITLE:
            deliverAckMsg = encode (SERVER_TYPE, serverID, \
               MASTER_TYPE, 0, JOIN_CLIENT_ACK_TITLE, EMPTY_CONTENT)
            port = getPortByMsg(deliverAckMsg)
            send(localhost, port, deliverAckMsg, logHeader)

        elif title == PRINT_LOG_TITLE:
            assert (st == MASTER_TYPE)
            ## generate logstr
            opLogs = [oplog for (_, _, opLogs) in writeLogs]
            logstr = writeLogs.join(LOG_SEP)

            ## send print log response back to master
            logMsg = encode(SERVER_TYPE, serverID, st, si, \
                           PRINT_LOG_RESPONSE_TITLE, logstr)
            send(localhost, MASTER_PORT, logMsg, logString)

        elif title == BREAK_CONNECTION_TITLE:
            toBreakServerId = int(content)
            assert (toBreakServerId in allServers)
            allServers.remove(toBreakServerId)
            ## TODO: send ack stuff?
            
        elif title == RESTORE_CONNECTION_TITLE:
            toRestoreServerId = int(content)
            assert (toRestoreServerId not in allServers)
            allServers.add(toRestoreServerId)
            ## TODO: send ack stuff??

        elif title == PAUSE_TITLE:
            ## switch the pause indicator
            pause = True
            ## send pause acknowledgement to master
            pauseAckMsg = encode(SERVER_TYPE, serverID, MASTER_TYPE, 0, \
                                 PAUSE_ACK_TITLE, EMPTY_CONTENT)
            send (localhost, MASTER_PORT, pauseAckMsg, logHeader)
            ## TODO: other mechanism to pause the system
            #  - cache the following incoming messages
            #  - cache the following anti-entroy messages

        elif title == RESTART_TITLE:
            ## switch the pause indicator
            pause = False
            ## send restart acknowledgement to master
            restartAckMsg = encode(SERVER_TYPE, serverID, MASTER_TYPE, 0 ,\
                                  RESTART_ACK_TITLE, EMPTY_CONTENT)
            send(localhost, MASTER_PORT, restartAckMsg, logHeader)
            ## TODO: other mechanism to restart the system
            #  - cache the following incoming messages
            #  - cache the following anti-entroy messages

        elif title == PUT_REQUEST_TITLE:
            ## update locallog
            [sn, URL] = content.split(SU_SEP)
            putlog = LOG_FORMAT % (PUT, OP_VALUE % (sn, URL), bool2str(False))
            writeLogs.append((accept_stamp, serverID, putlog))
            accept_stamp += 1
            ## update local datastore
            localData.update({sn:URL})
            ## send ack back
            putAckMsg = encode(rt, ri, st, si, PUT_ACK_TITLE, EMPTY_CONTENT)
            port = getPortByMsg(putAckMsg)
            send(localhost, port, putAckMsg, logHeader)

        elif title == GET_REQUEST_TITLE:
            ## look up local data store
            sn = content
            response_content = localData.get(sn)
            if response_content is None:
                response_content = ERR_KEY
            ## send response back
            reponseMsg = encode(rt, ri, st, si, GET_RESPONSE_TITLE, \
                                EMPTY_CONTENT)

        elif title == DELETE_REQUEST_TITLE:
            ## update locallog
            sn = content
            deletelog = LOG_FORMAT % (PUT, sn, bool2str(False))
            writeLogs.append((accept_stamp, serverID, deletelog))
            accept_stamp += 1
            ## update local datastore
            localData.pop(sn, None)
            ## send ack back
            deleteAckMsg = encode(rt, ri, st, si, DELETE_ACK_TITLE,\
                                  EMPTY_CONTENT)
            port = getPortByMsg(deleteAckMsg)
            send(localhost, port, deleteAckMsg, logHeader)

        elif title == EXIT_TITLE:
            conn.close() 
            s.close()
            print "Exit."
            return
        else:
            pass
        conn.close() 

if __name__ == '__main__':
    ## TODO: process cmd arguments and give it to main
    serverID = int(sys.argv[1])
    origin_out = sys.stdout
    origin_err = sys.stderr
    logFile = open (SERVER_LOG_FILENAME % serverID, 'w+', 0)
    sys.stdout = logFile
    sys.stderr = logFile
    main(sys.argv[1:])
    print "END OF ROUTINE"
    logFile.close()
    sys.stdout = origin_out 
    sys.stderr = origin_err
