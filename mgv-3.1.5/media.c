/*
 * media.c
 *	Media menu callbacks for mgv.
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
MU_ID("$Mu: mgv/media.c 1.39 1998/04/06 03:50:25 $")

#include <assert.h>
#include <Xm/ToggleB.h>

#include <wlib/typesanity.h>
#include <mine/mgv.h>
#include <mine/media.h>

/*
 * Media menu.
 * Note that the media menu widgets have the same names as the
 * corresponding media so we can get away with only one (simple)
 * callback.  This sort of thing is to be encouraged BTW.
 */
void
mgv_media_cb(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS	*cbs = (XmTBS *)call;
	MGV	*m   = (MGV *)closure;
	char	*s;
	int	i;
	assert(MgvOK(m));

	if(!cbs->set)
		return;
	for(i = 0, s = XtName(w); dsc_media[i].name != NULL; ++i) {
		if(strcmp(s, dsc_media[i].name) != 0)
			continue;
		mgv_media(m, i);
		m->mediatog = w;
		mgv_show(m, m->page);
		return;
	}

	/*
	 * if we've made this far, then there is a problem
	 */
	assert("mgv_media_cb(): unknown media menu toggle" != NULL);
	return;
}
