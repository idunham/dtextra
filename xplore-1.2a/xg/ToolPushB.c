/*@
 * Copyright(c) 1995-1997 Gregory M. Messner
 * All rights reserved
 *
 * Permission to use, copy, modify and distribute this material for
 * non-commercial personal and educational use without fee is hereby
 * granted, provided that the above copyright notice and this permission 
 * notice appear in all copies, and that the name of Gregory M. Messner
 * not be used in advertising or publicity pertaining to this material
 * without the specific, prior written permission of Gregory M. Messner 
 * or an authorized representative.
 *
 * GREGORY M. MESSNER MAKES NO REPRESENTATIONS AND EXTENDS NO WARRANTIES, 
 * EXPRESS OR IMPLIED, WITH RESPECT TO THE SOFTWARE, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * ANY PARTICULAR PURPOSE, AND THE WARRANTY AGAINST INFRINGEMENT OF PATENTS
 * OR OTHER INTELLECTUAL PROPERTY RIGHTS. THE SOFTWARE IS PROVIDED "AS IS",
 * AND IN NO EVENT SHALL GREGORY M. MESSNER BE LIABLE FOR ANY DAMAGES,
 * INCLUDING ANY LOST PROFITS OR OTHER INCIDENTAL OR CONSEQUENTIAL DAMAGES
 * RELATING TO THE SOFTWARE.
 *
 */


/* +++FHDR+++
 *
 *	Filename: ToolPushB.c
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *		This is the source file for a Pushbutton that is used in
 *		conjunction with the XgToolBar class to dynamically add
 *		buttons to the tool bar
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	GMM	07/06/94	Original file
 *
 *
 * ---FHDR--- */


#include	<stdio.h>
#include	<ctype.h>

/*
 * These includes are needed to describe the widget
 */
#include 	<Xm/XmP.h>


/*
 * This include file are for the elements of the widget
 */
#include 	"ToolPushBP.h"
#include	"ToolBar.h"


/*
 * Declaration of methods
 */
static void Initialize();
static void Destroy();


XtTranslations toolTranslations;


static XtResource resources[] = {

    {
        XgNtoolPixmap, XmCPixmap, XmRPixmap, sizeof(Pixmap),
	XtOffsetOf(XgToolPushButtonRec, toolbtn.tool_pixmap), 
	XmRImmediate, (XtPointer)None
    }, 
    { 
        XgNdragCursor, XmCCursor, XmRCursor, sizeof(Cursor),
	XtOffsetOf(XgToolPushButtonRec, toolbtn.drag_cursor), 
	XmRImmediate, (XtPointer)None
    },
    {
	XgNstartDragProc, XgCStartDragProc, XmRFunction, sizeof(void(*)()),
	XtOffsetOf(XgToolPushButtonRec, toolbtn.start_drag_proc),
	XmRImmediate, (XtPointer)NULL
    }
};


XgToolPushButtonClassRec XgtoolPushButtonClassRec = {
    {
    
    	/*
	 * core_class fields
	 */
	(WidgetClass) &xmPushButtonClassRec,	/* superclass           */
	"XgToolPushButton",			/* class_name           */
	sizeof(XgToolPushButtonRec),		/* widget_size          */
	NULL,					/* class_initialize     */
	NULL,					/* class_part_initialize*/ 
	FALSE,					/* class_inited		*/
	Initialize,    				/* initialize		*/ 
	NULL,					/* initialize_hook	*/
	XtInheritRealize,			/* realize		*/
	NULL,					/* actions		*/
	0,					/* num_actions		*/
	resources,				/* resources		*/
	XtNumber(resources),			/* num_resources	*/
	NULLQUARK,				/* xrm_class		*/
	TRUE,					/* compress_motion	*/
	TRUE,					/* compress_exposure	*/
	TRUE,					/* compress_enterleave	*/
	TRUE,					/* visible_interest	*/
	Destroy,				/* destroy		*/
	XtInheritResize,			/* resize		*/
	XtInheritExpose,			/* expose		*/
	NULL,					/* set_values		*/
	NULL,					/* set_values_hook	*/
	XtInheritSetValuesAlmost,		/* set_values_almost	*/
	NULL,					/* get_values_hook	*/
	XtInheritAcceptFocus,			/* accept_focus		*/
	XtVersion,				/* version		*/
	NULL,					/* callback_private	*/
	NULL,					/* tm_table		*/
	NULL,					/* query_geometry	*/
	XtInheritDisplayAccelerator,		/* display_accelerator  */
	NULL,					/* extension            */
    },

    {
    	/*
	 * primitive_class fields
	 */
        ((XtWidgetProc) _XtInherit),      /* border highlight        */
        ((XtWidgetProc) _XtInherit),      /* border_unhighlight      */
       	XtInheritTranslations,            /* translations            */
        NULL,                             /* arm and activate        */
        NULL,                             /* synthetic resources     */
        0,                                /* number of syn res       */
	NULL,				  /* extension 		     */
    },

    {
    	/*
	 * label_class fields 
	 */
	XmInheritSetOverrideCallback,	/* setOverrideCallback	   	*/
	((XmMenuProc) _XtInherit),	/* menuProcs			*/
	XtInheritTranslations,		/* translations			*/
	NULL,				/* extension               	*/
    },

    { 
    	/*
	 * pushbutton_class
	 */
	0,				/* foo */
    },

    {
    	/*
	 * toolbtn_class fields
	 */
	0,				/* foo */
    }
};


WidgetClass xgToolPushButtonWidgetClass = 
	(WidgetClass) &XgtoolPushButtonClassRec;



static void
Initialize(request, new, args, num_args)
XgToolPushButtonWidget	request, new;
ArgList			args;
Cardinal		*num_args;
{
	XgToolBarRegisterWidget((Widget)new, XgToolBarDefaultCloner,
		NULL);
}



static void
Destroy(widget)
Widget widget;
{
	XgToolBarUnregisterWidget(widget);
}

