/* $Id: test2.c,v 1.2 1999/10/26 09:16:25 amai Exp $ */

#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <BubbleButton.h>
#include <stdlib.h>
#include <stdio.h>

void
debug()
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
  XtManageChild(one);
  one = XltCreateBubbleButton(rc, "BubbleButton2", NULL, 0);
  XtAddCallback(one, XmNactivateCallback, (XtCallbackProc)Callback, NULL);
  XtManageChild(one);

  XtManageChild(rc);

  XtRealizeWidget(toplevel);
  XtAppMainLoop(app);

  exit(0);
}
