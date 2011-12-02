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
 * FILE NAME:	XbrLabel.c
 * CREATED:	Wed Oct 27 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Routines for handling Motif XmLabel widget.
 *
 */

/*
 * Fri Aug 19 1994
 * Function XbrLabelSetPixmap() to load and set the pixmap inside and XmLabel
 * or similar widget.
 *
 */

#include <Xm/Xm.h>
#include <Xm/Label.h>

#include "XbrGfx.h"

#include <stdio.h>
#include <stdarg.h>

char *XbrLabelGet(Widget);
void XbrLabelSet(Widget, ...);
Pixmap XbrLabelSetPixmap(Widget, char *, int, int, char *, int, int);

/* XbrLabelGet:***************************************************************/
/* XbrLabelGet: Simple routine to get the labelString resource so that we    */
/* XbrLabelGet: don't need to play about with XmStrings.                     */
/* XbrLabelGet:***************************************************************/

char *XbrLabelGet(Widget w)
{
    XmString MyString;
    char *label = NULL;

    XtVaGetValues(w, XmNlabelString, &MyString, NULL);
    XmStringGetLtoR(MyString, XmSTRING_DEFAULT_CHARSET, &label);
    return(label);
}

/* XbrLabelSet:***************************************************************/
/* XbrLabelSet: Simple routine to set the labelString resource so that we    */
/* XbrLabelSet: don't need.                                                  */
/* XbrLabelSet:***************************************************************/

void XbrLabelSet(Widget w, ...)
{
    char *fmt, buffer[BUFSIZ];
    va_list argp;
    XmString MyString;

    /* Get format string */
    va_start(argp, w);
    fmt = va_arg( argp, char *);
    vsprintf(buffer, fmt, argp);
    va_end(argp);

    /* Set value - as XmString */
    MyString = XmStringCreateLtoR(buffer, XmSTRING_DEFAULT_CHARSET);
    XtVaSetValues(w, XmNlabelString, MyString, NULL);
    XmStringFree(MyString);
}

/*----------------------------------------------------------------------------
  XbrLabelSetPixmap()
  Load the given #included bitmap and set the XmNlabelPixmap resource. If
  the second set of bitmap data is defined then also set the 
  XmNlabelInsensitivePixmap.
  ----------------------------------------------------------------------------*/
Pixmap XbrLabelSetPixmap(Widget w, char *bits,  int width,  int height,
			           char *sbits, int swidth, int sheight)
{
    Pixmap pixmap, spixmap;

    pixmap = XbrGfxLoadBitmap(w, bits, width, height, 1);

    XtVaSetValues(w, XmNlabelType, XmPIXMAP, XmNlabelPixmap, pixmap, NULL);

    if(sbits) {
       spixmap = XbrGfxLoadBitmap(w, sbits, swidth, sheight, 1);
       XtVaSetValues(w, XmNlabelInsensitivePixmap, spixmap, NULL);
    }
    
    return(pixmap);
}
