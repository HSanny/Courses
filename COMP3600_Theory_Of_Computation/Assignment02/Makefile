# COMP3630 assignment 02 MAKEFILE
# Jimmy Lin <u5223173@anu.edu.au> 

#######################################################################
# DO NOT EDIT THIS MAKEFILE UNLESS YOU KNOW WHAT YOU ARE DOING. 
#######################################################################	

COMP4670_ASS01_PATH = $(shell pwd)

all: pdf cleanOthers

pdf: ass2.tex
	pdflatex ass2.tex
	pdflatex ass2.tex

cleanOthers:
	rm -f ass2.aux ass2.lof ass2.log ass2.out ass2.toc ass2.lot q.log texput.log

clean:
	rm -f ass2.aux ass2.lof ass2.log ass2.out ass2.toc ass2.lot q.log texput.log ass2.pdf
