/* $Id: test2.c,v 1.4 2001/06/09 16:34:18 amai Exp $ */

#include <stdlib.h>
#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/PushB.h>

#include <FontChooser.h>

void Pop (Widget w, XtPointer client_data, XtPointer call_data);

Widget toplevel;

int
main (int argc, char **argv)
{
  XtAppContext theApp;
  Widget butt1;

  toplevel = XtVaAppInitialize (&theApp, "drawingArea", NULL, 0,
				&argc, argv, NULL, NULL);

  butt1 = XtVaCreateManagedWidget ("Button1", xmPushButtonWidgetClass, toplevel,
				   NULL, 0);

  XtAddCallback (butt1, XmNactivateCallback, Pop, NULL);

  XtRealizeWidget (toplevel);
  XtAppMainLoop(theApp);

  exit (0);
}

void
Quit(Widget w, XtPointer client, XtPointer call)
{
	char	*s;

	XtVaGetValues(w, XltNfontName, &s, NULL);
	fprintf(stderr, "Font is '%s'\n", s);
	exit(0);
}

void
Pop(Widget w, XtPointer client, XtPointer call)
{
    Widget dialog;
    Arg al[4];
    int ac;
    
    ac = 0;
    dialog = XltCreateFontChooserDialog(toplevel, "dialog2", al, ac);

    XtManageChild (dialog);
    XtAddCallback(dialog, XmNokCallback, Quit, NULL);
}
