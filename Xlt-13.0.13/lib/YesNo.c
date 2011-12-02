/**
 *
 * $Id: YesNo.c,v 1.6 2001/06/09 18:38:57 amai Exp $
 *
 * Copyright (C) 1996 Free Software Foundation, Inc.
 * Copyright © 1999-2001 by the LessTif developers.
 *
 * This file is part of the GNU LessTif Extension Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **/

#include "../config.h"

#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>

#include <Xlt.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif


static const char rcsid[] = "$Id: YesNo.c,v 1.6 2001/06/09 18:38:57 amai Exp $";

static void callback(Widget w, int *Result, XmAnyCallbackStruct *call_data)
{
	*Result = call_data->reason;
}

Boolean XltYesNo(Widget w, String Question)
{
static Widget Dialog = NULL;
static int Result;
Boolean Return;
XmString string;

	if (Dialog == NULL)
	{
	Widget shell = w;

		while (XtParent(shell) && !XtIsTopLevelShell(shell)) shell = XtParent(shell);
		Dialog = XmCreateQuestionDialog(shell, "YesNo", NULL, 0);
		XtUnmanageChild(XmMessageBoxGetChild(Dialog, XmDIALOG_HELP_BUTTON));
		XtAddCallback(Dialog, XmNokCallback, (XtCallbackProc)callback, &Result);
		XtAddCallback(Dialog, XmNcancelCallback, (XtCallbackProc)callback, &Result);
		XtAddCallback(Dialog, XmNunmapCallback, (XtCallbackProc)callback, &Result);
	}
	string = XmStringCreateSimple(Question);
	XtVaSetValues(Dialog,
		XmNmessageString, string,
		NULL);
	XmStringFree(string);
	XtManageChild(Dialog);
	Result = XmCR_NONE;
	while (Result == XmCR_NONE)
	{
		XtAppProcessEvent(XtWidgetToApplicationContext(w), XtIMAll);
	}
	switch (Result)
	{
	case XmCR_OK:
		Return = True;
		break;
	case XmCR_CANCEL:
	case XmCR_UNMAP:
		Return = False;
		break;
	default:
		fprintf(stderr, "%s(%d):XltYesNo() - Unknown result code >%i<\n",
			__FILE__, __LINE__, Result);
		Return = False;
		break;
	}
	return(Return);
}
