# COMP2100 Assignment 02 MAKEFILE
# Jimmy Lin <u5223173@anu.edu.au> 

#######################################################################
# DO NOT EDIT THIS MAKEFILE UNLESS YOU KNOW WHAT YOU ARE DOING. 
#######################################################################	

GAGA_PATH = $(shell pwd)
all:
	javac -verbose ./*.java
	javac -verbose ./types/paths/*.java
	for i in $(shell ls -p | grep "/") ; do javac -verbose ./$${i}*.java ; done

run:
	java Gaga

ls:
	ls -v *.java

lsall:
	for i in $(shell ls -p | grep "/") ; do cd "$${i}" && pwd && MAKE ls  && cd "${GAGA_PATH}"; done

lsdir:
	ls -p | grep "/"

initMakefile:
	for i in $(shell ls -p | grep "/") ; do cd "$${i}" && touch "Makefile" && cd ".."; done

clean:
	rm -f -v ./*.class
	for i in $(shell ls -p | grep "/") ; do cd "$${i}" && make clean && cd "${GAGA_PATH}"; done	
	
