#
# This is the main Makefile for MEDITRES
#
# Process this file with automake to produce Makefile.in
#
# $Header: /cvsroot-fuse/meditres/meditres/Makefile.am,v 1.8 2002/01/16 20:05:43 amai Exp $
#
AUTOMAKE_OPTIONS = foreign 1.4 readme-alpha
#ACLOCAL_AMFLAGS = -I m4

D=	`date +%G%m%d.%H%M%S`

#SUBDIRS = intl src po doc examples m4

# Remove this file here (it is created via configure), not from within intl.
# DISTCLEANFILES = intl/libintl.h

EXTRA_DIST = AUTHORS FAQ meditres.spec.in meditres.spec bootstrap \
	       meditres.html meditres1.png

#
# This can be used to put the files in place.
#
web:
	scp meditres.html dannybackx@shell.sourceforge.net:/home/groups/m/me/meditres/htdocs/index.html

docdir=        $(prefix)/Meditres
doc_DATA=	AUTHORS FAQ meditres.html meditres1.png

ChangeLog::

bin_PROGRAMS=	meditres

meditres_SOURCES=	motif.c meditres.h \
	actions.c comm.c editres.c geometry.c handler.c setvalues.c \
        svpopup.c utils.c wtree.c finddlg.c print.c editresP.h appres.c

meditres_LDADD= -lXmu

# INCLUDES+=	-I$(top_builddir)/intl

CFLAGS=		@CFLAGS@
