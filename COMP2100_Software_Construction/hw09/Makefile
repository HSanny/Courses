# COMP2100 HW09 MAKEFILE
# Jimmy Lin <u5223173@anu.edu.au> 

#######################################################################
# DO NOT EDIT THIS MAKEFILE UNLESS YOU KNOW WHAT YOU ARE DOING. 
#######################################################################	

all:
	javac -verbose ./Breadth.java -d ./bin/
	javac -verbose ./types/*.java -d ./bin/
	javac -verbose ./visitors/*.java -d ./bin/
	javac -verbose ./utilities/*.java -d ./bin/

test:
	cd bin && java Breadth < ./testFile.txt

pdf:
	pdflatex README.tex
	pdflatex README.tex
	rm -f README.aux README.aux README.toc README.log README.out

clean:
	rm -f ./*.class
	rm -f ./bin/*.class
	rm -f ./types/*.class
	rm -f ./visitors/*.class
	rm -f ./utilities/*.class
