/*
 * popup_dialog.c
 *	The wl_dlg_popup() function.
 *
 * Copyright (C) 1997 Eric A. Howe
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
MU_ID("$Mu: wlib/popup_dialog.c 1.4 1998/04/06 03:50:02 $")

#include <wlib/wlib.h>

/*
 * A simple function for doing all the standard things to bring
 * a dialog up.
 */
void
wl_dlg_popup(Widget dlg)
{
	XtManageChild(dlg);
	XtPopup(XtParent(dlg), XtGrabNone);
	wl_focus(dlg);
}
