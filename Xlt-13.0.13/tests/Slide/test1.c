/* $Id: test1.c,v 1.3 2001/06/09 16:34:18 amai Exp $ */

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>

#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/Display.h>
#include <Xm/MessageB.h>
#include <Xm/Form.h>

#include <NumEntry.h>
#include <SlideC.h>
#include <Sound.h>
#include <Xlt.h>
#include <Stroke.h>


static char *FallBack[] =
{
    "*slideInterval: 5",
    "*MB.messageAlignment: XmALIGNMENT_CENTER",
    "*MB.messageString: <Btn3Down> for a \"growup\" menu",
    "*MB.translations: #augment !<Btn3Down>: popupMenu()",
    "*Info.messageString: press Cancel to dissolve the dialog",
    "*XltNumEntry*strokes: 852 Up, 258 Down, 456 Up, 654 Down",
    "*XltNumEntry*translations: #override " DEFAULT_STROKE_TRANSLATION,

    "*.strokes: 654 ParentCancel, 456 ParentActivate",
    "*XmDialogShell*translations: #override " DEFAULT_STROKE_TRANSLATION,
    NULL
};

void
debug(void)
{
}

static void
Activate(Widget w)
{
    Widget slider;

    printf("Activate()\n");
    slider = XtVaCreateWidget("Slide", xltSlideContextWidgetClass,
			      XmGetXmDisplay(XtDisplay(w)),
			      XltNslideWidget, XtParent(w),
			      XltNslideDestWidth, 1,
			      XltNslideDestHeight, 1,
			      NULL);
/*      XtAddCallback(slider, XltNslideFinishCallback, (XtCallbackProc)finish, NULL); */
}

static void
ValueChange(Widget w, XtPointer client_data, XltNumEntryVerifyCallbackStruct * cbs)
{
    char buf[1024];

    sprintf(buf, "%09.4f", cbs->dvalue);
    XtFree(cbs->value);
    cbs->value = XtNewString(buf);
    printf("ValueChange(%s) - %s %s\n", XtName(w), cbs->value, cbs->value);
}

char *Program = "";

static void
slideUnmanageChild(Widget slider, Widget w)
{
    XtUnmanageChild(w);
}

static void
dissolve(Widget w)
{
Widget slider;
Window root, child;
int rx, ry;
int wx, wy;
unsigned int mask;

    XQueryPointer(XtDisplay(w), XtWindow(w),
    	&root, &child,
    	&rx, &ry,
    	&wx, &wy,
    	&mask);
    slider = XtVaCreateWidget("Slide", xltSlideContextWidgetClass,
			      XmGetXmDisplay(XtDisplay(w)),
			      XltNslideWidget, XtParent(w),
			      XltNslideDestX, rx,
			      XltNslideDestY, ry,
			      XltNslideDestWidth, 1,
			      XltNslideDestHeight, 1,
			      NULL);
    XtAddCallback(slider, XltNslideFinishCallback, (XtCallbackProc)slideUnmanageChild, w);
}

static void
createDialog(Widget w)
{
static Widget Dialog = NULL;
XtWidgetGeometry geo;

    if (Dialog == NULL)
    {
    	Dialog = XmCreateInformationDialog(w, "Info", NULL, 0);
    	XtVaSetValues(Dialog,
	    XmNautoUnmanage, False,
	    NULL);
	XtAddCallback(Dialog, XmNcancelCallback, (XtCallbackProc)dissolve, NULL);
    }
    XtQueryGeometry(Dialog, NULL, &geo);
    XtVaSetValues(Dialog,
    	XmNwidth, geo.width,
    	XmNheight, geo.height,
    	NULL);
    XtManageChild(Dialog);
}

static void
popupMenu(Widget w, XEvent *event)
{
static Widget Menu = NULL;
Widget slider;
XtWidgetGeometry geo;

    if (Menu == NULL)
    {
    Widget button;

    	Menu = XmCreatePopupMenu(w, "GraphPopup", NULL, 0);
    	button = XmCreatePushButton(Menu, "Reset", NULL, 0);
    	XtManageChild(button);

    	button = XmCreatePushButton(Menu, "Unknown", NULL, 0);
    	XtSetSensitive(button, False);
    	XtManageChild(button);

    	button = XmCreatePushButton(Menu, "Unknown", NULL, 0);
    	XtSetSensitive(button, False);
    	XtManageChild(button);

    	button = XmCreatePushButton(Menu, "Unknown", NULL, 0);
    	XtSetSensitive(button, False);
    	XtManageChild(button);

    	button = XmCreatePushButton(Menu, "Unknown", NULL, 0);
    	XtSetSensitive(button, False);
    	XtManageChild(button);
    }
    XmMenuPosition(Menu, (XButtonPressedEvent *)event);
    XtQueryGeometry(Menu, NULL, &geo);
    XtManageChild(Menu);
    XtVaSetValues(XtParent(Menu),
    	XmNwidth, 1,
    	XmNheight, 1,
    	NULL);
    slider = XtVaCreateWidget("Slide", xltSlideContextWidgetClass,
			      XmGetXmDisplay(XtDisplay(Menu)),
			      XltNslideWidget, XtParent(Menu),
			      XltNslideDestWidth, geo.width,
			      XltNslideDestHeight, geo.height,
			      NULL);
}

static XtActionsRec actions[] = {
    {"popupMenu", (XtActionProc)popupMenu},
};

int
main(int argc, char **argv)
{
    Widget toplevel, x, y, width, height, mb, form;
    XtAppContext app;

    XtSetLanguageProc(NULL, NULL, NULL);

    toplevel = XtVaAppInitialize(&app, "SlideTest", NULL, 0, &argc, argv, FallBack, NULL);
    StrokeInitialize(toplevel);
    XltSoundInitialize(toplevel);
    XtAppAddActions(XtWidgetToApplicationContext(toplevel),
    	actions, XtNumber(actions));

    mb = XmCreateMessageBox(toplevel, "MB", NULL, 0);
    XtAddCallback(mb, XmNokCallback, (XtCallbackProc)createDialog, toplevel);
    XtAddCallback(mb, XmNcancelCallback, (XtCallbackProc)exit, NULL);
    form = XmCreateForm(mb, "Form", NULL, 0);
    x = XltCreateNumEntry(form, "destX", NULL, 0);
    XtVaSetValues(x,
    	XmNtopAttachment, XmATTACH_FORM,
    	XmNleftAttachment, XmATTACH_FORM,
    	XmNrightAttachment, XmATTACH_FORM,
    	NULL);
    XtManageChild(x);
    y = XltCreateNumEntry(form, "destY", NULL, 0);
    XtVaSetValues(y,
    	XmNtopAttachment, XmATTACH_WIDGET,
    	XmNtopWidget, x,
    	XmNleftAttachment, XmATTACH_FORM,
    	XmNrightAttachment, XmATTACH_FORM,
    	NULL);
    XtManageChild(y);
    width = XltCreateNumEntry(form, "destWidth", NULL, 0);
    XtVaSetValues(width,
    	XmNtopAttachment, XmATTACH_WIDGET,
    	XmNtopWidget, y,
    	XmNleftAttachment, XmATTACH_FORM,
    	XmNrightAttachment, XmATTACH_FORM,
    	NULL);
    XtManageChild(width);
    height = XltCreateNumEntry(form, "destHeight", NULL, 0);
    XtVaSetValues(height,
    	XmNtopAttachment, XmATTACH_WIDGET,
    	XmNtopWidget, width,
    	XmNleftAttachment, XmATTACH_FORM,
    	XmNrightAttachment, XmATTACH_FORM,
    	NULL);
    XtManageChild(height);
    XtManageChild(form);
    XtManageChild(mb);

    XtRealizeWidget(toplevel);
    XltWaitTillMapped(toplevel);
    XtAppAddTimeOut(XtWidgetToApplicationContext(toplevel),
    	0,
    	(XtTimerCallbackProc)createDialog,
    	toplevel);
    XtAppMainLoop(app);

    exit(0);
}
