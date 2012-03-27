/*
 * $Id: CutPaste.c,v 1.2.1.5 1994/05/13 14:09:54 mallet Exp $
 * 
 * Copyright (c) 1991-1994  Lionel MALLET
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL Lionel MALLET	BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of Lionel MALLET shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization
 * from Lionel MALLET.
 *
 * Author:  Lionel Mallet, SIMULOG
 */

/*
 * $XConsortium: CutAndPaste.c,v 1.1 90/06/09 20:20:17 dmatic Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Davor Matic, MIT X Consortium
 */

#include <X11/IntrinsicP.h>
#include <X11/Xmu/Converters.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include "PixmapP.h"
    
#include <stdio.h>
#include <math.h>

#define XtStrlen(s)                   ((s) ? strlen(s) : 0)
#define abs(x)                        (((x) > 0) ? (x) : -(x))
#define min(x, y)                     (((x) < (y)) ? (x) : (y))
#define max(x, y)                     (((x) > (y)) ? (x) : (y))


/*****************************************************************************
 *                               Cut and Paste                               *
 *****************************************************************************/


Boolean ConvertSelection(w, selection, target, type, value, length, format)
    Widget w;
    Atom *selection, *target, *type;
    XtPointer *value;
    unsigned long *length;
    int *format;
{
    PixmapWidget PW = (PixmapWidget) w;
    Pixmap *pixmap;
    XImage *image;
    Dimension width, height;
 
    switch (*target) {
/*
    case XA_TARGETS:
	*type = XA_ATOM;
	*value = (XtPointer) pixmapClassRec.pixmap_class.targets;
	*length = pixmapClassRec.pixmap_class.num_targets;
	*format = 32;
	return True;
*/
    case XA_BITMAP:
    case XA_PIXMAP:
	if (!PWQueryMarked(w)) return False;
	width = PW->pixmap.mark.to_x - PW->pixmap.mark.from_x + 1;
	height = PW->pixmap.mark.to_y - PW->pixmap.mark.from_y + 1;
	image = CreatePixmapImage(PW, width, height);
	CopyImageData(PW->pixmap.image, image, 
		      PW->pixmap.mark.from_x, PW->pixmap.mark.from_y,
		      PW->pixmap.mark.to_x, PW->pixmap.mark.to_y, 0, 0);
	pixmap = (Pixmap *) XtMalloc(sizeof(Pixmap));
	*pixmap = GetPixmap(PW, image);
	DestroyPixmapImage(&image);
	*type = XA_PIXMAP;
	*value = (XtPointer) pixmap;
	*length = 1;
	*format = 32;
	return True;

    case XA_STRING:
	*type = XA_STRING;
	*value = "Hello world!\n";
	*length = XtStrlen(*value);
	*format = 8;
	return True;

    default:
	return False;
    }
}

void LoseSelection(w, selection)
    Widget w;
    Atom *selection;
{
    PixmapWidget PW = (PixmapWidget) w;

    if (_PWDEBUG)
	fprintf(stderr, "Lost Selection\n");
    PW->pixmap.selection.own = False;
    PWUnmark(w);
}

void SelectionDone(w, selection, target)
    Widget w;
    Atom *selection, *target;
{
/*  
    PixmapWidget PW = (PixmapWidget) w;

    if (*target != XA_TARGETS)
	XtFree(PW->pixmap.value);
*/
}

#if NeedFunctionPrototypes
void PWGrabSelection(Widget w, Time time)
#else
void PWGrabSelection(w, time)
    Widget w;
    Time time;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    PW->pixmap.selection.own = XtOwnSelection(w, XA_PRIMARY, time,
					      ConvertSelection, 
					      LoseSelection, 
					      SelectionDone);
	if (_PWDEBUG && PW->pixmap.selection.own)
	    fprintf(stderr, "Own the selection\n");
}

void SelectionCallback(w, client_data, selection, type, value, length, format)
    Widget w;
    XtPointer client_data;
    Atom *selection, *type;
    XtPointer value;
    unsigned long *length;
    int *format;
{
    PixmapWidget PW = (PixmapWidget) w;
    Pixmap *pixmap;

   switch (*type) {
	
    case XA_BITMAP:
    case XA_PIXMAP:
	DestroyPixmapImage(&PW->pixmap.storage);
	DestroyMaskImage(&PW->pixmap.mask_storage);
	pixmap = (Pixmap *) value;
	PW->pixmap.storage = GetImage(PW, *pixmap);
	PW->pixmap.mask_storage =
	  CreateMaskImage(PW,
			  (Dimension) PW->pixmap.mask_image->width,
			  (Dimension) PW->pixmap.mask_image->height);
	XFreePixmap(XtDisplay(w), *pixmap);
	break;
	
    case XA_STRING:
	if (_PWDEBUG)
	    fprintf(stderr, "Received:%s\n", (char *)value);
	break;

    default:
	XtAppWarning(XtWidgetToApplicationContext(w),
		     " selection request failed.  PixmapWidget");
	break;
    }

    PW->pixmap.selection.limbo = FALSE;
}

#if NeedFunctionPrototypes
void PWRequestSelection(Widget w, Time time, Boolean wait)
#else
void PWRequestSelection(w, time, wait)
    Widget w;
    Time time;
    Boolean wait;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    XtGetSelectionValue(w, XA_PRIMARY, XA_PIXMAP,
			SelectionCallback, NULL, time);

    PW->pixmap.selection.limbo = TRUE;

    if (wait)
	while (PW->pixmap.selection.limbo) {
	    XEvent event;
	    XtAppNextEvent(XtWidgetToApplicationContext(w), &event);
	    XtDispatchEvent(&event);
	}
}

/*****************************************************************************/
