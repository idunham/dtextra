#
# $Header: /cvsroot/xlt/Xlt/tests/LiteClue/Makefile.am,v 1.2 2005/05/18 22:16:00 rwscott Exp $
#
MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST=	LiteClue_test.xd

# noinst_SOURCES = LiteClue_test.c LiteClue_test_cb.c LiteClue_test.h LiteClue_testP.h LiteClue_test_stubs.c
noinst_PROGRAMS = LiteClueTest LiteClue_test 

INCLUDES = -I$(srcdir)/../../lib @MOTIF_CFLAGS@ @X_CFLAGS@ @XPM_CFLAGS@

noinst_HEADERS = LiteClue_test.h LiteClue_testP.h

LDADD = ../../lib/libXlt.la
LiteClueTest_LDADD = ../../lib/libXlt.la -lXaw

LiteClue_test_SOURCES=	LiteClue_test.c LiteClue_test_cb.c
