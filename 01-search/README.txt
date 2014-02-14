************************************************************************
*** TITLE: CS343H Artificial Intelligence Project 1
*** AUTHOR: Jimmy Lin (JIMMYLIN@UTEXAS.EDU)
*** EID: XL5224
***     
************************************************************************
Description of this project:
q1: Implement the Depth First Search algorithm: using stack
    Note that there is also an recusive implementation for DFS but we did not
    choose to do it.

q2: Implement the Breadth First Search algorithm: using stack

q3: Implement the Uniform Cost Search aglorithm: using priority queue

q4: Implement A star graph search algorithm: using priority queue

q5: Define the CornerProblem

q6: Corner heuristic function: Brute-force algorithm
    We try all possible permuations of corner fruits and use the
    length of the shortest path as heuristic function
 
q7: Heuristic function for eating all fruits: 

    We use mixture of brute-force algorithm and Prim algorithm.
      - For a problem with small size, brute-force algorithm is employed to
        secure admissibility and consistency.
      - For a problem with large size, prim algorithm is utilized to find the
        minimal spanning tree(MST). And we regard the total length of MST as
        heuristic. 
      - The drawing line for big and small problem is whether the number of
        fruits is bigger than five.

q8: Suboptimal solution
    
===========
Starting on 2-2 at 13:48:45

Question q1
===========

*** PASS: test_cases/q1/graph_bfs_vs_dfs.test
*** 	solution:		['2', '0']
*** 	expanded_states:	['A', 'D']
*** PASS: test_cases/q1/graph_infinite.test
*** 	solution:		['0', '1', '1']
*** 	expanded_states:	['A', 'B', 'C']
*** PASS: test_cases/q1/pacman_1.test
*** 	pacman layout:		mediumMaze
*** 	solution length: 130
*** 	nodes expanded:		146

### Question q1: 2/2 ###


Question q2
===========

*** PASS: test_cases/q2/graph_bfs_vs_dfs.test
*** 	solution:		['1']
*** 	expanded_states:	['A', 'B']
*** PASS: test_cases/q2/graph_infinite.test
*** 	solution:		['0', '1', '1']
*** 	expanded_states:	['A', 'B', 'C']
*** PASS: test_cases/q2/pacman_1.test
*** 	pacman layout:		mediumMaze
*** 	solution length: 68
*** 	nodes expanded:		269

### Question q2: 2/2 ###


Question q3
===========

*** PASS: test_cases/q3/graph_bfs_vs_dfs.test
*** 	solution:		['1']
*** 	expanded_states:	['A', 'B']
*** PASS: test_cases/q3/graph_infinite.test
*** 	solution:		['0', '1', '1']
*** 	expanded_states:	['A', 'B', 'C']
*** PASS: test_cases/q3/ucs_0_graph.test
*** 	solution:		['Right', 'Down', 'Down']
*** 	expanded_states:	['A', 'B', 'D', 'C', 'G']
*** PASS: test_cases/q3/ucs_1_problemC.test
*** 	pacman layout:		mediumMaze
*** 	solution length: 68
*** 	nodes expanded:		269
*** PASS: test_cases/q3/ucs_2_problemE.test
*** 	pacman layout:		mediumMaze
*** 	solution length: 74
*** 	nodes expanded:		260
*** PASS: test_cases/q3/ucs_3_problemW.test
*** 	pacman layout:		mediumMaze
*** 	solution length: 152
*** 	nodes expanded:		173
*** PASS: test_cases/q3/ucs_4_testSearch.test
*** 	pacman layout:		testSearch
*** 	solution length: 7
*** 	nodes expanded:		14
*** PASS: test_cases/q3/ucs_5_goalAtDequeue.test
*** 	solution:		['West', 'West', 'West']
*** 	expanded_states:	['A', 'B', 'C']

### Question q3: 2/2 ###


Question q4
===========

*** PASS: test_cases/q4/astar_0.test
*** 	solution:		['Right', 'Down', 'Down']
*** 	expanded_states:	['A', 'B', 'D', 'C', 'G']
*** PASS: test_cases/q4/astar_1_graph_heuristic.test
*** 	solution:		['0', '0', '2']
*** 	expanded_states:	['S', 'A', 'D', 'C']
*** PASS: test_cases/q4/astar_2_manhattan.test
*** 	pacman layout:		mediumMaze
*** 	solution length: 68
*** 	nodes expanded:		221
*** PASS: test_cases/q4/astar_3_goalAtDequeue.test
*** 	solution:		['West', 'West', 'West']
*** 	expanded_states:	['A', 'B', 'C']

### Question q4: 3/3 ###


Question q5
===========

*** PASS: test_cases/q5/corner_tiny_corner.test
*** 	pacman layout:		tinyCorner
*** 	solution length:		28

### Question q5: 2/2 ###


Question q6
===========

*** PASS: heuristic value less than true cost at start state
*** PASS: heuristic value less than true cost at start state
*** PASS: heuristic value less than true cost at start state
path: ['North', 'East', 'East', 'East', 'East', 'North', 'North', 'West', 'West', 'West', 'West', 'North', 'North', 'North', 'North', 'North', 'North', 'North', 'North', 'West', 'West', 'West', 'West', 'South', 'South', 'East', 'East', 'East', 'East', 'South', 'South', 'South', 'South', 'South', 'South', 'West', 'West', 'South', 'South', 'South', 'West', 'West', 'North', 'East', 'East', 'North', 'North', 'East', 'East', 'East', 'East', 'East', 'East', 'East', 'East', 'South', 'South', 'East', 'East', 'East', 'East', 'East', 'North', 'North', 'East', 'East', 'North', 'North', 'East', 'East', 'North', 'North', 'East', 'East', 'East', 'East', 'South', 'South', 'South', 'South', 'East', 'East', 'North', 'North', 'East', 'East', 'South', 'South', 'South', 'South', 'South', 'North', 'North', 'North', 'North', 'North', 'North', 'North', 'West', 'West', 'North', 'North', 'East', 'East', 'North', 'North']
path length: 106
*** PASS: Heuristic resulted in expansion of 189 nodes

### Question q6: 3/3 ###


Question q7
===========

*** PASS: test_cases/q7/food_heuristic_1.test
*** PASS: test_cases/q7/food_heuristic_10.test
*** PASS: test_cases/q7/food_heuristic_11.test
*** PASS: test_cases/q7/food_heuristic_12.test
*** PASS: test_cases/q7/food_heuristic_13.test
*** PASS: test_cases/q7/food_heuristic_14.test
*** PASS: test_cases/q7/food_heuristic_15.test
*** PASS: test_cases/q7/food_heuristic_16.test
*** PASS: test_cases/q7/food_heuristic_17.test
*** PASS: test_cases/q7/food_heuristic_2.test
*** PASS: test_cases/q7/food_heuristic_3.test
*** PASS: test_cases/q7/food_heuristic_4.test
*** PASS: test_cases/q7/food_heuristic_5.test
*** PASS: test_cases/q7/food_heuristic_6.test
*** PASS: test_cases/q7/food_heuristic_7.test
*** PASS: test_cases/q7/food_heuristic_8.test
*** PASS: test_cases/q7/food_heuristic_9.test
*** PASS: test_cases/q7/food_heuristic_grade_tricky.test
*** 	expanded nodes: 410
*** 	thresholds: [15000, 12000, 9000, 7000]

### Question q7: 5/4 ###


Question q8
===========

[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_1.test
*** 	pacman layout:		Test 1
*** 	solution length:		1
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_10.test
*** 	pacman layout:		Test 10
*** 	solution length:		1
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_11.test
*** 	pacman layout:		Test 11
*** 	solution length:		2
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_12.test
*** 	pacman layout:		Test 12
*** 	solution length:		3
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_13.test
*** 	pacman layout:		Test 13
*** 	solution length:		1
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_2.test
*** 	pacman layout:		Test 2
*** 	solution length:		1
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_3.test
*** 	pacman layout:		Test 3
*** 	solution length:		1
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_4.test
*** 	pacman layout:		Test 4
*** 	solution length:		3
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_5.test
*** 	pacman layout:		Test 5
*** 	solution length:		1
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_6.test
*** 	pacman layout:		Test 6
*** 	solution length:		2
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_7.test
*** 	pacman layout:		Test 7
*** 	solution length:		1
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_8.test
*** 	pacman layout:		Test 8
*** 	solution length:		1
[SearchAgent] using function depthFirstSearch
[SearchAgent] using problem type PositionSearchProblem
*** PASS: test_cases/q8/closest_dot_9.test
*** 	pacman layout:		Test 9
*** 	solution length:		1

### Question q8: 2/2 ###


Question extra
==============

*** Method not implemented: getAction at line 751 of searchAgents.py
*** FAIL: test_cases/extra/extra.test
*** 	Extra credit threw an exception: global name 'TimeoutFunctionException' is not defined.
Traceback (most recent call last):
  File "searchTestClasses.py", line 859, in execute
    except TimeoutFunctionException:
NameError: global name 'TimeoutFunctionException' is not defined

*** Tests failed.

### Question extra: 0/0 ###


Finished at 13:49:16

Provisional grades
==================
Question q1: 2/2
Question q2: 2/2
Question q3: 2/2
Question q4: 3/3
Question q5: 2/2
Question q6: 3/3
Question q7: 5/4
Question q8: 2/2
Question extra: 0/0
------------------
Total: 21/20

Your assignment has NOT been turned in, you must still submit your files via turnin, 
using the instructions on the assignment page.  If you turn in your code in the current 
state the grades assigned by this autograder will be a lower bound on your actual grade, 
unless your submission was not in the spirit of the course, such as if your submission 
simply hardcoded the answers to the tests, or in the case of academic dishonesty.  We 
will screen for this after the deadline.

Additionally, your final grade may be higher than that assigned by the autograder, if 
your incorrect solutions are deemed to warrant partial credit.

