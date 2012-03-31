/*
 * page_control.h
 *	Page stack control.
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
#ifndef	MGV_PAGE_CONTROL_H
#define	MGV_PAGE_CONTROL_H
MU_HID(page_control_h, "$Mu: mgv/mine/page_control.h 1.4 1998/09/01 21:10:44 $")

CDECLS_BEGIN
extern	MGVP	mgv_pgstack_alloc(MGV *);
extern	MGVP	mgv_pgstack_free(MGVP);
extern	void	mgv_pgstack_goto(MGVP, int);
extern	void	mgv_pgstack_back(MGVP);
extern	void	mgv_pgstack_forward(MGVP);
extern	void	mgv_pgstack_prefill(MGVP, int);
CDECLS_END

#endif
