/* $Id: test3.c,v 1.7 2001/06/09 16:34:18 amai Exp $ */

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>

#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>

#include <NumEntry.h>
#include <Sound.h>
#include <Stroke.h>

static char *FallBack[] =
{
	"*XltNumEntry*strokes: 852 Up, 258 Down, 456 Up, 654 Down",
	"*XltNumEntry*translations: #override " DEFAULT_STROKE_TRANSLATION,

	"*.strokes: 654 ParentCancel, 456 ParentActivate",
	"*XmDialogShell*translations: #override " DEFAULT_STROKE_TRANSLATION,
	NULL
};

void
debug(void)
{
}

static void
LosingFocus(void)
{
	printf("LosingFocus()\n");
}

static void
Activate(void)
{
	printf("Activate()\n");
}

static void
ValueChange(Widget w, XtPointer client_data, XltNumEntryVerifyCallbackStruct *cbs)
{
char buf[1024];

	sprintf(buf,"%09.4f", cbs->dvalue);
	XtFree(cbs->value);
	cbs->value = XtNewString(buf);
	printf("ValueChange(%s) - %s %s\n", XtName(w), cbs->value, cbs->value);
}

char *Program = "";

int
main(int argc, char **argv)
{
  Widget toplevel, one;
  XtAppContext app;

  XtSetLanguageProc(NULL, NULL, NULL);

  toplevel = XtVaAppInitialize(&app, "NumEntryTest", NULL, 0, &argc, argv, FallBack, NULL);

  one = XltCreateNumEntry(toplevel, "NumEntry1", NULL, 0);
  StrokeInitialize(toplevel);
  XltSoundInitialize(toplevel);
  XtAddCallback(one, XmNlosingFocusCallback, (XtCallbackProc)LosingFocus, (XtPointer)NULL);
  XtAddCallback(one, XmNactivateCallback, (XtCallbackProc)Activate, (XtPointer)NULL);
  XtAddCallback(one, XmNvalueChangedCallback, (XtCallbackProc)ValueChange, (XtPointer)NULL);
  XtManageChild(one);

  XtRealizeWidget(toplevel);
  XtAppMainLoop(app);

  exit(0);
}
