/* 
 * Copyright 1993, 1994 Liverpool University Computer Science Department
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of L.U.C.S. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. L.U.C.S. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * FILE NAME:	XbrCursor.c
 * CREATED:	Thu Oct 28 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION: Cursor handling routines. This is not very good at all and
 *              I'm sure there is a better way of doing this, but as the FAQ
 *              does not suggest many easier ways, here goes...
 *
 */

#ifndef	XBRLIB_XBRCURSOR_H
#define	XBRLIB_XBRCURSOR_H		1

extern void XbrCursorAdd(Widget, int);
extern void XbrCursorDelete(Widget);
extern void XbrCursorAddCB(Widget, XtPointer, XtPointer);
extern void XbrCursorDeleteCB(Widget, XtPointer, XtPointer);
extern void XbrCursorWatch(Widget, int);

#define	XbrCursorBusy(w)		XbrCursorWatch(w, 1)
#define	XbrCursorReset(w)		XbrCursorWatch(w, 0)

#define	XbrGRAB		1
#define	XbrNOGRAB	0

#endif
