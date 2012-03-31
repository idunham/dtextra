/*
 * log.c
 *	Error and message logging functions.
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
MU_ID("$Mu: mgv/log.c 1.67 1998/08/31 22:21:25 $")

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <Xm/ToggleB.h>

#include <wlib/wlib.h>
#include <wlib/log.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/log.h>
#include <mine/sens.h>
#include <mine/strings.h>

/*
 * log->hide
 */
static void
hidelog(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	mgv_log_hide(m, TRUE, TRUE);
}

static void
logit(Widget w, MGV *m, char *msg)
{
static	WLC cb[] = {
		{"*log*hide", XmNactivateCallback, NULL, hidelog},
		{"*log",      XmNunmapCallback,    NULL, hidelog},
		{NULL}
	};

	/*
	 * the zoomers and the main viewer share one log
	 * and that log is attached to the top level MGV
	 */
	m = mgv_top(m);

	if(m->log == NULL) {
		if((m->log = wl_log_open(m->main)) == NULL)
			return;
		wl_callback(XtParent(m->log), &cb[0], (XtPointer)m);
		wl_log_show(m->log, w != NULL && !(m->bits & MgvHIDELOG));
	}
	wl_log(m->log, "%s", msg);
}

void
mgv_log_hide(MGV *m, int hide, int toggle)
{
	Widget	w;

	m = mgv_top(m);

	if(hide)
		m->bits |= MgvHIDELOG;
	else
		m->bits &= ~MgvHIDELOG;
	if(toggle) {
		w = wl_find1(m->popup, "*hideLog");
		XmToggleButtonSetState(w, hide ? True : False, False);
		w = wl_find1(m->main, "*menuBar*view_pd*hideLog");
		XmToggleButtonSetState(w, hide ? True : False, False);
	}
	if(m->log != NULL)
		wl_log_show(m->log, !hide);

	mgv_appres(m->main)->hide_log = hide ? True : False;
}

/*
 * this gets errors (GvNoutputCallback)
 */
void
mgv_log_output(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	assert(MgvZOK(m));

	mgv_log_hide(m, FALSE, TRUE);
	logit(w, m, (char *)call);
}

/*
 * the ghostviewWidgetClass GvNmessageCallback
 */
void
mgv_log_message(Widget w, XtPointer closure, XtPointer call)
{
	char *msg = (char *)call, buf[1024];
	MGV  *m   = (MGV *)closure;
	int   type, err;
	int   error;

	assert(MgvZOK(m));
	m = mgv_top(m);

	/*
	 * I suppose we could use quarks here but there really
	 * isn't any point since this doesn't get hit very much.
	 */
	error = FALSE;
	if(strcmp(msg, "Refresh") == 0) {
		mgv_refresh(m);
		return;
	}
	else if(strcmp(msg, "Page") == 0) {
		type = MgvSmessage;
		err  = MgvSpageFinished;
	}
	else if(strcmp(msg, "Done") == 0) {
		/*
		 * this means that we are out of pages
		 */
		type = MgvSmessage;
		err  = MgvSinterpDone;
		m->sens &= ~MgvSNOTLAST;
		mgv_sens(m->senscache, m->sens);
	}
	else if(strcmp(msg, "Failed") == 0) {
		type  = MgvSerror;
		err   = MgvSinterpFailed;
		error = TRUE;
	}
	else if(strcmp(msg, "BadAlloc") == 0) {
		type  = MgvSerror;
		err   = MgvScantAllocPixmap;
		error = TRUE;
	}
	else {
		type  = MgvSerror;
		err   = MgvSunknown;
		error = TRUE;
	}
	sprintf(&buf[0], "%s: %s: %s\n", mgv_s(w, type), msg, mgv_s(w, err));
	if(error)
		mgv_log_hide(m, FALSE, TRUE);
	logit(w, (XtPointer)m, (XtPointer)&buf[0]);
}
