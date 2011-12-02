/* $Header: /cvsroot/xlt/Xlt/tests/FontChooser/test1.c,v 1.2 2001/06/09 16:34:18 amai Exp $ */

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>

#include <../../lib/Xlt.h>
#include <../../lib/FontChooser.h>


static char *FallBack[] =
{
	NULL
};

int
main(int argc, char **argv)
{
  Widget toplevel, one;
  XtAppContext app;

  XtSetLanguageProc(NULL, NULL, NULL);

  toplevel = XtVaAppInitialize(&app, "FontChooser", NULL, 0, &argc,
			       argv, FallBack, NULL); 

  one = XltCreateFontChooser(toplevel, "FontChooser", NULL, 0);
  XtManageChild(one);

  XtRealizeWidget(toplevel);
  XtAppMainLoop(app);
  exit(0);
}
