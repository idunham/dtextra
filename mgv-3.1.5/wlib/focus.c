/*
 * focus.c
 *	Focus initialization utilities.  We can't get away with
 *	XmNinitialFocus since it only gets used once and it doesn't
 *	deal with *blah things when coming from resource files (sigh).
 *
 * Copyright (C) 1996  Matthew D. Francey
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
MU_ID("$Mu: wlib/focus.c 1.6 1998/04/06 03:50:01 $")

#include <Xm/Xm.h>

#include <wlib/wlib.h>

/*
 * not really needed but...
 */
typedef struct {
	String	s;
} SSTRING;

/*
 * A pox on XmNinitialFocus! I curse its worthlessness!
 */
Widget
wl_focus(Widget w)
{
	XtResource r = {
		XwlNinitialFocus, XwlCInitialFocus,
		XtRString, sizeof(String), XtOffsetOf(SSTRING, s),
		XtRString, NULL
	};
	SSTRING	s;

	s.s = NULL;
	XtGetApplicationResources(w, &s, &r, 1, NULL, 0); 
	if(s.s == NULL
	|| (w = wl_find1(w, s.s)) == NULL)
		return NULL;
	return XmProcessTraversal(w, XmTRAVERSE_CURRENT) ? w : NULL;
}
