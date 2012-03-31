/*
 * sens.h
 *	Widget sensitivity handling for mgv.
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
#ifndef	MGV_SENS_H
#define	MGV_SENS_H
MU_HID(sens_h, "$Mu: mgv/mine/sens.h 1.25 1998/08/31 22:21:26 $")

/*
 * the sensitivity mask
 */
#define	MgvSNONE	0		/* nothing			*/
#define	MgvSLOG		(1 << 0x1)	/* something is in the log	*/
#define	MgvSMARKED	(1 << 0x2)	/* at least one page is marked	*/
#define	MgvSNOTFIRST	(1 << 0x3)	/* not on the first page	*/
#define	MgvSNOTLAST	(1 << 0x4)	/* not on the last page		*/
#define	MgvSOPEN	(1 << 0x5)	/* we have a file open		*/
#define	MgvSPAGES	(1 << 0x6)	/* we have page information	*/
#define	MgvSDOC		(1 << 0x7)	/* we have DSC comments		*/
#define	MgvSGOTZOOMS	(1 << 0x8)	/* zoom windows live		*/
#define	MgvSCANBACK	(1 << 0x9)	/* can Alt+Left			*/
#define	MgvSCANFORWARD	(1 << 0xa)	/* can Alt+Right		*/
#define	MgvSCANREOPEN	(1 << 0xb)	/* can file->reopen (kludgy)	*/
#define	MgvSOPENDOC	(MgvSOPEN | MgvSDOC)
#define	MgvSALL		(				\
				  MgvSLOG		\
				| MgvSMARKED		\
				| MgvSNOTFIRST		\
				| MgvSNOTLAST		\
				| MgvSOPEN		\
				| MgvSPAGES		\
				| MgvSDOC		\
				| MgvSGOTZOOMS		\
				| MgvSCANBACK		\
				| MgvSCANFORWARD	\
				| MgvSCANREOPEN		\
			)

typedef struct {
	char *name;
	int   primary, secondary;
} MGVSD;

CDECLS_BEGIN
extern	MGVS	mgv_sens_create(Widget, MGVSD *, int);
extern	MGVS	mgv_sens_free(MGVS);
extern	void	mgv_sens(MGVS, int);
CDECLS_END

#endif
