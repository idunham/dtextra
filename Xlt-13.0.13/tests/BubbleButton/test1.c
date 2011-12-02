/* $Id: test1.c,v 1.5 2002/04/12 17:19:55 rwscott Exp $ */

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <BubbleButton.h>
#ifdef HAVE_XM_XPMP_H
#include <Xm/XpmP.h>
#else
#include <X11/xpm.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#include "dates.xpm"

void
debug(void)
{
}

static void
Callback(Widget W)
{
Widget w;

	w = XtNameToWidget(XtParent(W), "*BubbleButton1");
	XtSetSensitive(w, !XtIsSensitive(w));
}

char *Program = "";

int
main(int argc, char **argv)
{
  Widget toplevel, rc, one;
  XtAppContext app;

  XtSetLanguageProc(NULL, NULL, NULL);

  toplevel = XtVaAppInitialize(&app, "BubbleButtonTest", NULL, 0, &argc, argv, NULL, NULL);

  rc = XmCreateRowColumn(toplevel, "RC", NULL, 0);

  one = XltCreateBubbleButton(rc, "BubbleButton1", NULL, 0);
  XtAddCallback(one, XmNactivateCallback, (XtCallbackProc)Callback, NULL);
  {
  XmString string;

  string = XmStringCreateSimple("MouseOver");
  XtVaSetValues(one,
  	XmNalignment, XmALIGNMENT_CENTER,
  	XltNmouseOverString, string,
  	NULL);
  XmStringFree(string);
  }
  XtManageChild(one);
  one = XltCreateBubbleButton(rc, "BubbleButton2", NULL, 0);
  {
    Pixmap pixmap;
    Pixmap pixmap_insens;
    XpmAttributes attrib;
    GC gc;

    {
	XpmColorSymbol colors[1];

	XtVaGetValues(one,
		      XmNbackground, &colors[0].pixel,
		      NULL);
	colors[0].name = NULL;
	colors[0].value = "None";
	attrib.valuemask = XpmCloseness | XpmColorSymbols;
	attrib.closeness = 40000;
	attrib.colorsymbols = colors;
	attrib.numsymbols = 1;
	XpmCreatePixmapFromData(XtDisplay(one),
				XRootWindowOfScreen(XtScreen(one)),
				dates,
				&pixmap,
				NULL,
				&attrib);

	pixmap_insens = XCreatePixmap(XtDisplay(one),
				      pixmap,
				      attrib.width,
				      attrib.height,
				      DefaultDepthOfScreen(XtScreen(one)));
	gc = XDefaultGCOfScreen(XtScreen(one));
	XSetFunction(XtDisplay(one),gc,GXcopy);
	XCopyArea(XtDisplay(one),
		  pixmap,
		  pixmap_insens,
		  gc,
		  0, 0,
		  attrib.width, attrib.height,
		  0, 0);
	XSetFillStyle(XtDisplay(one), gc, FillStippled);
	XSetStipple(XtDisplay(one), gc,
		    XmGetPixmapByDepth(XtScreen(one),
				       "25_foreground",
				       1 /*WhitePixelOfScreen(XtScreen(ToolBar))*/,
				       0 /*BlackPixelOfScreen(XtScreen(ToolBar))*/,
				       1));
	XFillRectangle(XtDisplay(one),
		       pixmap_insens,
		       gc,
		       0, 0,
		       attrib.width + 1, attrib.height + 1);

	XpmFreeAttributes(&attrib);

	XtVaSetValues(one,
		      XmNalignment, XmALIGNMENT_CENTER,
		      XmNlabelType, XmPIXMAP,
		      XmNlabelPixmap, pixmap,
		      XltNmouseOverPixmap, pixmap_insens,
		      NULL);
    }
  }
  XtAddCallback(one, XmNactivateCallback, (XtCallbackProc)Callback, NULL);
  XtManageChild(one);

  XtManageChild(rc);

  XtRealizeWidget(toplevel);
  XtAppMainLoop(app);

  exit(0);
}
