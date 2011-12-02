/**
 *
 * $Id: FilePicker.h,v 1.2 2001/06/09 18:38:57 amai Exp $
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
#ifndef _FILEPICKER_H
#define _FILEPICKER_H

#include <X11/IntrinsicP.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XltNfileSelectionDialog
#define XltNfileSelectionDialog "fileSelectionDialog"
#endif
#ifndef XltCFileSelectionDialog
#define XltCFileSelectionDialog "FileSelectionDialog"
#endif

#define XltFILEPICKER_TEXT 1
#define XltFILEPICKER_BUTTON 2
#define XltFILEPICKER_LAB 3

#if 0
typedef struct {
	int reason;
	XEvent *event;
	Boolean doit;
	char *value;
	double dvalue;
} XltFilePickerCallbackStruct;
#endif

extern WidgetClass xltFilePickerWidgetClass;

typedef struct _XltFilePickerRec *XltFilePickerWidget;
typedef struct _XltFilePickerClassRec *XltFilePickerWidgetClass;

#define XltIsFilePicker(w) XtIsSubclass((w), xltFilePickerWidgetClass)

extern Widget XltCreateFilePicker(Widget parent,
			     char *name,
			     Arg *arglist,
			     Cardinal argCount);
extern Widget XltFilePickerGetChild(Widget W, unsigned char child);
extern String XltFilePickerGetString(Widget w);
extern void XltFilePickerSetString(Widget w, String string);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
