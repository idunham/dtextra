#
# $Id: Makefile.am,v 1.21 2002/02/14 10:36:11 amai Exp $
#

MAINTAINERCLEANFILES = Makefile.in example.uid

noinst_PROGRAMS = xyplot realtime example


INCLUDES = -I$(srcdir)/../../lib @MOTIF_CFLAGS@ @X_CFLAGS@ @XPM_CFLAGS@

LDADD = ../../lib/libXlt.la

EXTRA_DIST= data.txt example.uil

example_SOURCES=example.c
example_LDADD = ../../lib/libXlt.la @MRM_LDADD@ $(MOTIF_LIBADD)

example.uid: example.uil
	uil $(srcdir)/../../tests/SciPlot/example.uil -I$(srcdir)/../../lib -o example.uid
