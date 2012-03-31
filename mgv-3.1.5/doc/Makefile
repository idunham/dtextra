# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.3 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

# @(#)$Mu: mgv/doc/Makefile.am 1.24 1998/09/06 01:09:16 $


SHELL = /bin/sh

srcdir = .
top_srcdir = ..
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DISTDIR =

pkgdatadir = $(datadir)/mgv
pkglibdir = $(libdir)/mgv
pkgincludedir = $(includedir)/mgv

top_builddir = ..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
CC = gcc
EDITRES_LIBS = -lXmu
LIBHELP_LIBS = 
MAKEINFO = makeinfo
PACKAGE = mgv
RANLIB = ranlib
VERSION = 3.1.5
X11_LIBS = -lX11
XEXT_LIBS = -lXext
XM_LIBS = -lXm
XPM_LIBS = -lXpm
XT_LIBS = -lXt
X_LIBS = 
X_PRE_LIBS =  -lSM -lICE

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
mkinstalldirs = $(SHELL) $(top_srcdir)/tools/mkinstalldirs
CONFIG_CLEAN_FILES = 
DATA =  $(help_DATA)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
all: Makefile $(DATA)

.SUFFIXES:
.SUFFIXES: .eps .gif .ps .sgml .sgml-in
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps doc/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


install-helpDATA: $(help_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(helpdir)
	@list='$(help_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(helpdir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(helpdir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(helpdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(helpdir)/$$p; \
	  fi; fi; \
	done

uninstall-helpDATA:
	@$(NORMAL_UNINSTALL)
	list='$(help_DATA)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(helpdir)/$$p; \
	done
tags: TAGS
TAGS:


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = doc

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
	done
info:
dvi:
check: all
	$(MAKE)
installcheck:
install-exec: 
	@$(NORMAL_INSTALL)

install-data: install-helpDATA
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-helpDATA

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(DATADIR)$(helpdir)


mostlyclean-generic:
	-test -z "$(MOSTLYCLEANFILES)" || rm -f $(MOSTLYCLEANFILES)

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-rm -f Makefile $(DISTCLEANFILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	-test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
	-test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
mostlyclean:  mostlyclean-generic

clean:  clean-generic mostlyclean

distclean:  distclean-generic clean
	-rm -f config.status

maintainer-clean:  maintainer-clean-generic distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: uninstall-helpDATA install-helpDATA tags distdir info dvi \
installcheck install-exec install-data install uninstall all \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


#.gif.eps:
#	giftopnm $< | ppmtopgm | pnmtops -noturn > $@ || (rm $@; exit 1)

#.sgml-in.sgml:
#	../tools/sgmlpp '$(VERSION)' < $< > $@ || (rm -f $@; exit 1)

#all: mgv.ps mgv-a4.ps

#mgv.ps: mgv.sgml $(mu_epsfiles)
#	sgml2latex --papersize=letter --output=ps --tabsize=8 mgv.sgml

#mgv-a4.ps: mgv.sgml $(mu_epsfiles)
#	cp mgv.sgml mgv-a4.sgml
#	sgml2latex --papersize=a4 --output=ps --tabsize=8 mgv-a4.sgml
#	rm mgv-a4.sgml

#mgv.html: mgv.sgml
#	sgml2html --charset=latin --imagebuttons --split=1 --tabsize=8 $<

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
