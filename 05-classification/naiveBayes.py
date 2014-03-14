import util
import classificationMethod
import math

class NaiveBayesClassifier(classificationMethod.ClassificationMethod):
  """
  See the project description for the specifications of the Naive Bayes classifier.
  
  Note that the variable 'datum' in this code refers to a counter of features
  (not to a raw samples.Datum).
  """
  def __init__(self, legalLabels):
    self.legalLabels = legalLabels
    self.type = "naivebayes"
    self.k = 1 # this is the smoothing parameter, ** use it in your train method **
    self.automaticTuning = False # Look at this flag to decide whether to choose k automatically ** use this in your train method **
    
  def setSmoothing(self, k):
    """
    This is used by the main method to change the smoothing parameter before training.
    Do not modify this method.
    """
    self.k = k

  def train(self, trainingData, trainingLabels, validationData, validationLabels):
    """
    Outside shell to call your method. Do not modify this method.
    """  
      
    self.features = trainingData[0].keys() # this could be useful for your code later...
    
    if (self.automaticTuning):
        kgrid = [0.001, 0.01, 0.05, 0.1, 0.5, 1, 5, 10, 20, 50]
    else:
        kgrid = [self.k]
        
    self.trainAndTune(trainingData, trainingLabels, validationData, validationLabels, kgrid)
      
  def trainAndTune(self, trainingData, trainingLabels, validationData, validationLabels, kgrid):
    """
    Trains the classifier by collecting counts over the training data, and
    stores the Laplace smoothed estimates so that they can be used to classify.
    Evaluate each value of k in kgrid to choose the smoothing parameter 
    that gives the best accuracy on the held-out validationData.
    
    trainingData and validationData are lists of feature Counters.  The corresponding
    label lists contain the correct label for each datum.
    
    To get the list of all possible features or labels, use self.features and 
    self.legalLabels.
    """

    "*** YOUR CODE HERE ***"
    optimal_accuracy = None

    # use counter to restore common values
    cPrior = util.Counter()
    cConditional = util.Counter()
    cCounts = util.Counter()
    
    # figure out the common prior, conditional probability and counts
    for i in range(0, len(trainingData)):
        entity = trainingData[i]
        label = trainingLabels[i]
        cPrior[label] += 1
        for feat, value in entity.items():
            if value > 0: ## binary variable
                cConditional[(feat, label)] += 1

    max_accuracy = None
    # computation for each smoothing
    for k in kgrid:
        PriorProb = util.Counter()
        ConditionalProb = util.Counter()

        nTrainingEntities = len(trainingData)
        ## compute prior probability for the specific k
        for label, count in cPrior.items():
            PriorProb[label] = 1.0 * count / nTrainingEntities
        ## compute conditional probability for the specific k
        for feat in self.features:
            for label in self.legalLabels:
                count = cConditional[(feat, label)]
                totalCount = cPrior[label]
                conditional = 1.0 * (count + k) / (totalCount + 2 * k)
                ConditionalProb[(feat, label)] = conditional

        self.PriorProb = PriorProb
        self.ConditionalProb = ConditionalProb

        predictions = self.classify(validationData)
        ## validate the data set
        nTestItems = len(validationLabels)
        assert nTestItems == len(predictions)

        comparisions = [predictions[i] == validationLabels[i] for i in range(0, nTestItems)]
        accuracy = 1.0 * sum(comparisions) / nTestItems
        if accuracy > max_accuracy or max_accuracy is None:
            self.k = k
            opt_setting = (PriorProb, ConditionalProb)
            max_accuracy = accuracy
    self.PriorProb, self.ConditionalProb = opt_setting


  def classify(self, testData):
    """
    Classify the data based on the posterior distribution over labels.
    
    You shouldn't modify this method.
    """
    guesses = []
    self.posteriors = [] # Log posteriors are stored for later data analysis (autograder).
    for datum in testData:
        posterior = self.calculateLogJointProbabilities(datum)
        guesses.append(posterior.argMax())
        self.posteriors.append(posterior)
    return guesses
      
  def calculateLogJointProbabilities(self, datum):
    """
    Returns the log-joint distribution over legal labels and the datum.
    Each log-probability should be stored in the log-joint counter, e.g.    
    logJoint[3] = <Estimate of log( P(Label = 3, datum) )>
    """
    logJoint = util.Counter()
    
    "*** YOUR CODE HERE ***"
    for label in self.legalLabels:
        logJoint[label] = math.log(self.PriorProb[label])
        for feat, value in datum.items():
            conditional = self.ConditionalProb[(feat,label)]
            if value > 0:
                logJoint[label] += math.log(conditional)
            else:
                logJoint[label] += math.log(1-conditional)
    return logJoint
  
  def findHighOddsFeatures(self, label1, label2):
    """
    Returns the 100 best features for the odds ratio:
            P(feature=1 | label1)/P(feature=1 | label2) 
    """
    featuresOdds = []
        
    "*** YOUR CODE HERE ***"
    tmp_pq = util.PriorityQueue()
    for feat in self.features:
        odds_ratio = self.ConditionalProb[feat, label1] / self.ConditionalProb[feat, label2] 
        tmp_pq.push((feat, odds_ratio), -odds_ratio) # highest value first out
    for i in range(0, 100):
        feat, odds_ratio = tmp_pq.pop()
        # print odds_ratio
        featuresOdds.append(feat)
    return featuresOdds
    

    
      
