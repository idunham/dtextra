/*
 * chkver.c
 *	The wl_chkver() function.
 *
 * Copyright (C) 1997 Eric A. Howe
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: wlib/chkver.c 1.9 1998/08/30 03:11:05 $")

#include <string.h>
#include <X11/StringDefs.h>

#include <wlib/wlib.h>
#include <wlib/wlibP.h>
#include <wlib/build.h>

typedef struct {
	String	version;
} BLAH;
#define	OFFSET(f)	XtOffsetOf(BLAH, f)
static XtResource res[] = {
	{
		XwlNapplicationVersion, XwlCApplicationVersion,
		XtRString, sizeof(String), OFFSET(version),
		XtRString, (XtPointer)"unknown"
	}
};
#undef	OFFSET

/*
 * You should make sure applicationVersion is set to whatever bld_verstring
 * ends up being if you're going to use this function; the build frame work
 * all my stuff uses takes care of these details with very little help.
 */
int
wl_chkver(Widget w)
{
	BLAH	v;
	w = wl_top(w);
	XtGetApplicationResources(w, &v, &res[0], XtNumber(res), NULL, 0);
	if(strcmp(bld_verstring, v.version) == 0)
		return TRUE;
	wl_warn(w, wlp_s(w, WlpSbadVersion), v.version, bld_verstring);
	return FALSE;
}
