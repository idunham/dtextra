/* Copyright (c) 2011, mghis.
 * All rights reserved.
 *
 * This software is provided under the BSD License,
 * ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES.
 */

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
#include <Xm/CascadeB.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/Separator.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeBG.h>
#include <Xm/LabelG.h>
#include <Xm/ComboBox.h>


#include "dtscan.h"

extern void quitCB(Widget, XtPointer, XtPointer) ;
extern void scanCB(Widget, XtPointer, XtPointer) ;
extern void lineartCB(Widget, XtPointer, XtPointer) ;
extern void grayCB(Widget, XtPointer, XtPointer) ;
extern void colorCB(Widget, XtPointer, XtPointer) ;
extern void set_toggleCB(Widget, XtPointer, XtPointer) ;
extern void nonumCB(Widget, XtPointer, XtPointer) ;
extern void oneCB(Widget, XtPointer, XtPointer) ;
extern void twoCB(Widget, XtPointer, XtPointer) ;
extern void searchCB(Widget, XtPointer, XtPointer) ;



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

Widget shell = (Widget) NULL;
Widget form = (Widget) NULL;
Widget pathW = (Widget) NULL;
Widget lineartW = (Widget) NULL;
Widget grayW = (Widget) NULL;
Widget colorW = (Widget) NULL;
Widget xW = (Widget) NULL;
Widget yW = (Widget) NULL;
Widget fileW = (Widget) NULL;
Widget nonumW = (Widget) NULL;
Widget oneW = (Widget) NULL;
Widget twoW = (Widget) NULL;
Widget scannerW = (Widget) NULL;
Widget listW = (Widget) NULL;
Widget statusW = (Widget) NULL;



void create_shell (Display *display, char *app_name, int app_argc, char **app_argv)
{
	Widget       children[16];          /* Children to manage */
	Arg          al[64];               /* Arg List */
	register int ac = 0;               /* Arg Count */
	XtPointer    tmp_value;            /* For resource conversion */
	XmString     xmstrings[16];        /* Temporary storage for XmStrings */

	Widget menuBar5 = (Widget)NULL;
	Widget cascade5 = (Widget)NULL;
	Widget menu4 = (Widget)NULL;
	Widget button7 = (Widget)NULL;
	Widget label5 = (Widget)NULL;
	Widget button8 = (Widget)NULL;
	Widget optionMenu1 = (Widget)NULL;
	Widget label7 = (Widget)NULL;
	Widget cascade8 = (Widget)NULL;
	Widget menu7 = (Widget)NULL;
	Widget label8 = (Widget)NULL;
	Widget label9 = (Widget)NULL;
	Widget label12 = (Widget)NULL;
	Widget menu1 = (Widget)NULL;
	Widget separator3 = (Widget)NULL;
	Widget separator2 = (Widget)NULL;
	Widget label1 = (Widget)NULL;
	Widget comboBox1 = (Widget)NULL;
	Widget scrolledList1 = (Widget)NULL;
	Widget scrollbar1 = (Widget)NULL;
	Widget button5 = (Widget)NULL;

	XtSetArg(al[ac], XmNallowShellResize, TRUE); ac++;
	XtSetArg(al[ac], XmNtitle, "Scanner Management"); ac++;
	XtSetArg(al[ac], XmNargc, app_argc); ac++;
	XtSetArg(al[ac], XmNargv, app_argv); ac++;
	shell = XtAppCreateShell(app_name, (char *) "XApplication", sessionShellWidgetClass, display, al, ac);

	ac = 0;
	XtSetArg(al[ac], XmNautoUnmanage, FALSE); ac++;
	form = XmCreateForm ( shell, (char *) "form", al, ac );
	ac = 0;
	menuBar5 = XmCreateMenuBar ( form, (char *) "menuBar5", al, ac );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "File", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	cascade5 = XmCreateCascadeButton ( menuBar5, (char *) "cascade5", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	menu4 = XmCreatePulldownMenu ( menuBar5, (char *) "menu4", al, ac );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Exit", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	button7 = XmCreatePushButton ( menu4, (char *) "button7", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Path:", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	label5 = XmCreateLabel ( form, (char *) "label5", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Scan", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	button8 = XmCreatePushButton ( form, (char *) "button8", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	pathW = XmCreateTextField ( form, (char *) "pathW", al, ac );
	xmstrings[0] = XmStringGenerate((XtPointer) "Mode:", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	optionMenu1 = XmCreateOptionMenu ( form, (char *) "optionMenu1", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	label7 = XmOptionLabelGadget ( optionMenu1 );
	cascade8 = XmOptionButtonGadget ( optionMenu1 );
	menu7 = XmCreatePulldownMenu ( optionMenu1, (char *) "menu7", al, ac );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Lineart", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	lineartW = XmCreatePushButton ( menu7, (char *) "lineartW", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Grayscale", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	grayW = XmCreatePushButton ( menu7, (char *) "grayW", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Color", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	colorW = XmCreatePushButton ( menu7, (char *) "colorW", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "X:", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	label8 = XmCreateLabel ( form, (char *) "label8", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	xW = XmCreateTextField ( form, (char *) "xW", al, ac );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Y:", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	label9 = XmCreateLabel ( form, (char *) "label9", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	yW = XmCreateTextField ( form, (char *) "yW", al, ac );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "File:", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	label12 = XmCreateLabel ( form, (char *) "label12", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	fileW = XmCreateTextField ( form, (char *) "fileW", al, ac );
	XtSetArg(al[ac], XmNpopupEnabled, XmPOPUP_AUTOMATIC); ac++;
	menu1 = XmCreatePopupMenu ( fileW, (char *) "menu1", al, ac );
	ac = 0;
	xmstrings[0] = XmStringGenerate ( (XtPointer) "No Numeration", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	nonumW = XmCreatePushButton ( menu1, (char *) "nonumW", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	separator3 = XmCreateSeparator ( menu1, (char *) "separator3", al, ac );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Step One", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	oneW = XmCreateToggleButton ( menu1, (char *) "oneW", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Step Two", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	twoW = XmCreateToggleButton ( menu1, (char *) "twoW", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	separator2 = XmCreateSeparator ( form, (char *) "separator2", al, ac );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Scanner:", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	label1 = XmCreateLabel ( form, (char *) "label1", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	comboBox1 = XmCreateDropDownComboBox ( form, (char *) "comboBox1", al, ac );
	scannerW = XtNameToWidget ( comboBox1, (char *) "*Text" );
	listW = XtNameToWidget ( comboBox1, (char *) "*List" );
	scrolledList1 = XtParent ( listW );

	XtSetArg(al[ac], XmNverticalScrollBar, &scrollbar1); ac++;
	XtGetValues(scrolledList1, al, ac );
	ac = 0;
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Find scanner", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	button5 = XmCreatePushButton ( form, (char *) "button5", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );
	xmstrings[0] = XmStringGenerate ( (XtPointer) "Ready", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL );
	XtSetArg(al[ac], XmNlabelString, xmstrings[0]); ac++;
	statusW = XmCreateLabel ( form, (char *) "statusW", al, ac );
	ac = 0;
	XmStringFree ( xmstrings [ 0 ] );


	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( menuBar5, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 0); ac++;
	XtSetArg(al[ac], XmNtopWidget, pathW); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 0); ac++;
	XtSetArg(al[ac], XmNbottomWidget, pathW); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 5); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_NONE); ac++;
	XtSetValues ( label5, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNtopPosition, 83); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNbottomPosition, 92); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNleftPosition, 87); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 10); ac++;
	XtSetValues ( button8, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopOffset, 36); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 45); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_NONE); ac++;
	XtSetValues ( pathW, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 5); ac++;
	XtSetArg(al[ac], XmNtopWidget, pathW); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); ac++;
	XtSetArg(al[ac], XmNbottomOffset, -104); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 5); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_OPPOSITE_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, -151); ac++;
	XtSetValues ( optionMenu1, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 0); ac++;
	XtSetArg(al[ac], XmNtopWidget, optionMenu1); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 0); ac++;
	XtSetArg(al[ac], XmNbottomWidget, optionMenu1); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNleftOffset, 5); ac++;
	XtSetArg(al[ac], XmNleftWidget, optionMenu1); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_NONE); ac++;
	XtSetValues ( label8, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 0); ac++;
	XtSetArg(al[ac], XmNtopWidget, label8); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 0); ac++;
	XtSetArg(al[ac], XmNbottomWidget, label8); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNleftOffset, 5); ac++;
	XtSetArg(al[ac], XmNleftWidget, label8); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_OPPOSITE_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, -252); ac++;
	XtSetValues ( xW, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 0); ac++;
	XtSetArg(al[ac], XmNtopWidget, xW); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 0); ac++;
	XtSetArg(al[ac], XmNbottomWidget, xW); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNleftOffset, 5); ac++;
	XtSetArg(al[ac], XmNleftWidget, xW); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_NONE); ac++;
	XtSetValues ( label9, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 0); ac++;
	XtSetArg(al[ac], XmNtopWidget, label9); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 0); ac++;
	XtSetArg(al[ac], XmNbottomWidget, label9); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNleftOffset, 5); ac++;
	XtSetArg(al[ac], XmNleftWidget, label9); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 10); ac++;
	XtSetValues ( yW, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 0); ac++;
	XtSetArg(al[ac], XmNtopWidget, pathW); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 0); ac++;
	XtSetArg(al[ac], XmNbottomWidget, pathW); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNleftOffset, 5); ac++;
	XtSetArg(al[ac], XmNleftWidget, pathW); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_NONE); ac++;
	XtSetValues ( label12, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 0); ac++;
	XtSetArg(al[ac], XmNbottomWidget, label12); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNleftOffset, 5); ac++;
	XtSetArg(al[ac], XmNleftWidget, label12); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 10); ac++;
	XtSetValues ( fileW, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 10); ac++;
	XtSetArg(al[ac], XmNtopWidget, optionMenu1); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 5); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 10); ac++;
	XtSetValues ( separator2, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 0); ac++;
	XtSetArg(al[ac], XmNtopWidget, comboBox1); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 0); ac++;
	XtSetArg(al[ac], XmNbottomWidget, comboBox1); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
	XtSetArg(al[ac], XmNleftOffset, 0); ac++;
	XtSetArg(al[ac], XmNleftWidget, separator2); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNrightOffset, 5); ac++;
	XtSetArg(al[ac], XmNrightWidget, comboBox1); ac++;
	XtSetValues ( label1, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopOffset, 5); ac++;
	XtSetArg(al[ac], XmNtopWidget, separator2); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 68); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 10); ac++;
	XtSetValues ( comboBox1, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNtopPosition, 83); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNbottomPosition, 92); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNleftPosition, 63); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNrightOffset, 10); ac++;
	XtSetArg(al[ac], XmNrightWidget, button8); ac++;
	XtSetValues ( button5, al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
	XtSetArg(al[ac], XmNtopPosition, 81); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 13); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 13); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_NONE); ac++;
	XtSetValues ( statusW, al, ac );
	ac = 0;
	if (button7)
		XtAddCallback( button7, XmNactivateCallback, quitCB, (XtPointer) 0 );
	if ((children[ac] = button7) != (Widget) 0) { ac++; }
	if (ac > 0) { XtManageChildren(children, ac); }
	ac = 0;
	XtSetArg(al[ac], XmNsubMenuId, menu4); ac++;
	XtSetValues(cascade5, al, ac );
	ac = 0;
	if ((children[ac] = cascade5) != (Widget) 0) { ac++; }
	if (ac > 0) { XtManageChildren(children, ac); }
	ac = 0;
	if (button8)
		XtAddCallback( button8, XmNactivateCallback, scanCB, (XtPointer) 0 );
	if (lineartW)
		XtAddCallback( lineartW, XmNactivateCallback, lineartCB, (XtPointer) 0 );
	if (grayW)
		XtAddCallback( grayW, XmNactivateCallback, grayCB, (XtPointer) 0 );
	if (colorW)
		XtAddCallback( colorW, XmNactivateCallback, colorCB, (XtPointer) 0 );
	if ((children[ac] = lineartW) != (Widget) 0) { ac++; }
	if ((children[ac] = grayW) != (Widget) 0) { ac++; }
	if ((children[ac] = colorW) != (Widget) 0) { ac++; }
	if (ac > 0) { XtManageChildren(children, ac); }
	ac = 0;
	XtSetArg(al[ac], XmNsubMenuId, menu7); ac++;
	XtSetValues(cascade8, al, ac );
	ac = 0;
	if (menu1)
		XtAddCallback( menu1, XmNmapCallback, set_toggleCB, (XtPointer) 0 );
	if (nonumW)
		XtAddCallback( nonumW, XmNactivateCallback, nonumCB, (XtPointer) 0 );
	if (oneW)
		XtAddCallback( oneW, XmNvalueChangedCallback, oneCB, (XtPointer) 0 );
	if (twoW)
		XtAddCallback( twoW, XmNvalueChangedCallback, twoCB, (XtPointer) 0 );
	if ((children[ac] = nonumW) != (Widget) 0) { ac++; }
	if ((children[ac] = separator3) != (Widget) 0) { ac++; }
	if ((children[ac] = oneW) != (Widget) 0) { ac++; }
	if ((children[ac] = twoW) != (Widget) 0) { ac++; }
	if (ac > 0) { XtManageChildren(children, ac); }
	ac = 0;
	if (button5)
		XtAddCallback( button5, XmNactivateCallback, searchCB, (XtPointer) 0 );
	if ((children[ac] = menuBar5) != (Widget) 0) { ac++; }
	if ((children[ac] = label5) != (Widget) 0) { ac++; }
	if ((children[ac] = button8) != (Widget) 0) { ac++; }
	if ((children[ac] = pathW) != (Widget) 0) { ac++; }
	if ((children[ac] = optionMenu1) != (Widget) 0) { ac++; }
	if ((children[ac] = label8) != (Widget) 0) { ac++; }
	if ((children[ac] = xW) != (Widget) 0) { ac++; }
	if ((children[ac] = label9) != (Widget) 0) { ac++; }
	if ((children[ac] = yW) != (Widget) 0) { ac++; }
	if ((children[ac] = label12) != (Widget) 0) { ac++; }
	if ((children[ac] = fileW) != (Widget) 0) { ac++; }
	if ((children[ac] = separator2) != (Widget) 0) { ac++; }
	if ((children[ac] = label1) != (Widget) 0) { ac++; }
	if ((children[ac] = comboBox1) != (Widget) 0) { ac++; }
	if ((children[ac] = button5) != (Widget) 0) { ac++; }
	if ((children[ac] = statusW) != (Widget) 0) { ac++; }
	if (ac > 0) { XtManageChildren(children, ac); }
	ac = 0;
	if (form != (Widget) 0) { XtManageChild ( form); }
}


/* Global Declarations */

XtAppContext app_context = (XtAppContext) 0 ;
Display     *display     = (Display *)    0 ;

int main(int argc, char **argv)
{

	/* Enable Localisation of the Application */

	XtSetLanguageProc((XtAppContext) 0, (XtLanguageProc) 0, (XtPointer) 0) ;

	/* Initialise the X Toolkit */

	XtToolkitInitialize ();

	/* Create a Global Application Context */

	app_context = XtCreateApplicationContext ();


	/* Open the Display */

	display = XtOpenDisplay(app_context, (String) 0, argv[0], "XApplication",
	                       (XrmOptionDescRec *) 0, 0,
	                       &argc, argv);
	if (display == (Display *) 0) {
		printf("%s: can't open display, exiting...\n", argv[0]);
		exit (-1);
	}

	/* Call the Create Procedures for the Dialogs in the Application */

	create_shell ( display, argv[0], argc, argv );

	/* Display the Primary Application Shell */

	XtRealizeWidget (shell);

	/* Entering X Main Loop... */

	XtAppMainLoop (app_context);

	/* NOTREACHED */

	exit (0);
}

