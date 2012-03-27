/*
 * $Id: Pixmap.c,v 1.22 1996/09/02 08:32:37 lmallet Exp $
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
 * $XConsortium: Pixmap.c,v 1.12 90/06/09 20:19:28 dmatic Exp $
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

static char rcsid[] = "$Id: Pixmap.c,v 1.22 1996/09/02 08:32:37 lmallet Exp $";
#include <Xft/Xft.h>
#include <X11/IntrinsicP.h>
#include <X11/Xmu/Converters.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/cursorfont.h>
#define AVOID    /* Avoid some clashing prototyping in Pixmap.h */
#include "PixmapP.h"
    
#include <stdio.h>
#include <math.h>

#define XtStrlen(s)                   ((s) ? strlen(s) : 0)
#define abs(x)                        (((x) > 0) ? (x) : -(x))
#define min(x, y)                     (((x) < (y)) ? (x) : (y))
#define max(x, y)                     (((x) > (y)) ? (x) : (y))



static Boolean _PWDEBUG = False;
static unsigned int depth;
static int screen;
static Display *dpy;
static int max_ncolors = -1;

/* picked up from rgb.c of Xpm lib */
static char *rgb_fname = RGBF ;
#ifndef UNUSE_XPM
typedef struct {  /* rgb values and ascii names (from rgb text file) */
   int  r, g, b;  /* rgb values, range of 0 -> 65535 */
   char *name;    /* color mnemonic of rgb value */
} RgbName;
#define MAX_RGBNAMES 1024
#endif /* UNUSE_XPM */
static RgbName rgb_table[MAX_RGBNAMES];

extern int xpmReadRgbNames();
extern char *xpmGetRgbName();

#define DefaultGridTolerance 5
#define DefaultPixmapWidth   32
#define DefaultPixmapHeight  32
#define DefaultStippled      TRUE
#define DefaultGrid          TRUE
#define DefaultResize        TRUE
#define DefaultProportional  TRUE
#define DefaultAxes          FALSE
#define DefaultDistance      10
#define DefaultSquareSize    20

/* max display depth to use an array for color table */
#define MAX_DEPTH 8

static XtResource resources[] = {
#define offset(field) XtOffset(PixmapWidget, pixmap.field)
{XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
   offset(cursor), XtRString, "tcross"},
{XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(foreground_pixel), XtRString, XtDefaultForeground},
{XtNhighlight, XtCHighlight, XtRPixel, sizeof(Pixel),
     offset(highlight_pixel), XtRString, XtDefaultForeground},
{XtNframing, XtCFraming, XtRPixel, sizeof(Pixel),
     offset(framing_pixel), XtRString, XtDefaultForeground},
{XtNtransparent, XtCTransparent, XtRPixel, sizeof(Pixel),
     offset(transparent_pixel), XtRString, XtDefaultTransparent},
{XtNproportional, XtCProportional, XtRBoolean, sizeof(Boolean),
     offset(proportional), XtRImmediate, (XtPointer) DefaultProportional},
{XtNgrid, XtCGrid, XtRBoolean, sizeof(Boolean),
     offset(grid), XtRImmediate, (XtPointer) DefaultGrid},
{XtNgridTolerance, XtCGridTolerance, XtRDimension, sizeof(Dimension),
     offset(grid_tolerance), XtRImmediate, (XtPointer) DefaultGridTolerance},
{XtNstippled, XtCStippled, XtRBoolean, sizeof(Boolean),
     offset(stippled), XtRImmediate, (XtPointer) DefaultStippled},
{XtNaxes, XtCAxes, XtRBoolean, sizeof(Boolean),
     offset(axes), XtRImmediate, (XtPointer) DefaultAxes},
{XtNresize, XtCResize, XtRBoolean, sizeof(Boolean),
     offset(resize), XtRImmediate, (XtPointer) DefaultResize},
{XtNdistance, XtCDistance, XtRDimension, sizeof(Dimension),
     offset(distance), XtRImmediate, (XtPointer) DefaultDistance},
{XtNsquareSize, XtCSquareSize, XtRDimension, sizeof(Dimension),
     offset(squareH), XtRImmediate, (XtPointer) DefaultSquareSize},
{XtNsquareSize, XtCSquareSize, XtRDimension, sizeof(Dimension),
     offset(squareW), XtRImmediate, (XtPointer) DefaultSquareSize},
{XtNpixmapWidth, XtCPixmapWidth, XtRDimension, sizeof(Dimension),
     offset(width), XtRImmediate, (XtPointer) DefaultPixmapWidth},
{XtNpixmapHeight, XtCPixmapHeight, XtRDimension, sizeof(Dimension),
     offset(height), XtRImmediate, (XtPointer) DefaultPixmapHeight},
{XtNbutton1Action, XtCButton1Action, XtRInt, sizeof(int),
     offset(button_action[0]), XtRImmediate, (XtPointer) Set},
{XtNbutton2Action, XtCButton2Action, XtRInt, sizeof(int),
     offset(button_action[1]), XtRImmediate, (XtPointer) Invert},
{XtNbutton3Action, XtCButton3Action, XtRInt, sizeof(int),
     offset(button_action[2]), XtRImmediate, (XtPointer) Clear},
{XtNbutton4Action, XtCButton4Action, XtRInt, sizeof(int),
     offset(button_action[3]), XtRImmediate, (XtPointer) Clear},
{XtNbutton5Action, XtCButton5Action, XtRInt, sizeof(int),
     offset(button_action[4]), XtRImmediate, (XtPointer) Clear},
{XtNfilename, XtCFilename, XtRString, sizeof(String),
     offset(filename), XtRImmediate, (XtPointer) XtNscratch},
{XtNaddColorNtfyProc, XtCAddColorNtfyProc, XtRFunction, sizeof(AddColorNotifyProc), offset(AddColorNotify), XtRImmediate, (XtPointer) NULL},
{XtNextensionNtfyProc, XtCExtensionNtfyProc, XtRFunction, sizeof(ExtensionNotifyProc), offset(extensionNotify), XtRImmediate, (XtPointer) NULL},
{XtNstipple, XtCStipple, XtRBitmap, sizeof(Pixmap),
     offset(stipple), XtRImmediate, (XtPointer) XtUnspecifiedPixmap},
#undef offset
};

void PWTMark();
void PWTUnmark();
void PWTPaste();
void PWTSetColor();

static XtActionsRec actions[] =
{
{"mark",               (XtActionProc)PWTMark},
{"unmark",             (XtActionProc)PWTUnmark},
{"paste",              (XtActionProc)PWTPaste},
{"set-color",          (XtActionProc)PWTSetColor},
{"PW-debug",           (XtActionProc)PWDebug},
{"terminate",          (XtActionProc)PWTerminate},
{"store-to-buffer",    (XtActionProc)PWStoreToBuffer},
{"change-notify",      (XtActionProc)PWChangeNotify},
{"set-changed",        (XtActionProc)PWSetChanged},
{"up",                 (XtActionProc)PWUp},
{"down",               (XtActionProc)PWDown},
{"left",               (XtActionProc)PWLeft},
{"right",              (XtActionProc)PWRight},
{"fold",               (XtActionProc)PWFold},
{"flip-horiz",         (XtActionProc)PWFlipHoriz},
{"flip-vert",          (XtActionProc)PWFlipVert},
{"rotate-right",       (XtActionProc)PWRotateRight},
{"rotate-left",        (XtActionProc)PWRotateLeft},
{"set",                (XtActionProc)PWSet},
{"clear",              (XtActionProc)PWClear},
{"undo",               (XtActionProc)PWUndo},
{"redraw",             (XtActionProc)PWRedraw},
};

static char translations[] =
"\
 Ctrl<Btn1Down>:   mark()\n\
 Ctrl<Btn2Down>:   paste()\n\
 Ctrl<Btn3Down>:   unmark()\n\
 Shift<BtnUp>:    set-color()\n\
 Ctrl<Key>l:       redraw()\n\
 <Key>d:           PW-debug()\n\
 <Key>t:           terminate()\n\
 <Key>Up:          store-to-buffer()\
                   up()\
                   change-notify()\
                   set-changed()\n\
 <Key>Down:        store-to-buffer()\
                   down()\
                   change-notify()\
                   set-changed()\n\
 <Key>Left:        store-to-buffer()\
                   left()\
                   change-notify()\
                   set-changed()\n\
 <Key>Right:       store-to-buffer()\
                   right()\
                   change-notify()\
                   set-changed()\n\
 <Key>f:           store-to-buffer()\
                   fold()\
                   change-notify()\
                   set-changed()\n\
 <Key>h:           store-to-buffer()\
                   flip-horiz()\
                   change-notify()\
                   set-changed()\n\
 <Key>v:           store-to-buffer()\
                   flip-vert()\
                   change-notify()\
                   set-changed()\n\
 <Key>r:           store-to-buffer()\
                   rotate-right()\
                   change-notify()\
                   set-changed()\n\
 <Key>l:           store-to-buffer()\
                   rotate-left()\
                   change-notify()\
                   set-changed()\n\
 <Key>s:           store-to-buffer()\
                   set()\
                   change-notify()\
                   set-changed()\n\
 <Key>c:           store-to-buffer()\
                   clear()\
                   change-notify()\
                   set-changed()\n\
 <Key>u:           undo()\
                   change-notify()\
                   set-changed()\n\
";

Atom targets[] = {
    XA_BITMAP,
    XA_PIXMAP,
    XA_STRING,
};

#include "Requests.h"

static void ClassInitialize();
static void Initialize();
static void Redisplay();
static void Resize();
static int InternalResize();
static void Destroy();
static Boolean SetValues();
 
PixmapClassRec pixmapClassRec = {
{   /* core fields */
    /* superclass		*/	(WidgetClass) &coreClassRec,
    /* class_name		*/	"Pixmap",
    /* widget_size		*/	sizeof(PixmapRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	FALSE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	TRUE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL,
  },
  {
    /* targets                  */      targets,
    /* num_trets                */      XtNumber(targets),
    /* requests                 */      requests,
    /* num_requests             */      XtNumber(requests),
  }
};
 
WidgetClass pixmapWidgetClass = (WidgetClass) &pixmapClassRec;

/* function to retreive the white pixel from a given colormap,
   assuming it has already been alocated */
#if NeedFunctionPrototypes
static Pixel WhitePixelOfColormap(Display *dpy, Colormap cmap)
#else
static Pixel WhitePixelOfColormap(dpy, cmap)
    Display *dpy;
    Colormap cmap;
#endif
{
   XColor color;
   color.red = 0xffff;
   color.blue = 0xffff;
   color.green = 0xffff;
   XAllocColor(dpy, cmap, &color);
   XFreeColors(dpy, cmap, &color.pixel, 1, 0);
   return color.pixel;
}


#if NeedFunctionPrototypes
static void ResetColorUse(PWColorInfo *root)
#else
static void ResetColorUse(root) 
    PWColorInfo *root;
#endif
{
  if (root != NULL) {
    root->in_xpm = -1;
    ResetColorUse(root->left);
    ResetColorUse(root->right);
  }
}

#if NeedFunctionPrototypes
static void ResetPWColorsUse(PixmapWidget PW)
#else
static void ResetPWColorsUse(PW) 
    PixmapWidget PW;
#endif
{
    if (depth <= MAX_DEPTH) {
    /* colorTable is an array, pixel is the entry index */
      int i;

      for (i = 0 ; i < (1<<depth) + 1; i++)
	if (PW->pixmap.colorTable[i])
	  PW->pixmap.colorTable[i]->in_xpm = -1;
    } else {
      ResetColorUse(*(PW->pixmap.colorTable));
    }
}

#if NeedFunctionPrototypes
static PWColorInfo *GetColorWithPixel(PWColorInfo *root, Pixel pixel)
#else
static PWColorInfo *GetColorWithPixel(root, pixel) 
    PWColorInfo *root;
    Pixel pixel;
#endif
{
  if (root != NULL) {
    if (root->pixel == pixel)
      return root;
    else if (pixel < root->pixel)
      return GetColorWithPixel(root->left, pixel);
    else
      return GetColorWithPixel(root->right, pixel);
  } else
    return root;
}

#if NeedFunctionPrototypes
static PWColorInfo *AddColor(PWColorInfo *root, PWColorInfo *color)
#else
static PWColorInfo *AddColor(root, color) 
    PWColorInfo *root;
    PWColorInfo *color;
#endif
{
  if (root == NULL) {
    return color;
  } else {
    if (color->pixel < root->pixel) 
      root->left = AddColor(root->left, color);
    else
      root->right = AddColor(root->right, color);

    return root;
  }
}

#if NeedFunctionPrototypes
Boolean PWQueryGrid(Widget w)
#else
Boolean PWQueryGrid(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    return PW->pixmap.grid;
}

#if NeedFunctionPrototypes
Boolean PWQueryAxes(Widget w)
#else
Boolean PWQueryAxes(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    return PW->pixmap.axes;
}

#if NeedFunctionPrototypes
Boolean PWQueryStored(Widget w)
#else
Boolean PWQueryStored(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    return (PW->pixmap.storage != NULL);
}


#if NeedFunctionPrototypes
Pixel PWGetTransparentPixel(Widget w)
#else
Pixel PWGetTransparentPixel(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    return PW->pixmap.transparent_pixel;
}

#if NeedFunctionPrototypes
Pixmap PWGetStipplePixmap(Widget w)
#else
Pixmap PWGetStipplePixmap(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    if (PW->pixmap.stippled) return PW->pixmap.stipple;
    else return XtUnspecifiedPixmap;
}

#include "Extensions.c"
#include "Graphics.c"
#include "ReqMach.c"
#include "CutPaste.c"
#include "Handlers.c"

#if NeedFunctionPrototypes
void PWDebug(Widget w)
#else
void PWDebug(w)
    Widget w;
#endif
{
     _PWDEBUG ^= True;
     XSynchronize(dpy, _PWDEBUG);
}

void PWTSetColor(w, event)
     Widget w;
     XEvent *event;
{
  PixmapWidget PW = (PixmapWidget) w;
  
  PWSetForeground(w, 
		  PWGetPxl(w, InPixmapX(PW, event->xbutton.x), 
			   InPixmapY(PW, event->xbutton.y)));
  if (PW->pixmap.colorNotify)
    (*PW->pixmap.colorNotify)(w, PW->pixmap.foreground_pixel);
}

#if NeedFunctionPrototypes
void ColorNotify(Widget w, UseColorNotifyProc proc)
#else
void ColorNotify(w, proc)
     Widget w;
     UseColorNotifyProc proc;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  PW->pixmap.colorNotify = proc;
}

#if NeedFunctionPrototypes
void ExtensionNotify(Widget w, ExtensionNotifyProc proc)
#else
void ExtensionNotify (w, proc)
     Widget                     w;
     ExtensionNotifyProc        proc;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;

  PW->pixmap.extensionNotify = proc;
}

#if NeedFunctionPrototypes
void PWPickPixelDraw(Widget w, Position x, Position y, int value)
#else
void PWPickPixelDraw(w, x, y, value)
    Widget   w;
    Position x; 
    Position y;
    int      value;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
 
    if (PW->pixmap.pickPixelDraw) 
      (*PW->pixmap.pickPixelDraw)(w, x, y, value);
}


#if NeedFunctionPrototypes
void PWPickPixelComplete(Widget w, Position x, Position y, int value)
#else
void PWPickPixelComplete(w, x, y, value)
    Widget   w;
    Position x; 
    Position y;
    int      value;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
 
    if (PW->pixmap.pickPixelComplete) 
      (*PW->pixmap.pickPixelComplete)(w, x, y, value);
}


Pixmap GetPixmap(PW, image)
     PixmapWidget PW;
     XImage *image;
{
  Pixmap pix;
  XGCValues gcv;
  GC gc ;
  
  pix = XCreatePixmap(dpy, XtWindow((Widget) PW),
		      image->width, image->height, image->depth);
  gcv.function = GXcopy;
  gc = XCreateGC(dpy, pix, GCFunction, &gcv);
  
  XPutImage(dpy, pix, gc, image, 0, 0, 0, 0,
	    image->width, image->height);
  return(pix);
}
  
XImage *GetImage(PW, pixmap)
    PixmapWidget PW;
    Pixmap pixmap;
{
    Window root;
    int x, y;
    unsigned int width, height, border_width, depth;
    XImage *image;

    XGetGeometry(dpy, pixmap, &root, &x, &y,
		 &width, &height, &border_width, &depth);

    image = XGetImage(dpy, pixmap, x, y, width, height,
		      AllPlanes, ZPixmap);

    return image;
}

XImage *CreateMaskImage(PW, width, height)
    PixmapWidget PW;
    Dimension width, height;
{
  XImage *mask_image;

  mask_image = XCreateImage(dpy, DefaultVisual(dpy, screen), 1, ZPixmap, 0, 
			    NULL, width, height, 8, 0);
  if (!mask_image) 
  {
      XtAppWarning(XtWidgetToApplicationContext((Widget) PW),
		   "Pixmap error in creating XImage\n");
      exit(1);
  }
  else mask_image->data = XtCalloc(1,
				   mask_image->bytes_per_line *
				     mask_image->height);

  /* Initialize all pixels to 1: default mask is rectangular */ 
  XAddPixel(mask_image, 1);
  
  return mask_image;
}

XImage *CreatePixmapImage(PW, width, height)
    PixmapWidget PW;
    Dimension width, height;
{
  int bitmap_pad;
  XImage *image;
  Pixel wp;
 
  if (depth <= 8) bitmap_pad = 8;
  else if (depth <= 16) bitmap_pad = 16;
  else bitmap_pad = 32;
  
  image = XCreateImage(dpy,
		       DefaultVisual(dpy, screen),
		       depth,
		       ZPixmap, 0,
		       NULL, width, height,
		       bitmap_pad, 0);

  if (!image) 
    {
      XtAppWarning(XtWidgetToApplicationContext((Widget) PW),
		   "Pixmap error in creating XImage\n");
      exit(1);
    }
  else image->data = XtCalloc(1, image->bytes_per_line * height);
  wp = WhitePixelOfColormap(dpy, PW->core.colormap);
  if (wp != 0)			/* to clear the image, hope white is or 0 
				       or greater then black */
    XAddPixel(image, wp);	/* because image pixels are initialized to 0 */
  
  if (_PWDEBUG) {
    printf("bytes_per_line %d\n", image->bytes_per_line);
    printf("size data %d\n", image->bytes_per_line * height);
    printf("bitmap pad %d\n", image->bitmap_pad);
  }
     
  return image;
}

void DestroyPixmapImage(image)
    XImage **image;
{
    if (*image) 
      {
	XDestroyImage(*image);
	*image = NULL;
      }
}

#if NeedFunctionPrototypes
XImage *PWGetImage(Widget w)
#else
XImage *PWGetImage(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    return PW->pixmap.image;
}

#if NeedFunctionPrototypes
void PWChangeNotify(Widget w, XtPointer client_data, XtPointer call_data)
#else
void PWChangeNotify(w, client_data, call_data)
     Widget       w;
     XtPointer      client_data;
     XtPointer      call_data;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    if (PW->pixmap.notify)
	(*PW->pixmap.notify)(w, client_data, call_data);
}

#if NeedFunctionPrototypes
void Notify(Widget w, void (*proc)())
#else
void Notify(w, proc)
     Widget   w;
     void   (*proc)();
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    PW->pixmap.notify = proc;
}

#if NeedFunctionPrototypes
void PWSetChanged(Widget w)
#else
void PWSetChanged(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
	
    PW->pixmap.changed = True;
}

#if NeedFunctionPrototypes
Boolean PWQueryChanged(Widget w)
#else
Boolean PWQueryChanged(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
	
    return PW->pixmap.changed;
}

#if NeedFunctionPrototypes
void PWClearChanged(Widget w)
#else
void PWClearChanged(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    PW->pixmap.changed = False;
}

#if NeedFunctionPrototypes
void PWSelect(Widget w, Position from_x, Position from_y, 
	      Position to_x, Position to_y, Time time)
#else
void PWSelect(w, from_x, from_y, to_x, to_y, time)
    Widget w;
    Position from_x, from_y, to_x, to_y;
    Time time;
#endif
{
    PWMark(w, from_x, from_y, to_x, to_y);
    PWGrabSelection(w, time);
}

#if NeedFunctionPrototypes
void PWSwitchAxes(Widget w)
#else
void PWSwitchAxes(w)
     Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    PW->pixmap.axes ^= True;
    PWHighlightAxes(w);
}

#if NeedFunctionPrototypes
void PWAxes(Widget w, Boolean _switch)
#else
void PWAxes(w, _switch)
    Widget w;
    Boolean _switch;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    if (PW->pixmap.axes != _switch)
	PWSwitchAxes(w);
}

#if NeedFunctionPrototypes
void PWRedrawAxes(Widget w)
#else
void PWRedrawAxes(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    if (PW->pixmap.axes)
	PWHighlightAxes(w);
}

#if NeedFunctionPrototypes
void PWPutImage(Widget w, Display *display, Drawable drawable, GC gc, 
		Position x, Position y)
#else
void PWPutImage(w, display, drawable, gc, x, y)
     PixmapWidget w;
     Display     *display;
     Drawable     drawable;
     GC           gc;
     Position     x, y;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

  XPutImage(display, drawable, gc, PW->pixmap.image,
	    0, 0, x, y, PW->pixmap.image->width, PW->pixmap.image->height);
}


String StripFilename(filename)
    String filename;
{
    char *begin = rindex (filename, '/');
    char *end, *result;
    int length;
    
    if (filename) {
	begin = (begin ? begin + 1 : filename);
	end = index (begin, '.'); /* change to rindex to allow longer names */
	length = (end ? (end - begin) : strlen (begin));
	result = (char *) XtMalloc (length + 1);
	strncpy (result, begin, length);
	result [length] = '\0';
	return (result);
    }
    else
	return (XtNdummy);
}


void SetTransparentPixels(pw, image, mask_image)
     PixmapWidget pw;
     XImage *image, *mask_image;
{
    register Position x, y;
    
  for (x = 0; x < image->width; x++)
    for (y = 0; y < image->height; y++)
      if (XGetPixel(mask_image, x, y) == 0)
	XPutPixel(image, x, y, pw->pixmap.transparent_pixel);
}

static void InitializeXpmAttributes(PW, attribs)
     PixmapWidget PW;
     XpmAttributes *attribs;
{
    attribs->valuemask = XpmVisual | XpmColormap | XpmDepth;
    attribs->visual = DefaultVisual(dpy, screen);
    attribs->colormap = PW->core.colormap;
    attribs->depth = depth;
    attribs->width = 0;
    attribs->height = 0;
    attribs->x_hotspot = 0;
    attribs->y_hotspot = 0;
    attribs->cpp = 0;
    attribs->pixels = (Pixel *)0;
    attribs->npixels = 0;
    attribs->colorsymbols = (XpmColorSymbol *)0;
    attribs->numsymbols = 0;
    attribs->rgb_fname = rgb_fname;
    attribs->nextensions = 0;
    attribs->extensions = (XpmExtension *)0;
    attribs->ncolors = 0;
    attribs->colorTable = (XpmColor *)0;
    attribs->hints_cmt = NULL;
    attribs->colors_cmt = NULL;
    attribs->pixels_cmt = NULL;
    attribs->closeness = 0;
    attribs->exactColors = 1;
}

void InitializeXpmInfo(infos)
     XpmInfo *infos;
{
    infos->hints_cmt = NULL;
    infos->colors_cmt = NULL;
    infos->pixels_cmt = NULL;
    infos->nextensions = 0;
    infos->extensions = NULL;
}

void InitializeXpmImage(xpm_image)
     XpmImage *xpm_image;
{
    xpm_image->width = 0;
    xpm_image->height = 0;
    xpm_image->cpp = 0;
    xpm_image->ncolors = 0;
    xpm_image->colorTable = (XpmColor *)0;
    xpm_image->data = (unsigned int *)0;
}

#if NeedFunctionPrototypes
void PWGetUnzoomedPixmap(Widget w, Pixmap *pixmap, Pixmap *pixmap_mask)
#else
void PWGetUnzoomedPixmap(w, pixmap, pixmap_mask)
     Widget w;
     Pixmap *pixmap, *pixmap_mask;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    XImage *mask_image, *image;
    
    if (PW->pixmap.zooming) 
      {    
	image = CreatePixmapImage(PW,
				  (Dimension) PW->pixmap.zoom.image->width,
				  (Dimension) PW->pixmap.zoom.image->height);
	CopyImageData(PW->pixmap.zoom.image, image, 0, 0, 
		      (Dimension) PW->pixmap.zoom.image->width,
		      (Dimension) PW->pixmap.zoom.image->height, 0, 0);
	CopyImageData(PW->pixmap.image, image, 
		      0, 0, 
		      PW->pixmap.image->width - 1,
		      PW->pixmap.image->height - 1,
		      PW->pixmap.zoom.at_x, PW->pixmap.zoom.at_y);

	mask_image = CreateMaskImage(PW,
				     (Dimension)PW->pixmap.zoom.image->width,
				     (Dimension)PW->pixmap.zoom.image->height);
	CopyImageData(PW->pixmap.zoom.mask_image, mask_image, 0, 0, 
		      (Dimension) PW->pixmap.zoom.mask_image->width,
		      (Dimension) PW->pixmap.zoom.mask_image->height, 0, 0);
	CopyImageData(PW->pixmap.mask_image, mask_image, 
		      0, 0, 
		      PW->pixmap.mask_image->width - 1,
		      PW->pixmap.mask_image->height - 1,
		      PW->pixmap.zoom.at_x, PW->pixmap.zoom.at_y);
	
      }
    else
      {
	  image = PW->pixmap.image;
	  mask_image = PW->pixmap.mask_image;
      }

    
    *pixmap = GetPixmap(PW, image);
    *pixmap_mask = GetPixmap(PW, mask_image);

    if (PW->pixmap.zooming) {
	DestroyPixmapImage(&image);
	DestroyMaskImage(&mask_image);
    }
}


void Refresh();

static void ClassInitialize()
{
  static XtConvertArgRec screenConvertArg[] = {
  {XtWidgetBaseOffset, (XtPointer) XtOffset(Widget, core.screen),
       sizeof(Screen *)}, 
  };

  XtAddConverter(XtRString, XtRBitmap, XmuCvtStringToBitmap,
		     screenConvertArg, XtNumber(screenConvertArg));

}

#if NeedFunctionPrototypes
static int readFile(PixmapWidget PW, String filename)
#else
static int readFile(PW, filename)
    PixmapWidget PW;
    String filename;
#endif
{
    int status, i, shift = 0;
    XImage *image, *mask_image;
    XpmAttributes attribs;
    Pixel *pixel;
    XpmInfo infos;
    XpmImage xpm_image;
    XpmColor *color;

    InitializeXpmInfo(&infos);
    InitializeXpmImage(&xpm_image);
    InitializeXpmAttributes(PW, &attribs);
    infos.valuemask = XpmReturnComments | XpmReturnColorTable
      | XpmReturnExtensions;
    attribs.valuemask |= XpmReturnPixels;

    status = XpmReadFileToXpmImage(filename, &xpm_image, &infos);
    if (status == XpmSuccess)
      status = XpmCreateImageFromXpmImage(dpy, &xpm_image, &image,
					  &mask_image, &attribs);
    if (status == XpmSuccess) {
	/* Notify colors to be loaded with this pixmap */
	for (i = 0, color = xpm_image.colorTable, pixel = attribs.pixels;
	     i < xpm_image.ncolors; i++, color++, pixel++)
	  {
	      /* look for a defined color value */
	      char *c_name;
	      char **defaults = (char **)color;
	      int default_index, def_index;

	      switch (depth)
		  {
		  case 1:
		      default_index = 2;
		      break;
		  case 4:
		      default_index = 3;
		      break;
		  case 6:
		      default_index = 4;
		      break;
		  case 8:
		  default:
		      default_index = 5;
		      break;
		  }
	      def_index = default_index;
	      while (def_index > 1 && defaults[def_index] == NULL)
		  --def_index;
	      if (def_index < 2) {/* nothing towards mono, so try
				   * towards color */
		  def_index = default_index + 1;
		  while (def_index <= 5 && defaults[def_index] == NULL)
		      ++def_index;
	      }
	      if (def_index > 5) {
		  fprintf(stderr, "color #%d undefined, use black.\n", i + 1);
		  c_name = "black";
	      } else
		  c_name = defaults[def_index];

	      if (_PWDEBUG) {
		  printf("Pixel %d\n", *(pixel-shift));
		  printf("Color name %s\n", c_name);
		  printf("colorTable[%d][str] %s\n", i, color->string);
		  printf("colorTable[%d][s] %s\n", i, color->symbolic);
		  printf("colorTable[%d][m] %s\n", i, color->m_color);
		  printf("colorTable[%d][g4] %s\n", i, color->g4_color);
		  printf("colorTable[%d][g] %s\n", i, color->g_color);
		  printf("colorTable[%d][c] %s\n", i, color->c_color);
	      }
	      
	      if (strcasecmp(c_name, "none") == 0) 
	      {
		  PWUpdateColorInTable((Widget)PW, 
				       TRANSPARENT(dpy, screen), 
				       color->string, 
				       color->symbolic,
				       color->m_color,
				       color->g4_color,
				       color->g_color,
				       color->c_color);
		shift++;
	      }
	      else 
	      {
		  PWUseColorInTable((Widget)PW, 
				    *(pixel-shift),
				    color->string, 
				    color->symbolic,
				    color->m_color,
				    color->g4_color,
				    color->g_color,
				    color->c_color);
	      }
	  }	      

	if (PW->pixmap.hints_cmt) XtFree(PW->pixmap.hints_cmt);
	if (infos.hints_cmt) 
	  PW->pixmap.hints_cmt = XtNewString(infos.hints_cmt);
	else
	  PW->pixmap.hints_cmt = NULL;

	if (PW->pixmap.colors_cmt) XtFree(PW->pixmap.colors_cmt);
	if (infos.colors_cmt) 
	  PW->pixmap.colors_cmt = XtNewString(infos.colors_cmt);
	else
	  PW->pixmap.colors_cmt = NULL;

	if (PW->pixmap.pixels_cmt) XtFree(PW->pixmap.pixels_cmt);
	if (infos.pixels_cmt) 
	  PW->pixmap.pixels_cmt = XtNewString(infos.pixels_cmt);
	else
	  PW->pixmap.pixels_cmt = NULL;

        if (!mask_image) /* Xpm returns NULL when transp. is not used */
	  mask_image = CreateMaskImage(PW, image->width, image->height);
        else SetTransparentPixels(PW, image, mask_image);
      
	DestroyPixmapImage(&PW->pixmap.buffer);
	DestroyMaskImage(&PW->pixmap.mask_buffer);

	PW->pixmap.buffer = PW->pixmap.image;
        PW->pixmap.mask_buffer = PW->pixmap.mask_image;

	PW->pixmap.image = image;
        PW->pixmap.mask_image = mask_image;
	PW->pixmap.width = xpm_image.width;
	PW->pixmap.height = xpm_image.height;
      
        if (infos.valuemask & XpmHotspot)
	  {
	    PW->pixmap.hot.x = infos.x_hotspot;
	    PW->pixmap.hot.y = infos.y_hotspot;	
	  }
        else
	  {
	    PW->pixmap.hot.x = PW->pixmap.hot.y = NotSet;
	  }

        CopyExtensions( &PW->pixmap.extensions, &PW->pixmap.nextensions,
                        infos.extensions, infos.nextensions );
        if (PW->pixmap.extensionNotify)
            PW->pixmap.extensionNotify ((Widget) PW);

	PW->pixmap.changed = False;
	PW->pixmap.zooming = False;
	XpmFreeAttributes(&attribs);
	XpmFreeXpmImage(&xpm_image);
	XpmFreeXpmInfo(&infos);
    }
    return (status);
}

static void Initialize(request, new, argv, argc)
    PixmapWidget request, new;
    ArgList argv;
    Cardinal argc;
{
    XGCValues  values;
    XtGCMask   mask;

    dpy = XtDisplay(new);
    screen = DefaultScreen(dpy);
    depth = DefaultDepth(dpy,screen);

    if (max_ncolors == -1)
      max_ncolors = xpmReadRgbNames(rgb_fname, rgb_table);

    /* allocate max colors + 1 colorTable entries because 0 is transparent */
    if (depth <= MAX_DEPTH) {
      new->pixmap.colorTable = (PWColorInfo **) XtCalloc((1<<depth) + 1, 
							 sizeof(PWColorInfo*));
    } else {
      new->pixmap.colorTable = XtNew(PWColorInfo *);
      *(new->pixmap.colorTable) = NULL;
    }
    
    new->pixmap.notify = NULL;
    new->pixmap.cardinal = 0;
    new->pixmap.current = 0;
    new->pixmap.fold = False;
    new->pixmap.changed = False;
    new->pixmap.zooming = False;
    new->pixmap.selection.own = False;
    new->pixmap.selection.limbo = False;
    new->pixmap.clear_pixel = WhitePixelOfColormap(dpy, new->core.colormap);
    new->pixmap.hints_cmt = NULL;
    new->pixmap.colors_cmt = NULL;
    new->pixmap.pixels_cmt = NULL;
    new->pixmap.filename = XtNewString(new->pixmap.filename);
    new->pixmap.status = NULL;

    new->pixmap.request_stack = (PWRequestStack *)
	XtMalloc(sizeof(PWRequestStack));
    new->pixmap.request_stack[0].request = NULL;
    new->pixmap.request_stack[0].call_data = NULL;
    new->pixmap.request_stack[0].trap = False;

    new->core.width = new->pixmap.width * new->pixmap.squareW + 
	2 * new->pixmap.distance;
    new->core.height = new->pixmap.height * new->pixmap.squareH + 
	2 * new->pixmap.distance;

    new->pixmap.extensions = NULL;
    new->pixmap.nextensions = 0;
    new->pixmap.buffer_extensions = NULL;
    new->pixmap.buffer_nextensions = 0;

    new->pixmap.text_string = NULL;
    new->pixmap.font_struct = NULL;
/*
    PWSetFont(new, "-*-fixed-medium-*-*-*-13-*-*-*-*-70-*-*" );
*/
  
    new->pixmap.hot.x = new->pixmap.hot.y = NotSet;
    new->pixmap.buffer_hot.x = new->pixmap.buffer_hot.y = NotSet;
    
    new->pixmap.mark.from_x = new->pixmap.mark.from_y = NotSet;
    new->pixmap.mark.to_x = new->pixmap.mark.to_y = NotSet;
    new->pixmap.buffer_mark.from_x = new->pixmap.buffer_mark.from_y = NotSet;
    new->pixmap.buffer_mark.to_x = new->pixmap.buffer_mark.to_y = NotSet;

    values.foreground = new->pixmap.foreground_pixel;
    values.background = new->core.background_pixel;
    values.foreground ^= values.background;
    values.function = GXcopy;
    values.plane_mask = AllPlanes;
    mask = GCForeground | GCBackground | GCFunction | GCPlaneMask;
    new->pixmap.drawing_gc = XCreateGC(dpy, RootWindow(dpy,screen), 
				       mask, &values);

    values.foreground = new->pixmap.highlight_pixel;
    values.background = new->core.background_pixel;
    values.foreground ^= values.background;
    values.function = GXxor;
    values.plane_mask = AllPlanes;
    mask = GCForeground | GCBackground | GCFunction | GCPlaneMask;
    new->pixmap.highlighting_gc = XCreateGC(dpy, RootWindow(dpy, screen),
					    mask, &values);

    values.foreground = new->pixmap.framing_pixel;
    values.background = new->core.background_pixel;
    values.foreground ^= values.background;
    values.function = GXxor;
    values.plane_mask = AllPlanes;
    mask = GCForeground | GCBackground | GCFunction | GCPlaneMask;
    new->pixmap.framing_gc = XCreateGC(dpy, RootWindow(dpy, screen),
				       mask, &values);

    values.foreground = new->pixmap.transparent_pixel;
    values.background = new->pixmap.clear_pixel;
    values.function = GXcopy;
    mask = GCForeground | GCBackground | GCFunction;
    if (new->pixmap.stipple != XtUnspecifiedPixmap)
      {
	values.stipple = new->pixmap.stipple;
	mask |= GCStipple | GCFillStyle;
      }
    values.fill_style = (new->pixmap.stippled ? 
			 FillOpaqueStippled : FillSolid);
    new->pixmap.transparent_gc = XCreateGC(dpy, RootWindow(dpy, screen),
					   mask, &values);
    
    new->pixmap.storage = NULL;
    new-> pixmap.mask_storage = NULL;
    
    new->pixmap.image = CreatePixmapImage(new, 
					  new->pixmap.width,
					  new->pixmap.height);
    new->pixmap.mask_image = CreateMaskImage(new,
					     new->pixmap.width,
					     new->pixmap.height);
    
    new->pixmap.buffer = CreatePixmapImage(new, 
					   new->pixmap.width,
					   new->pixmap.height);
    new->pixmap.mask_buffer = CreateMaskImage(new, 
					      new->pixmap.width,
					      new->pixmap.height);
    
    /* add transparent pixel in ColorTable */
    PWUseColorInTable((Widget)new, TRANSPARENT(dpy, screen), NULL,
		      NULL, NULL, NULL, NULL, NoColorName);
    
    /* add clear pixel in ColorTable */
    PWUseColorInTable((Widget)new, new->pixmap.clear_pixel, NULL,
		      NULL, NULL, NULL, NULL, "white");
    
    /* Read file */
    readFile(new, new->pixmap.filename);

    InternalResize(new);
    Resize(new);
}

#if NeedFunctionPrototypes
Boolean PWQueryMarked(Widget w)
#else
Boolean PWQueryMarked(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    return QuerySet(PW->pixmap.mark.from_x, PW->pixmap.mark.from_y);
}

#if NeedFunctionPrototypes
Boolean PWQueryStippled(Widget w)
#else
Boolean PWQueryStippled(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  return(PW->pixmap.stippled);
}

void FixMark(PW)
    PixmapWidget PW;
{
    if (QuerySet(PW->pixmap.mark.from_x, PW->pixmap.mark.from_y)) {
	PW->pixmap.mark.from_x = min(PW->pixmap.mark.from_x, 
				     PW->pixmap.image->width);
	PW->pixmap.mark.from_y = min(PW->pixmap.mark.from_y, 
				     PW->pixmap.image->height);
	PW->pixmap.mark.to_x = min(PW->pixmap.mark.to_x, 
				   PW->pixmap.image->width);
	PW->pixmap.mark.to_y = min(PW->pixmap.mark.to_y, 
				   PW->pixmap.image->height);
	
	if((PW->pixmap.mark.from_x == PW->pixmap.mark.from_y) &&
	   (PW->pixmap.mark.to_x   == PW->pixmap.mark.to_y))
	    PW->pixmap.mark.from_x = 
		PW->pixmap.mark.from_y =
		    PW->pixmap.mark.to_x = 
			PW->pixmap.mark.to_y = NotSet;
    }
}

#if NeedFunctionPrototypes
int PWStoreFile(Widget w, String filename)
#else
int PWStoreFile(w, filename)
    Widget w;
    String filename;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    int status, i, shift = 0;
    XImage *image, *mask_image;
    XpmAttributes attribs;
    Pixel *pixel;
    XpmInfo infos;
    XpmImage xpm_image;
    XpmColor *color;

    InitializeXpmInfo(&infos);
    InitializeXpmImage(&xpm_image);
    InitializeXpmAttributes(PW, &attribs);

    infos.valuemask = XpmReturnColorTable;
    attribs.valuemask |= XpmReturnPixels;

    if ((status = XpmReadFileToXpmImage(filename, &xpm_image, &infos))
	== XpmSuccess)
      status = XpmCreateImageFromXpmImage(dpy, &xpm_image, &image,
					  &mask_image, &attribs);
    if (status == XpmSuccess) {
	/* Notify colors to be loaded with this pixmap */
	for (i = 0, color = xpm_image.colorTable, pixel = attribs.pixels;
	     i < xpm_image.ncolors; i++, color++, pixel++)
	{
	    char *c_name;
		    
	    switch (depth)
	      {
	      case 1:
		  c_name = color->m_color;
		  break;
	      case 4:
		  c_name = color->g4_color;
		  break;
	      case 6:
		  c_name = color->g_color;
		  break;
	      case 8:
	      default:
		  c_name = color->c_color;
		  break;
	      }
	    
	    if (_PWDEBUG) {
		printf("Pixel %d\n", *(pixel-shift));
		printf("Color name %s\n", c_name);
		printf("colorTable[%d][str] %s\n", i, color->string);
		printf("colorTable[%d][s] %s\n", i, color->symbolic);
		printf("colorTable[%d][m] %s\n", i, color->m_color);
		printf("colorTable[%d][g4] %s\n", i, color->g4_color);
		printf("colorTable[%d][g] %s\n", i, color->g_color);
		printf("colorTable[%d][c] %s\n", i, color->c_color);
	    }
	    
	    if (strcasecmp(c_name, "none") == 0) 
	    {
		PWUpdateColorInTable((Widget)PW, 
				     TRANSPARENT(dpy, screen), 
				     color->string, 
				     color->symbolic,
				     color->m_color,
				     color->g4_color,
				     color->g_color,
				     color->c_color);
		shift = 1;
	    }
	    else 
	    {
		PWUseColorInTable((Widget)PW, 
				  *(pixel-shift),
				  color->string, 
				  color->symbolic,
				  color->m_color,
				  color->g4_color,
				  color->g_color,
				  color->c_color);
	    }
	}

        if (!mask_image) /* Xpm returns NULL when transp. is not used */
	  mask_image = CreateMaskImage(PW, image->width, image->height);
        else SetTransparentPixels(PW, image, mask_image);
	
        DestroyPixmapImage(&PW->pixmap.storage);
        DestroyMaskImage(&PW->pixmap.mask_storage);

        PW->pixmap.storage = image;
        PW->pixmap.mask_storage = mask_image;
    
        if (infos.valuemask & XpmHotspot)
	  {
	    PW->pixmap.storage_hot.x = infos.x_hotspot;
	    PW->pixmap.storage_hot.y = infos.y_hotspot;	
	  }
        else
	  {
	    PW->pixmap.storage_hot.x = PW->pixmap.storage_hot.y = NotSet;
	  }

	XpmFreeAttributes(&attribs);
	XpmFreeXpmImage(&xpm_image);
	XpmFreeXpmInfo(&infos);  
    }
    else
	XtAppWarning(XtWidgetToApplicationContext(w),
		     " read file failed.  PixmapWidget");
    
    return status;
}

#if NeedFunctionPrototypes
String PWUnparseStatus(Widget w)
#else
String PWUnparseStatus(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    char size_buf[80];
    char *status_ptr;
    
    sprintf(size_buf, "%dx%d", PW->pixmap.width, PW->pixmap.height);
    
    /* 16 is lenght of "Filename: " and " Size:"
       1 is NULL
       don't realloc pixmap.status since alloc might
       fail and we don't want to */
    if ((status_ptr = XtMalloc((strlen(size_buf) + 
			       strlen(PW->pixmap.filename) +
			       16 + 1) * sizeof(char))))
      {
	if (PW->pixmap.status)
	  XtFree(PW->pixmap.status);
	PW->pixmap.status = status_ptr;
	sprintf(PW->pixmap.status, 
		"Filename: %s Size:%dx%d",
		PW->pixmap.filename, PW->pixmap.width, PW->pixmap.height);
      }

    return PW->pixmap.status;
}

    
#if NeedFunctionPrototypes
void PWChangeFilename(Widget w, String str)
#else
void PWChangeFilename(w, str)
    Widget w;
    String str;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    if (str)
	if (strcmp(str, "")) {
	    XtFree(PW->pixmap.filename);
	    PW->pixmap.filename = XtNewString( str);
	}
}


/* Warning this function sets or gets the pixmap comments 
 * if passing null, a comment is returned, otherwise, it is stored 
 * memory is allocated for returned comments, should be free by application */
#if NeedFunctionPrototypes
void PWComments(Widget w, char **hints_cmt, char **colors_cmt, 
		char **pixels_cmt)
#else
void PWComments(w, hints_cmt, colors_cmt, pixels_cmt)
     Widget w;
     char **hints_cmt, **colors_cmt, **pixels_cmt;
#endif
{
  PixmapWidget PW = (PixmapWidget)w;
  
  if ((*hints_cmt) && (PW->pixmap.hints_cmt))
    {
      XtFree(PW->pixmap.hints_cmt);
      PW->pixmap.hints_cmt = XtNewString(*hints_cmt);
    }
  else if (*hints_cmt) PW->pixmap.hints_cmt = XtNewString(*hints_cmt);
  else *hints_cmt = XtNewString(PW->pixmap.hints_cmt);

  if ((*colors_cmt) && (PW->pixmap.colors_cmt))
    {
      XtFree(PW->pixmap.colors_cmt);
      PW->pixmap.colors_cmt = XtNewString(*colors_cmt);
    }
  else if (*colors_cmt) PW->pixmap.colors_cmt = XtNewString(*colors_cmt);
  else *colors_cmt = XtNewString(PW->pixmap.colors_cmt);

  if ((*pixels_cmt) && (PW->pixmap.pixels_cmt))
    {
      XtFree(PW->pixmap.pixels_cmt);
      PW->pixmap.pixels_cmt = XtNewString(*pixels_cmt);
    }
  else if (*pixels_cmt) PW->pixmap.pixels_cmt = XtNewString(*pixels_cmt);
  else *pixels_cmt = XtNewString(PW->pixmap.pixels_cmt);
}

  
#if NeedFunctionPrototypes
void PWAddColorNotifyProc(Widget w, AddColorNotifyProc proc)
#else
void PWAddColorNotifyProc(w, proc)
     Widget w;
     AddColorNotifyProc proc;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  PW->pixmap.AddColorNotify = proc;
}


#if NeedFunctionPrototypes
PWColorInfo **PWGetColorTable(Widget w)
#else
PWColorInfo **PWGetColorTable(w)
     Widget w;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  return(PW->pixmap.colorTable);
}

#if NeedFunctionPrototypes
PWColorInfo *PWGetColor(Widget w, Pixel pixel)
#else
PWColorInfo *PWGetColor(w, pixel)
     Widget w;
     Pixel pixel;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;

  if (depth <= MAX_DEPTH) {
    /* colorTable is an array, pixel is the entry index */
    /* index in colorTable is 0 for transparent and pixel+1 for others */
    int index = (pixel == TRANSPARENT(dpy, screen) ? 0 : pixel + 1);

    return (PW->pixmap.colorTable[index]);
  } else {
    /* colorTable is a B-tree, find proper entry recursively */
    return GetColorWithPixel(*(PW->pixmap.colorTable), pixel);
  }
}

#if NeedFunctionPrototypes
void PWAddColor(Widget w, PWColorInfo *color)
#else
void PWAddColor(w, color)
     Widget w;
     PWColorInfo *color;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;

  if (depth <= MAX_DEPTH) {
    /* colorTable is an array, pixel is the entry index */
    /* index in colorTable is 0 for transparent and pixel+1 for others */
    int index = (color->pixel == TRANSPARENT(dpy, screen) ? 0 : color->pixel + 1);

    PW->pixmap.colorTable[index] = color;
  } else {
    /* colorTable is a B-tree, find proper entry recursively */
    *(PW->pixmap.colorTable) = AddColor(*(PW->pixmap.colorTable), color);
  }
}

#if NeedFunctionPrototypes
void PWUseColorInTable(Widget w, Pixel pixel, char *symbol, char *sname, 
		       char *mname, char *g4name, char *gname, char *cname)
#else
void PWUseColorInTable(w, pixel, symbol, sname, mname, g4name, gname, cname)
     Widget w;
     Pixel pixel;
     char *symbol, *sname, *mname, *g4name, *gname, *cname;
#endif
     /* name are not used as is, instead memory is malloc'ed to fit in */
{
  PixmapWidget PW = (PixmapWidget) w;
  Boolean is_new_color = False;
  PWColorInfo *new_color;

  if (PWGetColor(w, pixel) == NULL)  /* not yet used color 
					probably not in colorTable */
    {
      is_new_color = True;

      new_color = XtNew(PWColorInfo);
      
      new_color->symbol = 0;
      new_color->s_name = 0;
      new_color->m_name = 0;
      new_color->g4_name = 0;
      new_color->g_name = 0;
      new_color->c_name = 0;
      new_color->pixel = pixel;
      new_color->left = NULL;
      new_color->right = NULL;

      PWAddColor(w, new_color);
    }
  
  PWUpdateColorInTable(w, pixel, symbol, sname, mname, g4name, 
		       gname, cname);

  if (is_new_color && 
      (PW->pixmap.AddColorNotify != (AddColorNotifyProc) 0))
    PW->pixmap.AddColorNotify(w, pixel, new_color->c_name);
}


#if NeedFunctionPrototypes
void PWUpdateColorInTable(Widget w, Pixel pixel, char *symbol, char *sname, 
		       char *mname, char *g4name, char *gname, char *cname)
#else
void PWUpdateColorInTable(w, pixel, symbol, sname, mname, g4name, gname, cname)
     Widget w;
     Pixel pixel;
     char *symbol, *sname, *mname, *g4name, *gname, *cname;
#endif
     /* name are not used as is, instead memory is malloc'ed to fit in */
{
  PixmapWidget PW = (PixmapWidget) w;
  PWColorInfo *color = PWGetColor(w, pixel);
  
  if (color == NULL) return; /* inexistent color in Table */
  /* Update the color info in ColorTable */
  if ((symbol) && ((!color->symbol) || (strcmp(symbol, color->symbol))))
    {
      if (color->symbol) 
	XtFree(color->symbol);
      color->symbol = XtNewString(symbol);
    }
  if ((sname) && ((!color->s_name) || (strcmp(sname, color->s_name))))
    {
      if (color->s_name) 
	XtFree(color->s_name);
      color->s_name = XtNewString(sname);
    }
  if ((mname) && ((!color->m_name) || (strcmp(mname, color->m_name))))
    {
      if (color->m_name) 
	XtFree(color->m_name);
      color->m_name = XtNewString(mname);
    }
  if ((g4name) && ((!color->g4_name) || (strcmp(g4name, color->g4_name))))
    {
      if (color->g4_name) 
	XtFree(color->g4_name);
      color->g4_name = XtNewString(g4name);
    }
  if ((gname) && ((!color->g_name) || (strcmp(gname, color->g_name))))
    {
      if (color->g_name) 
	XtFree(color->g_name);
      color->g_name = XtNewString(gname);
    }
  if ((cname) && ((!color->c_name) ||
		   ((strcmp(cname, color->c_name)) &&
		    ((cname[0] != '#') || (color->c_name[0] == '#')))))
    {
      if (color->c_name) 
	XtFree(color->c_name);

      /* try to get a name from rgb_table */
      if (cname[0] == '#')
	{
	  XColor xcolor;
	  char *rgb_name;

	  xcolor.pixel = pixel;
	  XQueryColor(dpy, PW->core.colormap, &xcolor);
	  rgb_name = xpmGetRgbName(rgb_table, max_ncolors, 
				   (int) xcolor.red, 
				   (int) xcolor.green, 
				   (int) xcolor.blue);

	  if (rgb_name)
	    cname=rgb_name;
	}

      color->c_name = XtNewString(cname);
    }
  else if ((!cname) && (!color->c_name))
    {
      XColor xcolor;
      char *rgb_name;

      xcolor.pixel = pixel;
      XQueryColor(dpy, PW->core.colormap, &xcolor);
      
      if (!(rgb_name = xpmGetRgbName(rgb_table, max_ncolors,
				     (int) xcolor.red,
				     (int) xcolor.green,
				     (int) xcolor.blue)))
	{
	  color->c_name = (char *)XtMalloc(15*sizeof(char));
	  sprintf(color->c_name, "#%04X%04X%04X", 
		  xcolor.red, xcolor.green, xcolor.blue);
	} else {
	  color->c_name = XtNewString(rgb_name);
	}
    }
  
}
    
#if NeedFunctionPrototypes
int PWReadFile(Widget w, String filename)
#else
int PWReadFile(w, filename)
    Widget w;
    String filename;
#endif
{
    int status, resized;
    PixmapWidget PW = (PixmapWidget) w;

    status = readFile(PW, filename);
    if (status == XpmSuccess)
      {
	XtFree(PW->pixmap.filename);
	PW->pixmap.filename = XtNewString(filename);

	PWUnmark(w);
	resized = InternalResize(PW);

	if (PW->core.visible && !resized) {
	    XClearArea(dpy, XtWindow(PW),
		       0, 0, 
		       PW->core.width, PW->core.height,
		       True);
	}
      }
    else
	XtAppWarning(XtWidgetToApplicationContext(w),
		     " read file failed.  PixmapWidget");
    
    return status;
}

#if NeedFunctionPrototypes
void PWSetImage(Widget w, XImage *image, XImage *mask_image)
#else
void PWSetImage(w, image, mask_image)
    Widget w;
    XImage *image, *mask_image;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    XImage *buffer, *mask_buffer;
    int resized;
    
    buffer = CreatePixmapImage(PW, 
			       (Dimension) image->width, 
			       (Dimension) image->height);
    mask_buffer = CreateMaskImage(PW, 
				  (Dimension) image->width, 
				  (Dimension) image->height);
    
    TransferImageData(PW->pixmap.image, buffer);
    TransferImageData(PW->pixmap.mask_image, mask_buffer);
    
    DestroyPixmapImage(&PW->pixmap.image);
    DestroyPixmapImage(&PW->pixmap.buffer);
    DestroyMaskImage(&PW->pixmap.mask_image);
    DestroyMaskImage(&PW->pixmap.mask_buffer);
    
    PW->pixmap.image = image;
    PW->pixmap.mask_image = mask_image;
    PW->pixmap.buffer = buffer;
    PW->pixmap.mask_buffer = mask_buffer;
    PW->pixmap.width = image->width;
    PW->pixmap.height = image->height;
    
    resized = InternalResize(PW);

    if (PW->core.visible && !resized) {
	XClearArea(dpy, XtWindow(PW),
		   0, 0, 
		   PW->core.width, PW->core.height,
		   True);    
    }
}

#define MAXPRINTABLE 92			/* number of printable ascii chars
					 * minus \ and " for string compat
					 * and ? to avoid ANSI trigraphs. */

static char *printable =
" .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjklzxcvbnmMNBVCZ\
ASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

void buildXpmImageAndAttributes(PW, image, mask_image,
				xpm_image_return, infos_return)
     PixmapWidget PW;
     XImage *image;
     XImage *mask_image;
     XpmImage **xpm_image_return;
     XpmInfo **infos_return;
     
{
  register Position x, y;
  register Pixel pxl;
  unsigned int *data, color_index = 0;
  unsigned int i, j, k;
  XpmColor *color;
  XpmImage *xpm_image = (XpmImage *)XtMalloc(sizeof(XpmImage));
  XpmInfo *infos = (XpmInfo *)XtMalloc(sizeof(XpmInfo));

  ResetPWColorsUse(PW);

  if (PW->pixmap.hints_cmt || PW->pixmap.colors_cmt || PW->pixmap.pixels_cmt)
    {
      infos->valuemask = XpmComments;
      if (PW->pixmap.hints_cmt)
        infos->hints_cmt = XtNewString(PW->pixmap.hints_cmt);
      else
	infos->hints_cmt = NULL;
      if (PW->pixmap.colors_cmt)
        infos->colors_cmt = XtNewString(PW->pixmap.colors_cmt);
      else
	infos->colors_cmt = NULL;
      if (PW->pixmap.pixels_cmt)
        infos->pixels_cmt = XtNewString(PW->pixmap.pixels_cmt);
      else
	infos->pixels_cmt = NULL;
    }
  else infos->valuemask = 0;

  infos->x_hotspot = 0;
  infos->y_hotspot = 0;
  infos->nextensions = 0;
  infos->extensions = (XpmExtension *)0;

  xpm_image->width = image->width;
  xpm_image->height = image->height;
  xpm_image->cpp = 0;
  xpm_image->data = (unsigned int *)XtCalloc(image->width * image->height,
						sizeof(unsigned int));
  xpm_image->colorTable = (XpmColor *)NULL;
  xpm_image->ncolors = 0;
  
  for (y = 0, data = xpm_image->data; y < image->height; y++)
    for (x = 0; x < image->width; x++, data++)
    {
      PWColorInfo *pw_color;

	pxl = GetPxlFromImageAndMask(image, mask_image, x, y);
	pw_color = PWGetColor((Widget) PW, pxl);

	if ((color_index = pw_color->in_xpm) == -1)
	{ /* Add the color in colorTable */
	    color_index = xpm_image->ncolors;
	    pw_color->in_xpm = color_index;

	    xpm_image->ncolors++;
	    xpm_image->colorTable =
	      (XpmColor *)XtRealloc((char *)xpm_image->colorTable,
				    xpm_image->ncolors * sizeof(XpmColor));
	    color = xpm_image->colorTable+color_index;
	    
	    color->symbolic = XtNewString(pw_color->s_name);
	    color->m_color = XtNewString(pw_color->m_name);
	    color->g4_color = XtNewString(pw_color->g4_name);
	    color->g_color =  XtNewString(pw_color->g_name);
	    color->c_color = XtNewString(pw_color->c_name);
	}

	/* put color index in XpmImage */
	*data = color_index;
    }

  xpm_image->cpp = (xpm_image->ncolors / MAXPRINTABLE) + 1;

  for (i = 0, color = xpm_image->colorTable; i < xpm_image->ncolors;
       i++, color++)
    {
      char * s;
      
      color->string = (char *)XtMalloc(xpm_image->cpp + 1);
      s = color->string;
      
      *s++ = printable[k = i % MAXPRINTABLE];
      for (j = 1; j < xpm_image->cpp; j++, s++)
	*s = printable[k = ((i - k) / MAXPRINTABLE) % MAXPRINTABLE];
      *s = '\0';
    }
	
  *xpm_image_return = xpm_image;
  *infos_return = infos;
}


#if NeedFunctionPrototypes
int PWWriteFile(Widget w, String filename)
#else
int PWWriteFile(w, filename)
    Widget w;
    String filename;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    XImage *image, *mask_image;
    XPoint hot;
    int status;
    XpmImage *xpm_image;
    XpmInfo *infos;
    
    if (PW->pixmap.zooming) {
	image = CreatePixmapImage(PW,
				  (Dimension) PW->pixmap.zoom.image->width,
				  (Dimension) PW->pixmap.zoom.image->height);
	CopyImageData(PW->pixmap.zoom.image, image, 0, 0, 
		      (Dimension) PW->pixmap.zoom.image->width,
		      (Dimension) PW->pixmap.zoom.image->height, 0, 0);
	CopyImageData(PW->pixmap.image, image, 
		      0, 0, 
		      PW->pixmap.image->width - 1,
		      PW->pixmap.image->height - 1,
		      PW->pixmap.zoom.at_x, PW->pixmap.zoom.at_y);

	mask_image = CreateMaskImage(PW,
				     (Dimension)PW->pixmap.zoom.image->width,
				     (Dimension)PW->pixmap.zoom.image->height);
	CopyImageData(PW->pixmap.zoom.mask_image, mask_image, 0, 0, 
		      (Dimension) PW->pixmap.zoom.mask_image->width,
		      (Dimension) PW->pixmap.zoom.mask_image->height, 0, 0);
	CopyImageData(PW->pixmap.mask_image, mask_image, 
		      0, 0, 
		      PW->pixmap.mask_image->width - 1,
		      PW->pixmap.mask_image->height - 1,
		      PW->pixmap.zoom.at_x, PW->pixmap.zoom.at_y);

	if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y))
	  {
	    hot.x = PW->pixmap.hot.x + PW->pixmap.zoom.at_x;
	    hot.y = PW->pixmap.hot.y + PW->pixmap.zoom.at_y;
	  }
	else hot = PW->pixmap.zoom.hot;
    }
    else {
	image = PW->pixmap.image;
	mask_image = PW->pixmap.mask_image;
	hot = PW->pixmap.hot;
    }
    
    if (filename) 
      {
	XtFree(PW->pixmap.filename);
	PW->pixmap.filename = XtNewString(filename);
      }

    if (_PWDEBUG)
	fprintf(stderr, "Saving filename: %s\n", filename);

    buildXpmImageAndAttributes(PW, image, mask_image,
			       &xpm_image, &infos);
    if (QuerySet(hot.x, hot.y))
      {
	infos->valuemask |= XpmHotspot;
	infos->x_hotspot = hot.x;
	infos->y_hotspot = hot.y;
      }
    
    /* need to account for zooming for ports ?? */
    infos->valuemask |= XpmExtensions;	/* save extensions */
    CopyExtensions( &infos->extensions, &infos->nextensions,
                    PW->pixmap.extensions, PW->pixmap.nextensions );
    
    status = XpmWriteFileFromXpmImage(PW->pixmap.filename, xpm_image,
				      infos);

    XpmFreeXpmImage(xpm_image);
    XtFree((char *)xpm_image);
    XpmFreeXpmInfo(infos);
    XtFree((char *)infos);
    
    if (PW->pixmap.zooming) {
	DestroyPixmapImage(&image);
	DestroyMaskImage(&mask_image);
    }
        
    if (status == XpmSuccess)
      PW->pixmap.changed = False;
    
    return status;
}

#if NeedFunctionPrototypes
String PWGetFilename(Widget w, String *str)
#else
String PWGetFilename(w, str)
    Widget w;
    String *str;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    *str = XtNewString(PW->pixmap.filename);

    return *str;
}

#if NeedFunctionPrototypes
String PWGetFilepath(Widget w, String *str)
#else
String PWGetFilepath(w, str)
    Widget w;
    String *str;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    String end;

    *str = XtNewString(PW->pixmap.filename);
    end = rindex(*str, '/');

    if (end)
	*(end + 1) = '\0';
    else 
	**str = '\0';

    return *str;
}


void FixHotSpot(PW)
    PixmapWidget PW;
{
    if (!QueryInPixmap(PW, PW->pixmap.hot.x, PW->pixmap.hot.y))
	PW->pixmap.hot.x = PW->pixmap.hot.y = NotSet;
}


void ZoomOut(PW)
    PixmapWidget PW;
{
    CopyImageData(PW->pixmap.image, PW->pixmap.zoom.image, 
		  0, 0, 
		  PW->pixmap.image->width - 1,
		  PW->pixmap.image->height - 1,
		  PW->pixmap.zoom.at_x, PW->pixmap.zoom.at_y);
    
    CopyImageData(PW->pixmap.mask_image, PW->pixmap.zoom.mask_image, 
		  0, 0, 
		  PW->pixmap.mask_image->width - 1,
		  PW->pixmap.mask_image->height - 1,
		  PW->pixmap.zoom.at_x, PW->pixmap.zoom.at_y);

    DestroyPixmapImage(&PW->pixmap.image);
    DestroyPixmapImage(&PW->pixmap.buffer);
    DestroyMaskImage(&PW->pixmap.mask_image);
    DestroyMaskImage(&PW->pixmap.mask_buffer);
    
    PW->pixmap.image = PW->pixmap.zoom.image;
    PW->pixmap.mask_image = PW->pixmap.zoom.mask_image;
    PW->pixmap.buffer = PW->pixmap.zoom.buffer;
    PW->pixmap.mask_buffer = PW->pixmap.zoom.mask_buffer;
    PW->pixmap.width = PW->pixmap.image->width;
    PW->pixmap.height = PW->pixmap.image->height;
    PW->pixmap.fold = PW->pixmap.zoom.fold;
    PW->pixmap.changed |= PW->pixmap.zoom.changed;
    PW->pixmap.grid = PW->pixmap.zoom.grid;

    if (QuerySet(PW->pixmap.hot.x, PW->pixmap.hot.y)) {
	PW->pixmap.hot.x += PW->pixmap.zoom.at_x;
	PW->pixmap.hot.y += PW->pixmap.zoom.at_y;
    }
    else
	PW->pixmap.hot = PW->pixmap.zoom.hot;

    PW->pixmap.mark.from_x = NotSet;
    PW->pixmap.mark.from_y = NotSet;
    PW->pixmap.mark.to_x = NotSet;
    PW->pixmap.mark.to_y = NotSet;
    PW->pixmap.zooming = False;
}    

#if NeedFunctionPrototypes
void PWZoomOut(Widget w)
#else
void PWZoomOut(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    int resized;
    
    if (PW->pixmap.zooming) {
	ZoomOut(PW);
	
	resized = InternalResize(PW);
	if (PW->core.visible && !resized)
	    XClearArea(dpy, XtWindow(PW),
		       0, 0, 
		       PW->core.width, PW->core.height,
		       True);
    }
}

#if NeedFunctionPrototypes
void PWZoomMarked(Widget w)
#else
void PWZoomMarked(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    PWZoomIn(w, 
	     PW->pixmap.mark.from_x, PW->pixmap.mark.from_y,
	     PW->pixmap.mark.to_x,   PW->pixmap.mark.to_y);
}

#if NeedFunctionPrototypes
void PWZoomIn(Widget w, Position from_x, Position from_y, 
	      Position to_x, Position to_y)
#else
void PWZoomIn(w, from_x, from_y, to_x, to_y)
    Widget w;
    Position from_x, from_y, to_x, to_y;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    XImage *image, *buffer, *mask_image, *mask_buffer;    
    Dimension width, height, coreWidth, coreHeight;
    int resized;

    if (PW->pixmap.zooming)
	PWZoomOut(w);
    
    QuerySwap(from_x, to_x);
    QuerySwap(from_y, to_y);
    from_x = max(0, from_x);
    from_y = max(0, from_y);
    to_x = min(PW->pixmap.width - 1, to_x);
    to_y = min(PW->pixmap.height - 1, to_y);
    
    width = to_x - from_x + 1;
    height = to_y - from_y + 1;

    image = CreatePixmapImage(PW, width, height);
    buffer = CreatePixmapImage(PW, width, height);
    mask_image = CreateMaskImage(PW, width, height);
    mask_buffer = CreateMaskImage(PW, width, height);

    CopyImageData(PW->pixmap.image, image, from_x, from_y, to_x, to_y, 0, 0);
    CopyImageData(PW->pixmap.buffer, buffer, from_x, from_y, to_x, to_y, 0, 0);
    CopyImageData(PW->pixmap.mask_image, mask_image,
		  from_x, from_y, to_x, to_y, 0, 0);
    CopyImageData(PW->pixmap.mask_buffer, mask_buffer,
		  from_x, from_y, to_x, to_y, 0, 0);
    
    PW->pixmap.zoom.image = PW->pixmap.image;
    PW->pixmap.zoom.buffer = PW->pixmap.buffer;
    PW->pixmap.zoom.mask_image = PW->pixmap.mask_image;
    PW->pixmap.zoom.mask_buffer = PW->pixmap.mask_buffer;
    PW->pixmap.zoom.at_x = from_x;
    PW->pixmap.zoom.at_y = from_y;
    PW->pixmap.zoom.fold = PW->pixmap.fold;
    PW->pixmap.zoom.changed = PW->pixmap.changed;
    PW->pixmap.zoom.hot = PW->pixmap.hot;
    PW->pixmap.zoom.grid = PW->pixmap.grid;

    PW->pixmap.image = image;
    PW->pixmap.buffer = buffer;
    PW->pixmap.mask_image = mask_image;
    PW->pixmap.mask_buffer = mask_buffer;
    PW->pixmap.width = width;
    PW->pixmap.height = height;
    PW->pixmap.changed = False;
    PW->pixmap.hot.x -= from_x;
    PW->pixmap.hot.y -= from_y;
    PW->pixmap.mark.from_x = NotSet;
    PW->pixmap.mark.from_y = NotSet;
    PW->pixmap.mark.to_x = NotSet;
    PW->pixmap.mark.to_y = NotSet;
    PW->pixmap.zooming = True;
    PW->pixmap.grid = True; /* potencially true, could use a resource here */

    FixHotSpot(PW);

    coreWidth = PW->core.width;
    coreHeight = PW->core.height;
    resized = InternalResize(PW);
    if (PW->core.visible && !resized)
	    Refresh(PW, 0, 0, PW->core.width, PW->core.height);
/*
	XClearArea(dpy, XtWindow(PW),
		   0, 0, 
		   coreWidth, coreHeight,
		   True);
*/
}

XImage *ScalePixmapImage();

#if NeedFunctionPrototypes
void PWRescale(Widget w, Dimension width, Dimension height)
#else
void PWRescale(w, width, height)
    Widget w;
    Dimension width, height;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    XImage *image, *mask_image;
    int resized;

    if (PW->pixmap.zooming)
	ZoomOut(PW);
        
    image = ScalePixmapImage(PW, PW->pixmap.image, 
		       (double) width / (double) PW->pixmap.image->width,
		       (double) height / (double) PW->pixmap.image->height);
    mask_image = ScaleMaskImage(PW, PW->pixmap.mask_image, 
		     (double) width / (double) PW->pixmap.mask_image->width,
		     (double) height / (double) PW->pixmap.mask_image->height);

    DestroyPixmapImage(&PW->pixmap.image);
    DestroyMaskImage(&PW->pixmap.mask_image);
    
    PW->pixmap.image = image;
    PW->pixmap.mask_image = mask_image;
    PW->pixmap.width = image->width;
    PW->pixmap.height = image->height;
    
    FixHotSpot(PW);
    FixMark(PW);

    resized = InternalResize(PW);
    if (PW->core.visible && !resized)
	XClearArea(dpy, XtWindow(PW),
		   0, 0, 
		   PW->core.width, PW->core.height,
		   True);
}

#if NeedFunctionPrototypes
Boolean PWQueryZooming(Widget w)
#else
Boolean PWQueryZooming(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    return PW->pixmap.zooming;
}

#if NeedFunctionPrototypes
void PWResize(Widget w, Dimension width, Dimension height)
#else
void PWResize(w, width, height)
    Widget w;
    Dimension width, height;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    XImage *image, *mask_image;
    int resized;

    if (PW->pixmap.zooming)
	ZoomOut(PW);

    image = CreatePixmapImage(PW, width, height);
    mask_image = CreateMaskImage(PW, width, height);

    TransferImageData(PW->pixmap.image, image);
    TransferImageData(PW->pixmap.mask_image, mask_image);
    
    DestroyPixmapImage(&PW->pixmap.image);
    DestroyMaskImage(&PW->pixmap.mask_image);

    PW->pixmap.image = image;
    PW->pixmap.mask_image = mask_image;
    PW->pixmap.width = width;
    PW->pixmap.height = height;

    FixHotSpot(PW);
    FixMark(PW);

    resized = InternalResize(PW);
    if (PW->core.visible && !resized)
	XClearArea(dpy, XtWindow(PW),
		   0, 0, 
		   PW->core.width, PW->core.height,
		   True);
}

static void Destroy(w)
    Widget w;
{
    PixmapWidget PW = (PixmapWidget) w;
    int i;
    
    /* free colorTable */
    for (i = 0; i < 1<<depth; i++)
      if (PW->pixmap.colorTable[i])
	{
	  if (PW->pixmap.colorTable[i]->symbol) 
	    XtFree(PW->pixmap.colorTable[i]->symbol);
	  if (PW->pixmap.colorTable[i]->s_name) 
	    XtFree(PW->pixmap.colorTable[i]->s_name);
	  if (PW->pixmap.colorTable[i]->m_name) 
	    XtFree(PW->pixmap.colorTable[i]->m_name);
	  if (PW->pixmap.colorTable[i]->g4_name) 
	    XtFree(PW->pixmap.colorTable[i]->g4_name);
	  if (PW->pixmap.colorTable[i]->g_name) 
	    XtFree(PW->pixmap.colorTable[i]->g_name);
	  if (PW->pixmap.colorTable[i]->c_name) 
	    XtFree(PW->pixmap.colorTable[i]->c_name);
	  XtFree((char *)PW->pixmap.colorTable[i]);
	}
    XtFree((char *)PW->pixmap.colorTable);

    XFreeGC(dpy, PW->pixmap.drawing_gc);
    XFreeGC(dpy, PW->pixmap.highlighting_gc);
    XFreeGC(dpy, PW->pixmap.framing_gc);
    XFreeGC(dpy, PW->pixmap.transparent_gc);

    DestroyPixmapImage(&PW->pixmap.image);
    DestroyPixmapImage(&PW->pixmap.buffer);
    DestroyMaskImage(&PW->pixmap.mask_image);
    DestroyMaskImage(&PW->pixmap.mask_buffer);
    
    PWRemoveAllRequests(w);
}


/* internal function that makes a gemotry request to PixmapWidget's parent */
static int InternalResize(PW)
     PixmapWidget PW;
{
    Dimension w, h, rw, rh, oldw, oldh;
    XtGeometryResult status;

    oldw = PW->core.width;
    oldh = PW->core.height;
    w = 2 * (int)PW->pixmap.distance +
      (int)PW->pixmap.width * PW->pixmap.squareW;
    h = 2 * (int)PW->pixmap.distance +
      (int)PW->pixmap.height * PW->pixmap.squareH;
    status = XtMakeResizeRequest((Widget)PW, w, h, &rw, &rh);

    if (status == XtGeometryAlmost)
      status = XtMakeResizeRequest((Widget)PW, rw, rh, NULL, NULL);
    else if (status == XtGeometryNo)
      XtWarning("PixmapWidget: Geometry request denied");

    return ((oldw != PW->core.width) || (oldh != PW->core.height));
}


static void Resize(PW)
     PixmapWidget PW;
{
    Dimension squareW, squareH;

    if (PW->pixmap.resize == True)
      {
	squareW = max(1, 
		      ((int)PW->core.width - 2 * (int)PW->pixmap.distance) / 
		      (int)PW->pixmap.width);
	squareH = max(1, 
		      ((int)PW->core.height - 2 * (int)PW->pixmap.distance) / 
		      (int)PW->pixmap.height);

	if (PW->pixmap.proportional)
	  PW->pixmap.squareW = PW->pixmap.squareH = min(squareW, squareH);
	else {
	  PW->pixmap.squareW = squareW;
	  PW->pixmap.squareH = squareH;
	}
      }

    PW->pixmap.horizOffset = max((Position)PW->pixmap.distance, 
				 (Position)(PW->core.width - 
					    PW->pixmap.width * 
					    PW->pixmap.squareW) / 2);
    PW->pixmap.vertOffset = max((Position)PW->pixmap.distance, 
				(Position)(PW->core.height - 
					   PW->pixmap.height * 
					   PW->pixmap.squareH) / 2);

    PW->pixmap.grid &= ((PW->pixmap.squareW > PW->pixmap.grid_tolerance) && 
			(PW->pixmap.squareH > PW->pixmap.grid_tolerance));
}

static void Redisplay(PW, event, region)
     PixmapWidget PW;
     XEvent      *event;
     Region       region;
{
    if(event->type == Expose) {
	if (PW->core.visible) {  
	    Refresh(PW, 
		    event->xexpose.x, event->xexpose.y,
		    event->xexpose.width, event->xexpose.height);
	}
    }
}

#if NeedFunctionPrototypes
void PWClip(Widget w, Position from_x, Position from_y, 
	    Position to_x, Position to_y)
#else
void PWClip(w, from_x, from_y, to_x, to_y)
    Widget w;
    Position from_x, from_y, to_x, to_y;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    XRectangle rectangle;
  
    QuerySwap(from_x, to_x);
    QuerySwap(from_y, to_y);
    from_x = max(0, from_x);
    from_y = max(0, from_y);
    to_x = min(PW->pixmap.width - 1, to_x);
    to_y = min(PW->pixmap.height - 1, to_y);

    rectangle.x = InWindowX(PW, from_x);
    rectangle.y = InWindowY(PW, from_y);
    rectangle.width = InWindowX(PW, to_x  + 1) - InWindowX(PW, from_x);
    rectangle.height = InWindowY(PW, to_y + 1) - InWindowY(PW, from_y);
    XSetClipRectangles(dpy,
		       PW->pixmap.highlighting_gc,
		       0, 0,
		       &rectangle, 1,
		       Unsorted);
    XSetClipRectangles(dpy,
		       PW->pixmap.drawing_gc,
		       0, 0,
		       &rectangle, 1,
		       Unsorted);
    XSetClipRectangles(dpy,
		       PW->pixmap.framing_gc,
		       0, 0,
		       &rectangle, 1,
		       Unsorted);
}

#if NeedFunctionPrototypes
void PWUnclip(Widget w)
#else
void PWUnclip(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    XRectangle rectangle;
  
    rectangle.x = InWindowX(PW, 0);
    rectangle.y = InWindowY(PW, 0);
    rectangle.width = InWindowX(PW, PW->pixmap.width) - InWindowX(PW, 0);
    rectangle.height = InWindowY(PW, PW->pixmap.height) - InWindowY(PW, 0);
    XSetClipRectangles(dpy,
		       PW->pixmap.highlighting_gc,
		       0, 0,
		       &rectangle, 1,
		       Unsorted);
    XSetClipRectangles(dpy,
		       PW->pixmap.drawing_gc,
		       0, 0,
		       &rectangle, 1,
		       Unsorted);
    XSetClipRectangles(dpy,
		       PW->pixmap.framing_gc,
		       0, 0,
		       &rectangle, 1,
		       Unsorted);
}

void Refresh(PW, x, y, width, height)
    PixmapWidget PW;
    Position     x, y;
    Dimension    width, height;
{
    XRectangle rectangle;

    XDefineCursor(dpy, XtWindow(PW), PW->pixmap.cursor);

    rectangle.x = min(x, InWindowX(PW, InPixmapX(PW, x)));
    rectangle.y = min(y, InWindowY(PW, InPixmapY(PW, y)));
    rectangle.width = max(x + width,
		     InWindowX(PW, InPixmapX(PW, x + width)+1)) - rectangle.x;
    rectangle.height = max(y + height,
		     InWindowY(PW, InPixmapY(PW, y + height)+1)) - rectangle.y;
    
    XClearArea(dpy, XtWindow(PW),
	       rectangle.x, rectangle.y,
	       rectangle.width, rectangle.height,
	       False);

    XSetClipRectangles(dpy,
		       PW->pixmap.framing_gc,
		       0, 0,
		       &rectangle, 1,
		       Unsorted);

    XDrawRectangle(dpy, XtWindow(PW),
		   PW->pixmap.framing_gc,
		   InWindowX(PW, 0) - 1, InWindowY(PW, 0) - 1,
		   InWindowX(PW, PW->pixmap.width) - InWindowX(PW, 0) + 1, 
		   InWindowY(PW, PW->pixmap.height) - InWindowY(PW, 0) + 1);

    PWClip((Widget) PW,
	   InPixmapX(PW, x),InPixmapY(PW, y),
	   InPixmapX(PW, x + width), InPixmapY(PW, y + height));

    PWRedrawSquares((Widget) PW, InPixmapX(PW, x), InPixmapY(PW, y),
		    InPixmapX(PW, x + width), InPixmapY(PW, y + height));
    
    PWRedrawGrid((Widget) PW,
		 InPixmapX(PW, x), InPixmapY(PW, y),
		 InPixmapX(PW, x + width), InPixmapY(PW, y + height));
    PWRedrawMark((Widget) PW);
    PWRedrawHotSpot((Widget) PW);
    if (PW->pixmap.redrawCallback)
        PW->pixmap.redrawCallback( (Widget) PW, Set );	/* redraw extensions */
    PWRedrawAxes((Widget) PW);
    PWUnclip((Widget) PW);
}

#if NeedFunctionPrototypes
void PWSwitchGrid(Widget w)
#else
void PWSwitchGrid(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    PW->pixmap.grid ^= TRUE;
    PWDrawGrid(w,
	       0, 0,
	       PW->pixmap.image->width - 1, PW->pixmap.image->height - 1);
}

#if NeedFunctionPrototypes
void PWGrid(Widget w, Boolean _switch)
#else
void PWGrid(w, _switch)
    Widget w;
    Boolean _switch;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    if (PW->pixmap.grid != _switch)
	PWSwitchGrid(w);
}

static Boolean SetValues(current, request, new)
     Widget current, request, new;
{
  PixmapWidget p_old = (PixmapWidget) current;
  PixmapWidget p_new = (PixmapWidget) new;
  PixmapWidget p_req = (PixmapWidget) request;
  
  if ((p_old->pixmap.cursor != p_req->pixmap.cursor) && XtIsRealized(new))
    XDefineCursor(XtDisplay(new), XtWindow(new), p_new->pixmap.cursor);
  if (p_old->pixmap.squareW != p_req->pixmap.squareW)
    {
	if ((p_old->pixmap.proportional == True) &&
	    (p_new->pixmap.squareH != p_new->pixmap.squareW))
	    p_new->pixmap.squareH = p_new->pixmap.squareW;
	InternalResize(p_new);
    }
  
  if (p_old->pixmap.squareH != p_req->pixmap.squareH)
    {
	if ((p_old->pixmap.proportional == True) &&
	    (p_new->pixmap.squareH != p_new->pixmap.squareW))
	    p_new->pixmap.squareW = p_new->pixmap.squareH;
	InternalResize(p_new);
    }
  
    return FALSE;
}

#if NeedFunctionPrototypes
Boolean PWQueryProportional(Widget w)
#else
Boolean PWQueryProportional(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    return (PW->pixmap.proportional);
}

#if NeedFunctionPrototypes
void PWSwitchProportional(Widget w)
#else
void PWSwitchProportional(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    int resized;

    PW->pixmap.proportional ^= True;

    resized = InternalResize(PW);
    if (PW->core.visible && !resized)
	XClearArea(dpy, XtWindow(PW),
		   0, 0, 
		   PW->core.width, PW->core.height,
		   True);
}

#if NeedFunctionPrototypes
void PWProportional(Widget w, Boolean _switch)
#else
void PWProportional(w, _switch)
    Widget w;
    Boolean _switch;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;

    if (PW->pixmap.proportional != _switch)
	PWSwitchProportional(w);
}


#if NeedFunctionPrototypes
void PWSetPickPixelDrawProc( Widget w, PickPixelProc proc )
#else
void PWSetPickPixelDrawProc( w, proc )
     Widget		w;
     PickPixelProc	proc;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  PW->pixmap.pickPixelDraw = proc;
}


#if NeedFunctionPrototypes
void PWSetPickPixelCompleteProc( Widget w, PickPixelProc proc )
#else
void PWSetPickPixelCompleteProc( w, proc )
     Widget		w;
     PickPixelProc	proc;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  PW->pixmap.pickPixelComplete = proc;
}


#if NeedFunctionPrototypes
void PWSetDrawPointProc( Widget w, PWDrawPointProc proc )
#else
void PWSetDrawPointProc( w, proc )
     Widget		w;
     PWDrawPointProc	proc;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;

  PW->pixmap.drawPointCallback = proc;
}


#if NeedFunctionPrototypes
void PWSetRedrawProc( Widget w, PWRedrawProc proc )
#else
void PWSetRedrawProc( w, proc )
     Widget		w;
     PWRedrawProc	proc;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;

  PW->pixmap.redrawCallback = proc;
}


#if NeedFunctionPrototypes
void PWSetTranslateProc( Widget w, PWTranslateProc proc )
#else
void PWSetTranslateProc( w, proc )
     Widget		w;
     PWTranslateProc	proc;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;

  PW->pixmap.translateCallback = proc;
}


#if NeedFunctionPrototypes
void PWSetRotateProc( Widget w, PWRotateProc proc )
#else
void PWSetRotateProc( w, proc )
     Widget		w;
     PWRotateProc	proc;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;
  
  PW->pixmap.rotateCallback = proc;
}


#if NeedFunctionPrototypes
void PWSetFlipProc( Widget w, PWFlipProc proc )
#else
void PWSetFlipProc( w, proc )
     Widget		w;
     PWFlipProc	proc;
#endif
{
  PixmapWidget PW = (PixmapWidget) w;

  PW->pixmap.flipCallback = proc;
}


#if NeedFunctionPrototypes
void PWSetFont( Widget w, XFontStruct *font_struct ) 
#else
void PWSetFont( w, font_struct ) 
     Widget	 w; 
     XFontStruct *font_struct; 
#endif
{
  PixmapWidget PW = (PixmapWidget) w;

  PW->pixmap.font_struct = font_struct;

  if ( PW->pixmap.font_struct && PW->pixmap.text_string ) {

      int          direction, ascent, descent;
      XCharStruct  size;

      XTextExtents( PW->pixmap.font_struct, 
                    PW->pixmap.text_string, strlen(PW->pixmap.text_string), 
                    &direction, &ascent, &descent, &size );

      PW->pixmap.text_rbearing = size.rbearing;
      PW->pixmap.text_lbearing = size.lbearing;
      PW->pixmap.text_ascent   = size.ascent;
      PW->pixmap.text_descent  = size.descent;
  }
}


#if NeedFunctionPrototypes
void PWSetText( Widget w, String s ) 
#else
void PWSetText( w, s ) 
     Widget	w; 
     String	s; 
#endif
{
  PixmapWidget PW = (PixmapWidget) w;

  PW->pixmap.text_string = XtNewString( s );

  if ( PW->pixmap.font_struct && PW->pixmap.text_string ) {

      int          direction, ascent, descent;
      XCharStruct  size;

      XTextExtents( PW->pixmap.font_struct, 
                    PW->pixmap.text_string, strlen(PW->pixmap.text_string), 
                    &direction, &ascent, &descent, &size );

      PW->pixmap.text_rbearing = size.rbearing;
      PW->pixmap.text_lbearing = size.lbearing;
      PW->pixmap.text_ascent   = size.ascent;
      PW->pixmap.text_descent  = size.descent;
  }
}
