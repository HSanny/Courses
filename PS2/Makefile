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
	git commit -m "more on personal statement"
	git push

clean:
	rm -f *.aux *.log *.snm *.out texput.log *.toc *.lot *.lof *.nav
