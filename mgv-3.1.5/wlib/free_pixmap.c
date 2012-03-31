/*
 * free_pixmap.c
 *	The wl_free_pixmap() function.
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
MU_ID("$Mu: wlib/free_pixmap.c 1.3 1998/04/06 03:50:01 $")

#include <X11/Xlib.h>
#include <wlib/sanity.h>

Pixmap
wl_free_pixmap(Display *dpy, Pixmap p)
{
	if(dpy != NULL && p != None)
		XFreePixmap(dpy, p);
	return None;
}
