/**
 *
 * $Id: AppShellP.h,v 1.3 2001/06/09 18:38:57 amai Exp $
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

#ifndef _XLT_APPSHELLP_H
#define _XLT_APPSHELLP_H

#include <X11/ShellP.h>
#include "AppShell.h"

typedef struct {
	Colormap colormap;
	Visual  *visual;
	int      vclass;
	int      depth;
} XltAppShellClassPart;

typedef struct {
	CoreClassPart             core_class;
	CompositeClassPart        composite_class;
	ShellClassPart            shell_class;
	WMShellClassPart          wm_shell_class;
	VendorShellClassPart      vendor_shell_class;
	TopLevelShellClassPart    top_level_class;
	ApplicationShellClassPart application_class;
	XltAppShellClassPart      app_shell_class;
} XltAppShellClassRec, *XltAppShellWidgetClass;

extern XltAppShellClassRec xltAppShellClassRec;

typedef struct {
	Boolean  use_private_colormap;
	int      vclass;
	int      depth;
	VisualID vid;
	Boolean  use_editres;
} XltAppShellPart;

typedef struct {
	CorePart             core;
	CompositePart        composite;
	ShellPart            shell;
	WMShellPart          wm;
	VendorShellPart      vendor; 
	TopLevelShellPart    top_level;
	ApplicationShellPart application;
	XltAppShellPart      app_shell;
} XltAppShellRec, *XltAppShellWidget;

#endif /* _XLT_APPSHELLP_H */
