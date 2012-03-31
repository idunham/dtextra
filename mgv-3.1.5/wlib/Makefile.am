# @(#)$Mu: wlib/Makefile.am 1.16 1998/09/01 20:49:43 $
##
## Makefile.am
##	automake file for wlib.
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

MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libwlib.a
libwlib_a_SOURCES = \
	WlAppShell.c WlSplashScreen.c WlToolBar.c about.c appclass.c \
	callback.c chkver.c create.c dismiss.c error.c fgbg.c find.c findup.c \
	focus.c free_pixmap.c help-html.c help-libhelp.c help.c icons.c \
	init.c log.c popdown_dialog.c popup_dialog.c scrollbar.c shell.c \
	strings.c togglesync.c top.c version.c xbm.c xpm.c

INCLUDES = -I.

##
## Semi-automatically generated dependencies.  See tools/mudep for things.
## Only local files are listed here (this makes things system-neutral).
## This is the only decent way I could think of to get deps with automake
## and autoconf.  This mudep trick works with Linux and the standard XFree86
## makedepend, YMMV anywhere else.
##
## --STUPID-MARKER--
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
