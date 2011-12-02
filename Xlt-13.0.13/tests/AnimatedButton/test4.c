/* $Id: test4.c,v 1.8 2002/04/12 17:19:48 rwscott Exp $ */

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>

#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <AnimatedButton.h>
#ifdef HAVE_XM_XPMP_H
#include <Xm/XpmP.h>
#else
#include <X11/xpm.h>
#endif

#include "eye1.xpm"
#include "eye2.xpm"
#include "eye3.xpm"
#include "eye4.xpm"
#include "eye5.xpm"
#include "eye6.xpm"
#include "eye7.xpm"
#include "eye8.xpm"
#include "eye9.xpm"
#include "eye10.xpm"
#include "eye11.xpm"
#include "eye12.xpm"
#include "eye13.xpm"
#include "eye14.xpm"
#include "eye15.xpm"
#include "eye16.xpm"
#include "eye17.xpm"
#include "eye18.xpm"
#include "eye19.xpm"
#include "eye20.xpm"
#include "eye21.xpm"
#include "eye22.xpm"
#include "eye23.xpm"
#include "eye24.xpm"

void
debug(void)
{
}

static void
Callback(Widget W)
{
Widget w;
Boolean state;

	w = XtNameToWidget(XtParent(W), "*AnimatedButton1");
	XtVaGetValues(w,
		XltNanimate, &state,
		NULL);
	XtVaSetValues(w,
		XltNanimate, !state,
		NULL);
}

char *Program = "";

int
main(int argc, char **argv)
{
  Widget toplevel, rc, one;
  XtAppContext app;

  XtSetLanguageProc(NULL, NULL, NULL);

  toplevel = XtVaAppInitialize(&app, "AnimatedButtonTest", NULL, 0, &argc, argv, NULL, NULL);

  rc = XmCreateRowColumn(toplevel, "RC", NULL, 0);

  one = XltCreateAnimatedButton(rc, "AnimatedButton1", NULL, 0);
  XtAddCallback(one, XmNactivateCallback, (XtCallbackProc)Callback, NULL);
  {
  int i;
  XmStringTable Labels;

  	Labels = (XmStringTable)XtMalloc(50 * sizeof(XmString));
  	i = 0;
	Labels[i] = XmStringCreateSimple("         "); i++;
	Labels[i] = XmStringCreateSimple("        A"); i++;
	Labels[i] = XmStringCreateSimple("       An"); i++;
	Labels[i] = XmStringCreateSimple("      Ani"); i++;
	Labels[i] = XmStringCreateSimple("     Anim"); i++;
	Labels[i] = XmStringCreateSimple("    Animi"); i++;
	Labels[i] = XmStringCreateSimple("   Animat"); i++;
	Labels[i] = XmStringCreateSimple("  Animati"); i++;
	Labels[i] = XmStringCreateSimple(" Animatio"); i++;
	Labels[i] = XmStringCreateSimple("Animation"); i++;
	Labels[i] = XmStringCreateSimple("Animation"); i++;
	Labels[i] = XmStringCreateSimple("         "); i++;
	Labels[i] = XmStringCreateSimple("         "); i++;
	Labels[i] = XmStringCreateSimple("Animation"); i++;
	Labels[i] = XmStringCreateSimple("Animation"); i++;
	Labels[i] = XmStringCreateSimple("         "); i++;
	Labels[i] = XmStringCreateSimple("         "); i++;
	Labels[i] = XmStringCreateSimple("Animation"); i++;
	Labels[i] = XmStringCreateSimple("Animation"); i++;
	Labels[i] = XmStringCreateSimple("nimation "); i++;
	Labels[i] = XmStringCreateSimple("imation  "); i++;
	Labels[i] = XmStringCreateSimple("mation   "); i++;
	Labels[i] = XmStringCreateSimple("ation    "); i++;
	Labels[i] = XmStringCreateSimple("tion     "); i++;
	Labels[i] = XmStringCreateSimple("ion      "); i++;
	Labels[i] = XmStringCreateSimple("on       "); i++;
	Labels[i] = XmStringCreateSimple("n        "); i++;
	XtVaSetValues(one,
	    XmNrecomputeSize, False,
	    XmNalignment, XmALIGNMENT_CENTER,
	    XltNanimationLabelCount, i,
	    XltNanimationLabels, Labels,
	    XltNanimate, True,
	    NULL);
  	for (i = 0; i < i; i++)
  	{
  		XmStringFree(Labels[i]);
  	}
  	XtFree((void *)Labels);
  }
  XtManageChild(one);
  one = XltCreateAnimatedButton(rc, "AnimatedButton2", NULL, 0);
  XtAddCallback(one, XmNactivateCallback, (XtCallbackProc)Callback, NULL);
  {
  char **pixmap_data[] = { 
  	       eye1,  eye2,  eye3,  eye4,  eye5,  eye6,  eye7,  eye8,  eye9,
  	eye10, eye11, eye12, eye13, eye14, eye15, eye16, eye17, eye18, eye19,
  	eye20, eye21, eye22, eye23, eye24 };
  Pixmap *pixmaps;
  Pixmap mask;
  XpmAttributes attrib;
  int i;

  	pixmaps = (Pixmap *)XtMalloc(XtNumber(pixmap_data) * sizeof(Pixmap));

	attrib.valuemask = 0;
	for (i = 0; i < XtNumber(pixmap_data); i++)
	{
		XpmCreatePixmapFromData(XtDisplay(one),
		    XRootWindowOfScreen(XtScreen(one)),
		    pixmap_data[i],
		    &pixmaps[i],
		    &mask,
		    &attrib);
	}
	XtVaSetValues(one,
	    XmNlabelPixmap, pixmaps[0],
	    XmNlabelType, XmPIXMAP,
	    XmNalignment, XmALIGNMENT_CENTER,
	    XltNanimationPixmapCount, i,
	    XltNanimationPixmaps, pixmaps,
	    XltNanimate, True,
	    NULL);
	XtVaSetValues(one,
	    XmNrecomputeSize, False,
	    NULL);
  }
  XtManageChild(one);

  XtManageChild(rc);

  XtRealizeWidget(toplevel);
  XtAppMainLoop(app);

  exit(0);
}
