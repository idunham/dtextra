/********************************************************************
 * $Id: getpixelbyname.c,v 1.1 2002/12/01 22:00:56 rockyb Exp $
 *         This example code is modified from the book:
 *
 *           The X Window System: Programming and Applications with Xt
 *           Second OSF/Motif Edition
 *         by
 *           Douglas Young
 *           Prentice Hall, 1994
 *
 *         Copyright 1994 by Prentice Hall
 *         All Rights Reserved
 *
 *  Permission to use, copy, modify, and distribute this software for 
 *  any purpose except publication and without fee is hereby granted, provided 
 *  that the above copyright notice appear in all copies of the software.
 * *****************************************************************************/

/*******************************************************************
 * getpixelbyname.c: Get a Pixel value to represent a named color
 *******************************************************************/
#include "getpixelbyname.h"

Boolean GetPixelByName (const Widget w, const char *colorname, 
			Pixel *pixel) 
{
    Display *dpy  = XtDisplay ( w );
    int      scr  = DefaultScreen ( dpy );
    Colormap cmap = DefaultColormap ( dpy, scr );
    XColor   color, ignore;

   /* 
    * Allocate the named color.
    */

    if ( XAllocNamedColor ( dpy, cmap, colorname, &color, &ignore ) ) {
      *pixel = color.pixel;
      return ( True );
    } else {
      return (False);
    }
}
