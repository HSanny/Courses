************************************************************************
*** NAME: XIN LIN
*** EID: xl5224
*** CS343H Artificial Intelligence Project #2 
************************************************************************

Q1: DONE
Things to consider while defining the evaluation function:
    a. Ghost killing mechanism. To chase the ghost when it is scared.
    b. Safety mechanism. Keep away from ghosts when it is far from that. 
    c. Food eating mechanism. Pursue the food when ghost is not nearby.  
    
Q2: DONE
Recursive calls are used to achieve *minimax algorithm*. Root node, immediate
nodes, and leaf nodes are distinguished. 

Note that I misunderstand the definition of "ply", for which plenty of time is
wasted on debugging. 

Q3: 


Q4: DONE
Implementation shares the same framework as Q2's. The only difference exists
in the opt operation. For the chance node, we use expectation rather than
original min for MIN player.

Q5:


Mini-Contest:
