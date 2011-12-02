/* 
 * Copyright 1993, 1994 Liverpool University Computer Science Department
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of L.U.C.S. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. L.U.C.S. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * FILE NAME:	help.c
 * CREATED:	Tue Jun  7 1994
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Quick help window for those too lazy to read the man page.
 *
 */

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>

#include "Xbr.h"
#include "common.h"

#define HELP_TEXT	"                                                          XTAR V1.1 HELP WINDOW\n\nThis is just a quick overview for those too lazy to read the man page.This tar browser can handle normal tar files which may be compressed with gzip or the standard compress utility.The contents of the archive are displayed in the main window; select a file by clicking with the left mouse button.Selected files may be viewed, extracted or extracted and renamed.These options are available from the Options pulldown menup, push buttons (if mapped) and a popup menu,displayed when you click the right mouse button.To enable the push buttons, set the resource \'showPB\' to True.The main menu allows you to open a new tar archive, unpack the current one or search for a filename.You can also initiate a search by pressing \'/\' and get the next match by pressing \'n\'.Searching is based on regular expression matching."

extern Widget TopLevel;

/*****************************************************************************/
/* General function declarations.                                            */
/*****************************************************************************/
void help_callback(Widget, XtPointer, XtPointer);
static void help_show(char *);

/*****************************************************************************/
/* Widget declarations.                                                      */
/*****************************************************************************/

/* Top level widgets */
static Widget HelpShell = NULL,	/* Shell to hold everyfink           */
              HelpForm,		/* Main widget is a form             */
              HelpClose,	/* Push Button to close window       */
              HelpText,		/* Label to display text             */
              HelpSeparator;	/* Separator to split display        */

/* help_show:*************************************************************/
/* help_show: Create widgets for ABOUT box if necessary and display it.  */
/* help_show:*************************************************************/

static void help_show(char *filename)
{
    Arg args[10];
    Cardinal n;
    XmString string;
    char *data;

    /* Only create the interface once - popup it up from then on */
    if(HelpShell != NULL) {
        XtPopup(HelpShell, XtGrabExclusive);
        return;
    }

    if((data = file_load(filename, NULL)) == NULL)
        data = "Unable to load help document";

    /* Top level shell */
    n = 0; 
    XtSetArg(args[n], XmNminWidth, 520); n++;
    XtSetArg(args[n], XmNminHeight, 400); n++;
    HelpShell = XtCreatePopupShell("HelpShell", xmDialogShellWidgetClass,
      TopLevel, args, n);
    XtAddCallback(HelpShell, XmNpopupCallback, XbrCursorAddCB,
     (XtPointer)XbrGRAB);
    XtAddCallback(HelpShell, XmNpopdownCallback, XbrCursorDeleteCB, NULL);

    /* Form widget - as usual */
    n = 0;
    XtSetArg(args[n], XmNmarginHeight, 5); n++;
    XtSetArg(args[n], XmNmarginWidth, 5); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
    HelpForm = XmCreateForm(HelpShell, "HelpForm", args, n);
    XtManageChild(HelpForm);

    /* Push button to close window */
    n = 0;
    string = XmStringCreateLocalized("OK");
    XtSetArg(args[n], XmNwidth, 150); n++;
    XtSetArg(args[n], XmNheight, 35); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 35); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    HelpClose = XmCreatePushButton(HelpForm, "HelpClose", args, n);
    XmStringFree(string);
    XtAddCallback(HelpClose, XmNactivateCallback, XbrCBPopdownShell, HelpShell);
    XtManageChild(HelpClose);

    /* Separator to make screen look nice */
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget, HelpClose); n++;
    HelpSeparator = XmCreateSeparator(HelpForm, "HelpSeparator", args, n);
    XtManageChild(HelpSeparator);

    /* Text Widget to hold details */
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget, HelpSeparator); n++;
    XtSetArg(args[n], XmNvalue, data); n++;
    XtSetArg(args[n], XmNwordWrap, False); n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    XtSetArg(args[n], XmNeditable, False); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    HelpText = XmCreateScrolledText(HelpForm, "HelpText", args, n);
    XtManageChild(HelpText);

    /* Display it */
    XtPopup(HelpShell, XtGrabExclusive);
}

/* help_callback:*********************************************************/
/* help_callback: A wrapper function to display the about box.           */
/* help_callback:*********************************************************/

void help_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
    char **doc = (char **)client_data;

    XbrCursorBusy(TopLevel);
    help_show(*doc);
    XbrCursorReset(TopLevel);
}
