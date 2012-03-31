/*
 * icons.c
 *	Icon handling for wlib.  This file exists to cleanly isolate
 *	the code that depends on the Xpm library.
 *
 * Copyright (C) 1996  Eric A. Howe
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: wlib/icons.c 1.13 1998/08/23 22:26:25 $")

#include <assert.h>
#include <limits.h>
#include <X11/Intrinsic.h>
#include <Xm/XmStrDefs.h>
#include <X11/Shell.h>

#include <wlib/wlib.h>
#include <wlib/wlibP.h>
#include <wlib/icons.h>
#include <wlib/sanity.h>

static WL_ICONS icons = {
	NULL,
	NULL, 0, 0,
	NULL, 0, 0,
};

Pixmap
wl_icon(Widget w)
{
	Pixmap	pix  = None;
	Display *dpy = XtDisplay(w);
	Pixel   fg, bg;

	wlp_fgbg(w, &fg, &bg);

	if(icons.color != NULL
	&& (pix = wlp_xpm_from_data(w, dpy, bg, icons.color)) != None)
		return pix;

	return wlp_xbm_from_data(w, dpy, fg, bg, icons.bits,
						icons.width, icons.height);
}

typedef struct {
	Boolean	install_icon;
	Boolean	use_color;
} SLOP;
int
wl_icons(Widget top, Pixmap *iconret, Pixmap *maskret)
{
	Pixmap      icon, mask;
	Pixel       fg, bg;
	Arg         a[2];
	Cardinal    n;
	Display	   *dpy = XtDisplay(top);
	SLOP        v;
	XtResource  r[] = {{
		XwlNinstallIcon, XwlCInstallIcon,
		XtRBoolean, sizeof(Boolean), XtOffsetOf(SLOP, install_icon),
		XtRImmediate, (XtPointer)True
	}, {
		XwlNuseColorIcon, XwlCUseColorIcon,
		XtRBoolean, sizeof(Boolean), XtOffsetOf(SLOP, use_color),
		XtRImmediate, (XtPointer)True
	}};

	if(iconret != NULL)
		*iconret = None;
	if(maskret != NULL)
		*maskret = None;

	if(icons.mask == NULL)
		return FALSE;

	XtGetApplicationResources(wl_top(top), &v, &r[0],
					sizeof(r)/sizeof(r[0]), NULL, 0);

	if(!v.install_icon)
		return TRUE;

	fg = BlackPixel(dpy, DefaultScreen(dpy));
	bg = WhitePixel(dpy, DefaultScreen(dpy));
	mask = wlp_xbm_from_data(NULL, dpy, fg, bg, icons.mask, icons.mwidth,
								icons.mheight);

	if(!v.use_color
	|| icons.color == NULL
	|| (icon = wlp_xpm_from_data(NULL, dpy, bg, icons.color)) == None)
		icon = wlp_xbm_from_data(NULL, dpy, fg, bg, icons.bits,
						icons.width, icons.height);
	if(icon == None) {
		wl_free_pixmap(dpy, mask);
		if(iconret != NULL)
			*iconret = None;
		if(maskret != NULL)
			*maskret = None;
		return FALSE;
	}

	n = 0;
	XtSetArg(a[n], XmNiconPixmap, icon);		++n;
	if(mask != None) {
		XtSetArg(a[n], XmNiconMask, mask);	++n;
	}
	wlSetValues(top, a, n);

	if(iconret != NULL)
		*iconret = icon;
	if(maskret != NULL)
		*maskret = mask;

	return TRUE;
}

int
wl_icons_init(WL_ICONS *i)
{
	icons = *i;
	return TRUE;
}
