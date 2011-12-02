#
# $Header: /cvsroot/xlt/Xlt/Makefile.am,v 1.27 2005/09/16 12:55:48 rwscott Exp $
#

MAINTAINERCLEANFILES=Makefile.in configure libtool config.guess config.sub \
                      ltconfig ltmain.sh aclocal.m4 config.h.in install-sh \
                      missing mkinstalldirs

SUBDIRS	= . lib doc tests scripts

EXTRA_DIST = COPYING.LIB ac_find_xlt.m4 ac_find_xpm.m4 ac_find_motif.m4

aclocaldir = @ACLOCALDIR@

if Aclocal

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(aclocaldir)
	-@$(INSTALL_DATA) $(srcdir)/ac_find_xlt.m4 $(DESTDIR)$(aclocaldir)/ac_find_xlt.m4
	-@$(INSTALL_DATA) $(srcdir)/ac_find_xpm.m4 $(DESTDIR)$(aclocaldir)/ac_find_xpm.m4
	-@$(INSTALL_DATA) $(srcdir)/ac_find_motif.m4 $(DESTDIR)$(aclocaldir)/ac_find_motif.m4

uninstall-local:
	-@rm -f $(DESTDIR)$(aclocaldir)/ac_find_xlt.m4
	-@rm -f $(DESTDIR)$(aclocaldir)/ac_find_xpm.m4
	-@rm -f $(DESTDIR)$(aclocaldir)/ac_find_motif.m4

endif

#
# Some convenient targets for users of Exuberant ctags
#

if Have_Exctags
# Specify options
#  - for recursive search
#  - to prevent to follow symbolic links
#  - to avoid picking up Makefiles, etc.
#    (selecting C++ support picks up the .h files, etc.)
CTAGS_CMD=ctags -R --links=no --languages=c,c++

# One target for each version and an unified one
ctags:
	$(CTAGS_CMD) \
              lib

ctags-all:
	$(CTAGS_CMD) \
              lib tests
endif

include $(srcdir)/spec.am
include $(srcdir)/MaintainerMode.am
