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
 * FILE NAME:	XbrCursor.c
 * CREATED:	Thu Oct 28 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION: Cursor handling routines. This is not very good at all and
 *              I'm sure there is a better way of doing this, but as the FAQ
 *              does not suggest many easier ways, here goes...
 *
 */

#include <Xm/Xm.h>

#include <X11/cursorfont.h>

#include "XbrCursorError.h"
#include "XbrGfx.h"

#define	MAXSTACKSIZE	50

#define	XbrCURSOR_RESET		0
#define	XbrCURSOR_GRAB		1
#define XbrCURSOR_WATCH		2

static int busy = 0;

typedef struct cursor {
    Widget w;
    struct cursor *prev; 
    struct cursor *next; 
    int grab_focus;
} CURSOR;

static CURSOR *first_cursor = NULL;
static CURSOR *last_cursor = NULL;

static Cursor grab_cursor = (Cursor) NULL;
static Cursor watch_cursor = (Cursor) NULL;

void XbrCursorAdd(Widget, int);
void XbrCursorDelete(Widget);
void XbrCursorAddCB(Widget, XtPointer, XtPointer);
void XbrCursorDeleteCB(Widget, XtPointer, XtPointer);
void XbrCursorWatch(Widget, int);
static void XbrCursorsCreate(Widget);
static Cursor XbrCursorCreate(Display *, Screen *, char *, int, int, int, int);
static void XbrCursorChange(Widget, CURSOR *, int);
static CURSOR *XbrCursorAlloc(Widget, int);
static CURSOR *XbrCursorUnlink(Widget);
static CURSOR *XbrCursorLink(Widget, int);
static CURSOR *XbrCursorFind(Widget);

/* XbrCursorCreate:***********************************************************/
/* XbrCursorCreate: Convert a bitmap into a cursor.                          */
/* XbrCursorCreate:***********************************************************/

static Cursor XbrCursorCreate(Display *display, Screen *screen, char *bits,
                              int xhot, int yhot, int width, int height)
{
    Cursor cursor;
    XColor colours[2];
    Pixmap pixmap;
    Colormap colourmap = DefaultColormapOfScreen(screen);
    Window root = RootWindowOfScreen(screen);

    /* Find out the black & white pixel values */
    XbrGfxBWPixels(display, screen, colourmap, &colours[0], &colours[1]);

    /* Convert the bitmap to a pixmap - we could use XbrReadBitmap but
       that requires a GC, so we use the normal X routine.
    */
    pixmap = XCreatePixmapFromBitmapData(display, root, bits, width, height,
               colours[1].pixel, colours[0].pixel, 1);

    /* Create a cursor from the pixmap */
    cursor = XCreatePixmapCursor(display, pixmap, pixmap, &colours[0],
             &colours[1], xhot, yhot);

    /* Free the pixmap now */
    XFreePixmap(display, pixmap);

    return(cursor);
}

/* XbrCursorsCreate:**********************************************************/
/* XbrCursorsCreate: Create the watch and grab cursors.                      */
/* XbrCursorsCreate:**********************************************************/

static void XbrCursorsCreate(Widget w)
{
    Display *display = XtDisplay(w);
    Screen *screen = XtScreen(w);

    if(grab_cursor == (Cursor)NULL) {
        grab_cursor = XbrCursorCreate(display, screen, (char *)err_bits,
          err_xhot, err_yhot, err_width, err_height);
    }
    if(watch_cursor == (Cursor)NULL) {
        watch_cursor = XCreateFontCursor(display, XC_watch);
    }
}

/* XbrCursorChange:***********************************************************/
/* XbrCursorChange: Change the cursor to the given type. The type is either  */
/* XbrCursorChange: no cursor, the error cursor or one of the cursor types   */
/* XbrCursorChange: from X11/cursorfont.h. The exclude widget is a widge not */
/* XbrCursorChange: to change the cursor on.                                 */
/* XbrCursorChange:***********************************************************/

static void XbrCursorChange(Widget w, CURSOR *start_cursor, int type)
{
    Display *display = XtDisplay(w);
    CURSOR *cptr;
    int found = 0;

    /* Create the cursors */
    XbrCursorsCreate(w);

    /* Change the cursor for each shell */
    for(cptr = start_cursor; cptr != NULL; cptr = cptr->prev) {
        XUndefineCursor(display, XtWindow(cptr->w));
        if(type == XbrCURSOR_RESET) {
            if(found)
                XDefineCursor(display, XtWindow(cptr->w), grab_cursor);
            else if(cptr->grab_focus)
                found = 1;
        }
        else if(type == XbrCURSOR_GRAB)
            XDefineCursor(display, XtWindow(cptr->w), grab_cursor);
        else if(type == XbrCURSOR_WATCH)
            XDefineCursor(display, XtWindow(cptr->w), watch_cursor);
    }
    XmUpdateDisplay(w);
}

/* XbrCursorWatch:************************************************************/
/* XbrCursorWatch: Set the watch cursor on/off.                              */
/* XbrCursorWatch:************************************************************/

void XbrCursorWatch(Widget w, int on)
{
    busy = on;

    if(on)
        XbrCursorChange(w, last_cursor, XbrCURSOR_WATCH);
    else
        XbrCursorChange(w, last_cursor, XbrCURSOR_RESET);
}

/* XbrCursorAdd:**************************************************************/
/* XbrCursorAdd: Add a cursor to the link list - external function.          */
/* XbrCursorAdd:**************************************************************/

void XbrCursorAdd(Widget w, int grab_focus)
{
    CURSOR *cursor;

    if(XbrCursorFind(w) == NULL) {
        cursor = XbrCursorLink(w, grab_focus);
        if(busy)
            XbrCursorChange(w, last_cursor, XbrCURSOR_WATCH);
        else if(grab_focus) {
            XUndefineCursor(XtDisplay(w), XtWindow(w));
            XbrCursorChange(w, cursor->prev, XbrCURSOR_GRAB);
        }
    }
}

/* XbrCursorDelete:***********************************************************/
/* XbrCursorDelete: Delete cursor from the list - external function.         */
/* XbrCursorDelete:***********************************************************/

void XbrCursorDelete(Widget w)
{
    CURSOR *cursor, *cptr;
    Display *display = XtDisplay(w);

    /* Create the cursors */
    XbrCursorsCreate(w);

    if((cursor = XbrCursorFind(w)) != NULL) {
        /* Find the next one with grab */
        for(cptr = cursor->prev; cptr != NULL; cptr = cptr->prev) {
            if(cptr->grab_focus) {
                XUndefineCursor(XtDisplay(cptr->w), XtWindow(cptr->w));
                XbrCursorChange(w, cptr->prev, XbrCURSOR_GRAB);
                break;
            }
            else {
                XUndefineCursor(display, XtWindow(cptr->w));
            }
        }
    }

    /* Take it out of the linked list */
    XbrCursorUnlink(w);

    /* If the application is busy make sure it stays busy */
    if(busy)
        XbrCursorChange(w, last_cursor, XbrCURSOR_WATCH);
}

/* XbrCursorAddCB:************************************************************/
/* XbrCursorAddCB: A wrap around callback for XbrCursorAdd().                */
/* XbrCursorAddCB:************************************************************/

void XbrCursorAddCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XbrCursorAdd(w, (int)client_data);
}

/* XbrCursorDeleteCB:*********************************************************/
/* XbrCursorDeleteCB: Just the XbrDelete call implemented as a callback.     */
/* XbrCursorDeleteCB:*********************************************************/

void XbrCursorDeleteCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XbrCursorDelete(w);
}

/* XbrCursorAlloc:************************************************************/
/* XbrCursorAlloc: Allocate memory for a new CURSOR.                         */
/* XbrCursorAlloc:************************************************************/

static CURSOR *XbrCursorAlloc(Widget w, int grab_focus)
{
    CURSOR *cursor;

    /* Allocate memory for the struture */
    if((cursor = (CURSOR *) XtMalloc(sizeof(CURSOR))) == NULL)
        return(NULL);

    /* Initiliase data */
    cursor->w = w;
    cursor->prev = NULL;
    cursor->next = NULL; 
    cursor->grab_focus = grab_focus;

    /* OK. */
    return(cursor);
}

/* XbrCursorLink:*************************************************************/
/* XbrCursorLink: Add cursor to the linked list.                             */
/* XbrCursorLink:*************************************************************/

static CURSOR *XbrCursorLink(Widget w, int grab_focus)
{
    CURSOR *cursor;

    /* Create CURSOR */
    cursor = XbrCursorAlloc(w, grab_focus);

    /* Add it to the list */
    if(!first_cursor) {
        first_cursor = cursor;
        last_cursor  = cursor;
    }
    else {
        cursor->prev = last_cursor;
        last_cursor->next = cursor;
        last_cursor = cursor;
    }

    return(cursor);
}

/* XbrCursorUnlink:***********************************************************/
/* XbrCursorUnlink: Unlink cursor from the linked list.                      */
/* XbrCursorUnlink:***********************************************************/

static CURSOR *XbrCursorUnlink(Widget w)
{
    CURSOR *cursor;

    /* Trying to delete first cursor in list? */
    if(first_cursor->w == w) {
        cursor = first_cursor;
        first_cursor = cursor->next;
        first_cursor->prev = NULL;
        return(cursor);
    }

    /* Trying to delete last widget in list? */
    if(last_cursor->w == w) {
        cursor = last_cursor;
        last_cursor = cursor->prev;
        last_cursor->next = NULL;
        return(cursor);
    }

    /* Must be in the middle */
    for(cursor = first_cursor; cursor != NULL; cursor = cursor->next) {
        if(cursor->w == w) {
            cursor->prev->next = cursor->next;
            cursor->next->prev = cursor->prev;
            return(cursor);
        }
    }
}

/* XbrCursorFind:*************************************************************/
/* XbrCursorFind: Search the linked list for the widget.                     */
/* XbrCursorFind:*************************************************************/

static CURSOR *XbrCursorFind(Widget w)
{
    CURSOR *cursor;

    for(cursor = first_cursor; cursor != NULL; cursor = cursor->next) {
        if(cursor->w == w)
            return(cursor);
    }

    return(NULL);
}
