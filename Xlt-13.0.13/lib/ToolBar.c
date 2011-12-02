/**
 *
 * $Id: ToolBar.c,v 1.19 2001/06/02 09:27:45 amai Exp $
 *
 * Copyright (C) 1996 Free Software Foundation, Inc.
 * Copyright © 1999-2001 by the LessTif developers.
 *
 * This file is part of the GNU LessTif Extension Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **/

#include "../config.h"

#include <stdio.h>
#include <string.h>

#include <X11/Intrinsic.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/ArrowB.h>
#include <Xm/ScrolledW.h>

#ifdef HAVE_XPM
#ifdef HAVE_XM_XPMP_H
#include <Xm/XpmP.h>
#else
#include <X11/xpm.h>
#endif
#else
#error You need Xpm for this
#endif

#include <BubbleButton.h>
#include <FilePicker.h>
#include "Xlt.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif


static const char rcsid[] = "$Id: ToolBar.c,v 1.19 2001/06/02 09:27:45 amai Exp $";

/* ******************** */

static void
ConfigureCancel(Widget w)
{
    XtDestroyWidget(w);
}

/* ******************** */

static void
PrintResourcePath(String *resource, Widget w)
{
    if (XtParent(w))
	PrintResourcePath(resource, XtParent(w));
    *resource = XtRealloc(*resource, strlen(*resource) + strlen(XtName(w)) + 2);
    strcat(*resource, XtName(w));
    strcat(*resource, ".");
}

/* ******************** */

static void
ConfigureOk(Widget w, Widget ToolBar)
{
    Widget FakeToolBar;
    WidgetList children;
    Cardinal numChildren;
    String itemResource = NULL;
    Cardinal i;

    FakeToolBar = XtNameToWidget(w, "*FakeToolBar");
    XtVaGetValues(FakeToolBar,
		  XmNchildren, &children,
		  XmNnumChildren, &numChildren,
		  NULL);
    itemResource = XtNewString("");
    PrintResourcePath(&itemResource, ToolBar);
    itemResource = XtRealloc(itemResource, strlen(itemResource) + 17);
    strcat(itemResource, "activeItemList: ");
    for (i = 0; i < numChildren; i++)
    {
	Widget RealButton = XtNameToWidget(ToolBar, XtName(children[i]));

	if (XtIsManaged(children[i]))
	{
	    XtManageChild(RealButton);
	    if (i != 0)
	    {
		itemResource = XtRealloc(itemResource, strlen(itemResource) + 3);
		strcat(itemResource, ", ");
	    }
	    itemResource = XtRealloc(itemResource, strlen(itemResource) + strlen(XtName(RealButton)) + 1);
	    strcat(itemResource, XtName(RealButton));
	}
	else
	{
	    XtUnmanageChild(RealButton);
	}
	XtVaSetValues(RealButton,
		      XmNpositionIndex, i,
		      NULL);
    }
#if 0
    {
	FILE *xrdb;
	char *xrdb_command = "xrdb -merge -nocpp -display ";

	xrdb_command = XtNewString(xrdb_command);
	xrdb_command = XtRealloc(xrdb_command, strlen(xrdb_command) + strlen(DisplayString(XtDisplay(w))) + 1);
	strcat(xrdb_command, DisplayString(XtDisplay(w)));
	fflush(stdout);
	xrdb = popen(xrdb_command, "w");
	if (xrdb != NULL)
	{
	    fprintf(xrdb, "%s\n", itemResource);
	    pclose(xrdb);
	}
	else
	{
	    fprintf(stderr, "Could not execute >%s<\n", xrdb_command);
	}
	XtFree(xrdb_command);
    }
#else
printf("itemResource >%s<\n", itemResource);
#endif
    XtFree(itemResource);

    ConfigureCancel(w);
}

/* ******************** */

static void
Shift(Widget Button, int dir)
{
    short index;
    Cardinal numChildren;

    XtVaGetValues(Button,
		  XmNpositionIndex, &index,
		  NULL);
    XtVaGetValues(XtParent(Button),
		  XmNnumChildren, &numChildren,
		  NULL);
    index += dir;
    if (index < 0)
    {
	index = XmLAST_POSITION;
    }
    else if (index > (int)(numChildren - 1))
    {
	index = 0;
    }
    XtVaSetValues(Button,
		  XmNpositionIndex, index,
		  NULL);
}

/* ******************** */

static void
ShiftLeft(Widget w, Widget Button)
{
    Shift(Button, -1);
}

/* ******************** */

static void
ShiftRight(Widget w, Widget Button)
{
    Shift(Button, 1);
}

/* ******************** */

static void
ToggleItem(Widget w, Widget Button)
{
    XtVaSetValues(Button,
		  XmNpositionIndex, XmLAST_POSITION,
		  NULL);
    if (XmToggleButtonGetState(w))
    {
	XtManageChild(Button);
    }
    else
    {
	XtUnmanageChild(Button);
    }
}

/* ******************** */

static void
ConfigureAddItem(Widget RC, Widget Button)
{
    Widget Frame;
    Widget Form;
    Widget Toggle;
    Pixmap labelPixmap;
    unsigned char labelType;
    Widget lArrow;
    Widget rArrow;
    Widget Browse;
    Widget menuHelpWidget;

    XtVaGetValues(XtParent(Button),
		  XmNmenuHelpWidget, &menuHelpWidget,
		  NULL);
    Frame = XmCreateFrame(RC, "ItemFrame", NULL, 0);
    Form = XmCreateForm(Frame, "Item", NULL, 0);
    Toggle = XmCreateToggleButton(Form, XtName(Button), NULL, 0);
    XtAddCallback(Toggle, XmNvalueChangedCallback, (XtCallbackProc)ToggleItem, Button);
    XmToggleButtonSetState(Toggle, XtIsManaged(Button), False);
    XtVaGetValues(Button,
		  XmNlabelType, &labelType,
		  XmNlabelPixmap, &labelPixmap,
		  NULL);
    XtVaSetValues(Toggle,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNlabelType, labelType,
		  XmNlabelPixmap, labelPixmap,
		  NULL);
    XtManageChild(Toggle);
    lArrow = XmCreateArrowButton(Form, "lArrow", NULL, 0);
    XtVaSetValues(lArrow,
		  XmNarrowDirection, XmARROW_LEFT,
		  XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
		  XmNtopWidget, Toggle,
		  XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
		  XmNbottomWidget, Toggle,
		  XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, Toggle,
		  NULL);
    XtAddCallback(lArrow, XmNactivateCallback, (XtCallbackProc)ShiftLeft, Button);
    XtManageChild(lArrow);
    rArrow = XmCreateArrowButton(Form, "rArrow", NULL, 0);
    XtVaSetValues(rArrow,
		  XmNarrowDirection, XmARROW_RIGHT,
		  XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
		  XmNtopWidget, Toggle,
		  XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
		  XmNbottomWidget, Toggle,
		  XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, lArrow,
		  NULL);
    XtAddCallback(rArrow, XmNactivateCallback, (XtCallbackProc)ShiftRight, Button);
    XtManageChild(rArrow);
    Browse = XltCreateFilePicker(Form, XtName(Button), NULL, 0);
    XtVaSetValues(Browse,
		  XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
		  XmNtopWidget, Toggle,
		  XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
		  XmNbottomWidget, Toggle,
		  XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, rArrow,
		  XmNrightAttachment, XmATTACH_FORM,
		  NULL);
    XtManageChild(Browse);

    if (Button == menuHelpWidget)
    {
	XtSetSensitive(lArrow, False);
	XtSetSensitive(rArrow, False);
    }
    XtManageChild(Form);
    XtManageChild(Frame);
}

/* ******************** */

void
XltToolBarConfigure(Widget w, Widget ToolBar)
{
    Cardinal numChildren;
    Cardinal numManagedChildren = 0;
    WidgetList children;
    Cardinal i;
    Widget Shell = w;
    Widget Dialog;
    Widget Form;
    Widget FakeToolBar;
    Widget SW;
    Widget RC;
    Widget menuHelpWidget;

    while (!XtIsTopLevelShell(Shell))
	Shell = XtParent(Shell);
    Dialog = XmCreateMessageDialog(Shell, "ToolBarConfigure", NULL, 0);
    XtAddCallback(Dialog, XmNcancelCallback, (XtCallbackProc)ConfigureCancel, NULL);
    XtAddCallback(Dialog, XmNokCallback, (XtCallbackProc)ConfigureOk, ToolBar);
    XtVaGetValues(ToolBar,
		  XmNmenuHelpWidget, &menuHelpWidget,
		  NULL);
    XtVaSetValues(XtParent(Dialog),
		  XmNdeleteResponse, XmDESTROY,
		  NULL);
    Form = XmCreateForm(Dialog, "TBCForm", NULL, 0);
    XtVaSetValues(Form,
		  XmNresizePolicy, XmRESIZE_GROW,
		  NULL);
    FakeToolBar = XmCreateRowColumn(Form, "FakeToolBar", NULL, 0);
    XtVaSetValues(FakeToolBar,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNorientation, XmHORIZONTAL,
		  NULL);
    SW = XtVaCreateWidget("TBCSW", xmScrolledWindowWidgetClass, Form,
			  XmNscrollingPolicy, XmAUTOMATIC,
			  NULL);
    XtVaSetValues(SW,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, FakeToolBar,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);
    RC = XmCreateRowColumn(SW, "TBCSWRC", NULL, 0);
    XtVaSetValues(RC,
    	XmNpacking, XmPACK_COLUMN,
    	XmNorientation, XmVERTICAL,
    	XmNnumColumns, 1,
    	NULL);
    XtVaGetValues(ToolBar,
		  XmNchildren, &children,
		  XmNnumChildren, &numChildren,
		  NULL);
    for (i = 0; i < numChildren; i++)
    {
	Widget Button;
	Pixmap labelPixmap;
	unsigned char labelType;
	XmString labelString;

	Button = XmCreatePushButton(FakeToolBar, XtName(children[i]), NULL, 0);
	XtVaGetValues(children[i],
		      XmNlabelType, &labelType,
		      XmNlabelPixmap, &labelPixmap,
		      XmNlabelString, &labelString,
		      NULL);
	XtVaSetValues(Button,
		      XmNlabelType, labelType,
		      XmNlabelPixmap, labelPixmap,
		      NULL);
	if (children[i] == menuHelpWidget)
	{
	    XtVaSetValues(FakeToolBar,
			  XmNmenuHelpWidget, Button,
			  NULL);
	}
	if (XtIsManaged(children[i]))
	{
	    XtManageChild(Button);
	    numManagedChildren++;
	}
	else
	{
	    XtUnmanageChild(Button);
	}
	ConfigureAddItem(RC, Button);
    }
    XtManageChild(RC);
    XtManageChild(SW);
    XtManageChild(FakeToolBar);
    XtManageChild(Form);
    XtManageChild(Dialog);
    {
    Dimension width;
    Dimension rc_width;
    Dimension rc_height;
    Dimension rc_margin_height;
    int columns;

    	XtVaGetValues(FakeToolBar,
    		XmNwidth, &width,
    		NULL);
    	XtVaGetValues(RC,
    		XmNwidth, &rc_width,
    		XmNheight, &rc_height,
    		XmNmarginHeight, &rc_margin_height,
    		NULL);
    	columns = (int)(width / rc_width);
    	XtVaSetValues(RC,
    		XmNorientation, XmHORIZONTAL,
    		XmNnumColumns, (int)((double)((double)numChildren + .5 * (double)columns) / (double)columns),
    		NULL);
    	XtVaSetValues(SW,
    		XmNheight, ((rc_height * 4) / numManagedChildren) + 2 * rc_margin_height,
    		NULL);
    }
}

/* ******************** */

Widget 
XltToolBarAddItem(Widget ToolBar, char *Label, char **PixmapData)
{
    Widget Button;
    Pixmap pixmap;
    Pixmap pixmap_insens;
    XpmAttributes attrib;
    GC gc;

    Button = XltCreateBubbleButton(ToolBar, Label, NULL, 0);

    if (PixmapData != NULL)
    {
	XpmColorSymbol colors[1];

	XtVaGetValues(Button,
		      XmNbackground, &colors[0].pixel /*bg*/,
		      NULL);
	colors[0].name = NULL;
	colors[0].value = "None";
	attrib.valuemask = XpmCloseness | XpmColorSymbols;
	attrib.closeness = 40000;
	attrib.colorsymbols = colors;
	attrib.numsymbols = 1;
	XpmCreatePixmapFromData(XtDisplay(ToolBar),
				XRootWindowOfScreen(XtScreen(ToolBar)),
				PixmapData,
				&pixmap,
				NULL,
				&attrib);

	pixmap_insens = XCreatePixmap(XtDisplay(ToolBar),
				      pixmap,
				      attrib.width,
				      attrib.height,
				      DefaultDepthOfScreen(XtScreen(ToolBar)));
	gc = XDefaultGCOfScreen(XtScreen(ToolBar));
	XSetFunction(XtDisplay(ToolBar),gc,GXcopy);
	XCopyArea(XtDisplay(ToolBar),
		  pixmap,
		  pixmap_insens,
		  gc,
		  0, 0,
		  attrib.width, attrib.height,
		  0, 0);
	XSetFillStyle(XtDisplay(ToolBar), gc, FillStippled);
	XSetStipple(XtDisplay(ToolBar), gc,
		    XmGetPixmapByDepth(XtScreen(ToolBar),
				       "25_foreground",
				       1 /*WhitePixelOfScreen(XtScreen(ToolBar))*/,
				       0 /*BlackPixelOfScreen(XtScreen(ToolBar))*/,
				       1));
	XFillRectangle(XtDisplay(ToolBar),
		       pixmap_insens,
		       gc,
		       0, 0,
		       attrib.width + 1, attrib.height + 1);

	{
	XImage *image;

	    image = XGetImage(XtDisplay(Button), pixmap, 
		0, 0, attrib.width, attrib.height,
		(unsigned long)-1, ZPixmap);
	    XmInstallImage(image, Label);
	}
	XpmFreeAttributes(&attrib);

	XtVaSetValues(Button,
		      XmNlabelType, XmPIXMAP,
		      XmNlabelPixmap, pixmap,
		      XmNlabelInsensitivePixmap, pixmap_insens,
		      NULL);
    }
    return (Button);
}
