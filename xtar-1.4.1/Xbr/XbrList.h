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
 * FILE NAME:	XbrList.h
 * CREATED:	Wed Oct 27  1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Header file for XbrList.c
 *
 */

#ifndef XBRLIB_XBRLIST_H
#define XBRLIB_XBRLIST_H	1

#define	XbrLIST_SHOWTOP		1
#define	XbrLIST_SHOWBOTTOM	0
#define	XbrLIST_SHOWNONE	-1

extern void XbrListClear(Widget);
extern void XbrListAddItem(Widget, char *, int);
extern char **XbrListGetSelection(Widget);
extern void XbrListSelectionFree(char **);
extern XbrListItemSelected(Widget, int *);
extern void XbrListTop(Widget);
extern int XbrListNumItems(Widget);

#endif
