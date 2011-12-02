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


#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <ctype.h>


#include "xgpm.h"

#ifdef USE_XGPM


typedef struct {
    unsigned int type;
    union {
        FILE *file;
        char **data;
    }     stream;
    char *cptr;
    unsigned int line;
    int CommentLength;
    char Comment[BUFSIZ];
    char *Bcmt, *Ecmt, Bos, Eos;
    unsigned int InsideString;          /* used during parsing: 0 or 1
                                         * whether we are inside or not */
} xgpmData;

#define XPMARRAY 0
#define XPMFILE  1
#define XPMPIPE  2

typedef unsigned char byte;

extern char *xgpmColorKeys[];

#define TRANSPARENT_COLOR "None"        /* this must be a string! */
#define BACKGROUND_COLOR  "Background"
#define FOREGROUND_COLOR  "Foreground"

/* number of xgpmColorKeys */
#define NKEYS 5

/*
 * key numbers for visual type, they must fit along with the number key of
 * each corresponding element in xgpmColorKeys[] defined in xgpm.h
 */
#define MONO    2
#define GRAY4   3
#define GRAY    4
#define COLOR   5

/* structure containing data related to an Xgpm pixmap */
typedef struct {
    char *name;
    unsigned int width;
    unsigned int height;
    unsigned int cpp;
    unsigned int ncolors;
    char ***colorTable;
    unsigned int *pixelindex;
    XColor *xcolors;
    char **colorStrings;
    unsigned int mask_pixel;            /* mask pixel's colorTable index */
} xgpmInternAttrib;

#define UNDEF_PIXEL 0x80000000

char *xgpmColorKeys[] =
{
	"s",		/* key #1: symbol */
	"m",		/* key #2: mono visual */
	"g4",		/* key #3: 4 grays visual */
	"g",		/* key #4: gray visual */
	"c",		/* key #5: color visual */
};

/* XPM private routines */

PROTO(static int xgpmCreateImage, (
	Display *display,
	xgpmInternAttrib * attrib,
	XImage ** image_return,
	XImage ** shapeimage_return,
	XgpmAttributes * attributes));

PROTO(static int xgpmParseData, (xgpmData * data,
	xgpmInternAttrib * attrib_return,
	XgpmAttributes * attributes));

PROTO(static int xgpmVisualType, (
	Visual * visual));

PROTO(static void xgpmFreeColorTable, (
	char ***colorTable, 
	int ncolors));

PROTO(static int xgpmGetBestColor, (Display *, Colormap, XColor *));

PROTO(static void xgpmInitInternAttrib, (
	xgpmInternAttrib *xmpdata));

PROTO(static void xgpmFreeInternAttrib, (
	xgpmInternAttrib * xmpdata));

PROTO(static void xgpmSetAttributes, (
	xgpmInternAttrib * attrib,
	XgpmAttributes * attributes));

/* I/O utility */

PROTO(static void xgpmNextString, (
	xgpmData * mdata));

PROTO(static int xgpmNextUI, (
	xgpmData * mdata, 
	unsigned int *ui_return));

PROTO(static int xgpmGetC, (
	xgpmData * mdata));

PROTO(static int xgpmUngetC, (
	int c, 
	xgpmData * mdata));

PROTO(static unsigned int xgpmNextWord, (
	xgpmData * mdata, 
	char *buf));

PROTO(static void xgpmGetCmt, (
	xgpmData * mdata, 
	char **cmt));

PROTO(static int xgpmOpenArray, (
	char **data, 
	xgpmData * mdata));

PROTO(static void XgpmDataClose, (
	xgpmData * mdata));

/* RGB utility */

PROTO(static void xgpm_xynormalizeimagebits, (
	register unsigned char *bp,
	register XImage * img));

PROTO(static void xgpm_znormalizeimagebits, (
	register unsigned char *bp,
	register XImage * img));

/* Image utility */

PROTO(static int SetColor, (
	Display *display, 
	Colormap colormap, 
	char *colorname,
	unsigned int color_index, 
	Pixel * image_pixel,
	Pixel * mask_pixel, 
	unsigned int *mask_pixel_index));

PROTO(static int CreateXImage, (
	Display *display, 
	Visual * visual,
	unsigned int depth, 
	unsigned int width,
	unsigned int height, 
	XImage ** image_return));

PROTO(static void SetImagePixels, (
	XImage * image, 
	unsigned int width,
	unsigned int height, 
	unsigned int *pixelindex,
	Pixel * pixels));

PROTO(static void SetImagePixels32, (
	XImage * image, 
	unsigned int width,
	unsigned int height, 
	unsigned int *pixelindex,
	Pixel * pixels));

PROTO(static void SetImagePixels16, (
	XImage * image, 
	unsigned int width,
	unsigned int height, 
	unsigned int *pixelindex,
	Pixel * pixels));

PROTO(static void SetImagePixels8, (
	XImage * image, 
	unsigned int width,
	unsigned int height, 
	unsigned int *pixelindex,
	Pixel * pixels));

PROTO(static void SetImagePixels1, (
	XImage * image, 
	unsigned int width,
	unsigned int height, 	
	unsigned int *pixelindex,
	Pixel * pixels));

PROTO(static unsigned int atoui, (
	char *p, 
	unsigned int l, 
	unsigned int *ui_return));

/*
 * Macros
 *
 * The XYNORMALIZE macro determines whether XY format data requires
 * normalization and calls a routine to do so if needed. The logic in
 * this module is designed for LSBFirst byte and bit order, so
 * normalization is done as required to present the data in this order.
 *
 * The ZNORMALIZE macro performs byte and nibble order normalization if
 * required for Z format data.
 *
 * The XYINDEX macro computes the index to the starting byte (char) boundary
 * for a bitmap_unit containing a pixel with coordinates x and y for image
 * data in XY format.
 *
 * The ZINDEX* macros compute the index to the starting byte (char) boundary
 * for a pixel with coordinates x and y for image data in ZPixmap format.
 *
 */

#define XYNORMALIZE(bp, img) \
    if ((img->byte_order == MSBFirst) || (img->bitmap_bit_order == MSBFirst)) \
        xgpm_xynormalizeimagebits((unsigned char *)(bp), img)

#define ZNORMALIZE(bp, img) \
    if (img->byte_order == MSBFirst) \
        xgpm_znormalizeimagebits((unsigned char *)(bp), img)

#define XYINDEX(x, y, img) \
    ((y) * img->bytes_per_line) + \
    (((x) + img->xoffset) / img->bitmap_unit) * (img->bitmap_unit >> 3)

#define ZINDEX(x, y, img) ((y) * img->bytes_per_line) + \
    (((x) * img->bits_per_pixel) >> 3)

#define ZINDEX32(x, y, img) ((y) * img->bytes_per_line) + ((x) << 2)

#define ZINDEX16(x, y, img) ((y) * img->bytes_per_line) + ((x) << 1)

#define ZINDEX8(x, y, img) ((y) * img->bytes_per_line) + (x)

#define ZINDEX1(x, y, img) ((y) * img->bytes_per_line) + ((x) >> 3)

#if __STDC__
#define Const const
#else
#define Const
#endif

static Boolean
CompareStrings(in, test)
String in, test;
{
int	i;

	if ( in == NULL || test == NULL )
		return False;

	/*
	 * Strip leading whitespace off the in string.
	 */
	while ( isspace(*in) )
		in++;

	for ( i = 0; *test != '\0' && !isspace(*in); test++, in++, i++ )
	{
	char c = *in;
	char t = *test;

		if ( isupper(c) )
			c = tolower(c);

		if ( isupper(t) )
			t = tolower(t);

	        if ( c != t )
			return False;
	}

	if ( *test == '\0' && (*in == '\0' || isspace(*in)) )
		return True;
	else
		return False;
}


static unsigned int 
atoui(p, l, ui_return)
register char *p;
unsigned int l;
unsigned int *ui_return;
{
    register int n, i;

    n = 0;
    for (i = 0; i < l; i++)
        if (*p >= '0' && *p <= '9')
            n = n * 10 + *p++ - '0';
        else
            break;

    if (i != 0 && i == l) {
        *ui_return = n;
        return 1;
    } else
        return 0;
}

#ifndef _NO_PROTO
XgpmCreatePixmapFromData(
	Display *display,
	Drawable d,
	char **data,
	Pixmap *pixmap_return,
	Pixmap *shapemask_return,
	XgpmAttributes *attributes
)

#else
int 
XgpmCreatePixmapFromData(display, d, data, pixmap_return, 
	shapemask_return, attributes)
Display *display;
Drawable d;
char **data;
Pixmap *pixmap_return;
Pixmap *shapemask_return;
XgpmAttributes *attributes;
#endif
{
XImage *image = NULL, **imageptr = NULL;
XImage *shapeimage = NULL, **shapeimageptr = NULL;
int ErrorStatus;
XGCValues gcv;
GC gc;

    /*
     * initialize return values
     */
    if (pixmap_return) {
        *pixmap_return = None;
        imageptr = &image;
    }
    if (shapemask_return) {
        *shapemask_return = None;
        shapeimageptr = &shapeimage;
    }

    /*
     * create the images
     */
    ErrorStatus = XgpmCreateImageFromData(display, data, imageptr,
                                         shapeimageptr, attributes);
    if (ErrorStatus < 0)
        return (ErrorStatus);

    /*
     * create the pixmaps
     */
    if (imageptr && image) {
        *pixmap_return = XCreatePixmap(display, d, image->width,
                                       image->height, image->depth);
        gcv.function = GXcopy;
        gc = XCreateGC(display, *pixmap_return, GCFunction, &gcv);

        XPutImage(display, *pixmap_return, gc, image, 0, 0, 0, 0,
                  image->width, image->height);

        XDestroyImage(image);
        XFreeGC(display, gc);
    }
    if (shapeimageptr && shapeimage) {
        *shapemask_return = XCreatePixmap(display, d, shapeimage->width,
                                          shapeimage->height,
                                          shapeimage->depth);
        gcv.function = GXcopy;
        gc = XCreateGC(display, *shapemask_return, GCFunction, &gcv);

        XPutImage(display, *shapemask_return, gc, shapeimage, 0, 0, 0, 0,
                  shapeimage->width, shapeimage->height);

        XDestroyImage(shapeimage);
        XFreeGC(display, gc);
    }
    return (ErrorStatus);
}


int 
XgpmCreateImageFromData(display, data, image_return, 
	shapeimage_return, attributes)
Display *display;
char **data;
XImage **image_return;
XImage **shapeimage_return;
XgpmAttributes *attributes;
{
xgpmData mdata;
int ErrorStatus;
xgpmInternAttrib attrib;

    /*
     * initialize return values
     */
    if (image_return)
        *image_return = NULL;
    if (shapeimage_return)
        *shapeimage_return = NULL;

    if ((ErrorStatus = xgpmOpenArray(data, &mdata)) != XgpmSuccess)
        return (ErrorStatus);

    xgpmInitInternAttrib(&attrib);

    ErrorStatus = xgpmParseData(&mdata, &attrib, attributes);

    if (ErrorStatus == XgpmSuccess)
        ErrorStatus = xgpmCreateImage(display, &attrib, image_return,
                                     shapeimage_return, attributes);

    if (ErrorStatus >= 0)
        xgpmSetAttributes(&attrib, attributes);
    else if (attributes)
        XgpmFreeAttributes(attributes);

    xgpmFreeInternAttrib(&attrib);
    XgpmDataClose(&mdata);

    return (ErrorStatus);
}

/*
 * open the given array to be read or written as an xgpmData which is returned
 */
static int 
xgpmOpenArray(data, mdata)
char **data;
xgpmData *mdata;
{
    mdata->type = XPMARRAY;
    mdata->stream.data = data;
    mdata->cptr = *data;
    mdata->line = 0;
    mdata->CommentLength = 0;
    mdata->Bcmt = mdata->Ecmt = NULL;
    mdata->Bos = mdata->Eos = '\0';
    mdata->InsideString = 0;
    return (XgpmSuccess);
}

/*
 * Intialize the xgpmInternAttrib pointers to Null to know
 * which ones must be freed later on.
 */
static void 
xgpmInitInternAttrib(attrib)
xgpmInternAttrib *attrib;
{
    attrib->ncolors = 0;
    attrib->colorTable = NULL;
    attrib->pixelindex = NULL;
    attrib->xcolors = NULL;
    attrib->colorStrings = NULL;
    attrib->mask_pixel = UNDEF_PIXEL;
}

/* function call in case of error, frees only localy allocated variables */
#undef RETURN
#define RETURN(status) \
  { if (colorTable) xgpmFreeColorTable(colorTable, ncolors); \
    if (chars) free(chars); \
    if (pixelindex) free((char *)pixelindex); \
    if (hints_cmt)  free((char *)hints_cmt); \
    if (colors_cmt) free((char *)colors_cmt); \
    if (pixels_cmt) free((char *)pixels_cmt); \
    return(status); }

/*
 * This function parses an Xgpm file or data and store the found informations
 * in an an xgpmInternAttrib structure which is returned.
 */
static int 
xgpmParseData(data, attrib_return, attributes)
xgpmData *data;
xgpmInternAttrib *attrib_return;
XgpmAttributes *attributes;
{
    /* variables to return */
    unsigned int width, height;
    unsigned int ncolors = 0;
    unsigned int cpp;
    unsigned int x_hotspot, y_hotspot, hotspot = 0;
    char ***colorTable = NULL;
    unsigned int *pixelindex = NULL;
    char *hints_cmt = NULL;
    char *colors_cmt = NULL;
    char *pixels_cmt = NULL;

    /* calculation variables */
    unsigned int rncolors = 0;		/* read number of colors, it is
					 * different to ncolors to avoid
					 * problem when freeing the
					 * colorTable in case an error
					 * occurs while reading the hints
					 * line */
    unsigned int key;			/* color key */
    char *chars = NULL, buf[BUFSIZ];
    unsigned int *iptr;
    unsigned int a, b, x, y, l;

    unsigned int curkey;		/* current color key */
    unsigned int lastwaskey;		/* key read */
    char curbuf[BUFSIZ];		/* current buffer */

    /*
     * read hints: width, height, ncolors, chars_per_pixel 
     */
    if (!(xgpmNextUI(data, &width) && xgpmNextUI(data, &height)
	  && xgpmNextUI(data, &rncolors) && xgpmNextUI(data, &cpp)))
	RETURN(XgpmFileInvalid);

    ncolors = rncolors;

    /*
     * read hotspot coordinates if any 
     */
    hotspot = xgpmNextUI(data, &x_hotspot) && xgpmNextUI(data, &y_hotspot);

    /*
     * store the hints comment line 
     */
    if (attributes && (attributes->valuemask & XgpmReturnInfos))
	xgpmGetCmt(data, &hints_cmt);

    /*
     * read colors 
     */
    colorTable = (char ***) calloc(ncolors, sizeof(char **));
    if (!colorTable)
	RETURN(XgpmNoMemory);

    for (a = 0; a < ncolors; a++) {
	xgpmNextString(data);		/* skip the line */
	colorTable[a] = (char **) calloc((NKEYS + 1), sizeof(char *));
	if (!colorTable[a])
	    RETURN(XgpmNoMemory);

	/*
	 * read pixel value 
	 */
	colorTable[a][0] = (char *) malloc(cpp);
	if (!colorTable[a][0])
	    RETURN(XgpmNoMemory);
	for (b = 0; b < cpp; b++)
	    colorTable[a][0][b] = xgpmGetC(data);

	/*
	 * read color keys and values 
	 */
	curkey = 0;
	lastwaskey = 0;
	while (l = xgpmNextWord(data, buf)) {
	    if (!lastwaskey) {
		for (key = 1; key < NKEYS + 1; key++)
		    if ((strlen(xgpmColorKeys[key - 1]) == l)
			&& (!strncmp(xgpmColorKeys[key - 1], buf, l)))
			break;
	    }
	    if (!lastwaskey && key <= NKEYS) {	/* open new key */
		if (curkey) {		/* flush string */
		    colorTable[a][curkey] =
			(char *) malloc(strlen(curbuf) + 1);
		    if (!colorTable[a][curkey])
			RETURN(XgpmNoMemory);
		    strcpy(colorTable[a][curkey], curbuf);
		}
		curkey = key;		/* set new key  */
		curbuf[0] = '\0';	/* reset curbuf */
		lastwaskey = 1;
	    } else {
		if (!curkey)
		    RETURN(XgpmFileInvalid);	/* key without value */
		if (!lastwaskey)
		    strcat(curbuf, " ");/* append space */
		buf[l] = '\0';
		strcat(curbuf, buf);	/* append buf */
		lastwaskey = 0;
	    }
	}
	if (!curkey)
	    RETURN(XgpmFileInvalid);	/* key without value */
	colorTable[a][curkey] = (char *) malloc(strlen(curbuf) + 1);
	if (!colorTable[a][curkey])
	    RETURN(XgpmNoMemory);
	strcpy(colorTable[a][curkey], curbuf);
    }

    /*
     * store the colors comment line 
     */
    if (attributes && (attributes->valuemask & XgpmReturnInfos))
	xgpmGetCmt(data, &colors_cmt);

    /*
     * read pixels and index them on color number 
     */
    pixelindex =
	(unsigned int *) malloc(sizeof(unsigned int) * width * height);
    if (!pixelindex)
	RETURN(XgpmNoMemory);

    iptr = pixelindex;

    chars = (char *) malloc(cpp);
    if (!chars)
	RETURN(XgpmNoMemory);

    for (y = 0; y < height; y++) {
	xgpmNextString(data);
	for (x = 0; x < width; x++, iptr++) {
	    for (a = 0; a < cpp; a++)
		chars[a] = xgpmGetC(data);
	    for (a = 0; a < ncolors; a++)
		if (!strncmp(colorTable[a][0], chars, cpp))
		    break;
	    if (a == ncolors)
		RETURN(XgpmFileInvalid);	/* no color matches */
	    *iptr = a;
	}
    }

    /*
     * store the pixels comment line 
     */
    if (attributes && (attributes->valuemask & XgpmReturnInfos))
	xgpmGetCmt(data, &pixels_cmt);

    free(chars);

    /*
     * store found informations in the xgpmInternAttrib structure 
     */
    attrib_return->width = width;
    attrib_return->height = height;
    attrib_return->cpp = cpp;
    attrib_return->ncolors = ncolors;
    attrib_return->colorTable = colorTable;
    attrib_return->pixelindex = pixelindex;

    if (attributes) {
	if (attributes->valuemask & XgpmReturnInfos) {
	    attributes->hints_cmt = hints_cmt;
	    attributes->colors_cmt = colors_cmt;
	    attributes->pixels_cmt = pixels_cmt;
	}
	if (hotspot) {
	    attributes->x_hotspot = x_hotspot;
	    attributes->y_hotspot = y_hotspot;
	    attributes->valuemask |= XgpmHotspot;
	}
    }
    return (XgpmSuccess);
}

/*
 * set the color pixel related to the given colorname,
 * return 0 if success, 1 otherwise.
 */

#ifndef _NO_PROTO
static int 
SetColor(
	Display *display,
	Colormap colormap,
	char *colorname,
	unsigned int color_index,
	Pixel *image_pixel,
	Pixel *mask_pixel,
	unsigned int *mask_pixel_index
)
#else
static int 
SetColor(display, colormap,colorname, color_index, image_pixel, 
	mask_pixel, mask_pixel_index)
Display *display;
Colormap colormap;
char *colorname;
unsigned int color_index;
Pixel *image_pixel;
Pixel *mask_pixel;
unsigned int *mask_pixel_index;
#endif
{
    XColor xcolor;

    if ( CompareStrings(colorname, TRANSPARENT_COLOR) == FALSE ) 
    {
	if (!XParseColor(display, colormap, colorname, &xcolor))
		return (1);

	if (!XAllocColor(display, colormap, &xcolor))
		if ( !xgpmGetBestColor(display, colormap,&xcolor) )
	    		return (1);
	*image_pixel = xcolor.pixel;
	*mask_pixel = 1;
    } else {
	*image_pixel = 0;
	*mask_pixel = 0;
	*mask_pixel_index = color_index;/* store the color table index */
    }
    return (0);
}

#ifndef _NO_PROTO
static int xgpmGetBestColor(Display *display, Colormap cmap, XColor *xcolor)
#else
static int xgpmGetBestColor(display, cmap, xcolor)
	Display *display;
	Colormap cmap;
	XColor *xcolor;
#endif
{
	XColor xcolors[256];
	long closest_distance, distance, tmp;
	int use_pixel = -1;
	int max_colors;
	int i;

	max_colors = DisplayCells(display, DefaultScreen(display));
	if (max_colors > 256)
		max_colors = 256;

	for ( i = 0; i < max_colors; i++ )
		xcolors[i].pixel = i;
	XQueryColors(display, cmap, xcolors, max_colors);

	tmp = 32766 * 30 / 100;
	closest_distance = tmp * tmp;
	tmp = 32766 * 61 / 100;
	closest_distance += tmp * tmp;
	tmp = 32766 * 11 / 100;
	closest_distance += tmp * tmp;


	for ( i = 0; i < max_colors; i++ )
	{
	    	/*
	    	 * Use Euclidean distance in RGB space, weighted by Y (of YIQ)
	   	 * as the objective function;  this accounts for differences
	    	 * in the color sensitivity of the eye.
	    	 */
		tmp = (((int)xcolor->red) - (int)xcolors[i].red) * 30 / 100;
		distance = tmp * tmp;
		tmp = (((int)xcolor->green) - (int)xcolors[i].green) * 61 / 100;
		distance += tmp * tmp;
		tmp = (((int)xcolor->blue) - (int)xcolors[i].blue) * 11 / 100;
		distance += tmp * tmp;
		if ( distance < closest_distance )
		{
			closest_distance = distance;
			use_pixel = i;
		}
	}

	if ( use_pixel == -1 )
		return (0);

	xcolor->pixel = use_pixel;
	
	return (1);		
}


/* function call in case of error, frees only localy allocated variables */
#undef RETURN
#define RETURN(status) \
  { if (image) XDestroyImage(image); \
    if (shapeimage) XDestroyImage(shapeimage); \
    if (image_pixels) free((char *)image_pixels); \
    if (mask_pixels) free((char *)mask_pixels); \
    return(status); }

static int 
xgpmCreateImage(display, attrib, image_return, shapeimage_return, attributes)
Display *display;
xgpmInternAttrib *attrib;
XImage **image_return;
XImage **shapeimage_return;
XgpmAttributes *attributes;
{
    /* variables stored in the XgpmAttributes structure */
    Visual *visual;
    Colormap colormap;
    unsigned int depth;
    XgpmColorSymbol *colorsymbols, *symbol;
    unsigned int numsymbols;

    /* variables to return */
    XImage *image = NULL;
    XImage *shapeimage = NULL;
    unsigned int mask_pixel;
    unsigned int ErrorStatus, ErrorStatus2;

    /* calculation variables */
    Pixel *image_pixels = NULL;
    Pixel *mask_pixels = NULL;
    char *colorname;
    unsigned int a, b, l;
    Boolean pixel_defined;
    unsigned int key;


    /*
     * retrieve information from the XgpmAttributes 
     */
    if (attributes && attributes->valuemask & XgpmColorSymbols) {
	colorsymbols = attributes->colorsymbols;
	numsymbols = attributes->numsymbols;
    } else
	numsymbols = 0;

    if (attributes && attributes->valuemask & XgpmVisual)
	visual = attributes->visual;
    else
	visual = DefaultVisual(display, DefaultScreen(display));

    if (attributes && attributes->valuemask & XgpmColormap)
	colormap = attributes->colormap;
    else
	colormap = DefaultColormap(display, DefaultScreen(display));

    if (attributes && attributes->valuemask & XgpmDepth)
	depth = attributes->depth;
    else
	depth = DefaultDepth(display, DefaultScreen(display));


    ErrorStatus = XgpmSuccess;

    /*
     * alloc pixels index tables 
     */

    key = xgpmVisualType(visual);
    image_pixels = (Pixel *) malloc(sizeof(Pixel) * attrib->ncolors);
    if (!image_pixels)
	RETURN(XgpmNoMemory);

    mask_pixels = (Pixel *) malloc(sizeof(Pixel) * attrib->ncolors);
    if (!mask_pixels)
	RETURN(XgpmNoMemory);

    mask_pixel = UNDEF_PIXEL;

    /*
     * get pixel colors, store them in index tables 
     */
    for (a = 0; a < attrib->ncolors; a++) {
	colorname = NULL;
	pixel_defined = False;

	/*
	 * look for a defined symbol 
	 */
	if ( numsymbols )
	{
	    for ( l = 0; l < numsymbols; l++, symbol++)
		if ( CompareStrings(colorsymbols[l].name, 
			attrib->colorTable[a][1]) == True )
				break;

	    if (l != numsymbols) 
	    {
		if (colorsymbols[l].value)
		    colorname = colorsymbols[l].value;
		else
		    pixel_defined = True;
	    }
	}
	if (!pixel_defined) {		/* pixel not given as symbol value */

	    if (colorname) {		/* colorname given as symbol value */
		if (!SetColor(display, colormap, colorname, a,
			   &image_pixels[a], &mask_pixels[a], &mask_pixel))
		    pixel_defined = True;
		else
		    ErrorStatus = XgpmColorError;
	    }
	    b = key;
	    while (!pixel_defined && b > 1) {
		if (attrib->colorTable[a][b]) {
		    if (!SetColor(display, colormap, attrib->colorTable[a][b],
				  a, &image_pixels[a], &mask_pixels[a],
				  &mask_pixel)) {
			pixel_defined = True;
			break;
		    } else
			ErrorStatus = XgpmColorError;
		}
		b--;
	    }

	    b = key + 1;
	    while (!pixel_defined && b < NKEYS + 1) {
		if (attrib->colorTable[a][b]) {
		    if (!SetColor(display, colormap, attrib->colorTable[a][b],
				  a, &image_pixels[a], &mask_pixels[a],
				  &mask_pixel)) {
			pixel_defined = True;
			break;
		    } else
			ErrorStatus = XgpmColorError;
		}
		b++;
	    }

	    if (!pixel_defined)
		RETURN(XgpmColorFailed);

	} else {
	    image_pixels[a] = colorsymbols[l].pixel;
	    mask_pixels[a] = 1;
	}
    }

    /*
     * create the image 
     */
    if (image_return) {
	ErrorStatus2 = CreateXImage(display, visual, depth,
				    attrib->width, attrib->height, &image);
	if (ErrorStatus2 != XgpmSuccess)
	    RETURN(ErrorStatus2);

	/*
	 * set the image data 
	 *
	 * In case depth is 1 or bits_per_pixel is 4, 6, 8, 24 or 32 use
	 * optimized functions, otherwise use slower but sure general one. 
	 *
	 */

	if (image->depth == 1)
	    SetImagePixels1(image, attrib->width, attrib->height,
			    attrib->pixelindex, image_pixels);
	else if (image->bits_per_pixel == 8)
	    SetImagePixels8(image, attrib->width, attrib->height,
			    attrib->pixelindex, image_pixels);
	else if (image->bits_per_pixel == 16)
	    SetImagePixels16(image, attrib->width, attrib->height,
			     attrib->pixelindex, image_pixels);
	else if (image->bits_per_pixel == 32)
	    SetImagePixels32(image, attrib->width, attrib->height,
			     attrib->pixelindex, image_pixels);
	else
	    SetImagePixels(image, attrib->width, attrib->height,
			   attrib->pixelindex, image_pixels);
    }

    /*
     * create the shape mask image 
     */
    if (mask_pixel != UNDEF_PIXEL && shapeimage_return) {
	ErrorStatus2 = CreateXImage(display, visual, 1, attrib->width,
				    attrib->height, &shapeimage);
	if (ErrorStatus2 != XgpmSuccess)
	    RETURN(ErrorStatus2);

	SetImagePixels1(shapeimage, attrib->width, attrib->height,
			attrib->pixelindex, mask_pixels);
    }
    free((char *)mask_pixels);

    /*
     * if requested store allocated pixels in the XgpmAttributes structure 
     */
    if (attributes &&
	(attributes->valuemask & XgpmReturnInfos
	 || attributes->valuemask & XgpmReturnPixels)) {
	if (mask_pixel != UNDEF_PIXEL) {
	    Pixel *pixels, *p1, *p2;

	    attributes->npixels = attrib->ncolors - 1;
	    pixels = (Pixel *) malloc(sizeof(Pixel) * attributes->npixels);
	    if (pixels) {
		p1 = image_pixels;
		p2 = pixels;
		for (a = 0; a < attrib->ncolors; a++, p1++)
		    if (a != mask_pixel)
			*p2++ = *p1;
		attributes->pixels = pixels;
	    } else {
		/* if error just say we can't return requested data */
		attributes->valuemask &= ~XgpmReturnPixels;
		attributes->valuemask &= ~XgpmReturnInfos;
		attributes->pixels = NULL;
		attributes->npixels = 0;
	    }
	    free((char *)image_pixels);
	} else {
	    attributes->pixels = image_pixels;
	    attributes->npixels = attrib->ncolors;
	}
	attributes->mask_pixel = mask_pixel;
    } else
	free((char *)image_pixels);


    /*
     * return created images 
     */
    if (image_return)
	*image_return = image;

    if (shapeimage_return)
	*shapeimage_return = shapeimage;

    return (ErrorStatus);
}


/*
 * Create an XImage
 */
static int 
CreateXImage(display, visual, depth, width, height, image_return)
Display *display;
Visual *visual;
unsigned int depth;
unsigned int width;
unsigned int height;
XImage **image_return;
{
    int bitmap_pad;

    /* first get bitmap_pad */
    if (depth > 16)
	bitmap_pad = 32;
    else if (depth > 8)
	bitmap_pad = 16;
    else
	bitmap_pad = 8;

    /* then create the XImage with data = NULL and bytes_per_line = 0 */

    *image_return = XCreateImage(display, visual, depth, ZPixmap, 0, 0,
				 width, height, bitmap_pad, 0);
    if (!*image_return)
	return (XgpmNoMemory);

    /* now that bytes_per_line must have been set properly alloc data */

    (*image_return)->data =
	(char *) malloc((*image_return)->bytes_per_line * height);

    if (!(*image_return)->data) {
	XDestroyImage(*image_return);
	*image_return = NULL;
	return (XgpmNoMemory);
    }
    return (XgpmSuccess);
}


/*
 * The functions below are written from X11R5 MIT's code (XImUtil.c)
 *
 * The idea is to have faster functions than the standard XPutPixel function
 * to build the image data. Indeed we can speed up things by supressing tests
 * performed for each pixel. We do exactly the same tests but at the image
 * level. Assuming that we use only ZPixmap images. 
 */

PROTO(static void _putbits, (
	register char *src, 
	int dstoffset,
	register int numbits, 
	register char *dst));

PROTO(static void _XReverse_Bytes, (
	register unsigned char *bpt, 
	register int nb));

static unsigned char Const _reverse_byte[0x100] = {
			    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
			    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
			    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
			    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
			    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
			    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
			    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
			    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
			    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
			    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
			    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
			    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
			    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
			    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
			    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
			    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
			    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
			    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
			    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
			    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
			    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
			    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
			    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
			    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
			    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
			    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
			    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
			    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
			    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
			    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
			    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
			     0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

static void 
_XReverse_Bytes(bpt, nb)
register unsigned char *bpt;
register int nb;
{
    do {
	*bpt = _reverse_byte[*bpt];
	bpt++;
    } while (--nb > 0);
}

static void 
xgpm_xynormalizeimagebits(bp,img)
register unsigned char *bp;
register XImage *img;
{
    register unsigned char c;

    if (img->byte_order != img->bitmap_bit_order) {
	switch (img->bitmap_unit) {

	case 16:
	    c = *bp;
	    *bp = *(bp + 1);
	    *(bp + 1) = c;
	    break;

	case 32:
	    c = *(bp + 3);
	    *(bp + 3) = *bp;
	    *bp = c;
	    c = *(bp + 2);
	    *(bp + 2) = *(bp + 1);
	    *(bp + 1) = c;
	    break;
	}
    }
    if (img->bitmap_bit_order == MSBFirst)
	_XReverse_Bytes(bp, img->bitmap_unit >> 3);
}

static void 
xgpm_znormalizeimagebits(bp,img)
register unsigned char *bp;
register XImage *img;
{
    register unsigned char c;

    switch (img->bits_per_pixel) {

    case 4:
	*bp = ((*bp >> 4) & 0xF) | ((*bp << 4) & ~0xF);
	break;

    case 16:
	c = *bp;
	*bp = *(bp + 1);
	*(bp + 1) = c;
	break;

    case 24:
	c = *(bp + 2);
	*(bp + 2) = *bp;
	*bp = c;
	break;

    case 32:
	c = *(bp + 3);
	*(bp + 3) = *bp;
	*bp = c;
	c = *(bp + 2);
	*(bp + 2) = *(bp + 1);
	*(bp + 1) = c;
	break;
    }
}

static unsigned char Const _lomask[0x09] = {
		     0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
static unsigned char Const _himask[0x09] = {
		     0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00};

static void 
_putbits(src, dstoffset, numbits, dst)
register char *src;	/* address of source bit string */
int dstoffset;		/* bit offset into destination; range is 0-31 */
register int numbits;	/* number of bits to copy to destination */
register char *dst;	/* address of destination bit string */
{
    register unsigned char chlo, chhi;
    int hibits;

    dst = dst + (dstoffset >> 3);
    dstoffset = dstoffset & 7;
    hibits = 8 - dstoffset;
    chlo = *dst & _lomask[dstoffset];
    for (;;) {
	chhi = (*src << dstoffset) & _himask[dstoffset];
	if (numbits <= hibits) {
	    chhi = chhi & _lomask[dstoffset + numbits];
	    *dst = (*dst & _himask[dstoffset + numbits]) | chlo | chhi;
	    break;
	}
	*dst = chhi | chlo;
	dst++;
	numbits = numbits - hibits;
	chlo = (unsigned char) (*src & _himask[hibits]) >> hibits;
	src++;
	if (numbits <= dstoffset) {
	    chlo = chlo & _lomask[numbits];
	    *dst = (*dst & _himask[numbits]) | chlo;
	    break;
	}
	numbits = numbits - dstoffset;
    }
}

/*
 * Default method to write pixels into a Z image data structure.
 * The algorithm used is:
 *
 *	copy the destination bitmap_unit or Zpixel to temp
 *	normalize temp if needed
 *	copy the pixel bits into the temp
 *	renormalize temp if needed
 *	copy the temp back into the destination image data
 */

static void
SetImagePixels(image, width, height, pixelindex, pixels)
XImage *image;
unsigned int width;
unsigned int height;
unsigned int *pixelindex;
Pixel *pixels;
{
    Pixel pixel;
    unsigned long px;
    register char *src;
    register char *dst;
    int nbytes;
    register unsigned int *iptr;
    register int x, y, i;

    iptr = pixelindex;
    if (image->depth == 1) {
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		pixel = pixels[*iptr];
		for (i = 0, px = pixel;
		     i < sizeof(unsigned long); i++, px >>= 8)
		    ((unsigned char *) &pixel)[i] = (unsigned char)px;
		src = &image->data[XYINDEX(x, y, image)];
		dst = (char *) &px;
		px = 0;
		nbytes = image->bitmap_unit >> 3;
		for (i = nbytes; --i >= 0;)
		    *dst++ = *src++;
		XYNORMALIZE(&px, image);
		i = ((x + image->xoffset) % image->bitmap_unit);
		_putbits((char *) &pixel, i, 1, (char *) &px);
		XYNORMALIZE(&px, image);
		src = (char *) &px;
		dst = &image->data[XYINDEX(x, y, image)];
		for (i = nbytes; --i >= 0;)
		    *dst++ = *src++;
	    }
    } else {
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		pixel = pixels[*iptr];
		if (image->depth == 4)
		    pixel &= 0xf;
		for (i = 0, px = pixel;
		     i < sizeof(unsigned long); i++, px >>= 8)
		    ((unsigned char *) &pixel)[i] = (unsigned char)px;
		src = &image->data[ZINDEX(x, y, image)];
		dst = (char *) &px;
		px = 0;
		nbytes = (image->bits_per_pixel + 7) >> 3;
		for (i = nbytes; --i >= 0;)
		    *dst++ = *src++;
		ZNORMALIZE(&px, image);
		_putbits((char *) &pixel,
			 (x * image->bits_per_pixel) & 7,
			 image->bits_per_pixel, (char *) &px);
		ZNORMALIZE(&px, image);
		src = (char *) &px;
		dst = &image->data[ZINDEX(x, y, image)];
		for (i = nbytes; --i >= 0;)
		    *dst++ = *src++;
	    }
    }
}

/*
 * write pixels into a 32-bits Z image data structure
 */

#ifndef WORD64
static unsigned long byteorderpixel = MSBFirst << 24;

#endif

static void
SetImagePixels32(image, width, height, pixelindex, pixels)
XImage *image;
unsigned int width;
unsigned int height;
unsigned int *pixelindex;
Pixel *pixels;
{
    register unsigned char *addr;
    register unsigned int *paddr;
    register unsigned int *iptr;
    register int x, y;

    iptr = pixelindex;
#ifndef WORD64
    if (*((char *) &byteorderpixel) == image->byte_order) {
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		paddr =
		    (unsigned int *)(&(image->data[ZINDEX32(x, y, image)]));
		*paddr = (unsigned int)pixels[*iptr];
	    }
    } else
#endif
    if (image->byte_order == MSBFirst)
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &((unsigned char *) image->data)[ZINDEX32(x, y, image)];
		addr[0] = (unsigned char)(pixels[*iptr] >> 24);
		addr[1] = (unsigned char)(pixels[*iptr] >> 16);
		addr[2] = (unsigned char)(pixels[*iptr] >> 8);
		addr[3] = (unsigned char)(pixels[*iptr]);
	    }
    else
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &((unsigned char *) image->data)[ZINDEX32(x, y, image)];
		addr[3] = (unsigned char)(pixels[*iptr] >> 24);
		addr[2] = (unsigned char)(pixels[*iptr] >> 16);
		addr[1] = (unsigned char)(pixels[*iptr] >> 8);
		addr[0] = (unsigned char)(pixels[*iptr]);
	    }
}

/*
 * write pixels into a 16-bits Z image data structure
 */

static void 
SetImagePixels16(image, width, height, pixelindex, pixels)
XImage * image;
unsigned int width;
unsigned int height;
unsigned int *pixelindex;
Pixel *pixels;
{
    register unsigned char *addr;
    register unsigned int *iptr;
    register int x, y;

    iptr = pixelindex;
    if (image->byte_order == MSBFirst)
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &((unsigned char *) image->data)[ZINDEX16(x, y, image)];
		addr[0] = (unsigned char)(pixels[*iptr] >> 8);
		addr[1] = (unsigned char)(pixels[*iptr]);
	    }
    else
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &((unsigned char *) image->data)[ZINDEX16(x, y, image)];
		addr[1] = (unsigned char)(pixels[*iptr] >> 8);
		addr[0] = (unsigned char)(pixels[*iptr]);
	    }
}

/*
 * write pixels into a 8-bits Z image data structure
 */

static void 
SetImagePixels8(image, width, height, pixelindex, pixels)
XImage *image;
unsigned int width;
unsigned int height;
unsigned int *pixelindex;
Pixel *pixels;
{
    register unsigned int *iptr;
    register int x, y;

    iptr = pixelindex;
    for (y = 0; y < height; y++)
	for (x = 0; x < width; x++, iptr++)
	    image->data[ZINDEX8(x, y, image)] = (char)pixels[*iptr];
}

/*
 * write pixels into a 1-bit depth image data structure and **offset null**
 */

static void
SetImagePixels1(image, width, height, pixelindex, pixels)
XImage *image;
unsigned int width;
unsigned int height;
unsigned int *pixelindex;
Pixel *pixels;
{
    unsigned char bit;
    int xoff, yoff;
    register unsigned int *iptr;
    register int x, y;

    if (image->byte_order != image->bitmap_bit_order)
	SetImagePixels(image, width, height, pixelindex, pixels);
    else {
	iptr = pixelindex;
	if (image->bitmap_bit_order == MSBFirst)
	    for (y = 0; y < height; y++)
		for (x = 0; x < width; x++, iptr++) {
		    yoff = ZINDEX1(x, y, image);
		    xoff = x & 7;
		    bit = 0x80 >> xoff;
		    if (pixels[*iptr] & 1)
			image->data[yoff] |= bit;
		    else
			image->data[yoff] &= ~bit;
		}
	else
	    for (y = 0; y < height; y++)
		for (x = 0; x < width; x++, iptr++) {
		    yoff = ZINDEX1(x, y, image);
		    xoff = x & 7;
		    bit = 1 << xoff;
		    if (pixels[*iptr] & 1)
			image->data[yoff] |= bit;
		    else
			image->data[yoff] &= ~bit;
		}
    }
}

/*
 * Store into the XgpmAttributes structure the required informations stored in
 * the xgpmInternAttrib structure.
 */

static void 
xgpmSetAttributes(attrib, attributes)
xgpmInternAttrib *attrib;
XgpmAttributes *attributes;
{
    if (attributes) {
        if (attributes->valuemask & XgpmReturnInfos) {
            attributes->cpp = attrib->cpp;
            attributes->ncolors = attrib->ncolors;
            attributes->colorTable = attrib->colorTable;

            attrib->ncolors = 0;
            attrib->colorTable = NULL;
        }
        attributes->width = attrib->width;
        attributes->height = attrib->height;
        attributes->valuemask |= XgpmSize;
    }
}

/*
 * Free the XgpmAttributes structure members
 * but the structure itself
 */

void 
XgpmFreeAttributes(attributes)
XgpmAttributes *attributes;
{
    if (attributes) {
        if (attributes->valuemask & XgpmReturnPixels && attributes->pixels) {
            free((char *)attributes->pixels);
            attributes->pixels = NULL;
            attributes->npixels = 0;
        }
        if (attributes->valuemask & XgpmInfos) {
            if (attributes->colorTable) {
                xgpmFreeColorTable(attributes->colorTable, attributes->ncolors);
                attributes->colorTable = NULL;
                attributes->ncolors = 0;
            }
            if (attributes->hints_cmt) {
                free(attributes->hints_cmt);
                attributes->hints_cmt = NULL;
            }
            if (attributes->colors_cmt) {
                free(attributes->colors_cmt);
                attributes->colors_cmt = NULL;
            }
            if (attributes->pixels_cmt) {
                free(attributes->pixels_cmt);
                attributes->pixels_cmt = NULL;
            }
            if (attributes->pixels) {
                free((char *)attributes->pixels);
                attributes->pixels = NULL;
            }
        }
        attributes->valuemask = 0;
    }
}

/*
 * Free the xgpmInternAttrib pointers which have been allocated
 */

static void 
xgpmFreeInternAttrib(attrib)
xgpmInternAttrib *attrib;
{
    unsigned int a;

    if (attrib->colorTable)
        xgpmFreeColorTable(attrib->colorTable, attrib->ncolors);
    if (attrib->pixelindex)
        free((char *)attrib->pixelindex);
    if (attrib->xcolors)
        free((char *)attrib->xcolors);
    if (attrib->colorStrings) {
        for (a = 0; a < attrib->ncolors; a++)
            if (attrib->colorStrings[a])
                free((char *)attrib->colorStrings[a]);
        free((char *)attrib->colorStrings);
    }
}

/*
 * close the file related to the xgpmData if any
 */
static void 
XgpmDataClose(mdata)
xgpmData *mdata;
{
    switch (mdata->type) {
    case XPMARRAY:
        break;
    case XPMFILE:
        if (mdata->stream.file != (stdout) && mdata->stream.file != (stdin))
            fclose(mdata->stream.file);
        break;
#ifdef ZPIPE
    case XPMPIPE:
        pclose(mdata->stream.file);
#endif
    }
}

/*
 * skip whitespace and compute the following unsigned int,
 * returns 1 if one is found and 0 if not
 */
static int 
xgpmNextUI(mdata, ui_return)
xgpmData *mdata;
unsigned int *ui_return;
{
    char buf[BUFSIZ];
    int l;

    l = xgpmNextWord(mdata, buf);
    return atoui(buf, l, ui_return);
}

/*
 * get the current comment line
 */
static void 
xgpmGetCmt(mdata, cmt)
xgpmData *mdata;
char **cmt;
{
    switch (mdata->type) {
    case XPMARRAY:
        *cmt = NULL;
        break;
    case XPMFILE:
    case XPMPIPE:
        if (mdata->CommentLength) {
            *cmt = (char *) malloc(mdata->CommentLength + 1);
            strncpy(*cmt, mdata->Comment, mdata->CommentLength);
            (*cmt)[mdata->CommentLength] = '\0';
            mdata->CommentLength = 0;
        } else
            *cmt = NULL;
        break;
    }
}

/*
 * skip to the end of the current string and the beginning of the next one
 */
static void
xgpmNextString(mdata)
xgpmData *mdata;
{
    int c;

    switch (mdata->type) {
    case XPMARRAY:
        mdata->cptr = (mdata->stream.data)[++mdata->line];
        break;
    case XPMFILE:
    case XPMPIPE:
        if (mdata->Eos)
            while ((c = xgpmGetC(mdata)) != mdata->Eos && c != EOF);
        if (mdata->Bos)                 /* if not natural XPM2 */
            while ((c = xgpmGetC(mdata)) != mdata->Bos && c != EOF);
        break;
    }
}

/*
 * return the current character, skipping comments
 */
static int 
xgpmGetC(mdata)
xgpmData *mdata;
{
    int c;
    register unsigned int n = 0, a;
    unsigned int notend;

    switch (mdata->type) {
    case XPMARRAY:
        return (*mdata->cptr++);
    case XPMFILE:
    case XPMPIPE:
        c = getc(mdata->stream.file);

        if (mdata->Bos && mdata->Eos
            && (c == mdata->Bos || c == mdata->Eos)) {
            /* if not natural XPM2 */
            mdata->InsideString = !mdata->InsideString;
            return (c);
        }
        if (!mdata->InsideString && mdata->Bcmt && c == mdata->Bcmt[0]) {
            mdata->Comment[0] = c;

            /*
             * skip the string begining comment
             */
            do {
                c = getc(mdata->stream.file);
                mdata->Comment[++n] = c;
            } while (c == mdata->Bcmt[n] && mdata->Bcmt[n] != '\0'
                     && c != EOF);

            if (mdata->Bcmt[n] != '\0') {
                /* this wasn't the begining of a comment */
                /* put characters back in the order that we got them */
                for (a = n; a > 0; a--)
                    xgpmUngetC(mdata->Comment[a], mdata);
                return (mdata->Comment[0]);
            }

            /*
             * store comment
             */
            mdata->Comment[0] = mdata->Comment[n];
            notend = 1;
            n = 0;
            while (notend) {
                while (mdata->Comment[n] != mdata->Ecmt[0] && c != EOF) {
                    c = getc(mdata->stream.file);
                    mdata->Comment[++n] = c;
                }
                mdata->CommentLength = n;
                a = 0;
                do {
                    c = getc(mdata->stream.file);
                    n++;
                    a++;
                    mdata->Comment[n] = c;
                } while (c == mdata->Ecmt[a] && mdata->Ecmt[a] != '\0'
                         && c != EOF);
                if (mdata->Ecmt[a] == '\0') {
                    /* this is the end of the comment */
                    notend = 0;
                    xgpmUngetC(mdata->Comment[n], mdata);
                }
            }
            c = xgpmGetC(mdata);
        }
        return (c);
    }
}


/*
 * push the given character back
 */
static int 
xgpmUngetC(c, mdata)
int c;
xgpmData *mdata;
{
    switch (mdata->type) {
    case XPMARRAY:
        return (*--mdata->cptr = c);
    case XPMFILE:
    case XPMPIPE:
        if (mdata->Bos && (c == mdata->Bos || c == mdata->Eos))
            /* if not natural XPM2 */
            mdata->InsideString = !mdata->InsideString;
        return (ungetc(c, mdata->stream.file));
    }
}

/*
 * skip whitespace and return the following word
 */
static unsigned int 
xgpmNextWord(mdata, buf)
xgpmData *mdata;
char *buf;
{
    register unsigned int n = 0;
    int c;

    switch (mdata->type) {
    case XPMARRAY:
        while (isspace(c = *mdata->cptr) && c != mdata->Eos)
            mdata->cptr++;
        do {
            c = *mdata->cptr++;
            buf[n++] = c;
        } while (!isspace(c) && c != mdata->Eos && c != '\0');
        n--;
        mdata->cptr--;
        break;
    case XPMFILE:
    case XPMPIPE:
        while (isspace(c = xgpmGetC(mdata)) && c != mdata->Eos);
        while (!isspace(c) && c != mdata->Eos && c != EOF) {
            buf[n++] = c;
            c = xgpmGetC(mdata);
        }
        xgpmUngetC(c, mdata);
        break;
    }
    return (n);
}

static int 
xgpmVisualType(visual)
Visual *visual;
{
    switch ( visual->class )
    {
    case StaticGray:
    case GrayScale:
	switch (visual->map_entries)
	{
	case 2:
	    return (MONO);
	case 4:
	    return (GRAY4);
	default:
	    return (GRAY);
	}
    default:
	return (COLOR);
    }
}

/*
 * Free the computed color table
 */

static void 
xgpmFreeColorTable(colorTable, ncolors)
char ***colorTable;
int ncolors;
{
    int a, b;

    if (colorTable) {
        for (a = 0; a < ncolors; a++)
            if (colorTable[a]) {
                for (b = 0; b < (NKEYS + 1); b++)
                    if (colorTable[a][b])
                        free(colorTable[a][b]);
                free((char *)colorTable[a]);
            }
        free((char *)colorTable);
    }
}


#endif /* USE_XGPM */


#ifdef _NO_PROTO
Pixmap XgpmWidgetPixmap(w, pixmapData, fg, bg, np)
Widget w;
char **pixmapData;
Pixel fg, bg, np;
#else
Pixmap XgpmWidgetPixmap(Widget w, char **pixmapData, 
	Pixel fg, Pixel bg, Pixel np)
#endif
{
#ifdef USE_XGPM
XgpmAttributes	attributes;
XgpmColorSymbol	trans[3];
#else
XpmAttributes	attributes;
XpmColorSymbol	trans;
#endif
Pixmap		pixmap;
Pixmap		shape = None;
int		returnValue;
Display		*display = XtDisplay(w);

    
	XtVaGetValues(w,
		XmNdepth, &(attributes.depth),
		XmNcolormap, &(attributes.colormap),
		NULL);

	attributes.visual = DefaultVisual(display, DefaultScreen(display));

	attributes.colorsymbols = trans;
	attributes.numsymbols = 3;
	trans[0].value = trans[1].value = trans[2].value = NULL;
	trans[0].name = "None";
	trans[1].name = "foreground";
	trans[2].name = "background";
	trans[0].pixel = np;
	trans[1].pixel = fg;
	trans[2].pixel = bg;

#ifdef USE_XGPM
	attributes.valuemask = (XgpmDepth | XgpmColormap | 
		XgpmVisual | XgpmColorSymbols);

	returnValue = XgpmCreatePixmapFromData(display,
		DefaultRootWindow(display),
		pixmapData, &pixmap, &shape,
		&attributes);
#else
	attributes.valuemask = (XpmDepth | XpmColormap | 
		XpmVisual | XpmColorSymbols);
    
	returnValue = XpmCreatePixmapFromData(display,
		  DefaultRootWindow(display),
		  pixmapData, &pixmap, &shape,
		  &attributes);
#endif

	if ( shape != None )
	{
		XFreePixmap(display, shape);
	}	

	switch ( returnValue )
	{
	  case XgpmOpenFailed:
	  case XgpmFileInvalid:
	  case XgpmNoMemory:
	  case XgpmColorFailed:
	  	XtWarning("Could not create pixmap.");
	  	return(XmUNSPECIFIED_PIXMAP);

	  default:
	        break;
	}

	return pixmap;
}

