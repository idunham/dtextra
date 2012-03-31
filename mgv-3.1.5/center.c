/*
 * center.c
 *	Page centering.
 *
 * Copyright (C) 1998 Eric A. Howe
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
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/center.c 1.1 1998/07/12 05:58:21 $")

#include <assert.h>
#include <Xm/ToggleB.h>

#include <wlib/wlib.h>
#include <mine/mgv.h>
#include <mine/center.h>

void
mgv_center_center(MGV *m)
{
	WL_SBV	v;
	Widget	w;
	assert(MgvOK(m));

	memset((void *)&v, '\0', sizeof(v));

	w = wl_getsbvalues(m->main, "*view*VertScrollBar", &v);
	v.value = (v.min + v.max - v.slider)/2;
	wl_setsbvalues(w, &v, True);

	w = wl_getsbvalues(m->main, "*view*HorScrollBar", &v);
	v.value = (v.min + v.max - v.slider)/2;
	wl_setsbvalues(w, &v, True);
}

void
mgv_center_noop(MGV *m)
{
	USEUP(m);
}
