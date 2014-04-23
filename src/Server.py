############################################################
##    FILENAME:   Server.py
##    VERSION:    1.4
##    SINCE:      2014-04-15
##    AUTHOR:
##        Jimmy Lin (xl5224) - JimmyLin@utexas.edu
##
############################################################
##    Edited by MacVim
##    Documentation auto-generated by Snippet
############################################################

import sys, socket
import json
from Protocol import *
from Logging import *
from Util import *

## TODO: static variable here
localhost = socket.gethostname()

def applyWrite (localData, write, writeLogs):
    (log_stamp, sid, csn, oplog) = write
    [op_type, op_value, stable_bool] = oplog.split(OPLOG_SEP)
    if op_type == PUT:
        [sn, URL] = op_value.strip("()").split(OP_VALUE_SEP)
        URL = URL.strip(" ")
        if localData.has_key(URL):
            ## get sequence number of the write leading to existing value
            maxwcsn = -1
            for ls, s, wcsn, opl in writeLogs:
                [oplt, oplv, oplsb] = opl.split(OPLOG_SEP)
                [wsn, _] = oplv.strip("()").split(OP_VALUE_SEP)
                if wsn == sn and wcsn > maxwcsn and oplsb == "TRUE":
                    maxwcsn = wcsn

            if maxwcsn < csn:
                localData.update({sn:URL})
            ## otherwise we do not update 
        else:
            localData.update({sn:URL})
    elif op_type == DELETE:
        sn = op_value.strip("()")
        localData.pop(sn, None)
    return

"""
def anti_entropy (Senderid, Reiceiverid):
    '''
    Anti_entropy algorithm by fig. 1 of FUG paper
    '''
    global logHeader
    ## send request to ask for receiver's version vector
    vvRequestMsg = encode(SERVER_TYPE, Senderid, SERVER_TYPE, \
                Reiceiverid, VERSION_VECTOR_REQUEST_TITLE, EMPTY_CONTENT)
    port = getPortByMsg(vvRequestMsg)
    send(localhost, port, vvRequestMsg, logHeader)
    ## block until response
    global vvResponseSema, vvResponseContent
    vvResponseSema = initEmptySemaphore()
    vvResponseSema.acquire()
    RVersionVector = str2vv(vvResponseContent)

    ## for every write-log of Sender
    for accept_stamp, sid, oplog in writeLogs:
        wStr = W_FORMAT % (accept_stamp, sid, oplog)
        if RVersionVector.get(sid) < accept_stamp:
            # this oplog is new for Receiver
            # send write to receiver
            sendWriteMsg = encode(SERVER_TYPE, Senderid, SERVER_TYPE, \
                   Reiceiverid, SEND_WRITE_TITLE, wStr)
            port = getPortByMsg(sendWriteMsg)
            send(localhost, port, sendWriteMsg, logHeader)
    return 
"""

def exchange_info (writeLogs, RVV, SCSN, RCSN, SID, RID):
    ## for every write-log of Sender
    global logHeader
    ## exchange commited write owned by serverID
    print RCSN, SCSN
    if RCSN < SCSN:
        for log_stamp, sid, csn, oplog in writeLogs:
            if not isInf(csn) and csn > RCSN:
                # this write is commited but R is not unknown
                wStr = W_FORMAT % (log_stamp, sid, csn, oplog)
                if log_stamp <= RVV.get(sid):
                    # R has the tentative write but has not commit
                    commitNotifyMsg = encode(SERVER_TYPE, SID, \
                         SERVER_TYPE, RID, COMMIT_NOTIFICATION_TITLE, wStr)
                    port = getPortByMsg(commitNotifyMsg)
                    send(localhost, port, commitNotifyMsg, logHeader)
                else:
                    # R does not even know the tentative write
                    sendWriteMsg = encode(SERVER_TYPE, SID, SERVER_TYPE, \
                              RID, SEND_WRITE_TITLE, wStr)
                    port = getPortByMsg(sendWriteMsg)
                    send(localhost, port, sendWriteMsg, logHeader)

    ## exchange tentative write owned by server SID
    for accept_stamp, sid, csn, oplog in writeLogs:
        if not isInf(csn):
            continue
        wStr = W_FORMAT % (accept_stamp, sid, csn, oplog)
        if RVV.get(sid) < accept_stamp:
            # this oplog is new for Receiver
            # send write to receiver
            sendWriteMsg = encode(SERVER_TYPE, SID, SERVER_TYPE, \
                   RID, SEND_WRITE_TITLE, wStr)
            port = getPortByMsg(sendWriteMsg)
            send(localhost, port, sendWriteMsg, logHeader)
    return 

def main (argv):
    """
    Main function of server
    """
    assert len(argv) >= 2, "SERVER: too less arguments"

    ## initialize static variables
    pause = False
    # serverID is the ID as given by Master and used for communication
    serverID = int(argv[0])
    # bayouID is the ID regarding the Bayou protocol
    bayouID = None
    # TODO: Remove uses of allServers except to find a server for CREATE/RETIRE
    allServers = str2set(argv[1])
    isPrimary = (argv[2] == PRIMARY_PETITION)
    global logHeader
    logHeader = SERVER_LOG_HEADER % serverID

    writeLogs = initWriteLogs()
    versionVector = initVersionVector()
    bayouToMaster = {}
    localData = {}
    CSN = -1

    accept_stamp = 0
    stableRound = -1
    global cachedMessages
    cachedMessages = []

    ## construct server socket
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    port = SERVER_PORT_BASE + serverID
    s.bind((localhost, port))

    # TODO: Browse through all uses of Master ID and change to Bayou ID as necessary
    firstRun = True
    s.listen(5)
    while True:
        if firstRun:
            if isPrimary:
                bayouID = serverID 
                bayouToMaster[bayouID] = serverID
                ackMsg = encode(SERVER_TYPE, serverID, MASTER_TYPE, 0,
                    JOIN_SERVER_ACK_TITLE, serverID)
                send(localhost, MASTER_PORT, ackMsg, logHeader)
            else:
                # Act as client and send CREATE message to a single server
                lowest_server = min(allServers)
                createMsg = encode(SERVER_TYPE, serverID, SERVER_TYPE,
                    lowest_server, PUT_REQUEST_TITLE, CREATE_CONTENT)
                port = getPortByMsg(createMsg)
                send(localhost, port, createMsg, logHeader)
            firstRun = False
                

        ## prioritize uncached message if system is non-paused
        if not pause and len(cachedMessages) > 0:
            ## uncached the cached messages
            recvMsg = cachedMessages.pop(0)
        else:
            conn, addr = s.accept()
            recvMsg = conn.recv(BUFFER_SIZE)

        '''Incoming message preprocessing'''
        st, si, rt, ri, title, content = decode(recvMsg)

        if pause and title in ANTI_ENTROPY_TITLES:
            printCachedMessage(recvMsg, False)
            cachedMessages.append(recvMsg)

        printRecvMessage(recvMsg, logHeader)

        '''Processing incoming messages'''
        if title == JOIN_SERVER_TITLE:
            # Add a new mapping from bayouID to MasterID
            newBayouID, newServerID = json.loads(content)
            if isinstance(newBayouID, list):
                newBayouID = tuple(newBayouID)
            bayouToMaster[newBayouID] = newServerID
        elif title == RETIRE_SERVER_TITLE:
            ## STEP ONE: exchange local log to at least one server it knows

            ## STEP TWO: notify all servers to forget it

            ## STEP THREE: lots of close

            pass
        elif title == JOIN_CLIENT_ACK_TITLE:
            deliverAckMsg = encode (SERVER_TYPE, serverID, \
               MASTER_TYPE, 0, JOIN_CLIENT_ACK_TITLE, EMPTY_CONTENT)
            port = getPortByMsg(deliverAckMsg)
            send(localhost, port, deliverAckMsg, logHeader)

        elif title == PRINT_LOG_TITLE:
            assert (st == MASTER_TYPE)
            ## generate logstr
            print writeLogs
            opLogs = [oplog for (_, _, _, oplog) in writeLogs]
            print opLogs
            logstr = LOG_SEP.join(opLogs)
            print logstr

            ## send print log response back to master
            logMsg = encode(SERVER_TYPE, serverID, MASTER_TYPE, 0, \
                           PRINT_LOG_RESPONSE_TITLE, logstr)
            send(localhost, MASTER_PORT, logMsg, logHeader)

        elif title == BREAK_CONNECTION_TITLE:
            toBreakServerId = int(content)
            assert (toBreakServerId in allServers)
            allServers.remove(toBreakServerId)
            ## remove that toBreakServerId from its version vector
            assert versionVector.pop(toBreakServerId, None) is not None, \
                "The server to break is unknown."

            ## TODO: send ack stuff?
        elif title == RESTORE_CONNECTION_TITLE:
            toRestoreServerId = int(content)
            assert (toRestoreServerId not in allServers)
            allServers.add(toRestoreServerId)
            ## add toRestoreServerId to versionVector
            versionVector.update({toRestoreServerId:0})
            ## TODO: send ack stuff??

        elif title == PAUSE_TITLE:
            ## switch the pause indicator
            assert not pause, "PAUSE: Cannot pause a paused system."
            pause = True
            ##  cache the following incoming messages
            cachedMessages = initCachedMessages()
            ## send pause acknowledgement to master
            pauseAckMsg = encode(SERVER_TYPE, serverID, MASTER_TYPE, 0, \
                                 PAUSE_ACK_TITLE, EMPTY_CONTENT)
            send (localhost, MASTER_PORT, pauseAckMsg, logHeader)

        elif title == RESTART_TITLE:
            ## switch the pause indicator
            assert pause, "START: Cannot restart a non-paused system."
            pause = False
            ## send restart acknowledgement to master
            restartAckMsg = encode(SERVER_TYPE, serverID, MASTER_TYPE, 0, \
                                  RESTART_ACK_TITLE, EMPTY_CONTENT)
            send(localhost, MASTER_PORT, restartAckMsg, logHeader)

        elif title == PUT_REQUEST_TITLE:
            ## update the accept stamp
            accept_stamp += 1
            if content == CREATE_CONTENT:
                putLog = CREATE_CONTENT
            else:
                ## update locallog
                [sn, URL] = content.split(SU_SEP)
                ## putlog: directly stable if this server is primary 
                putlog = OPLOG_FORMAT % (PUT, OP_VALUE_FORMAT % (sn, URL),\
                                      bool2str(isPrimary))
            if isPrimary:
                CSN += 1
                csn = CSN
            else:
                csn = INFINITY

            write = (accept_stamp, serverID, csn, putlog)
            writeLogs.append(write)
            ## update the version vector
            versionVector.update({serverID:accept_stamp})
            if content == CREATE_CONTENT:
                ## update the version vector
                newServerID = (accept_stamp, bayouID)
                versionVector.update({newServerID:0})
                # TODO: Run anti-entropy with new Server so he 'knows' someone
                putAckMsg = encode(rt, ri, st, si, PUT_ACK_TITLE, json.dumps(newServerID))
            else:
                ## update local datastore
                localData.update({sn:URL})
                putAckMsg = encode(rt, ri, st, si, PUT_ACK_TITLE, EMPTY_CONTENT)
            port = getPortByMsg(putAckMsg)
            send(localhost, port, putAckMsg, logHeader)

        elif title == PUT_ACK_TITLE:
            # Server only receives this in response to his own CREATE
            bayouID = tuple(json.loads(content))
            # Receive Bayou ID and add to version vector
            bayouToMaster[bayouID] = serverID
            # Send Bayou ID to Master
            ackMsg = encode(SERVER_TYPE, serverID, MASTER_TYPE, 0,
                JOIN_SERVER_ACK_TITLE, json.dumps(serverID))
            send(localhost, MASTER_PORT, ackMsg, logHeader)

        elif title == GET_REQUEST_TITLE: ## look up local data store
            sn = content
            response_content = localData.get(sn)
            if response_content is None:
                response_content = sn + SU_SEP + ERR_KEY
            else:
                response_content = sn + SU_SEP + response_content
            ## send response back
            reponseMsg = encode(rt, ri, st, si, GET_RESPONSE_TITLE, \
                                response_content)
            port = getPortByMsg(reponseMsg)
            send(localhost, port, reponseMsg, logHeader)

        elif title == DELETE_REQUEST_TITLE:
            ## update locallog
            sn = content
            if isPrimary:
                CSN += 1
                csn = CSN
            else:
                csn = INFINITY

            deletelog = LOG_FORMAT % (DELETE, sn, csn, bool2str(False))
            writeLogs.append((accept_stamp, serverID, deletelog))
            ## update version vector
            versionVector.update({serverID:accept_stamp})
            ## update the local accept stamp
            accept_stamp += 1
            ## update local datastore
            localData.pop(sn, None)
            ## send ack back
            deleteAckMsg = encode(rt, ri, st, si, DELETE_ACK_TITLE,\
                                  EMPTY_CONTENT)
            port = getPortByMsg(deleteAckMsg)
            send(localhost, port, deleteAckMsg, logHeader)

        elif title == VERSION_VECTOR_REQUEST_TITLE:
            ## convert version vector to string
            vvstr = vv2str(versionVector)
            vvMsg = encode(SERVER_TYPE, serverID, st, si, \
                           VERSION_VECTOR_RESPONSE_TITLE, vvstr)
            port = getPortByMsg(vvMsg)
            send(localhost, port, vvMsg, logHeader)

        elif title == VERSION_VECTOR_RESPONSE_TITLE:
            ## sema up
            global vvResponseSema, vvResponseContent
            vvResponseContent = content
            vvResponseSema.release()

        elif title == SEND_WRITE_TITLE:
            ## decode the content
            [log_stamp, sid, csn, oplog] = content.split(W_SEP)
            [log_stamp, sid, csn, oplog] = [int(log_stamp), int(sid), int(csn), oplog]
            write = (log_stamp, sid, csn, oplog)
            if log_stamp > versionVector.get(sid):
                pass
            else:
                continue
                    
            # commit new write log
            ## primary will commit an instable log to stable
            isPrimaryNewCommit = isPrimary and isInf(csn)
            if isPrimaryNewCommit:
                ## assignment commit sequence number
                CSN += 1
                csn = CSN
                ## set the STABLE_BOOL oplog
                oplog = setStableBool(oplog, True)
                ## apply log to local data
                write = (log_stamp, sid, csn, oplog)
                applyWrite(localData, write, writeLogs)

            ## non-primary receives a commited log
            # apply lots of update because it got a committed log
            isNonPrimaryNewCommit = not isPrimary and not isInf(csn)
            if isNonPrimaryNewCommit:
                ## apply log to local data
                oplog = setStableBool(oplog, True)
                write = (log_stamp, sid, csn, oplog)
                applyWrite(localData, write, writeLogs)
                ## update the CSN
                if csn > CSN:
                    CSN = csn

            ## update the version vector (this is done for all tentative write)
            versionVector.update({sid:log_stamp})
            ## write to log
            ## update the local write-logs
            writeLogs.append(write)
            print writeLogs

        elif title == COMMIT_NOTIFICATION_TITLE:
            ## decode the content
            [log_stamp, sid, csn, oplog] = content.split(W_SEP)
            [log_stamp, sid, csn, oplog] = [int(log_stamp), int(sid), int(csn), oplog]
            ## TODO: find the piece of log to set its STABLE_BOOL
            for l, sx, c, o  in writeLogs:
                if l == log_stamp and sx == sid:
                    writeLogs.remove((l,sx,c,o))
            write = (log_stamp, sid, csn, oplog)
            writeLogs.append(write)
            ## update the commit sequence number
            if csn > CSN:
                CSN = csn
            ## apply update on local data
            applyWrite(localData, write, writeLogs)

        elif title == CHECK_STABILIZATION_REQUEST_TITLE:
            ## stabilization check has been triggered in this server
            if int(content) <= stableRound:
                continue
            ## if this server does not connect to any other servers
            ## send stable decision to master
            if len(allServers) == 0:
                decisionMsg = encode(SERVER_TYPE, serverID, MASTER_TYPE, 0,\
                         CHECK_STABILIZATION_RESPONSE_TITLE, str(True))
                send(localhost, MASTER_PORT, decisionMsg, logHeader)

            ## deliver to all servers it knows about
            for sIdx in allServers - set([si]):
                checkMsg = encode(SERVER_TYPE, serverID, SERVER_TYPE, \
                     sIdx, CHECK_STABILIZATION_REQUEST_TITLE, content)
                port = getPortByMsg(checkMsg)
                send(localhost, port, checkMsg, logHeader)

            global stableCounter
            stableCounter = initAllNoneCounter(allServers)
            ## check stabilization
            for sIdx in allServers:
                ## directly send stable boolean
                stableVVReqMsg = encode(SERVER_TYPE, serverID, SERVER_TYPE, \
                      sIdx, STABLE_VV_REQUEST_TITLE, content)
                port = getPortByMsg(stableVVReqMsg)
                send (localhost, port, stableVVReqMsg, logHeader)
            ## update the local stable round
            stableRound = int(content)

        elif title == STABLE_VV_REQUEST_TITLE:
            vvstr = vv2str(versionVector)
            response = CSN_VV_FORMAT % (CSN, vvstr)
            responseMsg = encode(SERVER_TYPE, serverID, st, si, \
                           STABLE_VV_RESPONSE_TITLE, response)
            port = getPortByMsg(responseMsg)
            send(localhost, port, responseMsg, logHeader)

        elif title == STABLE_VV_RESPONSE_TITLE:
            [RCSN, RVV] = content.split(CSN_VV_SEP)
            [RCSN, RVV] = [int(RCSN), str2vv(RVV)]
            if RVV.get(serverID) < accept_stamp or CSN > RCSN:
                # not up to date
                stableCounter.update({si:False})
                # trigger the update
                exchange_info(writeLogs, RVV, CSN, RCSN, serverID, si)
            elif RVV.get(serverID) == accept_stamp:
                # up to date
                stableCounter.update({si:True})
            # check not all none
            if checkCounterAllNotNone(stableCounter):
                # if all not None, send master the decision
                decision = checkCounterAllTrue(stableCounter, False)
                response = str(CSN) + CHECK_STABLE_RESPONSE_SEP + str(decision)
                decisionMsg = encode(SERVER_TYPE, serverID, MASTER_TYPE, 0,\
                         CHECK_STABILIZATION_RESPONSE_TITLE, response)
                port = getPortByMsg(decisionMsg)
                send(localhost, port, decisionMsg, logHeader)

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
