/*
 * xpm.c
 *	Xpm wrappers.
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
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: wlib/xpm.c 1.3.2.1 1998/09/11 02:41:48 $")

#include <limits.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/XmStrDefs.h>
#if defined(HAVE_XPM) && HAVE_XPM
#	if defined(HAVE_X11_XPM_H) && HAVE_X11_XPM_H
#		include <X11/xpm.h>
#	else
#		include <xpm.h>
#	endif
#endif

#include <wlib/wlib.h>
#include <wlib/wlibP.h>
#include <wlib/sanity.h>

#if HAVE_XPM

Pixmap
wlp_xpm_from_file(Widget w, Display *dpy, Pixel bg, char *name)
{
	XpmColorSymbol c[2];
	XpmAttributes  attr;
	int            status;
	Drawable       d;
	Pixmap         p;

	memset((void *)&c[0], '\0', sizeof(c));
	c[0].name  = "none";	c[1].name  = "None";
	c[0].value = NULL;	c[1].value = NULL;
	c[0].pixel = bg;	c[1].pixel = bg;

	memset((void *)&attr, '\0', sizeof(attr));
	attr.colorsymbols = c;
	attr.numsymbols   = sizeof(c)/sizeof(c[0]);
	attr.closeness    = SHRT_MAX;
	attr.valuemask   |= XpmColorSymbols | XpmCloseness;

	if(w == NULL || !XtIsRealized(w)) {
		d = RootWindow(dpy, DefaultScreen(dpy));
	}
	else {
		Arg      a[3];
		Cardinal n;

		dpy = XtDisplay(w);
		d   = XtWindow(w);

		n = 0;
		XtSetArg(a[n], XmNdepth,    &attr.depth);	++n;
		XtSetArg(a[n], XmNcolormap, &attr.colormap);	++n;
		XtSetArg(a[n], XmNvisual,   &attr.visual);	++n;
		wlGetValues(wl_shell(w), a, n);

		attr.valuemask |= XpmVisual | XpmColormap | XpmDepth;
	}

	status = XpmReadFileToPixmap(dpy, d, name, &p, NULL, &attr);
	XpmFreeAttributes(&attr);
	return status == Success ? p : None;
}

Pixmap
wlp_xpm_from_data(Widget w, Display *dpy, Pixel bg, char **data)
{
	XpmColorSymbol c[2];
	XpmAttributes  attr;
	int            status;
	Drawable       d;
	Pixmap         p;

	memset((void *)&c[0], '\0', sizeof(c));
	c[0].name  = "none";	c[1].name  = "None";
	c[0].value = NULL;	c[1].value = NULL;
	c[0].pixel = bg;	c[1].pixel = bg;

	memset((void *)&attr, '\0', sizeof(attr));
	attr.colorsymbols = c;
	attr.numsymbols   = sizeof(c)/sizeof(c[0]);
	attr.closeness    = SHRT_MAX;
	attr.valuemask   |= XpmColorSymbols | XpmCloseness;

	if(w == NULL || !XtIsRealized(w)) {
		d = RootWindow(dpy, DefaultScreen(dpy));
	}
	else {
		Arg      a[3];
		Cardinal n;

		dpy = XtDisplay(w);
		d   = XtWindow(w);

		n = 0;
		XtSetArg(a[n], XmNdepth,    &attr.depth);	++n;
		XtSetArg(a[n], XmNcolormap, &attr.colormap);	++n;
		XtSetArg(a[n], XmNvisual,   &attr.visual);	++n;
		wlGetValues(wl_shell(w), a, n);

		attr.valuemask |= XpmVisual | XpmColormap | XpmDepth;
	}

	status = XpmCreatePixmapFromData(dpy, d, data, &p, NULL, &attr);
	XpmFreeAttributes(&attr);
	return status == Success ? p : None;
}

#else	/* !HAVE_XPM */

Pixmap
wlp_xpm_from_file(Widget w, Display *dpy, Pixel bg, char *name)
{
	USEUP(w); USEUP(dpy); USEUP(bg); USEUP(name);
	return None;
}

Pixmap
wlp_xpm_from_data(Widget w, Display *dpy, Pixel bg, char **data)
{
	USEUP(w); USEUP(dpy); USEUP(bg); USEUP(data);
	return None;
}

#endif
