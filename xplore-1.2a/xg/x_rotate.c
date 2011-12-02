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


/* +++FILEHDR+++
 *
 *	Filename: x_rotate.c
 *
 *	Description:
 *		This is the source file for X rotation functions.
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	GMM	07/10/95	Original file.
 *      GMM     07/13/95        Fixed memory leak where XFreeFont() was not
 *				being called after call to XQueryFont().
 *				This fixed by replacing	XQueryFont() and 
 *				XTextWidth() with XQueryTextExtents(). Also
 *				performed some code optimization.
 *
 *
 * ---FHDR--- */



#include <X11/Xlib.h>
#ifdef _HPUX_SOURCE
#  include <Xm/Xm.h>
#endif 

#include "x_rotate.h"


/* +++MANHDR+++
 *
 * 
 * NAME
 * 	XgDrawRotatedText - Draw rotated and aligned text
 * 
 * 
 * SYNOPSIS
 *	int XgDrawRotatedText(dpy, drawable, gc, x, y, alignment, 
 * 		degrees, text, len)
 *                   Display *dpy;
 *                   Drawable drawable;
 *                   GC gc;
 *                   int x;
 *                   int y;
 *                   int alignment;
 *                   int degrees;
 *                   char *text;
 *                   int len;
 * 
 * 
 * ARGUMENTS
 *	dpy	    Specifies the connection to the X server.
 *
 *	drawable    Specifies the drawable.
 *
 *      gc	    Specifies the GC.
 *
 *	x	     
 *	y	    Specify the x and y coordinates, which are rela-
 *                  tive to the origin of the specified drawable.
 *
 *	alignment   Specifies the allignment for the text. Possible
 *		    values are XgALIGN_CENTER, XgALIGN_LEFT, or
 *		    XgALIGN_RIGHT.
 *
 *	degrees	    Specifies the degrees of rotation for drawing the
 *		    text. Possible values are 0, 90, 180, or 270.
 *
 *      text	    Specifies the character string to draw.
 *
 *	len	    Specifies the number of characters in the text 
 *		    argument. A -1 value will cause XgDrawRotatedText
 *		    to calculate the length of the string.
 *
 * DESCRIPTION
 *	XgDrawRotatedText draws a rotated and aligned string. 
 *	It draws the characters in the foreground and fills the 
 *	bounding box with the background.
 * 
 * 
 * RETURN VALUE
 *	On success, zero is returned.  On error, -1 is returned.
 * 
 * 
 * ---MANHDR--- */ 

#ifndef _NO_PROTO
int 
XgDrawRotatedText(Display *dpy, Drawable drawable, GC gc, int x, int y,
    int alignment, int degrees, char *text, int len)
#else
int 
XgDrawRotatedText(dpy, drawable, gc, x, y, alignment, degrees, text, len)
    Display  *dpy;
    Drawable drawable;
    GC	 gc;
    int  x;
    int  y;
    int  alignment;
    int	 degrees;
    char *text;
    int	 len;
#endif
{
XImage 		*text_image;
Pixmap 		bitmap;
int 	        direction, font_height, font_descent, font_ascent;
unsigned long	black_pixel;
int 		text_len;
int 		w, h, width, height;
int		image_x, image_y;
XGCValues	values;
char		*image_data;
XCharStruct     overall;
static GC       font_gc = None;


	/*
	 * -1 as the length means we compute the string length
	 */
	if ( len == -1 )
		len = strlen(text);


	/*
	 * Get the font ID from the passed GC
	 */
	if ( XGetGCValues(dpy, gc, GCFont, &values) == 0 )
		return -1;

	/*
	 * Get the extents info for the text
	 */
	XQueryTextExtents(dpy, values.font, text, len, 
	    &direction, &font_ascent, &font_descent, &overall);
	font_height = font_ascent + font_descent;
	text_len = overall.width;


	/*
	 * If rotation is 0 degrees then draw it normally, but
	 * adjust for alignment
	 */
	if ( degrees == 0 )
	{
    		/* 
     		 * Adjust for alignment
     		 */
    		if ( alignment == XgALIGN_CENTER )        
        		x -= text_len / 2; 
    		else if ( alignment == XgALIGN_LEFT )  
   			x = x; 
    		else if ( alignment == XgALIGN_RIGHT )  
        		x -= text_len; 
     
    		/*
     		 * Draw the text
     		 */
    		XDrawImageString(dpy, drawable, gc, x, y, text, len);

		return 0;
	}

 	/*
	 * Create a 1 plane pixmap to render the normal text in
	 */
 	w = text_len;
 	h = font_height;
 	bitmap = XCreatePixmap(dpy, DefaultRootWindow(dpy), w, h, 1);
 	if ( bitmap == None ) 
		return -1;

	/*
	 * Create a 1 plane GC for drawing the text(only once)
	 */
	if ( font_gc == None )
	{
	XGCValues       font_values;

  		/* 
		 * Create GC against this 1 plane pixmap, we set the
		 * foreground to 1 and the background to 0 because that's
		 * what an XImage expects
		 */
		font_values.background = 0;
		font_values.foreground = 1;
		font_values.font = values.font;
  		font_gc = XCreateGC(dpy, bitmap, 
			GCFont | GCForeground | GCBackground, &font_values);

		if ( font_gc == None )
		{
			XFreePixmap(dpy, bitmap);
		    	return -1;
		}
	}
	else
	{
	  	XSetFont(dpy, font_gc, values.font);
	}

	
 	/* 
	 * Draw the text into the 1 plane pixmap we just created
	 */
 	XDrawImageString(dpy, bitmap, font_gc, 0, font_ascent, text, len);

	/*
	 * Round the width and height to 8 bit boundries, the rotation
	 * algorithim requires this
	 */
	width  = ((w + 7) / 8) * 8;
	height = ((h + 7) / 8) * 8;
	
	/*
	 * Create an XImage to hold the text for rotation, use calloc
	 * becuase we need the bitmap zeroed out (cleared)
	 */
	image_data = (char *)calloc((width * height) / 8, 1);

	text_image = XCreateImage(dpy, DefaultVisual(dpy, 0), 1, 
		XYBitmap, 0, image_data, width, height, 8, 0);


	/*
	 * Now get the text out of the pixmap and put it in the XImage
	 */
	text_image->bitmap_bit_order = MSBFirst;
	text_image->byte_order = MSBFirst;
	text_image->xoffset = 0;
	XGetSubImage(dpy, bitmap, 0, 0, 
		w, h, AllPlanes, XYPixmap, text_image, 0, 0);

	/*
	 * We don't need the pixmap (bitmap) anymore, free it
	 */
	XFreePixmap(dpy, bitmap);


	/*
	 * Now rotate the XImage (that is the text, boys)
	 */
	XgRotateXImage(text_image, degrees);


	/*
	 * Did we do a 90 or 270 degree rotation? Yea then swap width/height
	 * We also adjust things for the 8 bit boundry that we rounded to
	 */
	if ( degrees == 90 || degrees == 270 )
	{
	int	tmp_w;

		/*
 		 * Swap width and height
		 */
		tmp_w = width;
		width  = height;
		height = tmp_w;
		tmp_w = w;
		w = h;
		h = tmp_w;
		text_image->width = width;
		text_image->height = height;

		text_image->bytes_per_line = width / 8;

		/*
		 * Adjust for alignment
		 */
		if ( degrees == 90 )
		{
			image_y = 0;	
			if ( alignment == XgALIGN_CENTER )
				y -= text_len / 2;
			else if ( alignment == XgALIGN_RIGHT )
				y -= text_len;
		}
		else
		{
			image_y = height - h;
			if ( alignment == XgALIGN_LEFT )
				y -= text_len;
			else if ( alignment == XgALIGN_CENTER )
				y -= text_len / 2;
		}
	}
	else
	{
		image_y = height - h;

 		/*
	 	 * Adjust for alignment
	 	 */
    		if ( alignment == XgALIGN_CENTER )        
        		x -= text_len / 2; 
    		else if ( alignment == XgALIGN_RIGHT )  
        		x -= text_len; 
	}


 	/* 
	 * Now image the text by using XPutImage with the
	 * image we rotated on the passed drawable
	 */
	text_image->format = XYBitmap;
	image_x = width - w;

	XPutImage(dpy, drawable, gc, text_image, 
		image_x, image_y, x, y, w, h);


	/*
	 * Don't need the XImage anymore, destroy it
	 */
	XDestroyImage(text_image);

	return 0;
}



/*
 * This table is used with the 180 degree rotation stuff
 */
static unsigned char reverse_bits[] = {
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
	 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};


/************************************************************************
 *
 *   Function Name:  XgRotateXImage()
 *
 *   Description:
 *   Given a pointer to an XImage structure, rotate it the mentioned
 *   number of degrees clockwise.
 *
 *   This version assumes MSB-first data all around.
 *
 ************************************************************************/

int
XgRotateXImage(image, degrees)
XImage *image;
int degrees;
{
register unsigned char 	*bitmap;
int 			width, height;
register unsigned char 	*newbufp;
unsigned char 		*bitmap_save;
int			nbytes, row_inc, saved_nbyterows;

	/*
	 * We only do 90, 180, and 270 degrees here
	 */
	if (  degrees != 90 && degrees != 270 && degrees != 180 )
		return -1;

	/*
	 * We do not have the capability of rotating bitmaps whose dimensions
	 * are not even multiples of 8 (one byte).  If either of these two
	 * parameters are not a muliple of 8 return with error.
	 */
	width = (int)image->width;
	height = (int)image->height;
	if ( width % 8 || height % 8 )
	{
		return -1;
	}


	/*
	 * If rotation is 180 then we can do it in place
	 */
	if ( degrees == 180 )
	{
	register unsigned char *p, *q;

		width = width / 8;

		bitmap = (unsigned char *)image->data;

		for ( p = bitmap, q = (bitmap + height * width) - 1; p <= q; )
		{
			register unsigned char c = *p;

			*p++ = reverse_bits[*q];
			*q-- = reverse_bits[c];
		}

		return 0;
	}
 


	/*
	 * Set everything up
	 */
	bitmap = (unsigned char *)image->data;
	bitmap_save = bitmap;

	nbytes  = width * height / 8;
	row_inc = (height / 8);
	saved_nbyterows = (width / 8);


	/*
	 *  Try and allocate a buffer to rotate to, if not successful then
	 *  get outa here with an error code
	 */
	if ( newbufp = (unsigned char *) malloc(nbytes), newbufp == NULL )
		return -1;

	memset(newbufp, 0, nbytes);


	if ( degrees == 90 )
	{
	    /*
   	     * This outer loop runs once for each horizontal line of bits 
	     * in the original image. The input goes left->right, 
	     * top->bottom, and in the output image it fills top->bottom, 
	     * right->left.
	     */
	    while ( height-- > 0 )
	    {
	    int			nbyterows = saved_nbyterows;
	    register unsigned char	*outp     = newbufp + (height / 8);
	    unsigned int		mask      = 1 << (7 - (height % 8));

		while (nbyterows-- > 0)
		{
		register unsigned int	c = *bitmap++;

			/*
			 *  As a special case, we don't have to do anything
			 *  if the byte to be set is zero.
			 */		
  			if ( !c ) 
			{
				outp += (row_inc * 8);
				continue;
			}
	
			if (c & 0x80) *outp |= mask; outp += row_inc;
			if (c & 0x40) *outp |= mask; outp += row_inc;
			if (c & 0x20) *outp |= mask; outp += row_inc;
			if (c & 0x10) *outp |= mask; outp += row_inc;
			if (c & 0x08) *outp |= mask; outp += row_inc;
			if (c & 0x04) *outp |= mask; outp += row_inc;
			if (c & 0x02) *outp |= mask; outp += row_inc;
			if (c & 0x01) *outp |= mask; outp += row_inc;
		}
	    }
	}
	else
	{
	    /*
    	     * This outer loop runs once for each horizontal line of bits in 
	     * the original image. The input goes left->right, top->bottom, 
	     * and in the output image it fills bottom->top, left->right.
	     */
	    height += 7;
	    while ( height-- > 7 )
	    {
	    int	nbyterows = saved_nbyterows;
	    register unsigned char  *outp = newbufp + nbytes - (height / 8);
	    unsigned int	    mask  = 1 << (height % 8);

		while (nbyterows-- > 0)
		{
		register unsigned int	c = *bitmap++;

			/*
			 *  As a special case, we don't have to do anything
			 *  if the byte to be set is zero.
			 */		
  			if ( !c ) 
			{
				outp -= (row_inc * 8);
				continue;
			}
	
			if (c & 0x80) *outp |= mask; outp -= row_inc;
			if (c & 0x40) *outp |= mask; outp -= row_inc;
			if (c & 0x20) *outp |= mask; outp -= row_inc;
			if (c & 0x10) *outp |= mask; outp -= row_inc;
			if (c & 0x08) *outp |= mask; outp -= row_inc;
			if (c & 0x04) *outp |= mask; outp -= row_inc;
			if (c & 0x02) *outp |= mask; outp -= row_inc;
			if (c & 0x01) *outp |= mask; outp -= row_inc;
		}
	    }
	}

	/*
	 * Now we have copied the entire bitmap in to the newbufp, so copy
	 * the modified version back into the user's buffer.  Then we can
	 * free our memory area.
	 */
	memcpy(bitmap_save, newbufp, nbytes);

	free(newbufp);

	return 0;
}

