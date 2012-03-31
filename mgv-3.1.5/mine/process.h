/*
 * process.h
 *	Tools and such for dealing with child processes.
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
#ifndef	MGV_PROCESS_H
#define	MGV_PROCESS_H
MU_HID(process_h, "$Mu: mgv/mine/process.h 1.2 1998/08/28 21:25:13 $")

#define	PK_NONE	0
#define	PK_IN	(1 << 0)
#define	PK_ERR	(1 << 1)
#define	PK_OUT	(1 << 2)
#define	PK_ALL	(PK_IN | PK_OUT | PK_ERR)

typedef struct {
	int	in, out, err;
	pid_t	pid;
	int	flags;
} PIPEKID;

CDECLS_BEGIN
extern	int	mgv_popen(PIPEKID *, char *, char **);
extern	int	mgv_child_msg(Widget, int);
CDECLS_END

#endif
