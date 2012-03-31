/*
 * WlAppShell.h
 *	Public header for the WlAppShell widget.
 *
 * Copyright (C) 1997 Eric A. Howe
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
 *
 * This widget is loosely based an the AppPlusS widget by John L. Cwikla
 * <cwikla@wri.com>.
 */
#ifndef WLIB_WLAPPSHELL_H
#define WLIB_WLAPPSHELL_H
MU_HID(wlib_wlappshell_h, "$Mu: wlib/wlib/WlAppShell.h 1.5 1998/08/16 09:44:55 $")

#define	XwlNvisualClass	"wlVisualClass"
#define	XwlCVisualClass	"WlVisualClass"
#define	XwlRVisualClass	"WlVisualClass"

#define	XwlNvisualID	"wlVisualID"
#define	XwlCVisualID	"WlVisualID"
#define	XwlRVisualID	"WlVisualID"

#define	XwlNdepth	"wlDepth"
#define	XwlCDepth	"WlDepth"
#define	XwlRDepth	"WlDepth"

#define	XwlNusePrivateColormap	"wlUsePrivateColormap"
#define	XwlCUsePrivateColormap	"WlUsePrivateColormap"

#define	XwlNuseEditRes	"wlUseEditRes"
#define	XwlCUseEditRes	"WlUseEditRes"

extern WidgetClass wlAppShellWidgetClass;

#define XwlIsWlAppShell(a) (XtIsSubclass(a, wlAppShellWidgetClass))

#endif
