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
 * FILE NAME:	XbrDialog.c
 * CREATED:	Thu Oct 28 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Deals with popping up Xm dialog boxes.
 *
 */

#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>

#include <stdio.h>
#include <stdarg.h>

#include "XbrCB.h"
#include "XbrCursor.h"
#include "XbrFSB.h"
#include "XbrRes.h"

#if (XmVERSION < 1) || ((XmVERSION == 1) && (XmREVISION < 2))
#define XmStringCreateLocalized(string) XmStringCreateLtoR(string,XmSTRING_DEFAULT_CHARSET)
#endif

#define	XbrDIALOG_NOACTION		-1

void XbrDialogError(Widget, ...);
int XbrDialogQuestion(Widget, char *, char *, ...);
char *XbrDialogFSBox(Widget, char *, char *);
char *XbrDialogPrompt(Widget, ...);
static void XbrDialogCBYes(Widget, XtPointer, XtPointer);
static void XbrDialogCBNo(Widget, XtPointer, XtPointer);

/* XbrDialogError:************************************************************/
/* XbrDialogError: Display an error dialog containing the given error msg    */
/* XbrDialogError: in printf format.                                         */
/* XbrDialogError:************************************************************/

void XbrDialogError(Widget parent, ...)
{
    char *fmt, buffer[BUFSIZ];
    Display *display = XtDisplay(parent);
    va_list argp;
    Arg args[10];
    int n;
    Widget w;

    /* Get format string */
    va_start(argp, parent);
    fmt = va_arg( argp, char *);
    vsprintf(buffer, fmt, argp);
    va_end(argp);

    /* Create widgets */
    n = 0;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    w = XmCreateErrorDialog(parent, "message", args, n);
    XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_HELP_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(w, XmDIALOG_CANCEL_BUTTON));

    /* Register a callback which destroys the widget & another to reset the
       cursor.
    */
    XtAddCallback(w, XmNokCallback, XbrCBDestroyWidget, w);
    XtAddCallback(w, XmNdestroyCallback, XbrCursorDeleteCB, NULL);

    /* Set error message */
    XtVaSetValues(w, XtVaTypedArg, XmNmessageString, XmRString, buffer,
      sizeof(char *), NULL);

    /* Sound bell */
    XBell(display, 50);

    /* Display the Error Box */
    XtManageChild(w);
    XbrCursorAdd(w, XbrGRAB);
}

/* XbrDialogQuestion:*********************************************************/
/* XbrDialogQuestion: Pop up a question dialog and take over the event loop. */
/* XbrDialogQuestion: When we have an answer, exit the event loop and return */
/* XbrDialogQuestion: the answer.                                            */
/* XbrDialogQuestion:*********************************************************/

int XbrDialogQuestion(Widget parent, char *yes, char *no, ...)
{
    static int action = XbrDIALOG_NOACTION;
    char *fmt, buffer[BUFSIZ];
    va_list argp;
    Arg args[10];
    int n;
    XEvent event;
    Widget w;
    XmString yes_string, no_string, msg_string;

    /* Get format string */
    va_start(argp, no);
    fmt = va_arg( argp, char *);
    vsprintf(buffer, fmt, argp);
    va_end(argp);

    /* Create widgets */
    n = 0;
    yes_string = XmStringCreateLocalized(yes);
    no_string = XmStringCreateLocalized(no);
    msg_string = XmStringCreateLocalized(buffer);
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNcancelLabelString, no_string); n++;
    XtSetArg(args[n], XmNokLabelString, yes_string); n++;
    XtSetArg(args[n], XmNmessageString, msg_string); n++;
    w = XmCreateQuestionDialog(parent, "question", args, n);
    XmStringFree(yes_string); XmStringFree(no_string); XmStringFree(msg_string);
    XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_HELP_BUTTON));

    /* Register callbacks which reset the cursor and destroy the widgets */
    XtAddCallback(w, XmNokCallback, XbrDialogCBYes, (XtPointer) &action);
    XtAddCallback(w, XmNokCallback, XbrCBDestroyWidget, w);
    XtAddCallback(w, XmNcancelCallback, XbrDialogCBNo, (XtPointer) &action);
    XtAddCallback(w, XmNcancelCallback, XbrCBDestroyWidget, w);
    XtAddCallback(w, XmNdestroyCallback, XbrCursorDeleteCB, NULL);

    /* Display the Question Box */
    XtManageChild(w);
    XbrCursorAdd(w, XbrGRAB);

    /* Take over the event loop */
    action = XbrDIALOG_NOACTION;
    while(action == XbrDIALOG_NOACTION) {
        XtAppNextEvent(XtWidgetToApplicationContext(parent), &event);
        XtDispatchEvent(&event);
    }

    return(action);
}

/* XbrDialogCBYes:************************************************************/
/* XbrDialogCBYes: Set the client_data to yes.                               */
/* XbrDialogCBYes:************************************************************/

static void XbrDialogCBYes(Widget w, XtPointer client_data, XtPointer call_data)
{
    int *ptr = (int *) client_data;

    *ptr = 1;
}

/* XbrDialogCBNo:*************************************************************/
/* XbrDialogCBNo: Set the client_data to no.                               */
/* XbrDialogCBNo:************************************************************/

static void XbrDialogCBNo(Widget w, XtPointer client_data, XtPointer call_data)
{
    int *ptr = (int *) client_data;

    *ptr = 0;
}

/*----------------------------------------------------------------------------
  XbrDialogFSBox()
  Popup a file selection box and get the filename.
  
  Widget w		Parent shell of file selection box
  char *selection	Selection string
  char *filename	Suggested filename 
  ----------------------------------------------------------------------------*/
char *XbrDialogFSBox(Widget w, char *selection, char *filename)
{
    static Widget FSBShell = NULL, FSB;
    static int action = XbrDIALOG_NOACTION;
    XEvent event;
    XmString string;
    Arg args[10];
    Cardinal n;
    char *text, buffer[256];

    /* Build main windows */
    if(FSBShell == NULL) {
        FSBShell = XtCreatePopupShell("File Selection",topLevelShellWidgetClass,
          w, NULL, 0);
        FSB = XtVaCreateManagedWidget("XbrlibFSB",xmFileSelectionBoxWidgetClass,
          FSBShell, NULL);

        /* Unmanage HELP button */
        XtUnmanageChild(XmFileSelectionBoxGetChild(FSB, XmDIALOG_HELP_BUTTON));
   
        /* Cursor callbacks */
        XtAddCallback(FSBShell, XmNpopupCallback, XbrCursorAddCB,
          (XtPointer)XbrGRAB); 
        XtAddCallback(FSBShell, XmNpopdownCallback, XbrCursorDeleteCB, NULL);
        XtAddCallback(FSBShell, XmNpopdownCallback, XbrCBRefresh, NULL); 

        /* OK Callbacks */
        XtAddCallback(FSB, XmNokCallback, XbrDialogCBYes, &action);
        XtAddCallback(FSB, XmNokCallback, XbrCBPopdownShell, FSBShell);

        /* CANCEL Callbacks */
        XtAddCallback(FSB, XmNcancelCallback, XbrDialogCBNo, &action);
        XtAddCallback(FSB, XmNcancelCallback, XbrCBPopdownShell, FSBShell);
    }

    /* Set the selection. */
    if(selection == NULL)
	string = XmStringCreateLocalized("Selection");
    else
	string = XmStringCreateLocalized(selection);
    
    n = 0;
    XtSetArg(args[n], XmNselectionLabelString, string); n++;
    XtSetValues(FSB, args, n);
    XmStringFree(string);
    
    /* Re-search the filenames. */
    n = 0;
    XtSetArg(args[n], XmNdirMask, &string); n++;
    XtGetValues(FSB, args, n);
    XmFileSelectionDoSearch(FSB, string);
    XmStringFree(string);

    /* Set the suggested filename. */
    text = XbrFSBSelection(FSB);
    sprintf(buffer, "%s/%s", fname_dirname(text), filename?filename:"");
    string = XmStringCreateLocalized(buffer);
    n = 0;
    XtSetArg(args[n], XmNdirSpec, string); n++;
    XtSetValues(FSB, args, n);
    XmStringFree(string);
    
    /* Display the File Selection Box */
    XtPopup(FSBShell, XtGrabExclusive);

    /* Take over the event loop */
    action = XbrDIALOG_NOACTION;
    while(action == XbrDIALOG_NOACTION) {
        XtAppNextEvent(XtWidgetToApplicationContext(w), &event);
        XtDispatchEvent(&event);
    }

    /* What happened */
    if(action)
        return(XtNewString(XbrFSBSelection(FSB)));
    else
        return(NULL);
}

/*----------------------------------------------------------------------------
  XbrDialogPrompt()
  Prompt the user for a command or string.
  
  Widget w	The parent widget of the dialog
  ...		Variable argument of printf style statements
  ----------------------------------------------------------------------------*/
char *XbrDialogPrompt(Widget w, ...)
{
    Widget Prompt = NULL;
    static int action = XbrDIALOG_NOACTION;
    XEvent event;
    XmString string;
    Arg args[10];
    Cardinal n;
    char *fmt, buffer[BUFSIZ];
    va_list argp;
   
    /* Get format string */
    va_start(argp, w);
    fmt = va_arg( argp, char *);
    vsprintf(buffer, fmt, argp);
    va_end(argp);
   
    /* Build main windows */
    n = 0;
    string = XmStringCreateLocalized(buffer);
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNselectionLabelString, string); n++;
    Prompt = XmCreatePromptDialog(w, "XbrPrompt", args, n);
    XmStringFree(string);
    
    /* Get rid of the HELP button */
    XtUnmanageChild(XmSelectionBoxGetChild(Prompt, XmDIALOG_HELP_BUTTON));
    
    /* Cursor callbacks */
    XtAddCallback(XtParent(Prompt), XmNpopupCallback, XbrCursorAddCB,(XtPointer)XbrGRAB); 
    XtAddCallback(XtParent(Prompt), XmNdestroyCallback, XbrCursorDeleteCB, NULL); 

    /* OK Callbacks */
    XtAddCallback(Prompt, XmNokCallback, XbrDialogCBYes, &action);
    XtAddCallback(Prompt, XmNokCallback, XbrCBDestroyWidget, Prompt);

    /* CANCEL Callbacks */
    XtAddCallback(Prompt, XmNcancelCallback, XbrDialogCBNo, &action);
    XtAddCallback(Prompt, XmNcancelCallback, XbrCBDestroyWidget, Prompt);
    
    /* Display the Prompt */
    XtManageChild(Prompt);

    /* Take over the event loop */
    action = XbrDIALOG_NOACTION;
    while(action == XbrDIALOG_NOACTION) {
        XtAppNextEvent(XtWidgetToApplicationContext(w), &event);
        XtDispatchEvent(&event);
    }

    /* What happened */
    if(action)
        return(XbrResGetString(Prompt, XmNtextString));
    else
        return(NULL);
}
