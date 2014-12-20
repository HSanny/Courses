# Report for CS394N MAKEFILE
# Jimmy Lin <jimmylin@utexas.edu> 

#######################################################################
# DO NOT EDIT THIS MAKEFILE UNLESS YOU KNOW WHAT YOU ARE DOING. 
#######################################################################	

all:
	pdflatex main
	biblatex main
	pdflatex main
	pdflatex main

push:
	git add Makefile main.tex main.pdf
	git commit -m "more on report"
	git push

clean:
	rm -f *.aux *.log *.snm *.out texput.log *.toc *.lot *.lof *.nav
