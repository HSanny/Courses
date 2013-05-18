# COMP3630 assignment 03 MAKEFILE
# Jimmy Lin <u5223173@anu.edu.au> 

#######################################################################
# DO NOT EDIT THIS MAKEFILE UNLESS YOU KNOW WHAT YOU ARE DOING. 
#######################################################################	

COMP4670_ASS01_PATH = $(shell pwd)

all: pdf 

pdf: ass3.tex
	pdflatex ass3.tex

full:
	pdflatex ass3.tex
	pdflatex ass3.tex
	rm -f ass3.aux ass3.lof ass3.log ass3.out ass3.toc ass3.lot q.log texput.log
clean:
	rm -f ass3.aux ass3.lof ass3.log ass3.out ass3.toc ass3.lot q.log texput.log
