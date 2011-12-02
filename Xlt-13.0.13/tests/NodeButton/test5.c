/* $Id: test5.c,v 1.5 2001/06/09 16:34:18 amai Exp $ */

#include <stdlib.h>
#include <stdio.h>

#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>

#include <NodeButton.h>
#include <Stroke.h>


static char *FallBack[] =
{
	"*.strokes: 654 ManagerParentCancel, 456 ManagerParentActivate",
	"*NodeButton1.strokes: 852 Up, 258 Down, 456 Up, 654 Down",
	"*XmDialogShell*translations: #override" DEFAULT_STROKE_TRANSLATION,
	NULL
};

void
debug(void)
{
}

static void
Activate(void)
{
	printf("Activate()\n");
}

char *Program = "";
int
main(int argc, char **argv)
{
  Widget toplevel, rc, scrollWind, one;
  XtAppContext app;

  XtSetLanguageProc(NULL, NULL, NULL);

  toplevel = XtVaAppInitialize(&app, "NodeButtonTest", NULL, 0, &argc, argv, FallBack, NULL);
  XtVaSetValues(toplevel,
  	XmNallowShellResize, True,
  	NULL);

  scrollWind = XmCreateScrolledWindow(toplevel, "Window", NULL, 0);
  rc = XmCreateRowColumn(scrollWind, "rc", NULL, 0);
  one = XltCreateNodeButton(rc, "NodeButton1", NULL, 0);
#ifdef HAS_STROKE
  StrokeInitialize(toplevel);
#endif

  XtVaCreateManagedWidget("Two", xmPushButtonWidgetClass, one, NULL );
  XtVaCreateManagedWidget("Three", xmPushButtonWidgetClass, one, NULL );
  XtVaCreateManagedWidget("Four", xmPushButtonWidgetClass, one, NULL );

  XtAddCallback(one, XmNactivateCallback, (XtCallbackProc)Activate, (XtPointer)NULL);

  XtManageChild(one);
  XtManageChild(rc);
  XtManageChild(scrollWind);

  XtRealizeWidget(toplevel);
  XtAppMainLoop(app);

  exit(0);
}
