/*
 * WlSplashScreenP.h
 *	Private header for the WlSplashScreen widget.
 *
 * Copyright (C) 1998 Eric A. Howe
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
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#ifndef WLIB_WLSPLASHSCREENP_H
#define WLIB_WLSPLASHSCREENP_H
MU_HID(wlib_wlsplashscreenp_h, "$Mu: wlib/wlib/WlSplashScreenP.h 1.3 1998/04/06 06:05:03 $")

#include <X11/ShellP.h>
#include <wlib/WlSplashScreen.h>

typedef struct {
	XtPointer	extension;
} WlSplashScreenClassPart;

typedef struct {
	CoreClassPart			core_class;
	CompositeClassPart		composite_class;
	ShellClassPart			shell_class;
	WMShellClassPart		wm_shell_class;
	VendorShellClassPart		vendor_shell_class;
	TopLevelShellClassPart		top_level_class;
	ApplicationShellClassPart	application_class;
	WlSplashScreenClassPart		wl_splash_class;
} WlSplashScreenClassRec, *WlSplashScreenWidgetClass;

extern WlSplashScreenClassRec wlSplashScreenClassRec;

typedef struct {
	String	splash_pixfile;
	String	splash_bitfile;
	Boolean	show_splash;
	Boolean	use_mono;

	Window	splash;
} WlSplashScreenPart;

typedef struct {
	CorePart		core;
	CompositePart		composite;
	ShellPart		shell;
	WMShellPart		wm;
	VendorShellPart		vendor; 
	TopLevelShellPart	top_level;
	ApplicationShellPart	application;
	WlSplashScreenPart	wl_splash;
} WlSplashScreenRec, *WlSplashScreenWidget;
 
#endif
