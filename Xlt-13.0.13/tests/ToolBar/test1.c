/* $Header: /cvsroot/xlt/Xlt/tests/ToolBar/test1.c,v 1.6 2002/04/12 17:19:55 rwscott Exp $ */

#include "../../config.h"

#include <stdio.h>
#include <stdlib.h>

#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>

#include <Xlt.h>
#include <AnimatedButton.h>

#include "dates.xpm"
#include "floppy3.xpm"
#include "new_acc.xpm"
#include "new_trans.xpm"
#include "open.xpm"
#include "plot_acc.xpm"
#include "print.xpm"
#include "recur_trans.xpm"
#include "stopsign.xpm"
#include "view_acc.xpm"
#include "view_trans.xpm"

void
debug(void)
{
}

int
main(int argc, char **argv)
{
  Widget toplevel, ToolBar;
  XtAppContext app;

  XtSetLanguageProc(NULL, NULL, NULL);

  toplevel = XtVaAppInitialize(&app, "TooBarTest", NULL, 0, &argc, argv, NULL, NULL);

  ToolBar = XmCreateRowColumn(toplevel, "ToolBar", NULL, 0);
  XtVaSetValues(ToolBar,
  	XmNorientation, XmHORIZONTAL,
  	NULL);

        {
        Widget Button;

        Button = XltToolBarAddItem(ToolBar,"Configure",  floppy3);
        XtAddCallback(Button, XmNactivateCallback, (XtCallbackProc)XltToolBarConfigure, ToolBar);
        XtManageChild(Button);

        Button = XltToolBarAddItem(ToolBar,"Open",  open);
        XtManageChild(Button);

        Button = XltToolBarAddItem(ToolBar,"Print",  print);
        XtManageChild(Button);

        Button = XltToolBarAddItem(ToolBar,"PlotAcc",  plot_acc);
        XtManageChild(Button);

        Button = XltToolBarAddItem(ToolBar,"ViewAcc",  view_acc);
        XtManageChild(Button);

        Button = XltToolBarAddItem(ToolBar,"ViewTrans",  view_trans);
        XtManageChild(Button);

        Button = XltToolBarAddItem(ToolBar,"NewAcc",  new_acc);
        XtManageChild(Button);

        Button = XltToolBarAddItem(ToolBar,"NewTrans",  new_trans);
        XtManageChild(Button);

        Button = XltToolBarAddItem(ToolBar,"RecurTrans",  recur_trans);
        XtManageChild(Button);

        Button = XltToolBarAddItem(ToolBar,"Dates",  dates);
        XtManageChild(Button);

        Button = XltToolBarAddItem(ToolBar,"Exit", stopsign);
        XtAddCallback(Button, XmNactivateCallback, (XtCallbackProc)exit, NULL);
        XtManageChild(Button);
        XtVaSetValues(ToolBar,
        	XmNmenuHelpWidget,Button,
        	NULL);
        }
  XtManageChild(ToolBar);

  XtRealizeWidget(toplevel);
  XtAppMainLoop(app);

  exit(0);
}
