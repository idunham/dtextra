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

/* +++FHDR+++
 *
 *	Filename: grabarea.c
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	-------	---------------	---------------------------------------------
 *	messner	06/13/95	Original file
 *
 *
 * ---FHDR--- */


#include	<X11/Xos.h>
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/keysym.h>
#include	<X11/cursorfont.h>
#include	<X11/Xatom.h>

#ifndef CARD32
#include	<X11/Xmd.h>
#endif



/* +++PHDR+++
 *
 * Function:	XgGrabArea()
 *
 * Scope:	extern
 *
 * Description:	This function lets the user rubber-band a
 *		rectangle over the entire the screen and return
 *		its values in an XRectangle structure.  The
 *		server should be grabbed before calling this
 *              function because if the screen changes during
 *		XOR drawing leftover rectangles may be left on
 *		the screen.
 *		
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * display      Display *
 * confine_to   Window
 *
 *
 * Returns:	An XRectangle pointer containing the info for the grabbed area.
 *
 *
 * Notes:
 *
 * ---PHDR--- */

XRectangle *
XgGrabArea(display, confine_to, convert_to)
Display *display;
Window confine_to;
Window convert_to;
{
XEvent 		event;
unsigned int 	mask;
int 		box_drawn = False;
int		rootX, rootY, x, y;
int 		rX, rY, rW, rH;
int 		convert_x, convert_y;
int 		screen, status;
Window 		child, root;
XRectangle 	* rect;
Cursor 		ulCursor, lrCursor;
GC		gc;
XComposeStatus	compose;
KeySym		keysym;
char		key_buffer[20];
int		key_buflen = sizeof(key_buffer);


	/*
	 * Make sure the display pointer is not NULL, get outa here if it is
	 */
	if ( display == NULL )
		return NULL;

	/*
	 * Get the root window and screen of the display
	 */
	root = DefaultRootWindow(display);
	screen = DefaultScreen(display);

	if ( confine_to == None )
		confine_to = root;

	if ( convert_to == None )
		convert_to = confine_to;

  
	/*
	 *  Get the cursors
	 */
	ulCursor = XCreateFontCursor(display, XC_ul_angle);
	if ( ulCursor == None )
		return NULL;

   	lrCursor = XCreateFontCursor(display, XC_bottom_right_corner);
	if ( ulCursor == None )
	{
		XFreeCursor(display, ulCursor);
		return NULL;
	}

	/* 
	 * Create a graphics context to draw the rubber bands with
	 */
	gc = XCreateGC(display, convert_to, 0, NULL);
	if ( gc == None )
	{
		XFreeCursor(display, lrCursor);
		XFreeCursor(display, ulCursor);
		return NULL;
	}

	XSetSubwindowMode(display, gc, IncludeInferiors);
	XSetForeground(display, gc, 255);
	XSetFunction(display, gc, GXxor);

	/*
	 * Prior to grabing the screen flush the display
	 */
	XFlush(display);


	/*
	 * First thing we need to do is grab the pointer displaying the upper
	 * left hand cursor, get outa here if unsuccseeful
	 */
	status = XGrabPointer(display, confine_to, False, ButtonPressMask,
	 	GrabModeAsync, GrabModeAsync, confine_to, 
		ulCursor, CurrentTime);
	if ( status != GrabSuccess )
	{
		XFreeGC(display, gc);
		XFreeCursor(display, lrCursor);
		XFreeCursor(display, ulCursor);
		return NULL;
	}
  
	/* 
	 * Wait for a button-press event, compute the root location
	 * from the event members
	 */
	while ( True )
	{
		XMaskEvent(display, KeyPressMask | ButtonPressMask, &event);
		
		if ( event.type != KeyPress )
			break;

		XLookupString((XKeyEvent *)&event, 
			key_buffer, key_buflen, 
			&keysym, &compose);

		if ( keysym == XK_Escape )
		{
			/*
			 * Ungrab the pointer
			 */
			XUngrabPointer(display, CurrentTime);

			XFreeGC(display, gc);
			XFreeCursor(display, lrCursor);
			XFreeCursor(display, ulCursor);

			return NULL;
		}
	}


	rX = event.xbutton.x;
	rY = event.xbutton.y;
	if ( XTranslateCoordinates(display, confine_to, convert_to,
		rX, rY, &convert_x, &convert_y, &child) )
	{
		rX = convert_x;
		rY = convert_y;
	}
	rootX = rX;
	rootY = rY;


	/* 
	 * Change the grabbed pointer cursor to the lower right cursor
	 */
	XChangeActivePointerGrab(display, 
		ButtonMotionMask | ButtonReleaseMask,
		lrCursor, CurrentTime);


	/* 
	 * Define the macro CONVERT_RECT to convert the original root 
	 * coordinates and the event root oordinates into a rectangle
	 * in the rect structure
	 */
#define CONVERT_RECT(etype) 		\
	x = (int)event.etype.x;       	\
	y = (int)event.etype.y;      	\
	if ( convert_to != confine_to ) \
	{                               \
	    if ( XTranslateCoordinates(display,	  \
		confine_to, convert_to, x, y,     \
		&convert_x, &convert_y, &child) ) \
	    {                           \
		x = convert_x;          \
		y = convert_y;          \
	    }                           \
	}                               \
	rW  = x - rootX;             	\
	if ( rW  < 0 ) rW = -rW;	\
	rH  = y - rootY;              	\
	if ( rH  < 0 ) rH = -rH;      	\
	rX = x < rootX ? x : rootX;   	\
	rY = y < rootY ? y : rootY

	/* 
	 * Loop and echo the rubber band rectangle, exit loop when the
	 * button is released
	 */
	while ( True ) 
	{
		/*
		 * Wait for the next event
		 */
		XNextEvent(display, &event);

		switch(event.type) 
		{
		  case KeyPress:
		  	
			XLookupString((XKeyEvent *)&event, 
				key_buffer, key_buflen, 
				&keysym, &compose);

			if ( keysym != XK_Escape )
				continue;

		  	
		  	if ( box_drawn )
				XDrawRectangle(display, 
					convert_to, gc, rX, rY, rW, rH);

			/*
			 * Ungrab the pointer
			 */
			XUngrabPointer(display, CurrentTime);


			/*
			 * Free all the allocated resources
			 */
			XFreeGC(display, gc);
			XFreeCursor(display, lrCursor);
			XFreeCursor(display, ulCursor);

			return NULL;


		  case ButtonRelease:

		  	if ( box_drawn )
				XDrawRectangle(display, 
					convert_to, gc, rX, rY, rW, rH);

			XFlush(display);

			/* 
			 *  Save the rectangle information
			 */
			CONVERT_RECT(xbutton);
			rect = (XRectangle *)malloc(sizeof(XRectangle));
			rect->x      = rX;
			rect->y      = rY;
			rect->width  = rW;
			rect->height = rH;


			/*
			 * Ungrab the pointer
			 */
			XUngrabPointer(display, CurrentTime);

			/*
			 * Free all the allocated resources
			 */
			XFreeGC(display, gc);
			XFreeCursor(display, lrCursor);
			XFreeCursor(display, ulCursor);

			/*
			 * return the pointer to the static rectangle
			 */
			return ( rect );


		  case MotionNotify:

		  	if ( box_drawn )
			{
		  		XDrawRectangle(display, 
					convert_to, gc, rX, rY, rW, rH);

				box_drawn = False;
		  	}

			/*
			 * Throw away incoming motion notifies 
			 * while we handle this one 
			 */
			while (XCheckTypedEvent(display, MotionNotify, &event));

			CONVERT_RECT(xmotion);
			XDrawRectangle(display, convert_to, gc, rX, rY, rW, rH);
			box_drawn = True;
			break;
		}
	}

	/* NOT REACHED */
}

