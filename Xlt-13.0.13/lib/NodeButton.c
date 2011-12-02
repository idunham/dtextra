/**
 *
 * $Id: NodeButton.c,v 1.6 2001/03/31 18:09:05 amai Exp $
 *
 * Copyright (C) 1996 Free Software Foundation, Inc.
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
#include <Xm/Form.h>
#include <Xm/ArrowB.h>
#include <Xm/RowColumn.h>
#include <Xm/RowColumnP.h>
#include <Xm/MessageB.h>
#include <Xm/LabelP.h>
#include <Xm/PushB.h>
#include <NodeButtonP.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#undef DEBUG

static const char rcsid[] = "$Id: NodeButton.c,v 1.6 2001/03/31 18:09:05 amai Exp $";

static void	insert_child(Widget w);
static void	realize(Widget w, XtValueMask *, XSetWindowAttributes *);

/*
   Widget methods, forward declarations
 */

static void post_notify(Widget aw);
static void destroy(Widget aw);
static void initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args);
static Boolean set_values(Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args);

/*
   Helper functions, forward declarations
 */

static void Help(Widget w, XEvent *event, String *params, Cardinal *num_params);

/*
   Widget default resources
 */

#define Offset(field) XtOffsetOf(XltNodeButtonRec, node_button.field)
static XtResource resources[] =
{
	{
		XmNlabelString, XmCLabelString, XmRXmString,
		sizeof(XmString), Offset(LabelString),
		XmRImmediate, (XtPointer)NULL
	},
	{
		XmNactivateCallback, XtCCallback, XtRCallback,
		sizeof(XtCallbackList), Offset(ActivateCallback),
		XtRCallback, NULL
	},
	{
		XmNhelpCallback, XtCCallback, XtRCallback,
		sizeof(XtCallbackList), Offset(HelpCallback),
		XtRCallback, NULL
	}
};

/*
static void NBGetValue(Widget wid, int resource_offset, XtArgVal *value);
static void NBGetColumns(Widget wid, int resource_offset, XtArgVal *value);
*/

#define syn_resources		NULL
#define NUM_SYN_RESOURCES	0
#undef Offset

static XtActionsRec actions[] =
{
    {"Help", Help}
};

/*
   Widget class record
 */
/* *INDENT-OFF* */
XltNodeButtonClassRec xrwsNodeButtonClassRec = {
	/* Core Class Part */
	{
	/* superclass            */ (WidgetClass) & xmFormClassRec,
	/* class_name            */ "XltNodeButton",
	/* widget_size           */ sizeof(XltNodeButtonRec),
	/* class_initialize      */ NULL /*class_initialize*/,
	/* class_part_initialize */ NULL /*class_part_initialize*/,
	/* class_inited          */ False,
	/* initialize            */ initialize,
	/* initialize_hook       */ NULL,
	/* realize               */ /*XtInheritRealize*/ realize,
	/* actions               */ actions,
	/* num_actions           */ XtNumber(actions),
	/* resources             */ resources,
	/* num_resources         */ XtNumber(resources),
	/* xrm_class             */ NULLQUARK,
	/* compress_motion       */ True,
	/* compress_exposure     */ XtExposeCompressMultiple,
	/* compress_enterleave   */ True,
	/* visible_interest      */ True,
	/* destroy               */ destroy,
	/* resize                */ XtInheritResize,
	/* expose                */ XtInheritExpose,
	/* set_values            */ set_values,
	/* set_values_hook       */ NULL,
	/* set_values_almost     */ XtInheritSetValuesAlmost,
	/* get_values_hook       */ NULL /*get_values_hook*/,
	/* accept_focus          */ NULL,
	/* version               */ XtVersion,
	/* callback offsets      */ NULL,
	/* tm_table              */ XtInheritTranslations,
	/* query_geometry        */ XtInheritQueryGeometry,
	/* display_accelerator   */ XtInheritDisplayAccelerator,
	/* extension             */ NULL
	},
	/* Composite Class Part */
	{
	/* geometry manager */	XtInheritGeometryManager,
	/* change_managed   */	XtInheritChangeManaged,
	/* insert_child     */	insert_child, /*XtInheritInsertChild,*/
	/* delete_child     */	XtInheritDeleteChild,
	/* extension        */	NULL,
	},
	/* Constraint Class Part */
	{
	/* subresources      */ NULL,
	/* subresource_count */ 0,
	/* constraint_size   */ sizeof(XmFormConstraintRec),
	/* initialize        */ NULL /*constraint_initialize*/,
	/* destroy           */ destroy /*NULL*/,
	/* set_values        */ NULL /*constraint_set_values*/,
	/* extension         */ NULL,
	},
	/* Manager Class Part */
	{
	/* translations           */ XmInheritTranslations,
	/* syn_resources          */ NULL,
	/* num_syn_resources      */ 0,
	/* syn_constraint_res     */ NULL,
	/* num_syn_constraint_res */ 0,
	/* parent_process         */ XmInheritParentProcess,
	/* extension              */ NULL
	},
    /* XmBulletinBoard part */
    {
	/* always_install_accelerators  */ False,
	/* geo_matrix_create            */ NULL,
	/* focus_moved_proc             */ XmInheritFocusMovedProc,
	/* extension */ NULL
    },
    /* XmForm part */
    {
	/* extension */ NULL
    },
	/* NodeButton Class Part */
	{
		NULL
	}
};
/* *INDENT-ON* */




WidgetClass xrwsNodeButtonWidgetClass = (WidgetClass)&xrwsNodeButtonClassRec;

/*
   Helper routines
 */

/* ******************** */

static void
realize(Widget w, XtValueMask *value_mask, XSetWindowAttributes *attributes)
{
	Widget	parent;

	(* ((CompositeClassRec*)(XtClass(w)->core_class.superclass))->core_class.realize)(w,value_mask,attributes);

	parent = XtParent(w);

#ifdef DEBUG
	printf( "realize(%s)\n", XtName(w));
#endif 

	if (XmIsRowColumn(parent))
	{
		Widget sw = XtParent(parent);
#ifdef DEBUG
		printf( "RowColumn parent %s\n", XtName(parent));
#endif
		if (XmIsScrolledWindow(XtParent(sw)))
		{
			/* if my parent is a ScrolledWindow,
			   recompute the width needed to display
			   without horizontal scroll bar. */

			Dimension	width, sbwidth;
			Widget		vsb;

#ifdef DEBUG
			printf("Resizing %s to sizeof %s\n", XtName(XtParent(sw)),
				XtName(parent));
#endif

			XtVaGetValues( XtParent(sw), XmNverticalScrollBar, &vsb, NULL );
			if (vsb)
			{
				XtVaGetValues( parent, XmNwidth, &width, NULL );
				XtVaGetValues( vsb, XmNwidth, &sbwidth, NULL );
				XtVaSetValues( XtParent(sw), XmNwidth, width + sbwidth * 2,
					NULL );
			}
		}
#ifdef DEBUG
		else if (XltIsNodeButton(XtParent(sw)))
		{
			printf( "NodeButton parent %s\n", XtName(XtParent(sw)));
		}
		else
		{
			printf( "Parent %s\n", XtName(XtParent(sw)));
		}
#endif
	}
}

/* ******************** */

static void
insert_child(Widget w)
{
	Widget	parent;
	parent = XtParent(w);

	/*
	** Are we initializing the NodeButton. If so the inherited
	** insert_child function does just fine.
	*/
	if (NodeButton_InitMe(parent))
	{
#define superclass (&compositeClassRec)
		(*superclass->composite_class.insert_child)(w);
#undef  superclass
/*
		(* ((CompositeClassRec*)(XtClass(w)->core_class.superclass))->composite_class.insert_child)(w);
		*/
	}
	/*
	** Other children need to be reparented to the RowColumn.
	** This is a bit iffy, but seems to work.
	*/
	else
	{
		Widget	rowColumn = NodeButton_RowColumn(XtParent(w));
		XSync(XtDisplay(XtParent(w)), False);
		XGrabServer(XtDisplay(w));
		XtParent(w) = rowColumn;
		( (XmRowColumnWidgetClass)(XtClass(rowColumn)))->composite_class.insert_child(w);
		XUngrabServer(XtDisplay(w));

		XtSetSensitive(NodeButton_Arrow(parent), True);
	}
}

/* ******************** */

/*
**	post_notify is called when a NodeButton (un)manages the
**	RowColumn. We traverse up the tree while finding other
**	NodeButtons. It the ultimate parent is a scrolled window
**	then we play with its size so that no horizontal scroll
**	bar is needed.
*/

static void
post_notify(Widget aw)
{
	Widget	cw, lastRc;

#ifdef DEBUG
	printf("NodeButton - post_notify(%s) - %s\n", XtName(aw), XtName(XtParent(aw)));
#endif

	cw = aw;
	lastRc = NULL;
	while ((cw != NULL) && XltIsNodeButton(cw))
	{
		Widget	parent;

		parent = XtParent(cw);
		if (XmIsRowColumn(parent))
		{
			/* If my parent is a NodeButton notify
		   	it of the potential change in the
		   	size of the tree */
#ifdef DEBUG
			printf("NodeButton - post_notify parent(%s) - %s\n",
				XtName(parent), XtName(XtParent(parent)));
#endif
			cw = XtParent(parent);
			lastRc = parent;
		}
	}

	if (cw == NULL)
		return;

	if (XmIsScrolledWindow(XtParent(cw)) && lastRc != NULL)
	{
		/* if my parent is a ScrolledWindow,
		   recompute the width needed to display
		   without horizontal scroll bar. */

	   	Dimension	width, sbwidth;
	   	Widget		vsb;

#ifdef DEBUG
		printf("Resizing %s to sizeof %s\n", XtName(XtParent(cw)),
			XtName(lastRc));
#endif
	   	XtVaGetValues( XtParent(cw), XmNverticalScrollBar, &vsb, NULL );
	   	if (vsb)
	   	{
	   		XtVaGetValues( lastRc, XmNwidth, &width, NULL );
	   		XtVaGetValues( vsb, XmNwidth, &sbwidth, NULL );
	   		XtVaSetValues( XtParent(cw), XmNwidth, width + sbwidth * 2,
	   			NULL );
	   	}
	}
}

/* ******************** */

static void
ArrowActivate(Widget W, XtPointer client_data, XtPointer call_data)
{
	unsigned char	direction;
	Widget	nbw = XtParent(W);

#ifdef DEBUG
	printf("NodeButton - ArrowActivate(%s) - %s\n", XtName(W), XtName(XtParent(W)));
#endif

	XtVaGetValues( W, XmNarrowDirection, &direction, NULL);
	switch(direction)
	{
	case	XmARROW_UP:
		XtVaSetValues( W, XmNarrowDirection, XmARROW_DOWN, NULL);
		XtUnmanageChild(NodeButton_RowColumn(nbw));
		break;
	case	XmARROW_DOWN:
		XtVaSetValues( W, XmNarrowDirection, XmARROW_UP, NULL);
		XtManageChild(NodeButton_RowColumn(nbw));
		break;
	}

	post_notify(nbw);
}

/* ******************** */

static void
Activate(Widget W, XtPointer client_data, XtPointer call_data)
{
#ifdef DEBUG
	printf("NodeButton - Activate(%s) - %s\n", XtName(W), XtName(XtParent(W)));
#endif
	XtCallCallbacks(XtParent(W), XmNactivateCallback, call_data);
}

/* ******************** */

/*
   Widget methods
 */

static void
destroy(Widget aw)
{
#ifdef DEBUG
	printf("NodeButton - destroy(%s)\n", XtName(aw));
#endif
	XmStringFree(NodeButton_LabelString(aw));
}

static void
initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
Dimension h;

	NodeButton_InitMe(new_w) = True;
#ifdef DEBUG
	printf("NodeButton - initialize(%s) - %i args\n", XtName(new_w), *num_args);
#endif
	if (NodeButton_LabelString(new_w) == NULL ||
		NodeButton_LabelString(new_w) == (XmString)XmUNSPECIFIED)
	{
		NodeButton_LabelString(new_w) = XmStringCreateSimple(XtName(new_w));
	}
	else
	{
		NodeButton_LabelString(new_w) = XmStringCopy(NodeButton_LabelString(new_w));
	}

	NodeButton_Arrow(new_w) = XtVaCreateManagedWidget( "arrow", xmArrowButtonWidgetClass, new_w,
		XmNarrowDirection,	XmARROW_DOWN,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNsensitive,		False,
		NULL);

	NodeButton_Button(new_w) = XtVaCreateManagedWidget( "button", xmPushButtonWidgetClass, new_w,
		XmNalignment,		XmALIGNMENT_BEGINNING,
		XmNlabelString,		NodeButton_LabelString(new_w),
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		NodeButton_Arrow(new_w),
		XmNtopAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);

	NodeButton_RowColumn(new_w) = XtVaCreateWidget( "rowcol", xmRowColumnWidgetClass, new_w,
		XmNorientation,		XmVERTICAL,
		XmNmarginWidth,		0,
		XmNmarginHeight,	0,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		NodeButton_Button(new_w),
		XmNrightAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget,		NodeButton_Button(new_w),
		NULL);

	XtVaGetValues(NodeButton_Button(new_w), XmNheight, &h, NULL);
	XtVaSetValues(NodeButton_Arrow(new_w), XmNheight, h, NULL);
	XtAddCallback(NodeButton_Arrow(new_w), XmNactivateCallback, ArrowActivate, NULL );
	XtAddCallback(NodeButton_Button(new_w), XmNactivateCallback, Activate, NULL );
	XtAddCallback(new_w, XmNhelpCallback, (XtCallbackProc)Help, NULL);
	NodeButton_InitMe(new_w) = False;
}

static Boolean
set_values(Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
	Boolean refresh = False;

	if (NodeButton_LabelString(old) != NodeButton_LabelString(new_w))
	{
		Dimension	h;

		NodeButton_LabelString(new_w) = XmStringCopy(NodeButton_LabelString(new_w));
		XtVaSetValues(NodeButton_Button(new_w),
			XmNlabelString, NodeButton_LabelString(new_w),
			NULL);
		XtVaGetValues(NodeButton_Button(new_w), XmNheight, &h, NULL);
		XtVaSetValues(NodeButton_Arrow(new_w), XmNheight, h, NULL);
		if (NodeButton_LabelString(old))
		{
			XmStringFree(NodeButton_LabelString(old));
		}
	}
	return (refresh);
}

/*
   Synthetic resource hooks
 */

static void 
Help(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
static Widget Dialog = NULL;
char HelpText[] =
	"Pressing the Arrow button will alternately show and\n"
	"hide a submenu. Pressing the button activates the\n"
	"function attached to the button.\n"
	"The Arrow button is not active if there are no items\n"
	"int the submenu."
   ;

    if (Dialog == NULL)
    {
    Widget ParentShell;
    XmString string;

    	ParentShell = w;
    	while (!XtIsApplicationShell(ParentShell)) ParentShell = XtParent(ParentShell);
    	Dialog = XmCreateInformationDialog(ParentShell, "HelpText", NULL, 0);
    	string = XmStringCreateLtoR(HelpText,XmFONTLIST_DEFAULT_TAG);
    	XtVaSetValues(Dialog,
    		XmNmessageString, string,
    		NULL);
    }
    XtManageChild(Dialog);

}

/*
   Public functions
 */

Widget
XltCreateNodeButton(Widget parent,
		   char *name,
		   Arg *arglist,
		   Cardinal argCount)
{
	return XtCreateWidget(name, xrwsNodeButtonWidgetClass, parent, arglist, argCount);
}

Widget
XltNodeButtonGetChild(Widget W, unsigned char child)
{
	if (XltIsNodeButton(W))
	{
	    switch (child)
	    {
	    case XltNODEBUTTON_BUTTON:
		    return NodeButton_Button(W);
		    break;
	    case XltNODEBUTTON_ARROW:
		    return NodeButton_Arrow(W);
		    break;
	    case XltNODEBUTTON_ROWCOLUMN:
		    return NodeButton_RowColumn(W);
		    break;
	    default:
		    return NULL;
		    break;
	    }
	}
	else
	{
		    return NULL;
	}
}
