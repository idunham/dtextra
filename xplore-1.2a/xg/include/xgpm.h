/*@
 * Copyright(c) 1995-1997 Gregory M. Messner
 * All rights reserved
 *
 * Permission to use, copy, modify and distribute this material for
 * non-commercial personal and educational use without fee is hereby
 * granted, provided that the above copyright notice and this permission 
 * notice appear in all copies, and that the name of Gregory M. Messner
 * not be used in advertising or publicity pertaining to this material
 * without the specific, prior written permission of Gregory M. Messner 
 * or an authorized representative.
 *
 * GREGORY M. MESSNER MAKES NO REPRESENTATIONS AND EXTENDS NO WARRANTIES, 
 * EXPRESS OR IMPLIED, WITH RESPECT TO THE SOFTWARE, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * ANY PARTICULAR PURPOSE, AND THE WARRANTY AGAINST INFRINGEMENT OF PATENTS
 * OR OTHER INTELLECTUAL PROPERTY RIGHTS. THE SOFTWARE IS PROVIDED "AS IS",
 * AND IN NO EVENT SHALL GREGORY M. MESSNER BE LIABLE FOR ANY DAMAGES,
 * INCLUDING ANY LOST PROFITS OR OTHER INCIDENTAL OR CONSEQUENTIAL DAMAGES
 * RELATING TO THE SOFTWARE.
 *
 */

/*
 * Copyright 1990, 1991 GROUPE BULL
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of GROUPE BULL not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  GROUPE BULL makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * GROUPE BULL disclaims all warranties with regard to this software,
 * including all implied warranties of merchantability and fitness,
 * in no event shall GROUPE BULL be liable for any special,
 * indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits,
 * whether in an action of contract, negligence or other tortious
 * action, arising out of or in connection with the use 
 * or performance of this software.
 *
 */

#ifndef _XGPM_H_
#define _XGPM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifndef PROTO
#  if defined(__STDC__) || defined(__cplusplus)
#	define PROTO(f, a)	f a
#  else
#	define PROTO(f, a)	f ()
#  endif
#endif


PROTO(extern Pixmap XgpmWidgetPixmap, ( 
	Widget w, char **pixmapData, Pixel fg, Pixel bg, Pixel none_color ) );


#ifdef USE_XGPM



/* Return ErrorStatus codes:
 * null     if full success
 * positive if partial success
 * negative if failure
 */

#define XgpmColorError    1
#define XgpmSuccess       0
#define XgpmOpenFailed   -1
#define XgpmFileInvalid  -2
#define XgpmNoMemory     -3
#define XgpmColorFailed  -4

typedef struct {
    char *name;                         /* Symbolic color name */
    char *value;                        /* Color value */
    Pixel pixel;                        /* Color pixel */
} XgpmColorSymbol;

typedef struct {
    unsigned long valuemask;            /* Specifies which attributes are
                                         * defined */

    Visual *visual;                     /* Specifies the visual to use */
    Colormap colormap;                  /* Specifies the colormap to use */
    unsigned int depth;                 /* Specifies the depth */
    unsigned int width;                 /* Returns the width of the created
                                         * pixmap */
    unsigned int height;                /* Returns the height of the created
                                         * pixmap */
    unsigned int x_hotspot;             /* Returns the x hotspot's
                                         * coordinate */
    unsigned int y_hotspot;             /* Returns the y hotspot's
                                         * coordinate */
    unsigned int cpp;                   /* Specifies the number of char per
                                         * pixel */
    Pixel *pixels;                      /* List of used color pixels */
    unsigned int npixels;               /* Number of pixels */
    XgpmColorSymbol *colorsymbols;       /* Array of color symbols to
                                         * override */
    unsigned int numsymbols;            /* Number of symbols */
    char *rgb_fname;                    /* RGB text file name */

    /* Infos */
    unsigned int ncolors;               /* Number of colors */
    char ***colorTable;                 /* Color table pointer */
    char *hints_cmt;                    /* Comment of the hints section */
    char *colors_cmt;                   /* Comment of the colors section */
    char *pixels_cmt;                   /* Comment of the pixels section */
    unsigned int mask_pixel;            /* Transparent pixel's color table
                                         * index */
} XgpmAttributes;

/* Xgpm attribute value masks bits */
#define XgpmVisual          (1L<<0)
#define XgpmColormap        (1L<<1)
#define XgpmDepth           (1L<<2)
#define XgpmSize            (1L<<3)      /* width & height */
#define XgpmHotspot         (1L<<4)      /* x_hotspot & y_hotspot */
#define XgpmCharsPerPixel   (1L<<5)
#define XgpmColorSymbols    (1L<<6)
#define XgpmRgbFilename     (1L<<7)
#define XgpmInfos           (1L<<8)      /* all infos members */

#define XgpmReturnPixels    (1L<<9)
#define XgpmReturnInfos     XgpmInfos

PROTO(extern int XgpmCreatePixmapFromData, (
	Display *display,
	Drawable d,
	char **data,
	Pixmap * pixmap_return,
	Pixmap * shapemask_return,
	XgpmAttributes * attributes));

PROTO(extern int XgpmCreateImageFromData, (
	Display *display,
	char **data,
	XImage ** image_return,
	XImage ** shapemask_return,
	XgpmAttributes * attributes));


PROTO(extern void XgpmFreeAttributes, (
	XgpmAttributes * attributes));




#else	/* USE_XGPM */

#include "X11/xpm.h"
#define XgpmOpenFailed		XpmOpenFailed
#define XgpmFileInvalid		XpmFileInvalid
#define XgpmNoMemory		XpmNoMemory
#define XgpmColorFailed 	XpmColorFailed

#endif  /* USE_XGPM */



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _XGPM_H_ */

