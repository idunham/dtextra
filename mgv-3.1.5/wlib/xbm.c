/*
 * xbm.c
 *	Xbm wrappers.
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
MU_ID("$Mu: wlib/xbm.c 1.1 1998/08/23 06:29:23 $")

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Xm/XmStrDefs.h>

#include <wlib/wlibP.h>
#include <wlib/sanity.h>

Pixmap
wlp_xbm_from_file(Widget w, Display *dpy, char *name)
{
	Drawable d;
	Pixmap   b;
	unsigned width, height;
	int      hx, hy, status;

	if(w != NULL) {
		dpy = XtDisplay(w);
		d   = XtWindow(w);
	}
	else {
		d = RootWindow(dpy, DefaultScreen(dpy));
	}

	status = XReadBitmapFile(dpy, d, name, &width, &height, &b, &hx, &hy);
	return status == Success ? b : None;
}

Pixmap
wlp_xbm_from_data(Widget w, Display *dpy, Pixel fg, Pixel bg, char *data,
								int wd, int ht)
{
	Arg a;
	int depth;

	if(w == NULL)
		return XCreateBitmapFromData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					data, wd, ht);

	dpy = XtDisplay(w);

	XtSetArg(a, XmNdepth, &depth);
	XtGetValues(w, &a, 1);

	return XCreatePixmapFromBitmapData(dpy, XtWindow(w),
						data, wd, ht, fg, bg, depth);
}
