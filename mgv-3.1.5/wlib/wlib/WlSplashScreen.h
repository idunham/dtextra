/*
 * WlSplashScreen.h
 *	Public header for the WlSplashScreen widget.
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
#ifndef WLIB_WLSPLASHSCREEN_H
#define WLIB_WLSPLASHSCREEN_H
MU_HID(wlib_wlsplashscreen_h, "$Mu: wlib/wlib/WlSplashScreen.h 1.4 1998/08/16 09:44:55 $")

/**
 ** NEVER REALIZE ONE OF THESE!
 **
 ** Just create one with XtAppCreateShell ASAP and destroy it when
 ** the real actions starts.  This widget subclasses applicationShell
 ** because we want to be able to create one without having to worry
 ** about a parent widget of any kind.  All this widget does is create
 ** a window (via raw Xlib) with a pixmap or bitmap in it.
 **
 ** This widget is, to some extent, an abuse of Xt but so is XmDisplay
 ** so we've got some kind of precedent.
 **/

#define	XwlNsplashPixmap	"wlSplashPixmap"
#define	XwlCSplashPixmap	"WlSplashPixmap"

#define	XwlNsplashBitmap	"wlSplashBitmap"
#define	XwlCSplashBitmap	"WlSplashBitmap"

#define	XwlNshowSplashScreen	"wlShowSplashScreen"
#define	XwlCShowSplashScreen	"WlShowSplashScreen"

#define	XwlNuseMonoSplash	"wlUseMonoSplash"
#define	XwlCUseMonoSplash	"WlUseMonoSplash"

extern WidgetClass wlSplashScreenWidgetClass;

#define XwlIsWlSplashScreen(a) (XtIsSubclass(a, wlSplashScreenWidgetClass))

#endif
