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
 * FILE NAME:	XbrCB.h
 * CREATED:	Wed Oct 27 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Header file for XbrCB.c
 *
 */

#ifndef XBRLIB_XBRCB_H
#define XBRLIB_XBRCB_H		1

extern void XbrCBExitApplication(Widget, XtPointer, XtPointer);
extern void XbrCBDestroyWidget(Widget, XtPointer, XtPointer);
extern void XbrCBPassword(Widget, XtPointer, XtPointer);
extern void XbrCBPopdownShell(Widget, XtPointer, XtPointer);
extern void XbrCBRefresh(Widget, XtPointer, XtPointer);
extern void XbrCBUnmanageWidget(Widget, XtPointer, XtPointer);

#endif
