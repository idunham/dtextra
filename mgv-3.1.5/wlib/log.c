/*
 * log.c
 *	Log dialogs.  This file is part of wlib.
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
MU_ID("$Mu: wlib/log.c 1.57 1998/09/01 20:49:44 $")

#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <assert.h>
#include <Xm/Text.h>
#include <Xm/FileSB.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/wlibP.h>
#include <wlib/typesanity.h>
#include <wlib/log.h>
#include <wlib/sanity.h>

/*
 * user data for the log
 */
#define	LOG_MAGIC1	0x4c4f4700	/* 'L' 'O' 'G' */
#define	LOG_MAGIC2	0x00474f4c	/* 'G' 'O' 'L' */
typedef struct {
	long	magic1;
	Widget	form;
	Widget	text;
	Widget	fs;
	int	hidden;
	long	magic2;
} LOG;

static int
log_ok(LOG *lg)
{
	DIEIF(lg         == NULL);
	DIEIF(lg->magic1 != LOG_MAGIC1);
	DIEIF(lg->magic2 != LOG_MAGIC2);
	DIEIF(lg->form   == NULL);
	DIEIF(lg->text   == NULL);
	return TRUE;
}

/*
 * log->clear
 */
static void
clear(Widget w, XtPointer closure, XtPointer call)
{
	LOG	*lg = (LOG *)closure;
	USEUP(w); USEUP(call);
	assert(log_ok(lg));

	XmTextSetString(lg->text, "");
	XmTextSetCursorPosition(lg->text, (XmTextPosition)0);
	XmTextSetInsertionPosition(lg->text, (XmTextPosition)0);
	XmTextShowPosition(lg->text, (XmTextPosition)0);
}

/*
 * log->hide
 */
static void
hide(Widget w, XtPointer closure, XtPointer call)
{
	LOG	*lg = (LOG *)closure;
	USEUP(w); USEUP(call);
	assert(log_ok(lg));

	lg->hidden = TRUE;
	XtUnmanageChild(lg->form);
}

/*
 * log->fs->ok
 */
static void
save_ok(Widget w, XtPointer closure, XtPointer call)
{
	XmFSS	*cbs = (XmFSS *)call;
	LOG	*lg  = (LOG *)closure;
	char	*file, *value;
	FILE	*fp;
	assert(log_ok(lg));

	file = NULL;

	if(!XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &file)) {
		wl_error(w, "%s: %s", XtName(w), wlp_s(w, WlpScantGet));
		return;
	}
	if((value = XmTextGetString(lg->text)) == NULL
	|| value[0] == '\0') {
		wl_warn(lg->form, "%s", wlp_s(w, WlpSemptyLog));
		XtFree((XtPointer)value);
		XtUnmanageChild(w);
		return;
	}
	if((fp = fopen(file, "w")) == NULL
	|| fwrite(value, 1, strlen(value), fp) != strlen(value)) {
		wl_error(w, "%s: %s", file, strerror(errno));
		unlink(file);
	}
	else {
		XtUnmanageChild(w);
	}
	if(fp != NULL)
		fclose(fp);
	XtFree((XtPointer)value);
}

static WLW log_save[] = {
	{"logSave",	wlFileSBDialog,	0,	NULL},
	{NULL}
};
static WLC cbs[] = {
	{"*logSave",	XmNcancelCallback,	NULL,	wl_dismiss},
	{"*logSave",	XmNokCallback,		NULL,	save_ok},
	{NULL}
};

/*
 * log->save
 */
static void
save(Widget w, XtPointer closure, XtPointer call)
{
	LOG	*lg = (LOG *)closure;
	USEUP(call);
	assert(log_ok(lg));

	if(lg->fs == NULL) {
		if((lg->fs = wl_create(XtParent(w), &log_save[0])) == NULL)
			return;
		wl_callback(XtParent(lg->fs), &cbs[0], (XtPointer)lg);
	}
	else {
		XmFileSelectionDoSearch(lg->fs, NULL);
	}
	XtManageChild(lg->fs);
	XtPopup(XtParent(lg->fs), XtGrabNone);
}

static void
log_destroy(Widget w, XtPointer closure, XtPointer call)
{
	LOG *lg = (LOG *)closure;
	Arg a;
	USEUP(call);
	assert(log_ok(lg));

	XtSetArg(a, XmNuserData, NULL);
	XtSetValues(w, &a, 1);

	XtFree((void *)lg);
}

/*
 * the log dialog itself
 */
static WLW log_form[] = {
	{"hide",	wlPushB,	0,		NULL},
	{"save",	wlPushB,	0,		NULL},
	{"clear",	wlPushB,	0,		NULL},
	{"help",	wlPushB,	0,		NULL},
	{"sep",		wlSeparatorG,	0,		NULL},
	{"text",	wlScrolledText,	WL_MANAGE,	NULL},
	{NULL}
};
static WLW log_dlg[] = {
	{"log",		wlFormDialog,	0,	 	log_form},
	{NULL}
};
static WLC log_cb[] = {
	{"*hide",	XmNactivateCallback, NULL, hide},
	{"*save",	XmNactivateCallback, NULL, save},
	{"*clear",	XmNactivateCallback, NULL, clear},
	{"*help",	XmNactivateCallback, NULL, wl_help},
	{"*log",	XmNdestroyCallback,  NULL, log_destroy},
	{NULL}
};

/*
 * create a log
 */
Widget
wl_log_open(Widget parent)
{
	LOG     *lg;
	Widget   hide;
	Arg      a[3];
	Cardinal n;

	if((lg = (LOG *)XtCalloc(1, sizeof(LOG))) == NULL)
		return NULL;
	lg->magic1 = LOG_MAGIC1;
	lg->magic2 = LOG_MAGIC2;

	if((lg->form = wl_create(parent, &log_dlg[0])) == NULL) {
		XtFree((void *)lg);
		return NULL;
	}
	wl_callback(XtParent(lg->form), &log_cb[0], (XtPointer)lg);
	wl_find(lg->form,
		&hide,     "*hide",
		&lg->text, "*textSW*text",
		NULL);

	n = 0;
	XtSetArg(a[n], XmNuserData,      lg);	++n;
	XtSetArg(a[n], XmNdefaultButton, hide);	++n;
	XtSetArg(a[n], XmNcancelButton,  hide);	++n;
	wlSetValues(lg->form, a, n);

	assert(log_ok(lg));
	return lg->form;
}

/*
 * make log appear or disappear
 */
void
wl_log_show(Widget w, int show)
{
	LOG *lg;
	Arg  a;

	XtSetArg(a, XmNuserData, &lg);
	XtGetValues(w, &a, 1);
	assert(log_ok(lg));

	lg->hidden = !show;
	if(show) {
		XtManageChild(lg->form);
		XtPopup(XtParent(lg->form), XtGrabNone);
	}
	else {
		XtUnmanageChild(lg->form);
	}
}

/*
 * make an entry in a log
 */
void
wl_log(Widget w, char *fmt, ...)
{
	char	*s1, *s2;
	va_list	ap;
	char	msg[2048];
	int	n;
	LOG	*lg;
	Arg     a;

	if(w   == NULL
	|| fmt == NULL)
		return;

	va_start(ap, fmt);
	vsprintf(&msg[0], fmt, ap);
	va_end(ap);

	XtSetArg(a, XmNuserData, &lg);
	XtGetValues(w, &a, 1);
	assert(log_ok(lg));

	s1 = XmTextGetString(lg->text);
	s2 = XtRealloc((XtPointer)s1, strlen(s1) + strlen(msg) + 1);
	if(s2 != NULL)
		strcat(s1 = s2, msg);
	XmTextSetString(lg->text, s1);
	XmTextSetCursorPosition(lg->text, (XmTextPosition)(n = strlen(s1)));
	XmTextSetInsertionPosition(lg->text, (XmTextPosition)n);
	XmTextShowPosition(lg->text, (XmTextPosition)n);
	XtFree((XtPointer)s1);

	if(lg->hidden)
		return;

	XtManageChild(lg->form);
	XtPopup(XtParent(lg->form), XtGrabNone);
}
