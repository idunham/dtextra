/*$Id: dialog.h,v 1.1 2002/12/01 22:00:56 rockyb Exp $*/

/* 
 In popups and dialogs to find the parent dialog so we pop it 
 down or do some action on it. */
Widget GetTopShell(Widget w);

/* Should only be called as a result of NewDialog */
void destroy_widget_callback(Widget w, XtPointer client_data, 
			     XtPointer call_data);

/***************************************************************************
The below does the boilerplate stuff that needs to be done to 
make a new Motif Question dialog menu. 

It's the amount of garbage and the lack of prepackaged code such
as something like the below that make Microsoft look good.
***************************************************************************/
Widget NewDialog(Widget w, char *dialog_name, const char* dialog_title,
 Boolean have_help,   XtCallbackProc help_callback, XtPointer help,
 Boolean have_cancel, XtCallbackProc cancel_callback, XtPointer can,
 Boolean have_ok,     XtCallbackProc okay_callback, XtPointer ok
		 );


