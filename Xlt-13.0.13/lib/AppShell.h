/**
 *
 * $Id: AppShell.h,v 1.2 2001/06/09 18:38:57 amai Exp $
 *
 * Copyright (C) 1998 Eric A. Howe
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
 * This widget is loosely based an the AppPlusS widget by John L. Cwikla
 * <cwikla@wri.com>.
 **/

#ifndef _XLT_APPSHELL_H
#define _XTL_APPSHELL_H

#ifdef __cplusplus
extern "C" {
#endif

#define	XltNappVisualClass	"appVisualClass"
#define	XltCAppVisualClass	"AppVisualClass"
#define	XltRAppVisualClass	"AppVisualClass"

#define	XltNappVisualID	"appVisualID"
#define	XltCAppVisualID	"AppVisualID"
#define	XltRAppVisualID	"AppVisualID"

#define	XltNappDepth	"appDepth"
#define	XltCAppDepth	"AppDepth"
#define	XltRAppDepth	"AppDepth"

#define	XltNappUsePrivateColormap	"appUsePrivateColormap"
#define	XltCAppUsePrivateColormap	"AppUsePrivateColormap"

#define	XltNappUseEditres	"appUseEditRes"
#define	XltCAppUseEditres	"AppUseEditRes"

extern WidgetClass xltAppShellWidgetClass;

#define XltIsAppShell(a) (XtIsSubclass(a, xltAppShellWidgetClass))

/*
 * Drop-in XrmOptionDescRec list for command lines.
 */
#define XltAppShellXrmOptions \
	{"-depth",       "*appDepth",              XrmoptionSepArg, NULL},    \
	{"-editres",     "*appUseEditRes",         XrmoptionNoArg,  "True"},  \
	{"-install",     "*appUsePrivateColormap", XrmoptionNoArg,  "True"},  \
	{"-noeditres",   "*appUseEditRes",         XrmoptionNoArg,  "False"}, \
	{"-noinstall",   "*appUsePrivateColormap", XrmoptionNoArg,  "False"}, \
	{"-noprivatecm", "*appUsePrivateColormap", XrmoptionNoArg,  "False"}, \
	{"-privatecm",   "*appUsePrivateColormap", XrmoptionNoArg,  "True"},  \
	{"-visual",      "*appVisualClass",        XrmoptionSepArg, NULL},    \
	{"-visualid",    "*appVisualID",           XrmoptionSepArg, NULL},

/*
 * Replacement for XtAppInitialize which uses an XltAppShell instead of Xt's
 * ApplicationShell.  Note that R6 applications should be using
 * XtOpenApplication instead (XtOpenApplication also allows you to specify
 * the app shell widget class BTW).
 */
extern Widget XltAppInitialize(XtAppContext *ac, String cls,
		XrmOptionDescList options, Cardinal num_options,
		int *argc_in_out, String *argv_in_out, String *fallbacks,
		ArgList args, Cardinal num_args);

Widget XltCreateAppShell(Widget parent, char *name, Arg *al, Cardinal ac);
		
#ifdef __cplusplus
}
#endif

#endif /* _XTL_APPSHELL_H */
