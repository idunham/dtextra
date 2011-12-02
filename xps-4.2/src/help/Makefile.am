# $Id: Makefile.am,v 1.3 2003/04/23 23:11:48 rockyb Exp $
#  Copyright (C) 1999-2002 Rocky Bernstein
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# Make sure these files end up in the distribution tarball

BUILT_SOURCES = general.h interval.h process_window.h warranty.h \
     watch_user.h debug.h keymouse.h process_tree.h user_color.h \
     keyfns.h kill_signal.h button.h pscmdline.h

EXTRA_DIST = general.txt interval.txt process_window.txt warranty.txt \
     watch_user.txt debug.txt keymouse.txt process_tree.txt user_color.txt \
     keyfns.txt kill_signal.txt button.txt pscmdline.txt \
     $(BUILT_SOURCES) pod2c.pl

all: $(BUILT_SOURCES)

# These are the targets below for which the target isn't a file.
.PHONY:	all maintainer-clean

# These are the implict rules we've got here.
SUFFIXES = .txt .h

.txt.h:    $(srcdir)/pod2c.pl
	$(PERL) $(srcdir)/pod2c.pl $< >$@

ctags: tags 

tags:
	find . -type f -name "*.[ch]*" | xargs $(CTAGS)

clean-generic:
	-rm -f *h *~

