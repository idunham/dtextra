/*
 * view.h
 *	View menu callbacks for mgv.
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
#ifndef	MGV_VIEW_H
#define	MGV_VIEW_H
MU_HID(view_h, "$Mu: mgv/mine/view.h 1.2 1998/08/05 16:05:42 $")

CDECLS_BEGIN
extern	void	mgv_view_log(Widget, XtPointer, XtPointer);
extern	void	mgv_view_menu(Widget, XtPointer, XtPointer);
extern	void	mgv_view_page(Widget, XtPointer, XtPointer);
extern	void	mgv_view_status(Widget, XtPointer, XtPointer);
extern	void	mgv_view_toolbar(Widget, XtPointer, XtPointer);
extern	void	mgv_view_zoom(Widget, XtPointer, XtPointer);
CDECLS_END

#endif
