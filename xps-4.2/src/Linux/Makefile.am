#$Id: Makefile.am,v 1.1 2002/12/02 15:34:14 rockyb Exp $

# Set up the install target 

# Make sure these files end up in the distribution tarball
EXTRA_DIST = loadavg.c loadavg.h os.c os.h sigmap.h

all: sigmap.h

sigmap.h: 
	@PERL@ ../../makesig.pl >$@
