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
 * FILE NAME:	XbrGfx.c
 * CREATED:	Wed Oct 27 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Routines to support simple graphics operations.
 *
 */

/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 *
 * Based strongly on...
 *
 * gif2ras.c - Converts from a Compuserve GIF (tm) image to a Sun raster image.
 *
 * Copyright (c) 1988, 1989 by Patrick J. Naughton
 *
 * Author: Patrick J. Naughton
 * naughton@wind.sun.com
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include <Xm/Xm.h>

#include <X11/Intrinsic.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "XbrGfx.h"

#define	GIF_EXTENSION	0x21
#define	GIF_BLOCKEND	0x00
#define	GIF_GFXBLOCK	0xf9
#define	GIF_TRANSMASK	0x01

#define NEXTBYTE (*ptr++)
#define IMAGESEP 0x2c
#define INTERLACEMASK 0x40
#define COLORMAPMASK 0x80

/*****************************************************************************/
/* VARIABLES required by all procedures.                                     */
/*****************************************************************************/
static int XC = 0, YC = 0,	/* Output X and Y coords of current pixel    */
           Height, Width,       /* Image dimensions                          */
           BytesPerScanline,    /* Bytes per scanline in output raster       */
           numused,             /* Colours already used                      */
           Pass = 0,            /* Used by output routine if interlaced pic  */
           Interlace,		/* Interlaced image?                         */
           HasColormap,		/* Global colormap?                          */
           numcols, strip = 0,  /* Number of colours, Strip colour?          */
           nostrip = 0,		/* Don't strip colours?                      */
           alloc = 0;		/* Number of colours allocated               */
static unsigned char used[256], /* Colour already used?                      */
           Red[256],            /* REDs from GIF header                      */
           Green[256],          /* GREENs from GIF header                    */
           Blue[256];           /* BLUEs from GIF header                     */
unsigned long alloced[256],	/* Allocated colour cells                    */
           cols[256];           /* Actual colours used                       */
XColor defs[256];		/* Colours as XColor types                   */

/*****************************************************************************/
/* FUNCTION declarations.                                                    */
/*****************************************************************************/
Pixmap XbrGfxLoadPixmap(Widget, char *, int, int);
XBR_IMAGE *XbrGfxGIF(Display *, unsigned char *, int, int, XColor *, char **);
static int XbrGfxGetColours(Display *, Colormap , int, unsigned char *, XColor *, int, int);
static int XbrGfxReadCode(int *, unsigned char *, int, int);
static void XbrGfxAddtoPixel(unsigned char, unsigned char *);
Pixmap XbrGfxReadBitmap(Display *, Screen *, GC, char *, int, int);
Pixmap XbrGfxLoadBitmap(Widget, char *, int, int, int);
unsigned long XbrGfxBestColour(Display *, Colormap, XColor *, int);
void XbrGfxBWPixels(Display *, Screen *, Colormap, XColor *, XColor *);
unsigned char *XbrGfxParseXBMData(char *, unsigned int *, unsigned int *);
unsigned char *XbrGfxXBMtoRaw(unsigned char *, unsigned int, unsigned int);
Pixmap XbrGfxSetPixmap(Widget, char *, int, int);
void XbrGfxShadow(Widget);

/* XbrGfxLoadPixmap:**********************************************************/
/* XbrGfxLoadPixmap: Given a bitmap definition and a widget, create a pixmap */
/* XbrGfxLoadPixmap: and build the image in the widget.                      */
/* XbrGfxLoadPixmap:**********************************************************/

Pixmap XbrGfxLoadPixmap(Widget w, char *bits, int width, int height)
{
    XGCValues vals;
    Pixmap pixmap;
    GC gc;
    Display *display = XtDisplay(w);
    Screen *screen = XtScreen(w);
    Window root = RootWindowOfScreen(screen);
   
    /* Create a GC for drawing the pixmap */
    XtVaGetValues(w, XmNforeground, &vals.foreground,
                     XmNbackground, &vals.background,
                     NULL);
    gc = XCreateGC(display, root, GCForeground|GCBackground, &vals);

    /* Change the bitmap to a pixmap */
    pixmap = XbrGfxReadBitmap(display, screen, gc, bits, width, height);

    /* Display it */
    XtVaSetValues(w, XmNlabelType, XmPIXMAP, XmNlabelPixmap, pixmap, NULL);

    /* Free GC we have finished with */
    XFreeGC(display, gc);

    /* OK. */
    return(pixmap);
}

/* XbrGfxReadBitmap:**********************************************************/
/* XbrGfxReadBitmap: Convert a single plane bitmap to a pixmap.              */
/* XbrGfxReadBitmap:**********************************************************/

Pixmap XbrGfxReadBitmap(Display *display, Screen *screen, GC gc, char *bits,
                        int width, int height)
{
    Pixmap bitmap, pixmap;

    /* Create a bitmap from the bits passed in */
    bitmap = XCreateBitmapFromData(display, RootWindowOfScreen(screen), bits,
      width, height);

    /* Create a pixmap with the same dimensions */
    pixmap = XCreatePixmap(display, RootWindowOfScreen(screen), width, height,
      DefaultDepthOfScreen(screen));

    /* Copy the bitmap onto the first plane of the pixmap */
    XCopyPlane(display, bitmap, pixmap, gc, 0, 0, width, height, 0, 0, 1);

    /* Finished with the bitmap */
    XFreePixmap(display, bitmap);

    /* Done. */
    return(pixmap);
}

/* XbrGfxLoadBitmap:**********************************************************/
/* XbrGfxLoadBitmap: Use the widget passed in to create a GC, then read in   */
/* XbrGfxLoadBitmap: the bitmap.                                             */
/* XbrGfxLoadBitmap:**********************************************************/

Pixmap XbrGfxLoadBitmap(Widget w, char *bits, int width, int height, int inherit)
{
    XGCValues vals;
    Pixmap pixmap;
    GC gc;
    Display *display = XtDisplay(w);
    Screen *screen = XtScreen(w);
    Window root = RootWindowOfScreen(screen);
  
    /* Get the foreground and background colours of the widget */
    if(inherit) {
        XtVaGetValues(w, XmNforeground, &vals.foreground, XmNbackground,
          &vals.background, NULL);
    } else {
        vals.foreground = BlackPixelOfScreen(screen);
	vals.background = WhitePixelOfScreen(screen);
    }

    /* Create a GC for drawing the pixmap */
    gc = XCreateGC(display, root, GCForeground|GCBackground, &vals);

    /* Change the bitmap to a pixmap */
    pixmap = XbrGfxReadBitmap(display, screen, gc, bits, width, height);

    /* Free GC we have finished with */
    XFreeGC(display, gc);

    /* Return the pixmap */
    return(pixmap);
}

/* XbrGfxBestColour:**********************************************************/
/* XbrGfxBestColour: Given an XColor with the RGB data filled in, find the   */
/* XbrGfxBestColour: closest match for it in the default colormap. Ian Finch */
/* XbrGfxBestColour: wrote this. (I think).                                  */
/* XbrGfxBestColour:**********************************************************/

unsigned long XbrGfxBestColour(Display *display, Colormap defmap, XColor *color,
                               int num_colors)
{
    static XColor *colormap = NULL;
    int i, best_match = 0, best_diff = 65535 * 3, colorval;

    /* NULL parameter passed to indicate we can free our mem */
    if(color == NULL) {
        if(colormap) free(colormap);
        colormap = NULL;
        return(0);
    }

    /* If we have not yet read in the colormap do it */
    if(!colormap) {
        if((colormap = (XColor *) malloc(sizeof(XColor) * num_colors)) == NULL)
            return(0);
        for(i = 0; i < num_colors; i++)
            colormap[i].pixel = i;
        XQueryColors(display, defmap, colormap, num_colors);
    }

    /* Try and get a good match */
    for(i = 0; i < num_colors; i++) {
        colorval = abs(color->red   - colormap[i].red)   +
                   abs(color->green - colormap[i].green) +
                   abs(color->blue  - colormap[i].blue);

        /* Is it a perfect match */
        if(colorval == 0) {
            best_match = i;
            break;
        }

        /* Is it better than the best one so far */
        if(colorval < best_diff) {
            best_diff = colorval;
            best_match = i;
        }
    }

    return(colormap[best_match].pixel);
}

/* XbrGfxBWPixels:************************************************************/
/* XbrGfxBWPixels: Found the black and/or white pixels of the given screen.  */
/* XbrGfxBWPixels:************************************************************/

void XbrGfxBWPixels(Display *display, Screen *screen, Colormap colourmap,
                    XColor *black, XColor *white)
{
    if(black != NULL) {
        black->pixel = BlackPixelOfScreen(screen);
        XQueryColor(display, colourmap, black);
    }

    if(white != NULL) {
        white->pixel = WhitePixelOfScreen(screen);
        XQueryColor(display, colourmap, white);
    }
}

/* XbrGfxXBMtoRaw:************************************************************/
/* XbrGfxXBMtoRaw: Convert XBM data into raw data with RGB colours and an    */
/* XbrGfxXBMtoRaw: index into the data.                                      */
/* XbrGfxXBMtoRaw:************************************************************/

unsigned char *XbrGfxXBMtoRaw(unsigned char *data, unsigned int width,
                              unsigned int height)
{
    int alloc_width = ((width+8)/8)*8;
    int new_width = (alloc_width/8);
    unsigned char mask = 255>>(alloc_width-width);
    unsigned char *newdata = (unsigned char *)
      XtMalloc((alloc_width*height)*sizeof(char));
    int i, j, k,bit;

    for(i = new_width-1; i < (new_width*height); i+=new_width) {
        data[i] = data[i] & mask;
    }

    for(i = 0, k = 0; i < (new_width*height); i++, k+=8) {
        for(j = 0,bit=1; j < 8; j++,bit<<=1) {
            newdata[k+j] = ((int)(data[i] & bit))/bit ;
        }
    }

    return(newdata);
}

/* XbrGfxParseXBMData:********************************************************/
/* XbrGfxParseXBMData: Given a bitmap file loaded into memory, parse it into */
/* XbrGfxParseXBMData: individual data items.                                */
/* XbrGfxParseXBMData:********************************************************/

unsigned char *XbrGfxParseXBMData(char *data, unsigned int *retwidth,
                                  unsigned int *retheight)
{
    char *ptr, token[50];
    int done = 0, i = 0, j = 0, width, height;
    unsigned char val = 0;
    unsigned char *newdata;

    /* Try and find the width */
    if((ptr = strstr(data, "width")) == NULL) {
        fprintf(stderr, "Could not determine width!\n");
        exit(1);
    }
    while(!isdigit(*ptr)) ptr++;
    width = atoi(ptr);
    *retwidth = width;
    width = (width + 7)/8;

    /* Try and find the height */
    if((ptr = strstr(data, "height")) == NULL) {
        return(NULL);
    }
    while(!isdigit(*ptr)) ptr++;
    height = atoi(ptr);
    *retheight = height;

    /* Allocate new data */
    if((newdata = (unsigned char *) malloc(width * height)) == NULL)
        return(NULL);

    /* Jump to first bracket */
    if((ptr = strchr(data, '{')) == NULL) {
        free(newdata);
        return(NULL);
    }

    /* Parse a token at a time */
    while(!done) {
        ptr++;
        while(isspace(*ptr)) ptr++;

        i = 0;
        while(*ptr != ',' && *ptr != '}' && *ptr != EOF) {
            token[i++] = *ptr;
            ptr++;
        }
        token[i] = 0;
        val = (char) strtol(token, (char *)NULL, 16);
        newdata[j++] = val;

        if(*ptr == '}' || *ptr == EOF) done = 1;
    }

    return(newdata);
}

/* XbrGfxGIF:*****************************************************************/
/* XbrGfxGIF: Load a GIF into a pixmap and return its width, height and the  */
/* XbrGfxGIF: colours it allocated.                                          */
/* XbrGfxGIF:*****************************************************************/
 
XBR_IMAGE *XbrGfxGIF(Display *display, unsigned char *data, int size, int html,
                     XColor *backcol, char **errmsg)
{
    register unsigned char  ch, ch1;
    register unsigned char *ptr, *ptr1;
    register int   i;
    unsigned char *raster,	/* The raster data stream, unblocked */
                  *image;	/* The image data */
    int BitOffset = 0,		/* Bit Offset of next code */
    Prefix[4096],               /* Prefix table used for decoding */
    Suffix[4096],               /* Suffix table used for decoding */
    OutCode[1025],              /* Hash table used by the decompressor */
    OutCount = 0,		/* Decompressor output 'stack count' */
    RWidth, RHeight,		/* screen dimensions */
    LeftOfs, TopOfs,		/* image offset */
    BitsPerPixel,		/* Bits per pixel, read from GIF header */
    ColorMapSize,		/* number of colors */
    Background,			/* background color */
    Transparent = 0,		/* Transparent background? */
    TransparentColour,		/* The colour that is transparent. */
    CodeSize,			/* Code size, read from GIF header */
    InitCodeSize,		/* Starting code size, used during Clear */
    Code,			/* Value returned by XbrGfxReadCode */
    MaxCode,			/* limiting value for current code size */
    ClearCode,			/* GIF clear code */
    EOFCode,			/* GIF end-of-information code */
    CurCode, OldCode, InCode,	/* Decompressor variables */
    FirstFree,			/* First free code, generated per GIF spec */
    FreeCode,			/* Decompressor, next free slot in hash table */
    FinChar,			/* Decompressor variable */
    BitMask,			/* AND mask for data size */
    ReadMask,			/* Code AND mask for current code size */
    DispCells,			/* Number of display cells */
    screen;                     /* Default screen of display */
    Colormap colormap;          /* The colour map the get colours from */
    XImage *theImage;		/* The image to build up */
    Visual *theVisual;          /* Screen visual type */
    XBR_IMAGE *retval;
    GC gc;

    DispCells = DisplayCells(display, DefaultScreen(display));
    colormap = DefaultColormap(display, DefaultScreen(display));
    theVisual = DefaultVisual(display, DefaultScreen(display));
    screen = DefaultScreen(display);

    XC = 0, YC = 0; Pass = 0; alloc = 0;

    /* Allocate memory for raster data */
    if((raster = (unsigned char *) malloc(size)) == NULL) {
        if(errmsg) *errmsg = "Out of memory!";
        return(NULL);
    }

    ptr = data;

    if(strncmp((char *)ptr, "GIF87a", 6) && strncmp((char *)ptr, "GIF89a", 6)) {
        if(errmsg) *errmsg = "Not a GIF image!";
        return(NULL);
    }
    ptr += 6;

    /* Get variables from the GIF screen descriptor */
    ch = NEXTBYTE;
    RWidth = ch + 0x100 * NEXTBYTE;
    ch = NEXTBYTE;
    RHeight = ch + 0x100 * NEXTBYTE;

    ch = NEXTBYTE;
    HasColormap = ((ch & COLORMAPMASK) ? True : False);

    BitsPerPixel = (ch & 7) + 1;
    numcols = ColorMapSize = 1 << BitsPerPixel;
    BitMask = ColorMapSize - 1;

    Background = NEXTBYTE;

    /* Supposed to be NULL */
    if(NEXTBYTE) {
        if(errmsg) *errmsg = "GIF image corrupt!";
        return(NULL);
    }

    /* Read in global colormap. */
    if(HasColormap) {
	for (i = 0; i < ColorMapSize; i++) {
	    Red[i] = NEXTBYTE;
	    Green[i] = NEXTBYTE;
	    Blue[i] = NEXTBYTE;
            used[i] = 0;
	}
        numused = 0;
    }
    else {
        if(!numcols) numcols=256;
        for (i=0; i<numcols; i++)
            cols[i] = (unsigned long) i;
    }

    /* Chomp extensions. */
    while((ch = NEXTBYTE) == GIF_EXTENSION) {
        if(NEXTBYTE == GIF_GFXBLOCK) {
	    ch = NEXTBYTE;	/* Block size */
	    Transparent = ((NEXTBYTE & GIF_TRANSMASK) ? True : False);
	    ptr += 2;
	    TransparentColour = NEXTBYTE;
	    ch = NEXTBYTE;
	} else {
	    ch = NEXTBYTE;
	    ptr += ch;
	    while((ch = NEXTBYTE) != GIF_BLOCKEND)
		ptr += ch;
	}
    }
    
    /* Check for image seperator */
    if (ch != IMAGESEP) {
        if(errmsg) *errmsg = "Corrupt GIF file: *no image separator*";
        return(NULL);
    }

    /* Now read in values from the image descriptor */
    ch = NEXTBYTE;
    LeftOfs = ch + 0x100 * NEXTBYTE;
    ch = NEXTBYTE;
    TopOfs = ch + 0x100 * NEXTBYTE;
    ch = NEXTBYTE;
    Width = ch + 0x100 * NEXTBYTE;
    ch = NEXTBYTE;
    Height = ch + 0x100 * NEXTBYTE;
    Interlace = ((NEXTBYTE & INTERLACEMASK) ? True : False);

/*
    xbrimage_resize(Width, Height);
    xbrimage_dimensions(Width, Height);
*/

    /* Note that I ignore the possible existence of a local color map.
     * I'm told there aren't many files around that use them, and the spec
     * says it's defined for future use.  This could lead to an error
     * reading some files. 
     */

    /* Start reading the raster data. First we get the intial code size
     * and compute decompressor constant values, based on this code size.
     */
    CodeSize = NEXTBYTE;
    ClearCode = (1 << CodeSize);
    EOFCode = ClearCode + 1;
    FreeCode = FirstFree = ClearCode + 2;

    /* The GIF spec has it that the code size is the code size used to
     * compute the above values is the code size given in the file, but the
     * code size used in compression/decompression is the code size given in
     * the file plus one. (thus the ++).
     */
    CodeSize++;
    InitCodeSize = CodeSize;
    MaxCode = (1 << CodeSize);
    ReadMask = MaxCode - 1;

    /* Read the raster data.  Here we just transpose it from the GIF array
     * to the raster array, turning it from a series of blocks into one long
     * data stream, which makes life much easier for XbrGfxReadCode().
     */
    ptr1 = raster;
    do {
	ch = ch1 = NEXTBYTE;
	while (ch--) *ptr1++ = NEXTBYTE;
	if ((raster - ptr1) > size) {
	    if(errmsg) *errmsg = "Corrupt GIF file\n*unblock*";
            return(NULL);
        }
    } while(ch1);


    /* Allocate the X image */
    image = (unsigned char *) malloc(Width*Height);
    if (image == NULL) {
        if(errmsg) *errmsg = "Not enough memory for Ximage!";
        return(NULL);
    }
    if(!html) {
        theImage = XCreateImage(display,theVisual,8,ZPixmap,0,(char *)image,
          Width, Height,8,Width);

        if(theImage == NULL) {
            if(errmsg) *errmsg = "Unable to create Ximage!";
             return(NULL);
       }
    }

    BytesPerScanline = Width;

    /* Decompress the file, continuing until you see the GIF EOF code.
       One obvious enhancement is to add checking for corrupt files here.
    */
    Code = XbrGfxReadCode(&BitOffset, raster, CodeSize, ReadMask);
    while (Code != EOFCode) {

        /* Clear code sets everything back to its initial value, then reads the
          immediately subsequent code as uncompressed data.
        */
	if (Code == ClearCode) {
	    CodeSize = InitCodeSize;
	    MaxCode = (1 << CodeSize);
	    ReadMask = MaxCode - 1;
	    FreeCode = FirstFree;
	    CurCode = OldCode = Code = XbrGfxReadCode(&BitOffset, raster,
              CodeSize, ReadMask);
	    FinChar = CurCode & BitMask;
	    XbrGfxAddtoPixel(FinChar, image);
	}
	else {

            /* If not a clear code, then must be data:save same as CurCode and
               InCode
            */
	    CurCode = InCode = Code;

            /* If greater or equal to FreeCode, not in the hash table yet;
               repeat the last character decoded
            */
	    if (CurCode >= FreeCode) {
		CurCode = OldCode;
		OutCode[OutCount++] = FinChar;
	    }

            /* Unless this code is raw data, pursue the chain pointed to by
              CurCode through the hash table to its end; each code in the chain
              puts its associated output code on the output queue.
            */
	    while (CurCode > BitMask) {
		if (OutCount > 1024) {
		    if(errmsg) *errmsg = "Corrupt GIF file!*OutCount*";
                    if(!html) XDestroyImage(theImage);
                    theImage = NULL;
                    return(NULL);
                    }
		OutCode[OutCount++] = Suffix[CurCode];
		CurCode = Prefix[CurCode];
	    }

            /* The last code in the chain is treated as raw data. */
	    FinChar = CurCode & BitMask;
	    OutCode[OutCount++] = FinChar;

            /* Now we put the data out to the Output routine.
               It's been stacked LIFO, so deal with it that way...
            */
            for (i = OutCount - 1; i >= 0; i--)
		XbrGfxAddtoPixel(OutCode[i], image);
	    OutCount = 0;

            /* Build the hash table on-the-fly. No table is stored in the
               file.
            */
	    Prefix[FreeCode] = OldCode;
	    Suffix[FreeCode] = FinChar;
	    OldCode = InCode;

            /* Point to the next slot in the table.  If we exceed the current
               MaxCode value, increment the code size unless it's already 12.
               If it is, do nothing: the next code decompressed better be CLEAR
            */
	    FreeCode++;
	    if (FreeCode >= MaxCode) {
		if (CodeSize < 12) {
		    CodeSize++;
		    MaxCode *= 2;
		    ReadMask = (1 << CodeSize) - 1;
		}
	    }
	}
	Code = XbrGfxReadCode(&BitOffset, raster, CodeSize, ReadMask);
    }

    free(raster);
/*
    xbrimage_colours(numused, ColorMapSize);
*/

    retval = XtNew(XBR_IMAGE);
    retval->size = size;
    retval->width = Width;
    retval->height = Height;
    retval->data = (char *)data;

    if(html) {
        retval->pixmap = (Pixmap)NULL;
        retval->num_colors = numcols;
        retval->reds = (int *) XtMalloc(sizeof(int)*numcols);
        retval->greens = (int *) XtMalloc(sizeof(int)*numcols);
        retval->blues = (int *) XtMalloc(sizeof(int)*numcols);
        for(i = 0; i < numcols; i++) {
            retval->reds[i] = Red[i] << 8;
            retval->greens[i] = Green[i] << 8;
            retval->blues[i] = Blue[i] << 8;
        }
	if(Transparent) {
	    retval->reds[TransparentColour] = backcol->red;
	    retval->greens[TransparentColour] = backcol->green;
	    retval->blues[TransparentColour] = backcol->blue;
	}
        retval->image_data = image;
        retval->npixels = 0;
    } else {
        retval->pixmap = XCreatePixmap(display, DefaultRootWindow(display),
          Width, Height, DefaultDepth(display, screen));
        retval->num_colors = numcols;
        retval->reds = NULL;
        retval->greens = NULL;
        retval->blues = NULL;
        retval->image_data = NULL;
        if(!XbrGfxGetColours(display, colormap,DispCells, image,backcol,Transparent,TransparentColour)) {
            if(errmsg) *errmsg = "Error allocating colours!";
            XDestroyImage(theImage);
            theImage = NULL;
            return(NULL);
        }

        for(i = 0; i < alloc; i++) {
            retval->pixels[i] = alloced[i];
        }
        retval->npixels = alloc-1;

        gc = XCreateGC(display, retval->pixmap,  0, NULL);
        XPutImage(display, retval->pixmap, gc, theImage, 0, 0, 0, 0, Width,
          Height);
        XFreeGC(display, gc);
        XDestroyImage(theImage);
    }

    theImage = NULL;

    return(retval);
}


/* XbrGfxReadCode:************************************************************/
/* XbrGfxReadCode: Fetch the next code from the raster data stream.  The     */
/* XbrGfxReadCode: codes can be any length from 3 to 12 bits, packed into    */
/* XbrGfxReadCode: 8-bit unsigned chars, so we have to maintain our location */
/* XbrGfxReadCode: in the raster array as a BIT Offset.  We compute the      */
/* XbrGfxReadCode: unsigned char Offset into the raster array by dividing    */
/* XbrGfxReadCode: this by 8, pick upthree unsigned chars, compute the bit   */
/* XbrGfxReadCode: Offset into our 24-bit chunk, shift to bring the desired  */
/* XbrGfxReadCode: code to the bottom, then mask it off and return it.       */
/* XbrGfxReadCode:************************************************************/

static int XbrGfxReadCode(int *BitOffset, unsigned char *raster, int CodeSize,
                    int ReadMask)
{
    int RawCode, ByteOffset;

    ByteOffset = (*BitOffset) / 8;
    RawCode = raster[ByteOffset] + (0x100 * raster[ByteOffset + 1]);
    if (CodeSize >= 8) RawCode += (0x10000 * raster[ByteOffset + 2]);
    RawCode >>= (*(BitOffset) % 8);
    (*BitOffset) += CodeSize;
    return(RawCode & ReadMask);
}


/* XbrGfxAddtoPixel:**********************************************************/
/* XbrGfxAddtoPixel: Add the pixel with the appropriate index into the       */
/* XbrGfxAddtoPixel: colour map.                                             */
/* XbrGfxAddtoPixel:**********************************************************/

static void XbrGfxAddtoPixel(unsigned char Index, unsigned char *image)
{
    register int slider_val;

    if (YC<Height)
        *(image + YC * BytesPerScanline + XC) = Index;

    if (!used[Index]) { used[Index]=1;  numused++; }

    /* Update the X-coordinate, and if it overflows, update the Y-coordinate */
    if (++XC == Width) {

    /* If a non-interlaced picture, just increment YC to the next scan line. 
       If it's interlaced, deal with the interlace as described in the GIF
       spec.  Put the decoded scan line out to the screen if we haven't gone
        past the bottom of it
     */
	XC = 0;
	if (!Interlace) {
            YC++;
            /* slider_val = (int) (((float)YC/(float)Height) * 100.0);
            xbrimage_slide(slider_val); */
        }
	else {
	    switch (Pass) {
		case 0:
		    YC += 8;
		    if (YC >= Height) {
			Pass++;
			YC = 4;
		    }
		break;
		case 1:
		    YC += 8;
		    if (YC >= Height) {
			Pass++;
			YC = 2;
		    }
		break;
		case 2:
		    YC += 4;
		    if (YC >= Height) {
			Pass++;
			YC = 1;
		    }
		break;
		case 3:
		    YC += 2;
		break;
		default:
		break;
	    }
	}
    }
}

/* XbrGfxGetColours:**********************************************************/
/* XbrGfxGetColours: Once we have the colour requirements we can try and     */
/* XbrGfxGetColours: get the requested colours from the colourmap.           */
/* XbrGfxGetColours:**********************************************************/

static int XbrGfxGetColours(Display *display, Colormap colormap, int DispCells,
                            unsigned char *image, XColor *backcol,
			    int Transparent, int TransparentColour)
{
    register int i, j;
    unsigned char *ptr;

    /* No need to allocate any colors if no colormap in GIF file */
    if (!HasColormap) return(1);

    for(i = 0; i < numcols; i++) {
	if(Transparent && i == TransparentColour) {
	    cols[i] = backcol->pixel;
	} else if(used[i]) {
            defs[i].red = Red[i] <<8;
            defs[i].green = Green[i] <<8;
            defs[i].blue  = Blue[i] <<8;
            defs[i].flags = DoRed | DoGreen | DoBlue;
            if(XAllocColor(display, colormap, &defs[i])) {
                cols[i] = defs[i].pixel;
                alloced[alloc++] = defs[i].pixel;
            } else {
                defs[i].pixel = XbrGfxBestColour(display, colormap, &defs[i],
                    256);
                cols[i] = defs[i].pixel;
            }
        }
    }

    XbrGfxBestColour(display, colormap, NULL, 256);

    ptr = image;
    for (i=0; i<Height; i++)
        for (j=0; j<Width; j++,ptr++) 
            *ptr = (unsigned char) cols[*ptr];

    return(1);
                
}

/*----------------------------------------------------------------------------
  XbrGfxSetPixmap()
  Function to create and set the icon pixmap of a shell.
  ----------------------------------------------------------------------------*/
Pixmap XbrGfxSetPixmap(Widget shell, char *bits, int width, int height)
{
    Pixmap pixmap;

    pixmap = XbrGfxLoadBitmap(shell, bits, width, height, 0);

    XtVaSetValues(shell, XmNiconPixmap, pixmap, NULL);

    return(pixmap);
}

/*----------------------------------------------------------------------------
  XbrGfxShadow()
  Get the shadow colours and set them for the given widget.
  
  Widget w	The widget to wreck
  ----------------------------------------------------------------------------*/
void XbrGfxShadow(Widget w)
{
    Pixel background, top_shadow, bottom_shadow;
    Screen *screen = XtScreen(w);
    
    /* Get the background pixel of the widget. */
    XtVaGetValues(w, XmNbackground, &background, NULL);

    /* Generate the shadow colours */
    XmGetColors(screen, DefaultColormapOfScreen(screen), background, NULL,
		&top_shadow, &bottom_shadow, NULL);
    
    /* Set shadow values. */
    XtVaSetValues(w, XmNshadowThickness, 2,
		     XmNtopShadowColor,  top_shadow,
		     XmNbottomShadowColor, bottom_shadow,
		     NULL);
}
