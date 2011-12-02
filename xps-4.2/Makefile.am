# $Id: Makefile.am,v 1.6 2003/03/22 01:06:05 rockyb Exp $
## Process this file with automake to produce Makefile.in

# No International support yet.
SUBDIRS =  doc intl macros src

EXTRA_DIST = \
	makesig.pl autogen.sh \
	doc/home-page/index.html \
	PORTING xps.spec.in intl/Makefile.in

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)@PACKAGE_PIXMAPS_DIR@; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)@PACKAGE_PIXMAPS_DIR@; \
	    fi \
	  done \
	fi

# Copy RPM spec file. Touch *.h so we don't have to regen at user end
# Need to remove symlinks. 
dist-hook: xps.spec
	cp xps.spec $(distdir)
	touch $(distdir)/src/help/*.h; \
	rm -f $(distdir)/src/os.h; \
	rm -f $(distdir)/src/os.c; \
	rm -f $(distdir)/src/loadavg.h; \
	rm -f $(distdir)/src/loadavg.c; 

nextsrpm: all dist
#	@sudo @RPM@ -ta $(PACKAGE)-$(VERSION).tar.gz
	@RPMBUILD@ -ta $(PACKAGE)-$(VERSION).tar.gz

# Not currently needed, but may use sometime...
#	if test -d pixmaps; then \
#	  mkdir $(distdir)/pixmaps; \
#	  for pixmap in pixmaps/*; do \
#	    if test -f $$pixmap; then \
#	      cp -p $$pixmap $(distdir)/pixmaps; \
#	    fi \
#	  done \
#	fi

