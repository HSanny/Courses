echo "COMP2300 assignment 02 screen display on rPeANUt . Please use rPeANUt 2.3 version since that is default version of this test.  Results are presented in the testResult directory. "
echo "Author: Jimmy Lin (u5223173)"
mkdir testImage
mkdir testResult
for MODE in B X R
do
    for i in {0..9}
    do 
        echo "Processing ./testImage/test$i.png.$MODE..." 
        echo "Processing ./testImage/test$i.png.$MODE..." >> ./testResult/test$i.txt
    java -jar rPeANUt2.3.jar imageReader.s -dump < ./testImage/test$i.png.$MODE | diff -s - ./testImage/test$i.png.dmp >> ./testResult/test$i.txt
done
done
