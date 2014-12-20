# Report for COMP2550 MAKEFILE
# Jimmy Lin <u5223173@anu.edu.au> 

#######################################################################
# DO NOT EDIT THIS MAKEFILE UNLESS YOU KNOW WHAT YOU ARE DOING. 
#######################################################################	

all:
	pdflatex main
	biblatex main
	pdflatex main
	pdflatex main

push:
	git add main.tex main.pdf Makefile
	git commit -m "homework update"
	git push

clean:
	rm -f *.aux *.log *.snm *.out texput.log *.toc *.lot *.lof *.nav
