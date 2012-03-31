/*
 * strings.c
 *	string handling for wlib (mostly for language fun)
 *
 * Copyright (C) 1997 Eric A. Howe
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
MU_ID("$Mu: wlib/strings.c 1.11 1998/08/30 03:11:05 $")

#include <stdio.h>
#include <X11/StringDefs.h>

#include <wlib/wlib.h>
#include <wlib/wlibP.h>

typedef struct {
	String		s;
	XtResource	r;
} STRING;
/*
 * keep this in sync with the enum in wlibP.h, we always assume
 * the unknown is the last value--if something gets added after it
 * this needs adjustment
 */
#define	OFFSET(f)	XtOffsetOf(STRING, f)
static STRING strings[] = {
	{NULL, {
		XwlNhelpBrowser, XwlCHelpBrowser,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)""
	}}, {NULL, {
		XwlNhelpPrefix, XwlCHelpPrefix,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)""
	}}, {NULL, {
		XwlNstringCantGetFile, XwlCStringCantGetFile,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"could not get file name from file selector"
	}}, {NULL, {
		XwlNstringEmptyLog, XwlCStringEmptyLog,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"This log has nothing to save."
	}}, {NULL, {
		XwlNstringCantBrowse, XwlCStringCantBrowse,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Could not launch browser %s: %s"
	}}, {NULL, {
		XwlNstringNotSupported, XwlCStringNotSupported,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"%s help not supported, using %s."
	}}, {NULL, {
		XwlNstringBadVersion, XwlCStringBadVersion,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Bad app-defaults version: wanted '%s', found '%s'"
	}}, {NULL, {
		XwlNstringUnknown, XwlCStringUnknown,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Unknown message."
	}}
};
#undef OFFSET

String
wlp_s(Widget w, int name)
{
	if(strings[name].s == NULL)
		XtGetApplicationResources(wl_top(w), &strings[name].s,
						&strings[name].r, 1, NULL, 0);
	return strings[name].s;
}
