/**
 * $Id: Xlt.h,v 1.10 2004/07/02 19:04:04 rwscott Exp $
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

#ifndef _XLT_H
#define _XLT_H

#include <assert.h>
#include <X11/IntrinsicP.h>
#include <Xm/PushB.h>

#define XltCheckArgSize(a,n) \
        assert(sizeof(a)/sizeof(a[0]) <= n)

#define	XltGetValues(w,a,n) { \
	XltCheckArgSize(a, n); \
	XtGetValues(w, a, n); \
}

#define	XltSetValues(w,a,n) { \
	XltCheckArgSize(a, n); \
	XtSetValues(w, a, n); \
}

#ifdef __cplusplus
extern "C" {
#endif

void XltDisplayFallbackResources(char **Fallback);
void XltDisplayOptions(XrmOptionDescRec *opTable, Cardinal num_options);
Boolean XltYesNo(Widget w, String Question);
Boolean XltWorking(Widget w, String Question, int PercentDone);
void XltWaitTillMapped(Widget w);
void XltSetClientIcon(Widget W, char **icon);

Widget XltToolBarAddItem(Widget ToolBar, char *Label, char **PixmapData);
void XltToolBarConfigure(Widget w, Widget ToolBar);

int  XltRedirectStdErr(Widget Parent);
void XltSelectDebugLevel(Widget W, int *DebugLevelPtr, XmPushButtonCallbackStruct *reason);

void XltRdbPutString(Widget w, String name, String value);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XLT_H */
