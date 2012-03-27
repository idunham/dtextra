/*
 * $Id: PixEdit.c,v 1.20 1996/09/02 16:50:33 lmallet Exp $
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
 * $XConsortium: BitEdit.c,v 1.7 90/06/09 20:19:19 dmatic Exp $
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

static char rcsid[] = "$Id: PixEdit.c,v 1.20 1996/09/02 16:50:33 lmallet Exp $";


#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Xos.h>
#include <X11/Xfuncs.h>
#include <X11/StringDefs.h>
#include <X11/extensions/shape.h>

#ifndef USE_ATHENA
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleBG.h>
#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#ifndef NO_EDITRES
#include <X11/Xmu/Editres.h>
#endif /* NO_EDITRES */
#define CHARSET XmSTRING_DEFAULT_CHARSET
#if XmVersion > 1001 /* Motif 1.2 or > */
#include <Xm/Display.h>
#endif
#else /* USE_ATHENA */
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/bitmaps/xlogo16>
#include <X11/Xaw/Viewport.h>
#endif /* USE_ATHENA */

#include "Pixmap.h"
#include "Version.h"
#include "patchlevel.h"
static char *info_str = NULL;
#define TOGGLE True
#define BUTTON False


static char *usage = "[-options ...]\n\
\n\
where options include:\n\
     -size WIDTHxHEIGHT\n\
     -squares dimension\n\
     -grid, +grid\n\
     -stippled\n\
     -stipple pixmap\n\
     -axes, +axes\n\
     -proportional, +proportional\n\
     -hl color\n\
     -fr color\n\
     -tr color\n\
     -fn/-font fontname\n\
     -filename filename\n\
     -f filename\n\
     -in filename\n\
     -colormap\n\
     -pc\n\
\n\
The default WIDTHxHEIGHT is 32x32.\n";


static XrmOptionDescRec options[] = {
  { "-squares",      "*pixmap.squareSize",     XrmoptionSepArg, NULL},
  { "-grid",         "*pixmap.grid",           XrmoptionNoArg,  "False"},
  { "+grid",         "*pixmap.grid",           XrmoptionNoArg,  "True"},
  { "-stippled",     "*pixmap.stippled",       XrmoptionNoArg,  "False"},
  { "-stipple",      "*pixmap.pixmap*stipple", XrmoptionSepArg, NULL},
  { "-axes",         "*pixmap.axes",           XrmoptionNoArg,  "False"},
  { "+axes",         "*pixmap.axes",           XrmoptionNoArg,  "True"},
  { "-proportional", "*pixmap.proportional",   XrmoptionNoArg,  "False"},
  { "+proportional", "*pixmap.proportional",   XrmoptionNoArg,  "True"},
  { "-hl",           "*pixmap.highlight",      XrmoptionSepArg, NULL},
  { "-fr",           "*pixmap.framing",        XrmoptionSepArg, NULL},
  { "-tr",           "*pixmap.transparent",    XrmoptionSepArg, NULL},
  { "-fn",           "*font",                  XrmoptionSepArg, "fixed"},
  { "-font",         "*font",                  XrmoptionSepArg, "fixed"},
  { "-filename",     "*pixmap.filename",       XrmoptionSepArg, NULL},
  { "-f",            "*pixmap.filename",       XrmoptionSepArg, NULL},
  { "-in",           "*pixmap.filename",       XrmoptionSepArg, NULL},
  { "-colormap",     "*privateColormap",       XrmoptionNoArg, "True"},
  { "-pc",           "*privateColormap",       XrmoptionNoArg, "True"},
};

typedef struct  {
    Boolean privateColormap;
} Resources, *ResourcesPtr;

static XtResource appResources[] = {
#define offset(field) XtOffset(ResourcesPtr, field)
  {"privateColormap", "PrivateColormap", XtRBoolean, sizeof(Boolean),
     offset(privateColormap), XtRImmediate, (caddr_t)False},
#undef offset
};


/* Color is the maximal id usable for menu entries different from color */
#define Color 200
#define Color_offset Color

typedef struct {
  int             id;
  String          name;
  Boolean         trap;
  Widget          widget;
  } ButtonRec;

static ButtonRec file_menu[] = {
#define Load 26
  {Load, "load", BUTTON},
#define Insert 101
  {Insert, "insert", BUTTON},
#define Save 27
  {Save, "save", BUTTON},
#define SaveAs 28
  {SaveAs, "saveAs", BUTTON},
#define Dummy -1
  {Dummy, "line", False},
#define Resize 24
  {Resize, "resize", BUTTON},
#define Rescale 79
  {Rescale, "rescale", BUTTON},
#define Filename 74
  {Filename, "filename", BUTTON},
#define Hints 106
  {Hints, "hintsCmt", BUTTON},
#define Colors 107
  {Colors, "colorsCmt", BUTTON},
#define Pixels 108
  {Pixels, "pixelsCmt", BUTTON},    
  {Dummy, "line", False},
#define Quit 75
  {Quit, "quit", BUTTON},
};

static ButtonRec edit_menu[] = {
#define Image 77
  {Image, "image", TOGGLE},
  {Dummy, "line", False},
#define Grid 23
  {Grid, "grid", TOGGLE},
#define Axes 34
  {Axes, "axes", TOGGLE},
#define Proportional 97
  {Proportional, "proportional", TOGGLE},
#define Zoom 100
  {Zoom, "zoom", TOGGLE},
#define ZoomIn 115
  {ZoomIn, "zoomIn", BUTTON},
#define ZoomOut 116
  {ZoomOut, "zoomOut", BUTTON},
#define ZoomFactor 117
  {ZoomFactor, "zoomFactor", BUTTON},
/* Dummy */
  {Dummy, "line", False},
#define Cut 30
  {Cut, "cut", BUTTON},
#define Copy 31
  {Copy, "copy", BUTTON},
#define Paste 4
  {Paste, "paste", BUTTON},
#define Crop 118
  {Crop, "crop", BUTTON},
};

static ButtonRec color_menu[] = {
#define ColorPixel 109
  {ColorPixel, "addColor", BUTTON},
#define ColorSname 110
  {ColorSname, "symbolicName", BUTTON},
#define ColorMname 111
  {ColorMname, "monochromeName", BUTTON},
#define ColorG4name 112
  {ColorG4name, "g4Name", BUTTON},
#define ColorGname 113
  {ColorGname, "gName", BUTTON},
#define ColorCname 114
  {ColorCname, "cName", BUTTON},
};


static ButtonRec extension_menu[] = {

#define AddExtension		119

  /* id,               name,               trap,   widget */
  { AddExtension,     "addExtension",      BUTTON, NULL },
  { Dummy,            "line",              False,  NULL },
};


static ButtonRec buttons[] = {
#define Undo 25
  {Undo, "undo", BUTTON},
/*#define Clear 1*/
  {Clear, "clear", BUTTON},
/*#define Set 2*/
  {Set, "set", BUTTON},
#define Redraw 5
  {Redraw, "redraw", BUTTON},
#define CopyImm 102
  {CopyImm, "copy", TOGGLE},
#define MoveImm 103
  {MoveImm, "move", TOGGLE},
#define MarkImm 104
  {MarkImm, "mark", TOGGLE},
#define UnmarkImm 105
  {UnmarkImm, "unmark", BUTTON},
#define FlipHoriz 11
  {FlipHoriz, "flipHoriz", BUTTON},
#define Up 7
  {Up, "up", BUTTON},
#define FlipVert 12
  {FlipVert, "flipVert", BUTTON},
#define Left 9
  {Left, "left", BUTTON},
#define Fold 99
  {Fold, "fold", BUTTON},
#define Right 10
  {Right, "right", BUTTON},
#define RotateLeft 33
  {RotateLeft, "rotateLeft", BUTTON},
#define Down 8
  {Down, "down", BUTTON},
#define RotateRight 13
  {RotateRight, "rotateRight", BUTTON},
#define Point 14
  {Point, "point", TOGGLE},
#define Curve 41
  {Curve, "curve", TOGGLE},
#define Line 15
  {Line, "line", TOGGLE},
#define Rectangle 16
  {Rectangle, "rectangle", TOGGLE},
#define FilledRectangle 17
  {FilledRectangle, "filledRectangle", TOGGLE},
#define Circle 18
  {Circle, "circle", TOGGLE},
#define FilledCircle 19
  {FilledCircle, "filledCircle", TOGGLE},
#define FloodFill 20
  {FloodFill, "floodFill", TOGGLE},
#define Text 29
  {Text, "text", TOGGLE},
#define SetHotSpot 21
  {SetHotSpot, "setHotSpot", TOGGLE},
#define ClearHotSpot 22
  {ClearHotSpot, "clearHotSpot", BUTTON},
};

#include "Dialog.h"


XtAppContext pixmap_context;
Widget 
    top_widget, 
    parent_widget,
    formy_widget,
    infoButton_widget,
    fileButton_widget, fileMenu_widget,
    editButton_widget, editMenu_widget,
    colorButton_widget, colorMenu_widget,
    extensionMenu_widget,
    extensionEditor, 
    status_widget, statusb_widget,
    pane_widget, color_pane_widget,
    form_widget,
    pixmap_widget,
    image_shell,
    image_widget;

#include "TextOpt.h"
TextOptions *textOptions;
#include "TextOpt.c"

Display *dpy;
int screen;
int ncolors;
Pixel current_color;
Colormap cmap;
Pixel black, white;

char *hints_cmt, *colors_cmt, *pixels_cmt;
PWColorInfo **colorTable;

Boolean image_visible = False;
Pixmap check_mark;
Dialog info_dialog, input_dialog, error_dialog, qsave_dialog, file_dialog;
String filename = NULL, format = "";
char message[80];

void FixMenu(), SwitchImage(), SwitchGrid(), SwitchAxes(), 
  SwitchProportional(),   SwitchZoom(), DoLoad(), DoInsert(), DoSave(), 
  DoSaveAs(), DoResize(), DoRescale(), DoFilename(), DoHintsCmt(), 
  DoColorsCmt(), DoPixelsCmt(), DoQuit(), DoAddColor(), DoSymbolicName(), 
  DoMonochromeName(), DoGrey4Name(), DoGreyName(), DoColorName(), DoCut(), 
  DoCopy(), DoPaste(), DoCrop(), DoZoomIn(), DoZoomOut(), DoZoomFactor();

void TheCallback();
void MakeMenuPanel();

static XtActionsRec actions_table[] = {
  {"fix-menu", FixMenu},
  {"switch-image", SwitchImage},
  {"switch-grid", SwitchGrid},
  {"switch-axes", SwitchAxes},
  {"switch-proportional", SwitchProportional},
  {"switch-zoom", SwitchZoom},
  {"zoom-in", DoZoomIn},
  {"zoom-out", DoZoomOut},
  {"zoom-factor", DoZoomFactor},
  {"do-load", DoLoad},
  {"do-insert", DoInsert},
  {"do-save", DoSave},
  {"do-save-as", DoSaveAs},
  {"do-resize", DoResize},
  {"do-rescale", DoRescale},
  {"do-filename", DoFilename},
  {"do-hintscmt", DoHintsCmt},
  {"do-colorscmt", DoColorsCmt},
  {"do-pixelscmt", DoPixelsCmt},
  {"do-quit", DoQuit},
  {"do-addcolor", DoAddColor},
  {"do-symbname", DoSymbolicName},
  {"do-mononame", DoMonochromeName},
  {"do-grey4name", DoGrey4Name},
  {"do-greyname", DoGreyName},
  {"do-colorname", DoColorName},
  {"do-cut", DoCut},
  {"do-copy", DoCopy},
  {"do-paste", DoPaste},
  {"do-crop", DoCrop}
};

static Atom protocols[2];

void client_message(w, tag, event, b)
     Widget w;
     XtPointer tag;
     XEvent *event;
     Boolean *b;
{
  if (event->xclient.message_type == protocols[1] &&
      event->xclient.data.l[0] == protocols[0])
    /* the widget got a kill signal */
    {
      if (w == image_shell) 
	SwitchImage();
    }
}

void unsetKillfromWM(w)
     Widget w;
{
  
  /* set WM property to receive a window deletion and avoid getting killed */
  protocols[0] = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
  protocols[1] = XInternAtom(dpy, "WM_PROTOCOLS", True);
  XSetWMProtocols(dpy, XtWindow(w), protocols, 1);
  /* add handler to get WM's client message */
  XtAddEventHandler(w, ClientMessage, True, 
		    (XtEventHandler)client_message, NULL);
}


void FixColorMenuLabel(old_name, name)
     char *old_name;
     char *name;
{
  Widget colorButton;
  Arg wargs[1];
#ifndef USE_ATHENA
  XmString xmstr;

  xmstr = XmStringCreateLtoR(name, CHARSET);
  XtSetArg(wargs[0], XmNlabelString, xmstr);
#else /* USE_ATHENA */
  XtSetArg(wargs[0], XtNlabel, name);
#endif /* USE_ATHENA */

  colorButton = XtNameToWidget( formy_widget, "colorButton" );
  XtSetValues( colorButton, wargs, 1 );

#ifndef USE_ATHENA
  XmStringFree(xmstr);
#endif /* USE_ATHENA */
}


void colorToAdd(w, pixel, name)
     Widget w;
     Pixel pixel;
     char *name;
{
  Widget wi;
  int max_colors = 1<<DisplayPlanes(dpy,screen);
  
  if ((pixel != TRANSPARENT(dpy, screen)) && (pixel >= max_colors))
    printf(
    "Warning: Color pixel %d out of bounds for display (pixel range [0->%d]\n",
	   pixel, max_colors-1);
  else
    {
      int *id = (int *)XtMalloc(sizeof(int));
      Arg wargs[1];

      *id = pixel + Color_offset;

      /* Two kinds of colors: not transparent and transparent */
      if (pixel != TRANSPARENT(dpy, screen))
      {
#ifndef USE_ATHENA
	  XtSetArg(wargs[0], XmNbackground, pixel);
#else /* USE_ATHENA */
	  XtSetArg(wargs[0], XtNbackground, pixel);
#endif /* USE_ATHENA */
	  
#ifdef DEBUG
	  printf("Adding color %d %s to menu\n", pixel, name);
#endif /* DEBUG */
	  
#ifndef USE_ATHENA
	  wi = (Widget)XmCreatePushButton(color_pane_widget, name,
					  wargs, 1);
	  XtAddCallback(wi, XmNactivateCallback, TheCallback, id);
	  XtManageChild(wi);
#else /* USE_ATHENA */
	  wi = XtCreateManagedWidget(name, commandWidgetClass, 
				     color_pane_widget, wargs, 1);
	  XtAddCallback(wi, XtNcallback, TheCallback, id);
#endif /* USE_ATHENA */
      }
      else if (pixel == TRANSPARENT(dpy, screen))
      {
#ifndef USE_ATHENA
	  XtSetArg(wargs[0], XmNbackground,
		   PWGetTransparentPixel(w));
	  wi = (Widget)XmCreatePushButton(color_pane_widget, name,
					  wargs, 1);
	  XtAddCallback(wi, XmNactivateCallback, TheCallback, id);
	  XtManageChild(wi);
#else /* USE_ATHENA */
	  XtSetArg(wargs[0], XtNbackground,
		   PWGetTransparentPixel(w));
	  wi = XtCreateManagedWidget(name, commandWidgetClass, 
				     color_pane_widget, wargs, 1);
	  XtAddCallback(wi, XtNcallback, TheCallback, id);
#endif /* USE_ATHENA */
      }
  }
}


void FixImage(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
    Pixmap image, image_mask;
    Window root;
    unsigned int width, height, border_width, depth;
    int x, y;

    if (!image_visible)
      {
#ifndef USE_ATHENA
	XmToggleButtonGadgetSetState(edit_menu[0].widget, False, False);
#endif /* USE_ATHENA */
	return;
      }
#ifndef USE_ATHENA
    else XmToggleButtonGadgetSetState(edit_menu[0].widget, True, False);
#endif /* USE_ATHENA */
    
    PWGetUnzoomedPixmap(pixmap_widget, &image, &image_mask);
    XGetGeometry(XtDisplay(image_widget), image, &root, &x, &y, 
		 &width, &height, &border_width, &depth);
    XResizeWindow(XtDisplay(image_widget), XtWindow(image_shell), 
		  width, height);
    
    XSetWindowBackgroundPixmap(XtDisplay(pixmap_widget), 
			       XtWindow(image_widget), image); 

    if (image_mask) 
      {
	XShapeCombineMask(XtDisplay(image_widget), 
			  XtWindow(image_widget), ShapeBounding,
			  0, 0, image_mask, ShapeSet);
	XShapeCombineMask(XtDisplay(pixmap_widget), 
			  XtWindow(image_shell), ShapeBounding,
			  0, 0, image_mask, ShapeSet);
      }
    else 
      {
	XShapeCombineMask(XtDisplay(image_widget), 
			  XtWindow(image_widget), ShapeBounding,
			  0, 0, None, ShapeSet);
	XShapeCombineMask(XtDisplay(pixmap_widget), 
			  XtWindow(image_shell), ShapeBounding,
			  0, 0, None, ShapeSet);
      }

    XClearWindow(XtDisplay(pixmap_widget), XtWindow(image_widget));
    XFreePixmap(XtDisplay(pixmap_widget), image);
    if (image_mask) XFreePixmap(XtDisplay(image_widget), image_mask);
}

void FixEntry(w, id)
    Widget w;
    int *id;
{
    int n;
    Arg wargs[2];


    n = 0;
    switch (*id) {
	
    case Image:
#ifndef USE_ATHENA
        XtSetArg(wargs[n], XmNset, image_visible); n++;
#else /* USE_ATHENA */
	XtSetArg(wargs[n], XtNleftBitmap, 
		 image_visible ? check_mark : None); n++;
#endif /* USE_ATHENA */ 
	break;
	
    case Grid:
#ifndef USE_ATHENA
        XtSetArg(wargs[n], XmNset, PWQueryGrid(pixmap_widget)); n++;
#else /* USE_ATHENA */
	XtSetArg(wargs[n], XtNleftBitmap, 
		 PWQueryGrid(pixmap_widget) ? check_mark : None); n++;
#endif /* USE_ATHENA */
	break;

    case Axes:
#ifndef USE_ATHENA
        XtSetArg(wargs[n], XmNset, PWQueryAxes(pixmap_widget)); n++;
#else /* USE_ATHENA */
	XtSetArg(wargs[n], XtNleftBitmap, 
		 PWQueryAxes(pixmap_widget) ? check_mark : None); n++;
#endif /* USE_ATHENA */
	break;
	
    case Proportional:
#ifndef USE_ATHENA
        XtSetArg(wargs[n], XmNset, PWQueryProportional(pixmap_widget)); n++;
#else /* USE_ATHENA */
	XtSetArg(wargs[n], XtNleftBitmap, 
		 PWQueryProportional(pixmap_widget) ? check_mark : None); n++;
#endif /* USE_ATHENA */
	break;
	
    case Zoom:
#ifndef USE_ATHENA
        XtSetArg(wargs[n], XmNset, PWQueryZooming(pixmap_widget)); n++;
#else /* USE_ATHENA */
	XtSetArg(wargs[n], XtNleftBitmap, 
		 PWQueryZooming(pixmap_widget) ? check_mark : None); n++;
#endif /* USE_ATHENA */
	break;

    case Copy:
    case Cut:
#ifndef USE_ATHENA
	XtSetArg(wargs[n], XmNsensitive, PWQueryMarked(pixmap_widget)); n++;
#else /* USE_ATHENA */
	XtSetArg(wargs[n], XtNsensitive, PWQueryMarked(pixmap_widget)); n++;
#endif /* USE_ATHENA */
	break;

    case Paste:
#ifndef USE_ATHENA
	XtSetArg(wargs[n], XmNsensitive, PWQueryStored(pixmap_widget)); n++;
#else /* USE_ATHENA */
	XtSetArg(wargs[n], XtNsensitive, PWQueryStored(pixmap_widget)); n++;
#endif /* USE_ATHENA */
	break;

    case Crop:
#ifndef USE_ATHENA
	XtSetArg(wargs[n], XmNsensitive,
		 PWQueryMarked(pixmap_widget) || PWQueryStored(pixmap_widget));
	n++;
#else /* USE_ATHENA */
	XtSetArg(wargs[n], XtNsensitive,
		 PWQueryMarked(pixmap_widget) || PWQueryStored(pixmap_widget));
	n++;
#endif /* USE_ATHENA */
	break;

    default:
	return;
    }
    
    XtSetValues(w, wargs, n);
}

void FixMenu(w, event)
    Widget w;
    XEvent *event;
{
    int i;

    for (i = 0; i < XtNumber(edit_menu); i++)
	FixEntry(edit_menu[i].widget, &edit_menu[i].id);
}

#ifndef USE_ATHENA
void FixMenuCB(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  FixMenu(w, NULL);
}
#endif /* USE_ATHENA */

void FixStatus()
{
    int n;
    Arg wargs[2];
    String str;
#ifndef USE_ATHENA
    char *fn;
    XmString xmstr;
#endif /* USE_ATHENA */

    str = PWUnparseStatus(pixmap_widget);

    n = 0;
#ifndef USE_ATHENA
    fn = str;
    while ((*str != ' ') || (*(str-1) == ':')) str++;
    *str = 0;
    xmstr = XmStringCreateLtoR(fn, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(wargs[n], XmNlabelString, xmstr); n++;
    XtSetValues(status_widget, wargs, n);
    XmStringFree(xmstr);
    *str++ = ' ';
    n = 0;
    xmstr = XmStringCreateLtoR(str, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(wargs[n], XmNlabelString, xmstr); n++;
    XtSetValues(statusb_widget, wargs, n);
    XmStringFree(xmstr);
#else /* USE_ATHENA */
    XtSetArg(wargs[n], XtNlabel, str); n++;
    XtSetValues(status_widget, wargs, n);
#endif /* USE_ATHENA */
}

void FixColor(w, current)
     Widget w;
     Pixel current;
{
  Widget colorButton;
  Arg wargs[1];
  PWColorInfo *color;
#ifndef USE_ATHENA
  XmString xmstr;
#endif /* USE_ATHENA */

  current_color = current;
  color = PWGetColor(w, current);

#ifndef USE_ATHENA
  xmstr = XmStringCreateLtoR(color->c_name, CHARSET);
  XtSetArg(wargs[0], XmNlabelString, xmstr);
#else /* USE_ATHENA */  
  XtSetArg(wargs[0], XtNlabel, color->c_name);
#endif /* USE_ATHENA */

  colorButton = XtNameToWidget( formy_widget, "colorButton" );
  XtSetValues( colorButton, wargs, 1 );
#ifndef USE_ATHENA
  XmStringFree(xmstr);
#endif /* USE_ATHENA */
}

static int zero = 0;
#define Plain  (XtPointer)&zero,(Cardinal)sizeof(int)

void TheCallback(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
    int   *id = (int *)client_data;
    
    switch (*id) {
      
    case Load:
      DoLoad();
      break;
      
    case Insert:
      DoInsert();
      break;
      
    case Save:
      DoSave();
      break;
      
    case SaveAs:
      DoSaveAs();
      break;
      
    case Filename:
      DoFilename();
      break;
      
    case Hints:
      DoHintsCmt();
      break;
    case Colors:
      DoColorsCmt();
      break;
    case Pixels:
      DoPixelsCmt();
      break;
      
    case Quit:
      DoQuit();
      break;
      
    case ColorPixel:
      DoAddColor();
      break;
    case ColorSname:
      DoSymbolicName();
      break;
    case ColorMname:
      DoMonochromeName();
      break;
    case ColorG4name:
      DoGrey4Name();
      break;
    case ColorGname:
      DoGreyName();
      break;
    case ColorCname:
      DoColorName();
      break;
      
    case Image:
      SwitchImage();
      break;
      
    case Grid:
      SwitchGrid();
      break;
      
    case Axes:
      SwitchAxes();
      break;	
      
    case Proportional:
      SwitchProportional();
      break;
      
    case Zoom:
      SwitchZoom();
      break;

    case ZoomIn:
      DoZoomIn();
      break;
      
    case ZoomOut:
      DoZoomOut();
      break;
      
    case ZoomFactor:
      DoZoomFactor();
      break;

    case Resize:
      DoResize();
      break;
      
    case Rescale:
      DoRescale();
      break;

    case Copy:
      DoCopy();
      break;
      
    case Cut:
      DoCut();
      break;
      
    case Paste:
      DoPaste();
      break;
      
    case Crop:
      DoCrop();
      break;
      
    case Clear:
      PWStoreToBuffer(pixmap_widget);
      PWClear(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      break;
      
    case Set:
      PWStoreToBuffer(pixmap_widget);
      PWSet(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      break;
      
    case Redraw:
      PWRedraw(pixmap_widget);
      FixImage(pixmap_widget, NULL, NULL);
      break;
      
    case CopyImm:
      PWRemoveAllRequests(pixmap_widget);
      if (PWQueryMarked(pixmap_widget)) {
	PWAddRequest(pixmap_widget, MarkRequest, False, Plain);
	PWEngageRequest(pixmap_widget, CopyRequest, True, Plain);
      }
      else {
	PWEngageRequest(pixmap_widget, MarkRequest, False, Plain);
	PWAddRequest(pixmap_widget, CopyRequest, True, Plain);
      }
      break;
      
    case MoveImm:
      PWRemoveAllRequests(pixmap_widget);
      if (PWQueryMarked(pixmap_widget)) {
	PWAddRequest(pixmap_widget, MarkRequest, False, Plain);
	PWEngageRequest(pixmap_widget, MoveRequest, True, Plain);
      }
      else {
	PWEngageRequest(pixmap_widget, MarkRequest, False, Plain);
	PWAddRequest(pixmap_widget, MoveRequest, True, Plain);
      }
      break;
      
    case MarkImm:
      PWRemoveAllRequests(pixmap_widget);
      PWEngageRequest(pixmap_widget, MarkRequest, True, Plain);
      break;
      
    case UnmarkImm:
      PWUnmark((Widget)pixmap_widget);
      break;
      
    case Up:
      PWStoreToBuffer(pixmap_widget);
      PWUp(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      break;
      
    case Down:
      PWStoreToBuffer(pixmap_widget);
      PWDown(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      break;
      
    case Left:
      PWStoreToBuffer(pixmap_widget);
      PWLeft(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      break;
      
    case Right:
      PWStoreToBuffer(pixmap_widget);
      PWRight(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      break;
      
    case Fold:
      PWStoreToBuffer(pixmap_widget);
      PWFold(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      break;
      
    case FlipHoriz:
      PWStoreToBuffer(pixmap_widget);
      PWFlipHoriz(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      break;
      
    case FlipVert:
      PWStoreToBuffer(pixmap_widget);
      PWFlipVert(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      break;
      
    case RotateRight:
      PWStoreToBuffer(pixmap_widget);
      PWRotateRight(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      break;
      
    case RotateLeft:
      PWStoreToBuffer(pixmap_widget);
      PWRotateLeft(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      break;
      
    case Point:
      PWRemoveAllRequests(pixmap_widget);
      PWEngageRequest(pixmap_widget, PointRequest, True, Plain);
      break;
      
    case Curve:
      PWRemoveAllRequests(pixmap_widget);
      PWEngageRequest(pixmap_widget, CurveRequest, True, Plain);
      break;
      
    case Line:
      PWRemoveAllRequests(pixmap_widget);
      PWEngageRequest(pixmap_widget, LineRequest, True, Plain);
      break;
      
    case Rectangle:
      PWRemoveAllRequests(pixmap_widget);
      PWEngageRequest(pixmap_widget, RectangleRequest, True, Plain);
      break;
      
    case FilledRectangle:
      PWRemoveAllRequests(pixmap_widget);
      PWEngageRequest(pixmap_widget, FilledRectangleRequest, True, Plain);
      break;
      
    case Circle:
      PWRemoveAllRequests(pixmap_widget);
      PWEngageRequest(pixmap_widget, CircleRequest, True, Plain);
      break;
      
    case FilledCircle:
      PWRemoveAllRequests(pixmap_widget);
      PWEngageRequest(pixmap_widget, FilledCircleRequest, True, Plain);
      break;
      
    case FloodFill:
      PWRemoveAllRequests(pixmap_widget);
      PWEngageRequest(pixmap_widget, FloodFillRequest, True, Plain);
      break;
      
    case Text:
      PopupTextOptions( w, textOptions );
      PWRemoveAllRequests(pixmap_widget);
      PWEngageRequest(pixmap_widget, TextRequest, True, Plain);
      break;
      
    case SetHotSpot:
	PWRemoveAllRequests(pixmap_widget);
	PWEngageRequest(pixmap_widget, HotSpotRequest, True, Plain);
	break;
	
    case ClearHotSpot:
	PWStoreToBuffer(pixmap_widget);
	PWClearHotSpot(pixmap_widget);
	PWChangeNotify(pixmap_widget, NULL, NULL);
	PWSetChanged(pixmap_widget);
	break;

    case Undo:
      PWUndo(pixmap_widget);
#ifdef USE_ATHENA
      PWRedraw(pixmap_widget);
#endif /* USE_ATHENA */
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      FixStatus();
      break;	
    case Color: /* Start of colors management */
    default: /* Don't change this default section: color management */
      PWSetForeground(pixmap_widget,(*id - Color_offset));
      FixColor(pixmap_widget, *id - Color_offset);
      break; 
    }
}


void DoLoad()
{
  int status;

  if (PWQueryChanged(pixmap_widget)) {
    PWGetFilename(pixmap_widget, &filename);
  RetryLoadSave:
    switch (PopupDialog(qsave_dialog, "Save file before loading?",
			filename, &filename, XtGrabExclusive)) {
    case Yes:
      status = PWWriteFile(pixmap_widget, filename);
      if (status != XpmSuccess) {
	sprintf(message, "Can't write file: %s - %s", filename,
		XpmGetErrorString(status));
	if (PopupDialog(error_dialog, message, 
			NULL, NULL, XtGrabExclusive) == Retry) 
	  goto RetryLoadSave;
      }
      break;
      
    case Cancel:
      return;
    }
  }
  PWGetFilepath(pixmap_widget, &filename);
 RetryLoad:
  if (PopupFDialog(file_dialog, "Load file:", 
#ifndef USE_ATHENA
		  filename, &filename, XtGrabExclusive) == Okay) {
#else /* USE_ATHENA */
		  filename, &filename, "r") == Okay) {
#endif /* USE_ATHENA */
    status = PWReadFile(pixmap_widget, filename);
    if (status != XpmSuccess) {
      sprintf(message, "Can't read file: %s - %s", filename,
	      XpmGetErrorString(status));
      if (PopupDialog(error_dialog, message, 
		      NULL, NULL, XtGrabExclusive) == Retry)
	goto RetryLoad;
    }
    else {
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWClearChanged(pixmap_widget);
      FixStatus();
    }
  }
}

void DoInsert()
{
  PWGetFilepath(pixmap_widget, &filename);
 RetryInsert:
#ifndef USE_ATHENA
  if (PopupFDialog(file_dialog, "Insert file:", 
		  filename, &filename, XtGrabExclusive) == Okay) {
#else /* USE_ATHENA */
  if (PopupFDialog(file_dialog, "Insert file:", 
		  filename, &filename, "r") == Okay) {
#endif  /* USE_ATHENA */
    if (PWStoreFile(pixmap_widget, filename) != XpmSuccess) {
      sprintf(message, "Can't read file: %s", filename);
      if (PopupDialog(error_dialog, message, 
		      NULL, NULL, XtGrabExclusive) == Retry)
	goto RetryInsert;
    }
    else {
      PWEngageRequest(pixmap_widget, RestoreRequest, False, Plain);
    }
  }
}

void DoSave()
{
  PWGetFilename(pixmap_widget, &filename);
  if (PWWriteFile(pixmap_widget, filename) != XpmSuccess) 
    {
      sprintf(message, "Can't write file: %s", filename);
      if (PopupDialog(error_dialog, message, 
		      NULL, NULL, XtGrabExclusive) == Retry) 
	DoSaveAs();
    }
  else {
    PWClearChanged(pixmap_widget);
  }
}

void DoSaveAs()
{
  PWGetFilename(pixmap_widget, &filename);
 RetrySave:
#ifndef USE_ATHENA
  if (PopupFDialog(file_dialog, "Save file:", 
		   filename, &filename, XtGrabExclusive) == Okay) {
#else /* USE_ATHENA */
  if (PopupFDialog(file_dialog, "Save file:", 
		   filename, &filename, "w") == Okay) {
#endif /* USE_ATHENA */
    if (PWWriteFile(pixmap_widget, filename) != XpmSuccess) {
      sprintf(message, "Can't write file: %s", filename);
      if (PopupDialog(error_dialog, message, 
		      NULL, NULL, XtGrabExclusive) == Retry)
	goto RetrySave;
    }
    else {
      PWClearChanged(pixmap_widget);
      FixStatus();
    }
  }
}

void DoFilename()
{
  PWGetFilename(pixmap_widget, &filename);
#ifndef USE_ATHENA
  if (PopupFDialog(file_dialog, "Change filename:", 
		  filename, &filename, XtGrabExclusive) == Okay) {
#else /* USE_ATHENA */
  if (PopupFDialog(file_dialog, "Change filename:", 
		  filename, &filename, "w") == Okay) {
#endif /* USE_ATHENA */
    PWChangeFilename(pixmap_widget, filename);
    FixStatus();
  }
}

void DoHintsCmt()
{
  PWComments(pixmap_widget, &hints_cmt, &colors_cmt, &pixels_cmt);
  if (PopupDialog(input_dialog, "Pixmap hints comment:", 
		  (hints_cmt ? hints_cmt : ""), &hints_cmt, 
		  XtGrabExclusive) == Okay)
     PWComments(pixmap_widget, &hints_cmt, &colors_cmt, &pixels_cmt);

#ifndef USE_ATHENA /* R6 Athena also frees memory */
  if (hints_cmt) XtFree(hints_cmt);
#endif /* USE_ATHENA */
  if (colors_cmt) XtFree(colors_cmt);
  if (pixels_cmt) XtFree(pixels_cmt);
  hints_cmt = 0;
  colors_cmt = 0;
  pixels_cmt = 0;
}

void DoColorsCmt()
{
  PWComments(pixmap_widget, &hints_cmt, &colors_cmt, &pixels_cmt);
  if (PopupDialog(input_dialog, "Pixmap colors comment:", 
		  (colors_cmt ? colors_cmt : ""), &colors_cmt, 
		  XtGrabExclusive) == Okay)
     PWComments(pixmap_widget, &hints_cmt, &colors_cmt, &pixels_cmt);

  if (hints_cmt) XtFree(hints_cmt);
#ifndef USE_ATHENA /* R6 Athena also frees memory */
  if (colors_cmt) XtFree(colors_cmt);
#endif /* USE_ATHENA */
  if (pixels_cmt) XtFree(pixels_cmt);
  hints_cmt = 0;
  colors_cmt = 0;
  pixels_cmt = 0;
}

void DoPixelsCmt()
{
  PWComments(pixmap_widget, &hints_cmt, &colors_cmt, &pixels_cmt);
  if (PopupDialog(input_dialog, "Pixmap pixels comment:", 
		  (pixels_cmt ? pixels_cmt : ""), &pixels_cmt, 
		  XtGrabExclusive) == Okay)
     PWComments(pixmap_widget, &hints_cmt, &colors_cmt, &pixels_cmt);

  if (hints_cmt) XtFree(hints_cmt);
  if (colors_cmt) XtFree(colors_cmt);
#ifndef USE_ATHENA /* R6 Athena also frees memory */
  if (pixels_cmt) XtFree(pixels_cmt);
#endif /* USE_ATHENA */
  hints_cmt = 0;
  colors_cmt = 0;
  pixels_cmt = 0;
}

void DoQuit()
{
  if (PWQueryChanged(pixmap_widget)) {
    PWGetFilename(pixmap_widget, &filename);
  RetryQuit:
    switch (PopupDialog(qsave_dialog, "Save file before quitting?",
			filename, &filename, XtGrabExclusive)) {
    case Yes:
      if (PWWriteFile(pixmap_widget, filename) != XpmSuccess) {
	sprintf(message, "Can't write file: %s", filename);
	if (PopupDialog(error_dialog, message, 
			NULL, NULL, XtGrabExclusive) == Retry) 
	  goto RetryQuit;
	else return;
      }
      break;
      
    case Cancel:
      return;
    }
  }
  exit(0);
}

void DoAddColor()
{
  char *cname;
  
  if (PopupDialog(input_dialog, "Color name (name or #rgb) to add:",
		  "", &cname, XtGrabExclusive) == Okay)
    {
      XColor xcolor;
      
      if ((!XParseColor(dpy, cmap, cname, &xcolor)) ||
	  (!XAllocColor(dpy, cmap, &xcolor)))
	{
	  fprintf(stderr, 
		  "Warning: color %s could not be parsed/allocated!",
		  cname);
	}
      else if (PWGetColor(pixmap_widget, xcolor.pixel) == NULL)
	{
	  PWUseColorInTable(pixmap_widget, xcolor.pixel, NULL, NULL, NULL,
			    NULL, NULL, cname);
	}
    }
}

void DoSymbolicName()
{
  char *s_name;
  char message[80];
  PWColorInfo *color = PWGetColor(pixmap_widget, current_color);
  
  sprintf(message, "Color `%s' [%d] symbolic name:", 
	  color->c_name, current_color);
  
  if (PopupDialog(input_dialog, message, 
		  (color->s_name ? color->s_name : ""), 
		  &s_name,
		  XtGrabExclusive) == Okay)
    PWUpdateColorInTable(pixmap_widget, 
			 current_color,
			 color->symbol,
			 s_name,
			 color->m_name,
			 color->g4_name,
			 color->g_name,
			 color->c_name);
}

void DoMonochromeName()
{
  char *m_name;
  char message[80];
  PWColorInfo *color = PWGetColor(pixmap_widget, current_color);
  
  sprintf(message, "Color `%s' [%d] monochrome display name:", 
	  color->c_name, current_color);
  
  if (PopupDialog(input_dialog, message, 
		  (color->m_name ? color->m_name : ""), 
		  &m_name,
		  XtGrabExclusive) == Okay)
  {
      XColor xcolor;
      
      if (!XParseColor(dpy, cmap, m_name, &xcolor))
	{
	  fprintf(stderr, "Warning: color %s could not be parsed!\n",
		  m_name);
	}
      else
	PWUpdateColorInTable(pixmap_widget, 
			     current_color,
			     color->symbol,
			     color->s_name,
			     m_name,
			     color->g4_name,
			     color->g_name,
			     color->c_name);
  }
}

void DoGrey4Name()
{
  char *g4_name;
  char message[80];
  PWColorInfo *color = PWGetColor(pixmap_widget, current_color);
  
  sprintf(message, "Color `%s' [%d] grey scale 4 display name:", 
	  color->c_name, current_color);
  
  if (PopupDialog(input_dialog, message, 
		  (color->g4_name ? color->g4_name : ""), 
		  &g4_name,
		  XtGrabExclusive) == Okay)
  {
      XColor xcolor;
      
      if (!XParseColor(dpy, cmap, g4_name, &xcolor))
	{
	  fprintf(stderr, "Warning: color %s could not be parsed!\n",
		  g4_name);
	}
      else
	PWUpdateColorInTable(pixmap_widget, 
			     current_color,
			     color->symbol,
			     color->s_name,
			     color->m_name,
			     g4_name,
			     color->g_name,
			     color->c_name);
  }
}

void DoGreyName()
{
  char *g_name;
  char message[80];
  PWColorInfo *color = PWGetColor(pixmap_widget, current_color);

  sprintf(message, "Color `%s' [%d] grey scale display name:", 
	  color->c_name, current_color);
  
  if (PopupDialog(input_dialog, message, 
		  (color->g_name ? color->g_name : ""), 
		  &g_name,
		  XtGrabExclusive) == Okay)
  {
      XColor xcolor;
      
      if (!XParseColor(dpy, cmap, g_name, &xcolor))
	{
	  fprintf(stderr, "Warning: color %s could not be parsed!\n",
		  g_name);
	}
      else
	PWUpdateColorInTable(pixmap_widget, 
			     current_color,
			     color->symbol,
			     color->s_name,
			     color->m_name,
			     color->g4_name,
			     g_name,
			     color->c_name);
  }
}

void DoColorName()
{
  char *c_name;
  char message[80];
  PWColorInfo *color = PWGetColor(pixmap_widget, current_color);

  sprintf(message, "Color `%s' [%d] color display name:", 
	  color->c_name, current_color);
/*  RetryColorCname: */
  if ((PopupDialog(input_dialog, message, 
		   (color->c_name ? color->c_name : ""), 
		   &c_name,
		   XtGrabExclusive) == Okay) && (c_name))
  {
      XColor xcolor;
      
/*      if (strcasecmp(c_name, NoColorName) == 0)
	{
	  if (PopupDialog(error_dialog, "Can't rename color to None",
			  NULL, NULL, XtGrabExclusive) == Retry)
	    goto RetryColorCname;
	}
      else*/ if ((strcasecmp(c_name, NoColorName) != 0) &&
		 (!XParseColor(dpy, cmap, c_name, &xcolor)))
	{
	  fprintf(stderr, "Warning: color %s could not be parsed!\n",
		  c_name);
	}
      else
	{
	  FixColorMenuLabel(color->c_name, c_name);
	  PWUpdateColorInTable(pixmap_widget, 
			       current_color,
			       color->symbol,
			       color->s_name,
			       color->m_name,
			       color->g4_name,
			       color->g_name,
			       c_name);
	}
    }
}

void SwitchImage()
{
    if (image_visible) {
      XtPopdown(image_shell);
      image_visible = False;
    }
    else {
      Position image_x, image_y;
      int n;
      Arg wargs[3];
      
      XtTranslateCoords(XtParent(pixmap_widget), 
			10, 10, &image_x, &image_y);
      
      n = 0;
      XtSetArg(wargs[n], XtNx, image_x); n++;
      XtSetArg(wargs[n], XtNy, image_y); n++;
      XtSetValues(image_shell, wargs, n);
      
      image_visible = True;
      
      XtPopup(image_shell, XtGrabNone);
      unsetKillfromWM(image_shell);
      FixImage(pixmap_widget, NULL, NULL);
    }
}

void SwitchGrid()
{
  PWSwitchGrid(pixmap_widget);
}

void SwitchAxes()
{
  PWSwitchAxes(pixmap_widget);
}

void SwitchProportional()
{
  PWSwitchProportional(pixmap_widget);
}

void SwitchZoom()
{
  if (PWQueryZooming(pixmap_widget)) {
    PWZoomOut(pixmap_widget);
    PWChangeNotify(pixmap_widget, NULL, NULL);
  }
  else {
    if (PWQueryMarked(pixmap_widget)) {
      PWStoreToBuffer(pixmap_widget);
      PWZoomMarked(pixmap_widget);
      PWChangeNotify(pixmap_widget, NULL, NULL);
    }
    else {
      PWEngageRequest(pixmap_widget, ZoomInRequest, False, Plain);
    }
  }
}

void DoZoomIn()
{
    Dimension square_size, inc = 2;
    Arg args[2];

    XtSetArg(args[0], XtNsquareSize, &square_size);
    XtGetValues(pixmap_widget, args, 1);
    square_size += inc;
    XtSetArg(args[0], XtNsquareSize, square_size);
    XtSetValues(pixmap_widget, args, 1);
#ifdef USE_ATHENA
    PWRedraw(pixmap_widget);
#endif /* USE_ATHENA */
}

void DoZoomOut()
{
    Dimension square_size, inc = 2;
    Arg args[2];

    XtSetArg(args[0], XtNsquareSize, &square_size);
    XtGetValues(pixmap_widget, args, 1);
    square_size -= inc;
    if (square_size > 0)
      {
	  XtSetArg(args[0], XtNsquareSize, square_size);
	  XtSetValues(pixmap_widget, args, 1);
#ifdef USE_ATHENA
	  PWRedraw(pixmap_widget);
#endif /* USE_ATHENA */
      }
}

void DoZoomFactor()
{
    Dimension square_size;
    Arg args[2];
    char message[80], buffer[80], *returned;

    XtSetArg(args[0], XtNsquareSize, &square_size);
    XtGetValues(pixmap_widget, args, 1);
    sprintf(buffer, "%d", square_size);
    strcpy(message, "Set Zoom Factor:");
    if ((PopupDialog(input_dialog, message, 
		     buffer, &returned, XtGrabExclusive) == Okay) &&
	(returned))
      {
	  if ((square_size = atoi(returned)) > 0)
	    {
		XtSetArg(args[0], XtNsquareSize, square_size);
		XtSetValues(pixmap_widget, args, 1);
#ifdef USE_ATHENA
		PWRedraw(pixmap_widget);
#endif /* USE_ATHENA */
	    }
      }
}

    
void DoResize()
{
  char x;
  int width, height;

  format = "";
 RetryResize:
  if (PopupDialog(input_dialog, "Resize to WIDTHxHEIGHT:", 
		  format, &format, XtGrabExclusive) == Okay) {
    sscanf(format, "%d%c%d", &width, &x, &height);
    if ((width >0) && (height > 0) && (x == 'x')) {
      PWStoreToBuffer(pixmap_widget);
      PWResize(pixmap_widget, (Dimension)width, (Dimension)height);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      FixStatus();
    }
    else {
      sprintf(message, "Wrong format: %s", format);
      if (PopupDialog(error_dialog, message, 
		      NULL, NULL, XtGrabExclusive) == Retry)
	goto RetryResize;
    }
  }
}

void DoRescale()
{
  char x;
  int width, height;

  format = "";
 RetryRescale:
  if (PopupDialog(input_dialog, "Rescale to WIDTHxHEIGHT:", 
		  format,	&format, XtGrabExclusive) == Okay) {
    sscanf(format, "%d%c%d", &width, &x, &height);
    if ((width >0) && (height > 0) && (x == 'x')) {
      PWStoreToBuffer(pixmap_widget);
      PWRescale(pixmap_widget, (Dimension)width, (Dimension)height);
      PWChangeNotify(pixmap_widget, NULL, NULL);
      PWSetChanged(pixmap_widget);
      FixStatus();
    }
    else {
      sprintf(message, "Wrong format: %s", format);
      if (PopupDialog(error_dialog, message, 
		      NULL, NULL, XtGrabExclusive) == Retry)
	goto RetryRescale;
    }
  }
}


void DoCut()
{
  PWStore(pixmap_widget);
  PWStoreToBuffer(pixmap_widget);
  PWClearMarked(pixmap_widget);
  PWUnmark(pixmap_widget);
  PWChangeNotify(pixmap_widget, NULL, NULL);
}

void DoCopy()
{
  PWStore(pixmap_widget);
  PWUnmark(pixmap_widget);
}

void DoPaste()
{
  PWEngageRequest(pixmap_widget, RestoreRequest, False, Plain);
}

void DoCrop()
{
  if (!PWQueryMarked(pixmap_widget) && !PWQueryStored(pixmap_widget)) return;
  
  PWStoreToBuffer(pixmap_widget);
  PWRotateBufferAndImage(pixmap_widget);
#ifdef USE_ATHENA
  PWRedraw(pixmap_widget);
#endif /* USE_ATHENA */
}

void InfoCallback()
{
    if (info_str == NULL)
      {
	info_str = XtMalloc((strlen(info) + 1) * sizeof(char));
	sprintf(info_str, info, PATCHLEVEL);
      }

    PopupDialog(info_dialog, info_str,
		NULL, NULL, XtGrabExclusive);
}



#include "PortEdit.c"
#include "ExtEdit.c"


/* PixmapWidget callback for drawing a point */
#if NeedFunctionPrototypes
void DrawPointExtensions(Widget w, Position x, Position y, int value )
#else
void DrawPointExtensions( w, x, y, value )
    Widget	w;
    Position	x;
    Position	y;
    int		value;
#endif
{
    DrawIfPort( w, x, y, value );
}


/* PixmapWidget callback for redrawing/refreshing everything */ 

#if NeedFunctionPrototypes
void DrawExtensions(Widget w, int value )
#else
void DrawExtensions( w, value )
    Widget w;
    int	   value;
#endif
{
    RedrawPorts( w, value );
}


/* PixmapWidget callback for translating by dx, dy */

#if NeedFunctionPrototypes
void TranslateExtensions(Widget w, Position dx, Position dy )
#else
void TranslateExtensions( w, dx, dy )
    Widget	w;
    Position	dx;
    Position	dy;
#endif
{
    TranslatePorts( w, dx, dy );
}


/* PixmapWidget callback for rotating right/left */

#if NeedFunctionPrototypes
void RotateExtensions(Widget w, enum RotateDirection direction )
#else
void RotateExtensions( w, direction )
    Widget			w;
    enum RotateDirection	direction;
#endif
{
    RotatePorts( w, direction );
}


/* PixmapWidget callback for flipping horizontal/vertical */

#if NeedFunctionPrototypes
void FlipExtensions(Widget w, enum FlipAxis axis )
#else
void FlipExtensions( w, axis )
    Widget		w;
    enum FlipAxis	axis;
#endif
{
    FlipPorts( w, axis );
}



/* loading pixmap editors colors */
void loadPixEditColors()
{
  FILE *colorfile;
  static char *fname = ".pixmap";
  char filename[256];


  /* first try to open in local dir */
  if (!(colorfile = fopen(fname, "r")))
    { /* try in homedir */
      sprintf(filename, "%s/%s", getenv("HOME"), fname);
      if (!(colorfile = fopen(filename, "r")))
	{ /* try in X11 lib dir */
	  sprintf(filename, "%s/%s", APPDEFDIR , fname);
	  colorfile = fopen(filename, "r");
	}
    }
  
  if (colorfile) 
    { /* parse it, i.e., read name, try to alloc in cmap, add menu entry */
      char cname[512];
      int status;
      XColor xcolor;
      
      while ((status = fscanf(colorfile,"%[^\n]\n", cname))
	     && (status != EOF))
	{
	  if (!XParseColor(dpy, cmap, cname, &xcolor)) continue;
	  if (!XAllocColor(dpy, cmap, &xcolor))
	    {
	      fprintf(stderr, "Warning: color %s could not be allocated!\n",
		      cname);
	      continue;
	    }
	  else
	    {
	      PWUseColorInTable(pixmap_widget,  xcolor.pixel, NULL, NULL,
				NULL, NULL, NULL, cname);
	    }
	}
      fclose(colorfile);
    }
  else
    { /* allocate black in colormap */
      PWUseColorInTable(pixmap_widget, black,
			NULL, NULL, NULL, NULL, NULL, "black");
    }
}

void MakeMenuPanel(father, menu, num, callback)
     Widget father;
     ButtonRec	   menu[];
     Cardinal       num;
     XtCallbackProc callback;
{
    Widget	bw;
    int		i;

    for (i = 0; i < num; i++) {
#ifndef USE_ATHENA
	if (menu[i].id == Dummy)
	  bw = XmCreateSeparatorGadget(father, menu[i].name, NULL, 0);
	else if (menu[i].trap == TOGGLE)
	  {
	      bw = XmCreateToggleButtonGadget(father, menu[i].name, NULL, 0);
	      XtAddCallback(bw, XmNvalueChangedCallback, callback,
			    (XtPointer)&menu[i].id);
	  }
	else
	  {
	      bw =  XmCreatePushButtonGadget(father, menu[i].name, NULL, 0);
	      XtAddCallback(bw, XmNactivateCallback, callback,
			    (XtPointer)&menu[i].id);
	  }
	menu[i].widget = bw;
	XtManageChild(bw);
#else /* USE_ATHENA */
        bw = XtCreateManagedWidget( menu[i].name,
                                    ((menu[i].id != Dummy) ?
                                      smeBSBObjectClass : smeLineObjectClass),
                                    father, NULL, 0);
        XtAddCallback( bw, XtNcallback, callback, (XtPointer) &menu[i].id );
        menu[i].widget = bw;
#endif /* USE_ATHENA */
    }
}

static Widget
MakeMenu( menubar, menuName, buttonName, menu, num, callback )
    Widget	   menubar;
    String	   menuName;
    String	   buttonName;
    ButtonRec	   menu[];
    Cardinal       num;
    XtCallbackProc callback;
{
    Widget	mw;
    Widget	bw;

#ifndef USE_ATHENA
    Arg wargs[1];

    mw = XmCreatePulldownMenu(menubar, menuName, NULL, 0);

    XtSetArg(wargs[0], XmNsubMenuId, mw);
    bw = XmCreateCascadeButtonGadget(menubar, buttonName, wargs, 1);
    XtManageChild(bw);
    if (menu == edit_menu) 
      XtAddCallback(bw, XmNcascadingCallback, FixMenuCB, NULL);
#else /* USE_ATHENA */
    mw = XtCreatePopupShell( menuName, simpleMenuWidgetClass,
                             menubar, NULL, 0 );

    bw = XtCreateManagedWidget( buttonName, menuButtonWidgetClass, 
                                menubar, NULL, 0 );
#endif /* USE_ATHENA */
    
    MakeMenuPanel(mw, menu, num, callback);

    return mw;
}



void main(argc, argv)
    int    argc;
    char  *argv[];
{
    int i, n;
    Arg wargs[5];
    Widget w, pview_widget, cview_widget, vpane_widget;
#ifndef USE_ATHENA
    Widget formh;
    static int Resize_id = Resize, Filename_id = Filename;
#else /* USE_ATHENA */
    Widget radio_group; XtPointer radio_data;
#endif /* USE_ATHENA */
    int x, y;
    unsigned int width = 0, height = 0;
    Resources resources;
    
    top_widget = XtAppInitialize(&pixmap_context, "Pixmap", 
				 options, XtNumber(options), 
				 &argc, argv, NULL, NULL, 0);
    XtGetApplicationResources(top_widget, &resources, appResources, 
			      XtNumber(appResources), NULL, 0);	
#ifndef USE_ATHENA
#ifndef NO_EDITRES
    XtAddEventHandler(top_widget, 0, True, _XEditResCheckMessages, NULL);
#endif
#endif /* USE_ATHENA */

    if (argc > 1) {
      if ((argc ==3) && (strcmp(argv[1], "-size") == 0))
	  XParseGeometry(argv[2], &x, &y, &width, &height);
      else
	{
	  fprintf(stderr, "%s %s", argv[0], usage);
	  exit (-1);
	}
    }

    /* some intializations */
    dpy = XtDisplay(top_widget);
    screen = DefaultScreen(dpy);
    ncolors = 1<<DisplayPlanes(dpy, screen);
#ifdef DEBUG
    printf("Num colors %d\n",ncolors);
    printf("Transparent entry in colormap %d\n", TRANSPARENT(dpy, screen));
    XSynchronize(dpy, True);
#endif /* DEBUG */

    if (resources.privateColormap) {
	XColor xcolor;
	/* create a private colormap */
	cmap = XCreateColormap(dpy, DefaultRootWindow(dpy),
			       DefaultVisual(dpy, screen), AllocNone);
	/* allocate black and white  */
	xcolor.red = 0xffff;
	xcolor.blue = 0xffff;
	xcolor.green = 0xffff;
	XAllocColor(dpy, cmap, &xcolor);
	white = xcolor.pixel;
	xcolor.red = 0;
	xcolor.blue = 0;
	xcolor.green = 0;
	XAllocColor(dpy, cmap, &xcolor);
	black = xcolor.pixel;

	n = 0;
	XtSetArg(wargs[n], XtNcolormap, cmap); n++;
	XtSetValues(top_widget, wargs, n);
    } else {
	cmap = DefaultColormap(dpy,screen);
	white = WhitePixel(dpy,screen);
	black = BlackPixel(dpy,screen);
    }

#ifdef USE_ATHENA
    check_mark = XCreateBitmapFromData(XtDisplay(top_widget),
				      RootWindowOfScreen(XtScreen(top_widget)),
				      xlogo16_bits, 
				      xlogo16_width, 
				      xlogo16_height);
#else
#if XmVersion > 1001 /* Motif 1.2 or > */
    /* Disable Motif 1.2 drag and drop */
    XtVaSetValues(XmGetXmDisplay(dpy), XmNdragInitiatorProtocolStyle, 
		  XmDRAG_NONE, NULL);
#endif
#endif /* USE_ATHENA */
    XtAppAddActions(pixmap_context, actions_table, XtNumber(actions_table));
#ifndef USE_ATHENA
    parent_widget = XtCreateWidget("parent", xmFormWidgetClass,
				   top_widget, NULL, 0);
    formy_widget = XmCreateMenuBar(parent_widget, "formy", NULL, 0);
    
#else /* USE_ATHENA */

    parent_widget = XtCreateWidget("parent", panedWidgetClass,
				   top_widget, NULL, 0);

    formy_widget = XtCreateWidget("formy", formWidgetClass,
				  parent_widget, NULL, 0);

#endif /* USE_ATHENA */

    MakeMenu(formy_widget, "fileMenu", "fileButton",
	     file_menu, XtNumber(file_menu), TheCallback);
        
    MakeMenu(formy_widget, "editMenu", "editButton",
             edit_menu, XtNumber(edit_menu), TheCallback );

    MakeMenu(formy_widget, "colorMenu", "colorButton",
             color_menu, XtNumber(color_menu), TheCallback );

    extensionMenu_widget = 
      MakeMenu(formy_widget, "extensionMenu", "extensionButton",
               extension_menu, XtNumber(extension_menu),
	       ExtensionMenuCallback);

#ifndef USE_ATHENA
    status_widget = XmCreateCascadeButtonGadget(formy_widget, "status",
				                NULL, 0);
    XtManageChild(status_widget);
    XtAddCallback(status_widget, XmNactivateCallback, TheCallback, 
                  &Filename_id);
    statusb_widget = XmCreateCascadeButtonGadget(formy_widget, "statusb",
			 	                 NULL, 0);
    XtManageChild(statusb_widget);
    XtAddCallback(statusb_widget, XmNactivateCallback, TheCallback, 
                  &Resize_id);


    infoButton_widget = XmCreateCascadeButtonGadget(formy_widget, 
						 "infoButton", NULL, 0);
    XtManageChild(infoButton_widget);
    XtVaSetValues(formy_widget, 
		  XmNmenuHelpWidget, infoButton_widget,
		  NULL, NULL);
    XtAddCallback(infoButton_widget, XmNactivateCallback, (XtCallbackProc)InfoCallback,
		  NULL);

    n = 0;
    XtSetArg(wargs[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(wargs[n], XmNtopWidget, formy_widget); n++;
    pane_widget = XmCreateForm(parent_widget, "pane", wargs, n);

    form_widget = XmCreateRadioBox(pane_widget, "form", NULL, 0);
    XtSetArg(wargs[0], XmNisHomogeneous, False);
    XtSetValues(form_widget, wargs, 1);
        
    for (i = 0; i < XtNumber(buttons); i++) {
      if (buttons[i].trap == TOGGLE)
	{
	  w = XmCreateToggleButtonGadget(form_widget, buttons[i].name, 
					 NULL, 0);
	  XtAddCallback(w, XmNvalueChangedCallback, TheCallback, 
			&buttons[i].id);
	}
      else 
	{
	  if ((!strcmp(buttons[i].name, "flipHoriz")) ||
	      (!strcmp(buttons[i].name, "left")) ||
	      (!strcmp(buttons[i].name, "rotateLeft")))
	    {
	      formh = form_widget;
	      form_widget = XmCreateRowColumn(formh, "formh", NULL, 0);
	      XtManageChild(form_widget);
	    }
	  
	  w = XmCreatePushButtonGadget(form_widget, buttons[i].name, NULL, 0);
	  XtAddCallback(w, XmNactivateCallback, TheCallback, &buttons[i].id);

	  if ((!strcmp(buttons[i].name, "flipVert")) ||
	      (!strcmp(buttons[i].name, "right")) ||
	      (!strcmp(buttons[i].name, "rotateRight")))
	    form_widget = formh;
	}

      buttons[i].widget = w;
      XtManageChild(w);
      
      if (buttons[i].id == Point) 
	XmToggleButtonGadgetSetState(buttons[i].widget, True, False);
    }

    n = 0;
    XtSetArg(wargs[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(wargs[n], XmNleftWidget, form_widget); n++;
    vpane_widget = (Widget) XmCreatePanedWindow(pane_widget, "vPane", 
						wargs, n);

    /* the color pane should be created before the pixmap widget as the
       creation of the pixmap widget will ``add'' a new color in the pane */
    n = 0;
    cview_widget = (Widget)XmCreateScrolledWindow(vpane_widget, "colorView",
						 wargs, n);
    n = 0;
    color_pane_widget = XmCreateRowColumn(cview_widget, "colorPane",
					  wargs, n);
    XmScrolledWindowSetAreas(cview_widget,
			     XmCreateScrollBar(cview_widget, "cviewHS",
					       NULL, 0),
			     XmCreateScrollBar(cview_widget, "cviewVS",
					       NULL, 0),
			     color_pane_widget);

    n = 0;
    pview_widget = (Widget)XmCreateScrolledWindow(vpane_widget, "pixmapView",
						 wargs, n);

#else /* USE_ATHENA */

    infoButton_widget = XtCreateManagedWidget("infoButton",
					commandWidgetClass, 
					formy_widget, NULL, 0);
    status_widget = XtCreateManagedWidget("status", labelWidgetClass,
					  formy_widget, NULL, 0);

    XtAddCallback(infoButton_widget, XtNcallback, InfoCallback, NULL);
    pane_widget = XtCreateWidget("pane", panedWidgetClass,
				 parent_widget, NULL, 0);

    form_widget = XtCreateWidget("form", formWidgetClass, 
				 pane_widget, NULL, 0);
        
    for (i = 0; i < XtNumber(buttons); i++) {
	w = XtCreateManagedWidget(buttons[i].name, 
				  ((buttons[i].trap == TOGGLE) ? 
				   toggleWidgetClass : commandWidgetClass),
				  form_widget, NULL, 0);
	XtAddCallback(w,
		      XtNcallback,
		      TheCallback,
		      &buttons[i].id);

	buttons[i].widget = w;

	if (buttons[i].id == Point) {
	    radio_group = buttons[i].widget;
	    radio_data  = buttons[i].name;
	}
    }

    vpane_widget = XtCreateWidget("vPane", panedWidgetClass,
				  pane_widget, NULL, 0);

    /* the color pane should be created before the pixmap widget as the
       creation of the pixmap widget will ``add'' a new color in the pane */
    cview_widget = XtCreateWidget("colorView", viewportWidgetClass,
				  vpane_widget, NULL, 0);

    color_pane_widget = XtCreateWidget("colorPane", boxWidgetClass,
				       cview_widget, NULL, 0);

    pview_widget = XtCreateWidget("pixmapView", viewportWidgetClass,
				  vpane_widget, NULL, 0);
#endif /* USE_ATHENA */

    /* foreach extension editor, create and manage editor buttons */ 
    CreatePortEditorButtons(form_widget,
#ifndef USE_ATHENA
			    NULL);
#else /* USE_ATHENA */
			    radio_group);
#endif /* USE_ATHENA */
    ManagePortEditorButtons(True); 

    n = 0;
    XtSetArg(wargs[n], XtNaddColorNtfyProc,
	     (AddColorNotifyProc)colorToAdd); n++;
    XtSetArg(wargs[n], XtNextensionNtfyProc,
	     (ExtensionNotifyProc)FixExtensionMenu); n++;
    if (width) XtSetArg(wargs[n], XtNpixmapWidth, width), n++;
    if (height) XtSetArg(wargs[n], XtNpixmapHeight, height), n++;
    pixmap_widget = XtCreateManagedWidget("pixmap", pixmapWidgetClass,
					  pview_widget, wargs, n);

    /* register extension callbacks with pixmap widget */
    PWSetDrawPointProc(pixmap_widget, DrawPointExtensions);
    PWSetRedrawProc(   pixmap_widget, DrawExtensions);
    PWSetTranslateProc(pixmap_widget, TranslateExtensions);
    PWSetRotateProc(   pixmap_widget, RotateExtensions);
    PWSetFlipProc(     pixmap_widget, FlipExtensions);

#ifndef USE_ATHENA

    XmScrolledWindowSetAreas(pview_widget,
			     XmCreateScrollBar(pview_widget, "pviewHS",
					       NULL, 0),
			     XmCreateScrollBar(pview_widget, "pviewVS",
					       NULL, 0),
			     pixmap_widget);

#endif /* USE_ATHENA */ 

    XtManageChild(formy_widget);
    XtManageChild(form_widget);
    XtManageChild(color_pane_widget);
    XtManageChild(pview_widget);
    XtManageChild(cview_widget);
    XtManageChild(vpane_widget);
    XtManageChild(pane_widget);
    XtManageChild(parent_widget);

    XtSetKeyboardFocus(top_widget, pixmap_widget);
    
    PWSetForeground(pixmap_widget, black);
    current_color = black;

    loadPixEditColors();
    
    image_shell = XtCreatePopupShell("image", transientShellWidgetClass,
				     top_widget, NULL, 0);
#ifndef USE_ATHENA
    XtSetArg(wargs[0], XmNlabelString, 
	     XmStringCreateLtoR("", XmSTRING_DEFAULT_CHARSET));
    image_widget = XmCreateLabel(image_shell, "label", wargs, 1);
    XtManageChild(image_widget);
#else /* USE_ATHENA */
    image_widget = XtCreateManagedWidget("label", labelWidgetClass,
					 image_shell, NULL, 0);
#endif /* USE_ATHENA */

    Notify(pixmap_widget, FixImage);
    ColorNotify(pixmap_widget, (UseColorNotifyProc)FixColor);

    FixStatus();
    
    info_dialog  = CreateDialog(top_widget, "info", Okay);
    input_dialog = CreateDialog(top_widget, "input", Okay | Cancel);
    error_dialog = CreateDialog(top_widget, "error", Abort | Retry);    
    qsave_dialog = CreateDialog(top_widget, "qsave", Yes | No | Cancel);
    file_dialog  = CreateFDialog(top_widget, "file", Okay | Cancel);

    extensionEditor = CreateExtensionEditor(top_widget, "extEditor");

    textOptions = CreateTextOptions(top_widget, "textOptions");
    PWSetFont(pixmap_widget, textOptions->font_struct);
    PWSetText(pixmap_widget, textOptions->text_string);

#ifdef USE_ATHENA
    XawToggleSetCurrent(radio_group, radio_data);
#endif /* USE_ATHENA */
    PWEngageRequest(pixmap_widget, PointRequest, True, Plain);

    XtRealizeWidget(top_widget);
    XtAppMainLoop(pixmap_context);
}
