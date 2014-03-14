Team members:
Name1: Aming Ni
UT ID: an23366
CS ID: ani91

Name2: XIN LIN
UT ID: xl5224
CS ID: jimmylin

Team members' contribution: 
    Aming Ni  50%
    XIN LIN  50%

---------------------------Description-----------------------

Q1:
   Since the value iteration uses the batch version, my valueIteration goes as follows:
   1: Copy all the values to a tempValues util.counter.
   2: Update each newValues using tempValues.
   3: The algorithm for updating the values is exactly the same from the
   powerpoint, which uses dynamic programming techniques.
   ComputingQvalueFromValues takes the max possible values from the set of actions over  the next state.

Q2:
   We set:
	 discount = 0.9
	 noise = 0
   So the agent will have a deterministic move, which will force the optimal policy to go straight
without considering falling off the bridge.

Q3:
1:
  We set:
        discount = 0.1
        noise = 0
        livingReward = 0
   So the agent will prefer the close exit without worrying about risking the cliff.

2:
  We set:
	    discount = 0.1
        noise = 0.1
        livingReward = 0.4
   So the agent will prefer the close exit while avoiding the cliff.

3:
  We set:
        discount = 0.9
        noise = 0
        livingReward = 0.1
   So the agent is forced to prefer near feature exit with greater value without
worrying about risking the cliff.

4:
  We set:
	    discount = 0.9
        noise = 0.2
        livingReward = 0.1
   So the agent is forced to prefer distant feature exit with greater value while avoiding the cliff.

5:
  I set:
        discount = 1
        noise = 0.2
        livingreward = 10
   So the agent doesn't care about the infinite feature reward, and wants to live.

Q4:
  I use util.counter() as my data structure to store q values. All the methods are almost the same
as in the valueiteration, except for the update:
	self.Qvalues[tuple([state,action])] = (1-self.alpha) * self.getQValue(state,action)+self.alpha*(reward+self.discount*tempMax)
  Which pick the best action from the set of actions, and store it into tempMax.
 Then update that state using the formula from the powerpoint.

Q5:
    The getAction is implemented just by two lines:
    if (randommove): get a random action
    else: get the best action from the qvalues
    return action

Q6:
   The string 'NOT POSSIBLE' is returned for this question.
   Because, no set of parameters can guarantee the optimal policy.

Q7:
   Nothing is particularly implemented for this problem, since everything was
   implemented before.

Q8:
   I used the formula from the website to update the weights.
   The underlying principle is the same as the Q value updates.
