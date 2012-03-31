/*
 * error.c
 *	Warning and Error dialog convenience functions for wlib.
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
MU_ID("$Mu: wlib/error.c 1.31 1998/08/25 06:41:26 $")

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <Xm/MessageB.h>
#include <Xm/MwmUtil.h>

#include <wlib/wlib.h>

static void
wl_error_ok(Widget w, XtPointer closure, XtPointer call)
{
	USEUP(call);
	XtDestroyWidget(w);
	if(closure != NULL)
		XtSetSensitive((Widget)closure, True);
}

#define	WLWE_ERROR	1
#define	WLWE_WARN	2
#define	WLWE_MSG	3

static Widget
worker(Widget parent, char *msg, int type)
{
	Widget		(*box_maker)(Widget, String, ArgList, Cardinal);
	Widget		mb;
	char		*name;
	Arg		a;
	XtPointer	closure;

	switch(type) {
	case WLWE_WARN:
		name      = "wlWarn";
		box_maker = XmCreateWarningDialog;
		closure   = NULL;
		break;
	case WLWE_MSG:
		name      = "wlMessage";
		box_maker = XmCreateMessageDialog;
		closure   = NULL;
		break;
	case WLWE_ERROR:
		name      = "wlError";
		box_maker = XmCreateErrorDialog;
		closure   = (XtPointer)parent;
		break;
	default:
		assert("wl_we(): confused by dialog type" != NULL);
		return NULL;
	}

	assert(name      != NULL);
	assert(box_maker != NULL);
	XtSetArg(a, XmNmwmDecorations, MWM_DECOR_ALL|MWM_DECOR_MENU);
	if((mb = box_maker(parent, name, &a, 1)) == NULL)
		return NULL;
	XtVaSetValues(mb,
		XtVaTypedArg,	XmNmessageString,
				XtRString, msg,
				strlen(msg) + 1,
		NULL);
	XtAddCallback(mb, XmNokCallback,    wl_error_ok, closure);
	XtAddCallback(mb, XmNunmapCallback, wl_error_ok, closure);
	XtUnmanageChild(XmMessageBoxGetChild(mb, XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(mb, XmDIALOG_HELP_BUTTON));

	if(type == WLWE_ERROR)
		XtSetSensitive(parent, False);
	XtManageChild(mb);
	return mb;
}

Widget
wl_error(Widget parent, char *fmt, ...)
{
	va_list		ap;
	char		msg[2048];

	va_start(ap, fmt);
	vsprintf(&msg[0], fmt, ap);
	va_end(ap);
	return worker(parent, &msg[0], WLWE_ERROR);
}

Widget
wl_warn(Widget parent, char *fmt, ...)
{
	va_list		ap;
	char		msg[2048];

	va_start(ap, fmt);
	vsprintf(&msg[0], fmt, ap);
	va_end(ap);
	return worker(parent, &msg[0], WLWE_WARN);
}

Widget
wl_msg(Widget parent, char *fmt, ...)
{
	va_list	ap;
	char	msg[2048];

	va_start(ap, fmt);
	vsprintf(&msg[0], fmt, ap);
	va_end(ap);
	return worker(parent, &msg[0], WLWE_MSG);
}
