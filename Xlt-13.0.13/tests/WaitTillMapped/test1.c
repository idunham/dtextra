/* $Header: /cvsroot/xlt/Xlt/tests/WaitTillMapped/test1.c,v 1.4 2001/06/09 16:34:18 amai Exp $ */

#include "../../config.h"

#include <stdio.h>
#include <stdlib.h>

#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/MessageB.h>

#include <Xlt.h>
#include <Host.h>


static char *FallBack[] =
{
	NULL
};

void Activate(Widget  w, XtPointer client, XtPointer call)
{
static Widget Dialog = NULL;

  if (Dialog == NULL)
  {
  	Dialog = XmCreateMessageDialog(w, "Message", NULL, 0);
  }
  XtManageChild(Dialog);
  XltWaitTillMapped(Dialog);
  XtUnmanageChild(Dialog);
}


int
main(int argc, char **argv)
{
  Widget toplevel, one;
  XtAppContext app;

  XtSetLanguageProc(NULL, NULL, NULL);

  toplevel = XtVaAppInitialize(&app, "NodeButtonTest", NULL, 0, &argc,
			       argv, FallBack, NULL); 

  one = XmCreatePushButton(toplevel, "Push Me", NULL, 0);
  XtAddCallback(one, XmNactivateCallback, (XtCallbackProc)Activate, (XtPointer)NULL);
  XtManageChild(one);

  XtRealizeWidget(toplevel);
  XtAppMainLoop(app);
  exit(0);
}
