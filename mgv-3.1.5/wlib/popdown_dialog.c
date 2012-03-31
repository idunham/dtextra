/*
 * popdown_dialog.c
 *	The wl_dlg_popdown() function.
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
 *   Authors: Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: wlib/popdown_dialog.c 1.2 1998/04/06 03:50:01 $")

#include <Xm/Xm.h>

#include <wlib/wlib.h>

typedef struct {
	Boolean	auto_dismiss;
} ADR;

void
wl_dlg_popdown(Widget dlg)
{
	XtResource r = {
		XwlNautoDismiss, XwlCAutoDismiss,
		XtRBoolean, sizeof(Boolean), XtOffsetOf(ADR, auto_dismiss),
		XtRImmediate, (XtPointer)True
	};
	ADR	res;

	XtGetApplicationResources(dlg, &res, &r, 1, NULL, 0);
	if(res.auto_dismiss)
		XtUnmanageChild(dlg);
}
