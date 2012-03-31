/*
 * init.c
 *	Application initialization functions.  This file is part of wlib.
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
MU_ID("$Mu: wlib/init.c 1.44 1998/09/01 20:49:44 $")

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Xm/RepType.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/wlibP.h>

static	XtAppContext ac;

static void
context_action(Widget w, XEvent *ev, String *argv, Cardinal *argc)
{
	XmAnyCallbackStruct	cbs;
	USEUP(argv); USEUP(argc);

	memset((void *)&cbs, '\0', sizeof(cbs));
	cbs.reason = XmCR_HELP;
	cbs.event  = ev;
	wl_help_context(w, NULL, &cbs);
}

static Boolean
string2percent(Display *dpy, XrmValue *a, Cardinal *n, XrmValue *from,
						XrmValue *to, XtPointer *data)
{
	int	alloced = FALSE;
	double	value;
	char	*p;
	USEUP(data); USEUP(a);

	if(*n != 0) {
		XtWarningMsg("wrongParameters", "cvtStringToPercentage",
				"XtToolkitError",
				"String to Percentage needs no extra arguments",
				NULL, NULL);
	}
	if(from->addr == NULL)
		return False;
	if(to->addr != NULL && to->size < sizeof(int)) {
		to->size = sizeof(int);
		return False;
	}
	if(to->addr == NULL) {
		to->addr = XtMalloc(sizeof(int));
		to->size = sizeof(int);
		alloced  = TRUE;
	}

	/*
	 * Convert and check error conditions.
	 * We use an int to store the result since pushing floats
	 * and doubles through the resources is kinda' brutal on the
	 * Alpha (and possibly the UltraSPARC); hence, the scaling
	 * factor of 1e3 instead of 1e2.
	 */
	value = strtod((char *)from->addr, &p);
	if(value !=  HUGE_VAL
	&& value != -HUGE_VAL
	&& p     !=  from->addr) {
		if(*p == '%')
			value /= 100.0;
		*(int *)(to->addr) = (int)(value * 1000.0);
		return True;
	}

	if(alloced) {
		XtFree(to->addr);
		to->addr = NULL;
	}
	XtDisplayStringConversionWarning(dpy,(char *)from->addr,XwlRPercentage);
	return False;
}

static XtActionsRec actions[] = {
	{"wlHelpContext",	context_action},
};

Display *
wl_init(int *argc, char **argv, String cls, XrmOptionDescList opt,
						Cardinal nopt, char **fallbacks)
{
	Display	*dpy;

	/*
	 * On the version of Sinix that I have access to (5.42)
	 * tear off menus cause a core dump when they are torn
	 * off (this even happens with the Motif sample programs)
	 * but we'll install the type converter anyway:
	 *
	 *	DO NOT SHOOT YOURSELF IN THE HEAD, it hurts.
	 */
	XtSetLanguageProc(NULL, NULL, NULL);
	XmRepTypeInstallTearOffModelConverter();
	XtToolkitInitialize();
	XtSetTypeConverter(XmRString, XwlRPercentage, string2percent, NULL, 0,
							XtCacheNone, NULL);
	if((ac = XtCreateApplicationContext()) == NULL) {
		fprintf(stderr, "wl_init(): could not create AppContext\n");
		return NULL;
	}
	XtAppAddActions(ac, actions, XtNumber(actions)); 
	if(fallbacks != NULL)
		XtAppSetFallbackResources(ac, fallbacks);
	dpy = XtOpenDisplay(ac, NULL, NULL, cls, opt, nopt, argc, argv);
	if(dpy == NULL) {
		fprintf(stderr, "wl_init(): could not open display\n");
		return NULL;
	}

	return dpy;
}

XtAppContext
wl_app(void)
{
	return ac;
}
