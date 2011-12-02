/**
 *
 * $Id: SetClientIcon.c,v 1.10 2004/09/01 20:28:58 rwscott Exp $
 *
 * Copyright (C) 1996 Free Software Foundation, Inc.
 * Copyright © 1999-2001 by the LessTif developers.
 *
 * This file is part of the GNU LessTif Extension Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **/

#include "../config.h"

#include <stddef.h>

#include <X11/Xmd.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <Xlt.h>

#ifdef HAVE_XPM
#ifdef HAVE_XM_XPMP_H
#include <Xm/XpmP.h>
#else
#include <X11/xpm.h>
#endif
#else
#error You need Xpm for this
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif


static const char rcsid[] = "$Id: SetClientIcon.c,v 1.10 2004/09/01 20:28:58 rwscott Exp $";

/* ***************************************************************************
 * build and set a net icon from a pixmap
 * ***************************************************************************/
static void
SetProperty(Widget W, XtPointer call_data, XEvent *event, Boolean *cont)
{
Pixmap pixmap = None;
Pixmap mask = None;
XImage *image = NULL;
XImage *m_image = NULL;
CARD32 *new_icon = NULL;
unsigned int i,j,k,l,m;
Colormap Pcmap;
unsigned int width, height;

    if (event == NULL || event->type == ConfigureNotify)
    {
	if (event != NULL)
	{
	    XtRemoveEventHandler(W, StructureNotifyMask, False, (XtEventHandler)SetProperty, NULL);
	}
	XtVaGetValues(W,
		XtNiconMask, &mask,
		XtNiconPixmap, &pixmap,
		NULL);
	if (pixmap != None)
	{
	Window root_return;
	int x_return, y_return;
	unsigned int border_width_return;
	unsigned int depth_return;

	    XGetGeometry(XtDisplay(W), pixmap,
		    &root_return,
		    &x_return, &y_return,
		    &width, &height,
		    &border_width_return,
		    &depth_return);
	    image = XGetImage(XtDisplay(W), pixmap, 0, 0, width, height, AllPlanes, ZPixmap);
	    if (image != NULL)
	    {
		if (mask != None)
		{
		    m_image = XGetImage(XtDisplay(W), mask, 0, 0, width, height, AllPlanes, ZPixmap);
		}
		new_icon = (CARD32 *)XtMalloc((height * width + 2) * sizeof(CARD32));
		new_icon[0] = width;
		new_icon[1] = height;
		k = 0;
		l = 2;
		m = 0;
		switch(image->depth)
		{
		case 1:
		    {
			    XColor colors[2];
			    CARD32 fg,bg;

			    XtVaGetValues(W,
				    XmNforeground, &colors[0].pixel,
				    XmNbackground, &colors[1].pixel,
				    XmNcolormap, &Pcmap,
				    NULL);
			    XQueryColors(XtDisplay(W), Pcmap, colors, 2);
			    fg = 0xff000000 + (((colors[0].red >> 8) & 0xff) << 16) +
				    (((colors[0].green >> 8) & 0xff) << 8) +
				    ((colors[0].blue >> 8) & 0xff);
			    bg = 0xff000000 + (((colors[1].red >> 8) & 0xff) << 16) +
				    (((colors[1].green >> 8) & 0xff) << 8) +
				    ((colors[1].blue >> 8) & 0xff);
			    for (j = 0; j < height; j++)
			    {
				    for (i = 0; i < width; i++)
				    {
					    if (m_image != NULL &&
						(XGetPixel(m_image, i, j) == 0))
					    {
						    new_icon[l++] = 0;
					    }
					    else
					    {
						    if (XGetPixel(image, i, j) == 0)
						    {
							    new_icon[l++] = bg;
						    }
						    else
						    {
							    new_icon[l++] = fg;
						    }
					    }
				    }
			    }
			    break;

		    }
		default: /* depth = Pdepth */
		    {
		    XColor *colors;
		    unsigned char *cm;

			    colors = (XColor *)XtMalloc(width * height * sizeof(XColor));
			    cm = (unsigned char *)XtMalloc(width * height * sizeof(unsigned char));
			    for (j = 0; j < height; j++)
			    {
				    for (i = 0; i < width; i++)
				    {
					    if (m_image != NULL &&
						(XGetPixel(m_image, i, j) == 0))
					    {
						    cm[m++] = 0;
					    }
					    else
					    {
						    cm[m++] = 255;
						    colors[k++].pixel =
							    XGetPixel(image, i, j);
					    }
				    }
			    }
			    XtVaGetValues(W,
				    XmNcolormap, &Pcmap,
				    NULL);
			    for (i = 0; i < k; i += 256)
			    {
				XQueryColors(XtDisplay(W), Pcmap, &colors[i], k - i < 256 ? k - i : 256);
			    }

			    k = 0;m = 0;
			    for (j = 0; j < height; j++)
			    {
				    for (i = 0; i < width; i++)
				    {
					    if (cm[m] > 0)
					    {
						    new_icon[l++] =
							    ((cm[m] & 0xff) << 24) +
							    (((colors[k].red >> 8) & 0xff)
							     << 16) +
							    (((colors[k].green >> 8) & 0xff)
							     << 8) +
							    ((colors[k].blue >> 8) & 0xff);
						    k++;
					    }
					    else
					    {
						    new_icon[l++] = 0;
					    }
					    m++;
				    }
			    }
			    XtFree((char *)colors);
			    XtFree(cm);
			    break;
		    }
		}
		XChangeProperty(XtDisplay(W), XtWindow(W),
				    XInternAtom(XtDisplay(W), "_NET_WM_ICON", False),
				    XInternAtom(XtDisplay(W), "CARDINAL", False),
				    32, PropModeReplace,
				    (unsigned char *)new_icon,
				    height * width + 2);

		XtFree((char *)new_icon);
	    }
	    if (image)
	    {
		XDestroyImage(image);
	    }
	    if (m_image)
	    {
		XDestroyImage(m_image);
	    }
	}
    }
}

void
XltSetClientIcon(Widget W, char **icon)
{
XpmAttributes attrib;
Pixmap pixmap = None, mask = None;

	XtVaGetValues(W,
		XtNiconMask, &mask,
		XtNiconPixmap, &pixmap,
		NULL);
	if (mask)
	{
	    XFreePixmap(XtDisplay(W),mask);
	}
	if (pixmap)
	{
	    XFreePixmap(XtDisplay(W),pixmap);
	}
	attrib.valuemask = 0;
	XpmCreatePixmapFromData(XtDisplay(W),
		XRootWindowOfScreen(XtScreen(W)),
		icon,
		&pixmap,
		&mask,
		&attrib);
	if (mask != None)
	{
		XtVaSetValues(W,
			XtNiconMask,mask,
			NULL);
	}

	XtVaSetValues(W,
		XtNiconPixmap,pixmap,
		NULL);

	if (XtIsRealized(W))
	{
	    SetProperty(W, NULL, NULL, NULL);
	}
	else
	{
	    XtAddEventHandler(W, StructureNotifyMask, False, (XtEventHandler)SetProperty, NULL);
	}
	XpmFreeAttributes(&attrib);
}
