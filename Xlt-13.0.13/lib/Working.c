/**
 *
 * $Id: Working.c,v 1.10 2001/06/20 18:45:30 amai Exp $
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
#include <Xm/DrawingA.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>

#include "Xlt.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif


static const char rcsid[] = "$Id: Working.c,v 1.10 2001/06/20 18:45:30 amai Exp $";

typedef struct {
	XtIntervalId Id;
	Widget Da;
	int percent;
	GC gc;
} WorkingData_t;


static void callback(Widget w, Boolean *Result, XmAnyCallbackStruct *call_data)
{
	switch (call_data->reason)
	{
	case XmCR_OK:
	case XmCR_UNMAP:
	case XmCR_CANCEL:
		*Result = True;
		break;
	default:
		*Result = False;
		break;
	}
}


static void
StructureNotifyHandler(Widget w, Boolean *mapped, XEvent *event, Boolean *cont)
{
	switch(event->type)
	{
	case UnmapNotify:
		*mapped = False;
		break;
	case MapNotify:
		*mapped = True;
		break;
	default:
		break;
	}
}


extern void
XltWaitTillMapped(Widget dialog)
{
XWindowAttributes window_attributes;
Boolean mapped = False;
Widget w = dialog;

    while (!XtIsShell(w)) w = XtParent(w);
    XGetWindowAttributes(XtDisplay(w), XtWindow(w), &window_attributes);
    if (window_attributes.map_state == IsUnmapped)
    {
	XtAddEventHandler(w,
		    StructureNotifyMask,
		    False,
		    (XtEventHandler)StructureNotifyHandler,
		    (XtPointer)&mapped);
	while (/*XtIsManaged(w) &&*/ !mapped)
	{
	    XtAppProcessEvent(XtWidgetToApplicationContext(w), XtIMXEvent);
	}
	XtRemoveEventHandler(w,
		    StructureNotifyMask,
		    False,
		    (XtEventHandler)StructureNotifyHandler,
		    (XtPointer)&mapped);
    }
    XmUpdateDisplay(w);
}


static void
UpdateGraph(WorkingData_t *Data)
{
Dimension width, height;
Widget title = XtNameToWidget(XtParent(Data->Da), "*Title");
XmString string;
char buf[1024];

	XtVaGetValues(Data->Da,
		XmNwidth, &width,
		XmNheight, &height,
		NULL);
	sprintf(buf, "%i%%", Data->percent);
	string = XmStringCreateSimple(buf);
	XtVaSetValues(title,
		XmNlabelString, string,
		NULL);
	XmStringFree(string);
	XFillRectangle(XtDisplay(Data->Da), XtWindow(Data->Da), Data->gc,
		0, 0,
		(width * Data->percent) / 100,
		height);
	XmUpdateDisplay(Data->Da);
	Data->Id = XtAppAddTimeOut(XtWidgetToApplicationContext(Data->Da),
			    1000, (XtTimerCallbackProc)UpdateGraph, Data);
}


extern Boolean
XltWorking(Widget w, String Question, int PercentDone)
{
static Widget Dialog = NULL;
static WorkingData_t Data;
static Boolean abortflag = False;
static Boolean mapped = False;
XmString string;

	if (Dialog == NULL)
	{
	Widget shell = w;
	Widget frame;
	Widget frame_title;
	XGCValues values;

		Dialog = XmCreateWorkingDialog(shell, "Working", NULL, 0);
		XtVaSetValues(Dialog,
			XmNautoUnmanage, False,
			NULL);
		XtUnmanageChild(XmMessageBoxGetChild(Dialog, XmDIALOG_HELP_BUTTON));
		XtUnmanageChild(XmMessageBoxGetChild(Dialog, XmDIALOG_CANCEL_BUTTON));
		XtAddCallback(Dialog, XmNokCallback, (XtCallbackProc)callback, &abortflag);
		XtAddCallback(Dialog, XmNunmapCallback, (XtCallbackProc)callback, &abortflag);
		XtAddEventHandler(XtParent(Dialog),
			StructureNotifyMask,
			False,
			(XtEventHandler)StructureNotifyHandler,
			(XtPointer)&mapped);
		mapped = False;
		frame = XmCreateFrame(Dialog,"Frame",NULL,0);
		frame_title = XmCreateLabel(frame,"Title", NULL,0);
		XtVaSetValues(frame_title,
			XmNchildType, XmFRAME_TITLE_CHILD,
			NULL);
		Data.Da = XmCreateDrawingArea(frame, "Graph", NULL, 0);
		XtManageChild(frame_title);
		XtManageChild(Data.Da);
		XtManageChild(frame);
		XtRealizeWidget(Dialog);
		Data.Id = (XtIntervalId)NULL;
		XtVaGetValues(Data.Da,
			XmNforeground, &values.foreground,
			XmNbackground, &values.background,
			NULL);
		Data.gc = XCreateGC(XtDisplay(Data.Da),
				XtWindow(Data.Da),
				GCForeground | GCBackground, &values);
	}
	if (Question)
	{
	    string = XmStringCreateSimple(Question);
	    XtVaSetValues(Dialog,
		    XmNmessageString, string,
		    NULL);
	    XmStringFree(string);
	}
	if (PercentDone >= 0)
	{
	    Data.percent = PercentDone;
	}
	switch (PercentDone)
	{
	case 0:
		abortflag = False;
		XtManageChild(Dialog);
		if (Data.Id == (XtIntervalId)NULL)
		{
			XClearArea(XtDisplay(Data.Da),
				XtWindow(Data.Da),
				0, 0, 0, 0, False);
			Data.Id = XtAppAddTimeOut(XtWidgetToApplicationContext(Dialog),
				0, (XtTimerCallbackProc)UpdateGraph, &Data);
		}
		break;
	case 100:
		if (Data.Id != (XtIntervalId)NULL)
		{
			XtRemoveTimeOut(Data.Id);
			Data.Id = (XtIntervalId)NULL;
		}
		while (XtIsManaged(Dialog) && !mapped)
		{
		    XtAppProcessEvent(XtWidgetToApplicationContext(Dialog), XtIMXEvent);
		}
		UpdateGraph(&Data);
		XtRemoveTimeOut(Data.Id);
		Data.Id = (XtIntervalId)NULL;
		XtUnmanageChild(Dialog);
		while (!XtIsManaged(Dialog) && mapped)
		{
		    XtAppProcessEvent(XtWidgetToApplicationContext(Dialog), XtIMXEvent);
		}
		abortflag = False;
		break;
	default:
	    {
	    XtInputMask mask;

		while ((mask = (XtAppPending(XtWidgetToApplicationContext(Dialog)) & ~XtIMAlternateInput)))
		{
		    XtAppProcessEvent(XtWidgetToApplicationContext(Dialog), mask);
		}
		if (!XtIsManaged(Dialog))
		{
		    abortflag = True;
		}
	    }
	    break;
	}
	return(abortflag);
}
