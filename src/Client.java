/*##############################################################
## MODULE: Client.java
## VERSION: 1.0 
## SINCE: 2014-03-30
## AUTHOR: 
##     JIMMY LIN (xl5224) - JimmyLin@utexas.edu  
##     
## DESCRIPTION: 
##      
#################################################################
## Edited by MacVim
## Class Info auto-generated by Snippet 
################################################################*/
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.PrintStream;
import java.io.File;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.InetAddress;

public class Client extends Util {
    static String logHeader;
    static String logfilename;

    /* Knowledge of global scenario */
    static int clientID;
    static int numServers;

    static InetAddress localhost;

    /* sequence number */
    static int cid; // client-local command sequence number

    public static void main (String [] args) throws IOException {
        // parse the given id
        clientID = Integer.parseInt(args[0]);
        numServers = Integer.parseInt(args[1]);
        // configure the LOG setting
        logHeader = String.format(CLIENT_LOG_HEADER, clientID);
        logfilename = String.format(CLIENT_LOG_FILENAME, clientID);
        PrintStream log = new PrintStream (new File(logfilename));
        // redirect output to specified file
        System.setOut(log);
        System.setErr(log);

        localhost = InetAddress.getLocalHost();
        // construct stable server socket
        ServerSocket listener = new ServerSocket(CLIENT_PORT_BASE+clientID, 0,
                InetAddress.getLocalHost());
        listener.setReuseAddress(true);
        // send acknowledgement to the master
        String setup_ack = String.format(MESSAGE, CLIENT_TYPE, clientID,
                MASTER_TYPE, 0, START_ACK_TITLE, EMPTY_CONTENT);
        send (localhost, MASTER_PORT, setup_ack, logHeader);
        // indicate the listener setup
        System.out.println(logHeader + listener.toString());
        int port;
        try { while (true) {
            Socket socket = listener.accept();
            try {
                BufferedReader in = new BufferedReader(new
                        InputStreamReader(socket.getInputStream()));
                // CHANNEL IS ESTABLISHED
                String recMessage = in.readLine();
                System.out.println(logHeader + "Message Received: " + recMessage);
                String [] recInfo = recMessage.split(",");

                String sender_type = recInfo[SENDER_TYPE_IDX];
                int sender_idx = Integer.parseInt(recInfo[SENDER_INDEX_IDX]);
                String receiver_type = recInfo[RECEIVER_TYPE_IDX];
                int receiver_idx = Integer.parseInt(recInfo[RECEIVER_INDEX_IDX]);
                String title = recInfo[TITLE_IDX];
                String content = recInfo[CONTENT_IDX];

                if (title.equals(SEND_MESSAGE_TITLE)) {
                    // send chat message (command) to all servers
                    String command = String.format(COMMAND, clientID, cid, content);
                    for (int serverIndex = 0; serverIndex < numServers; serverIndex++) {
                        String request = String.format(MESSAGE, CLIENT_TYPE,
                            clientID, SERVER_TYPE, serverIndex,
                                REQUEST_TITLE, command);
                        port = SERVER_PORT_BASE + serverIndex;
                        send (localhost, port, request, logHeader);
                    }
                }
                else if (title.equals(RESPONSE_TITLE)) {
                    // TODO: confirm that the message is accepted by servers
                    ;

                } else if (title.equals(EXIT_TITLE)) {
                    socket.close();
                    listener.close();
                    System.out.println(logHeader + "Exit.");
                    System.exit(0);
                   }
                } finally {
                    socket.close();
                }

            }
        } finally {
            listener.close();
        }
    }
}
