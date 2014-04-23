import util
import classificationMethod

import numpy as NP
from scipy import linalg as LA
import random

class contestClassifier(classificationMethod.ClassificationMethod):
    """
    Create any sort of classifier you want. You might copy over one of your
    existing classifiers and improve it.
    """
    def __init__(self, legalLabels):
        self.guess = None
        self.type = "minicontest"
        self.eigenVectors = None
        self.rescaled_train_data = None

    def train (self, data, labels, validationData, validationLabels):
        """
        Please describe your training procedure here.
        """
        train_data = []
        for datum in data:
            train_data.append(datum.values())

        """PCA"""
        truncated_data = NP.matrix(random.sample(train_data, 512))
        print train_data

        train_data = NP.matrix(train_data)

        mn = NP.mean(truncated_data, axis=0)
        truncated_data -= mn
        self.mean = mn

        C = NP.cov(truncated_data.T)

        eigenValues, eigenVectors = LA.eig(C)
        idx = NP.argsort(eigenValues)[::-1]
        eigenVectors = eigenVectors[:,idx]
        eigenValues = eigenValues[idx]

        eigenVectors = eigenVectors[:,:64]
        self.eigenVectors = eigenVectors

        rescaled_data = NP.dot(eigenVectors.T, train_data.T).T
        self.rescaled_train_data = rescaled_data
        self.labels = labels

    def classify(self, testData):
        """
        Please describe how data is classified here.
        """
        test_matrix = []
        for instance in testData:
            test_matrix.append(instance.values())
        

        test_matrix = NP.matrix(test_matrix) - self.mean
        rescaled_test_data = NP.dot(self.eigenVectors.T, test_matrix.T).T

        """ KNN """
        K = 10
        predictions = []
        print rescaled_test_data.shape
        for i in range(0, rescaled_test_data.shape[0]):
            counter = util.Counter()
            dists = util.PriorityQueue()
            test_instance = rescaled_test_data[i,:]
            closest_neighbours = []
            for j in range(0, self.rescaled_train_data.shape[1]):
                train_instance = self.rescaled_train_data[j,:]
                dists.push(j, distance(test_instance, train_instance))
            for k in range(0, K):
                counter[self.labels[dists.pop()]] += 1
            predictions.append(counter.argMax())

        print predictions
        print len(predictions)
        return predictions


def distance(item1, item2):
    dist = (item1 - item2) * (item1 - item2).T
    return dist
