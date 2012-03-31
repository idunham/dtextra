# @(#)$Mu: mgv/doc/Makefile.am 1.24 1998/09/06 01:09:16 $
##
## Makefile.am
##	automake file for mgv/doc.
##
## Copyright (C) 1998 Eric A. Howe
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
##   Authors:	Eric A. Howe (mu@trends.net)
##

mu_epsfiles = file-info.eps file-open.eps file-print.eps file-save.eps \
	gs-log.eps help-about.eps help-bldopts.eps main-zoom.eps \
	mainwindow.eps

MAINTAINERCLEANFILES = *.ps *.html *.sgml Makefile.in *.eps
DISTCLEANFILES = mgv.ps mgv-a4.ps $(mu_epsfiles)

mu_htmlchunks = mgv-1.html mgv-2.html mgv-3.html mgv-4.html mgv-5.html \
	mgv-6.html mgv-7.html mgv-8.html mgv-9.html mgv-10.html \
	mgv-11.html mgv-12.html mgv-13.html mgv-14.html mgv-15.html \
	mgv-16.html mgv-17.html mgv-18.html mgv-19.html mgv-20.html \
	mgv-21.html mgv-22.html mgv-23.html mgv-24.html mgv-25.html \
	mgv-26.html mgv-27.html mgv-28.html mgv-29.html mgv-30.html

helpdir = $(pkgdatadir)/$(VERSION)/help
help_DATA = mgv.html $(mu_htmlchunks) *.gif

SUFFIXES = .gif .sgml-in .sgml .ps .eps

##
## Maintainer only things.
##
if MU_MAINTAINER

.gif.eps:
	giftopnm $< | ppmtopgm | pnmtops -noturn > $@ || (rm $@; exit 1)

.sgml-in.sgml:
	../tools/sgmlpp '$(VERSION)' < $< > $@ || (rm -f $@; exit 1)

all: mgv.ps mgv-a4.ps

mgv.ps: mgv.sgml $(mu_epsfiles)
	sgml2latex --papersize=letter --output=ps --tabsize=8 mgv.sgml

mgv-a4.ps: mgv.sgml $(mu_epsfiles)
	cp mgv.sgml mgv-a4.sgml
	sgml2latex --papersize=a4 --output=ps --tabsize=8 mgv-a4.sgml
	rm mgv-a4.sgml

mgv.html: mgv.sgml
	sgml2html --charset=latin --imagebuttons --split=1 --tabsize=8 $<

endif
