/*
 * sens.c
 *	Widget sensitivity handling for mgv.
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
MU_ID("$Mu: mgv/sens.c 1.55 1998/09/02 02:59:05 $")

#include <X11/Intrinsic.h>

#include <wlib/wlib.h>
#include <mine/mgv.h>
#include <mine/sens.h>

typedef struct {
	char  *name;
	Widget w;
	int    primary, secondary;
} MGVSE;

struct MGVS_s {
	Widget root;
	MGVSE *sens;
	int    n_sens;
};

void
mgv_sens(MGVS s, int mask)
{
	int	i;
	Boolean	b;

	if(s == NULL)
		return;

	for(i = 0; i < s->n_sens; ++i) {
		if(s->sens[i].w == NULL
		&& (s->sens[i].w = wl_find1(s->root, s->sens[i].name)) == NULL)
			continue;
		b = False;
		if((s->sens[i].primary & mask) == s->sens[i].primary)
			b = True;
		else if(s->sens[i].secondary != 0
		    && (s->sens[i].secondary & mask) == s->sens[i].secondary)
			b = True;
		if(XtIsSensitive(s->sens[i].w) != b)
			XtSetSensitive(s->sens[i].w, b);
	}
}

MGVS
mgv_sens_free(MGVS s)
{
	int i;

	if(s == NULL)
		return s;
	for(i = 0; i < s->n_sens; ++i)
		XtFree(s->sens[i].name);
	XtFree((XtPointer)s->sens);
	XtFree((XtPointer)s);
	return NULL;
}

MGVS
mgv_sens_create(Widget root, MGVSD *sd, int n_sd)
{
	MGVS s;
	int  i;

	if((s = (MGVS)XtCalloc(sizeof(struct MGVS_s), 1)) == NULL
	|| (s->sens = (MGVSE *)XtCalloc(sizeof(MGVSE), n_sd)) == NULL)
		return mgv_sens_free(s);

	s->root   = wl_top(root);
	s->n_sens = n_sd;

	for(i = 0; i < n_sd; ++i) {
		s->sens[i].name      = XtNewString(sd[i].name);
		s->sens[i].w         = NULL;
		s->sens[i].primary   = sd[i].primary;
		s->sens[i].secondary = sd[i].secondary;
	}

	return s;
}
