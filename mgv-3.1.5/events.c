/*
 * events.c
 *	Event handlers.
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
MU_ID("$Mu: mgv/events.c 1.12 1998/04/06 03:50:24 $")

#include <assert.h>
#include <Xm/RowColumn.h>

#include <wlib/wlib.h>
#include <mine/mgv.h>
#include <mine/events.h>

void
mgv_events(Widget w, XtPointer closure, XEvent *ev, Boolean *cont)
{
	MGV	*m = (MGV *)closure;
	USEUP(w); USEUP(cont);
	assert(MgvOK(m));

	if(ev->type != ButtonPress
	|| ev->xbutton.button != 3)
		return;
	XmMenuPosition(m->popup, &ev->xbutton);
	XtManageChild(m->popup);
	XtPopup(XtParent(m->popup), XtGrabNone);
}
