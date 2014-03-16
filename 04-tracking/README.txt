Team members:
Name1: Aming Ni
UT ID: an23366
CS ID: ani91

Name2: Xin Lin
UT ID: x15224
CS ID: jimmylin

Team members' contribution:
	Aming Ni: 50%
	Xin Lin: 50%

==============================================================
Q1:
	In the observe function:
		1: We check if a ghost is captured or not:
			->If a ghost is not captured, update the particles using observation data.
			->If a ghost is captured, update every particle to indicate it's in jail.

Q2:
	In the elapseTime funciton:
		1: We get the beliefDistribution for all the particles.
		2: We loop through all legal positions, and get distribution for that position.
		   The new distribution is then added up to represent the overall weight.
		3: We check if the total weight is 0.
			If it's 0, we resample all particles with uniform distribution.
			If it's not 0, we resample all particles according to the weights.

Q3:
	In the chooseAction function:
		1: It loops through all the belief distributions, and find the closest ghost distances for each ghost.
		2: It loops all ghost distances, and find the closest distance for that ghost.

Q4:
	In the initializeUniform function:
		1: It checks for number of legal positions, 
		   and create (totalNumParticles/totalNumrOfLegalPosition) for each legal position.
		
	In the getBeliefDistribution function:
		1: It loops through all the particles in self.particles, 
		   and for each particle, it counts the total number of particles for that particle.
		   Lastly, it normalizes the weight.
	
	In the observe function:
		1: It checks to see if the belief distribution has a 0 weight.
		2: It creates a counter class, and using the belief distribution with observation
		   to get weights, and resample all the particles with the new weights.
		3: It checks if a ghost is in jail.

Q5:
	In the elapseTime function:
		1: It loops all the legal position, and for each legal position,
		   it loops through all the possible distribution, and calculate the new distribution
		   using both the belief distribution for each legal position and prior distribution
		2: It resamples the weight if it has 0 weights.

Q6:
	In the initializeParticles function:
		The uniform distribution is initialized by:
			1:Calculate all possible catesianProducts for legal positions
			2:Create (totalNumParticles/totalNumOfLegalPosition) for each legal position.
	
	In the getBeliefDistribution function:
		1:It creates a utility counter. The utility counter counts all the particles in the weight.
		2:It then normalizes the weight.
	
	In the observeState function:
		1:It recalculates the weight based on observation and prior distribution 
		  for each position in a particle, and for each particle.
		2:It resamples the weight by calling initializeParticles if the weight is 0.
		3:It resamples each particle by using the weight above.
		4:It checks if each ghost is in ghost, and update the positions when necessary.

Q7:
	In the elapseTime function:
		for each particle, and for each position in that particle. 
		It gets the distribution, and sample that position using said distribution.
