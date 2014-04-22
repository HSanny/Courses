############################################################
##    FILENAME:   Util.py
##    VERSION:    1.0
##    SINCE:      2014-04-15
##    AUTHOR:
##        Jimmy Lin (xl5224) - JimmyLin@utexas.edu
##
############################################################
##    Edited by MacVim
##    Documentation auto-generated by Snippet
############################################################

import socket
from Protocol import *
import threading

printSwitch = True

LOAD_SERVER_CMD = "python -u src/Server.py"
LOAD_CLIENT_CMD = "python -u src/Client.py"

def encode (st, si, rt, ri, title, content):
    '''
    API: encode the message by components
    '''
    msg = MESSAGE % (st, si, rt, ri, title, content)
    return msg

def decode (msg):
    '''
    API: Decode the message to various components
    '''
    components = msg.split(MESSAGE_SEP)
    sender_type = components[SENDER_TYPE_IDX];
    sender_index = int(components[SENDER_INDEX_IDX])
    receiver_type = components[RECEIVER_TYPE_IDX]
    receiver_idx = int(components[RECEIVER_INDEX_IDX])
    title = components[TITLE_IDX]
    content = components[CONTENT_IDX]
    return sender_type, sender_index, receiver_type, receiver_idx,\
            title, content

def printLog (toprint, logHeader):
    '''
    API: print log
    '''
    string = ""
    string += logHeader + " " + toprint
    if printSwitch:
        print string
    return

def printSentMessage (sentMessage, logHeader):
    '''
    API: send message
    '''
    if printSwitch:
        components = sentMessage.split(MESSAGE_SEP)
        receiver_type = components[RECEIVER_TYPE_IDX]
        receiver_idx = components[RECEIVER_INDEX_IDX]
        title = components[TITLE_IDX];
        content = components[CONTENT_IDX];

        string = "";
        string += logHeader + "send *" + title + "* to ";
        string += "{" + receiver_type + " #" + receiver_idx + "}: ";
        string += content
        print string
    return

def printRecvMessage (recvMessage, logHeader):
    '''
    API: send message
    '''
    if printSwitch:
        components = recvMessage.split(MESSAGE_SEP)
        sender_type = components[SENDER_TYPE_IDX];
        sender_index = components[SENDER_INDEX_IDX];
        title = components[TITLE_IDX];
        content = components[CONTENT_IDX];

        string = "";
        string += logHeader + "|| Receive *" + title + "* from ";
        string += "{" + sender_type + " #" + sender_index + "}: ";
        string += content
        print string
    return

def printCachedMessage (recvMessage, logHeader, cached=False):
    '''
    API: send message
    '''
    if printSwitch:
        components = recvMessage.split(MESSAGE_SEP)
        sender_type = components[SENDER_TYPE_IDX];
        sender_index = components[SENDER_INDEX_IDX];
        title = components[TITLE_IDX];
        content = components[CONTENT_IDX];

        string = "";
        if cached:
            string += logHeader + ">> Cached *" + title + "* from ";
        else:
            string += logHeader + ">> Uncached *" + title + "* from ";
        string += "{" + sender_type + " #" + sender_index + "}: ";
        string += content
        print string
    return

def send (host, port, message, Header):
    '''
    API: send message
    '''
    s = socket.socket()
    s.connect((host, port))
    printSentMessage(message, Header)
    s.send(message)
    s.close()
    return True

def broadcastServers (host, sampleMsg, Header, allServers):
    '''
    API: broadcast message with $title$ to all given servers
    with empty content
    '''
    st, si, _, _, title, content = decode (sampleMsg)

    #print allServers
    for serverIdx in allServers:
        msg = encode (st, si, SERVER_TYPE, serverIdx, title, content)
        port = getPortByMsg (msg)
        send (host, port, msg, Header)
    return True

def broadcast (host, sampleMsg, Header, allServers, allClients):
    '''
    API: broadcast message with $title$ to all given servers and clients
    with empty content
    '''
    st, si, _, _, title, content = decode (sampleMsg)

    #print allServers
    for serverIdx in allServers:
        msg = encode (st, si, SERVER_TYPE, serverIdx, title, content)
        port = getPortByMsg (msg)
        send (host, port, msg, Header)

    #print allClients
    for clientIdx in allClients:
        msg = encode (st, si, CLIENT_TYPE, clientIdx, title, content)
        port = getPortByMsg (msg)
        send (host, port, msg, Header)

    return True

def checkCounterAllTrue (boolCounter, NoneIgnore=True):
    '''
    API: determine whether the input counter contains all True value
    '''
    assert (boolCounter is not None)
    decision = True
    for idx, bvalue in boolCounter.items():
        if bvalue == None and not NoneIgnore:
            decision = False
            break
        if not bvalue:
            decision = False
            break
    return decision

def checkCounterAllNotNone (boolCounter):
    '''
    API: determine whether the input counter contains all not NOne
    '''
    assert (boolCounter is not None)
    for idx, bvalue in boolCounter.items():
        if bvalue is None:
            return False
    return True

def initAllFalseCounter (allIndex):
    '''
    API: determine
    '''
    counter = {}
    for index in allIndex:
        counter.update({index:False})
    return counter

def initAllNoneCounter (allIndex):
    '''
    API: determine
    '''
    counter = {}
    for index in allIndex:
        counter.update({index:None})
    return counter


def initVersionVector (allServers=None):
    if allServers is None:
        return {}
    else:
        result = {}
        for sIdx in allServers:
            result.update({sIdx: 0})
        return result

def initWriteLogs ():
    return list([])

def initCachedMessages():
    return list([])

def initEmptySemaphore ():
    return threading.Semaphore(0)

def getPortByMsg (msg):
    _, _, receiver_type, receiver_idx, _, _ = decode(msg)
    [base, port] = [-1, -1]
    if receiver_type == MASTER_TYPE:
        return MASTER_PORT
    elif receiver_type == SERVER_TYPE:
        base = SERVER_PORT_BASE
    elif receiver_type == CLIENT_TYPE:
        base = CLIENT_PORT_BASE
    port = base + receiver_idx
    return port

def set2str (inSet):
    if len(inSet) == 0:
        return 'None'
    outString = ",".join(str(x) for x in inSet)
    return outString

def str2set (inString):
    if inString == 'None' or len(inString) == 0:
        return set([])
    outSet = set([int(x) for x in inString.split(",")])
    return outSet

def args2cmd (args):
    cmd = ""
    for arg in args:
        cmd += arg + " "
    return cmd

def bool2str (bvalue):
    if bvalue is None:
        return "NONE"
    elif bvalue:
        return "TRUE"
    elif not bvalue:
        return "FALSE"

def str2bool (bstr):
    if bstr is None or not bstr:
        return False
    else: return True

def vv2str (versionVector):
    return str(versionVector)

def str2vv (vvStr):
    return eval(vvStr)

INFINITY = float('inf')

def isInf (num):
    return num == INFINITY
            
