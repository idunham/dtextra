#$Id: Makefile.am,v 1.4 2003/03/04 17:20:58 rockyb Exp $

# Set up the install target 

EXT = 1
man1_MANS = $(PACKAGE).$(EXT)

# Make sure these files end up in the distribution tarball
EXTRA_DIST = $(PACKAGE).pod $(man1_MANS)

all: $(PACKAGE).$(EXT) $(PACKAGE).html

$(PACKAGE).html: $(PACKAGE).pod
	pod2html --infile=$(PACKAGE).pod --outfile=$@

$(PACKAGE).$(EXT): $(PACKAGE).pod
	pod2man --center='User Commands' $(PACKAGE).pod >$@

MOSTLYCLEANFILES= $(PACKAGE).html $(PACKAGE).$(EXT)
CLEANFILES= *~
