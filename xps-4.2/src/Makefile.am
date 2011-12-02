# $Id: Makefile.am,v 1.17 2003/04/24 00:21:29 rockyb Exp $
# Process this file with automake to produce Makefile.in
# Copyright (C) 2001, 2002, 2003 Free Software Foundation, Inc.
# This Makefile.am is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This file is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

SUBDIRS = AIX BSDI NetBSD FreeBSD Linux SunOS libgtop help
noinst_SCRIPTS = postconf.pl

EXTRA_DIST = TAGS tags ad2c postconf.pl Xps.ad OpenBSD

INTLLIBS =

DISTCLEANFILES = Xps

bin_PROGRAMS = xps

xps_SOURCES = \
MenuSupport.c    MenuSupport.h \
askquestion.c    askquestion.h \
callback.c       callback.h \
common.c         common.h \
dialog.c         dialog.h \
getpixelbyname.c getpixelbyname.h \
loadavg.c        loadavg.h \
os.c		 os.h \
process.c        process.h \
xpsplay.c        xpsplay.h \
xstuff.c         xstuff.h \
xpstypes.h	 xps.h	  \
Xps.ad.h	 user_config.h

# Stuff for X 
X_CFLAGS	= @X_CFLAGS@
X_LIBS		= @X_LIBS@
X_PRE_LIBS	= @X_PRE_LIBS@
X_EXTRA_LIBS	= @X_EXTRA_LIBS@
XAPPLOADDIR     = @x_applibdir@
KMEM_GROUP      = kmem

# Stuff for compiling and loading C programs.
INCLUDES        = -I. -I$(srcdir) @XINCLUDES@
LIBS            = @LIBS@
ALL_CFLAGS      = $(CFLAGS) $(X_CFLAGS) $(INCLUDES) 

xps_LDADD = $(LIBS) @X_LIBS@ @X_PRE_LIBS@ @X_EXTRA_LIBS@


appdefaultdir   = @x_applibdir@
appdefault_DATA = Xps

# Little test program to show load average
getloadavg: loadavg.c loadavg.h
	$(CC) -DSTANDALONE -c $(CFLAGS) $< -o $@.o
	$(LINK) $@.o $(xps_LDFLAGS) $(xps_LDADD) -o $@

Xps.ad.h: $(srcdir)/Xps.ad
	$(srcdir)/ad2c $(srcdir)/Xps.ad >$@

Xps: $(srcdir)/Xps.ad
	@if test -f $@; then \
	  rm Xps; \
	fi
#	echo '!! Uncomment only those entries below that you wish to change' >$@
#	cat $< | sed -e "s/^/\!/" >> $@
	cat $(srcdir)/Xps.ad  >> $@

.c.o:
	$(CC) -c $(ALL_CFLAGS) $<

# What we need to do to install this on your system.
install: @INSTALL_TARGET@ install-man install-appdefaultDATA

# Try to install as suid so we can see info about all processes.
# However that may fail if installing across mounted filesystems, 
# so revert to non-suid if that fails.
install-suid: install-binPROGRAMS
	@if [ -w /bin/su ]; then set -x; \
	-chmod 4775 $(DESTDIR)/$(bindir)/$(PACKAGE);\
	else /bin/echo "You are not root, not installing as suid."; \
	fi

# Install, need to sgid.
install-sgid: install-binPROGRAMS
	@if [ -w /bin/su ]; then set -x; \
	-chgrp $(KMEM_GROUP) $(DESTDIR)/$(bindir)/$(PACKAGE) && \
	chmod 4775 $(DESTDIR)/$(bindir)/$(PACKAGE);\
	else /bin/echo "You are not root, not installing as sgid."; \
	fi

# Install, but don't need to suid to see info about all processes.
install-nosuid: install-binPROGRAMS

ctags: tags 

tags:
	find . -type f -name "*.[ch]*" | xargs $(CTAGS)
