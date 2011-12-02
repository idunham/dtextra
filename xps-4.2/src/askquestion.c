/********************************************************************
 * Modifications from example code from the book:
 *
 *           The X Window System: Programming and Applications with Xt
 *           Second OSF/Motif Edition
 *         by
 *           Douglas Young
 *           Prentice Hall, 1994
 *
 *         Copyright 1994 by Prentice Hall
 *         All Rights Reserved
 *
 *  Permission to use, copy, modify, and distribute this software for 
 *  any purpose except publication and without fee is hereby granted, provided 
 *  that the above copyright notice appear in all copies of the software.
 * *****************************************************************************/

/****************************************************************
 * AskQuestion: A function that posts a question dialog and
 *              does not return until the user has responded.
 ****************************************************************/
#include "user_config.h"
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#ifdef HAVE_X11_XMU_EDITRES_H
#include <X11/Xmu/Editres.h>
#endif

#include "askquestion.h"
#include "os.h"
#include "xpstypes.h"

static void YesCallback ( Widget    w,
                          XtPointer clientData, 
                         XtPointer callData );

Boolean 
AskQuestion ( Widget parent, char *str )
{
    static int result;
    Widget dialog;

    dialog = XmCreateQuestionDialog ( parent, "question", NULL, 0 );

    XtVaSetValues ( dialog, 
                    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
                    XtVaTypedArg, XmNmessageString, 
                    XmRString, str, strlen ( str ) + 1,
                    NULL );


#ifdef DO_XEDITRESCHECKMESSAGES
    XtAddEventHandler(dialog, (EventMask) 0, True, 
		      _XEditResCheckMessages, NULL);
#endif

   /*
    * Register a callback to be called if the user answers OK.
    * Pass a pointer to the variable "result".
    */

    XtAddCallback ( dialog, XmNokCallback,  
                    YesCallback, ( XtPointer ) &result );

    XtUnmanageChild( XmMessageBoxGetChild ( dialog,
					  XmDIALOG_HELP_BUTTON ) );
   /*
    * Initialize the result to FALSE. If the user dismisses the dialog
    * in any way other than activating the OK button, the result will
    * be FALSE.
    */

    result = FALSE;

   /*
    * Manage the dialog to display it.
    */

    XtManageChild ( dialog );

    XtPopup(XtParent(dialog), XtGrabNone);
    
    while ( FALSE == result ) {
      XtAppProcessEvent ( base.app_con, XtIMAll );
    }

    XtPopdown(XtParent(dialog));
    /* 
     * Return the value of result, which may have been set by the 
     * YesCallback function.
     */

     return ( result );
}
                                                                
static void YesCallback ( Widget    w,
                          XtPointer clientData,
                          XtPointer callData )
{
    int *result = ( int * ) clientData;

    *result = TRUE;
}
        

void 
ShowDialog ( Widget parent, const int dialog_type, char *str )
{
    Widget dialog;

    dialog = XmCreateMessageDialog( parent, "dialog", NULL, 0 );

    XtVaSetValues ( dialog, 
		    XmNdialogType, dialog_type,
                    XmNdialogStyle,  XmDIALOG_FULL_APPLICATION_MODAL,
                    XtVaTypedArg, XmNmessageString, 
                    XmRString, str, strlen ( str ) + 1,
                    NULL );

#ifdef DO_XEDITRESCHECKMESSAGES
    XtAddEventHandler(dialog, (EventMask) 0, True, 
		      _XEditResCheckMessages, NULL);
#endif

    XtUnmanageChild( XmMessageBoxGetChild ( dialog,
					  XmDIALOG_HELP_BUTTON ) );

    XtUnmanageChild( XmMessageBoxGetChild ( dialog,
					    XmDIALOG_CANCEL_BUTTON ) );
   /*
    * Manage the dialog to display it.
    */

    XtManageChild ( dialog );

   /*
    * Enter an event loop that lasts as long as the dialog is managed.
    * The event loop will be exited when the user dismisses the dialog
    * in any way.
    */

    while ( XtIsManaged ( dialog ) ) 
    {
        XEvent event;

        XtAppNextEvent ( XtWidgetToApplicationContext ( dialog ),
                         &event );

        XtDispatchEvent ( &event );
   }

    /* 
     * Destroy the dialog because it is no longer needed.
     */

     XtDestroyWidget ( dialog );
}
                                                                
/* 
   Same as ShowDialog but uses allows for scroll bars and allows for 
   copying text. 
 */
void 
ShowScrolledDialog ( Widget parent, const int dialog_type, char *str )
{
    Widget dialog, scrolled_text;

    dialog = XmCreateMessageDialog( parent, "dialog", NULL, 0 );

    XtVaSetValues ( dialog, 
		    XmNdialogType, dialog_type,
                    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
                    NULL );

  /* Widget for ps info. XmCreateText doesn't do variable args.
     So set up a local temp argument array. Uck! Motif sucks. */
  {
    Arg args[20];
    int n = 0;
    int rows = 1;
    int maxcols = 0;
    char *last_nl = str;
    char *p = str;
    while (*p) {
      if ('\n' == *p) { 
	int cols = p - last_nl;
	last_nl = p;
	rows++;
	if (cols > maxcols) maxcols = cols;
      }
      p++;
    }
    XtSetArg(args[n], XmNeditable, False); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    XtSetArg(args[n], XmNscrollHorizontal, True); n++;
    XtSetArg(args[n], XmNscrollVertical, True); n++;
    XtSetArg(args[n], XmNtraversalOn, True); n++;
    XtSetArg(args[n], XmNvisualPolicy,       XmVARIABLE); n++;
    XtSetArg(args[n], XmNscrollingPolicy,    XmAUTOMATIC); n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED); n++;
    XtSetArg(args[n], XmNvalue, str); n++;
    /* XtSetArg(args[n], XmNrows, rows); n++;
       XtSetArg(args[n], XmNcolumns, maxcols); n++; */
    printf("columns: %d\n", maxcols);
    
    scrolled_text = XmCreateScrolledText(dialog, "ScrolledMessage", args, n);
  }

#ifdef DO_XEDITRESCHECKMESSAGES
    XtAddEventHandler(dialog, (EventMask) 0, True, 
		      _XEditResCheckMessages, NULL);
#endif

    XtUnmanageChild( XmMessageBoxGetChild ( dialog,
					  XmDIALOG_HELP_BUTTON ) );

    XtUnmanageChild( XmMessageBoxGetChild ( dialog,
					    XmDIALOG_CANCEL_BUTTON ) );
   /*
    * Manage the dialog to display it.
    */

    XtManageChild ( scrolled_text );
    XtManageChild ( dialog );

   /*
    * Enter an event loop that lasts as long as the dialog is managed.
    * The event loop will be exited when the user dismisses the dialog
    * in any way.
    */

    while ( XtIsManaged ( dialog ) ) 
    {
        XEvent event;

        XtAppNextEvent ( XtWidgetToApplicationContext ( dialog ),
                         &event );

        XtDispatchEvent ( &event );
   }

    /* 
     * Destroy the dialog because it is no longer needed.
     */

     XtDestroyWidget ( dialog );
}
                                                                
#ifdef STANDALONE
void AskQuestionCallback ( Widget    w, 
                          XtPointer clientData, 
                          XtPointer callData )
{

   if(AskQuestion(w, "Do you want to quit?"))
       exit(0);
}

void main ( int argc, char **argv )
{
    Widget       shell, button;
    XtAppContext app;

   /*
    * Initialize Xt
    */

    shell = XtAppInitialize ( &app, "Askquestion", NULL, 0, 
                              &argc, argv, NULL, NULL, 0 );


   /* Create a button and add a callback to launch a dialog */

    button = XtCreateManagedWidget ( "Quit", 
                                     xmPushButtonWidgetClass,
                                     shell, NULL, 0 );

    XtAddCallback ( button, XmNactivateCallback, 
                    AskQuestionCallback, NULL );

   /*
    * Realize the shell and enter the event loop
    */

    XtRealizeWidget ( shell );
    XtAppMainLoop ( app );
}
#endif /*STANDALONE*/
