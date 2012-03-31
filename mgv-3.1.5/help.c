/*
 * help.c
 *	Help menu callbacks for mgv.
 *
 * Copyright (C) 1996  Eric A. Howe
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/help.c 1.78 1998/08/25 07:35:18 $")

#include <assert.h>

#include <wlib/wlib.h>
#include <wlib/build.h>
#include <wlib/icons.h>
#include <wlib/about.h>
#include <mine/mgv.h>
#include <mine/help.h>

static String cpright =
	"MGv %s\n"
	"A Motif user interface for ghostscript.\n"
	"Copyright (C) 1996-1998 Eric A. Howe, Matthew D. Francey";
static String authors =
	"Authors:\n"
	"Eric A. Howe (mu@trends.net), http://www.trends.net/~mu)\n"
	"Matthew D. Francey\n"
	"\n"
	"Based on work by:\n"
	"Tim Theisen (tim@cs.wisc.edu) (Ghostview 1.5)\n"
	"and\n"
	"Johannes Plass (plass@dipmza.physik.uni-mainz.de)\n"
	"(Ghostview-VMS 2.3)";

static String gpl[] = {
	"This program is free software; you can redistribute it and/or\n",
	"modify it under the terms of the GNU General Public License as\n",
	"published by the Free Software Foundation; either version 2 of the\n",
	"License, or (at your option) any later version.\n",
	"\n",
	"This program is distributed in the hope that it will be useful\n",
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n",
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n",
	"GNU General Public License for more details.\n",
	"\n",
	"You should have received a copy of the GNU General Public License\n",
	"along with this program; if not, write to the Free Software\n",
	"Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n",
	NULL
};

/*
 * help->about
 */
void
mgv_help_about(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	char	buf[256];
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	if(m->about == NULL) {
		sprintf(buf, cpright, bld_verstring);
		m->about = wl_about(m->main, buf, authors, gpl, wl_icon, NULL);
	}
	wl_dlg_popup(m->about);
}
