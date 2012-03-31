/*
 * file.h
 *	Declarations for the File menu callbacks.
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
#ifndef	MGV_FILE_H
#define	MGV_FILE_H
MU_HID(file_h, "$Mu: mgv/mine/file.h 1.26 1998/03/29 05:42:36 $")

CDECLS_BEGIN
extern	void	mgv_file_close(Widget, XtPointer, XtPointer);
extern	void	mgv_file_info(Widget, XtPointer, XtPointer);
extern	void	mgv_file_open(Widget, XtPointer, XtPointer);
extern	void	mgv_file_reopen(Widget, XtPointer, XtPointer);
extern	void	mgv_file_save(Widget, XtPointer, XtPointer);
CDECLS_END

#endif
