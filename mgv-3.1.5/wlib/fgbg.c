/*
 * fgbg.c
 *	Function for getting the fg/bg colors from a widget.
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
MU_ID("$Mu: wlib/fgbg.c 1.1 1998/08/23 06:28:00 $")

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <wlib/wlibP.h>
#include <wlib/sanity.h>

void
wlp_fgbg(Widget w, Pixel *fg, Pixel *bg)
{
	Arg      a[2];
	Cardinal n;

	n = 0;
	if(fg != NULL) {
		XtSetArg(a[n], XtNforeground, fg);
		++n;
	}
	if(bg != NULL) {
		XtSetArg(a[n], XtNbackground, bg);
		++n;
	}

	wlGetValues(w, a, n);
}
