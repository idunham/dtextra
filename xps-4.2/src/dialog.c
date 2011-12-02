/* 
  $Id: dialog.c,v 1.1 2002/12/01 22:00:56 rockyb Exp $ 

  routine to create a dialog and ancillary routines needed by it.

  Copyright (C) 1998,1999 Rocky Bernstein
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
*/
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include "dialog.h"

/* 
 In popups and dialogs to find the parent dialog so we pop it 
 down or do some action on it. */
Widget
GetTopShell(Widget w) 
{
  while (w && !XtIsWMShell(w))
    w = XtParent(w);
  
  return w;
}

/* Should only be called as a result of NewDialog */
void
destroy_widget_callback(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
  XtDestroyWidget(GetTopShell(w));
}

/***************************************************************************
The below does the boilerplate stuff that needs to be done to 
make a new Motif Question dialog menu. 

It's the amount of garbage and the lack of prepackaged code such
as something like the below that make Microsoft look good.
***************************************************************************/
Widget
NewDialog(Widget w, char *dialog_name, const char* dialog_title,
	  Boolean have_help,   XtCallbackProc help_callback, XtPointer help,
	  Boolean have_cancel, XtCallbackProc cancel_callback, XtPointer can,
	  Boolean have_ok,     XtCallbackProc okay_callback, XtPointer ok
	  )
{
  Widget dialog = 
    XmCreateQuestionDialog(GetTopShell(w),  dialog_name, NULL, 0);
  XtVaSetValues ( dialog, 
		  XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
		  XtVaTypedArg, XmNmessageString, 
		  XmRString, dialog_title,
		  strlen(dialog_title)+1,
		  NULL );

  if (have_ok)
    XtAddCallback(dialog, XmNokCallback, okay_callback, ok);
  else 
    XtUnmanageChild( XmMessageBoxGetChild ( dialog,
					  XmDIALOG_OK_BUTTON ) );

  if (have_cancel)
    XtAddCallback(dialog, XmNcancelCallback, cancel_callback, can);
  else 
    XtUnmanageChild( XmMessageBoxGetChild ( dialog,
					  XmDIALOG_CANCEL_BUTTON ) );

  if (have_help)
    XtAddCallback(dialog, XmNhelpCallback, help_callback, help);
  else 
    XtUnmanageChild( XmMessageBoxGetChild ( dialog,
					  XmDIALOG_HELP_BUTTON ) );
  return dialog;
}

