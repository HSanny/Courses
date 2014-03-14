************************************************************************
*** NAME: XIN LIN (100%)
*** EID: xl5224
*** CS343H Artificial Intelligence Project #2 
************************************************************************

Q1: [DONE] Naive evaluation function
Things to consider while defining the evaluation function:
    a. Ghost killing mechanism. To chase the ghost when it is scared.
    b. Safety mechanism. Keep away from ghosts when it is far from that. 
    c. Food eating mechanism. Pursue the food when ghost is not nearby.  
    
Q2: [DONE] Minimax Algorithm
Recursive calls are used to achieve *minimax algorithm*. Root node, immediate
nodes, and leaf nodes are distinguished. 

Note that I misunderstand the definition of "ply", for which plenty of time is
wasted on debugging. 

Q3: [DONE] Alpha-Beta Pruning
Slight modification based on the framwork of minimax algorithm to achieve
alpha-beta pruning. One thing troubles me for a long time is how to inherit
the alpha and beta from parent game state to the child. Later it is solved by
updating alpha and beta value when it is swithed to traverse another branch of
a given parent node.

Q4: [DONE] Expectimax Algorithm
Implementation shares the same framework as Q2's. The only difference exists
in the opt operation. For the chance node, we use expectation rather than
original min for MIN player.

Q5: [DONE] Heuristic Function over game state 
To evaluate the heuristic function over game state of pacman game, there are
three elements we are supposed to consider:
    a. the desire of pacman to keep it safe (away from ghost)
    b. the desire of pacman to devour the scared ghost
    c. the desire of eating food
    d. the desire of avoiding wasting time

    for a, we introduce safety mechanism with highest priority (desire = 1000)
    for b, we introduce mechanism of approaching ghost with secondary priority
    (100, should be this value since the score of devouring a ghost is about
    300.)
    for c, we directly use the provided getScore() function
    for d, we employ random.random() to avoid DIRECTIONS.STOP.

Mini-Contest:
    The overall strategy of designing a good pacman agent can be seen as the
    following three stages: 
        a. defend pacman itself
        b. eat fruits
        c. attack ghost at scared time
    
