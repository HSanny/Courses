# multiAgents.py
# --------------
# Licensing Information: Please do not distribute or publish solutions to this
# project. You are free to use and extend these projects for educational
# purposes. The Pacman AI projects were developed at UC Berkeley, primarily by
# John DeNero (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and Pieter 
# Abbeel in Spring 2013.
# For more info, see http://inst.eecs.berkeley.edu/~cs188/pacman/pacman.html

import searchAgents, search

from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

class ReflexAgent(Agent):
    """
      A reflex agent chooses an action at each choice point by examining
      its alternatives via a state evaluation function.

      The code below is provided as a guide.  You are welcome to change
      it in any way you see fit, so long as you don't touch our method
      headers.
    """
    def __init__(self):
        self.heuristicInfo = None

    def getAction(self, gameState):
        """
        You do not need to change this method, but you're welcome to.

        getAction chooses among the best options according to the evaluation function.

        Just like in the previous project, getAction takes a GameState and returns
        some Directions.X for some X in the set {North, South, West, East, Stop}
        """
        # Collect legal moves and successor states
        legalMoves = gameState.getLegalActions()

        # Choose one of the best actions
        scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
        bestScore = max(scores)
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
        chosenIndex = random.choice(bestIndices) # Pick randomly among the best

        "Add more of your code here if you want to"

        return legalMoves[chosenIndex]

    def evaluationFunction(self, currentGameState, action):
        """
        Design a better evaluation function here.

        The evaluation function takes in the current and proposed successor
        GameStates (pacman.py) and returns a number, where higher numbers are better.

        The code below extracts some useful information from the state, like the
        remaining food (newFood) and Pacman position after moving (newPos).
        newScaredTimes holds the number of moves that each ghost will remain
        scared because of Pacman having eaten a power pellet.

        Print out these variables to see what you're getting, then combine them
        to create a masterful evaluation function.
        """
        # Useful information you can extract from a GameState (pacman.py)
        successorGameState = currentGameState.generatePacmanSuccessor(action)
        newPos = successorGameState.getPacmanPosition()
        newFood = successorGameState.getFood()
        # list of ghost in (x,y), direction
        newGhostStates = successorGameState.getGhostStates()
        newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]
        #print newScaredTimes
        # current position
        curPos = currentGameState.getPacmanPosition()

        "*** YOUR CODE HERE ***"
        px, py = newPos
        ghostDistance = 0
        isGhostNearby = False
        ## GHOST EATING MECHANISM:
        findGhost = 0
        killGhost = sum(newScaredTimes)
        for gindex in range(0, len(newScaredTimes)):
            if newScaredTimes[gindex] > 0:
                gposition = newGhostStates[gindex].getPosition()
                gposition = (int(gposition[0]), int(gposition[1]))
                findGhost -= searchAgents.mazeDistance(newPos,
                                           gposition, currentGameState)
        if killGhost > 0:
            return findGhost

        ## SAFETY MECHANISM: 
        ## if the ghost occurs within 6 steps of pacman, we indicate that
        ## the pacman is in danger, thus take measures to protect it
        for ghost in newGhostStates:
            gx, gy = ghost.getPosition()
            gPos = (int(gx), int(gy))
            gDist = searchAgents.mazeDistance(gPos, newPos, successorGameState)
            if gDist <= 3:
                ghostDistance += gDist
                isGhostNearby = True
        if isGhostNearby: # here we care about safety
            self.heuristicInfo = None
            return ghostDistance - 1000

        ## FOOD EATING MECHANISM:
        ## search the closest food and find actions to reach it
        heuristic = 0
        agent = searchAgents.ClosestDotSearchAgent(fn="breadthFirstSearch")
        actions = agent.findPathToClosestDot(currentGameState)
        if action == actions[0]:
            heuristic = 1

        return heuristic

def scoreEvaluationFunction(currentGameState):
    """
      This default evaluation function just returns the score of the state.
      The score is the same one displayed in the Pacman GUI.

      This evaluation function is meant for use with adversarial search agents
      (not reflex agents).
    """
    return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
    """
      This class provides some common elements to all of your
      multi-agent searchers.  Any methods defined here will be available
      to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

      You *do not* need to make any changes here, but you can if you want to
      add functionality to all your adversarial search agents.  Please do not
      remove anything, however.

      Note: this is an abstract class: one that should not be instantiated.  It's
      only partially specified, and designed to be extended.  Agent (game.py)
      is another abstract class.
    """

    def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
        self.index = 0 # Pacman is always agent index 0
        self.evalFn = evalFn
        self.evaluationFunction = util.lookup(evalFn, globals())
        self.depth = int(depth)

class MinimaxAgent(MultiAgentSearchAgent):
    """
      Your minimax agent (question 2)
    """

    def getAction(self, gameState):
        """
          Returns the minimax action from the current gameState using self.depth
          and self.evaluationFunction.

          Here are some method calls that might be useful when implementing minimax.

          gameState.getLegalActions(agentIndex):
            Returns a list of legal actions for an agent
            agentIndex=0 means Pacman, ghosts are >= 1

          gameState.generateSuccessor(agentIndex, action):
            Returns the successor game state after an agent takes an action

          gameState.getNumAgents():
            Returns the total number of agents in the game
        """
        "*** YOUR CODE HERE ***"
        nAgents = gameState.getNumAgents() 
        agentIndex = self.index % nAgents
        ## note that in game, a ply corresponds to one move of all agents
        if self.index / nAgents == self.depth: # for the leaf node
            return self.evaluationFunction(gameState)
        elif self.index == 0: # for the root node
            succesors = []
            actions = gameState.getLegalActions(self.index)
            for action in actions:
                successor = gameState.generateSuccessor(self.index, action)
                succesors.append(successor)
            max_score = None
            max_asgn = None
            tmp_agent = MinimaxAgent()
            tmp_agent.index = self.index + 1
            tmp_agent.depth = self.depth
            for i in range(0, len(succesors)):
                suc = succesors[i]
                tmp_score = tmp_agent.getAction(suc)
                if max_score is None or tmp_score > max_score:
                    max_score = tmp_score
                    max_asgn = i
                #print tmp_score, i
            #print  max_score, actions[max_asgn]
            return actions[max_asgn]
        else: # for all other immediate node
            if self.index % nAgents == 0: opt = max
            else: opt = min
            succesors = []
            for action in gameState.getLegalActions(agentIndex):
                successor = gameState.generateSuccessor(agentIndex, action)
                succesors.append(successor)
            scores = []
            tmp_agent = MinimaxAgent()
            tmp_agent.index = self.index + 1
            tmp_agent.depth = self.depth
            for suc in succesors:
                tmp_score = tmp_agent.getAction(suc)
                scores.append(tmp_score)
            if len(scores) == 0: return tmp_agent.evaluationFunction(gameState)
            else: return opt(scores) 

infinity = float('inf')   
class AlphaBetaAgent(MultiAgentSearchAgent):
    """
      Your minimax agent with alpha-beta pruning (question 3)
    """
    def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
        MultiAgentSearchAgent.__init__(self,evalFn,depth)

        self.alpha = -float('inf')
        self.beta = float('inf')
        self.lastAgent = None

    def initialize(self,index,lastAgent):
        self.index = index
        self.lastAgent = lastAgent

    def update(self, alpha, beta):
        self.alpha = alpha
        self.beta = beta

    def getAction(self, gameState):
        """
          Returns the minimax action using self.depth and self.evaluationFunction
        """
        "*** YOUR CODE HERE ***"
        nAgents = gameState.getNumAgents() 
        agentIndex = self.index % nAgents
        actions = gameState.getLegalActions(agentIndex)
        if len(actions) == 0:
            return self.evaluationFunction(gameState)
#{{{
        ## note that in game, a ply corresponds to one move of all agents
        if self.index == self.depth * nAgents: # for the leaf node
            return self.evaluationFunction(gameState)

        if self.index == 0: # for the root node
            max_score = None
            max_asgn = None
            for i in range(0, len(actions)):
                successor = gameState.generateSuccessor(self.index, actions[i])
                if self.index + 1 == nAgents * self.depth:
                    tmp_score = self.evaluationFunction(successor)
                else:
                    tmp_agent = AlphaBetaAgent(depth=self.depth)
                    tmp_agent.initialize(self.index+1, self)
                    tmp_agent.update(self.alpha, self.beta)
                    tmp_score = tmp_agent.getAction(successor)
                self.alpha = max(self.alpha, tmp_score)
                if max_score is None or tmp_score > max_score:
                    max_score = tmp_score
                    max_asgn = i
            #print  max_score, actions[max_asgn]
            return actions[max_asgn]
        else: # for all other immediate node 
            isMaxValue = (self.index % nAgents == 0)
            if isMaxValue: 
                opt = max
                optimal = -float('inf')
            else: 
                opt = min
                optimal = float('inf')
            for action in actions:
                successor = gameState.generateSuccessor(agentIndex, action)
                if self.index + 1 == nAgents * self.depth:
                    tmp_score = self.evaluationFunction(successor)
                else:
                    tmp_agent = AlphaBetaAgent(depth=self.depth)
                    tmp_agent.initialize(self.index+1, self)
                    tmp_agent.update(self.alpha, self.beta)
                    tmp_score = tmp_agent.getAction(successor)
                optimal = opt(optimal, tmp_score)
                #print tmp_score, optimal, (self.alpha, self.beta), (self.lastAgent.alpha, self.lastAgent.beta)
                if isMaxValue:
                    if optimal > self.beta: return optimal
                    self.alpha = opt(self.alpha, optimal)
                else:
                    if optimal < self.alpha: return optimal
                    self.beta = opt(self.beta, optimal)

            return optimal
#}}}

def expectation (scores):
    return 1.0 * sum(scores) / len(scores)

class ExpectimaxAgent(MultiAgentSearchAgent):
    """
      Your expectimax agent (question 4)
    """
    def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
        MultiAgentSearchAgent.__init__(self,evalFn,depth)

    def update(self, index, depth):
        self.index = index
        self.depth = depth

    def getAction(self, gameState):
        """
          Returns the expectimax action using self.depth and self.evaluationFunction

          All ghosts should be modeled as choosing uniformly at random from their
          legal moves.
        """
        "*** YOUR CODE HERE ***"
        nAgents = gameState.getNumAgents() 
        agentIndex = self.index % nAgents
        ## note that in game, a ply corresponds to one move of all agents
        if self.index == nAgents * self.depth: # for the leaf node
            return self.evaluationFunction(gameState)
        elif self.index == 0: # for the root node
            succesors = []
            actions = gameState.getLegalActions(self.index)
            for action in actions:
                successor = gameState.generateSuccessor(self.index, action)
                succesors.append(successor)
            max_score = None
            max_asgn = None
            tmp_agent = ExpectimaxAgent(evalFn=self.evalFn, depth=self.depth)
            tmp_agent.update(self.index + 1, self.depth)
            for i in range(0, len(succesors)):
                suc = succesors[i]
                tmp_score = tmp_agent.getAction(suc)
                if max_score is None or tmp_score > max_score:
                    max_score = tmp_score
                    max_asgn = i
            return actions[max_asgn]
        else: # for all other immediate node
            if self.index % nAgents == 0: opt = max
            else: opt = expectation
            succesors = []
            for action in gameState.getLegalActions(agentIndex):
                successor = gameState.generateSuccessor(agentIndex, action)
                succesors.append(successor)
            scores = []
            tmp_agent = ExpectimaxAgent(evalFn=self.evalFn, depth=self.depth)
            tmp_agent.update(self.index + 1, self.depth)
            for suc in succesors:
                tmp_score = tmp_agent.getAction(suc)
                scores.append(tmp_score)
            if len(scores) == 0: return tmp_agent.evaluationFunction(gameState)
            else: return opt(scores) 

global nTotalFood, target, nRemained
nTotalFood = None
target = None
def betterEvaluationFunction(currentGameState):
    """
      Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
      evaluation function (question 5).

      DESCRIPTION: returns a number, where higher numbers are better
    """
    "*** YOUR CODE HERE ***"
    return currentGameState.getScore() 
    pacmanPosition = currentGameState.getPacmanPosition()
    GhostStates = currentGameState.getGhostStates()
    foodGrid = currentGameState.getFood()
    actions = currentGameState.getLegalActions(0)
    ScaredTimes = [ghostState.scaredTimer for ghostState in GhostStates]

    if target is None:
        agent = searchAgents.ClosestDotSearchAgent(fn="breadthFirstSearch")
        actions = agent.findPathToClosestDot(currentGameState)
    # distance measure
    DM = searchAgents.mazeDistance

    nGhosts = len(GhostStates)
    distToGhost = [0 for x in range(0, nGhosts)]

    ## GHOST MECHANISM:
    for gindex in range(0, nGhosts):
        gPosition = GhostStates[gindex].getPosition()
        gPosition = (int(gPosition[0]), int(gPosition[1]))
        distToGhost[gindex] = DM(pacmanPosition, gPosition, currentGameState)
    
    ## penalty to close ghost
    penGhosts = 0
    DANGERDISTANCE = 3
    SAFETYDESIRE = 1000
    for gindex in range(0, nGhosts):
        if distToGhost[gindex] < DANGERDISTANCE and ScaredTimes[gindex] <= 0:
            penGhosts += 1.0 * SAFETYDESIRE / (distToGhost[gindex]+1)
        elif ScaredTimes[gindex] > 0: 
            penGhosts -= 1.0 * SAFETYDESIRE / (distToGhost[gindex]+1)

    ## reward to close food
    rwdFood = 0
    FOODDESIRE = 100
    #agent = searchAgents.ClosestDotSearchAgent(fn="breadthFirstSearch")
    #actions = agent.findPathToClosestDot(currentGameState)

    rwdFood = 50 * (nTotalFood - nRemainedFood)

    ### NOTE: the final score of current state, the higher the better
    score = -penGhosts + rwdFood + random.random()
    
    '''
    ## SAFETY MECHANISM: 
    ## if the ghost occurs within 6 steps of pacman, we indicate that
    ## the pacman is in danger, thus take measures to protect it
    for ghost in newGhostStates:
        gx, gy = ghost.getPosition()
        gPos = (int(gx), int(gy))
        gDist = searchAgents.mazeDistance(gPos, newPos, successorGameState)
        if gDist <= 3:
            ghostDistance += gDist
            isGhostNearby = True
    if isGhostNearby: # here we care about safety
        self.heuristicInfo = None
        return ghostDistance - 1000

    ## FOOD EATING MECHANISM:
    ## search the closest food and find actions to reach it
    heuristic = 0
    agent = searchAgents.ClosestDotSearchAgent(fn="breadthFirstSearch")
    actions = agent.findPathToClosestDot(currentGameState)
    if action == actions[0]:
        heuristic = 1

    return heuristic
    '''

# Abbreviation
better = betterEvaluationFunction

class ContestAgent(MultiAgentSearchAgent):
    """
      Your agent for the mini-contest
    """

    def getAction(self, gameState):
        """
          Returns an action.  You can use any method you want and search to any depth you want.
          Just remember that the mini-contest is timed, so you have to trade off speed and computation.

          Ghosts don't behave randomly anymore, but they aren't perfect either -- they'll usually
          just make a beeline straight towards Pacman (or away from him if they're scared!)
        """
        "*** YOUR CODE HERE ***"
        util.raiseNotDefined()

