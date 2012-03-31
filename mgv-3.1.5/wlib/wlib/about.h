/*
 * about.h
 *	Standard wlib about box.
 *
 * Copyright (C) 1996 Matthew D. Francey
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
#ifndef	WLIB_ABOUT_H
#define	WLIB_ABOUT_H
MU_HID(wlib_about_h, "$Mu: wlib/wlib/about.h 1.3 1998/08/25 06:39:58 $")

CDECLS_BEGIN
extern	Widget	wl_about(Widget parent, char *header, char *footer,
			char **cpright,
			Pixmap (*p)(Widget w),
			char *(*version)(void));
CDECLS_END

#endif
