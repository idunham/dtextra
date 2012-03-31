/*
 * scrollbar.c
 *	Scrollbar convenience routines for wlib.
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
MU_ID("$Mu: wlib/scrollbar.c 1.7 1998/08/14 21:05:34 $")

#include <Xm/ScrollBar.h>

#include <wlib/wlib.h>
#include <wlib/sanity.h>

Widget
wl_getsbvalues(Widget w, char *name, WL_SBV *v)
{
	Arg      a[2];
	Cardinal n;

	if(name != NULL
	&& (w = wl_find1(w, name)) == NULL)
		return NULL;

	n = 0;
	XtSetArg(a[n], XmNmaximum, &v->max);	++n;
	XtSetArg(a[n], XmNminimum, &v->min);	++n;
	XmScrollBarGetValues(w, &v->value, &v->slider, &v->incr, &v->pincr);
	wlGetValues(w, a, n);

	return w;
}

void
wl_setsbvalues(Widget w, WL_SBV *v, Boolean notify)
{
	Arg      a[2];
	Cardinal n;

	n = 0;
	XtSetArg(a[n], XmNmaximum, v->max);	++n;
	XtSetArg(a[n], XmNminimum, v->min);	++n;
	wlSetValues(w, a, n);
	XmScrollBarSetValues(w, v->value, v->slider, v->incr, v->pincr, notify);
}
