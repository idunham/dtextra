/*
 * mark.h
 *	Mark menu callbacks for mgv.
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
#ifndef	MGV_MARK_H
#define	MGV_MARK_H
MU_HID(mark_h, "$Mu: mgv/mine/mark.h 1.7 1998/08/20 21:57:58 $")

CDECLS_BEGIN
extern	void	mgv_action_mark(Widget, XEvent *, String *, Cardinal *);
extern	void	mgv_mark_even(Widget, XtPointer, XtPointer);
extern	void	mgv_mark_invert(Widget, XtPointer, XtPointer);
extern	void	mgv_mark_mark(Widget, XtPointer, XtPointer);
extern	void	mgv_mark_markall(Widget, XtPointer, XtPointer);
extern	void	mgv_mark_odd(Widget, XtPointer, XtPointer);
extern	void	mgv_mark_unmarkall(Widget, XtPointer, XtPointer);
CDECLS_END

#endif
