/*
 * strings.h
 *	message string stuff
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
#ifndef	MGV_STRINGS_H
#define	MGV_STRINGS_H
MU_HID(strings_h, "$Mu: mgv/mine/strings.h 1.18 1998/08/30 03:11:04 $")

/*
 * the MgvS<x> entry in this list corresponds to the MgvNstring<x> resource
 */
enum {
	MgvSbadArgument,
	MgvScantAllocPixmap,
	MgvScantDecompress,
	MgvScantHideMenu,
	MgvScantOpen,
	MgvScantPrint,
	MgvSerror,
	MgvSfileExists,
	MgvSinterpDone,
	MgvSinterpFailed,
	MgvSmessage,
	MgvSnoCoordinates,
	MgvSnoDate,
	MgvSnoFile,
	MgvSnoPrintCommand,
	MgvSpageFinished,
	MgvSunknownSwitch,
	MgvSPS,
	MgvSEPS,
	MgvSPDF,
	MgvSunknownType,
	MgvSunknownMedia,
	MgvSunknown
};
CDECLS_BEGIN
extern	String	mgv_s(Widget, int);
CDECLS_END

#endif
