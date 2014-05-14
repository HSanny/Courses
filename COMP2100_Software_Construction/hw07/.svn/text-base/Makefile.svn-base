# COMP2100 homework 07 MAKEFILE
# Jimmy Lin <u5223173@anu.edu.au> 

#######################################################################
# DO NOT EDIT THIS MAKEFILE UNLESS YOU KNOW WHAT YOU ARE DOING. 
#######################################################################	

HW07_PATH = $(shell pwd)

all: compile

compile:
	javac -verbose ./Paragrapher.java

run:
	java -verbose Paragrapher ./inputText.txt

input:
	cat inputText.txt

clean:
	rm -f ./*.class
