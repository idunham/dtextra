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
 * FILE NAME:	XbrMenu.h
 * CREATED:	Mon Nov  1 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Header file for XbrMenu.c
 *
 */

#ifndef XBRLIB_XBRMENU_H
#define XBRLIB_XBRMENU_H

#define	XbrMENU_POPUP		1
#define	XbrMENU_PULLDOWN	2
#define XbrMENU_PULLDOWN_RADIO	3
#define XbrMENU_OPTION		4

#define	XbrLABEL		1
#define	XbrPB			2
#define	XbrSEPARATOR		3
#define XbrSUBMENU		4
#define XbrTOGGLE		5

#define	XbrSET			1
#define	XbrUNSET		0
#define	XbrNOSET		-1

typedef struct menu_data {
    int type;			/* Type of widget 1 from 5 above */
    char *label;		/* Label for widget              */
    XtCallbackProc callback;	/* Callback to be added          */
    XtPointer data;		/* Data for callback             */
    int set;			/* Set this (if toggle button)   */
    struct menu_data *submenu;	/* Possible submenu              */
    char *title;		/* Title for submenu             */
    int stype;			/* Type of submenu               */
    int n;			/* Number of items in submenu    */
} XbrMenuData;

extern Widget *XbrMenu(Widget, char *, int, XbrMenuData[], int);

#endif
