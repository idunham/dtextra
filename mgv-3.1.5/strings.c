/*
 * strings.c
 *	String handling for mgv (mostly for i18n/l10n stuff which no one
 *	has bothered to do yet).  All of the strings are loaded and cached
 *	on first use.
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
MU_ID("$Mu: mgv/strings.c 1.23 1998/08/30 03:11:04 $")

#include <stdio.h>
#include <wlib/wlib.h>
#include <X11/StringDefs.h>

#include <mine/strings.h>
#include <mine/strdefs.h>

typedef struct {
	String		s;
	XtResource	r;
} STRING;
/*
 * keep this in sync with the enum in strings.h, we always assume
 * the unknown is the last value--if something gets added after it
 * this needs adjustment
 */
#define	OFFSET(f)	XtOffsetOf(STRING, f)
static STRING strings[] = {
	{NULL, {
		MgvNstringBadArgument, MgvCStringBadArgument,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"%s:  unknown argument '%s' to action"
	}}, {NULL, {
		MgvNstringCantAllocPixmap, MgvCStringCantAllocPixmap,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"backing pixmap could not be allocated"
	}}, {NULL, {
		MgvNstringCantDecompress, MgvCStringCantDecompress,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Cannot decompress"
	}}, {NULL, {
		MgvNstringCantHideMenu, MgvCStringCantHideMenu,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Menu bar won't let itself be hidden."
	}}, {NULL, {
		MgvNstringCantOpen, MgvCStringCantOpen,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Could not open"
	}}, {NULL, {
		MgvNstringCantPrint, MgvCStringCantPrint,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Print command failed:"
	}}, {NULL, {
		MgvNstringError, MgvCStringError,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Error"
	}}, {NULL, {
		MgvNstringFileExists, MgvCStringFileExists,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"File exists, overwrite?"
	}}, {NULL, {
		MgvNstringInterpDone, MgvCStringInterpDone,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Interpreter done."
	}}, {NULL, {
		MgvNstringInterpFailed, MgvCStringInterpFailed,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"GhostScript Interpreter failure."
	}}, {NULL, {
		MgvNstringMessage, MgvCStringMessage,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Message"
	}}, {NULL, {
		MgvNstringNoCoordinates, MgvCStringNoCoordinates,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"(?, ?)"
	}}, {NULL, {
		MgvNstringNoDate, MgvCStringNoDate,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"No Date"
	}}, {NULL, {
		MgvNstringNoFile, MgvCStringNoFile,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"No File"
	}}, {NULL, {
		MgvNstringNoPrintCommand, MgvCStringNoPrintCommand,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"No print command given"
	}}, {NULL, {
		MgvNstringPageFinished, MgvCStringPageFinished,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Page finished."
	}}, {NULL, {
		MgvNstringUnknownSwitch, MgvCStringUnknownSwitch,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Unknown switch(es): %signoring."
	}}, {NULL, {
		MgvNstringPS, MgvCStringPS,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"PostScript (PS)"
	}}, {NULL, {
		MgvNstringEPS, MgvCStringEPS,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Encapsulated PostScript (EPS)"
	}}, {NULL, {
		MgvNstringPDF, MgvCStringPDF,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Portable Document Format (PDF)"
	}}, {NULL, {
		MgvNstringUnknownType, MgvCStringUnknownType,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Unknown Document Type"
	}}, {NULL, {
		MgvNstringUnknownMedia, MgvCStringUnknownMedia,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Unknown Media"
	}}, {NULL, {
		MgvNstringUnknown, MgvCStringUnknown,
		XtRString, sizeof(String), OFFSET(s),
		XtRString, (XtPointer)"Unknown message."
	}}
};
#undef OFFSET

String
mgv_s(Widget w, int name)
{
	if(strings[name].s == NULL)
		XtGetApplicationResources(wl_top(w), &strings[name].s,
						&strings[name].r, 1, NULL, 0);
	return strings[name].s;
}
