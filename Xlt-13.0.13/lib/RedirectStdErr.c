/**
 *
 * $Id: RedirectStdErr.c,v 1.10 2004/09/24 15:00:05 rwscott Exp $
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
#include <unistd.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include <Xm/MessageB.h>
#include <Xm/Text.h>

#include <Xlt.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static const char rcsid[] = "$Id: RedirectStdErr.c,v 1.10 2004/09/24 15:00:05 rwscott Exp $";

/* ******************** */

static Widget 
CreateDialog(Widget Parent)
{
    Widget Text = NULL;
    Widget Shell = Parent;
    Widget StdErrShell;
    Arg args[2];
    Cardinal n = 0;
    String title;
    Atom titleEncoding;

    while (XtParent(Shell) && !XtIsTopLevelShell(Shell))
	Shell = XtParent(Shell);
    XtVaGetValues(Shell,
    	XmNtitle, &title,
    	XmNtitleEncoding, &titleEncoding,
    	NULL);
    StdErrShell = XmCreateMessageDialog(Shell, "StdErrShell", NULL, 0);
    XtVaSetValues(XtParent(StdErrShell),
    	XmNtitle, title,
    	XmNtitleEncoding, titleEncoding,
    	NULL);
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    XtSetArg(args[n], XmNeditable, False); n++;
    XltCheckArgSize(args, n);
    Text = XmCreateScrolledText(StdErrShell, "StdErrText", args, n);
    XtManageChild(Text);
    return (Text);
}

/* ******************** */

static void 
StdErrInput(Widget Parent, int *Fd, XtInputId *Id)
{
    static char data[1024];
    static Widget Text = NULL;
    int n;
    fd_set rfds;
    struct timeval tv;

    if (Text == NULL)
    {
	Text = CreateDialog(Parent);
    }

    FD_ZERO(&rfds);
    FD_SET(*Fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    if (select(*Fd + 1, SELECT_TYPE_ARG234(&rfds), NULL, NULL, &tv))
    {
	XtManageChild(XtParent(XtParent(Text)));
	n = read(*Fd, &data[0], sizeof(data) - 1);
	data[n] = '\0';
	XmTextInsert(Text, XmTextGetLastPosition(Text), data);
	XmTextSetInsertionPosition(Text, XmTextGetLastPosition(Text));
    }
}

/* ******************** */

int
XltRedirectStdErr(Widget Parent)
{
    int NewStdErr[2];
    int pipe_rc;

    pipe_rc=pipe(NewStdErr);
    if (pipe_rc!=0) {
       perror("XltRedirectStdErr()-Can't open pipe:");
       return (1);
    } else {
       int dup2_rc;

       XtAppAddInput(XtWidgetToApplicationContext(Parent),
	  	  NewStdErr[0],
		  (caddr_t)XtInputReadMask,
		  (XtInputCallbackProc)StdErrInput,
		  (caddr_t)Parent);
       dup2_rc=dup2(NewStdErr[1], 2);
       close(NewStdErr[1]);
       return 0;
    }
}
