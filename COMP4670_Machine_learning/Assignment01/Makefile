# COMP4670 assignment 01 MAKEFILE
# Jimmy Lin <u5223173@anu.edu.au> 

#######################################################################
# DO NOT EDIT THIS MAKEFILE UNLESS YOU KNOW WHAT YOU ARE DOING. 
#######################################################################	

COMP4670_ASS01_PATH = $(shell pwd)

all: pdf cleanOthers

pdf: ass01.tex
	pdflatex ass01.tex
	pdflatex ass01.tex

cleanOthers:
	rm -f ass01.aux ass01.lof ass01.log ass01.out ass01.toc ass01.lot q.log texput.log

clean:
	rm -f ass01.aux ass01.lof ass01.log ass01.out ass01.toc ass01.lot q.log texput.log ass01.pdf
