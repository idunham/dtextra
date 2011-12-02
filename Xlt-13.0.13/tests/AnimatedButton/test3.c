/* $Id: test3.c,v 1.5 2002/04/12 17:19:46 rwscott Exp $ */

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


static XrmOptionDescRec opTable[] = {
	{"-delay","*AnimatedButton.frameDelay",XrmoptionSepArg, NULL},
};

int
main(int argc, char **argv)
{
    Widget toplevel, one;
    int i;
    XtAppContext app;
    Pixmap *pixmaps;
    XpmAttributes attrib;
    Pixel bg;
    XpmColorSymbol colors[1];

    XtSetLanguageProc(NULL, NULL, NULL);

    toplevel = XtVaAppInitialize(&app, 
	"AnimatedButtonTest", 
    	opTable, XtNumber(opTable), 
    	&argc, argv, 
    	NULL, 
    	NULL);

    one = XltCreateAnimatedButton(toplevel, "AnimatedButton", NULL, 0);

    XtVaGetValues(one,
    	XmNbackground, &bg,
    	NULL);
    colors[0].name = NULL;
    colors[0].value = "None";
    colors[0].pixel = bg;
    attrib.valuemask = XpmCloseness | XpmColorSymbols;
    attrib.closeness = 40000;
    attrib.colorsymbols = colors;
    attrib.numsymbols = 1;
    pixmaps = (Pixmap *)XtMalloc((argc - 1) * sizeof(Pixmap));
    for (i = 1; i < argc; i++)
    {
	XpmReadFileToPixmap(XtDisplay(one),
				    XRootWindowOfScreen(XtScreen(one)),
				    argv[i],
				    &pixmaps[i - 1],
				    NULL,
				    &attrib);
    }
    if (argc > 1)
    {
	XtVaSetValues(one,
		  XmNlabelPixmap, pixmaps[0],
		  XmNlabelType, XmPIXMAP,
		  XltNanimationPixmapCount, argc - 1,
		  XltNanimationPixmaps, pixmaps,
		  XltNanimate, True,
		  NULL);
    }
    XtManageChild(one);

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app);
    exit(0);
}
