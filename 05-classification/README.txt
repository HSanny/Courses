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
    For this part, we classify the set of training data one by one and update
    the weight of involved labels according to the given documentations.

Q4. 
    In this question, we simply loop through all features in self.features to
    compute difference of weight for each feature in terms of input label.

    In addition to that, the data structure we use is priority queue. Note
    that we use negative odds ratio as the priority when (feature) is pushed
    into the queue.

Q5.
    In this question, we have two phases to evaluate the accuracy led by C,
    the training phase and validation phase.

    During the training phase, we iterate through the training data for
    max_iteration times. Each time iteration simulates the operations we apply
    in Perceptron training. It is online learning (one entity updates model).

    At the phase of validation, we evaluate the accuracy of the model derived
    in the training phase as we do in naivebayes model.

Q6. 
    In this question, we need to design new featuers in order to improve the
    accuracy to 84% for testing set. 

    Specifically, what we do is as follows:
        For each pixel, we compute the gradient from every direction 
        The gradient is characterized by -1, 0, +1.
    
    In summary, the feature we created is described as at a position (x,y),
    the category of that gradient at direction left, right, up, or down. For
    example, 

            features[(x, y, 'left', +1)] = 1 or 0

    By doing the feature extraction above, the validation accuracy improves to
    88% and testing accuracy improves to 84%.

Minicontest.

   data:           digits
   classifier:             minicontest
   using minicontest feature extractor
   training set size:      5000
   Extracting features...
   Training...
   Validating...
   840 correct out of 1000 (84.0%).
   Testing...
   807 correct out of 1000 (80.7%).

