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

# @(#)$Mu: mgv/lib/toolbar/Makefile.am 1.6 1998/08/29 00:01:53 $


SHELL = /bin/sh

srcdir = .
top_srcdir = ../..
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

top_builddir = ../..

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

MAINTAINERCLEANFILES = Makefile.in

mu_bitmaps = \
	fileOpen-i.xbm fileOpen.xbm \
	filePrint-i.xbm filePrint.xbm \
	helpContext-i.xbm helpContext.xbm \
	magstepDecr-i.xbm magstepDecr.xbm \
	magstepIncr-i.xbm magstepIncr.xbm \
	mark-i.xbm mark.xbm \
	markAll-i.xbm markAll.xbm \
	markEven-i.xbm markEven.xbm \
	markNone-i.xbm markNone.xbm \
	markOdd-i.xbm markOdd.xbm \
	pageNext-i.xbm pageNext.xbm \
	pagePrev-i.xbm pagePrev.xbm

mu_pixmaps = \
	fileOpen-i.xpm fileOpen.xpm \
	filePrint-i.xpm filePrint.xpm \
	helpContext-i.xpm helpContext.xpm \
	magstepDecr-i.xpm magstepDecr.xpm \
	magstepIncr-i.xpm magstepIncr.xpm \
	mark-i.xpm mark.xpm \
	markAll-i.xpm markAll.xpm \
	markEven-i.xpm markEven.xpm \
	markNone-i.xpm markNone.xpm \
	markOdd-i.xpm markOdd.xpm \
	pageNext-i.xpm pageNext.xpm \
	pagePrev-i.xpm pagePrev.xpm

toolbardir = $(pkgdatadir)/$(VERSION)/toolbar
toolbar_DATA = $(mu_bitmaps) $(mu_pixmaps)
mkinstalldirs = $(SHELL) $(top_srcdir)/tools/mkinstalldirs
CONFIG_CLEAN_FILES = 
DATA =  $(toolbar_DATA)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
all: Makefile $(DATA)

.SUFFIXES:
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps lib/toolbar/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


install-toolbarDATA: $(toolbar_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(toolbardir)
	@list='$(toolbar_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(toolbardir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(toolbardir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(toolbardir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(toolbardir)/$$p; \
	  fi; fi; \
	done

uninstall-toolbarDATA:
	@$(NORMAL_UNINSTALL)
	list='$(toolbar_DATA)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(toolbardir)/$$p; \
	done
tags: TAGS
TAGS:


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = lib/toolbar

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

install-data: install-toolbarDATA
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-toolbarDATA

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(DATADIR)$(toolbardir)


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

.PHONY: uninstall-toolbarDATA install-toolbarDATA tags distdir info dvi \
installcheck install-exec install-data install uninstall all \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
