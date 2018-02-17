##                           -*- Mode: Makefile -*- 
## Makefile --- 
## Author          : Marcel Turcotte
## Created On      : Tue Jul 12 10:25:22 2005
## Last Modified By: Marcel Turcotte
## Last Modified On: Fri Jul 29 15:51:21 2005
##
## This copyrighted source code is freely distributed under the terms
## of the GNU General Public License. 
## See the files COPYRIGHT and LICENSE for details.
                                                                               
all: refman.pdf refman.ps refman.html

refman.pdf: refman.tex
	pdflatex refman
	bibtex refman
	pdflatex refman
	pdflatex refman

refman.dvi: refman.tex
	latex refman.tex
	bibtex refman
	latex refman
	latex refman

refman.ps: refman.dvi
	dvips -o refman.ps refman.dvi

refman.html: refman.dvi
	htlatex refman

clean:
	rm -f *.4ct *.4tc *.aux *.idv *.lg *.log *.dvi
	rm -f *.tmp *.xref *~ texutil.tuf *.bbl *.blg

ultraclean:
	make clean
	rm -f refman.html refman.css refman.pdf refman.dvi refman.ps 

