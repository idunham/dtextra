/*
 * options.h
 *	Declarations for the Options menu callbacks.
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
#ifndef	MGV_OPTIONS_H
#define	MGV_OPTIONS_H
MU_HID(options_h, "$Mu: mgv/mine/options.h 1.12 1998/03/28 20:55:26 $")

CDECLS_BEGIN
extern	void	mgv_options_antialias(Widget, XtPointer, XtPointer);
extern	void	mgv_options_bits(Widget, XtPointer, XtPointer);
extern	void	mgv_options_palette(Widget, XtPointer, XtPointer);
CDECLS_END

#endif
