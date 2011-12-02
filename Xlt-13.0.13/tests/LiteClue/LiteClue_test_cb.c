#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>

#include <Xm/XmAll.h>

#include <LiteClue.h>
#include "LiteClue_test.h"
#include "LiteClue_testP.h"

static Widget liteClue ;

/*
********************************************************************************
** InstallLiteClues()
********************************************************************************
*/
void
InstallLiteClues(void)
{

   liteClue = XtVaCreatePopupShell( "liteClue_popup",
      xcgLiteClueWidgetClass, LiteClueTestShell,
      NULL);

   XcgLiteClueAddWidget(liteClue, ToolbarOpenBtn,
      "Open", 0, 0) ;
   
   XcgLiteClueAddWidget(liteClue, ToolbarCopyBtn,
      "Copy", 0, 0) ;
   
   XcgLiteClueAddWidget(liteClue, ToolbarExitBtn,
      "Exit", 0, 0) ;
}

/*
********************************************************************************
** QuitButton_cb()
********************************************************************************
*/
void
QuitButton_cb(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
   exit(EXIT_SUCCESS) ;
}


/*
********************************************************************************
** CopyOkBtn_cb()
********************************************************************************
*/
void
CopyOkBtn_cb(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
   XtPopdown(CopyShell) ;
}

/*
********************************************************************************
** EditCopyBtn_cb()
********************************************************************************
*/
void
EditCopyBtn_cb(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
/*
   XtVaSetValues(CopyForm,
      XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
      NULL) ;
*/
   XtManageChild(CopyForm) ;
   XtPopup(CopyShell, XtGrabNone) ;
}


