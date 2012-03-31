/*
 * WlAppShellP.h
 *	Private header for the WlAppShell widget.
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
#ifndef WLIB_WLAPPSHELLP_H
#define WLIB_WLAPPSHELLP_H
MU_HID(wlib_wlappshellp_h, "$Mu: wlib/wlib/WlAppShellP.h 1.7 1998/03/28 20:55:33 $")

#include <X11/ShellP.h>
#include <wlib/WlAppShell.h>

typedef struct {
	Colormap	colormap;
	Visual		*visual;
	int		vclass;
	int		depth;
} WlAppShellClassPart;

typedef struct {
	CoreClassPart			core_class;
	CompositeClassPart		composite_class;
	ShellClassPart			shell_class;
	WMShellClassPart		wm_shell_class;
	VendorShellClassPart		vendor_shell_class;
	TopLevelShellClassPart		top_level_class;
	ApplicationShellClassPart	application_class;
	WlAppShellClassPart		wl_app_shell_class;
} WlAppShellClassRec, *WlAppShellWidgetClass;

extern WlAppShellClassRec wlAppShellClassRec;

typedef struct {
	Boolean		use_private_colormap;
	int		vclass;
	int		depth;
	VisualID	vid;
	Boolean		use_editres;
} WlAppShellPart;

typedef struct {
	CorePart		core;
	CompositePart		composite;
	ShellPart		shell;
	WMShellPart		wm;
	VendorShellPart		vendor; 
	TopLevelShellPart	top_level;
	ApplicationShellPart	application;
	WlAppShellPart		wl_app_shell;
} WlAppShellRec, *WlAppShellWidget;
 
#endif
