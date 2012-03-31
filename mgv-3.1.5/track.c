/*
 * track.c
 *	Pointer tracking callbacks.
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
MU_ID("$Mu: mgv/track.c 1.38 1998/08/30 03:11:04 $")

#include <assert.h>
#include <Xm/Label.h>

#include <mine/mgv.h>
#include <mine/track.h>
#include <mine/strings.h>

/*
 * update position label
 */
void
mgv_track(Widget w, XtPointer closure, XtPointer call)
{
	GhostviewReturnStruct	*cbs = (GhostviewReturnStruct *)call;
	MGV			*m   = (MGV *)closure;
	char			buf[1024];
	USEUP(w);
	assert(MgvOK(m));

	/*
	 * Locator events (the only ones that we are concerned
	 * with here) have zero width and height.
	 */
	if(cbs->width != 0 || cbs->height != 0)
		return;
	assert(cbs->event->type == EnterNotify 
	    || cbs->event->type == LeaveNotify
	    || cbs->event->type == MotionNotify);
	if(cbs->event->type == LeaveNotify)
		sprintf(buf, "%s", mgv_s(w, MgvSnoCoordinates));
	else
		sprintf(buf, "(%d, %d)", cbs->psx, cbs->psy);
	XtVaSetValues(m->tracker,
		XtVaTypedArg,	XmNlabelString,
				XmRString, buf,
				strlen(buf) + 1,
		NULL);
}
