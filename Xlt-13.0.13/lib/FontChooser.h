/**
 *
 * $Header: /cvsroot/xlt/Xlt/lib/FontChooser.h,v 1.5 2001/06/09 18:38:57 amai Exp $
 *
 * Copyright (C) 1995 Free Software Foundation, Inc.
 * Copyright © 1999-2001 by the LessTif developers.
 *
 * This file is part of the GNU LessTif Library.
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

#ifndef _XLT_FONTCHOOSER_H
#define _XLT_FONTCHOOSER_H

#include <Xm/BulletinB.h>

#ifdef __cplusplus
extern "C" {
#endif

extern WidgetClass xltFontChooserWidgetClass;

typedef struct _XltFontChooserRec *XltFontChooserWidget;
typedef struct _XltFontChooserClassRec *XltFontChooserWidgetClass;

extern Widget XltCreateFontChooser(Widget parent, char *name, Arg *arglist, Cardinal argCount);
extern Widget XltCreateFontChooserDialog(Widget parent, char *name, Arg *arglist, Cardinal argCount);

#ifndef XltIsFontChooser
#define XltIsFontChooser(w) XtIsSubclass((w), xltFontChooserWidgetClass)
#endif

#define	XltNshowDemo	"showDemo"
#define	XltCShowDemo	"ShowDemo"

#define	XltNdemoString	"demoString"
#define	XltCDemoString	"DemoString"

#define	XltNcountString	"countString"
#define	XltCCountString	"CountString"

#define	XltNfieldNames	"fieldNames"
#define	XltCFieldNames	"FieldNames"

#define	XltNfontName	"fontName"
#define	XltCFontName	"FontName"

#define	XltNpattern	"pattern"
#define	XltCPattern	"Pattern"

#define	XltNsortCriteria	"sortCriteria"
#define	XltCSortCriteria	"SortCriteria"

#ifdef __cplusplus
}
#endif

#endif /* _XLT_FONTCHOOSER_H */
