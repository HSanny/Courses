# search.py
# ---------
# Licensing Information: Please do not distribute or publish solutions to this
# project. You are free to use and extend these projects for educational
# purposes. The Pacman AI projects were developed at UC Berkeley, primarily by
# John DeNero (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and Pieter 
# Abbeel in Spring 2013.
# For more info, see http://inst.eecs.berkeley.edu/~cs188/pacman/pacman.html

""" In search.py, you will implement generic search algorithms which are called by Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples,
        (successor, action, stepCost), where 'successor' is a
        successor to the current state, 'action' is the action
        required to get there, and 'stepCost' is the incremental
        cost of expanding to that successor
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.  The sequence must
        be composed of legal moves
        """
        util.raiseNotDefined()

def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other
    maze, the sequence of moves will be incorrect, so only use this for tinyMaze
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return  [s,s,w,s,w,w,s,w]

def depthFirstSearch(problem):
    """
    Search the deepest nodes in the search tree first

    Your search algorithm needs to return a list of actions that reaches
    the goal.  Make sure to implement a graph search algorithm

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:

    print "Start:", problem.getStartState()
    print "Is the start a goal?", problem.isGoalState(problem.getStartState())
    print "Start's successors:", problem.getSuccessors(problem.getStartState())
    """
    "*** YOUR CODE HERE ***"
    #util.raiseNotDefined()
    stack = util.Stack()
    ## use the stack to restore (state, actions, stepCost) pair
    stack.push((problem.getStartState(), list([]), 0))
    ## use set to label the explored set
    explored = set()
    
    ## non-recursive version of stack
    while not stack.isEmpty():
        # pop out the top of stack
        (current, actions, totalcost) = stack.pop()
        if current in explored: # explored state
            continue
        elif problem.isGoalState(current): 
            # return a series of actions to current state
            return actions
        else:
            successors = problem.getSuccessors(current)
            explored.update([current]) # label current state as explored

            if successors is None: 
                continue
            # add all states as successor
            for (successor, action, cost) in successors:
                if (successor) not in explored:
                    stack.push((successor, list(actions) + [action], totalcost+cost))

def breadthFirstSearch(problem):
    """
    Search the shallowest nodes in the search tree first.
    """
    "*** YOUR CODE HERE ***"
    #util.raiseNotDefined()
    queue = util.Queue()
    queue.push((problem.getStartState(), list([]), 0))
    explored = set()

    while not queue.isEmpty():
        (current, actions, pathcost) = queue.pop()
        if current in explored:
            continue
        elif problem.isGoalState(current):
            return actions
        else: # further breadth search
            successors = problem.getSuccessors(current)
            explored.update([current])
            for (suc, act, stepcost) in successors:
                if suc not in explored:
                    queue.push((suc, list(actions) + [act], pathcost+stepcost))
    

def uniformCostSearch(problem):
    "Search the node of least total cost first. "
    "*** YOUR CODE HERE ***"
    #util.raiseNotDefined()
    pq = util.PriorityQueue()
    pq.push((problem.getStartState(), list([]), 0.0), 0.0)
    explored = set()

    while not pq.isEmpty():
        (current, actions, pathcost) = pq.pop()
        if current in explored:
            continue
        elif problem.isGoalState(current):
            return actions
        else:
            successors = problem.getSuccessors(current)
            explored.update([current])
            if successors is None:
                continue
            for (suc, act, stepcost) in successors:
                if suc not in explored:
                    pq.push((suc, list(actions)+[act], pathcost+stepcost),
                            pathcost+stepcost)
    

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    return 0

def aStarSearch(problem, heuristic=nullHeuristic):
    "Search the node that has the lowest combined cost and heuristic first."
    "*** YOUR CODE HERE ***"
    pq = util.PriorityQueue()
    pq.push((problem.getStartState(), list([]), 0.0), 0.0)
    explored = set()
    while not pq.isEmpty():
        (current, actions, pathcost) = pq.pop()
        if current in explored:
            continue
        elif problem.isGoalState(current):
            return actions
        else:
            successors = problem.getSuccessors(current)
            explored.update([current])
            for (suc, act, cost) in successors:
                if suc not in explored:
                    sucPathCost = pathcost + cost
                    pq.push((suc, list(actions)+[act], sucPathCost),
                            sucPathCost+heuristic(suc, problem))

    #util.raiseNotDefined()


# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
