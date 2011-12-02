#$Id: Makefile.am,v 1.2 2002/12/02 18:38:31 rockyb Exp $

# Set up the install target 

# Make sure these files end up in the distribution tarball
EXTRA_DIST = loadavg.c loadavg.h os.c os.h sigmap.h

all: sigmap.h

sigmap.h: 
	@PERL@ ../../makesig.pl >$@

ctags: tags 

tags:
	find . -type f -name "*.[ch]*" | xargs $(CTAGS)
