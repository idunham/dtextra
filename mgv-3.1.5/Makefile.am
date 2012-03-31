# @(#)$Mu: mgv/Makefile.am 1.43.8.1 1998/09/28 09:46:33 $
##
## Makefile.am
##	automake file for mgv.
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

bin_PROGRAMS = mgv
mgv_SOURCES  = \
	Ghostview.c app.c auto.c center.c dsc.c events.c file.c help.c \
	log.c magstep.c main.c mark.c media.c mgv.c mgv_create.c options.c \
	orientation.c page.c page_control.c print.c process.c scroll.c sens.c \
	strings.c toolbar.c track.c uiutil.c unmangle.c util.c view.c zoom.c \
	_ad.c _switch_help.c
mgv_LDADD = _bld.o -Lwlib -lwlib \
	@LIBHELP_LIBS@ @XM_LIBS@ @EDITRES_LIBS@ @XT_LIBS@ @XPM_LIBS@ \
	@XEXT_LIBS@ @X11_LIBS@

mgvresdir = $(pkgdatadir)/$(VERSION)
mgvres_DATA = MGv

##
## Make sure this is kept in sync with the _bld.c rule below.
##
mgv_DEPENDENCIES = _bld.o wlib/libwlib.a

man_MANS = mgv.1

BUILD_SOURCE = _ad.c _bld.c _switch_help.c mgv.1 MGv

CLEANFILES = .been_warned
DISTCLEANFILES = _ad.c _bld.c mgv.1 MGv

INCLUDES = -I. -Iwlib

SUBDIRS = doc wlib lib

_ad.c: MGv
	tools/ad2c -a bld_fallbacks MGv > _ad.c

MGv: MGv.ad
	sed -e 's;__DATADIR__;$(datadir)/mgv/$(VERSION);g' -e 's;__VERSION__;$(VERSION);g' < MGv.ad > MGv || (rm -f MGv; exit 1)

mgv.1: mgv.man
	sed -e 's;__VERSION__;$(VERSION);g' \
		-e 's;__DATADIR__;@datadir@/mgv/$(VERSION)/help;g' \
		< mgv.man > mgv.1 || (rm -f mgv.1; exit 1)

##
## Maintainer only things.
##
if MU_MAINTAINER

res.m4: doc/mgv.sgml-in
	tools/mkresm4 --prefix=mgv --output=res.m4 $<

MGv.ad: MGv.ad.in res.m4 
	m4 -P -D__CLS__=MGv -I. -Iwlib res.m4 MGv.ad.in > MGv.ad \
		|| (rm -f MGv.ad; exit 1)

mgv.man: mgv.man.in doc/mgv.sgml
	tools/yanksect --section 'Command Line Switches' \
		< doc/mgv.sgml > _switch_help.sgml
	sgml2txt -f _switch_help.sgml
	tools/txt2man mgv.man.in < _switch_help.txt > mgv.man
	rm -f _switch_help.sgml _switch_help.txt

_switch_help.c: doc/mgv.sgml
	tools/yanksect --section 'Command Line Switches' \
		< doc/mgv.sgml > _switch_help.sgml
	sgml2txt -f _switch_help.sgml
	tools/txt2swh < _switch_help.txt > _switch_help.c
	rm -f _switch_help.sgml _switch_help.txt

endif

##
## We really want to use $(mgv_DEPENDENCIES) here as well but, alas,
## that will yield a circular dependency because _bld.o is in
## $(mgv_DEPENDENCIES).  Hence, we duplicate parts of $(mgv_DEPENDENCIES)
## here.
##
_bld.c: $(mgv_OBJECTS) wlib/libwlib.a
	tools/mkver '$(VERSION)' '$(COMPILE)' '$(LDFLAGS)' '-lwlib @LIBHELP_LIBS@ @XM_LIBS@ @XT_LIBS@ @XPM_LIBS@ @EDITRES_LIBS@ @X11_LIBS@' '$(bindir)' '$(datadir)' > _bld.c || (rm -f _bld.c; exit 1)

MAINTAINERCLEANFILES = Makefile.in MGv.ad _switch_help.c mgv.man res.m4

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

##
## Semi-automatically generated dependencies.  See tools/mudep for things.
## Only local files are listed here (this makes things system-neutral).
## This is the only decent way I could think of to get deps with automake
## and autoconf.  This mudep trick works with Linux and the standard XFree86
## makedepend, YMMV anywhere else.
##
## --STUPID-MARKER--
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
