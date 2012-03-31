/*
 * appclass.c
 *	Convenience functions for accessing the class and application
 *	names.
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
MU_ID("$Mu: wlib/appclass.c 1.7 1998/08/27 04:24:17 $")

#include <X11/Intrinsic.h>

#include <wlib/wlib.h>

int
wl_appnames(Widget w, char **name, char **cls)
{
	XClassHint	hints;

	memset((void *)&hints, '\0', sizeof(hints));
	w = wl_top(w);
	if(XGetClassHint(XtDisplay(w), XtWindow(w), &hints) == 0)
		return FALSE;
	if(name != NULL)
		*name = XtNewString(hints.res_name);
	if(cls != NULL)
		*cls = XtNewString(hints.res_class);
	XFree(hints.res_name);
	XFree(hints.res_class);

	return TRUE;
}

char *
wl_appname(Widget w)
{
	char *s = NULL;
	wl_appnames(w, &s, NULL);
	return s;
}

char *
wl_appclass(Widget w)
{
	char *s = NULL;
	wl_appnames(w, NULL, &s);
	return s;
}
