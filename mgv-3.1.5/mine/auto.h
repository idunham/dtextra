/*
 * auto.h
 *	Automatic window width and height setting functions.
 *
 * Copyright (C) 1997 Eric A. Howe
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
#ifndef	MGV_AUTO_H
#define	MGV_AUTO_H
MU_HID(auto_h, "$Mu: mgv/mine/auto.h 1.4 1998/03/28 20:55:26 $")

CDECLS_BEGIN
extern	void	mgv_auto_height(MGV *);
extern	void	mgv_auto_none(MGV *);
extern	void	mgv_auto_size(MGV *);
extern	void	mgv_auto_width(MGV *);
CDECLS_END

#endif
