/* * Last edited: Dec 19 10:46 1991 (mallet) */
/*
 * $Id: Graphics.c,v 1.8.2.1 1994/01/25 15:53:26 mallet Exp $
 * 
 * Copyright 1991 Lionel Mallet
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appears in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Lionel MALLET not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Lionel MALLET makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * Lionel MALLET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL Lionel MALLET BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION 
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *  This software is opened and free. Furthermore, everybody is kindly
 * invited to participate to improve it for the benefit of all.
 * Improvements can be new features, bugs fixes and porting issues
 * resolution.
 *
 * Author:  Lionel Mallet, SIMULOG
 */

/*
 * $XConsortium: Graphics.c,v 1.1 90/06/09 20:20:29 dmatic Exp $
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



#include <X11/IntrinsicP.h>
#include <X11/Xmu/Converters.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include "PixmapP.h"

#include <stdio.h>
#include <math.h>

#define XtStrlen(s)                   ((s) ? strlen(s) : 0)
#define abs(x)                        (((x) > 0) ? (x) : -(x))
#define min(x, y)                     (((x) < (y)) ? (x) : (y))
#define max(x, y)                     (((x) > (y)) ? (x) : (y))
#define roundint(x)                   floor((x) + 0.5)


/**************************************************************************
 *
 * Utility functions
 *
 **************************************************************************/

typedef enum { Lazy, NotLazy} DrawingMode;

#define WithinMark(PW, xi, yi) \
  ((xi <= max(PW->pixmap.mark.from_x, PW->pixmap.mark.to_x)) &&\
   (xi >= min(PW->pixmap.mark.from_x, PW->pixmap.mark.to_x)) &&\
   (yi <= max(PW->pixmap.mark.from_y, PW->pixmap.mark.to_y)) &&\
   (yi >= min(PW->pixmap.mark.from_y, PW->pixmap.mark.to_y)))

#define HotInMark(PW) WithinMark(PW, PW->pixmap.hot.x, PW->pixmap.hot.y)

#define IsHotSpot(PW, xi, yi) \
  ((xi== PW->pixmap.hot.x) && (yi == PW->pixmap.hot.y))

Pixel GetPxlFromImageAndMask(image, mask_image, x, y)
     XImage *image, *mask_image;
     Position x, y;
{
    Pixel pixel;

    pixel = XGetPixel(image, x, y);
    if (XGetPixel(mask_image, x, y) == 0) return TRANSPARENT(dpy, screen);
    else return pixel;
}

    
Pixel GetPxl(pw, x, y)
     PixmapWidget pw;
     Position x, y;
{
    Pixel pixel;
    
    pixel = XGetPixel(pw->pixmap.image, x, y);
    if ((pixel == pw->pixmap.transparent_pixel) &&
	(XGetPixel(pw->pixmap.mask_image, x, y) == 0))
      return TRANSPARENT(dpy, screen);
    else return pixel;
}

void SetPxl(pw, x, y, color)
     PixmapWidget pw;
     Position x, y;
     Pixel color;
{
    if (color == TRANSPARENT(dpy, screen))
    {
	XPutPixel(pw->pixmap.image, x, y, pw->pixmap.transparent_pixel);
	XPutPixel(pw->pixmap.mask_image, x, y, 0);
    }
    else 
    {
	XPutPixel(pw->pixmap.image, x, y, color);
	XPutPixel(pw->pixmap.mask_image, x, y, 1);
    }
}
	  

#define ClearPxl(pw, x, y, white)\
  SetPxl(pw, x, y, white)


#if NeedFunctionPrototypes
Pixel PWGetPxl(Widget w, Position x, Position y)
#else
Pixel PWGetPxl(w, x, y)
     Widget  w;
     Position      x, y;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  if (QueryInPixmap(PW, x, y))
    return GetPxl(PW, x, y);
  else
    return NotSet;
}


XPoint *HotSpotShape(PW, x ,y)
    PixmapWidget PW;
    Position     x, y;
{
    static XPoint points[5];
  
    points[0].x = InWindowX(PW, x);
    points[0].y = InWindowY(PW, y + 1.0/2);
    points[1].x = InWindowX(PW, x + 1.0/2);
    points[1].y = InWindowY(PW, y + 1);
    points[2].x = InWindowX(PW, x + 1);
    points[2].y = InWindowY(PW, y + 1.0/2);
    points[3].x = InWindowX(PW, x + 1.0/2);
    points[3].y = InWindowY(PW, y);
    points[4].x = InWindowX(PW, x);
    points[4].y = InWindowY(PW, y + 1.0/2);
    
    return points;
}

#define DrawHotSpot(PW, x, y)\
  XFillPolygon(XtDisplay(PW), XtWindow(PW), PW->pixmap.framing_gc,\
	       HotSpotShape(PW, x, y), 5, Convex, CoordModeOrigin)

#define HighlightHotSpot(PW, x, y)\
  XFillPolygon(XtDisplay(PW), XtWindow(PW), PW->pixmap.highlighting_gc,\
	       HotSpotShape(PW, x, y), 5, Convex, CoordModeOrigin)


void HighlightSquare(PW, x, y)
     PixmapWidget PW;
     Position x, y;
{
  XFillRectangle(dpy, XtWindow(PW),
		 PW->pixmap.highlighting_gc,
		 InWindowX(PW, x), InWindowY(PW, y),
		 PW->pixmap.squareW, PW->pixmap.squareH);
}

void RedrawAxesInSquare();


void ClearSquare(PW, x, y)
     PixmapWidget PW;
     Position x, y;
{
  XSetForeground(dpy, PW->pixmap.drawing_gc, PW->pixmap.clear_pixel);
  XFillRectangle(dpy, XtWindow(PW),
		 PW->pixmap.drawing_gc,
		 InWindowX(PW, x), InWindowY(PW, y),
		 PW->pixmap.squareW, PW->pixmap.squareH);

}


void DrawSquare(PW, x, y, color)
     PixmapWidget PW;
     Position x, y;
     Pixel color;
{
  if (color == PW->pixmap.clear_pixel) ClearSquare(PW, x, y);
  else if (color == TRANSPARENT(dpy, screen))
    {
      XFillRectangle(dpy, XtWindow(PW),
		     PW->pixmap.transparent_gc,
		     InWindowX(PW, x), InWindowY(PW, y),
		     PW->pixmap.squareW, PW->pixmap.squareH);
    }
  else
    {
      XSetForeground(dpy, PW->pixmap.drawing_gc, color);
      XFillRectangle(dpy, XtWindow(PW),
		     PW->pixmap.drawing_gc,
		     InWindowX(PW, x), InWindowY(PW, y),
		     PW->pixmap.squareW, PW->pixmap.squareH);
      XSetForeground(dpy,PW->pixmap.drawing_gc,
		     PW->pixmap.foreground_pixel);
    }
}

#define DrawPoint(PW, x, y, color, mode)\
  if ((mode == NotLazy) || (GetPxl(PW, x, y) != color)) {\
    DrawSquare(PW, x, y, color); \
    SetPxl(PW, x, y, color); \
    /* now put back the status of the pixel */ \
    PWRedrawGrid((Widget )PW, x, y, x, y); \
    RedrawAxesInSquare(PW, x, y); \
    if (WithinMark(PW, x, y)) HighlightSquare(PW, x, y); \
    if (IsHotSpot(PW, x, y)) PWRedrawHotSpot((Widget)PW);\
    if (PW->pixmap.drawPointCallback) \
        PW->pixmap.drawPointCallback( (Widget) PW, x, y, Highlight );\
  }

#define ClearPoint(PW, x, y, mode) \
  if ((mode == NotLazy) || \
      (GetPxl(PW, x, y) != PW->pixmap.clear_pixel)) {\
    ClearSquare(PW, x, y);  \
    ClearPxl(PW, x, y, PW->pixmap.clear_pixel); \
    /* now put back the status of the pixel */ \
    PWRedrawGrid((Widget)PW, x, y, x, y); \
    RedrawAxesInSquare(PW, x, y); \
    if (WithinMark(PW, x, y)) HighlightSquare(PW, x, y); \
    if (IsHotSpot(PW, x, y)) PWRedrawHotSpot((Widget)PW);\
    if (PW->pixmap.drawPointCallback) \
        PW->pixmap.drawPointCallback( (Widget) PW, x, y, Highlight );\
  }


#define QueryFlood(PW, x, y, value)\
  (QueryInPixmap(PW, x, y)\
     && (GetPxl(PW, x, y) == value))

#define Flood(PW, x, y, value)\
  DrawPoint(PW, x, y, value, NotLazy)

void FloodLoop(PW, x, y, value, toflood)
     PixmapWidget PW;
     Position     x, y;
     Pixel          value, toflood;
{
  Position save_x, save_y, x_left, x_right;

  if (value == toflood) return; /* nothing to flood in this case */
  
  if (QueryFlood(PW, x, y, toflood)) 
    Flood(PW, x, y, value);
      
  save_x = x;
  save_y = y;
  
  x++;
  while (QueryFlood(PW, x, y, toflood)) {
    Flood(PW, x, y, value);
    x++;
  }
  x_right = --x;
  
  x = save_x;
  x--;
  while (QueryFlood(PW, x, y, toflood)) {
    Flood(PW, x, y, value);
    x--;
  }
  x_left = ++x;
  
  
  x = x_left;
  y = save_y;
  y++;
  
  while (x <= x_right) {
    Boolean flag = False;
    Position x_enter;
    
    while (QueryFlood(PW, x, y, toflood) && (x <= x_right)) {
      flag = True;
      x++;
    }
    
    if (flag) {
      if ((x == x_right) && QueryFlood(PW, x, y, toflood))
	FloodLoop(PW, x, y, value, toflood);
      else
	FloodLoop(PW, x - 1, y, value, toflood);
    }
    
    x_enter = x;
    
    while (!QueryFlood(PW, x, y, toflood) && (x < x_right))
      x++;
    
    if (x == x_enter) x++;
  }
  
  x = x_left;
  y = save_y;
  y--;
  
  while (x <= x_right) {
    Boolean flag = False;
    Position x_enter;
    
    while (QueryFlood(PW, x, y, toflood) && (x <= x_right)) {
      flag = True;
      x++;
    }
    
    if (flag) {
      if ((x == x_right) && QueryFlood(PW, x, y, toflood))
	FloodLoop(PW, x, y, value, toflood);
      else
	FloodLoop(PW, x - 1, y, value, toflood);
    }
    
    x_enter = x;
    
    while (!QueryFlood(PW, x, y, toflood) && (x < x_right))
      x++;
    
    if (x == x_enter) x++;
  }
}

void CopyImageData(source, destination, from_x, from_y, to_x, to_y, at_x, at_y)
     XImage *source, *destination;
     Position  from_x, from_y, to_x, to_y, at_x, at_y;
{
  register Position x, y, delta_x, delta_y;
  
  delta_x = to_x - from_x + 1;
  delta_y = to_y - from_y + 1;
  
  for (x = 0; x < delta_x; x++)
    for (y = 0; y < delta_y; y++)
      XPutPixel(destination, at_x + x, at_y + y, 
	     XGetPixel(source, from_x + x, from_y + y));
}


void TransferImageData(source, destination)
     XImage *source, *destination;
{
  register Position x, y;
  register Pixel color;
  
  for (x = 0; x < min(source->width, destination->width); x++)
    for (y = 0; y < min(source->height, destination->height); y++)
      if ((color = XGetPixel(source, x, y)) != XGetPixel(destination, x, y))
	XPutPixel(destination, x, y, color);
}

void DrawImageData(PW, image, mask_image, at_x, at_y, value, mode)
     PixmapWidget PW;
     XImage *image, *mask_image;
     Position at_x, at_y;
     int value;
     DrawingMode mode;
{
  Position x, y;
  Pixel      A, B;
  register Pixel Cl, Fg, val;
  
  Cl = PW->pixmap.clear_pixel;
  Fg = PW->pixmap.foreground_pixel;
  
  for (x = 0; x < image->width; x++) 
    for (y = 0; y < image->height; y++) {
      if (!QueryInPixmap(PW, at_x + x, at_y + y)) break;
      A = GetPxlFromImageAndMask(image, mask_image, x, y);
      B = GetPxl(PW, at_x + x, at_y + y);
      switch (value)
	{
	case Clear:
	  if ((mode == NotLazy) || (B != Cl)) val = Cl;
	  else val = NotSet;
	  break;
	case Set:
	  if ((mode == NotLazy) || (B != A)) val = A;
	  else val = NotSet;
	  break;
	case Invert:
	  if (((mode == NotLazy) && (A != Cl)) || ((A != Cl) && (B != Cl)))
	    val = Cl;
	  else if (((mode == NotLazy) && (A == Cl)) || 
		   ((A == Cl) && (B != Fg))) val = Fg;
	  else val = NotSet;
	  break;
	case Highlight:
	  if (A != B) PWDrawPoint((Widget)PW, at_x + x, at_y + y, A);
	  break;
	}
      if ((val != NotSet) && (val != Highlight))
	{
	  DrawPoint(PW, at_x + x, at_y + y, val, mode);
	}
      else 
	if (value == Highlight) HighlightSquare(PW, x, y);
	
    }
}

#define PRECISION 0.00001
#define OR(p1, p2) (p1 ? p1 : (p2 ? p2 : NULL))

/* this function checks the intersection of two lines
 * returns intersection if it exists, first point if two lines are equal,
 * null otherwise. Space in which result in returned should be free by the
 * caller
 */
XPoint *intersect(P1, Q1, P2, Q2)
     XPoint P1, Q1, P2, Q2;
     
{
  XPoint *result;
  int vert1 = 0, vert2 = 0, horiz1 = 0, horiz2 = 0;
  double alpha1, beta1, alpha2, beta2;
  double x, y;

  if (P1.x != Q1.x) /* not vertical */
    {
      alpha1 = (Q1.y*1. - P1.y*1.) / (Q1.x*1. - P1.x*1.);
      beta1 = P1.y*1. - alpha1 * P1.x*1.;
      if (Q1.y == P1.y) horiz1 = 1;
    }
  else /* vertical */
    {
      vert1 = 1;
      /* check line1 not point */
      if (P1.y == Q1.y) return(NULL);
      alpha1 = 0.;
      beta1 = P1.x;
    }

  if (P2.x != Q2.x) /* not vertical */
    {
      if (P2.y == Q2.y) horiz2 = 1;
      if (!vert1)
	{
	  alpha2 = (Q2.y*1. - P2.y*1.) / (Q2.x*1. - P2.x*1.);
	  beta2 = P2.y*1. - alpha2 * P2.x*1.;
	}
      else if (!horiz2) /* and vert1 implicitly */
	{
	  alpha2 = (Q2.x*1. - P2.x*1.) / (Q2.y*1. - P2.y*1.);
	  beta2 = P2.x*1. - alpha2 * P2.y*1.;
	}
      else /* vert1 and horiz2 */
	{
	  alpha2 = 0.;
	  beta2 = P2.y;
	}
    }
  else /* vertical */
    {
      vert2 = 1;
      /* check line2 not point */
      if (P2.y == Q2.y) return(NULL);
      alpha2 = 0.;
      beta2 = P2.x;
      if ((!vert1) && (!horiz1))
	{
	  alpha1 = (Q1.x*1. - P1.x*1.) / (Q1.y*1. - P1.y*1.);
	  beta1 = P1.x*1. - alpha1 * P1.y*1.;
	}	  
    }
  
#ifdef DEBUG
  printf("D1 vert %d horiz %d, D2 vert %d horiz %d\n", vert1, horiz1, 
	 vert2, horiz2);
  printf("alpha1 %lf beta1 %lf, alpha2 %lf beta2 %lf\n", alpha1, beta1,
	 alpha2, beta2);
#endif /* DEBUG */

  if ((vert1 && vert2) || (horiz1 && horiz2))
    {
      if (!fabs(beta1 - beta2) > PRECISION) return(NULL);
      x = P1.x;
      y = P1.y;
    }
  else if ((vert1 && horiz2) || (vert2 && horiz1))
    {
      if (vert1)
	  x = P1.x , y = P2.y;
      else 
	  x = P2.x , y = P1.y;
    }
  else if ((vert1 && !vert2 && !horiz2) || (vert2 && !vert1 && !horiz1))
    {
      if (vert1)
	  x = P1.x , y = (x - beta2) / alpha2;
      else x = P2.x , y = (x - beta1) / alpha1;
    }
  else if ((horiz1 && !vert2 && !horiz2) || (horiz2 && !vert1 && !horiz1))
    {
      if (horiz1)
	y = P1.y , x = (y - beta2) / alpha2;
      else y = P2.y , x = (y - beta1) / alpha1;
    }
  else if (!vert1 && !horiz1 && !vert2 && !horiz2)
    {
      if (fabs(alpha1 - alpha2) > PRECISION) /* alpha1 != alpha2 */
	{
	  x = (beta2 - beta1) / (alpha1 - alpha2);
	  y = alpha1 * x + beta1;
	}
      else if (fabs(beta1 - beta2) > PRECISION) /* alpha1 = alpha2
						   && beta1 != beta2 */
	return(NULL);
      else /* alpha1 = alpha2 && beta1 = beta2 */
	x = P1.x , y = P1.y;
    }

  /* now check if solution in between P1, Q1 and P2, Q2, else return NULL */
  if (((Position)roundint(x) < min(P1.x, Q1.x)) || 
      ((Position)roundint(x) > max(P1.x, Q1.x)) ||
      ((Position)roundint(y) < min(P1.y, Q1.y)) ||
      ((Position)roundint(y) > max(P1.y, Q1.y)) ||
      ((Position)roundint(x) < min(P2.x, Q2.x)) || 
      ((Position)roundint(x) > max(P2.x, Q2.x)) ||
      ((Position)roundint(y) < min(P2.y, Q2.y)) ||
      ((Position)roundint(y) > max(P2.y, Q2.y)))
    return(NULL);
  else
    {
      result = (XPoint *) XtMalloc(sizeof(XPoint));
      result->x = (Position) roundint(x);
      result->y = (Position) roundint(y);
      return(result);
    }
}


/* the algorithm is : find A1, A2, B1, B2 intersections of the axe to draw
 * and the borders of the square (t, l, b, r) and then draw line between
 * (A1 or A2) and (B1 or B2) 
 */
void RedrawAxesInSquare(PW, x, y)
     PixmapWidget PW;
     Position x, y;
{
  int w = PW->pixmap.width, h =PW->pixmap.height;
  XPoint P1, Q1, P2, Q2, *A1, *A2, *B1, *B2, *from, *to;

  if (PWQueryAxes((Widget)PW))
    {
      /* first diagonal */
      P2.x = InWindowX(PW, 0); P2.y = InWindowY(PW, 0);
      Q2.x = InWindowX(PW, w); Q2.y = InWindowY(PW, h);
      
      /* A1: top intersect diagonal */
      P1.x = InWindowX(PW, x); P1.y = InWindowY(PW, y);
      Q1.x = InWindowX(PW, x+1); Q1.y = P1.y;
      A1 = intersect(P1, Q1, P2, Q2);
      
      /* A2: left intersect diagonal */
      P1.x = InWindowX(PW, x); P1.y = InWindowY(PW, y);
      Q1.x = P1.x; Q1.y = InWindowY(PW, y+1);
      A2 = intersect(P1, Q1, P2, Q2);
      
      /* B1: bottom intersect diagonal */
      P1.x = InWindowX(PW, x); P1.y = InWindowY(PW, y+1);
      Q1.x = InWindowX(PW, x+1); Q1.y = P1.y;
      B1 = intersect(P1, Q1, P2, Q2);
      
      /* B2: right intersect diagonal */
      P1.x = InWindowX(PW, x+1); P1.y = InWindowY(PW, y);
      Q1.x = P1.x; Q1.y = InWindowY(PW, y+1);
      B2 = intersect(P1, Q1, P2, Q2);
      
      /* line to draw from=OR(a1, a2) to=OR(b1, b2) */
      from = OR(A1, A2); to = OR(B1, B2);
      if (from && to) XDrawLine(dpy, XtWindow(PW), PW->pixmap.framing_gc,
				from->x, from->y, to->x, to->y);
      XtFree((char *)A1); XtFree((char *)A2);
      XtFree((char *)B1); XtFree((char *)B2);
      
      /* second diagonal */
      P2.x = InWindowX(PW, 0); P2.y = InWindowY(PW, h);
      Q2.x = InWindowX(PW, w); Q2.y = InWindowY(PW, 0);
      
      /* A1: top intersect diagonal */
      P1.x = InWindowX(PW, x); P1.y = InWindowY(PW, y);
      Q1.x = InWindowX(PW, x+1); Q1.y = P1.y;
      A1 = intersect(P1, Q1, P2, Q2);
      
      /* A2: right intersect diagonal */
      P1.x = InWindowX(PW, x+1); P1.y = InWindowY(PW, y);
      Q1.x = P1.x; Q1.y = InWindowY(PW, y+1);
      A2 = intersect(P1, Q1, P2, Q2);
      
      /* B1: bottom intersect diagonal */
      P1.x = InWindowX(PW, x); P1.y = InWindowY(PW, y+1);
      Q1.x = InWindowX(PW, x+1); Q1.y = P1.y;
      B1 = intersect(P1, Q1, P2, Q2);
      
      /* B2: left intersect diagonal */
      P1.x = InWindowX(PW, x); P1.y = InWindowY(PW, y);
      Q1.x = P1.x; Q1.y = InWindowY(PW, y+1);
      B2 = intersect(P1, Q1, P2, Q2);
      
      /* line to draw from=OR(a1, a2) to=OR(b1, b2) */
      from = OR(A1, A2); to = OR(B1, B2);
      if (from && to) XDrawLine(dpy, XtWindow(PW), PW->pixmap.framing_gc,
				from->x, from->y, to->x, to->y);
      XtFree((char *)A1); XtFree((char *)A2);
      XtFree((char *)B1); XtFree((char *)B2);

      /* horizontal */
      if (((h % 2) != 0) || (!PWQueryGrid((Widget)PW))) /* height is even 
                                                           or grid is off */
	{
	  Position y0;
	  
	  y0 = InWindowY(PW, 0)
	    + roundint((InWindowY(PW, h) - InWindowY(PW, 0)) / 2.);
	  if ((y0 > InWindowY(PW, y)) && (y0 < InWindowY(PW, y+1)))
	    XDrawLine(dpy, XtWindow(PW), PW->pixmap.framing_gc,
		      InWindowX(PW, x), y0, InWindowX(PW, x+1), y0);
	}
      
      /* vertical */
      if (((w % 2) != 0) || (!PWQueryGrid((Widget)PW))) /* width is even 
                                                           or grid is off */
	{
	  Position x0;
	  
	  x0 = InWindowX(PW, 0)
	    + roundint((InWindowX(PW, w) - InWindowX(PW, 0)) / 2.);
	  if ((x0 > InWindowX(PW, x)) && (x0 < InWindowX(PW, x+1)))
	    XDrawLine(dpy, XtWindow(PW), PW->pixmap.framing_gc,
		      x0, InWindowY(PW, y), x0, InWindowY(PW, y+1));
	}
    }
}

typedef struct {
  Position *x, *y;
  Dimension *width, *height;
} Table;

XImage *ScalePixmapImage(PW, src, scale_x, scale_y)
     PixmapWidget PW;
     XImage *src;
     double scale_x, scale_y;
{
  XImage *dst;
  Table table;    
  Position x, y;
  Dimension width, height, w, h;
  Pixel pixel;
  
  width = max(roundint(scale_x * src->width), 1);
  height = max(roundint(scale_y * src->height), 1);
  
  dst = CreatePixmapImage(PW, width, height);
  
  /*
   * It would be nice to check if width or height < 1.0 and
   * average the skipped pixels. But, it is slow as it is now.
   */
  if (scale_x == 1.0 && scale_y == 1.0)
    CopyImageData(src, dst, 0, 0, width-1 , height-1, 0, 0);
  else {
    table.x = (Position *) XtMalloc(sizeof(Position) * src->width);
    table.y = (Position *) XtMalloc(sizeof(Position) * src->height);
    table.width = (Dimension *) XtMalloc(sizeof(Dimension) * src->width);
    table.height = (Dimension *) XtMalloc(sizeof(Dimension) * src->height);
    
    for (x = 0; x < src->width; x++) {
      table.x[x] = roundint(scale_x * x);
      table.width[x] = roundint(scale_x * (x + 1)) - roundint(scale_x * x);
    }
    for (y = 0; y < src->height; y++) {
      table.y[y] = roundint(scale_y * y);
      table.height[y] = roundint(scale_y * (y + 1)) - roundint(scale_y * y);
    }
    
    for (x = 0; x < src->width; x++)
      for (y = 0; y < src->height; y++) {
	pixel = XGetPixel(src, x, y);
	for (w = 0; w < table.width[x]; w++)
	  for (h = 0; h < table.height[y]; h++)
	    if (pixel != PW->pixmap.clear_pixel) XPutPixel(dst, 
							   table.x[x] + w, 
							   table.y[y] + h,
							   pixel);
      }
    
    XtFree((char *)table.x);
    XtFree((char *)table.y);
    XtFree((char *)table.width);
    XtFree((char *)table.height);
  }
  
  return (dst);
}

XImage *ScaleMaskImage(PW, src, scale_x, scale_y)
     PixmapWidget PW;
     XImage *src;
     double scale_x, scale_y;
{
  XImage *dst;
  Table table;    
  Position x, y;
  Dimension width, height, w, h;
  Pixel pixel;
  
  width = max(roundint(scale_x * src->width), 1);
  height = max(roundint(scale_y * src->height), 1);
  
  dst = CreateMaskImage(PW, width, height);
  
  /*
   * It would be nice to check if width or height < 1.0 and
   * average the skipped pixels. But, it is slow as it is now.
   */
  if (scale_x == 1.0 && scale_y == 1.0)
    CopyImageData(src, dst, 0, 0, width-1 , height-1, 0, 0);
  else {
    table.x = (Position *) XtMalloc(sizeof(Position) * src->width);
    table.y = (Position *) XtMalloc(sizeof(Position) * src->height);
    table.width = (Dimension *) XtMalloc(sizeof(Dimension) * src->width);
    table.height = (Dimension *) XtMalloc(sizeof(Dimension) * src->height);
    
    for (x = 0; x < src->width; x++) {
      table.x[x] = roundint(scale_x * x);
      table.width[x] = roundint(scale_x * (x + 1)) - roundint(scale_x * x);
    }
    for (y = 0; y < src->height; y++) {
      table.y[y] = roundint(scale_y * y);
      table.height[y] = roundint(scale_y * (y + 1)) - roundint(scale_y * y);
    }
    
    for (x = 0; x < src->width; x++)
      for (y = 0; y < src->height; y++) {
	pixel = XGetPixel(src, x, y);
	for (w = 0; w < table.width[x]; w++)
	  for (h = 0; h < table.height[y]; h++)
	    if (pixel == 0) XPutPixel(dst, table.x[x] + w, table.y[y] + h,
				      pixel);
      }
    
    XtFree((char *)table.x);
    XtFree((char *)table.y);
    XtFree((char *)table.width);
    XtFree((char *)table.height);
  }
  
  return (dst);
}


/*************************************************************************
 *
 * Interface functions
 *
 *************************************************************************/
/* high level procedures */

#if NeedFunctionPrototypes
GC PWHighlightingGC(Widget w)
#else
GC PWHighlightingGC(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    return PW->pixmap.highlighting_gc;
}

#if NeedFunctionPrototypes
GC PWFramingGC(Widget w)
#else
GC PWFramingGC(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    return PW->pixmap.framing_gc;
}

#if NeedFunctionPrototypes
GC PWDrawingGC(Widget w)
#else
GC PWDrawingGC(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    return PW->pixmap.drawing_gc;
}

#if NeedFunctionPrototypes
Position PWInWindowX(Widget w, float x)
#else
Position PWInWindowX(w,x)
    Widget   w;
    float    x;
#endif
{
    return InWindowX( ((PixmapWidget) w), x );
}

#if NeedFunctionPrototypes
Position PWInWindowY(Widget w, float y)
#else
Position PWInWindowY(w,y)
    Widget   w;
    float    y;
#endif
{
    return InWindowY( ((PixmapWidget) w), y );
}


#if NeedFunctionPrototypes
void PWRedrawHotSpot(Widget w)
#else
void PWRedrawHotSpot(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y))
	DrawHotSpot(PW, PW->pixmap.hot.x, PW->pixmap.hot.y);
}

#if NeedFunctionPrototypes
void PWClearHotSpot(Widget w)
#else
void PWClearHotSpot(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    DrawHotSpot(PW, PW->pixmap.hot.x, PW->pixmap.hot.y);
    PW->pixmap.hot.x = PW->pixmap.hot.y = NotSet;
}

#if NeedFunctionPrototypes
void PWDrawHotSpot(Widget w, Position x, Position y, int value)
#else
void PWDrawHotSpot(w, x, y, value)
    Widget w;
    Position x, y;
    int value;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    if (QueryInPixmap(PW, x, y)) {
	if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y) &&
	    ((PW->pixmap.hot.x == x) && (PW->pixmap.hot.y == y))) {
	    if ((value == Clear) || (value == Invert)) {
		PWClearHotSpot(w);
	    }
	}
	else
	    if ((value == Set) || (value == Invert)) {
		PWClearHotSpot(w);
		DrawHotSpot(PW, x, y);
		PW->pixmap.hot.x = x;
		PW->pixmap.hot.y = y;
	    }
	
	if (value == Highlight)
	    HighlightHotSpot(PW, x, y); 
    }
}

#if NeedFunctionPrototypes
void PWSetHotSpot(Widget w, Position x, Position y)
#else
void PWSetHotSpot(w, x, y)
    Widget w;
    Position x, y;
#endif
{
    if (QuerySet(x, y))
	PWDrawHotSpot(w, x, y, Set);
    else
	PWClearHotSpot(w);
}

#if NeedFunctionPrototypes
void PWDrawPoint(Widget w, Position x, Position y, int value)
#else
void PWDrawPoint(w, x, y, value)
     Widget  w;
     Position      x, y;
     int           value;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  if (QueryInPixmap(PW, x, y)) 
    {
      if (value == Highlight)
	HighlightSquare(PW, x, y);
      else if ((value == Clear) || 
	       ((value == Invert) && 
		(GetPxl(PW, x, y) != PW->pixmap.clear_pixel)))
	{
	  ClearPoint(PW, x, y, Lazy);
	}
      else /* value == Set || (value == Invert && Pxl == clear_pixel) */
	{
	  DrawPoint(PW, x, y, PW->pixmap.foreground_pixel, Lazy);
	}
    }
}

#if NeedFunctionPrototypes
void PWRedrawSquares(Widget w, Position from_x, Position from_y, 
		     Position to_x, Position to_y)
#else
void PWRedrawSquares(w, from_x, from_y, to_x, to_y)
     Widget  w;
     Position from_x, from_y, to_x, to_y;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  register int x, y;
  
  QuerySwap(from_x, to_x);
  QuerySwap(from_y, to_y);
  from_x = max(0, from_x);
  from_y = max(0, from_y);
  to_x = min(PW->pixmap.image->width - 1, to_x);
  to_y = min(PW->pixmap.image->height - 1, to_y);
  
  for (x = from_x; x <= to_x; x++)
    for (y = from_y; y <= to_y; y++)
      DrawSquare(PW, x, y, GetPxl(PW, x, y));
}

#if NeedFunctionPrototypes
void PWRedrawPoints(Widget w, Position from_x, Position from_y, 
		     Position to_x, Position to_y)
#else
void PWRedrawPoints(w, from_x, from_y, to_x, to_y)
     Widget  w;
     Position from_x, from_y, to_x, to_y;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  register int x, y;
  
  QuerySwap(from_x, to_x);
  QuerySwap(from_y, to_y);
  from_x = max(0, from_x);
  from_y = max(0, from_y);
  to_x = min(PW->pixmap.image->width - 1, to_x);
  to_y = min(PW->pixmap.image->height - 1, to_y);
  
  for (x = from_x; x <= to_x; x++)
    for (y = from_y; y <= to_y; y++)
      DrawPoint(PW, x, y, GetPxl(PW, x, y), NotLazy);
}

#if NeedFunctionPrototypes
void PWDrawGrid(Widget w, Position from_x, Position from_y, 
		Position to_x, Position to_y)
#else
void PWDrawGrid(w, from_x, from_y, to_x, to_y)
     Widget w;
     Position from_x, from_y, to_x, to_y;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  int i;
  
  QuerySwap(from_x, to_x);
  QuerySwap(from_y, to_y);
  from_x = max(0, from_x);
  from_y = max(0, from_y);
  to_x = min(PW->pixmap.image->width - 1, to_x);
  to_y = min(PW->pixmap.image->height - 1, to_y);
  
  for(i = from_x + (from_x == 0); i <= to_x; i++)
    XDrawLine(dpy, XtWindow(PW), 
	      PW->pixmap.framing_gc,
	      InWindowX(PW, i), InWindowY(PW, from_y),
	      InWindowX(PW, i), InWindowY(PW, to_y + 1));
  
  for(i = from_y + (from_y == 0); i <= to_y; i++)
    XDrawLine(dpy, XtWindow(PW), 
	      PW->pixmap.framing_gc,
	      InWindowX(PW, from_x), InWindowY(PW, i),
	      InWindowX(PW, to_x + 1), InWindowY(PW, i));
}


#if NeedFunctionPrototypes
void PWRedrawGrid(Widget w, Position from_x, Position from_y, 
		  Position to_x, Position to_y)
#else
void PWRedrawGrid(w, from_x, from_y, to_x, to_y)
     Widget w;
     Position from_x, from_y, to_x, to_y;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  if (PW->pixmap.grid)
    PWDrawGrid(w, from_x, from_y, to_x, to_y);
}

#if NeedFunctionPrototypes
void PWDrawLine(Widget w, Position from_x, Position from_y, 
		Position to_x, Position to_y, int value)
#else
void PWDrawLine(w, from_x, from_y, to_x, to_y, value)
     Widget  w;
     Position from_x, from_y, to_x, to_y;
     int value;
#endif
{
  Position i;
  register double x, y;
  double dx, dy, delta;
  
  dx = to_x - from_x;
  dy = to_y - from_y;
  x = from_x + 0.5;
  y = from_y + 0.5;
  delta = max(abs(dx), abs(dy));
  if (delta > 0) {
    dx /= delta;
    dy /= delta;
    for(i = 0; i <= delta; i++) {
      PWDrawPoint(w, (Position) x, (Position) y, value);
      x += dx;
      y += dy;
    }
  }
  else
    PWDrawPoint(w, from_x, from_y, value);
}

#if NeedFunctionPrototypes
void PWBlindLine(Widget w, Position from_x, Position from_y, 
		 Position to_x, Position to_y, int value)
#else
void PWBlindLine(w, from_x, from_y, to_x, to_y, value)
     Widget  w;
     Position from_x, from_y, to_x, to_y;
     int value;
#endif
{
  Position i;
  register double x, y;
  double dx, dy, delta;
  
  dx = to_x - from_x;
  dy = to_y - from_y;
  x = from_x + 0.5;
  y = from_y + 0.5;
  delta = max(abs(dx), abs(dy));
  if (delta > 0) {
    dx /= delta;
    dy /= delta;
    x += dx;
    y += dy;
    for(i = 1; i <= delta; i++) {
      PWDrawPoint(w, (Position) x, (Position) y, value);
      x += dx;
      y += dy;
    }
  }
  else
    PWDrawPoint(w, from_x, from_y, value);
}

#if NeedFunctionPrototypes
void PWDrawRectangle(Widget w, Position from_x, Position from_y, 
		     Position to_x, Position to_y, int value)
#else
void PWDrawRectangle(w, from_x, from_y, to_x, to_y, value)
     Widget w;
     Position from_x, from_y, to_x, to_y;
     int value;
#endif
{
  register Position i;
  Dimension delta, width, height;
  
  QuerySwap(from_x, to_x);
  QuerySwap(from_y, to_y);
  
  width = to_x - from_x;
  height = to_y - from_y;
  
  delta = max(width, height);
  
  if (!QueryZero(width, height)) {
    for (i = 0; i < (Position)delta; i++) {
      if (i < (Position)width) {
	PWDrawPoint(w, from_x + i, from_y, value);
	PWDrawPoint(w, to_x - i, to_y, value);
      }
      if (i < (Position)height) {
	PWDrawPoint(w, from_x, to_y - i, value);
	PWDrawPoint(w, to_x, from_y + i, value);
      }
    }
  }
  else
    PWDrawLine(w, 
	       from_x, from_y, 
	       to_x, to_y, value);
}

#if NeedFunctionPrototypes
void PWDrawFilledRectangle(Widget w, Position from_x, Position from_y, 
			   Position to_x, Position to_y, int value)
#else
void PWDrawFilledRectangle(w, from_x, from_y, to_x, to_y, value)
     Widget w;
     Position from_x, from_y, to_x, to_y;
     int value;
#endif
{
  register Position x, y;
  
  QuerySwap(from_x, to_x);
  QuerySwap(from_y, to_y);
  
  for (x = from_x; x <= to_x; x++)
    for (y = from_y; y <= to_y; y++)
      PWDrawPoint(w, x, y, value);
}

#if NeedFunctionPrototypes
void PWDrawCircle(Widget w, Position origin_x, Position origin_y, 
		  Position point_x, Position point_y, int value)
#else
void PWDrawCircle(w, origin_x, origin_y, point_x, point_y, value)
     Widget w;
     Position origin_x, origin_y, point_x, point_y;
     int value;
#endif
{
  register Position i, delta;
  Dimension dx, dy, half;
  double radius;
  
  dx = abs(point_x - origin_x);
  dy = abs(point_y - origin_y);
  radius = sqrt((double) (dx * dx + dy * dy));
  if (radius < 1.0) {
    PWDrawPoint(w, origin_x, origin_y, value);
  }
  else {
    PWDrawPoint(w, origin_x - (Position) floor(radius), origin_y, value);
    PWDrawPoint(w, origin_x + (Position) floor(radius), origin_y, value);
    PWDrawPoint(w, origin_x, origin_y - (Position) floor(radius), value);
    PWDrawPoint(w, origin_x, origin_y + (Position) floor(radius), value);
  }
  half = radius / sqrt(2.0);
  for(i = 1; i <= (Position)half; i++) {
    delta = sqrt(radius * radius - i * i);
    PWDrawPoint(w, origin_x - delta, origin_y - i, value);
    PWDrawPoint(w, origin_x - delta, origin_y + i, value);
    PWDrawPoint(w, origin_x + delta, origin_y - i, value);
    PWDrawPoint(w, origin_x + delta, origin_y + i, value);
    if (i != delta) {
      PWDrawPoint(w, origin_x - i, origin_y - delta, value);
      PWDrawPoint(w, origin_x - i, origin_y + delta, value);
      PWDrawPoint(w, origin_x + i, origin_y - delta, value);
      PWDrawPoint(w, origin_x + i, origin_y + delta, value);
    }
  }
}

#if NeedFunctionPrototypes
void PWDrawFilledCircle(Widget w, Position origin_x, Position origin_y, 
			Position point_x, Position point_y, int value)
#else
void PWDrawFilledCircle(w, origin_x, origin_y, point_x, point_y, value)
     Widget w;
     Position origin_x, origin_y, point_x, point_y;
     int value;
#endif
{
  register Position i, j, delta;
  Dimension dx, dy;
  double radius;
  
  dx = abs(point_x - origin_x);
  dy = abs(point_y - origin_y);
  radius = sqrt((double) (dx * dx + dy * dy));
  for(j = origin_x - (Position) floor(radius); 
      j <= origin_x + (Position) floor(radius); j++)
    PWDrawPoint(w, j, origin_y, value);
  for(i = 1; i <= (Position) floor(radius); i++) {
    delta = sqrt(radius * radius - i * i);
    for(j = origin_x - delta; j <= origin_x + delta; j++) {
      PWDrawPoint(w, j, origin_y - i, value);
      PWDrawPoint(w, j, origin_y + i, value);
    }
  }
}


#if NeedFunctionPrototypes
void PWFloodFill(Widget w, Position x, Position y, int value)
#else
void PWFloodFill(w, x, y, value)
     Widget w;
     Position x, y;
     int value;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  Pixel pixel, foreground = PW->pixmap.foreground_pixel;
  Pixel clear = PW->pixmap.clear_pixel;
  
  pixel = GetPxl(PW, x, y);
  
  if (value == Invert)
    FloodLoop(PW, x, y, ((pixel != clear) ? clear : foreground), pixel);
  else if (value == Clear)
    FloodLoop(PW, x, y, clear, pixel);
  else
    FloodLoop(PW, x, y, foreground, pixel); 
}

#if NeedFunctionPrototypes
void PWUp(Widget w)
#else
void PWUp(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  register Position x, y;
  Pixel first, up, down;
  Position from_x, from_y, to_x, to_y;
  
  if (PWQueryMarked(w)) {
    from_x = PW->pixmap.mark.from_x;
    from_y = PW->pixmap.mark.from_y;
    to_x = PW->pixmap.mark.to_x;
    to_y = PW->pixmap.mark.to_y;
  }
  else {
    from_x = 0;
    from_y = 0;
    to_x = PW->pixmap.width - 1;
    to_y = PW->pixmap.height - 1;
  }
  
  if ((to_y - from_y) == 0)
    return;
  
  for(x = from_x; x <= to_x; x++) {
    first = up = GetPxl(PW, x, to_y);
    for(y = to_y - 1; y >= from_y; y--) {
      down = GetPxl(PW, x, y);
      if (up != down) 
	{
	  DrawPoint(PW, x, y, up, Lazy);
	}
      up = down;
    }
    if (first != down)
      {
	DrawPoint(PW, x, to_y, down, Lazy);
      }
  }
  
  if (!PWQueryMarked(w)) {
      if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y)) {
          PWSetHotSpot(w,
		       PW->pixmap.hot.x,
		       (PW->pixmap.hot.y - 1 + PW->pixmap.image->height) % 
		       PW->pixmap.image->height);
      }
      if (PW->pixmap.translateCallback) 
          PW->pixmap.translateCallback( w, 0, -1 );
  }
}

#if NeedFunctionPrototypes
void PWDown(Widget w)
#else
void PWDown(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  register Position x, y;
  Pixel first, down, up;
  Position from_x, from_y, to_x, to_y;
  
  if (PWQueryMarked(w)) {
    from_x = PW->pixmap.mark.from_x;
    from_y = PW->pixmap.mark.from_y;
    to_x = PW->pixmap.mark.to_x;
    to_y = PW->pixmap.mark.to_y;
  }
  else {
    from_x = 0;
    from_y = 0;
    to_x = PW->pixmap.width - 1;
    to_y = PW->pixmap.height - 1;
  }
  
  if ((to_y - from_y) == 0)
    return;
  
  for(x = from_x; x <= to_x; x++) {
    first = down = GetPxl(PW, x, from_y);
    for(y = from_y + 1; y <= to_y; y++) {
      up = GetPxl(PW, x, y);
      if (down != up)
	{
	  DrawPoint(PW, x, y, down, Lazy);
	}
      down = up;
    }
    if(first != up) 
      {
	DrawPoint(PW, x, from_y, up, Lazy);
      }
  }
  
  if (!PWQueryMarked(w)) {
      if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y)) {
          PWSetHotSpot(w,
		       PW->pixmap.hot.x,
		       (PW->pixmap.hot.y + 1) % PW->pixmap.image->height);
      }
      if (PW->pixmap.translateCallback) 
          PW->pixmap.translateCallback( w, 0, 1 );
  }
}

#if NeedFunctionPrototypes
void PWLeft(Widget w)
#else
void PWLeft(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  register Position x, y;
  Pixel first, left, right;
  Position from_x, from_y, to_x, to_y;
  
  if (PWQueryMarked(w)) {
    from_x = PW->pixmap.mark.from_x;
    from_y = PW->pixmap.mark.from_y;
    to_x = PW->pixmap.mark.to_x;
    to_y = PW->pixmap.mark.to_y;
  }
  else {
    from_x = 0;
    from_y = 0;
    to_x = PW->pixmap.width - 1;
    to_y = PW->pixmap.height - 1;
  }
  
  if ((to_x - from_x) == 0)
    return;
  
  for(y = from_y; y <= to_y; y++) {
    first = left = GetPxl(PW, to_x, y);
    for(x = to_x - 1; x >= from_x; x--) {
      right = GetPxl(PW, x, y);
      if (left != right)
	{
	  DrawPoint(PW, x, y, left, Lazy);
	}
      left = right;
    }
    if(first != right)
      {
	DrawPoint(PW, to_x, y, right, Lazy);
      }
  }

  if (!PWQueryMarked(w)) {
      if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y)) {
          PWSetHotSpot(w,
		       (PW->pixmap.hot.x - 1 + PW->pixmap.image->width) % 
		           PW->pixmap.image->width,
		       PW->pixmap.hot.y);
      }
      if (PW->pixmap.translateCallback) 
          PW->pixmap.translateCallback( w, -1, 0 );
  }
}

#if NeedFunctionPrototypes
void PWRight(Widget w)
#else
void PWRight(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  register Position x, y;
  Pixel first, right, left;
  Position from_x, from_y, to_x, to_y;
  
  if (PWQueryMarked(w)) {
    from_x = PW->pixmap.mark.from_x;
    from_y = PW->pixmap.mark.from_y;
    to_x = PW->pixmap.mark.to_x;
    to_y = PW->pixmap.mark.to_y;
  }
  else {
    from_x = 0;
    from_y = 0;
    to_x = PW->pixmap.width - 1;
    to_y = PW->pixmap.height - 1;
  }
  
  if ((to_x - from_x) == 0)
    return;
  
  for(y = from_y; y <= to_y; y++) {
    first = right = GetPxl(PW, from_x, y);
    for(x = from_x + 1; x <= to_x; x++) {
      left = GetPxl(PW, x, y);
      if (right != left)
	{
	  DrawPoint(PW, x, y, right, Lazy);
	}
      right = left;
    }
    if(first != left)
      {
	DrawPoint(PW, from_x, y, left, Lazy);
      }
  }

  if (!PWQueryMarked(w)) {
      if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y)) {
          PWSetHotSpot(w,
		       (PW->pixmap.hot.x + 1) % PW->pixmap.image->width,
		       PW->pixmap.hot.y);
      }
      if (PW->pixmap.translateCallback) 
          PW->pixmap.translateCallback( w, 1, 0 );
  }
}

#if NeedFunctionPrototypes
void PWRedrawMark(Widget w)
#else
void PWRedrawMark(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  if (QuerySet(PW->pixmap.mark.from_x, PW->pixmap.mark.from_y)) 
    XFillRectangle(dpy, XtWindow(PW), PW->pixmap.highlighting_gc,
		   InWindowX(PW, PW->pixmap.mark.from_x), 
		   InWindowY(PW, PW->pixmap.mark.from_y), 
		   InWindowX(PW, PW->pixmap.mark.to_x + 1) - 
		   InWindowX(PW, PW->pixmap.mark.from_x),
		   InWindowY(PW, PW->pixmap.mark.to_y + 1) -
		   InWindowY(PW, PW->pixmap.mark.from_y));
}

#if NeedFunctionPrototypes
void PWFold(Widget w)
#else
void PWFold(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  Position x, y, new_x, new_y;
  Dimension horiz, vert;
  XImage *storage, *mask_storage;
  Pixel color;
  
  storage = CreatePixmapImage(PW, (Dimension) PW->pixmap.image->width, 
			      (Dimension) PW->pixmap.image->height);
  mask_storage = CreateMaskImage(PW, (Dimension) PW->pixmap.mask_image->width, 
				 (Dimension) PW->pixmap.mask_image->height);
  
  TransferImageData(PW->pixmap.image, storage);
  TransferImageData(PW->pixmap.mask_image, mask_storage);
  
  PW->pixmap.fold ^= True;
  horiz = (PW->pixmap.image->width + PW->pixmap.fold) / 2;
  vert = (PW->pixmap.image->height + PW->pixmap.fold) / 2;
  
  for (x = 0; x < PW->pixmap.image->width; x++)
    for (y = 0; y < PW->pixmap.image->height; y++) {
      new_x = (x + horiz) % PW->pixmap.image->width;
      new_y = (y + vert) % PW->pixmap.image->height;
      if(GetPxl(PW, new_x, new_y) != 
	 (color = GetPxlFromImageAndMask(storage, mask_storage, x, y)))
	{
	  DrawPoint(PW, new_x, new_y, color, Lazy);
	}
    }
  
  DestroyPixmapImage(&storage);
  DestroyMaskImage(&mask_storage);
  
  if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y))
    PWSetHotSpot(w, 
		 (Position) 
		 ((PW->pixmap.hot.x + horiz) % PW->pixmap.image->width),
		 (Position)
		 ((PW->pixmap.hot.y + vert) % PW->pixmap.image->height));
  if (PW->pixmap.translateCallback) 
      PW->pixmap.translateCallback( w, horiz, vert );
}

#if NeedFunctionPrototypes
void PWClear(Widget w)
#else
void PWClear(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  register Position x, y;
  register Position from_x, from_y, to_x, to_y;
  
  if (PWQueryMarked(w)) {
    from_x = PW->pixmap.mark.from_x;
    from_y = PW->pixmap.mark.from_y;
    to_x = PW->pixmap.mark.to_x;
    to_y = PW->pixmap.mark.to_y;
  }
  else {
    from_x = 0;
    from_y = 0;
    to_x = PW->pixmap.image->width - 1;
    to_y = PW->pixmap.image->height - 1;
  }
  for (x = from_x; x <= to_x; x++)
    for (y = from_y; y <= to_y; y++)
      ClearPoint(PW, x, y, Lazy);
/* This way is too flashy
      ClearPxl(PW->pixmap.image, x, y, PW->pixmap.clear_pixel);
  XClearArea(dpy, XtWindow(PW),
	     0, 0, PW->core.width, PW->core.height, True);*/

}

#if NeedFunctionPrototypes
void PWSet(Widget w)
#else
void PWSet(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  register Position x, y;
  register Position from_x, from_y, to_x, to_y;
  
  if (PWQueryMarked(w)) {
    from_x = PW->pixmap.mark.from_x;
    from_y = PW->pixmap.mark.from_y;
    to_x = PW->pixmap.mark.to_x;
    to_y = PW->pixmap.mark.to_y;
  }
  else {
    from_x = 0;
    from_y = 0;
    to_x = PW->pixmap.image->width - 1;
    to_y = PW->pixmap.image->height - 1;
  }
  for (x = from_x; x <= to_x; x++)
    for (y = from_y; y <= to_y; y++)
      {
	DrawPoint(PW, x, y, PW->pixmap.foreground_pixel, Lazy);
      }

}

#if NeedFunctionPrototypes
void PWRedraw(Widget w)
#else
void PWRedraw(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  XClearArea(dpy, XtWindow(PW),
	     0, 0, PW->core.width, PW->core.height,
	     True);
}


/*--------------------------------------------------------------------------*/
/*
			P W T r a n s l a t e P o i n t

	Translate a point by dx, dy wrapping around if the point goes off
	the pixmap
*/
/*--------------------------------------------------------------------------*/
void PWTranslatePoint( w, x, y, dx, dy )
    Widget	w;
    Position	*x;
    Position	*y;
    Position	dx;
    Position	dy;
{
    PixmapWidget PW = (PixmapWidget) w;
/*
    printf("PWTranslatePoint : %d,%d by %d,%d\n", *x, *y, dx, dy);
*/
    *x = (*x + dx + PW->pixmap.image->width)  % PW->pixmap.image->width;
    *y = (*y + dy + PW->pixmap.image->height) % PW->pixmap.image->height;
}


/*--------------------------------------------------------------------------*/
/*
                        P W F l i p P o i n t

        Flip a given point guaranteeing that the flipped point will be
        in the pixmap.
*/
/*--------------------------------------------------------------------------*/

void PWFlipPoint( w, x, y, axis )
    Widget          w;
    Position        *x;
    Position        *y;
    enum FlipAxis   axis;
{
    PixmapWidget PW = (PixmapWidget) w;

    if ( axis == Vertical ) 
        *x = PW->pixmap.image->width  - 1 - *x;
    else 
        *y = PW->pixmap.image->height - 1 - *y;
}


/*--------------------------------------------------------------------------*/
/*
                        R o t a t e P o i n t

        Rotate a single point left or right 90 degrees. Guarantee that
        result is in pixmap.
*/
/*--------------------------------------------------------------------------*/

void PWRotatePoint( w, x, y, direction )
    Widget                      w;
    Position                    *x;
    Position                    *y;
    enum RotateDirection        direction;
{
    PixmapWidget PW = (PixmapWidget) w;

    Position from_x, from_y, to_x, to_y;
    Position half_width, half_height, shift;
    Position tmp;

    if (PWQueryMarked(w)) {
      from_x = PW->pixmap.mark.from_x;
      from_y = PW->pixmap.mark.from_y;
      to_x   = PW->pixmap.mark.to_x;
      to_y   = PW->pixmap.mark.to_y;
    }
    else {
      from_x = 0;
      from_y = 0;
      to_x   = PW->pixmap.width - 1;
      to_y   = PW->pixmap.height - 1;
    }
    half_width = floor((to_x - from_x) / 2.0 + 0.5);
    half_height= floor((to_y - from_y ) / 2.0 + 0.5);
    shift      = min((Position)(to_x - from_x), (Position)(to_y - from_y )) % 2;

    *x -= half_width;
    *y -= half_height;
    if (*x >= 0) *x += shift;
    if (*y >= 0) *y += shift;
    tmp = *x;
    if (direction == Right) {
        *x = - *y;
        *y = tmp;
    } else {
        *x = *y;
        *y = - tmp;
    }
    if (*x > 0) *x -= shift;
    if (*y > 0) *y -= shift;
    *x += half_width;
    *y += half_height;
}


#if NeedFunctionPrototypes
void PWFlipHoriz(Widget w)
#else
void PWFlipHoriz(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  register Position x, y;
  Position from_x, from_y, to_x, to_y;
  XPoint hot;
  float half;
  Pixel color1, color2;
  
  if (PWQueryMarked(w)) {
    from_x = PW->pixmap.mark.from_x;
    from_y = PW->pixmap.mark.from_y;
    to_x = PW->pixmap.mark.to_x;
    to_y = PW->pixmap.mark.to_y;
  }
  else {
    from_x = 0;
    from_y = 0;
    to_x = PW->pixmap.width - 1;
    to_y = PW->pixmap.height - 1;
  }
  half = (float) (to_y - from_y) / 2.0 + 0.5;
  
  if (half == 0.0)
    return;
  
  for (x = from_x; x <= to_x; x++) 
    for (y = 0; y <  half; y++)
      if ((color1 = GetPxl(PW, x, from_y + y)) != 
	  (color2 = GetPxl(PW, x, to_y - y))) {
	DrawPoint(PW, x, from_y + y, color2, Lazy);
	DrawPoint(PW, x, to_y - y, color1, Lazy);
      }

  if (!PWQueryMarked(w)) {
      if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y)) {
          hot.x = PW->pixmap.hot.x;
          hot.y = PW->pixmap.hot.y;
          PWFlipPoint( w, &hot.x, &hot.y, Horizontal ); 
          PWSetHotSpot(w, hot.x, hot.y );
      }
      /* flip extensions */
      if (PW->pixmap.flipCallback) 
          PW->pixmap.flipCallback( w, Horizontal );
  }
}


#if NeedFunctionPrototypes
void PWFlipVert(Widget w)
#else
void PWFlipVert(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  register Position x, y;
  Position from_x, from_y, to_x, to_y;
  XPoint hot;
  float half;
  Pixel color1, color2;
  
  if (PWQueryMarked(w)) {
    from_x = PW->pixmap.mark.from_x;
    from_y = PW->pixmap.mark.from_y;
    to_x = PW->pixmap.mark.to_x;
    to_y = PW->pixmap.mark.to_y;
  }
  else {
    from_x = 0;
    from_y = 0;
    to_x = PW->pixmap.width - 1;
    to_y = PW->pixmap.height - 1;
  }
  half = (float) (to_x - from_x) / 2.0 + 0.5;
  
  if (half == 0)
    return;
  
  for (y = from_y; y <= to_y; y++)
    for (x = 0; x < half; x++)
      if ((color1 = GetPxl(PW, from_x + x, y)) != 
	  (color2 = GetPxl(PW, to_x - x, y))) {
	DrawPoint(PW, from_x + x, y, color2, Lazy);
	DrawPoint(PW, to_x - x, y, color1, Lazy);
      }

  if (!PWQueryMarked(w)) {
      if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y)) {
          hot.x = PW->pixmap.hot.x;
          hot.y = PW->pixmap.hot.y;
          PWFlipPoint( w, &hot.x, &hot.y, Vertical ); 
          PWSetHotSpot(w, hot.x, hot.y );
      }
      /* flip extensions */
      if (PW->pixmap.flipCallback) 
          PW->pixmap.flipCallback( w, Vertical );
  }
}


#if NeedFunctionPrototypes
void PWRotateRight(Widget w)
#else
void PWRotateRight(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  Position x, y, delta, shift;
  Position half_width, half_height;
  XPoint hot;
  Pixel quad1, quad2, quad3, quad4;
  Position from_x, from_y, to_x, to_y;
  
  if (PWQueryMarked(w)) {
    from_x = PW->pixmap.mark.from_x;
    from_y = PW->pixmap.mark.from_y;
    to_x = PW->pixmap.mark.to_x;
    to_y = PW->pixmap.mark.to_y;
  }
  else {
    from_x = 0;
    from_y = 0;
    to_x = PW->pixmap.width - 1;
    to_y = PW->pixmap.height - 1;
  }
  
  half_width = floor((to_x - from_x) / 2.0 + 0.5);
  half_height = floor((to_y - from_y ) / 2.0 + 0.5);
  shift = min((Position)(to_x - from_x), (Position)(to_y - from_y )) % 2;
  delta = min((Position) half_width, (Position) half_height) - shift;
  
  for (x = 0; x <= delta; x++) {
    for (y = 1 - shift; y <= delta; y++) {
      quad1 = GetPxl(PW, 
		     from_x + (Position)half_width + x, 
		     from_y + (Position)half_height + y);
      quad2 = GetPxl(PW, 
		     from_x + (Position)half_width + y, 
		     from_y + (Position)half_height - shift - x);
      quad3 = GetPxl(PW, 
		     from_x + (Position)half_width - shift - x, 
		     from_y + (Position)half_height - shift - y);
      quad4 = GetPxl(PW, 
		     from_x + (Position)half_width - shift - y, 
		     from_y + (Position)half_height + x);
      
      if (quad1 != quad2)
	{
	  DrawPoint(PW, from_x + (Position)half_width + x, 
		    from_y + (Position)half_height + y, quad2, Lazy);
	}
      if (quad2 != quad3)
	{
	  DrawPoint(PW, from_x + (Position)half_width + y, 
		    from_y + (Position)half_height - shift - x, quad3, Lazy);
	}
      if (quad3 != quad4)
	{
	  DrawPoint(PW, from_x + (Position)half_width - shift - x,
		    from_y + (Position)half_height - shift - y, quad4, Lazy);
	}
      if (quad4 != quad1)
	{
	  DrawPoint(PW, from_x + (Position)half_width - shift - y, 
		    from_y + (Position)half_height + x, quad1, Lazy);
	}
    }
  }
  
  if (!PWQueryMarked(w)) {
      if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y)) {
          hot.x = PW->pixmap.hot.x;
          hot.y = PW->pixmap.hot.y;
          PWRotatePoint( w, &hot.x, &hot.y, Right ); 
          if (QueryInPixmap(PW, hot.x, hot.y))
            PWSetHotSpot(w, hot.x, hot.y);
      }
      /* rotate extensions */
      if (PW->pixmap.rotateCallback) 
          PW->pixmap.rotateCallback( w, Right );
  }
}


#if NeedFunctionPrototypes
void PWRotateLeft(Widget w)
#else
void PWRotateLeft(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  Position x, y,delta, shift;
  Position half_width, half_height;
  XPoint hot;
  Pixel quad1, quad2, quad3, quad4;
  Position from_x, from_y, to_x, to_y;
  
  if (PWQueryMarked(w)) {
    from_x = PW->pixmap.mark.from_x;
    from_y = PW->pixmap.mark.from_y;
    to_x = PW->pixmap.mark.to_x;
    to_y = PW->pixmap.mark.to_y;
  }
  else {
    from_x = 0;
    from_y = 0;
    to_x = PW->pixmap.width - 1;
    to_y = PW->pixmap.height - 1;
  }
  
  half_width = floor((to_x - from_x) / 2.0 + 0.5);
  half_height = floor((to_y - from_y ) / 2.0 + 0.5);
  shift = min((Position)(to_x - from_x), (Position)(to_y - from_y )) % 2;
  delta = min((Position) half_width, (Position) half_height) - shift;
  
  for (x = 0; x <= delta; x++) {
    for (y = 1 - shift; y <= delta; y++) {
      quad1 = GetPxl(PW, 
		     from_x + (Position)half_width + x, 
		     from_y + (Position)half_height + y);
      quad2 = GetPxl(PW, 
		     from_x + (Position)half_width + y, 
		     from_y + (Position)half_height - shift - x);
      quad3 = GetPxl(PW, 
		     from_x + (Position)half_width - shift - x, 
		     from_y + (Position)half_height - shift - y);
      quad4 = GetPxl(PW, 
		     from_x + (Position)half_width - shift - y, 
		     from_y + (Position)half_height + x);
      
      if (quad1 != quad4)
	{
	  DrawPoint(PW, from_x + (Position)half_width + x, 
		    from_y + (Position)half_height + y, quad4, Lazy);
	}
      if (quad2 != quad1)
	{
	  DrawPoint(PW, from_x + (Position)half_width + y, 
		    from_y + (Position)half_height - shift - x, quad1, Lazy);
	}
      if (quad3 != quad2)
	{
	  DrawPoint(PW, from_x + (Position)half_width - shift - x,
		    from_y + (Position)half_height - shift - y, quad2, Lazy);
	}
      if (quad4 != quad3)
	{
	  DrawPoint(PW, from_x + (Position)half_width - shift - y, 
		    from_y + (Position)half_height + x, quad3, Lazy);
	}
    }
  }

  if (!PWQueryMarked(w)) {
      if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y)) {
          hot.x = PW->pixmap.hot.x;
          hot.y = PW->pixmap.hot.y;
          PWRotatePoint( w, &hot.x, &hot.y, Left ); 
          if (QueryInPixmap(PW, hot.x, hot.y))
            PWSetHotSpot(w, hot.x, hot.y);
      }
      /* rotate extensions */
      if (PW->pixmap.rotateCallback) 
          PW->pixmap.rotateCallback( w, Left );
  }
}


#if NeedFunctionPrototypes
void PWStore(Widget w)
#else
void PWStore(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  Dimension width, height;
  
  if (QuerySet(PW->pixmap.mark.from_x, PW->pixmap.mark.from_y)) {
    
    DestroyPixmapImage(&PW->pixmap.storage);
    DestroyMaskImage(&PW->pixmap.mask_storage);
    
    width = PW->pixmap.mark.to_x - PW->pixmap.mark.from_x + 1;
    height = PW->pixmap.mark.to_y - PW->pixmap.mark.from_y + 1;
    
    PW->pixmap.storage = CreatePixmapImage(PW, width, height);
    PW->pixmap.mask_storage = CreateMaskImage(PW, width, height);
    
    CopyImageData(PW->pixmap.image, PW->pixmap.storage,
		  PW->pixmap.mark.from_x,  PW->pixmap.mark.from_y,
		  PW->pixmap.mark.to_x,  PW->pixmap.mark.to_y,
		  0, 0);
    
    CopyImageData(PW->pixmap.mask_image, PW->pixmap.mask_storage,
		  PW->pixmap.mark.from_x,  PW->pixmap.mark.from_y,
		  PW->pixmap.mark.to_x,  PW->pixmap.mark.to_y,
		  0, 0);
  }
}

#if NeedFunctionPrototypes
void PWClearMarked(Widget w)
#else
void PWClearMarked(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  if (QuerySet(PW->pixmap.mark.from_x, PW->pixmap.mark.from_y))
    {
      Position from_x = PW->pixmap.mark.from_x, 
      from_y = PW->pixmap.mark.from_y, to_x = PW->pixmap.mark.to_x, 
      to_y = PW->pixmap.mark.to_y;
      Position x, y;
      QuerySwap(from_x, to_x);
      QuerySwap(from_y, to_y);
      
      for (x = from_x; x <= to_x; x++)
	for (y = from_y; y <= to_y; y++)
	  {
	    ClearPoint(PW, x, y, Lazy);
	  }
    }
}


#if NeedFunctionPrototypes
void PWDragMarked(Widget w, Position at_x, Position at_y)
#else
void PWDragMarked(w, at_x, at_y)
     Widget w;
     Position     at_x, at_y;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  if (QuerySet(PW->pixmap.mark.from_x, PW->pixmap.mark.from_y))
    PWDrawRectangle(w, 
		    at_x, at_y, 
		    at_x + PW->pixmap.mark.to_x - PW->pixmap.mark.from_x,
		    at_y + PW->pixmap.mark.to_y - PW->pixmap.mark.from_y,
		    Highlight);
}

#if NeedFunctionPrototypes
void PWDragStored(Widget w, Position at_x, Position at_y)
#else
void PWDragStored(w, at_x, at_y)
     Widget w;
     Position     at_x, at_y;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  if (PW->pixmap.storage)
    PWDrawRectangle(w, 
		    at_x, at_y,
		    at_x + PW->pixmap.storage->width - 1,
		    at_y + PW->pixmap.storage->height - 1,
		    Highlight);
}

#if NeedFunctionPrototypes
void PWRestore(Widget w, Position at_x, Position at_y, int value)
#else
void PWRestore(w, at_x, at_y, value)
     Widget w;
     Position     at_x, at_y;
     int          value;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  if (PW->pixmap.storage)
    DrawImageData(PW, PW->pixmap.storage, PW->pixmap.mask_storage,
		  at_x, at_y, value, Lazy);
}

#if NeedFunctionPrototypes
void PWCopy(Widget w, Position at_x, Position at_y, int value)
#else
void PWCopy(w, at_x, at_y, value)
     Widget w;
     Position     at_x, at_y;
     int          value;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  XImage *storage, *mask_storage;
  Dimension width, height;
  
  if (QuerySet(PW->pixmap.mark.from_x, PW->pixmap.mark.from_y)) {
    
    width = PW->pixmap.mark.to_x - PW->pixmap.mark.from_x + 1;
    height = PW->pixmap.mark.to_y - PW->pixmap.mark.from_y + 1;
    
    storage = CreatePixmapImage(PW, width, height);
    mask_storage = CreateMaskImage(PW, width, height);
    
    CopyImageData(PW->pixmap.image, storage,
		  PW->pixmap.mark.from_x,  PW->pixmap.mark.from_y,
		  PW->pixmap.mark.to_x,  PW->pixmap.mark.to_y,
		  0, 0);
    CopyImageData(PW->pixmap.mask_image, mask_storage,
		  PW->pixmap.mark.from_x,  PW->pixmap.mark.from_y,
		  PW->pixmap.mark.to_x,  PW->pixmap.mark.to_y,
		  0, 0);
    
    DrawImageData(PW, storage, mask_storage, at_x, at_y, value, NotLazy);
    
    PWMark(w, at_x, at_y,
	   at_x + PW->pixmap.mark.to_x - PW->pixmap.mark.from_x,
	   at_y + PW->pixmap.mark.to_y - PW->pixmap.mark.from_y); 

    DestroyPixmapImage(&storage);
    DestroyMaskImage(&mask_storage);
  }
}

#if NeedFunctionPrototypes
void PWMove(Widget w, Position at_x, Position at_y, int value)
#else
void PWMove(w, at_x, at_y, value)
     Widget   w;
     Position at_x, at_y;
     int      value;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  XImage *storage, *mask_storage;
  Dimension width, height;
  
  if (QuerySet(PW->pixmap.mark.from_x, PW->pixmap.mark.from_y)) {
    
    width = PW->pixmap.mark.to_x - PW->pixmap.mark.from_x + 1;
    height = PW->pixmap.mark.to_y - PW->pixmap.mark.from_y + 1;
    
    storage = CreatePixmapImage(PW, width, height);
    mask_storage = CreateMaskImage(PW, width, height);
    
    CopyImageData(PW->pixmap.image, storage,
		  PW->pixmap.mark.from_x,  PW->pixmap.mark.from_y,
		  PW->pixmap.mark.to_x,  PW->pixmap.mark.to_y,
		  0, 0);
    
    CopyImageData(PW->pixmap.mask_image, mask_storage,
		  PW->pixmap.mark.from_x,  PW->pixmap.mark.from_y,
		  PW->pixmap.mark.to_x,  PW->pixmap.mark.to_y,
		  0, 0);
    
    PWClearMarked(w);

    DrawImageData(PW, storage, mask_storage, at_x, at_y, value, NotLazy); 
    
    PWMark(w, at_x, at_y,
	   at_x + PW->pixmap.mark.to_x - PW->pixmap.mark.from_x,
	   at_y + PW->pixmap.mark.to_y - PW->pixmap.mark.from_y);

    DestroyPixmapImage(&storage);
    DestroyMaskImage(&mask_storage);
  }
}

#if NeedFunctionPrototypes
void PWStoreToBuffer(Widget w)
#else
void PWStoreToBuffer(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  /* first check available space and resize if necessary */
  if ((PW->pixmap.image->width != PW->pixmap.buffer->width) ||
      (PW->pixmap.image->height != PW->pixmap.buffer->height))
    {
      XImage *buffer = CreatePixmapImage(PW, PW->pixmap.image->width, 
					 PW->pixmap.image->height);
      XImage *mask_buffer = CreateMaskImage(PW, PW->pixmap.mask_image->width, 
					    PW->pixmap.mask_image->height);
      
      DestroyPixmapImage(&PW->pixmap.buffer);
      DestroyMaskImage(&PW->pixmap.mask_buffer);
      PW->pixmap.buffer = buffer;
      PW->pixmap.mask_buffer = mask_buffer;
    }
  
  CopyImageData(PW->pixmap.image, PW->pixmap.buffer, 0, 0, 
		PW->pixmap.image->width-1, PW->pixmap.image->height-1, 0, 0);
  CopyImageData(PW->pixmap.mask_image, PW->pixmap.mask_buffer, 0, 0, 
		PW->pixmap.mask_image->width-1,
		PW->pixmap.mask_image->height-1, 0, 0);
  
  PW->pixmap.buffer_hot  = PW->pixmap.hot;
  PW->pixmap.buffer_mark = PW->pixmap.mark;

  /* save extensions to undo buffer */
  XpmFreeExtensions( PW->pixmap.buffer_extensions, 
                     PW->pixmap.buffer_nextensions );
  CopyExtensions( &PW->pixmap.buffer_extensions, 
                  &PW->pixmap.buffer_nextensions,
                   PW->pixmap.extensions,
                   PW->pixmap.nextensions );
}

#if NeedFunctionPrototypes
void PWUnmark(Widget w)
#else
void PWUnmark(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  PW->pixmap.buffer_mark = PW->pixmap.mark;
  
  if (QuerySet(PW->pixmap.mark.from_x, PW->pixmap.mark.from_y)) {
    XFillRectangle(dpy, XtWindow(PW), PW->pixmap.highlighting_gc,
		   InWindowX(PW, PW->pixmap.mark.from_x), 
		   InWindowY(PW, PW->pixmap.mark.from_y), 
		   InWindowX(PW, PW->pixmap.mark.to_x + 1) - 
		   InWindowX(PW, PW->pixmap.mark.from_x),
		   InWindowY(PW, PW->pixmap.mark.to_y + 1) -
		   InWindowY(PW, PW->pixmap.mark.from_y));
    
    PW->pixmap.mark.from_x = PW->pixmap.mark.from_y = NotSet;
    PW->pixmap.mark.to_x = PW->pixmap.mark.to_y = NotSet;
  }
}

#if NeedFunctionPrototypes
void PWMark(Widget w, Position from_x, Position from_y, 
	    Position to_x, Position to_y)
#else
void PWMark(w, from_x, from_y, to_x, to_y)
     Widget w;
     Position from_x, from_y, to_x, to_y;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  PWUnmark(w);
  
  if (QuerySet(from_x, from_y)) {
    if ((from_x == to_x) && (from_y == to_y)) {
      /*
	PW->pixmap.mark.from_x = 0;
	PW->pixmap.mark.from_y = 0;
	PW->pixmap.mark.to_x = PW->pixmap.image->width - 1;
	PW->pixmap.mark.to_y = PW->pixmap.image->height - 1;
	*/
      return;
    }
    else {
      QuerySwap(from_x, to_x);
      QuerySwap(from_y, to_y);
      from_x = max(0, from_x);
      from_y = max(0, from_y);
      to_x = min(PW->pixmap.image->width - 1, to_x);
      to_y = min(PW->pixmap.image->height - 1, to_y);
      
      PW->pixmap.mark.from_x = from_x;
      PW->pixmap.mark.from_y = from_y;
      PW->pixmap.mark.to_x = to_x;
      PW->pixmap.mark.to_y = to_y;
    }
    
    XFillRectangle(dpy, XtWindow(PW), PW->pixmap.highlighting_gc,
		   InWindowX(PW, PW->pixmap.mark.from_x),
		   InWindowY(PW, PW->pixmap.mark.from_y), 
		   InWindowX(PW, PW->pixmap.mark.to_x + 1) -
		   InWindowX(PW, PW->pixmap.mark.from_x),
		   InWindowY(PW, PW->pixmap.mark.to_y +1) - 
		   InWindowY(PW, PW->pixmap.mark.from_y));
  }
}

#if NeedFunctionPrototypes
void PWUndo(Widget w)
#else
void PWUndo(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  Position x, y;
  int tmp_nextensions;
  XpmExtension *tmp_extensions;
  XPoint tmp_hot;
  XImage *tmp_image, *mask_tmp_image;
  PWArea tmp_mark;
  Pixel color;
  
  tmp_image = PW->pixmap.image;
  mask_tmp_image = PW->pixmap.mask_image;
  PW->pixmap.image = PW->pixmap.buffer;
  PW->pixmap.mask_image = PW->pixmap.mask_buffer;
  PW->pixmap.buffer = tmp_image;
  PW->pixmap.mask_buffer = mask_tmp_image;

  tmp_hot = PW->pixmap.hot;
  tmp_mark = PW->pixmap.mark;
  PW->pixmap.mark = PW->pixmap.buffer_mark;
  PW->pixmap.buffer_mark= tmp_mark;

  /* erase extensions then save */
  /* This part is commented now cause it seems to be useless */
/*  if (PW->pixmap.redrawCallback)
      PW->pixmap.redrawCallback( w, Clear ); */
  tmp_nextensions = PW->pixmap.nextensions;
  tmp_extensions  = PW->pixmap.extensions;
  PW->pixmap.nextensions = PW->pixmap.buffer_nextensions;
  PW->pixmap.extensions  = PW->pixmap.buffer_extensions;
  PW->pixmap.buffer_nextensions = tmp_nextensions;
  PW->pixmap.buffer_extensions  = tmp_extensions;

  if ((PW->pixmap.image->width != PW->pixmap.buffer->width) ||
      (PW->pixmap.image->height != PW->pixmap.buffer->height))
    {
      PW->pixmap.width = PW->pixmap.image->width;
      PW->pixmap.height = PW->pixmap.image->height;
      InternalResize(PW);
    }
  else
    {
      for (x = 0; x < PW->pixmap.image->width; x++)
	for (y = 0; y < PW->pixmap.image->height; y++)
	  if (GetPxlFromImageAndMask(PW->pixmap.buffer, PW->pixmap.mask_buffer,
				     x, y) != (color = GetPxl(PW, x, y)))
	    {
	      DrawPoint(PW, x, y, color, NotLazy);
	    }
  
      /* Now treating Undo copy seperatly :
	 when mark is set and is different from buffer one, redraw them */
      if ((QuerySet(PW->pixmap.mark.from_x, PW->pixmap.mark.from_y)) &&
	  (QuerySet(PW->pixmap.buffer_mark.from_x, 
		    PW->pixmap.buffer_mark.from_y))
	  && ((PW->pixmap.mark.from_x != PW->pixmap.buffer_mark.from_x) ||
	      (PW->pixmap.mark.from_y != PW->pixmap.buffer_mark.from_y) ||
	      (PW->pixmap.mark.to_x != PW->pixmap.buffer_mark.to_x) ||
	      (PW->pixmap.mark.to_y != PW->pixmap.buffer_mark.to_y)))
	{
	  PWRedrawPoints((Widget) PW, PW->pixmap.mark.from_x, 
			 PW->pixmap.mark.from_y, 
			 PW->pixmap.mark.to_x, 
			 PW->pixmap.mark.to_y);
	  PWRedrawPoints((Widget) PW, PW->pixmap.buffer_mark.from_x, 
			 PW->pixmap.buffer_mark.from_y, 
			 PW->pixmap.buffer_mark.to_x, 
			 PW->pixmap.buffer_mark.to_y);
	}
    }

  PWSetHotSpot(w, PW->pixmap.buffer_hot.x, PW->pixmap.buffer_hot.y);
  PW->pixmap.buffer_hot = tmp_hot;

  /* This part is commented now cause it seems to be useless */
/*  if (PW->pixmap.redrawCallback)
      PW->pixmap.redrawCallback( w, Set );*/
}


#if NeedFunctionPrototypes
void PWRotateBufferAndImage(Widget w)
#else
void PWRotateBufferAndImage(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  XPoint tmp_hot;
  XImage *tmp_image, *mask_tmp_image;
  Boolean SetHotSpot = False;

  if (QuerySet(PW->pixmap.mark.from_x, PW->pixmap.mark.from_y)) {
      /* Rotate Image and marked area */
      Dimension width, height;
      XImage *storage, *mask_storage;

      width = PW->pixmap.mark.to_x - PW->pixmap.mark.from_x + 1;
      height = PW->pixmap.mark.to_y - PW->pixmap.mark.from_y + 1;
      
      storage = CreatePixmapImage(PW, width, height);
      mask_storage = CreateMaskImage(PW, width, height);
      
      CopyImageData(PW->pixmap.image, storage,
		    PW->pixmap.mark.from_x,  PW->pixmap.mark.from_y,
		    PW->pixmap.mark.to_x,  PW->pixmap.mark.to_y,
		    0, 0);
      
      CopyImageData(PW->pixmap.mask_image, mask_storage,
		    PW->pixmap.mark.from_x,  PW->pixmap.mark.from_y,
		    PW->pixmap.mark.to_x,  PW->pixmap.mark.to_y,
		    0, 0);
      tmp_image = PW->pixmap.image;
      mask_tmp_image = PW->pixmap.mask_image;
      tmp_hot = PW->pixmap.hot;
      PW->pixmap.image = storage;
      PW->pixmap.mask_image = mask_storage;

      /* reset all marked area as there are problems with undo */
      PW->pixmap.mark.from_x = PW->pixmap.mark.to_x = NotSet;
      PW->pixmap.mark.from_y = PW->pixmap.mark.to_y = NotSet;
      PW->pixmap.buffer_mark.from_x = PW->pixmap.buffer_mark.to_x = NotSet;
      PW->pixmap.buffer_mark.from_y = PW->pixmap.buffer_mark.to_y = NotSet;
  } else {
      /* Rotate Image and Cut/Insert buffer */
      SetHotSpot = True;
      tmp_image = PW->pixmap.image;
      mask_tmp_image = PW->pixmap.mask_image;
      tmp_hot = PW->pixmap.hot;
      PW->pixmap.image = PW->pixmap.storage;
      PW->pixmap.mask_image = PW->pixmap.mask_storage;
      PW->pixmap.storage = tmp_image;
      PW->pixmap.mask_storage = mask_tmp_image;
  }

  if ((PW->pixmap.image->width != tmp_image->width) ||
      (PW->pixmap.image->height != tmp_image->height)) {
      PW->pixmap.width = PW->pixmap.image->width;
      PW->pixmap.height = PW->pixmap.image->height;
      InternalResize(PW);
  }
  else PWRedraw(w);

  if (SetHotSpot) /* if storage taken from buffer */
    PWSetHotSpot(w, PW->pixmap.storage_hot.x, PW->pixmap.storage_hot.y);
  else {
      DestroyPixmapImage(&tmp_image);
      DestroyMaskImage(&mask_tmp_image);
  }
  
  PW->pixmap.storage_hot = tmp_hot;
}


#if NeedFunctionPrototypes
void PWHighlightAxes(Widget w)
#else
void PWHighlightAxes(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  XDrawLine(dpy, XtWindow(PW),
	    PW->pixmap.framing_gc,
	    InWindowX(PW, 0), 
	    InWindowY(PW, 0),
	    InWindowX(PW, PW->pixmap.width),
	    InWindowY(PW, PW->pixmap.height));
  XDrawLine(dpy, XtWindow(PW),
	    PW->pixmap.framing_gc,
	    InWindowX(PW, PW->pixmap.width),
	    InWindowY(PW, 0), 
	    InWindowX(PW, 0),
	    InWindowY(PW, PW->pixmap.height));
  if (PWQueryGrid((Widget)PW))
    {
      if ((((int)(PW->pixmap.height / 2.0)) * 2) != PW->pixmap.height)
	XDrawLine(dpy, XtWindow(PW),
		  PW->pixmap.framing_gc,
		  InWindowX(PW, 0),
		  InWindowY(PW, PW->pixmap.height / 2.0),
		  InWindowX(PW, PW->pixmap.width),
		  InWindowY(PW, PW->pixmap.height / 2.0));
      if ((((int)(PW->pixmap.width / 2.0)) * 2) != PW->pixmap.width)
	XDrawLine(dpy, XtWindow(PW),
		  PW->pixmap.framing_gc,
		  InWindowX(PW, PW->pixmap.width / 2.0),
		  InWindowY(PW, 0),
		  InWindowX(PW, PW->pixmap.width / 2.0),
		  InWindowY(PW, PW->pixmap.height));
    }
}

#if NeedFunctionPrototypes
void PWSetForeground(Widget w, Pixel color)
#else
void PWSetForeground(w,color)
     Widget w;
     Pixel color;
#endif     
{
  PixmapWidget PW = (PixmapWidget) w;
  
  XSetForeground(dpy,PW->pixmap.drawing_gc, color);
  PW->pixmap.foreground_pixel = color;
}



/* drag a rectangle representing the extent of the text */

#if NeedFunctionPrototypes
void PWDragText(Widget w, Position at_x, Position at_y, int value)
#else
void PWDragText(w, at_x, at_y, value)
     Widget	w;
     Position	at_x, at_y;	/* text origin */
     int        value;		/* unused */
#endif
{
/*
  PixmapWidget PW = (PixmapWidget) w;

  calculate text extent 
  translate from text origin to upper left of text extent 
  draw rectangle from upper left to lower right of text extent

  Dimension width  = PW->pixmap.text_rbearing - PW->pixmap.text_lbearing;
  Dimension height = PW->pixmap.text_ascent + PW->pixmap.text_descent;
  Position x = at_x + PW->pixmap.text_lbearing;
  Position y = at_y - PW->pixmap.text_ascent + 1;
  PWDrawRectangle(w, 
		  x, y,
                  x + width, y + height,
		  value);
*/

  PWDrawText(w, at_x, at_y, value);
}

#if NeedFunctionPrototypes
void PWDrawText(Widget w, Position at_x, Position at_y, int value)
#else
void PWDrawText(w, at_x, at_y, value)
     Widget	w;
     Position	at_x, at_y;	/* text origin */
     int        value;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;

  Pixmap     pixmap;
  XImage     *image;
  GC         gc;
  XGCValues  gcv;
  Position   x, y, i, j;
  Dimension  width, height;

  if ( ! PW->pixmap.text_string || 
       ! PW->pixmap.font_struct ||
       strlen(PW->pixmap.text_string) == 0 )
      return;

  /* 
     To draw text: 
     put text in tmp pixmap of depth 1 then get image 
     foreach pixel in image, if set draw into pixmapwidget 
  */

  /* text extent, add 1 pixel pad because fonts vary is how ascent is defined */
  width  = PW->pixmap.text_rbearing - PW->pixmap.text_lbearing + 2;
  height = PW->pixmap.text_ascent + PW->pixmap.text_descent + 2;

  /* create white pixmap size of text extent with depth one */
  pixmap = XCreatePixmap( dpy, XtWindow(PW), width, height, 1 );
  gcv.font       = PW->pixmap.font_struct->fid;
  gcv.function   = GXcopy;
  gcv.foreground = WhitePixel( dpy, screen );
  gcv.background = WhitePixel( dpy, screen );
  gc = XCreateGC( dpy, pixmap, 
                  GCFunction | GCFont | GCForeground | GCBackground, 
                  &gcv );
  XFillRectangle( dpy, pixmap, gc, 0, 0, width, height ); /* clear */
  
  /* draw text in pixmap */
  x = (PW->pixmap.text_lbearing < 0) ? - PW->pixmap.text_lbearing : 0;
  y = PW->pixmap.text_ascent /* - 1 */; /* include pad */
  XSetForeground( dpy, gc, BlackPixel(dpy, screen) );
  XDrawString( dpy, pixmap, gc, x, y,
               PW->pixmap.text_string, strlen(PW->pixmap.text_string) );

  /* get image of text */
  image = XGetImage(dpy, pixmap, 0, 0, width, height, -1/*allplanes*/,ZPixmap);

  /* translate (at_x,at_y) from text origin to upper left of text extent */
  x = at_x;
  y = at_y;
  x += PW->pixmap.text_lbearing;
  y -= PW->pixmap.text_ascent - 1;

  /* foreach set pixel in image, draw into pixmapwidget */
  for (i = 0; i < (Position)width; i++) {
      for (j = 0; j < (Position)height; j++) {

          if (!QueryInPixmap(PW, x + i, y + j)) continue;

          if ( XGetPixel(image, i, j) == BlackPixel(dpy, screen) ) {
              PWDrawPoint( (Widget) PW, x + i, y + j, value );
          }
      }
  }
  
  /* clean up */
  XFreePixmap( dpy, pixmap );
  XFreeGC( dpy, gc );
  XDestroyImage( image );
}
