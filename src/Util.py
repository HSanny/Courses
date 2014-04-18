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
import Protocol as P

printSwitch = True

def encode (st, si, rt, ri, title, content):
    '''
    API: encode the message by components
    '''
    msg = P.MESSAGE % (st, si, rt, ri, title, content)
    return msg

def decode (msg):
    ''' 
    API: Decode the message to various components
    '''
    components = msg.split(P.MESSAGE_SEP)
    sender_type = components[P.SENDER_TYPE_IDX];
    sender_index = int(components[P.SENDER_INDEX_IDX])
    receiver_type = components[P.RECEIVER_TYPE_IDX]
    receiver_idx = int(components[P.RECEIVER_INDEX_IDX])
    title = components[P.TITLE_IDX]
    content = components[P.CONTENT_IDX]
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
        components = sentMessage.split(P.MESSAGE_SEP)
        sender_type = components[P.SENDER_TYPE_IDX];
        sender_index = components[P.SENDER_INDEX_IDX];
        title = components[P.TITLE_IDX];
        content = components[P.CONTENT_IDX];

        string = "";
        string += logHeader + "|| Receive *" + title + "* from ";
        string += "{" + sender_type + " #" + sender_index + "}: ";
        string += content
        print string
    return 

def printRecvMessage (recvMessage, logHeader):
    '''
    API: send message
    '''
    if printSwitch:
        components = recvMessage.split(P.MESSAGE_SEP)
        sender_type = components[P.SENDER_TYPE_IDX];
        sender_index = components[P.SENDER_INDEX_IDX];
        title = components[P.TITLE_IDX];
        content = components[P.CONTENT_IDX];

        string = "";
        string += logHeader + "|| Receive *" + title + "* from ";
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
    s.send(message)
    printSentMessage(message, Header)
    s.close()
    return True

def broadcast (host, sampleMsg, Header, allServers, allClients):
    '''
    API: broadcast message with $title$ to all given servers and clients
    with empty content
    '''
    st, si, _, _, title, content = decode (sampleMsg)
    for serverIdx in allServers:
        msg = encode (st, si, SERVER_TYPE, serverIdx, title, content)
        port = P.SERVER_PORT_BASE + serverIdx
        send (host, port, msg, Header)
    for clientIdx in allClients:
        msg = encode (st, si, CLIENT_TYPE, clientIdx, title, content)
        port = P.CLIENT_PORT_BASE + clientIdx
        send (host, port, msg, Header)
    return True
    
def checkCounterAllTrue (boolCounter, NoneIgnore=True):
    '''
    API: determine whether the input array contains all boolean value
    '''
    decision = True
    for idx, bvalue in boolArray.items():
        if bvalue == None and !NoneIgnore:
            decision = False
            break
        if !bvalue:
            decision = False
            break
    return decision

def initAllFalseCounter (allIndex):
    '''
    API: determine
    '''
    counter = {}
    for index in allIndex:
        counter.update({index:False})
    return counter

def getPortByMsg (msg):
    _, _, receiver_type, receiver_idx, _, _ = decode(msg)
    [base, port] = [-1, -1]
    if receiver_type == P.MASTER_TYPE:
        return P.MASTER_PORT
    elif receiver_type == P.SERVER_TYPE:
        base = P.SERVER_PORT_BASE
    elif receiver_type == P.CLIENT_TYPE:
        base = P.CLIENT_PORT_BASE
    port = base + receiver_idx
    return port
