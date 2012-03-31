/*
 * WlSplashScreen.c
 *	Implementation of the WlSplashScreen widget.
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
#include <wlib/rcs.h>
MU_ID("$Mu: wlib/WlSplashScreen.c 1.12 1998/08/27 04:24:47 $")

#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <Xm/Label.h>

#include <wlib/WlSplashScreenP.h>
#include <wlib/wlibP.h>

static void initialize(Widget, Widget, ArgList, Cardinal *);
static void realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void destroy(Widget);

#define	core_background(w) \
	(((WlSplashScreenWidget)(w))->core.background_pixel)

#define	is_iconic(w) \
	(((WlSplashScreenWidget)(w))->top_level.iconic)

#define	splash_pixfile(w) \
	(((WlSplashScreenWidget)(w))->wl_splash.splash_pixfile)
#define	splash_bitfile(w) \
	(((WlSplashScreenWidget)(w))->wl_splash.splash_bitfile)
#define	splash_show(w) \
	(((WlSplashScreenWidget)(w))->wl_splash.show_splash)
#define	splash_window(w) \
	(((WlSplashScreenWidget)(w))->wl_splash.splash)
#define	splash_usemono(w) \
	(((WlSplashScreenWidget)(w))->wl_splash.use_mono)

#define OFFSET(field) XtOffset(WlSplashScreenWidget, wl_splash.field)
static XtResource resources[] =
{
	{
		XwlNsplashPixmap, XwlCSplashPixmap,
		XtRString, sizeof(String), OFFSET(splash_pixfile),
		XtRImmediate, (XtPointer)NULL
	}, {
		XwlNsplashBitmap, XwlCSplashBitmap,
		XtRString, sizeof(String), OFFSET(splash_bitfile),
		XtRImmediate, (XtPointer)NULL
	}, {
		XwlNshowSplashScreen, XwlCShowSplashScreen,
		XtRBoolean, sizeof(Boolean), OFFSET(show_splash),
		XtRImmediate, (XtPointer)False
	}, {
		XwlNuseMonoSplash, XwlCUseMonoSplash,
		XtRBoolean, sizeof(Boolean), OFFSET(use_mono),
		XtRImmediate, (XtPointer)False
	}
};
#undef OFFSET

WlSplashScreenClassRec wlSplashScreenClassRec = 
{
	{	/** Core **/
	/* superclass		*/	(WidgetClass)&applicationShellClassRec,
	/* class_name		*/	"WlSplashScreen",
	/* widget_size		*/	sizeof(WlSplashScreenRec),
	/* class_initialize	*/	NULL,
	/* class_initialize_hook*/		NULL,
	/* class_inited		*/	False,
	/* initialize		*/	initialize,
	/* initialize_hook	*/	NULL,
	/* realize		*/	realize,
	/* actions		*/	NULL,
	/* num_actions		*/	0,
	/* resources		*/	resources,
	/* num_resources	*/	XtNumber(resources),
	/* xrm_class		*/	NULLQUARK,
	/* compress_motion	*/	False,
	/* compress_exposures	*/	False,
	/* compress_enterleave	*/	False,
	/* visible_interest	*/	False,
	/* destroy		*/	destroy,
	/* resize		*/	XtInheritResize,
	/* expose		*/	XtInheritExpose,
	/* set_values		*/	NULL,
	/* set_values_hook	*/	NULL,
	/* set_values_almost	*/	XtInheritSetValuesAlmost,
	/* get_values_hook	*/	NULL,
	/* accept_focus		*/	XtInheritAcceptFocus,
	/* version		*/	XtVersion,
	/* callback_private	*/	NULL,
	/* tm_table		*/	NULL,
	/* query_geometry	*/	XtInheritQueryGeometry,	
	/* display_accelerator	*/	XtInheritDisplayAccelerator,
	/* extension		*/	NULL,
	},

	{	/** Composite **/
	/* geometry_manager	*/	XtInheritGeometryManager,
	/* change_managed	*/	XtInheritChangeManaged,
	/* insert_child		*/	XtInheritInsertChild,
	/* delete_child		*/	XtInheritDeleteChild,
		NULL,
	},

	{	/** Shell **/
	/* extension		*/	0,
	},

	{	/** WindowManagerShell **/
	/* extension		*/	0,
	},

	{	/** VendorShell **/
	/* extension		*/	NULL,
	},

	{	/** TopLevelShell **/
	/* extension		*/	NULL,
	},

	{	/** ApplicationShell **/
	/* extension		*/	NULL,
	},

	{	/** WlSplashScreen **/
	/* extension		*/	NULL,
	}
};

WidgetClass wlSplashScreenWidgetClass = (WidgetClass)&wlSplashScreenClassRec;

static Pixmap
loadpix(Widget w)
{
	if(splash_usemono(w)
	|| splash_pixfile(w) == NULL)
		return None;
	return wlp_xpm_from_file(w, XtDisplay(w), core_background(w),
							splash_pixfile(w));
}

static Pixmap
loadbit(Widget w)
{
	if(splash_bitfile(w) == NULL)
		return None;
	return wlp_xbm_from_file(NULL, XtDisplay(w), splash_bitfile(w));
}

#define drawable_geom(dpy, d, width, height, depth) \
	{ \
		Window		root; \
		unsigned	border; \
		int		x, y; \
		XGetGeometry(dpy, d, &root, &x, &y, width, height, \
							&border, depth); \
	}

static void
initialize(Widget inw, Widget w, ArgList a, Cardinal *n)
{
	XSetWindowAttributes xa;
	Display		*dpy = XtDisplay(w);
	Screen		*scr = DefaultScreenOfDisplay(dpy);
	unsigned long	mask = 0;
	unsigned	wd, ht, depth;
	int		x, y;
	GC		gc;
	Pixmap		pix;
	USEUP(inw); USEUP(a); USEUP(n);

	splash_window(w) = None;

	if(is_iconic(w)
	|| !splash_show(w))
		return;

	if((pix = loadpix(w)) == None
	&& (pix = loadbit(w)) == None) {
		destroy(w);
		return;
	}

	drawable_geom(dpy, pix, &wd, &ht, &depth);
	x = (WidthOfScreen(XtScreen(w))  - wd)/2;
	y = (HeightOfScreen(XtScreen(w)) - ht)/2;

	mask |= CWOverrideRedirect;
	xa.override_redirect = TRUE;

	splash_window(w) = XCreateWindow(dpy, RootWindowOfScreen(scr),
					x, y, wd, ht, 0,
					DefaultDepthOfScreen(scr),
					InputOutput,
					DefaultVisualOfScreen(scr),
					mask, &xa);

	XSetTransientForHint(dpy, splash_window(w), RootWindowOfScreen(scr));
	XDefineCursor(dpy, splash_window(w), XCreateFontCursor(dpy, XC_watch));
	XMapWindow(dpy, splash_window(w));

	gc = XCreateGC(dpy, splash_window(w), 0, NULL);
	if(depth != 1)
		XCopyArea(dpy, pix, splash_window(w), gc, 0, 0, wd, ht, 0, 0);
	else
		XCopyPlane(dpy, pix, splash_window(w), gc, 0, 0, wd, ht, 0,0,1);
	XFreeGC(dpy, gc);
	XFreePixmap(dpy, pix);
	XSync(dpy, False);
}

/*
 * Note the we never actually realize the widget, the window for this
 * thing never gets made.  This could be construed as an abuse of
 * Xt but what the hell.  You should (obviously) never even try
 * to realize one of these widgets.
 */
static void
realize(Widget w, XtValueMask *m, XSetWindowAttributes *wa)
{
	USEUP(w); USEUP(m); USEUP(wa);
	assert("aha! no one expects a realized splash screen widget!" != NULL);
}

static void
destroy(Widget w)
{
	if(splash_window(w) == None)
		return;
	XDestroyWindow(XtDisplay(w), splash_window(w));
	splash_window(w) = None;
}
