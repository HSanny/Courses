/*##############################################################
## MODULE: Logging.java
## VERSION: 1.0 
## SINCE: 2014-04-01
## AUTHOR: 
##     JIMMY LIN (xl5224) - JimmyLin@utexas.edu  
##     CALVIN SZETO - Szeto.calvin@gmail.com
## DESCRIPTION: 
##      
#################################################################
## Edited by MacVim
## Class Info auto-generated by Snippet 
################################################################*/

interface Logging {
    /* LOG HEADERS */
    final static String MASTER_LOG_HEADER = "[MASTER] ";
    final static String SERVER_LOG_HEADER = "[SERVER #%d] ";
    final static String CLIENT_LOG_HEADER = "[CLIENT #%d] ";
    final static String ACCEPTOR_LOG_HEADER = "[ACCEPTOR #%d] ";
    final static String LEADER_LOG_HEADER = "[LEADER #%d] ";
    final static String COMMANDER_LOG_HEADER = "[COMMANDER #%d] ";
    final static String SCOUT_LOG_HEADER = "[SCOUT #%d] ";

    /* LOG FILENAME */
    final static String CLIENT_LOG_FILENAME = "logs/Client%d.txt";
    final static String SERVER_LOG_FILENAME = "logs/Server%d.txt";


}
