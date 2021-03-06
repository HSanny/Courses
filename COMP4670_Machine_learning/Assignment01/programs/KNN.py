''' 
    Filename:   KNN.py    
    Version:    1.0
    Since:      2013-03-19
    Author: 
        Jimmy Lin (u5223173) - u5223173@uds.anu.edu.au
        
    Edited by MacVim
    Documentation auto-generated by Snippet 
'''
import numpy as np
import random

class KNNClassifier():
    def __init__(self, trainingData, testData,  k = 3):
        self.trainingData = trainingData
        self.testData = testData
        self.K = k

    def getFeatureVector(self, labeledObj):
        return labeledObj[0:4]

    def getLabel(self, labeledObj):
        return labeledObj[4]    

    def getEuclidianDistance(self, vec1, vec2):
#{{{
        ## Input Vector Verification
        if type(vec1) != type(vec2) or len(vec1) != len(vec2):
            print "Euclidian Distance: input vector mismatch"
            return 0;
        distance = 0
        for i in range(0,len(vec1)):
            distance += pow( vec2[i] - vec1[i],2)
        return distance
#}}}

    def getDistance(self, testObj):
        #{{{
        ''' 
        This method accept the vector of the object whose distance is to be calculated
        This method return 
            (1) Dictionary containing pairs of E-distance -> Training Data Object
            (2) List of E-distance
        '''
        distanceList = []
        distanceDict = {}
        for labeledObj in self.trainingData:
            temp = self.getEuclidianDistance( labeledObj[0:4], testObj[0:4])
            distanceList.append(temp)
            distanceDict.update({temp:labeledObj})
#}}}
        return distanceDict, distanceList
    
    def getKNearestNeighbours(self, distanceDict, distanceList):
#{{{
        neighbours = []
        distanceList = sorted(distanceList, reverse=False)

        distanceList = distanceList[0:self.K]
        for key in distanceList:
            temp = distanceDict.get(key)
            neighbours.append(temp)
#}}}
        return neighbours

    def majorityVoting(self, neighbours):
#{{{
        ## Apply Voting for neighbours 
        voting = {}
        for labeledObj in neighbours:
            key = self.getLabel(labeledObj)
            value = voting.get( key, None)
            if value is not None:
                ## increment the occurence
                voting.update({key:(value+1)})
            else:
                ## insert new piece key with occurrence 1
                voting.update({key:1})
        
        ## After voting, pick out the one was elected
        max = -1
        for label in voting:
            times = voting.get( label)
            if times > max :
                max = times
                result = label
#}}}
        return result

    def run(self):
#{{{
        result = []
        for i in range(0, len(self.testData)):
            result.append([])
        labels = []
        for testObj in self.testData:
            distanceDict, distanceList = self.getDistance( testObj)
            neighbours = self.getKNearestNeighbours( distanceDict, distanceList)
            temp = self.majorityVoting( neighbours)
            labels.append( temp)
        #print len(self.testData)
        for i in range(0, len(labels)):
            result[i] += self.testData[i] + [ labels[i] ]
            #print result[i]
#}}}
        return result

def classMapping(flowerName):
    '''
     map the string-valued target variable to be
    numerical representation
    '''
#{{{
    if flowerName == "Iris-setosa":
        return 0
    elif flowerName == "Iris-versicolor":
        return 1
    elif flowerName == "Iris-virginica":
        return 2
    else :
        print "Flower Name Exception: unknown flower"
        return -1
#}}}

def readMatrix(filePath):
#{{{
    ## Exception handling for file opening
    try:
        IrisFile = open(filePath)
    except:
        print "Iris Data IO error, please check your input path"
    
    ## read in raw string, and store in temporary matrix (string only)
    tempList = []
    for tempStr in IrisFile: # read each line of IrisData
        tempStr = tempStr.replace("\n","")
        tempList.append( tempStr.split(",") )
    #printMatrix(tempList)

    ## now convert string to numeric value and store in another matrix IrisData
    Data = []
    for row in range(0,len(tempList)):
        temp = []
        if len(tempList[row]) < 5: ## encounter empty line
            break
        ## 0-3 features need direct conversion
        for col in range(0,4): 
            temp.append( float( tempList[row][col] ) )
        ## 4th feature need particular mapping
        temp.append( classMapping( tempList[row][4] ) )
        Data.append( temp )
    #printMatrix(IrisData)
#}}}
    return Data

def scaling(data, numOfFeatures = 4):
    ''' Scale each entry in input data to the interval [0,1]'''
#{{{
    ## Initialize the maximum and minimum of each feature
    
    maxList = [-9999] * numOfFeatures
    minList = [9999] * numOfFeatures

    ## Obtain the maximum and minimum by tranversal
    for row in range(0,len(data)):
        for col in range(0, numOfFeatures):
            temp = data[row][col]
            if temp > maxList[col]:
                maxList[col] = temp
                continue
            if temp < minList[col]:
                minList[col] = temp
                continue
    
    ## Start Scaling
    scaledData = []
    for row in range(0,len(data)):
        tempRow = []
        for col in range(0, numOfFeatures):
            tempEntry =  (data[row][col] - minList[col]) / (maxList[col] - minList[col])
            tempRow.append( tempEntry)
        ## directly copy the class label
        tempRow.append( data[row][numOfFeatures])
        scaledData.append( tempRow)
#}}}

    return scaledData

class CrossValidation():
    def __init__(self, rawData , foldNumber = 2):
        self.foldNumber = foldNumber
        ## copy content of rawData to self.rawData instead of copy reference
        self.rawData = []
        for row in range(0, len(rawData)):
            temp = []
            for col in range(0, len(rawData[row])):
                temp += [ rawData[row][col] ]
            self.rawData.append(temp)
        ## group data for cross validation
        self.randomGroup = []
        for i in range(0, self.foldNumber):
            self.randomGroup.append([])
        self.getRandomGroup(self.rawData[0:50])
        self.getRandomGroup(self.rawData[50:100])
        self.getRandomGroup(self.rawData[100:150])
       
    def getRandomGroup(self, data):
        ## get the random group for objects of each class
#{{{
        size = len(data) / self.foldNumber
        for i in range(0, self.foldNumber):
            for j in range(0, size):
                self.randomGroup[i].append(data[j+i*size])
#}}}
        return

    def getError(self, result):
#{{{
        numOfMissClassified = 0
        sizeOfTestData = len(result)
        for testObj in result:
            if testObj[-2] != testObj[-1]:
                numOfMissClassified += 1
                # print testObj[-2], testObj[-1]
        errorRate = numOfMissClassified * 1.0 / sizeOfTestData
#}}}
        return errorRate

    def run(self, k = 3): # k is the neighbour parameter
#{{{
        errorList = []
        for i in range(0, self.foldNumber):
            ## separate data for training and testing
            testData = self.randomGroup[i]
            trainingData = []
            for j in range(0, self.foldNumber):
                if j != i:
                    trainingData += self.randomGroup[j]
            ## start using KNN classifier
            IrisKNN = KNNClassifier( trainingData, testData, k)
            result = IrisKNN.run()

            errorList.append( self.getError(result) )
        
        displayStr = "" + str(k) + "-NN "
        displayStr += str(self.foldNumber) + "-fold CV "
        displayStr += ' %.2f%% ' %(100*sum(errorList)/len(errorList))
        displayStr += " ("
        for i in range(0, len(errorList) - 1):
            displayStr += ' %.2f%%,' %(100*errorList[i])
        displayStr += ' %.2f%% )' %(100*errorList[-1])
        print displayStr
#}}}
        return 100*sum(errorList)/len(errorList)

def getSolution_4_1_1_(Data, scaled=True, PRINT = False):
#{{{
    if scaled:
        Data = scaling( Data)
    ### 4.1.1 KNN algorithm implementation (separate testData & training data Manually)
    ## Here we just use the last data object as testing data, and all others as training data
    testData = [Data[-1]]
    trainingData = Data[0:-1]

    IrisKNN = KNNClassifier( trainingData, testData, k = 3)
    result = IrisKNN.run()
    if PRINT:
        for each in result:
            print each[0:-2], " label:", each[-2], " prediction: ", each[-1]
#}}}
    return

def getSolution_4_1_2_(Data, scaled=True, PRINT=False):
#{{{
    if scaled:
        Data = scaling( Data)
    ### 4.1.2 Cross Validation
    ## 2-fold Cross-Validation implementation
    twoFoldCV = CrossValidation(Data, foldNumber = 2)
    twoFoldResult = []
    for k in range(2, 40, 1):
        temp = twoFoldCV.run(k)
        twoFoldResult.append((k, temp))
    sortedTwoFoldResult = sorted(twoFoldResult, key = lambda d:(d[1]*1000-d[0]))
    print "sorted result for 2-fold cross validation"
    for each in sortedTwoFoldResult:
        print each 
    print "the optimal k we choose in 2-fold CV: ", sortedTwoFoldResult[0]
    print ""

    ## 5-fold Cross-Validation implementation
    fiveFoldCV = CrossValidation(Data, foldNumber = 5)
    fiveFoldResult = []
    for k in range(2, 40, 1):
        temp = fiveFoldCV.run(k)
        fiveFoldResult.append((k, temp))
    sortedFiveFoldResult = sorted(fiveFoldResult, key = lambda d:(d[1]*1000-d[0]))
    print "sorted result for 5-fold cross validation"
    for each in sortedFiveFoldResult:
        print each 
    print "the optimal k we choose in 5-fold CV: ", sortedFiveFoldResult[0] 
    print ""

    ## 10-fold Cross-Validation implementation
    tenFoldCV = CrossValidation(Data, foldNumber = 10)
    tenFoldResult = []
    for k in range(2, 40, 1):
        temp = tenFoldCV.run(k)
        tenFoldResult.append((k,temp))
    sortedTenFoldResult = sorted(tenFoldResult, key = lambda d:(d[1]*1000-d[0]))
    print "sorted result for 10-fold cross validation"
    for each in sortedTenFoldResult:
        print each 
    print "the optimal k we choose in 10-fold CV: ", sortedTenFoldResult[0]
    print ""
#}}}
    return

def getSolution_4_1_3_(Data, scaled=True, PRINT=False):
#{{{
    if scaled:
        Data = scaling( Data)
    ### 4.1.3 report for different k
    twoFoldCV = CrossValidation(Data, foldNumber = 2)
    fiveFoldCV = CrossValidation(Data, foldNumber = 5)
    tenFoldCV = CrossValidation(Data, foldNumber = 10)
    for k in range(2, 42, 2):
        twoFoldCV.run(k)
        fiveFoldCV.run(k)
        tenFoldCV.run(k)
#}}}
    return

def main():
    ## Data import to memory
    IrisDataFile = "./bezdekIris.data.txt"
    IrisData = readMatrix( IrisDataFile)
    ## We have to do scaling because KNN is vulnerable to bad scaling

    #getSolution_4_1_1_(IrisData, scaled=True, PRINT=True)
    #getSolution_4_1_2_(IrisData, scaled=False, PRINT=True)
    getSolution_4_1_3_(IrisData, scaled=True, PRINT=True)

main()
