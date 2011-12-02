/*
**LIBS: -lXm -lXt -lX11
*/

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <Xm/RepType.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/SeparatoG.h>


#include "main.h"

extern void clearInW_CB(Widget, XtPointer, XtPointer) ;
extern void clearOutW_CB(Widget, XtPointer, XtPointer) ;
extern void runCB(Widget, XtPointer, XtPointer) ;
extern void quitCB(Widget, XtPointer, XtPointer) ;



void XDmanage_link( Widget w, XtPointer client_data, XtPointer call_data)
{
	if (client_data && *(Widget *) client_data)
		XtManageChild(*(Widget *) client_data);
}

void XDunmanage_link( Widget w, XtPointer client_data, XtPointer call_data)
{
	if (client_data && *(Widget *) client_data)
		XtUnmanageChild(*(Widget *) client_data);
}

void XDpopup_link( Widget w, XtPointer client_data, XtPointer call_data)
{
	if (client_data && *(Widget *) client_data)
		XtPopup(*(Widget *) client_data, XtGrabNone);
}

void XDpopdown_link( Widget w, XtPointer client_data, XtPointer call_data)
{
	if (client_data && *(Widget *) client_data)
		XtPopdown(*(Widget *) client_data);
}

void XDmap_link( Widget w, XtPointer client_data, XtPointer call_data)
{
	if (client_data && *(Widget *) client_data)
		XtMapWidget(*(Widget *) client_data);
}

void XDunmap_link( Widget w, XtPointer client_data, XtPointer call_data)
{
	if (client_data && *(Widget *) client_data)
		XtUnmapWidget(*(Widget *) client_data);
}

void XDenable_link( Widget w, XtPointer client_data, XtPointer call_data)
{
	if (client_data && *(Widget *) client_data)
		XtSetSensitive(*(Widget *) client_data, TRUE);
}

void XDdisable_link( Widget w, XtPointer client_data, XtPointer call_data)
{
	if (client_data && *(Widget *) client_data)
		XtSetSensitive(*(Widget *) client_data, FALSE);
}

Widget toplevel = (Widget) NULL;
Widget inW = (Widget) NULL;
Widget outW = (Widget) NULL;



void create_toplevel (Display *display, char *app_name, int app_argc, char **app_argv)
{
	Widget       children[4];          /* Children to manage */
	Arg          al[64];               /* Arg List */
	register int ac = 0;               /* Arg Count */
	XtPointer    tmp_value;            /* For resource conversion */
	XmString     xmstrings[16];        /* Temporary storage for XmStrings */

	Widget dialogTemplate1 = (Widget)NULL;
	Widget form2 = (Widget)NULL;
	Widget label3 = (Widget)NULL;
	Widget menu1 = (Widget)NULL;
	Widget button9 = (Widget)NULL;
	Widget separator6 = (Widget)NULL;
	Widget scrolledText1 = (Widget)NULL;
	Widget scrollbar3 = (Widget)NULL;
	Widget scrollbar4 = (Widget)NULL;
	Widget menu2 = (Widget)NULL;
	Widget button10 = (Widget)NULL;
	Widget button7 = (Widget)NULL;
	Widget button8 = (Widget)NULL;

	XtSetArg(al[ac], XmNallowShellResize, TRUE); ac++;
	XtSetArg(al[ac], XmNargc, app_argc); ac++;
	XtSetArg(al[ac], XmNargv, app_argv); ac++;
	toplevel = XtAppCreateShell(app_name, (char *) "XApplication", applicationShellWidgetClass, display, al, ac);

	ac = 0;
	XtSetArg(al[ac], XmNautoUnmanage, FALSE); ac++;
	XtSetArg(al[ac], XmNdialogType, XmDIALOG_TEMPLATE); ac++;
	dialogTemplate1 = XmCreateMessageBox ( toplevel, (char *) "dialogTemplate1", al, ac );
	ac = 0;
	form2 = XmCreateForm ( dialogTemplate1, (char *) "form2", al, ac );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Command", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	label3 = XmCreateLabel ( form2, (char *) "label3", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	inW = XmCreateTextField ( form2, (char *) "inW", al, ac );
	XtSetArg(al[ac], XmNpopupEnabled, XmPOPUP_AUTOMATIC); ac++;
	menu1 = XmCreatePopupMenu ( inW, (char *) "menu1", al, ac );
	ac = 0;
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Clear", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	button9 = XmCreatePushButton ( menu1, (char *) "button9", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	separator6 = XmCreateSeparator ( form2, (char *) "separator6", al, ac );
	XtSetArg(al[ac], XmNeditMode, XmMULTI_LINE_EDIT); ac++;
	outW = XmCreateScrolledText ( form2, (char *) "outW", al, ac );
	ac = 0;
	scrolledText1 = XtParent ( outW );

	XtSetArg(al[ac], XmNhorizontalScrollBar, &scrollbar3); ac++;
	XtSetArg(al[ac], XmNverticalScrollBar, &scrollbar4); ac++;
	XtGetValues(scrolledText1, al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNpopupEnabled, XmPOPUP_AUTOMATIC); ac++;
	menu2 = XmCreatePopupMenu ( outW, (char *) "menu2", al, ac );
	ac = 0;
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Clear", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	button10 = XmCreatePushButton ( menu2, (char *) "button10", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Run", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	button7 = XmCreatePushButton ( dialogTemplate1, (char *) "button7", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Exit", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	button8 = XmCreatePushButton ( dialogTemplate1, (char *) "button8", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );


	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopOffset, 15); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 10); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNrightOffset, 3); ac++;
	XtSetArg(al[ac], XmNrightWidget, inW); ac++;
	XtSetValues ( label3, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopOffset, 10); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 97); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 10); ac++;
	XtSetValues ( inW, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 10); ac++;
	XtSetArg(al[ac], XmNtopWidget, inW); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 10); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 10); ac++;
	XtSetValues ( separator6, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 10); ac++;
	XtSetArg(al[ac], XmNtopWidget, separator6); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 10); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 10); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 10); ac++;
	XtSetValues ( scrolledText1, al, ac );
	ac = 0;
	if (button9)
		XtAddCallback( button9, XmNactivateCallback, clearInW_CB, (XtPointer) 0 );
	if ((children[ac] = button9) != (Widget) 0) { ac++; }
	if (ac > 0) { XtManageChildren(children, ac); }
	ac = 0;
	if (button10)
		XtAddCallback( button10, XmNactivateCallback, clearOutW_CB, (XtPointer) 0 );
	if ((children[ac] = button10) != (Widget) 0) { ac++; }
	if (ac > 0) { XtManageChildren(children, ac); }
	ac = 0;
	if (outW != (Widget) 0) { XtManageChild(outW); }
	if ((children[ac] = label3) != (Widget) 0) { ac++; }
	if ((children[ac] = inW) != (Widget) 0) { ac++; }
	if ((children[ac] = separator6) != (Widget) 0) { ac++; }
	if (ac > 0) { XtManageChildren(children, ac); }
	ac = 0;
	if (button7)
		XtAddCallback( button7, XmNactivateCallback, runCB, (XtPointer) 0 );
	if (button8)
		XtAddCallback( button8, XmNactivateCallback, quitCB, (XtPointer) 0 );
	if ((children[ac] = form2) != (Widget) 0) { ac++; }
	if ((children[ac] = button7) != (Widget) 0) { ac++; }
	if ((children[ac] = button8) != (Widget) 0) { ac++; }
	if (ac > 0) { XtManageChildren(children, ac); }
	ac = 0;
	if (dialogTemplate1 != (Widget) 0) { XtManageChild ( dialogTemplate1); }
}


/* Global Declarations */

XtAppContext app_context = (XtAppContext) 0 ;
Display     *display     = (Display *)    0 ;

int main(int argc, char **argv)
{


	XtSetLanguageProc((XtAppContext) 0, (XtLanguageProc) 0, (XtPointer) 0) ;

	XtToolkitInitialize ();

	app_context = XtCreateApplicationContext ();


	display = XtOpenDisplay(app_context, (String) 0, argv[0], "XApplication",
	                       (XrmOptionDescRec *) 0, 0,
	                       &argc, argv);
	if (display == (Display *) 0) {
		printf("%s: can't open display, exiting...\n", argv[0]);
		exit (-1);
	}

	create_toplevel ( display, argv[0], argc, argv );

	XtRealizeWidget (toplevel);

	XtAppMainLoop (app_context);

	exit (0);
}

