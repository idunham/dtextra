/*
 * callback.c
 *	Callback binding functions for wlib.
 *
 * Copyright (C) 1996  Matthew D. Francey
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
MU_ID("$Mu: wlib/callback.c 1.24 1998/08/27 04:23:56 $")

#include <wlib/wlib.h>

void
wl_callback(Widget parent, WLC *cb, XtPointer defclosure)
{
	XtPointer closure;
	Widget    w;
	WLC       *last;

	for(w = NULL, last = NULL; cb->name != NULL; last = cb++) {
		/*
		 * the callbacks for each widget are generally
		 * grouped together so this minor optimization
		 * might speed things up a little (will anyone notice?
		 * probably not but "One small optimization for man, ...")
		 */
		if((last == NULL || strcmp(last->name, cb->name) != 0)
		&& (w = wl_find1(parent, cb->name)) == NULL)
			continue;
		closure = cb->closure == NULL ? defclosure : cb->closure;
		XtAddCallback(w, cb->call, cb->func, closure);
	}
}
