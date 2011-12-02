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
 * FILE NAME:	XbrDialog.h
 * CREATED:	Thu Oct 28 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Header file for XbrDialog.c
 *
 */

#ifndef XBRLIB_XBRDIALOG_H
#define XBRLIB_XBRDIALOG_H		1

#define	XbrDialogFSB(w)	XbrDialogFSBox(w, NULL, NULL)

extern void XbrDialogError(Widget, ...);
extern int XbrDialogQuestion(Widget, char *, char *, ...);
extern char *XbrDialogFSBox(Widget, char *, char *);
extern char *XbrDialogPrompt(Widget, ...);

#endif
