/*
 * findup.c
 *	Find a widget in your ancestors.
 *	This file is part of wlib.
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
MU_ID("$Mu: wlib/findup.c 1.8 1998/04/06 03:50:01 $")

#include <string.h>

#include <wlib/wlib.h>

Widget
wl_findup(Widget w, char *name)
{
	if(name == NULL)
		return NULL;
	for(; w != NULL; w = XtParent(w))
		if(strcmp(XtName(w), name) == 0)
			break;
	return w;
}
