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
 * FILE NAME:	XbrCB.c
 * CREATED:	Wed Oct 27 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Routines which are regular needed as callback routines.
 *
 */

#include <Xm/Xm.h>

void XbrCBExitApplication(Widget, XtPointer, XtPointer);
void XbrCBDestroyWidget(Widget, XtPointer, XtPointer);
void XbrCBPopdownShell(Widget, XtPointer, XtPointer);
void XbrCBUnmanageWidget(Widget, XtPointer, XtPointer);
void XbrCBRefresh(Widget, XtPointer, XtPointer);
void XbrCBPassword(Widget, XtPointer, XtPointer);

/* XbrCBExitApplication:******************************************************/
/* XbrCBExitApplication: Given the top level widget this routine closes the  */
/* XbrCBExitApplication: display and performs an exit().                     */
/* XbrCBExitApplication:******************************************************/

void XbrCBExitApplication(Widget w, XtPointer client_data, XtPointer call_data)
{
    XtCloseDisplay(XtDisplay(w));
    exit(0);
}

/* XbrCBDestroyWidget:********************************************************/
/* XbrCBDestroyWidget: Widget to destroy is passed in as client data.        */
/* XbrCBDestroyWidget: If the program has previously used XbrAddShel then it */
/* XbrCBDestroyWidget: is assumed they want to remove this widget from the   */
/* XbrCBDestroyWidget: cursor list.                                          */
/* XbrCBDestroyWidget:********************************************************/

void XbrCBDestroyWidget(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget dialog = (Widget) client_data;
    XtDestroyWidget(dialog);
}

/* XbrCBPassword:************************************************************/
/* XbrCBPassword: Callback to add to your text widget so that stars         */
/* XbrCBPassword: appear instead of what is typed. This one's Terry's fault.*/
/* XbrCBPassword:************************************************************/

void XbrCBPassword(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmTextVerifyCallbackStruct *cb=(XmTextVerifyCallbackStruct *) call_data;
    char        **password = (char **) client_data;
    int         flen,blen,nlen,plen,i;
    char        *front,*back,*new,*old;

    old = *password?*password:"";
    plen = strlen(old) ;
    flen = cb->startPos;
    blen = plen - cb->endPos;
    nlen = flen + blen + cb->text->length;

    front=XtMalloc(flen+1);
    back=XtMalloc(blen+1);
    new=XtMalloc(nlen+1);

    strncpy(front,old,flen);
    front[flen]=0;
    strncpy(back,&old[cb->endPos],blen);
    back[blen]=0;

    sprintf(new,"%s%s%s",front,cb->text->ptr,back);
    XtFree(*password);
    *password=new;
    XtFree(front);
    XtFree(back);

    for( i=0 ;i < cb->text->length;i++)
      cb->text->ptr[i]='*';
}

/* XbrCBPopdownShell:*********************************************************/
/* XbrCBPopdownShell: Widget passed in is assumed to be some sort of shell   */
/* XbrCBPopdownShell: which has been previously displayed with               */
/* XbrCBPopdownShell: XtPopupShell().                                        */
/* XbrCBPopdownShell:*********************************************************/

void XbrCBPopdownShell(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget topshell = (Widget) client_data;
    XtPopdown(topshell);
}

/* XbrCBUnmanageWidget:*******************************************************/
/* XbrCBUnmanageWidget: Useful for closing windows which you may want to     */
/* XbrCBUnmanageWidget: to reopen at some time as this routine does not      */
/* XbrCBUnmanageWidget: destroy the widget.                                  */
/* XbrCBUnmanageWidget:*******************************************************/

void XbrCBUnmanageWidget(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget widget = (Widget) client_data;
    XtUnmanageChild(widget);
}

/*----------------------------------------------------------------------------
  XbrCBRefresh()
  Refresh the display - useful when closing dialog boxes.
  ----------------------------------------------------------------------------*/
void XbrCBRefresh(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmUpdateDisplay(w);
}
