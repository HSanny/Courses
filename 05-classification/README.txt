Team members:
Name1: XIN LIN
UT ID: xl5224
CS ID: jimmylin

Name2: Aming Ni
UT ID: an23366
CS ID: ani91

Team members' contribution: 
    XIN LIN  50%
    Aming Ni  50%

===========================================================

Q1. 
    Besides, we use counter to restore commonly used counts, such as prior count
    and conditional count. Based on these two common counters, we compute for
    accuracy of the model trained by training set applying on validation set
    for each specific k. 

    Specifically, we have:
    1. compute prior probability p(label) and conditional probability
    p(feature|label), cache them in distinct Counter object
    2. compute predictions based on the pre-computed priors and conditionals
    3. compare predictions of validation data and its ground truth label
    4. compute the accuracy ratio

    After evaluation of all k values, we pick up the one with highest accuracy
    and select its environmental settings as field of that object, such as
    prior probs and conditional probs.

Q2. 
    In this question, we simply loop through all features in self.features to
    compute odds ratio of each feature in terms of label1 and label2.

    In addition to that, the data structure we use is priority queue. Note
    that we use negative odds ratio as the priority when (feature, odd_ratio)
    is pushed into the queue.


Q3. 
