/*
 * magstep.h
 *	Magstep menu callbacks for mgv.
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
#ifndef	MGV_MAGSTEP_H
#define	MGV_MAGSTEP_H
MU_HID(magstep_h, "$Mu: mgv/mine/magstep.h 1.23 1998/08/16 08:12:34 $")

CDECLS_BEGIN
extern	int	mgv_ui_magnify(MGV *, int);

extern	void	mgv_magstep_decrease(Widget, XtPointer, XtPointer);
extern	void	mgv_magstep_edit(Widget, XtPointer, XtPointer);
extern	void	mgv_magstep_increase(Widget, XtPointer, XtPointer);
extern	void	mgv_magstep_n(Widget, XtPointer, XtPointer);
extern	void	mgv_magstep_set(Widget, XtPointer, XtPointer);
extern	void	mgv_magstep_text(Widget, XtPointer, XtPointer);
CDECLS_END

#endif
