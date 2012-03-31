/*
 * help.c
 *	Help support functions.  This file is part of wlib.
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
MU_ID("$Mu: wlib/help.c 1.60 1998/09/01 20:49:44 $")

#include <string.h>
#include <ctype.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/wlibP.h>

static struct {
	char		*s;
	WL_HELPFN	f;
} helptypes[] = {
	{"html",	wlp_help_html},
	{"html-remote",	wlp_help_remote},
	{"libhelp",	wlp_help_libhelp},
	{"netscape",	wlp_help_remote},
	{"remote",	wlp_help_remote},
};
#define	N_HELPTYPES	(int)(sizeof(helptypes)/sizeof(helptypes[0]))

typedef struct {
	WL_HELPFN	f;
	String		chapter;
	String		section;
} HELP;

/*
 * string->help function type converter
 */
static Boolean
string2help(Display *dpy, XrmValue *a, Cardinal *n, XrmValue *from,
						XrmValue *to, XtPointer *data)
{
	int	i, alloced = FALSE;
	char	*lower, *p;
	USEUP(dpy); USEUP(a); USEUP(n); USEUP(data);

	/*
	 * Sanity.
	 */
	if(*n != 0) {
		XtWarningMsg("wrongParameters", "cvsStringToHelpType",
			"XtToolkitError",
			"String to HelpType conversion needs no extra arguments",
			NULL, NULL);
	}

	if(from->addr == NULL)
		return False;
	if(to->addr != NULL && to->size < sizeof(XtPointer)) {
		to->size = sizeof(XtPointer);
		return False;
	}
	if(to->addr == NULL) {
		to->addr = XtMalloc(sizeof(XtPointer));
		to->size = sizeof(XtPointer);
		alloced  = TRUE;
	}

	lower = XtNewString((String)from->addr);
	for(p = lower; *p != '\0'; ++p)
		*p = tolower(*p);

	for(i = 0; i < N_HELPTYPES; ++i) {
		if(strcmp(lower, helptypes[i].s) != 0)
			continue;
		*(XtPointer *)to->addr = (XtPointer)helptypes[i].f;
		XtFree(lower);
		return True;
	}

	if(alloced) {
		XtFree(to->addr);
		to->addr = NULL;
	}
	XtDisplayStringConversionWarning(dpy, (String)from->addr, XwlRHelpType);

	XtFree(lower);

	return False;
}

/*
 * standard help callback
 */
void
wl_help(Widget w, XtPointer closure, XtPointer call)
{
static	int	firsttime = TRUE;
	XtResource hr[] = {
		{
			XwlNhelpChapter, XwlCHelpChapter,
			XtRString, sizeof(String), XtOffsetOf(HELP, chapter),
			XtRString, NULL
		}, {
			XwlNhelpSection, XwlCHelpSection,
			XtRString, sizeof(String), XtOffsetOf(HELP, section),
			XtRString, NULL
		}, {
			XwlNhelpType, XwlCHelpType,
			XwlRHelpType, sizeof(WL_HELPFN), XtOffsetOf(HELP, f),
			XtRString, (XtPointer)"remote"
		}
	};
	HELP		help;
	WLP_HELP	h;
	USEUP(call); USEUP(closure);

	if(firsttime) {
		XtSetTypeConverter(XtRString, XwlRHelpType, string2help,
						NULL, 0, XtCacheNone, NULL);
		firsttime = FALSE;
	}

	for(; w != NULL; w = XtParent(w))
		if(XtHasCallbacks(w, XmNhelpCallback) == XtCallbackHasSome)
			break;
	if(w == NULL)
		return;

	memset((void *)&help, '\0', sizeof(help));
	XtGetApplicationResources(w, &help, &hr[0], XtNumber(hr), NULL, 0);
	if(help.f == NULL || help.chapter == NULL)
		return;
	h.chapter = help.chapter;
	h.section = help.section;

	help.f(w, &h);
}

void
wl_help_context(Widget w, XtPointer closure, XtPointer call)
{
	XmAnyCallbackStruct	cbs;
	Cursor			qa;
	XEvent			ev;
	Widget			top;

	USEUP(closure); USEUP(call);

	top = wl_top(w);
	memset((void *)&ev,  '\0', sizeof(ev));
	qa = XCreateFontCursor(XtDisplay(top), XC_question_arrow);
	w  = XmTrackingEvent(top, qa, False, &ev);
	while(w != NULL) {
		if(XtHasCallbacks(w, XmNhelpCallback) != XtCallbackHasSome) {
			w = XtParent(w);
			continue;
		}
		memset((void *)&cbs, '\0', sizeof(cbs));
		cbs.reason = XmCR_HELP;
		cbs.event  = &ev;
		XtCallCallbacks(w, XmNhelpCallback, (XtPointer)&cbs);
		w = NULL;
	}

	/*
	 * XmTrackingEvent() will take care of resetting the cursor
	 */
	XFreeCursor(XtDisplay(top), qa);
}
