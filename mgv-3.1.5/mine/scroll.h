/*
 * scroll.h
 *	Page scrolling and panning actions.
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
#ifndef	MGV_SCROLL_H
#define	MGV_SCROLL_H
MU_HID(scroll_h, "$Mu: mgv/mine/scroll.h 1.1 1998/06/13 21:15:40 $")

CDECLS_BEGIN
extern	void	mgv_action_pan(Widget, XEvent *, String *, Cardinal *);
extern	void	mgv_action_scroll(Widget, XEvent *, String *, Cardinal *);
CDECLS_END

#endif
