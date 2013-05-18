#!/bin/bash
#### test for each different output ... 

### first of all, test filtering functionality 
mkdir test_filtered 
for threshold in 0.01 0.05 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0
do
./bmpedit -t $threshold -o ./test_filtered/filtered_women_$threshold.bmp ./Amar.bmp    
./bmpedit -t $threshold -o ./test_filtered/filtered_helicopter_$threshold.bmp ./ah64longbow.bmp 
done

### secondly, test cropping functionality 
## take width of output file as variant 
mkdir test_cropped 
for step in 50 100 150 200 300 500 700 900
do
./bmpedit -c $step 300 100 100 -o ./test_cropped/cropped_women_width$step.bmp ./Amar.bmp
./bmpedit -c $step 300 100 100 -o ./test_cropped/cropped_helicopter_width$step.bmp ./ah64longbow.bmp
done 
## take height of output file as variant
for step in 50 100 150 200 300 500 700 900
do
./bmpedit -c 150 $step 100 100 -o ./test_cropped/cropped_helicopter_height$step.bmp ./ah64longbow.bmp   
./bmpedit -c 150 $step 100 100 -o ./test_cropped/cropped_women_height$step.bmp ./Amar.bmp   
done
 
## take x-coordinate of leftmost remained pixel
for step in 50 100 150 200 300 500 700 900
do
./bmpedit -c 150 150 $step 100  -o ./test_cropped/cropped_helicopter_x$step.bmp ./ah64longbow.bmp   
./bmpedit -c 150 150 $step 100  -o ./test_cropped/cropped_women_x$step.bmp ./Amar.bmp   
done 
## take y-coordinate of downmost remained pixel
for step in 50 100 150 200 300 500 700 900
do
./bmpedit -c 150 150 50 $step -o ./test_cropped/cropped_helicopter_y$step.bmp ./ah64longbow.bmp   
./bmpedit -c 150 150 50 $step -o ./test_cropped/cropped_women_y$step.bmp ./Amar.bmp   
done 

## thirdly, test rotation
mkdir test_rotation 
for DEGREE in -360 -270 -180 -90 0 90 180 270 360 540 720 1080
do
./bmpedit -r $DEGREE -o ./test_rotation/rotate_helicopter_$DEGREE.bmp ./ah64longbow.bmp
./bmpedit -r $DEGREE -o ./test_rotation/rotate_women_$DEGREE.bmp ./Amar.bmp 
done
## test for no exception handling
./bmpedit -r -10 -o ./test_rotation/rotate10.bmp ./Amar.bmp 
./bmpedit -r -10.5 -o ./test_rotation/rotate11.bmp ./Amar.bmp 

## test for bluring
mkdir test_bluring
for WINDOWSIZE in 3 5 7 10 12 15 20
do
./bmpedit -o ./test_bluring/blur_helicopter_$WINDOWSIZE.bmp -b $WINDOWSIZE ./ah64longbow.bmp
./bmpedit -o ./test_bluring/blur_women_$WINDOWSIZE.bmp -b $WINDOWSIZE ./Amar.bmp
done

## finally, test the combination of filtering and cropping
mkdir test_combo 
# ordering of parameters is changeable .. 
./bmpedit -t 0.3 -c 100 100 100 200 -o ./test_combo/01.bmp ./Amar.bmp  
./bmpedit -c 100 100 100 100 -t 0.4 -o ./test_combo/02.bmp ./Amar.bmp   
./bmpedit -c 500 300 100 100 -o ./test_combo/03.bmp -t 0.2 ./Amar.bmp   

# -o parameter can be default (check it on the root directory) 
./bmpedit -c 500 300 100 100 -t 0.2 ./Amar.bmp  ## ./result.bmp
printf "\nThe Content of README file..\n"
cat ./README.txt
