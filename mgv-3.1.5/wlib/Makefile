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

# @(#)$Mu: wlib/Makefile.am 1.16 1998/09/01 20:49:43 $


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

MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libwlib.a
libwlib_a_SOURCES = \
	WlAppShell.c WlSplashScreen.c WlToolBar.c about.c appclass.c \
	callback.c chkver.c create.c dismiss.c error.c fgbg.c find.c findup.c \
	focus.c free_pixmap.c help-html.c help-libhelp.c help.c icons.c \
	init.c log.c popdown_dialog.c popup_dialog.c scrollbar.c shell.c \
	strings.c togglesync.c top.c version.c xbm.c xpm.c

INCLUDES = -I.
mkinstalldirs = $(SHELL) $(top_srcdir)/tools/mkinstalldirs
CONFIG_CLEAN_FILES = 
LIBRARIES =  $(noinst_LIBRARIES)


DEFS =  -DPACKAGE=\"mgv\" -DVERSION=\"3.1.5\" -DHAVE_PUTENV=0 -DUSE_DMALLOC=0 -DHAVE_XPM=1 -DHAVE_X11_XPM_H=1 -DHAVE_MOTIF=1 -DHAVE_LIBHELP=0 -DHAVE_EDITRES=1  -I. -I$(srcdir) 
CPPFLAGS =  
LDFLAGS = -s  
LIBS =  
X_CFLAGS = 
X_EXTRA_LIBS = 
libwlib_a_LIBADD = 
libwlib_a_OBJECTS =  WlAppShell.o WlSplashScreen.o WlToolBar.o about.o \
appclass.o callback.o chkver.o create.o dismiss.o error.o fgbg.o find.o \
findup.o focus.o free_pixmap.o help-html.o help-libhelp.o help.o \
icons.o init.o log.o popdown_dialog.o popup_dialog.o scrollbar.o \
shell.o strings.o togglesync.o top.o version.o xbm.o xpm.o
AR = ar
CFLAGS = -O2  -D_POSIX_SOURCE 
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
LINK = $(CC) $(CFLAGS) $(LDFLAGS) -o $@
DIST_COMMON =  COPYING.LIB Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
SOURCES = $(libwlib_a_SOURCES)
OBJECTS = $(libwlib_a_OBJECTS)

all: Makefile $(LIBRARIES)

.SUFFIXES:
.SUFFIXES: .S .c .o .s
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps wlib/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-noinstLIBRARIES:

clean-noinstLIBRARIES:
	-test -z "$(noinst_LIBRARIES)" || rm -f $(noinst_LIBRARIES)

distclean-noinstLIBRARIES:

maintainer-clean-noinstLIBRARIES:

.c.o:
	$(COMPILE) -c $<

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

libwlib.a: $(libwlib_a_OBJECTS) $(libwlib_a_DEPENDENCIES)
	-rm -f libwlib.a
	$(AR) cru libwlib.a $(libwlib_a_OBJECTS) $(libwlib_a_LIBADD)
	$(RANLIB) libwlib.a

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $(SOURCES) $(HEADERS) $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = wlib

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

install-data: 
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: 

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:


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
mostlyclean:  mostlyclean-noinstLIBRARIES mostlyclean-compile \
		mostlyclean-tags mostlyclean-generic

clean:  clean-noinstLIBRARIES clean-compile clean-tags clean-generic \
		mostlyclean

distclean:  distclean-noinstLIBRARIES distclean-compile distclean-tags \
		distclean-generic clean
	-rm -f config.status

maintainer-clean:  maintainer-clean-noinstLIBRARIES \
		maintainer-clean-compile maintainer-clean-tags \
		maintainer-clean-generic distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: mostlyclean-noinstLIBRARIES distclean-noinstLIBRARIES \
clean-noinstLIBRARIES maintainer-clean-noinstLIBRARIES \
mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile tags mostlyclean-tags distclean-tags \
clean-tags maintainer-clean-tags distdir info dvi installcheck \
install-exec install-data install uninstall all installdirs \
mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


WlAppShell.o: wlib/WlAppShell.h wlib/WlAppShellP.h wlib/rcs.h wlib/stdlib.h
WlSplashScreen.o: wlib/WlSplashScreen.h wlib/WlSplashScreenP.h wlib/rcs.h
WlSplashScreen.o: wlib/wlibP.h
WlToolBar.o: wlib/WlToolBar-iunknown.xbm wlib/WlToolBar-iunknown.xpm
WlToolBar.o: wlib/WlToolBar-unknown.xbm wlib/WlToolBar-unknown.xpm
WlToolBar.o: wlib/WlToolBar.h wlib/WlToolBarP.h wlib/rcs.h wlib/sanity.h
WlToolBar.o: wlib/stdlib.h wlib/wlibP.h
about.o: wlib/about.h wlib/rcs.h wlib/sanity.h wlib/stdlib.h wlib/strdefs.h
about.o: wlib/wlib.h
appclass.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h
callback.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h
chkver.o: wlib/build.h wlib/rcs.h wlib/strdefs.h wlib/wlib.h wlib/wlibP.h
create.o: wlib/rcs.h wlib/sanity.h wlib/stdlib.h wlib/strdefs.h wlib/wlib.h
dismiss.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h
error.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h
fgbg.o: wlib/rcs.h wlib/sanity.h wlib/wlibP.h
find.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h
findup.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h
focus.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h
free_pixmap.o: wlib/rcs.h wlib/sanity.h
help-html.o: wlib/rcs.h wlib/sanity.h wlib/stdlib.h wlib/strdefs.h wlib/wlib.h
help-html.o: wlib/wlibP.h
help-libhelp.o: wlib/rcs.h wlib/stdlib.h wlib/strdefs.h wlib/wlib.h
help-libhelp.o: wlib/wlibP.h
help.o: wlib/rcs.h wlib/stdlib.h wlib/strdefs.h wlib/wlib.h wlib/wlibP.h
icons.o: wlib/icons.h wlib/rcs.h wlib/sanity.h wlib/strdefs.h wlib/wlib.h
icons.o: wlib/wlibP.h
init.o: wlib/rcs.h wlib/stdlib.h wlib/strdefs.h wlib/wlib.h wlib/wlibP.h
log.o: wlib/log.h wlib/rcs.h wlib/sanity.h wlib/stdlib.h wlib/strdefs.h
log.o: wlib/typesanity.h wlib/wlib.h wlib/wlibP.h
popdown_dialog.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h
popup_dialog.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h
scrollbar.o: wlib/rcs.h wlib/sanity.h wlib/strdefs.h wlib/wlib.h
shell.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h
strings.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h wlib/wlibP.h
togglesync.o: wlib/rcs.h wlib/strdefs.h wlib/typesanity.h wlib/wlib.h
top.o: wlib/rcs.h wlib/strdefs.h wlib/wlib.h
version.o: wlib/build.h wlib/rcs.h wlib/stdlib.h wlib/strdefs.h wlib/wlib.h
xbm.o: wlib/rcs.h wlib/sanity.h wlib/wlibP.h
xpm.o: wlib/rcs.h wlib/sanity.h wlib/strdefs.h wlib/wlib.h wlib/wlibP.h

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
