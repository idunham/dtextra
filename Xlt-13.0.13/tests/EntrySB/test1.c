/* $Header: /cvsroot/xlt/Xlt/tests/EntrySB/test1.c,v 1.6 2001/06/09 16:34:18 amai Exp $ */

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
/*#include <X11/Xmu/Editres.h> */
#include <Xm/RowColumn.h>
#include <EntrySB.h>

static String fallback_resources[] = {
  "*tester1.minval: 00",
  "*tester1.maxval: 100",
  "*tester1.value: 50",
  "*tester2.minval: 100",
  "*tester2.maxval: 1000",
  "*tester2.value: 250",
  "*tester2.decimals: 2",
  "*tester3.minval: 0",
  "*tester3.maxval: 1",
  "*tester3.value: 0.5",
  "*tester3.textWidth: 5",
  "*tester3.scrollWidth: 300",
  "*tester3.decimals: 5",
  "*tester1.label: Slide Me",
  "*tester2.label: And Me",
  "*tester3.label: And Me",
  "*rowcol.width: 250",
  NULL,
};

static void val_changed(Widget w, XtPointer client, XtPointer call)
{
  XtInputMask m;
  XtAppContext GXAppContext=XtWidgetToApplicationContext(w);

  sleep(1);
  while ((m = XtAppPending(GXAppContext)))
    XtAppProcessEvent(GXAppContext, m);
    XmUpdateDisplay(w);
  printf("EntrySB #%d now: %s\n",(int)client,(char *)call);
}

int main(int argc, char *argv[])
{
  Widget toplevel,tester,rowcol;
  XtAppContext app_context;
#ifdef EFENCE
  extern int EF_ON;
  
  EF_ON=TRUE;
#endif
  toplevel = XtVaAppInitialize
    (&app_context, "EntrySBT", NULL, 0, &argc, argv,
     fallback_resources, NULL);
  rowcol = XtVaCreateManagedWidget("rowcol", xmRowColumnWidgetClass,
				   toplevel, NULL); 
  tester = XtVaCreateManagedWidget("tester1", xltEntrySBWidgetClass,
  				   rowcol, 
				   NULL);
  XtAddCallback(tester, XtNvalueChanged, val_changed, (XtPointer)1);

  tester = XtVaCreateManagedWidget("tester2", xltEntrySBWidgetClass,
				   rowcol, 
				   NULL);
  XtAddCallback(tester, XtNvalueChanged, val_changed, (XtPointer)2);
  tester = XtVaCreateManagedWidget("tester3", xltEntrySBWidgetClass,
				   rowcol, 
				   NULL);
  XtAddCallback(tester, XtNvalueChanged, val_changed, (XtPointer)3);
  XtRealizeWidget(toplevel);
  XtAppMainLoop(app_context);
  
  exit(0); /* keep compiler happy */
}
