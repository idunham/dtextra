/*
 * util.h
 *	Miscellaneous utility functions for mgv.
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
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#ifndef	MGV_UTIL_H
#define	MGV_UTIL_H
MU_HID(util_h, "$Mu: mgv/mine/util.h 1.11 1998/08/28 10:06:35 $")

CDECLS_BEGIN
extern	char	*stolower(char *);
extern	char	*lowdup(char *);
extern	char	**stoargv(char *);
CDECLS_END

#endif
