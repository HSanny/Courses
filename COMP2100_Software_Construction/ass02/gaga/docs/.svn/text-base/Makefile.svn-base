# COMP2100 documentaiton MAKEFILE
# Jimmy Lin <u5223173@anu.edu.au> 

#######################################################################
# DO NOT EDIT THIS MAKEFILE UNLESS YOU KNOW WHAT YOU ARE DOING. 
#######################################################################	

all: pdf clean

pdf:
	pdflatex documentation.tex
	pdflatex documentation.tex

run: pdf
	open documentaiton.pdf
	
clean:
	rm -f -v documentation.aux documentation.log documentation.out documentation.toc
