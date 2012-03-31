/*
 * center.h
 *	Automatic page centering things.
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
#ifndef	MGV_CENTER_H
#define	MGV_CENTER_H
MU_HID(center_h, "$Mu: mgv/mine/center.h 1.1 1998/07/12 05:58:29 $")

CDECLS_BEGIN
extern	void	mgv_center_center(MGV *);
extern	void	mgv_center_noop(MGV *);
CDECLS_END

#endif
