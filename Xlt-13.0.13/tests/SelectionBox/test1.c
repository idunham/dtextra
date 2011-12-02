/* $Id: test1.c,v 1.3 2001/06/09 16:34:18 amai Exp $ */

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>

#include <Xm/SelectioB.h>
#include <Xm/List.h>

#include <SelectionBox.h>
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


char *Program = "";
int
main(int argc, char **argv)
{
  Widget toplevel, one, list;
  XtAppContext app;
  int i;
  String items[] = {
	"one",
	"two",
	"three",
	"four",
	"five",
	"six",
	"TD CHEQUING",
	"TD SAVINGS",
	"TD VISA",
	"seven",
	"eight",
	"nine",
	"One",
	"Two",
	"Three",
	"Four",
	"Five",
	"Six",
	"Seven",
	"Eight",
	"Nine",
};

  XtSetLanguageProc(NULL, NULL, NULL);

  toplevel = XtVaAppInitialize(&app, "XltSelectionBox", NULL, 0, &argc, argv, FallBack, NULL);
  StrokeInitialize(toplevel);
  XltSoundInitialize(toplevel);

  one = XltCreateSelectionBox(toplevel, "XltSelectionBox", NULL, 0);
  list = XmSelectionBoxGetChild(one, XmDIALOG_LIST);
  for (i = 0; i < XtNumber(items); i++)
  {
  	XmListAddItem(list, XmStringCreateSimple(items[i]), 0);
  }
  XtManageChild(one);

  XtRealizeWidget(toplevel);
  XtAppMainLoop(app);

  exit(0);
}
