/**
 *
 * $Id: NumEntry.h,v 1.3 2001/06/09 18:38:57 amai Exp $
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
#ifndef _NUMENTRY_H
#define _NUMENTRY_H

#include <X11/IntrinsicP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XltNUMENTRY_TEXT 1
#define XltNUMENTRY_UP 2
#define XltNUMENTRY_DN 3
#define XltNUMENTRY_LAB 4
#ifndef XltNdelay
#define XltNdelay "delay"
#endif
#ifndef XltCDelay
#define XltCDelay "Delay"
#endif
#ifndef XltNinitialDelay
#define XltNinitialDelay "initialDelay"
#endif
#ifndef XltCInitialDelay
#define XltCInitialDelay "InitialDelay"
#endif

typedef struct {
	int reason;
	XEvent *event;
	Boolean doit;
	char *value;
	double dvalue;
} XltNumEntryVerifyCallbackStruct;

extern WidgetClass xrwsNumEntryWidgetClass;

typedef struct _XltNumEntryRec *XltNumEntryWidget;
typedef struct _XltNumEntryClassRec *XmNumEntryWidgetClass;

#define XltIsNumEntry(w) XtIsSubclass((w), xrwsNumEntryWidgetClass)

extern Widget XltCreateNumEntry(Widget parent,
			     char *name,
			     Arg *arglist,
			     Cardinal argCount);
extern Widget XltVaCreateNumEntry(Widget parent, char *name, ...);
extern Widget XltNumEntryGetChild(Widget W, unsigned char child);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
