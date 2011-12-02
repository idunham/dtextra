/**
 *
 * $Id: HostTranscript.c,v 1.15 2005/05/20 11:57:01 rwscott Exp $
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
#include <string.h>
#include <errno.h>

#include <Xm/Text.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <Xm/RowColumn.h>
#include <Xm/FileSB.h>

#include <HostP.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static void
Modify(Widget Transcript, Widget Host, XmTextVerifyCallbackStruct *call_data)
{
String tmp, tmp1;
int left;

	tmp = call_data->text->ptr;
	left = call_data->text->length;
	/*
	printf("%s:%s(%d) - %p %i %i %i\n", 
	    __FILE__, __FUNCTION__, __LINE__, 
	    call_data->event,
	    left,
	    call_data->startPos, call_data->endPos);
	    */
	if (left == 0 && call_data->startPos == 0 && call_data->endPos == XmTextGetLastPosition(Transcript))
	{
	}
	else
	{
	    while ((tmp1 = memchr(tmp, '\n', left)) != NULL)
	    {
		if (tmp != tmp1)
		{
			XltHostSendData(Host, tmp, tmp1 - tmp + 1);
		}
		XltHostSendString(Host, "");
		left -= (tmp1 - tmp + 1);
		tmp = tmp1;
		tmp++;
	    }
	    XltHostSendData(Host, tmp, left);
	    call_data->newInsert = XmTextGetLastPosition(Transcript) + call_data->text->length;
	    call_data->startPos = XmTextGetLastPosition(Transcript);
	    call_data->endPos = XmTextGetLastPosition(Transcript);
	}
}

static void
AsciiInput(Widget Host, Widget Transcript, XltHostCallbackStruct *call_data)
{
	XtRemoveCallback(Transcript, XmNmodifyVerifyCallback, (XtCallbackProc)Modify, Host);
	XmTextInsert(Transcript,
		XmTextGetLastPosition(Transcript),
		call_data->data);
	XmTextInsert(Transcript,
		XmTextGetLastPosition(Transcript),
		"\n");
	XmTextSetInsertionPosition(Transcript, 
		XmTextGetLastPosition(Transcript));
	XtAddCallback(Transcript, XmNmodifyVerifyCallback, (XtCallbackProc)Modify, Host);
}

static void
ClearTranscript(Widget w, XtPointer client_data, XtPointer call_data)
{
Widget text = (Widget)client_data;

    XmTextSetString(text, "");
}

static void
_SaveTranscript(Widget w, XtPointer client_data, XtPointer call_data)
{
Widget host = (Widget)client_data;
XmFileSelectionBoxCallbackStruct *cbs = (XmFileSelectionBoxCallbackStruct *)call_data;
String file;

    if (XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &file))
    {
    FILE *File;

	File = fopen(file, "w");
	if (File)
	{
	String value;
	WidgetList kids;
	Cardinal numKids;
	Widget cancel;

	    XtVaGetValues(w,
		XmNchildren, &kids,
		XmNnumChildren, &numKids,
		NULL);
	    cancel = XmFileSelectionBoxGetChild(w, XmDIALOG_CANCEL_BUTTON);
	    for (; numKids >0; numKids--)
	    {
		if (kids[numKids - 1] != cancel)
		{
		    XtSetSensitive(kids[numKids - 1], False);
		}
	    }
	    value = XmTextGetString(Host_Transcript(host));
	    fwrite(value, 1, strlen(value), File);
	    XtFree(value);
	    fclose(File);
	    XtUnmanageChild(w);
	}
	else
	{
	    fprintf(stderr, "%s:%s(%d) - %s - %s\n",
		__FILE__, __FUNCTION__, __LINE__,
		file,
		strerror(errno));
	    XtUnmanageChild(w);
	}
	XtFree(file);
    }
}

static void
SaveTranscript(Widget w, XtPointer client_data, XtPointer call_data)
{
Widget host = (Widget)client_data;
WidgetList kids;
Cardinal numKids;

    if (Host_SaveDialog(host) == NULL)
    {
    Widget shell = w;

    	while (XtParent(shell)) shell = XtParent(shell);
    	Host_SaveDialog(host) = XmCreateFileSelectionDialog(shell, "SaveTranscript", NULL, 0);
	XtAddCallback(Host_SaveDialog(host), XmNcancelCallback, (XtCallbackProc)XtUnmanageChild, Host_SaveDialog(host));
	XtAddCallback(Host_SaveDialog(host), XmNokCallback, (XtCallbackProc)_SaveTranscript, host);
    }
    XtVaGetValues(Host_SaveDialog(host),
    	XmNchildren, &kids,
    	XmNnumChildren, &numKids,
    	NULL);
    for (; numKids >0; numKids--)
    {
    	XtSetSensitive(kids[numKids - 1], True);
    }
    XtManageChild(Host_SaveDialog(host));
}

static void
transcript_destroy(Widget w, XtPointer client_data, XtPointer call_data)
{
Widget host = (Widget)client_data;

    Host_Transcript(host) = NULL;
}

Widget
XltHostCreateTranscript(Widget parent, Widget Host, Arg *args_in, Cardinal n_in)
{
Widget Popup;
Widget Button;
ArgList merged;
Arg new_args[] = {
    {XmNeditMode, XmMULTI_LINE_EDIT},
    {XmNpopupEnabled, XmPOPUP_AUTOMATIC},
};

    merged = XtMergeArgLists(new_args, XtNumber(new_args), args_in, n_in);
    Host_Transcript(Host) = XmCreateScrolledText(parent, "HostTranscript", merged, n_in + XtNumber(new_args));
    XtAddCallback(Host_Transcript(Host), XtNdestroyCallback, (XtCallbackProc)transcript_destroy, Host);
    Popup = XmCreatePopupMenu(Host_Transcript(Host), "HostTranscriptPopup", merged, n_in + XtNumber(new_args));
    Button = XmCreateLabel(Popup, "TranscriptFunctions", NULL, 0);
    XtManageChild(Button);
    Button = XmCreateSeparator(Popup, "TranscriptFunctionsSeparator", NULL, 0);
    XtManageChild(Button);
    Button = XmCreatePushButton(Popup, "Clear", NULL, 0);
    XtAddCallback(Button, XmNactivateCallback, (XtCallbackProc)ClearTranscript, Host_Transcript(Host));
    XtManageChild(Button);
    Button = XmCreatePushButton(Popup, "Save", NULL, 0);
    XtAddCallback(Button, XmNactivateCallback, (XtCallbackProc)SaveTranscript, Host);
    XtManageChild(Button);
    XtFree((char *)merged);
    XtAddCallback(Host_Transcript(Host), XmNmodifyVerifyCallback, (XtCallbackProc)Modify, Host);
    XtAddCallback(Host, XltNasciiInputCallback, (XtCallbackProc)AsciiInput, Host_Transcript(Host));
    return(Host_Transcript(Host));
}
