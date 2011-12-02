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
 * FILE NAME:	Xbr.h
 * CREATED:	Mon Nov  1 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Main header file for the Xbr X windows library.
 *
 */

#ifndef	XBRLIB_XBR_H
#define	XBRLIB_XBR_H

#if (XmVERSION < 1) || ((XmVERSION == 1) && (XmREVISION < 2))
#define XmFONTLIST_DEFAULT_TAG  XmSTRING_DEFAULT_CHARSET
#define XmStringCreateLocalized(string) XmStringCreateLtoR(string,XmSTRING_DEFAULT_CHARSET)
#endif

#include "XbrCB.h"
#include "XbrCreate.h"
#include "XbrCursor.h"
#include "XbrDialog.h"
#include "XbrFSB.h"
#include "XbrGfx.h"
#include "XbrLabel.h"
#include "XbrList.h"
#include "XbrMenu.h"
#include "XbrRes.h"

#endif
