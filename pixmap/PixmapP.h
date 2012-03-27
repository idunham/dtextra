/*
 * $Id: PixmapP.h,v 1.12 1996/08/21 16:26:16 lmallet Exp $
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
 * $XConsortium: PixmapP.h,v 1.6 90/06/09 20:19:47 dmatic Exp $
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



#ifndef _PixmapP_h
#define _PixmapP_h

#include "Pixmap.h"
#include <X11/CoreP.h>

typedef struct {
    Atom           *targets;
    Cardinal        num_targets;
    PWRequestRec   *requests;
    Cardinal        num_requests;
    PWRequestRec   *request[100];
  
} PixmapClassPart;

/* Full class record declaration */
typedef struct _PixmapClassRec {
  CoreClassPart          core_class;
  PixmapClassPart        pixmap_class;
} PixmapClassRec;

extern PixmapClassRec pixmapClassRec;

/**********/
struct _PWRequestRec {
  char       *name;
  int         status_size;
  void      (*engage)();
  XtPointer     engage_client_data;
  void      (*terminate)();
  XtPointer     terminate_client_data;
  void      (*remove)();
  XtPointer     remove_client_data;
};

typedef struct {
  Position from_x, from_y,
           to_x, to_y;
} PWArea;

typedef struct {
    PWRequestRec *request;
    XtPointer       status;
    Boolean       trap;
    XtPointer       call_data;
} PWRequestStack;

typedef struct {
    XImage   *image, *mask_image, *buffer, *mask_buffer;
    XPoint    hot;
    Position  at_x, at_y;
    Boolean   fold;
    Boolean   grid;
    Boolean   changed;
} PWZoom;

typedef struct {
    Boolean   own;
    Boolean   limbo;
} PWSelection;


/* New fields for the Pixmap widget record */
typedef struct {

  /* resources */

  Cursor           cursor;
  Pixel            foreground_pixel;
  Pixel            highlight_pixel;
  Pixel            framing_pixel;
  Pixel            transparent_pixel;
  Pixmap           stipple;
  Boolean          stippled;
  Boolean          proportional;
  Boolean          grid;
  Dimension        grid_tolerance;
  Boolean          axes;
  Boolean          resize;
  Dimension        distance, squareW, squareH, width, height;
  int              button_action[5];
  String           filename;
  AddColorNotifyProc AddColorNotify;

  /* private state */

  int             nextensions, buffer_nextensions;
  XpmExtension    *extensions, *buffer_extensions;
  PWDrawPointProc drawPointCallback; 
  PWRedrawProc    redrawCallback; 
  PWTranslateProc translateCallback; 
  PWRotateProc    rotateCallback; 
  PWFlipProc      flipCallback; 

  XFontStruct     *font_struct;
  String          text_string; 
  int             text_lbearing, text_rbearing;
  int             text_ascent, text_descent; 

  XPoint           hot;
  XPoint           buffer_hot;
  XPoint           storage_hot;
  Pixel            clear_pixel;
  Position         horizOffset, vertOffset;
  void           (*notify)();
  UseColorNotifyProc   colorNotify;
  ExtensionNotifyProc  extensionNotify;
  PickPixelProc    pickPixelDraw;
  PickPixelProc    pickPixelComplete;
  PWRequestStack  *request_stack;
  Cardinal         cardinal, current;
  /*Boolean          trapping;*/
  XImage          *image, *mask_image, *buffer, *mask_buffer;
  XImage          *storage, *mask_storage;
  PWArea           mark, buffer_mark;
  GC               drawing_gc;
  GC               highlighting_gc;
  GC               framing_gc;
  GC               transparent_gc;
  Boolean          changed;
  Boolean          fold;
  Boolean          zooming;
  PWZoom           zoom;
  XtPointer         *value;
  char             *status;
  PWSelection      selection;
  PWColorInfo    **colorTable;
  char            *hints_cmt;
  char            *colors_cmt;
  char            *pixels_cmt;
} PixmapPart;

/* Full instance record declaration */
typedef struct _PixmapRec {
  CorePart      core;
  PixmapPart    pixmap;
} PixmapRec;

/* Private functions */

#define InPixmapX(PW, x)\
	(Position) (min((Position)((max(PW->pixmap.horizOffset, x)  -\
				   PW->pixmap.horizOffset) /\
				   PW->pixmap.squareW), PW->pixmap.width - 1))
    
#define InPixmapY(PW, y)\
	(Position) (min((Position)((max(PW->pixmap.vertOffset, y)  -\
				   PW->pixmap.vertOffset) /\
				   PW->pixmap.squareH), PW->pixmap.height - 1))
    
#define InWindowX(PW, x)\
	(Position) (PW->pixmap.horizOffset + ((x) * PW->pixmap.squareW))

#define InWindowY(PW, y)\
	(Position) (PW->pixmap.vertOffset + ((y) * PW->pixmap.squareH))
     
#define QuerySet(x, y) (((x) != NotSet) && ((y) != NotSet))

#define QueryZero(x, y) (((x) == 0) || ((y) == 0))

#define Swap(x, y) {Position t; t = x; x = y; y = t;}

#define QuerySwap(x, y) if(x > y) Swap(x, y)

#define QueryInPixmap(PW, x, y)\
  (((x) >= 0) && ((x) < PW->pixmap.image->width) &&\
   ((y) >= 0) && ((y) < PW->pixmap.image->height))

#define Value(PW, button)   (PW->pixmap.button_action[button - 1])

XImage *GetImage();
Pixmap GetPixmap();
XImage *CreatePixmapImage();
XImage *CreateMaskImage();
void DestroyPixmapImage();
#define DestroyMaskImage(mask) DestroyPixmapImage(mask)

#endif /* _PixmapP_h */
