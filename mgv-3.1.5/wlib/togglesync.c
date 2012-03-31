/*
 * togglesync.c
 *	Callbacks for synchronizing toggle buttons (useful if two menus
 *	have the same toggle buttons in them).
 *
 * Copyright (C) 1997 Eric A. Howe
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
MU_ID("$Mu: wlib/togglesync.c 1.5 1998/04/06 03:50:02 $")

#include <assert.h>
#include <Xm/ToggleB.h>

#include <wlib/typesanity.h>
#include <wlib/wlib.h>

/*
 * The "closure" should be a widget path from the top level widget
 * to the _other_ toggle button.
 */
void
wl_toggle_sync(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS	*cbs  = (XmTBS *)call;
	char	*name = (char *)closure;

	assert(name != NULL);
	w = wl_find1(wl_top(w), name);
	assert(w != NULL);
	XmToggleButtonSetState(w, cbs->set, False);
}
