import util
import classificationMethod

from matplotlib.mlab import PCA
import numpy as NP
from scipy import linalg as LA
import random
import mira

class contestClassifier(classificationMethod.ClassificationMethod):
    """
    Create any sort of classifier you want. You might copy over one of your
    existing classifiers and improve it.
    """
    def __init__(self, legalLabels):
        self.guess = None
        self.type = "minicontest"
        self.eigenVectors = None
        self.pp_train_data = None
        self.mira = mira.MiraClassifier(range(10), 10)

    def train (self, data, labels, validationData, validationLabels):
        """
        Please describe your training procedure here.
        """
        self.mira.train(data, labels, validationData, validationLabels)

        '''
        train_data = []
        for datum in data:
            train_data.append(datum.values())
        tmp = set([tuple(elem) for elem in train_data])
        train_data = []
        for t in tmp:
            train_data.append(list(t))
        '''

        """PCA"""
        '''
        ## training data
        truncated_data = NP.matrix(random.sample(train_data, 512))
        train_data = NP.matrix(train_data)
        print "train_data.shape:", train_data.shape
        print "truncated_data.shape:", truncated_data.shape

        result = PCA (train_data)
        print result
        '''
        '''
        ## move to the center
        mn = NP.mean(train_data, axis=0)
        print "mean.shape:", mn.shape
        train_data -= mn
        self.mean = mn

        ## get covariance
        C = NP.cov(train_data.T)

        eigenValues, eigenVectors = LA.eig(C)
        idx = NP.argsort(eigenValues)[::-1]
        eigenVectors = eigenVectors[:,idx]
        eigenValues = eigenValues[idx]

        eigenVectors = eigenVectors[:,:64]
        print eigenValues
        self.eigenVectors = eigenVectors

        pp_train_data = NP.dot(eigenVectors.T, train_data.T).T
        self.pp_train_data = pp_train_data
        print "self.pp_train_data.shape:", self.pp_train_data.shape
        self.labels = labels
        '''

    def classify(self, testData):
        """
        Please describe how data is classified here.
        """
        '''
        test_matrix = []
        for instance in testData:
            test_matrix.append(instance.values())
        

        test_matrix = NP.matrix(test_matrix) - self.mean
        pp_test_data = NP.dot(self.eigenVectors.T, test_matrix.T).T

        K = 3
        predictions = []
        print pp_test_data.shape
        for i in range(0, pp_test_data.shape[0]):
            counter = util.Counter()
            dists = util.PriorityQueue()
            test_instance = pp_test_data[i,:]
            closest_neighbours = []
            for j in range(0, self.pp_train_data.shape[1]):
                train_instance = self.pp_train_data[j,:]
                dists.push(j, distance(test_instance, train_instance))
            for k in range(0, K):
                counter[self.labels[dists.pop()]] += 1
            predictions.append(counter.argMax())

        print predictions
        print len(predictions)
        return predictions
        '''
        return self.mira.classify(testData)


def distance(item1, item2):
    dist = (item1 - item2) * (item1 - item2).T
    return dist
