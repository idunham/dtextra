/* 
 * Copyright 1993, 1994, 1995  Liverpool University Computer Science Department
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
 * FILE NAME:	about.c
 * CREATED:	Thu Nov 18 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Simply displays an about box with the University logo and my
 *              name in it.
 *
 */

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>

#include "Xbr.h"
#include "common.h"

#include "crest.h"

#define ABOUT_TEXT	"XTAR V1.4\n\n(c) University of Liverpool 1994, 1995\n\nWritten by Rik Turnbull\n(rik@csc.liv.ac.uk)"

extern Widget TopLevel;

/*****************************************************************************/
/* General function declarations.                                            */
/*****************************************************************************/
void about_callback(Widget, XtPointer, XtPointer);
static void about_show();
void about_loadPixmap(Widget);

/*****************************************************************************/
/* Widget declarations.                                                      */
/*****************************************************************************/

/* Top level widgets */
static Widget AboutShell = NULL,	/* Shell to hold everyfink           */
              AboutForm,		/* Main widget is a form             */
              AboutClose,		/* Push Button to close window       */
              AboutLabel,		/* Label to display text             */
              AboutPixmap,		/* Label to display bitmap           */
              AboutSeparator;		/* Separator to split display        */

/* about_show:*************************************************************/
/* about_show: Create widgets for ABOUT box if necessary and display it.  */
/* about_show:*************************************************************/

static void about_show()
{
    Arg args[10];
    Cardinal n;

    /* Only create the interface once - popup it up from then on */
    if(AboutShell != NULL) {
        XtPopup(AboutShell, XtGrabExclusive);
        return;
    }

    /* Top level shell */
    n = 0; 
    XtSetArg(args[n], XmNminWidth, 420); n++;
    XtSetArg(args[n], XmNminHeight, 220); n++;
    AboutShell = XtCreatePopupShell("AboutShell", xmDialogShellWidgetClass,
      TopLevel, args, n);
    XtAddCallback(AboutShell, XmNpopupCallback, XbrCursorAddCB,
     (XtPointer)XbrGRAB);
    XtAddCallback(AboutShell, XmNpopdownCallback, XbrCursorDeleteCB, NULL);

    /* Form widget - as usual */
    n = 0;
    XtSetArg(args[n], XmNmarginHeight, 5); n++;
    XtSetArg(args[n], XmNmarginWidth, 5); n++;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
    AboutForm = XmCreateForm(AboutShell, "AboutForm", args, n);
    XtManageChild(AboutForm);

    /* Push button to close window */
    AboutClose = XbrCreatePB(AboutForm, "OK", XmALIGNMENT_CENTER,
      XmATTACH_POSITION, 35, XmATTACH_POSITION, 65, XmATTACH_POSITION,  80,
      XmATTACH_FORM, 0);
    XtAddCallback(AboutClose, XmNactivateCallback, XbrCBPopdownShell,
      AboutShell);

    /* Label to display University logo */
    AboutPixmap = XbrCreateLabel(AboutForm, "PIXMAP", XmALIGNMENT_CENTER,
      XmATTACH_FORM, 0, XmATTACH_POSITION, 50, XmATTACH_FORM, 0,
      XmATTACH_POSITION, 75);
    about_loadPixmap(AboutPixmap);

    /* Label to put my name in */
    AboutLabel = XbrCreateLabel(AboutForm, ABOUT_TEXT, XmALIGNMENT_CENTER,
      XmATTACH_POSITION, 51, XmATTACH_FORM, 0, XmATTACH_FORM, 0,
      XmATTACH_POSITION, 75);

    /* Separator to make screen look nice */
    AboutSeparator = XbrCreateSep(AboutForm, XmSHADOW_ETCHED_IN, XmATTACH_FORM,
      0, XmATTACH_FORM, 0, XmATTACH_POSITION, 76, XmATTACH_POSITION, 77);

    /* Display it */
    XtPopup(AboutShell, XtGrabExclusive);
}

/* about_loadPixmap:*******************************************************/
/* about_loadPixmap: Takes the included logo and creates a pixmap and     */
/* about_loadPixmap: then loads it into the label.                        */
/* about_loadPixmap:*******************************************************/

void about_loadPixmap(Widget w)
{
    Display *display = XtDisplay(w);
    Screen *screen = XtScreen(w);
    Colormap colourmap = DefaultColormap(display, DefaultScreen(display));
    XBR_IMAGE *xbrimage;
    XColor colour;
    int size = sizeof(crest);
    Pixel background;
 
    XtVaGetValues(w, XmNbackground, &background, NULL);

    colour.pixel = background;
    XQueryColor(display, colourmap, &colour);

    xbrimage = XbrGfxGIF(display, crest, size, 0, &colour, NULL);
    XtVaSetValues(w,XmNlabelType,XmPIXMAP,XmNlabelPixmap,xbrimage->pixmap,NULL);
}

/* about_callback:*********************************************************/
/* about_callback: A wrapper function to display the about box.           */
/* about_callback:*********************************************************/

void about_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
    XbrCursorBusy(TopLevel);
    about_show();
    XbrCursorReset(TopLevel);
}
