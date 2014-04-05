/*##############################################################
## MODULE: Util.java
## VERSION: 1.0 
## SINCE: 2014-03-31
## AUTHOR: 
##     JIMMY LIN (xl5224) - JimmyLin@utexas.edu  
##     CALVIN SZETO - Szeto.calvin@gmail.com
## DESCRIPTION: 
##      
#################################################################
## Edited by MacVim
## Class Info auto-generated by Snippet 
################################################################*/

import java.io.IOException;
import java.net.Socket;
import java.net.InetAddress;
import java.io.PrintWriter;

class Util implements Protocol, Logging {
    final static boolean PrintingSwitch = true;
    /* get client port given the clientIndex */
    public static int getClientPort (int clientIndex) { return 0; }
    public static int getServerPort (int clientIndex) { return 0; }

    /* Print general stuff */
    public static void print(String toprint, String logHeader) {
        String str = "";
        str += logHeader + " " + toprint;
        if (PrintingSwitch)
            System.out.println(str);
    }

    /* Print the received message */
    public static void printReceivedMessage (String recMessage, String logHeader) {
        String [] recParts = recMessage.split(MESSAGE_SEP);
        String sender_type = recParts[SENDER_TYPE_IDX];
        int sender_index = Integer.parseInt(recParts[SENDER_INDEX_IDX]);
        String title = recParts[TITLE_IDX];
        String content = recParts[CONTENT_IDX];

        String str = "";
        str += logHeader + "|Receive *" + title + "* from ";
        str += "{" + sender_type + " #" + sender_index + "}: ";
        str += content;

        if (PrintingSwitch)
            System.out.println(str);
    }

    /* Print the sent message */
    public static void printSentMessage (String sentMessage, String logHeader) {
        String [] sentParts = sentMessage.split(MESSAGE_SEP);
        String receiver_type = sentParts[RECEIVER_TYPE_IDX];
        int receiver_idx = Integer.parseInt(sentParts[RECEIVER_INDEX_IDX]);
        String title = sentParts[TITLE_IDX];
        String content = sentParts[CONTENT_IDX];

        String str = "";
        str += logHeader + "Sent *" + title + "* to ";
        str += "{" + receiver_type + " #" + receiver_idx + "}: ";
        str += content;

        if (PrintingSwitch)
            System.out.println(str);
    }

    /* Send util functions */
    public static void send (InetAddress host, int port, String message, 
            String HEADER) throws IOException { 
        // set null header as empty string
        if (HEADER == null) HEADER = "";
        // establish temporary socket connection
        Socket socket = new Socket (host, port);
        try {
            PrintWriter out =
                new PrintWriter(socket.getOutputStream(), true);
            out.println(message);
            printSentMessage(message, HEADER);
        } finally {
            socket.close();
        }
    }

    /* Send all messages */
    public static void sendAll (InetAddress host, int port, String [] messages,
            String HEADER) throws IOException {
        // set null header as empty string
        if (HEADER == null) HEADER = "";
        // establish temporary socket connection
        Socket socket = new Socket(host, port);
        try {
            PrintWriter out =
                new PrintWriter(socket.getOutputStream(), true);
            for (int i = 0; i < messages.length; i ++) {
                out.println(messages[i]);
                printSentMessage(messages[i], HEADER);
            }
        } finally {
            socket.close();
        }
    }
}
