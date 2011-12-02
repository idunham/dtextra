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
 * FILE NAME:	XbrGfx.h
 * CREATED:	Wed Oct 27 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Header file for XbrGfx.c.
 *
 */

#ifndef	XBRLIB_XBRGFX_H
#define	XBRLIB_XBRGFX_H		1

typedef struct xbrimage {
    int size, width, height;
    char *data;
    unsigned long pixels[256];
    unsigned char *image_data;
    int *reds, *greens, *blues;
    int num_colors, npixels;
    Pixmap pixmap;
} XBR_IMAGE;

extern Pixmap XbrGfxLoadPixmap(Widget, char *, int, int);
extern Pixmap XbrGfxReadBitmap(Display *, Screen *, GC, char *, int, int);
extern unsigned long XbrGfxBestColour(Display *, Colormap, XColor *, int);
extern void XbrGfxBWPixels(Display *, Screen *, Colormap, XColor *, XColor *);
extern unsigned char *XbrGfxParseXBMData(char *, unsigned int *,unsigned int *);
extern XBR_IMAGE *XbrGfxGIF(Display *, unsigned char *, int, int, XColor *, char **);
extern unsigned char *XbrGfxXBMtoRaw(unsigned char *,unsigned int,unsigned int);
extern Pixmap XbrGfxLoadBitmap(Widget, char *, int, int, int);
extern Pixmap XbrGfxSetPixmap(Widget, char *, int, int);
extern void XbrGfxShadow(Widget);

#endif
