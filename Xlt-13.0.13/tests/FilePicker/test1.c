/* $Header: /cvsroot/xlt/Xlt/tests/FilePicker/test1.c,v 1.2 2001/06/09 16:34:18 amai Exp $ */

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>

#include <../../lib/Xlt.h>
#include <../../lib/FilePicker.h>


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

  toplevel = XtVaAppInitialize(&app, "FilePicker", NULL, 0, &argc,
			       argv, FallBack, NULL); 

  one = XltCreateFilePicker(toplevel, "FilePicker", NULL, 0);
  XtManageChild(one);

  XtRealizeWidget(toplevel);
  XtAppMainLoop(app);
  exit(0);
}
