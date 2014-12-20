NAME: JIMMY LIN
UTEID: xl5224
==================================================================
||                HOMEWORK 01: SOM AND LVQ REPORT               ||
==================================================================

part 1a. I generate two network that approximates Peano Graph. One is large
and the other is slightly smaller. 

Q. How close to a Peano curve can you get? Are you convinced that it will
actually get there if you keep adding units?

A: Both of two networks decently approximate to Peano graph and occupy the
whole square as much as possible. But obviously, the one with more unit
(simulation file as "1dsimu_large") is closer to Peano and occupies larger
area inside the square. I would convince that if more units are added, and
parameters are adjusted appropriately, the network derived will be closer to
Peano Graph.

==================================================================
part 1b. I generate one 2-d network that does reflect the non-uniform
distribution. see the simulation file in "2dsimu".

Q. How well can the map approximate a nonuniform distribution? How many units
would you need to have to get a good approximation?

A: The density of the mesh does reveal the probabilistic density of the
non-uniform distribution. In x-component, the mesh does reflect a normal
distribution centered in the middle. And in y-component, the mesh also
reflects two normal distribution centers. To get a good approximation of the
distribution, I employed 225 (15x15) units.

==================================================================
part 2. In the task from now on, I employed two set of images:
   - 10 images created by myself in the help of digit program
   - about 180 images restored in digit-test.pat

Q. Can you see a topological order on the map? What relations are
preserved and what are lost? Is it easy to draw the lines that separate the
classes? 
A: It is obvious that all digits with similar figure are gathered in the same
region of the map. This also indicates the continuity of units on the map.
(Close unit captures close nature of patterns.) In addition to that, most units
wins only one pattern or none and then the whole self-organizing map became
separable. This may be because we make use of map with appropriate size (4
times of number of input patterns). 

Q. In case you have more examples of certain types of input patterns, do
you see a magnifying effect? When you are done with training/testing, try add
more training examples. Do you get better performance?
A: Compare those two maps I uploaded, it is obvious that there is a magnifying
effect. Each type of input patterns occupies one particular region, not just
one particular unit anymore. Note that the part2_map_test uses images in
digit-test.pat, where there are multiple examples for each type of input
pattern.

==================================================================
part 3a. Classfication. 

Q. Now use the test patterns from digit-test.pat as input: how many of
them are classified correctly?

A: 27.5% of images in testvectors are correctly classified if the train data
created by myself (10 images in toal) is used.  37.5% of images in testvectors
are correctly classified if the images restored in digit-test.pat is used for
training. In this sense, we can conclude that larger training data would
contribute to the classfication performance. 

Remark: of course, the quality of image bitmap and label for training does
matter a lot for the performance on classfication. Note that above conclusion
is just an intuitive statement.

==================================================================
part 3b. LVQ

Q. How does the map change? Test the classification again with the test input
set. 

A: For self-created train image set, the map changes in the sense that some
units switch to be the winner of different type of patterns, as shown on the
maps I uploaded. For "digit-test.pat" image set, the map does not even change.
Every units recognizes the same type of patterns, comparing to the map
generated from part3a.  

Q. How does the performance compare to the self-organizing map of Part 3a?
(If not, why?)
A: Unfortunately, for both cases of using different training data, the
performance remained unchanged. The possible reasons for no improvement are as
follows:
    - the testing data (testvectors) contains errorneous bitmap/label pair.
      (bad testing)
    - the training data contains errorneous bitmap/label pair (bad
      supervision.)
    - dimensionality reduction removes the room for possible improvement. 
