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

# @(#)$Mu: mgv/Makefile.am 1.43.8.1 1998/09/28 09:46:33 $


SHELL = /bin/sh

srcdir = .
top_srcdir = .
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

top_builddir = .

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

bin_PROGRAMS = mgv
mgv_SOURCES  = \
	Ghostview.c app.c auto.c center.c dsc.c events.c file.c help.c \
	log.c magstep.c main.c mark.c media.c mgv.c mgv_create.c options.c \
	orientation.c page.c page_control.c print.c process.c scroll.c sens.c \
	strings.c toolbar.c track.c uiutil.c unmangle.c util.c view.c zoom.c \
	_ad.c _switch_help.c
mgv_LDADD = _bld.o -Lwlib -lwlib \
	 -lXm -lXmu -lXt -lXpm \
	-lXext -lX11

mgvresdir = $(pkgdatadir)/$(VERSION)
mgvres_DATA = MGv

mgv_DEPENDENCIES = _bld.o wlib/libwlib.a

man_MANS = mgv.1

BUILD_SOURCE = _ad.c _bld.c _switch_help.c mgv.1 MGv

CLEANFILES = .been_warned
DISTCLEANFILES = _ad.c _bld.c mgv.1 MGv

INCLUDES = -I. -Iwlib

SUBDIRS = doc wlib lib

MAINTAINERCLEANFILES = Makefile.in MGv.ad _switch_help.c mgv.man res.m4
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
mkinstalldirs = $(SHELL) $(top_srcdir)/tools/mkinstalldirs
CONFIG_CLEAN_FILES = 
PROGRAMS =  $(bin_PROGRAMS)


DEFS =  -DPACKAGE=\"mgv\" -DVERSION=\"3.1.5\" -DHAVE_PUTENV=0 -DUSE_DMALLOC=0 -DHAVE_XPM=1 -DHAVE_X11_XPM_H=1 -DHAVE_MOTIF=1 -DHAVE_LIBHELP=0 -DHAVE_EDITRES=1  -I. -I$(srcdir) 
CPPFLAGS =  
LDFLAGS = -s  
LIBS =  
X_CFLAGS = 
X_EXTRA_LIBS = 
mgv_OBJECTS =  Ghostview.o app.o auto.o center.o dsc.o events.o file.o \
help.o log.o magstep.o main.o mark.o media.o mgv.o mgv_create.o \
options.o orientation.o page.o page_control.o print.o process.o \
scroll.o sens.o strings.o toolbar.o track.o uiutil.o unmangle.o util.o \
view.o zoom.o _ad.o _switch_help.o
mgv_LDFLAGS = 
CFLAGS = -O2  -D_POSIX_SOURCE 
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
LINK = $(CC) $(CFLAGS) $(LDFLAGS) -o $@
man1dir = $(mandir)/man1
MANS = $(man_MANS)

NROFF = nroff
DATA =  $(mgvres_DATA)

DIST_COMMON =  README AUTHORS COPYING ChangeLog INSTALL Makefile.am \
Makefile.in NEWS acinclude.m4 aclocal.m4 configure configure.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
SOURCES = $(mgv_SOURCES)
OBJECTS = $(mgv_OBJECTS)

all: all-recursive all-am

.SUFFIXES:
.SUFFIXES: .S .c .o .s
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$@ CONFIG_HEADERS= $(SHELL) ./config.status

$(ACLOCAL_M4):  configure.in  acinclude.m4
	cd $(srcdir) && $(ACLOCAL)

config.status: $(srcdir)/configure
	$(SHELL) ./config.status --recheck
$(srcdir)/configure: $(srcdir)/configure.in $(ACLOCAL_M4) $(CONFIGURE_DEPENDENCIES)
	cd $(srcdir) && $(AUTOCONF)

mostlyclean-binPROGRAMS:

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)

distclean-binPROGRAMS:

maintainer-clean-binPROGRAMS:

install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "  $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed '$(transform)'`"; \
	     $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed '$(transform)'`; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	list='$(bin_PROGRAMS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$p|sed '$(transform)'`; \
	done

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

mgv: $(mgv_OBJECTS) $(mgv_DEPENDENCIES)
	@rm -f mgv
	$(LINK) $(mgv_LDFLAGS) $(mgv_OBJECTS) $(mgv_LDADD) $(LIBS)

install-man1:
	$(mkinstalldirs) $(DESTDIR)$(man1dir)
	@list='$(man1_MANS)'; \
	l2='$(man_MANS)'; for i in $$l2; do \
	  case "$$i" in \
	    *.1*) list="$$list $$i" ;; \
	  esac; \
	done; \
	for i in $$list; do \
	  if test -f $(srcdir)/$$i; then file=$(srcdir)/$$i; \
	  else file=$$i; fi; \
	  ext=`echo $$i | sed -e 's/^.*\\.//'`; \
	  inst=`echo $$i | sed -e 's/\\.[0-9a-z]*$$//'`; \
	  inst=`echo $$inst | sed '$(transform)'`.$$ext; \
	  echo " $(INSTALL_DATA) $$file $(DESTDIR)$(man1dir)/$$inst"; \
	  $(INSTALL_DATA) $$file $(DESTDIR)$(man1dir)/$$inst; \
	done

uninstall-man1:
	@list='$(man1_MANS)'; \
	l2='$(man_MANS)'; for i in $$l2; do \
	  case "$$i" in \
	    *.1*) list="$$list $$i" ;; \
	  esac; \
	done; \
	for i in $$list; do \
	  ext=`echo $$i | sed -e 's/^.*\\.//'`; \
	  inst=`echo $$i | sed -e 's/\\.[0-9a-z]*$$//'`; \
	  inst=`echo $$inst | sed '$(transform)'`.$$ext; \
	  echo " rm -f $(DESTDIR)$(man1dir)/$$inst"; \
	  rm -f $(DESTDIR)$(man1dir)/$$inst; \
	done
install-man: $(MANS)
	@$(NORMAL_INSTALL)
	$(MAKE) install-man1
uninstall-man:
	@$(NORMAL_UNINSTALL)
	$(MAKE) uninstall-man1

install-mgvresDATA: $(mgvres_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(mgvresdir)
	@list='$(mgvres_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(mgvresdir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(mgvresdir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(mgvresdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(mgvresdir)/$$p; \
	  fi; fi; \
	done

uninstall-mgvresDATA:
	@$(NORMAL_UNINSTALL)
	list='$(mgvres_DATA)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(mgvresdir)/$$p; \
	done

# This directory's subdirectories are mostly independent; you can cd
# into them and run `make' without going through this Makefile.
# To change the values of `make' variables: instead of editing Makefiles,
# (1) if the variable is set in `config.status', edit `config.status'
#     (which will cause the Makefiles to be regenerated when you run `make');
# (2) otherwise, pass the desired values on the `make' command line.



all-recursive install-data-recursive install-exec-recursive \
installdirs-recursive install-recursive uninstall-recursive  \
check-recursive installcheck-recursive info-recursive dvi-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo "Making $$target in $$subdir"; \
	  (cd $$subdir && $(MAKE) $$target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"

mostlyclean-recursive clean-recursive distclean-recursive \
maintainer-clean-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	rev=''; list='$(SUBDIRS)'; for subdir in $$list; do \
	  rev="$$subdir $$rev"; \
	done; \
	for subdir in $$rev; do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo "Making $$target in $$subdir"; \
	  (cd $$subdir && $(MAKE) $$target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"
tags-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  (cd $$subdir && $(MAKE) tags); \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $(SOURCES) $(HEADERS) $(LISP)

TAGS: tags-recursive $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test -f $$subdir/TAGS && tags="$$tags -i $$here/$$subdir/TAGS"; \
	done; \
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

distdir = $(PACKAGE)-$(VERSION)
top_distdir = $(distdir)

# This target untars the dist file and tries a VPATH configuration.  Then
# it guarantees that the distribution is self-contained by making another
# tarfile.
distcheck: dist
	-rm -rf $(distdir)
	GZIP=$(GZIP) $(TAR) zxf $(distdir).tar.gz
	mkdir $(distdir)/=build
	mkdir $(distdir)/=inst
	dc_install_base=`cd $(distdir)/=inst && pwd`; \
	cd $(distdir)/=build \
	  && ../configure --srcdir=.. --prefix=$$dc_install_base \
	  && $(MAKE) \
	  && $(MAKE) dvi \
	  && $(MAKE) check \
	  && $(MAKE) install \
	  && $(MAKE) installcheck \
	  && $(MAKE) dist
	-rm -rf $(distdir)
	@echo "========================"; \
	echo "$(distdir).tar.gz is ready for distribution"; \
	echo "========================"
dist: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP) $(TAR) chozf $(distdir).tar.gz $(distdir)
	-rm -rf $(distdir)
dist-all: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP) $(TAR) chozf $(distdir).tar.gz $(distdir)
	-rm -rf $(distdir)
distdir: $(DISTFILES)
	-rm -rf $(distdir)
	mkdir $(distdir)
	-chmod 777 $(distdir)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
	done
	for subdir in $(SUBDIRS); do \
	  test -d $(distdir)/$$subdir \
	  || mkdir $(distdir)/$$subdir \
	  || exit 1; \
	  chmod 777 $(distdir)/$$subdir; \
	  (cd $$subdir && $(MAKE) top_distdir=../$(distdir) distdir=../$(distdir)/$$subdir distdir) \
	    || exit 1; \
	done
info: info-recursive
dvi: dvi-recursive
check: all-am
	$(MAKE) check-recursive
installcheck: installcheck-recursive
all-am: Makefile $(PROGRAMS) $(MANS) $(DATA)

install-exec-am: install-binPROGRAMS

install-data-am: install-man install-mgvresDATA

uninstall-am: uninstall-binPROGRAMS uninstall-man uninstall-mgvresDATA

install-exec: install-exec-recursive install-exec-am
	@$(NORMAL_INSTALL)

install-data: install-data-recursive install-data-am
	@$(NORMAL_INSTALL)

install: install-recursive install-exec-am install-data-am
	@:

uninstall: uninstall-recursive uninstall-am

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs: installdirs-recursive
	$(mkinstalldirs)  $(DATADIR)$(bindir) $(DESTDIR)$(mandir)/man1 \
		$(DATADIR)$(mgvresdir)


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
mostlyclean-am:  mostlyclean-binPROGRAMS mostlyclean-compile \
		mostlyclean-tags mostlyclean-generic

clean-am:  clean-binPROGRAMS clean-compile clean-tags clean-generic \
		mostlyclean-am

distclean-am:  distclean-binPROGRAMS distclean-compile distclean-tags \
		distclean-generic clean-am

maintainer-clean-am:  maintainer-clean-binPROGRAMS \
		maintainer-clean-compile maintainer-clean-tags \
		maintainer-clean-generic distclean-am

mostlyclean:  mostlyclean-recursive mostlyclean-am

clean:  clean-recursive clean-am

distclean:  distclean-recursive distclean-am
	-rm -f config.status

maintainer-clean:  maintainer-clean-recursive maintainer-clean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	-rm -f config.status

.PHONY: mostlyclean-binPROGRAMS distclean-binPROGRAMS clean-binPROGRAMS \
maintainer-clean-binPROGRAMS uninstall-binPROGRAMS install-binPROGRAMS \
mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile install-man1 uninstall-man1 install-man \
uninstall-man uninstall-mgvresDATA install-mgvresDATA \
install-data-recursive uninstall-data-recursive install-exec-recursive \
uninstall-exec-recursive installdirs-recursive uninstalldirs-recursive \
all-recursive check-recursive installcheck-recursive info-recursive \
dvi-recursive mostlyclean-recursive distclean-recursive clean-recursive \
maintainer-clean-recursive tags tags-recursive mostlyclean-tags \
distclean-tags clean-tags maintainer-clean-tags distdir info dvi \
installcheck all-am install-exec-am install-data-am uninstall-am \
install-exec install-data install uninstall all installdirs \
mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


_ad.c: MGv
	tools/ad2c -a bld_fallbacks MGv > _ad.c

MGv: MGv.ad
	sed -e 's;__DATADIR__;$(datadir)/mgv/$(VERSION);g' -e 's;__VERSION__;$(VERSION);g' < MGv.ad > MGv || (rm -f MGv; exit 1)

mgv.1: mgv.man
	sed -e 's;__VERSION__;$(VERSION);g' \
		-e 's;__DATADIR__;${prefix}/share/mgv/$(VERSION)/help;g' \
		< mgv.man > mgv.1 || (rm -f mgv.1; exit 1)

#res.m4: doc/mgv.sgml-in
#	tools/mkresm4 --prefix=mgv --output=res.m4 $<

#MGv.ad: MGv.ad.in res.m4 
#	m4 -P -D__CLS__=MGv -I. -Iwlib res.m4 MGv.ad.in > MGv.ad \
#		|| (rm -f MGv.ad; exit 1)

#mgv.man: mgv.man.in doc/mgv.sgml
#	tools/yanksect --section 'Command Line Switches' \
#		< doc/mgv.sgml > _switch_help.sgml
#	sgml2txt -f _switch_help.sgml
#	tools/txt2man mgv.man.in < _switch_help.txt > mgv.man
#	rm -f _switch_help.sgml _switch_help.txt

#_switch_help.c: doc/mgv.sgml
#	tools/yanksect --section 'Command Line Switches' \
#		< doc/mgv.sgml > _switch_help.sgml
#	sgml2txt -f _switch_help.sgml
#	tools/txt2swh < _switch_help.txt > _switch_help.c
#	rm -f _switch_help.sgml _switch_help.txt

_bld.c: $(mgv_OBJECTS) wlib/libwlib.a
	tools/mkver '$(VERSION)' '$(COMPILE)' '$(LDFLAGS)' '-lwlib  -lXm -lXt -lXpm -lXmu -lX11' '$(bindir)' '$(datadir)' > _bld.c || (rm -f _bld.c; exit 1)

all: .been-warned
.been-warned:
	@echo
	@echo "**********************************************************"
	@echo
	@echo "If you have installed a previous release of mgv,"
	@echo "you should remove the old library files (these"
	@echo "are probably in /usr/local/lib/mgv* or"
	@echo "/usr/local/share/mgv/<last-version>) after installing"
	@echo "this release.  The library file location for version 3+"
	@echo "has changed to match GNU autoconf standards. If you installed"
	@echo "a 1.x version of mgv, you should remove the application defaults"
	@echo "from /usr/X11/lib/X11/app-defaults (or where ever they are"
	@echo "on your system) or the mgv user interface will complain"
	@echo "and be very confused."
	@echo
	@echo "**********************************************************"
	@echo
	@touch .been-warned

Ghostview.o: mine/Ghostview.h mine/GhostviewP.h mine/util.h wlib/wlib/rcs.h
Ghostview.o: wlib/wlib/sanity.h wlib/wlib/stdlib.h
app.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/mark.h mine/mgv.h
app.o: mine/opaque.h mine/page.h mine/scroll.h mine/strdefs.h mine/strings.h
app.o: mine/track.h mine/util.h mine/zoom.h wlib/wlib/build.h wlib/wlib/rcs.h
app.o: wlib/wlib/sanity.h wlib/wlib/stdlib.h wlib/wlib/strdefs.h
app.o: wlib/wlib/wlib.h
auto.o: mine/Ghostview.h mine/auto.h mine/dsc.h mine/mgv.h mine/opaque.h
auto.o: wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/strdefs.h wlib/wlib/wlib.h
center.o: mine/Ghostview.h mine/center.h mine/dsc.h mine/mgv.h mine/opaque.h
center.o: wlib/wlib/rcs.h wlib/wlib/strdefs.h wlib/wlib/wlib.h
dsc.o: mine/dsc.h mine/util.h wlib/wlib/rcs.h wlib/wlib/stdlib.h
events.o: mine/Ghostview.h mine/dsc.h mine/events.h mine/mgv.h mine/opaque.h
events.o: wlib/wlib/rcs.h wlib/wlib/strdefs.h wlib/wlib/wlib.h
file.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/file.h mine/mgv.h
file.o: mine/opaque.h mine/page_control.h mine/process.h mine/sens.h
file.o: mine/strings.h mine/uiutil.h mine/util.h wlib/wlib/WlAppShell.h
file.o: wlib/wlib/rcs.h wlib/wlib/stdlib.h wlib/wlib/strdefs.h
file.o: wlib/wlib/typesanity.h wlib/wlib/wlib.h
help.o: mine/Ghostview.h mine/dsc.h mine/help.h mine/mgv.h mine/opaque.h
help.o: wlib/wlib/about.h wlib/wlib/build.h wlib/wlib/icons.h wlib/wlib/rcs.h
help.o: wlib/wlib/strdefs.h wlib/wlib/wlib.h
log.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/log.h mine/mgv.h
log.o: mine/opaque.h mine/sens.h mine/strings.h wlib/wlib/log.h wlib/wlib/rcs.h
log.o: wlib/wlib/strdefs.h wlib/wlib/wlib.h
magstep.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/magstep.h mine/mgv.h
magstep.o: mine/opaque.h wlib/wlib/rcs.h wlib/wlib/stdlib.h wlib/wlib/strdefs.h
magstep.o: wlib/wlib/typesanity.h wlib/wlib/wlib.h
main.o: lib/icon.xbm lib/icon.xpm lib/mask.xbm mine/Ghostview.h mine/dsc.h
main.o: mine/help.h mine/mgv.h mine/opaque.h mine/page_control.h mine/strdefs.h
main.o: mine/strings.h wlib/wlib/WlAppShell.h wlib/wlib/WlSplashScreen.h
main.o: wlib/wlib/WlToolBar.h wlib/wlib/build.h wlib/wlib/icons.h
main.o: wlib/wlib/rcs.h wlib/wlib/stdlib.h wlib/wlib/strdefs.h wlib/wlib/wlib.h
mark.o: mine/Ghostview.h mine/dsc.h mine/mark.h mine/mgv.h mine/opaque.h
mark.o: mine/uiutil.h wlib/wlib/rcs.h wlib/wlib/strdefs.h
mark.o: wlib/wlib/typesanity.h wlib/wlib/wlib.h
media.o: mine/Ghostview.h mine/dsc.h mine/media.h mine/mgv.h mine/opaque.h
media.o: wlib/wlib/rcs.h wlib/wlib/typesanity.h
mgv.o: mine/Ghostview.h mine/app.h mine/auto.h mine/dsc.h mine/log.h mine/mgv.h
mgv.o: mine/opaque.h mine/page_control.h mine/sens.h mine/strings.h
mgv.o: mine/uiutil.h mine/unmangle.h mine/zoom.h wlib/wlib/rcs.h
mgv.o: wlib/wlib/sanity.h wlib/wlib/stdlib.h wlib/wlib/strdefs.h
mgv.o: wlib/wlib/wlib.h
mgv_create.o: mine/Ghostview.h mine/app.h mine/auto.h mine/center.h mine/dsc.h
mgv_create.o: mine/events.h mine/file.h mine/help.h mine/log.h mine/magstep.h
mgv_create.o: mine/mark.h mine/media.h mine/mgv.h mine/opaque.h mine/options.h
mgv_create.o: mine/orientation.h mine/page.h mine/print.h mine/sens.h
mgv_create.o: mine/toolbar.h mine/track.h mine/view.h mine/zoom.h
mgv_create.o: wlib/wlib/icons.h wlib/wlib/rcs.h wlib/wlib/stdlib.h
mgv_create.o: wlib/wlib/strdefs.h wlib/wlib/wlib.h
options.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/mgv.h mine/opaque.h
options.o: mine/options.h wlib/wlib/rcs.h wlib/wlib/typesanity.h
orientation.o: mine/Ghostview.h mine/dsc.h mine/mgv.h mine/opaque.h
orientation.o: mine/orientation.h wlib/wlib/rcs.h wlib/wlib/typesanity.h
page.o: mine/Ghostview.h mine/app.h mine/auto.h mine/center.h mine/dsc.h
page.o: mine/log.h mine/mgv.h mine/opaque.h mine/page.h mine/page_control.h
page.o: mine/sens.h mine/uiutil.h wlib/wlib/rcs.h wlib/wlib/strdefs.h
page.o: wlib/wlib/typesanity.h wlib/wlib/wlib.h
page_control.o: mine/Ghostview.h mine/dsc.h mine/mgv.h mine/opaque.h
page_control.o: mine/page_control.h mine/sens.h wlib/wlib/rcs.h
page_control.o: wlib/wlib/sanity.h
print.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/log.h mine/mgv.h
print.o: mine/opaque.h mine/print.h mine/process.h mine/sens.h mine/strings.h
print.o: mine/uiutil.h mine/util.h wlib/wlib/rcs.h wlib/wlib/stdlib.h
print.o: wlib/wlib/strdefs.h wlib/wlib/wlib.h
process.o: mine/process.h wlib/wlib/rcs.h wlib/wlib/stdlib.h
process.o: wlib/wlib/strdefs.h wlib/wlib/wlib.h
scroll.o: mine/Ghostview.h mine/center.h mine/dsc.h mine/mgv.h mine/opaque.h
scroll.o: mine/page_control.h mine/scroll.h mine/sens.h mine/strings.h
scroll.o: mine/util.h wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/stdlib.h
scroll.o: wlib/wlib/strdefs.h wlib/wlib/wlib.h
sens.o: mine/Ghostview.h mine/dsc.h mine/mgv.h mine/opaque.h mine/sens.h
sens.o: wlib/wlib/rcs.h wlib/wlib/strdefs.h wlib/wlib/wlib.h
strings.o: mine/strdefs.h mine/strings.h wlib/wlib/rcs.h wlib/wlib/strdefs.h
strings.o: wlib/wlib/wlib.h
toolbar.o: lib/toolbar/fileOpen-i.xbm lib/toolbar/fileOpen-i.xpm
toolbar.o: lib/toolbar/fileOpen.xbm lib/toolbar/fileOpen.xpm
toolbar.o: lib/toolbar/filePrint-i.xbm lib/toolbar/filePrint-i.xpm
toolbar.o: lib/toolbar/filePrint.xbm lib/toolbar/filePrint.xpm
toolbar.o: lib/toolbar/helpContext-i.xbm lib/toolbar/helpContext-i.xpm
toolbar.o: lib/toolbar/helpContext.xbm lib/toolbar/helpContext.xpm
toolbar.o: lib/toolbar/magstepDecr-i.xbm lib/toolbar/magstepDecr-i.xpm
toolbar.o: lib/toolbar/magstepDecr.xbm lib/toolbar/magstepDecr.xpm
toolbar.o: lib/toolbar/magstepIncr-i.xbm lib/toolbar/magstepIncr-i.xpm
toolbar.o: lib/toolbar/magstepIncr.xbm lib/toolbar/magstepIncr.xpm
toolbar.o: lib/toolbar/mark-i.xbm lib/toolbar/mark-i.xpm lib/toolbar/mark.xbm
toolbar.o: lib/toolbar/mark.xpm lib/toolbar/markAll-i.xbm
toolbar.o: lib/toolbar/markAll-i.xpm lib/toolbar/markAll.xbm
toolbar.o: lib/toolbar/markAll.xpm lib/toolbar/markEven-i.xbm
toolbar.o: lib/toolbar/markEven-i.xpm lib/toolbar/markEven.xbm
toolbar.o: lib/toolbar/markEven.xpm lib/toolbar/markNone-i.xbm
toolbar.o: lib/toolbar/markNone-i.xpm lib/toolbar/markNone.xbm
toolbar.o: lib/toolbar/markNone.xpm lib/toolbar/markOdd-i.xbm
toolbar.o: lib/toolbar/markOdd-i.xpm lib/toolbar/markOdd.xbm
toolbar.o: lib/toolbar/markOdd.xpm lib/toolbar/pageNext-i.xbm
toolbar.o: lib/toolbar/pageNext-i.xpm lib/toolbar/pageNext.xbm
toolbar.o: lib/toolbar/pageNext.xpm lib/toolbar/pagePrev-i.xbm
toolbar.o: lib/toolbar/pagePrev-i.xpm lib/toolbar/pagePrev.xbm
toolbar.o: lib/toolbar/pagePrev.xpm mine/toolbar.h wlib/wlib/WlToolBar.h
toolbar.o: wlib/wlib/rcs.h
track.o: mine/Ghostview.h mine/dsc.h mine/mgv.h mine/opaque.h mine/strings.h
track.o: mine/track.h wlib/wlib/rcs.h
uiutil.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/mgv.h mine/opaque.h
uiutil.o: mine/sens.h mine/uiutil.h wlib/wlib/rcs.h
unmangle.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/mgv.h mine/opaque.h
unmangle.o: mine/strings.h mine/unmangle.h wlib/wlib/rcs.h wlib/wlib/stdlib.h
unmangle.o: wlib/wlib/strdefs.h wlib/wlib/wlib.h
util.o: mine/util.h wlib/wlib/rcs.h wlib/wlib/stdlib.h
view.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/mgv.h mine/opaque.h
view.o: mine/strings.h mine/view.h mine/zoom.h wlib/wlib/log.h wlib/wlib/rcs.h
view.o: wlib/wlib/sanity.h wlib/wlib/strdefs.h wlib/wlib/typesanity.h
view.o: wlib/wlib/wlib.h
zoom.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/log.h mine/mgv.h
zoom.o: mine/opaque.h mine/sens.h mine/strings.h mine/util.h mine/zoom.h
zoom.o: wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/stdlib.h
zoom.o: wlib/wlib/strdefs.h wlib/wlib/wlib.h

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
