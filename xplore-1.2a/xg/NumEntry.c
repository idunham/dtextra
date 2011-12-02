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
 *	Filename: NumEntry.c
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *		This is the source file for a Motif Numerical Entry widget.
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	GMM	06/09/94	Original file
 *
 *
 * ---FHDR--- */


#include 	<math.h>
#include	<ctype.h>

/*
 * These includes are needed to describe the widget
 */
#include 	<Xm/XmP.h>
#include	<X11/StringDefs.h>


/*
 * These includ file are for the elements of the widget
 */
#include	<Xm/Text.h>
#include	<Xm/ArrowB.h>


#include 	"NumEntryP.h"


#ifndef	XmRFloat
#	define	XmRFloat	XtRFloat
#endif

/*
 * Declaration of methods
 */
static void Destroy();
static void Class_Initialize();
static void Initialize();
static Boolean SetValues();
static void Resize();
static XtGeometryResult GeometryManager();
static XtGeometryResult QueryGeometry();
static void InsertChild();


/*
 * Internal use functions and callbacks
 */
static void numentryArrowCB();
static void digitsOnlyCB();
static void ActivateCallback();
static Boolean CheckFocus();
static void FocusChangeHandler();
static void CopyBg();

#define AddFocusChangeHandler(w)	\
	XtAddEventHandler(w, FocusChangeMask, False, FocusChangeHandler, NULL)

#define TextChild(w)		((CompositeWidget)(w->numentry.text))
#define UpArrowChild(w)		((CompositeWidget)(w->numentry.up_arrow))
#define DownArrowChild(w)	((CompositeWidget)(w->numentry.down_arrow))




static char textTranslations[] =
    "<Key>osfUp:	    NumEntry-Manager(up)		       \n\
     <Key>osfDown:	    NumEntry-Manager(down)		       \n\
     <Key>osfPageUp:	    NumEntry-Manager(page-up)		       \n\
     <Key>osfPageDown:	    NumEntry-Manager(page-down)		       \n\
     <Btn1Down>:	    NumEntry-Manager(mouse-down) grab-focus()  \n\
     <Btn1Motion>:	    NumEntry-Manager(mouse-motion) extend-end() ";

static char noEditTextTranslations[] = 
    "<Key>osfBeginLine:	    NumEntry-Manager(top)  \n\
     <Key>osfEndLine:	    NumEntry-Manager(bottom)";

static void NumEntryActions();

static XtActionsRec actions[] = {
    { "NumEntry-Manager", NumEntryActions },
    { NULL, NULL }
};

static XtTranslations newTextTranslations, newNoEditTextTranslations;




static XtResource resources[] = {
    { 
	XmNcolumns, XmCColumns, XmRShort, sizeof(short),
       	XtOffsetOf(XgNumEntryRec, numentry.textColumns), 
	XmRImmediate, (XtPointer)10 
    },
    { 
	XgNtextBackground, XmCBackground, XmRPixel, sizeof(Pixel),
       	XtOffsetOf(XgNumEntryRec, numentry.textBg),
	XtRCallProc, (XtPointer)CopyBg 
    },
    { 
	XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension, 
	sizeof(Dimension), XtOffsetOf(XgNumEntryRec, numentry.margin_height),
	XmRImmediate, (XtPointer)5 
    },
    { 
	XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension, 
        sizeof(Dimension), XtOffsetOf(XgNumEntryRec, numentry.margin_width),
	XmRImmediate, (XtPointer)5 
    },
    { 
	XmNmaxLength, XmCMaxLength, XmRInt, sizeof(int),
       	XtOffsetOf(XgNumEntryRec, numentry.textMaxLength), 
	XmRImmediate, (XtPointer) 10 
    },
    { 
	XmNeditable, XmCEditable, XmRBoolean, sizeof(Boolean),
       	XtOffsetOf(XgNumEntryRec, numentry.editable),
	XmRImmediate, (XtPointer) True 
    },
    { 
	XmNverifyBell, XmCVerifyBell, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XgNumEntryRec, numentry.audible),
	XmRImmediate, (XtPointer) False 
    },
    { 
	XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
	XtOffsetOf(XgNumEntryRec, numentry.font_list),
	XmRImmediate, (XtPointer) NULL 
    },
    { 
	XmNminimum, XmCMinimum, XmRFloat, sizeof(float),
	XtOffsetOf(XgNumEntryRec, numentry.minimum),
	XmRString, "0.0" 
    },
    { 
	XmNmaximum, XmCMaximum, XmRFloat, sizeof(float),
	XtOffsetOf(XgNumEntryRec, numentry.maximum),
	XmRString, "100.0" 
    },
    { 
	XmNvalue, XmCValue, XmRFloat, sizeof(float),
	XtOffsetOf(XgNumEntryRec, numentry.value),
	XmRString, "0.0" 
    },
    { 
	XmNdecimalPoints, XmCDecimalPoints, XmRShort, sizeof(short),
	XtOffsetOf(XgNumEntryRec, numentry.precision),
	XmRImmediate, (XtPointer) 0 
    },
    { 
	XmNincrement, XmCIncrement, XmRFloat, sizeof(float),
	XtOffsetOf(XgNumEntryRec, numentry.increment),
	XmRString, "1.0" 
    },
    { 
	XmNfocusCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
	XtOffsetOf( XgNumEntryRec, numentry.focus_list ), 
	XmRImmediate, (XtPointer) NULL 
    },
    { 
	XmNlosingFocusCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
	XtOffsetOf( XgNumEntryRec, numentry.losing_focus_list ), 
	XmRImmediate, (XtPointer) NULL 
    },
    { 
	XmNvalueChangedCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
	XtOffsetOf( XgNumEntryRec, numentry.value_changed ), 
	XmRImmediate, (XtPointer) NULL 
    },
    { 
	XmNactivateCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
	XtOffsetOf( XgNumEntryRec, numentry.activate ), 
	XmRImmediate, (XtPointer) NULL 
    }
};

XgNumEntryClassRec XgnumEntryClassRec = {
    {
    /* core_class fields */
	/* superclass		*/ (WidgetClass) &xmManagerClassRec,
	/* class_name		*/ "XgNumEntry",
	/* widget_size		*/ sizeof(XgNumEntryRec),
	/* class_initialize	*/ Class_Initialize,
	/* class_part_initialize*/ NULL,
	/* class_inited		*/ False,
	/* initialize		*/ Initialize,
	/* initialize_hook	*/ NULL,
	/* realize		*/ XtInheritRealize,
	/* actions		*/ NULL,
	/* num_actions		*/ 0,
	/* resources		*/ resources,
	/* num_resources	*/ XtNumber(resources),
	/* xrm_class		*/ NULLQUARK,
	/* compress_motion	*/ True,
	/* compress_exposure	*/ XtExposeCompressMaximal,
	/* compress_enterleave	*/ True,
	/* visible_interest	*/ True,
	/* destroy		*/ Destroy,
	/* resize		*/ Resize,
	/* expose		*/ XtInheritExpose,
	/* set_values		*/ SetValues,
	/* set_values_hook	*/ NULL,
	/* set_values_almost	*/ XtInheritSetValuesAlmost,
	/* get_values_hook	*/ NULL,
	/* accept_focus		*/ XtInheritAcceptFocus,
	/* version		*/ XtVersion,
	/* callback_private	*/ NULL,
	/* tm_table		*/ XtInheritTranslations,
	/* query_geometry	*/ QueryGeometry,
	/* display_accelerator	*/ NULL,
	/* extension		*/ NULL
    },

    /* composite_class fields 	*/
    {
	/* geometry_manager	*/ GeometryManager,
	/* changed_managed	*/ XtInheritChangeManaged,
	/* insert_child		*/ InsertChild,
	/* delete_child		*/ XtInheritDeleteChild,
	/* extension		*/ NULL 
    },

    /* constraint_class fields 	*/
    {
	NULL,				/* resource list	*/		
	0,				/* num resources 	*/
	0,				/* constraint size	*/
	NULL,				/* init proc		*/
	NULL,				/* destroy proc       	*/
	NULL,				/* set values proc    	*/
	NULL
    },

    /* manager_class     */
    { 
	XtInheritTranslations, 		/* translations			*/
	NULL, 				/* syn_resources		*/
	0, 				/* num_syn_resources  		*/
	NULL, 				/* syn_cont_resources 		*/
	0, 				/* num_syn_cont_resources	*/
	XmInheritParentProcess, 	/* parent_process		*/
	NULL
    },

    /* NumEntry fields */
    {
	0,
    }
};



WidgetClass xgNumEntryWidgetClass = (WidgetClass) &XgnumEntryClassRec;


static Boolean
CvtStringToFloat(display, args, num_args, from, to)
Display 	*display;
XrmValuePtr	args;
Cardinal	*num_args;
XrmValuePtr	from;
XrmValuePtr	to;
{
int		i;
char		*ptr;
static float	value = (float) 0.0;

	if ( *num_args != 0 )
		XtWarningMsg("XtToolkitError", "wrongParameters",
			"CvtStringToFloat",
			"conversion needs no arguments", (String *) NULL,
			(Cardinal *) NULL);


	to->size = sizeof(float);

	/*
	 * User didn't provide enough space
	 */
	if ( to->addr != NULL && to->size < sizeof(float) )
		return False;


	/*
	 * Get the float from the string 
	 */
	value = (float)strtod((char *)from->addr, &ptr);

	if ( to->addr == NULL )
		to->addr = (caddr_t) &value;
	else
		*(float *) to->addr = value;

	/*
	 * Was the passed message a valid number?, if not then 
	 * send it back to the client and get outa here
	 */
	if ( value == (float)0.0 && ptr == (char *)from->addr)
		return False;


	return True;
}


static void 
CopyBg(cbw, offset, value)
XgNumEntryWidget cbw;
int  offset;
XrmValue * value;
{
    value->addr = (XtPointer) &cbw->core.background_pixel;
}




static void
Class_Initialize()
{
	XtSetTypeConverter(XmRString, XmRFloat, 
		(XtTypeConverter)CvtStringToFloat, 
		NULL, 0, XtCacheNone, NULL);

    	newTextTranslations = 
	    	XtParseTranslationTable(textTranslations);
    	newNoEditTextTranslations = 
		XtParseTranslationTable(noEditTextTranslations);
}


/* +++PHDR+++
 *
 * Function:	Initialize()
 *
 * Scope:	static
 *
 * Description:
 *	This function is the Initialize() method for the XgNumEntry widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * request	XgNumEntryWidget	Widget filled in from resources
 * new		XgNumEntryWidget	Copy of request widget that has
 *					  been potentialy altered by 
 *					  XgNumEntry's superclasses
 *
 *
 * Returns:	void
 *
 *
 * ---PHDR--- */

static void Initialize(request, new, args, num_args)
XgNumEntryWidget	request, new;
ArgList			args;
Cardinal		*num_args;
{
Dimension 	textWidth, textHeight, width, height, arrowWidth;
Position 	arrowX, arrowY;
Boolean		editable, cursorPositionVisible;


	new->numentry.initializing = True;

	/*
	 * Add the NumEntry's actions
	 */
	XtAppAddActions(XtWidgetToApplicationContext((Widget) new),
       		actions, XtNumber(actions));


	new->numentry.activate = new->numentry.value_changed = NULL;
	new->numentry.focus_list = new->numentry.losing_focus_list = NULL;
	new->numentry.workproc_id = None;
	new->numentry.focus_widget = NULL;
	new->numentry.timeout = None;

	XtVaSetValues((Widget)new, 
		XmNnavigationType, XmEXCLUSIVE_TAB_GROUP, NULL);



	/*
	 * Create the Text part of the Combo Box
	 */
	if ( new->numentry.editable == False )
		editable = cursorPositionVisible = False; 
	else 
		editable = cursorPositionVisible = True;


	/*
	 * Make sure the requested columns is valid
	 */
	if ( new->numentry.textColumns <= 0 ||
		new->numentry.textColumns > 32 )
	{
       		XtWarning("Columns must be > 0 and < 33");
		new->numentry.textColumns = 10;
	}

	/*
	 * Make sure the requested max length is valid
	 */
	if ( new->numentry.textMaxLength <= 0 ||
		new->numentry.textMaxLength > 32 )
	{
       		XtWarning("Max Length must be > 0 and < 33");
		new->numentry.textMaxLength = 10;
	}


	new->numentry.text = XtVaCreateManagedWidget(NULL,
		xmTextWidgetClass, (Widget) new,
		XmNfontList, new->numentry.font_list,
		XmNeditable, editable,
		XmNverifyBell, new->numentry.audible,
		XmNcursorPositionVisible, cursorPositionVisible,
		XmNrows, 1,
		XmNcolumns, new->numentry.textColumns,
		XmNmaxLength, new->numentry.textMaxLength,
		XmNmarginHeight, new->numentry.margin_height,
		XmNmarginWidth, new->numentry.margin_width,
		XmNwordWrap, False,
		XmNresizeWidth, False,
		XmNresizeHeight, False,
		XmNtraversalOn, True,
		XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNforeground, new->manager.foreground,
		XmNbackground, new->numentry.textBg,
		XmNnavigationType, XmNONE,
		NULL);

	/*
	 * Add an event handler for focus change events
	 */
	AddFocusChangeHandler(new->numentry.text);


	/*
	 * Now overridw the Text widgets translations with our actions
	 */
    	XtOverrideTranslations(new->numentry.text, newTextTranslations);
    	if ( !new->numentry.editable ) 
		XtOverrideTranslations(new->numentry.text, 
			newNoEditTextTranslations);

	/*
	 * Retieve the FontList
	 */
	if ( new->numentry.font_list == NULL )
		XtVaGetValues(new->numentry.text,
			XmNfontList, &new->numentry.font_list, NULL);


	/*
 	 *  Get the size and location of the Text Widget, we use this
	 *  to locate the arrow buttons
	 */
	XtVaGetValues(new->numentry.text,
		XmNwidth, &textWidth,
		XmNheight, &textHeight,
		XmNx, &arrowX,
		XmNy, &arrowY,
		NULL);
		

	/*
	 * Now create and manage the arrow buttons
	 */
	new->numentry.up_arrow = XtVaCreateManagedWidget(NULL,
		xmArrowButtonWidgetClass, (Widget) new,
		XmNarrowDirection, XmARROW_UP,
		XmNheight, (Dimension)textHeight / 2,
		XmNx, arrowX + textWidth,
		XmNy, 0,
		XmNforeground, new->manager.foreground,
		XmNbackground, new->core.background_pixel,
		XmNshadowThickness, 0,
		XmNhighlightThickness, 0,
		XmNtraversalOn, False,
		NULL);

	/*
	 * Add an event handler for focus change events
	 */
	AddFocusChangeHandler(new->numentry.up_arrow);

	new->numentry.down_arrow = XtVaCreateManagedWidget(NULL,
		xmArrowButtonWidgetClass, (Widget) new,
		XmNarrowDirection, XmARROW_DOWN,
		XmNheight, (Dimension)textHeight / 2,
		XmNx, arrowX + textWidth,
		XmNy, textHeight / 2,
		XmNforeground, new->manager.foreground,
		XmNbackground, new->core.background_pixel,
		XmNshadowThickness, 0,
		XmNhighlightThickness, 0,
		XmNtraversalOn, False,
		NULL);

	/*
	 * Add an event handler for focus change events
	 */
	AddFocusChangeHandler(new->numentry.down_arrow);

	/*
	*  Add the width and spacing of the ArrowButton to the textWidth
	*/
	XtVaGetValues(new->numentry.up_arrow, XmNwidth, &arrowWidth, NULL);
	textWidth += arrowWidth;


	/*
	 * Save these values as the minimal combo box size
	 */
	new->numentry.minWidth = textWidth;
	new->numentry.minHeight = textHeight;


	/*
	 *  Set the Combo Box to it's computed size, or resize it to the 
	 * passed size
	 */
	if ( textWidth > new->core.width )
		new->core.width = textWidth;

	if ( textHeight > new->core.height )
		new->core.height = textHeight;


	Resize(new);


	new->numentry.timeout = None;
	new->numentry.initializing = False;


	/*
	 * Put the original value in the text field
	 */
	XgNumEntrySetValue((Widget)new, new->numentry.value);


	/*
	 *  Add a callback to verify the text typed into the text widget
	 */
	XtAddCallback(new->numentry.text,
		XmNmodifyVerifyCallback, digitsOnlyCB, NULL);

	/*
	 * Add a callback for activate
	 */
	XtAddCallback(new->numentry.text,
		XmNactivateCallback, ActivateCallback, NULL);

	/*
	 *
	 */
	XtAddCallback(new->numentry.up_arrow, 
		XmNarmCallback, numentryArrowCB, (XtPointer)1);
	XtAddCallback(new->numentry.up_arrow,
		XmNdisarmCallback, numentryArrowCB, (XtPointer)1);

	XtAddCallback(new->numentry.down_arrow,
		XmNarmCallback, numentryArrowCB, (XtPointer)-1);
	XtAddCallback(new->numentry.down_arrow,
		XmNdisarmCallback, numentryArrowCB, (XtPointer)-1);
}


static Boolean
CheckFocus(ne)
XgNumEntryWidget        ne;
{
XgNumEntryCallbackStruct        cbs;

        ne->numentry.workproc_id = None;

        /*
         * See if we've lost or gained focus
         */
        if ( ne->numentry.focus_widget == NULL )
        {
		if ( ne->manager.has_focus == False )
			return True;

		ne->manager.has_focus = False;

                if ( ne->numentry.losing_focus_list == NULL )
                        return True;

                cbs.reason = XmCR_LOSING_FOCUS;
                cbs.event = NULL;
                cbs.value = ne->numentry.value;
                XtCallCallbacks((Widget)ne, XmNlosingFocusCallback, &cbs);
        }
        else if ( ne->manager.has_focus == False )
        {
		ne->manager.has_focus = True;

                if ( ne->numentry.focus_list == NULL )
                        return True;

                cbs.reason = XmCR_FOCUS;
                cbs.event = NULL;
                cbs.value = ne->numentry.value;
                XtCallCallbacks((Widget)ne, XmNfocusCallback, &cbs);
        }


        /*
         * Tell XtMainLoop to remove the work procedure
         */
        return True;
}


static void
FocusChangeHandler(w, not_used, event, continue_dispatch)
Widget                  w;
XFocusChangeEvent       *event;
XtPointer               not_used;
Boolean                 continue_dispatch;
{
XgNumEntryWidget        ne = (XgNumEntryWidget)XtParent(w);

        if ( event->type == FocusIn )
        {
                if ( ne->numentry.workproc_id == None &&
			ne->numentry.focus_widget == NULL )
		{
			
			ne->numentry.workproc_id = XtAppAddWorkProc(
				XtWidgetToApplicationContext(w),
				(XtWorkProc)CheckFocus, (XtPointer)ne);
		}

		ne->numentry.focus_widget = w;
	}

	if ( event->type == FocusOut )
	{
		ne->numentry.focus_widget = NULL;

                if ( ne->numentry.workproc_id == None )
		{
			ne->numentry.workproc_id = XtAppAddWorkProc(
				XtWidgetToApplicationContext(w),
				(XtWorkProc)CheckFocus, (XtPointer)ne);
		}
		
	}	
}


static XtGeometryResult
QueryGeometry(w, proposed, desired)
XgNumEntryWidget w;
XtWidgetGeometry *proposed, *desired;
{
#define Set(bit) (proposed->request_mode & bit)

	desired->width = TextChild(w)->core.width + 
		UpArrowChild(w)->core.width;
	desired->height = TextChild(w)->core.height;
	desired->request_mode = CWWidth | CWHeight;

	if ( Set(CWWidth) && proposed->width == desired->width &&
		Set(CWHeight) && proposed->height == desired->height )
			return XtGeometryYes;

	if ( desired->width == w->core.width &&
		desired->height == w->core.height )
			return XtGeometryNo;

	return XtGeometryAlmost;

#undef Set
}



static void
InsertChild(w)
Widget w;
{
XgNumEntryWidget	ne = (XgNumEntryWidget)XtParent(w);

	if ( ne->numentry.initializing != True ) 
	{
	        XtAppWarningMsg(XtWidgetToApplicationContext(w),
                        "insertChild", "badChild", "XgNumEntry",
                        "XgNumEntry: Cannot add children.",
                        (String *)NULL, (Cardinal *)NULL);
        	return;
    	}

    	(*((CompositeWidgetClass)
           (xgNumEntryWidgetClass->core_class.superclass))->composite_class.
     	    insert_child) (w);
}

static void
Destroy(w)
XgNumEntryWidget	w;
{
	/*
	 * Make sure this is a NumEntry widget
	 */
	if ( !XgIsNumEntry((Widget)w) )
		return;

	if ( w->numentry.timeout != None )
		XtRemoveTimeOut(w->numentry.timeout);

	if ( w->numentry.workproc_id != None )
		XtRemoveWorkProc(w->numentry.workproc_id);

	/*
	 * Remove the callbacks, if any installed
	 */
	if ( w->numentry.focus_list != NULL )
		XtRemoveCallbacks( (Widget)w, XmNfocusCallback,
			w->numentry.focus_list);

	if ( w->numentry.losing_focus_list != NULL )
		XtRemoveCallbacks( (Widget)w, XmNlosingFocusCallback,
			w->numentry.losing_focus_list);

	if ( w->numentry.value_changed != NULL )
		XtRemoveCallbacks( (Widget)w, XmNvalueChangedCallback,
			w->numentry.value_changed );

	if ( w->numentry.activate != NULL )
		XtRemoveCallbacks( (Widget)w, XmNactivateCallback,
			w->numentry.activate);
}


static XtGeometryResult 
GeometryManager(w, request, reply)
Widget		w;
XtWidgetGeometry	*request;
XtWidgetGeometry	*reply;
{
XgNumEntryWidget	cb = (XgNumEntryWidget)XtParent(w);

	if ( w == cb->numentry.up_arrow || w == cb->numentry.down_arrow )
	{
		if ( !(request->request_mode & (CWX | CWHeight)) )
			return XtGeometryNo;
			
	}
	else if ( w == cb->numentry.text )
	{
		if ( !(request->request_mode & (CWHeight | CWWidth)) )
			return XtGeometryNo;

	}
	else
		return XtGeometryNo;

	return XtGeometryYes;
}


static void
Resize(w)
XgNumEntryWidget	w;
{
Dimension	newWidth, width, height, border;
Position	y;

	/*
	 * What we need to do here is get the width of the arrow button
	 * and subtract that from the passwd width to compute the new
	 * width of the text portion of the combo box. We then use
	 * XtVaSetValues to set the new width of the text and the X location
	 * of the arrow button.
	 */ 
	XtVaGetValues((Widget)w->numentry.up_arrow, 
		XmNy, &y, XmNwidth, &width, NULL);
	newWidth = w->core.width - width;
	XtMoveWidget((Widget)w->numentry.up_arrow, newWidth, y);

	XtVaGetValues((Widget)w->numentry.down_arrow, 
		XmNy, &y, NULL);
	newWidth = w->core.width - width;
	XtMoveWidget((Widget)w->numentry.down_arrow, newWidth, y);

	XtVaGetValues((Widget)w->numentry.text, 
		XmNheight, &height, XmNborderWidth, &border, NULL);
	XtResizeWidget((Widget)w->numentry.text, newWidth, height, border);	

}


/* +++PHDR+++
 *
 * Function:	SetValues()
 *
 * Scope:	static	
 *
 * Description:
 *	SetValues() method for the XgNumEntry widget. 
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * currnet	Widget
 * request	Widget
 * new		Widget
 *
 *
 * Returns:	Boolean which indicates wether the widget needs to be redrawn.
 *
 *
 * ---PHDR--- */

static Boolean SetValues(current, request, new)
Widget	current, request, new;
{
XgNumEntryWidget	cur = (XgNumEntryWidget)current,
		        req = (XgNumEntryWidget)request,
		        neww = (XgNumEntryWidget)new;
Boolean			redraw = False;

	/* 
	 * Check for changed columns
	 */
	if ( cur->numentry.textColumns != neww->numentry.textColumns )
	{
		/*
		 * Make sure the requested columns is valid
		 */
		if ( neww->numentry.textColumns <= 0 ||
			neww->numentry.textColumns > 32 )
		{
       			XtWarning("Columns must be > 0 and < 33");
			neww->numentry.textColumns = 
				cur->numentry.textColumns;
		}
		/*
		 * Requested columns is valid, so set it
		 */
		else if ( neww->numentry.text != NULL )
		{
			XtVaSetValues(neww->numentry.text,
				XmNcolumns, neww->numentry.textColumns,
				NULL);

		}
		
		redraw = True;
	}

	/* 
	 * Check for changed max length of text
	 */
	if ( cur->numentry.textMaxLength != neww->numentry.textMaxLength )
	{
		/*
		 * Make sure the requested max length is valid
		 */
		if ( neww->numentry.textMaxLength <= 0 ||
			neww->numentry.textMaxLength > 32 )
		{
       			XtWarning("Max Length must be > 0 and < 33");
			neww->numentry.textMaxLength = 
				cur->numentry.textMaxLength;
		}
		/*
		 * Requested max length is valid, so set it
		 */
		else if ( neww->numentry.text != NULL )
		{
			XtVaSetValues(neww->numentry.text,
				XmNmaxLength, neww->numentry.textMaxLength,
				NULL);

		}
	}


	/* 
	 * Check for changed margin height
	 */
	if ( cur->numentry.margin_height != neww->numentry.margin_height && 
		neww->numentry.text != NULL )
	{
		XtVaSetValues(neww->numentry.text,
			XmNmarginHeight, neww->numentry.margin_height,
			NULL);

		redraw = True;
	}


	/* 
	 * Check for changed margin width
	 */
	if ( cur->numentry.margin_width != neww->numentry.margin_width && 
		neww->numentry.text != NULL )
	{
		XtVaSetValues(neww->numentry.text,
			XmNmarginWidth, neww->numentry.margin_width,
			NULL);

		redraw = True;
	}


	/* 
	 * Check for changed editable
	 */
	if ( cur->numentry.editable != neww->numentry.editable && 
		neww->numentry.text != NULL )
	{
		XtVaSetValues(neww->numentry.text,
			XmNeditable, neww->numentry.editable,
			NULL);
	}


	/* 
	 * Check for changed verifyBell 
	 */
	if ( cur->numentry.audible != neww->numentry.audible && 
		neww->numentry.text != NULL )
	{
		XtVaSetValues(neww->numentry.text,
			XmNverifyBell, neww->numentry.audible,
			NULL);
	}


        /*
         * Check for changed textBackground
         */
        if ( cur->numentry.textBg!= neww->numentry.textBg &&
                neww->numentry.text != NULL )
        {
                XtVaSetValues(neww->numentry.text,
                        XmNbackground, neww->numentry.textBg,
                        NULL);
        }


	/* 
	 * Check for changed font_list
	 */
	if ( cur->numentry.font_list != neww->numentry.font_list )
	{
		XtVaSetValues(neww->numentry.text,
			XmNfontList, neww->numentry.font_list,
			NULL);

		redraw = True;
	}

	/*
	 * Did the foreground color change?
	 */
	if ( neww->manager.foreground != cur->manager.foreground )
	{
		XtVaSetValues(neww->numentry.text,
			XmNforeground, neww->manager.foreground,
			NULL);

		XtVaSetValues(neww->numentry.up_arrow,
			XmNforeground, neww->manager.foreground,
			NULL);


		XtVaSetValues(neww->numentry.down_arrow,
			XmNforeground, neww->manager.foreground,
				NULL);
	}



	/*
	 * Did the background color change?
	 */
	if ( neww->core.background_pixel != cur->core.background_pixel )
	{
		XtVaSetValues(neww->numentry.text,
			XmNbackground, neww->core.background_pixel,
			NULL);

		XtVaSetValues(neww->numentry.up_arrow,
			XmNbackground, neww->core.background_pixel,
			NULL);


		XtVaSetValues(neww->numentry.down_arrow,
			XmNbackground, neww->core.background_pixel,
				NULL);
	}


	if ( redraw == True )
	{
	Dimension	width, height, bwidth;

		XtVaGetValues(neww->numentry.text, XmNwidth, &width,
			XmNheight, &height, NULL);

		XtVaSetValues(neww->numentry.up_arrow, 
			XmNwidth, (Dimension)(height * .75),
			XmNx, width, 
			XmNheight, height / 2, NULL);

		XtVaSetValues(neww->numentry.down_arrow, 
			XmNwidth, (Dimension)(height * .75),
			XmNx, width, 
			XmNy, height / 2,
			XmNheight, height / 2, NULL);

		XtVaGetValues(neww->numentry.up_arrow, 
			XmNwidth, &bwidth, NULL);

		if ( width + bwidth > neww->core.width )
			neww->core.width = req->core.width = width + bwidth;
	}


	return redraw;
}



static void
ActivateCallback(w, client_data, cb)
Widget			w;
XtPointer		client_data;
XmAnyCallbackStruct	*cb;
{
XgNumEntryWidget		ne = (XgNumEntryWidget)XtParent(w);
XgNumEntryCallbackStruct	cbs;

	if ( ne->numentry.activate != NULL )
	{
		cbs.reason = cb->reason;
		cbs.event = cb->event;
		cbs.value = ne->numentry.value;

		XtCallCallbacks((Widget)ne, XmNactivateCallback, &cbs);
	}
}



static void
digitsOnlyCB(w, client_data, cb)
Widget		w;
XtPointer	client_data;
XmTextVerifyPtr	cb;
{
char			*ptr, *text, digit_char, buf[64];
int			textLen;
float			value = (float) 0.0;
XmTextBlock		textBlock;
XgNumEntryWidget	ne = (XgNumEntryWidget)XtParent(w);
XgNumEntryCallbackStruct	cbs;


	if ( ne->numentry.setting_value == True )
		return;

	textBlock = cb->text;

	text = textBlock->ptr;

	if ( textLen = textBlock->length, textLen == 0 )
	{
		cb->doit = True;
		return;
	}

	/*
	 *  If it's a digit character then go on, otherwise enter the 
	 *  special checking mode
	 */
	digit_char = text[0];
	if ( !isdigit(digit_char) && digit_char != '-' && digit_char != '+' &&
		digit_char != '.' )
	{
		/*
		 *  Invalid data - don't modify
		 */			
		cb->doit = False;
		return;

	}		


	cb->doit = True;


	/*
	 * Convert the string in the Text widget to a char *
	 */
	if ( text = XmTextGetString(w), text == NULL )
		return;

	/*
	 * Make a copy of the text and free the original
	 */
	strncpy(buf, text, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = '\0';
	XtFree(text);
	textLen = strlen(buf);


	if ( digit_char == '-' || digit_char == '+' )
	{
		if ( buf[0] == '-' || buf[0] == '+' || 
			ne->numentry.minimum >= 0.0 )
				cb->doit = False;

		return;
	}

	if ( digit_char == '.' )
	{

		if ( ne->numentry.precision == 0 )
			cb->doit = False;
		else if ( strchr(buf, (int)'.') != NULL )
			cb->doit = False;

		return;
	}

	/*
	 * Now verify that we have not exceeded the precision
	 */
	if ( isdigit(digit_char) )
	{
	int	i;

		for ( i = 0, ptr = NULL; ptr == NULL && i < textLen; i++ )
			if ( buf[i] == '.' )
				ptr = buf + i;


		if ( ptr != NULL )
			if ( ptr + ne->numentry.precision < buf + textLen )
			{
				cb->doit = False;
				return;
			}
	}


	/* 
	 * Check that the number is within the specified min and max 
	 */
	if ( ne->numentry.minimum == ne->numentry.maximum )
	{
		ne->numentry.value = value;

		if ( ne->numentry.value_changed != NULL )
		{
			cbs.reason = XmCR_VALUE_CHANGED;
			cbs.event = cb->event;
			cbs.value = value;
			XtCallCallbacks((Widget)ne, 
				XmNvalueChangedCallback, &cbs);
		}
		return;
	}

	buf[textLen] = digit_char;
	buf[textLen + 1] = '\0';

	value = (float)strtod(buf, &ptr);

	if ( value == (float)0. && ptr == buf )
	{
		cb->doit = False;
		return;
	}

	if ( value < ne->numentry.minimum || value > ne->numentry.maximum )
	{
		cb->doit = False;
		return;
	}

	ne->numentry.value = value;

	if ( ne->numentry.value_changed != NULL )
	{
		cbs.reason = XmCR_VALUE_CHANGED;
		cbs.event = cb->event;
		cbs.value = value;
		XtCallCallbacks((Widget)ne, XmNvalueChangedCallback, &cbs);
	}
}


static void
arrow_timeout(ne)
XgNumEntryWidget        ne;
{
float	value;

	value = ne->numentry.value +
		((float)ne->numentry.direction * ne->numentry.increment);

	if ( XgNumEntrySetValue((Widget)ne, value) == FALSE )
	{
		if ( ne->numentry.timeout != None )
			XtRemoveTimeOut(ne->numentry.timeout);

		ne->numentry.timeout = None;
		return;
	}

	ne->numentry.timeout = XtAppAddTimeOut(
		XtWidgetToApplicationContext((Widget)ne),
		50, (XtTimerCallbackProc)arrow_timeout, (XtPointer)ne);
}


/* +++PHDR+++
 *
 * Function:	numentryArrowCB()
 *
 * Scope:	static
 *
 * Description:
 *	This function is called 
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:	void
 *
 *
 * ---PHDR--- */

static void
numentryArrowCB(arrow, direction, cbd)
Widget 			arrow;
int			direction;
XmAnyCallbackStruct 	*cbd;
{
XgNumEntryWidget        ne = (XgNumEntryWidget)XtParent(arrow);
float			value;

	if ( ne->numentry.increment == 0.0 )
		return;

	value = ne->numentry.value + 
		((float)direction * ne->numentry.increment);

	if ( cbd->reason == XmCR_DISARM )
	{
	XgNumEntryCallbackStruct	cbs;

		if ( ne->numentry.timeout != None )
			XtRemoveTimeOut(ne->numentry.timeout);
			
		ne->numentry.timeout = None;

		if ( ne->numentry.value_changed != NULL )
		{
			cbs.reason = XmCR_VALUE_CHANGED;
			cbs.event = cbd->event;
			cbs.value = ne->numentry.value;
			XtCallCallbacks((Widget)ne, 
				XmNvalueChangedCallback, &cbs);
		}
		
		return;
	}


	if ( XgNumEntrySetValue((Widget)ne, value) == FALSE )
	{
		if ( ne->numentry.timeout != None )
			XtRemoveTimeOut(ne->numentry.timeout);
			
		ne->numentry.timeout = None;
		return;
	}


	ne->numentry.direction = direction;
	ne->numentry.timeout = 
		XtAppAddTimeOut( XtWidgetToApplicationContext(arrow),
		500, (XtTimerCallbackProc)arrow_timeout, (XtPointer)ne);
}

 
/* +++PHDR+++
 *
 * Function:	XgNumEntryGetChild()
 *
 * Scope:	extern
 *
 * Description:
 *	This function returns a child widget of the passed XgNumEntry widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgNumEntry
 * child	unsigned char		Constant for child to return
 *
 * child constants:
 *	XgNUMENTRY_TEXT
 *	XgNUMENTRY_UP_ARROW
 *	XgNUMENTRY_DOWN_ARROW
 *
 *
 * Returns:	Widget id of child widget, NULL if error
 *
 *
 * ---PHDR--- */


Widget 
XgNumEntryGetChild(w, child)
Widget w; 
int child;
{
	/*
	 * Make sure this is a NumEntry widget
	 */
	if ( !XgIsNumEntry(w) )
		return NULL;

	switch ( (int) child )
	{
	  case XgNUMENTRY_TEXT:
		return(((XgNumEntryWidget)w)->numentry.text);

	  case XgNUMENTRY_UP_ARROW:
		return(((XgNumEntryWidget)w)->numentry.up_arrow);

	  case XgNUMENTRY_DOWN_ARROW:
		return(((XgNumEntryWidget)w)->numentry.down_arrow);
	}


	return NULL;
}

/* +++PHDR+++
 *
 * Function:	XgNumEntryGetString()
 *
 * Scope:	extern
 *
 * Description:
 *	This function returns a the string in the text portion of the 
 *	NumEntry widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgNumEntry
 *
 *
 * Returns: 	float
 *
 *
 * ---PHDR--- */


float
XgNumEntryGetValue(w)
Widget w; 
{
float 	value;
char	*ptr;
XgNumEntryWidget	ne = (XgNumEntryWidget)w;;


	/*
	 * Make sure this is a NumEntry widget
	 */
	if ( !XgIsNumEntry(w) )
		return (float)0.0;

	if ( ptr = XmTextGetString(ne->numentry.text), ptr == NULL )
		return (float)0.0;

	value = (float)atof(ptr);

	XtFree(ptr);

	return (float) value;
}


/* +++PHDR+++
 *
 * Function:	XgNumEntrySetString()
 *
 * Scope:	extern
 *
 * Description:
 *	This function sets the string in the text portion of the 
 *	NumEntry widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgNumEntry
 *
 *
 * Returns: 	void
 *
 *
 * ---PHDR--- */

int
XgNumEntrySetValue(w, value)
Widget 	w; 
double	value;
{
char			buf[80], *ptr;
XgNumEntryWidget	ne = (XgNumEntryWidget) w;

	/*
	 * Make sure this is a NumEntry widget
	 */
	if ( !XgIsNumEntry(w) )
		return FALSE;

	if ( ne->numentry.minimum != ne->numentry.maximum )
		if ( value < ne->numentry.minimum ||
			value > ne->numentry.maximum )
				return FALSE;

	sprintf(buf, "%-*.*hf", ne->numentry.textColumns, 
		ne->numentry.precision, value);

	for ( ptr = buf + strlen(buf) - 1; ptr > buf && *ptr == ' '; ptr-- )
		*ptr = '\0';

	ne->numentry.setting_value = True;
	XmTextSetString(ne->numentry.text, buf);
	ne->numentry.setting_value = False;

	ne->numentry.value = value;

	return TRUE;
}


XtArgVal
XgNumEntryFloatToXtArgVal(f)
double f;
{
static	XtArgVal	av;
static float		value;

	value = (float)f;

	memcpy((char *)&av, (char *)&value, 4);	

	return av;
}




static void NumEntryActions(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
XgNumEntryWidget	ne;
float			value;

    
    /*
     * Get the NumEntry widget
     */
    ne = (XgNumEntryWidget) XtParent(w);
    

    /*
     * See which action to take
     */
    switch ( *(params[0]) )
    {
      case 'u': /* up                */
        XgNumEntrySetValue((Widget)ne, 
		ne->numentry.value + ne->numentry.increment);
	break;

      case 'm': /* mouse             */

        /* mouse-down */
        if ( *((char *)params[0] + 6) == 'd' )
        {
	     ne->numentry.y = ((XButtonEvent *)event)->y;
	     break;
	}

	/* mouse-motion */
	value = ne->numentry.value + (float)((ne->numentry.y - 
		((XButtonEvent *)event)->y) / 
		(HeightOfScreen(XtScreen(w)) / 512)) *
		ne->numentry.increment;

	if ( value > ne->numentry.maximum )
		value = ne->numentry.maximum;
	else if ( value < ne->numentry.minimum )
		value = ne->numentry.minimum;

	XgNumEntrySetValue((Widget)ne, value);

	ne->numentry.y = ((XButtonEvent *)event)->y;

        break;


      case 'd': /* down              */
        XgNumEntrySetValue((Widget)ne, 
		ne->numentry.value - ne->numentry.increment);
	break;

      case 't': /* top               */
        XgNumEntrySetValue((Widget)ne, ne->numentry.maximum);
	break;

      case 'b': /* bottom            */
        XgNumEntrySetValue((Widget)ne, ne->numentry.minimum);
	break;


      case 'p': /* page-up/page-down */

	if ( *((char *)params[0] + 5) == 'u' )
		value = ne->numentry.value + ne->numentry.increment * 4;
	else
		value = ne->numentry.value - ne->numentry.increment * 4;;

	if ( value > ne->numentry.maximum )
		value = ne->numentry.maximum;
	else if ( value < ne->numentry.minimum )
		value = ne->numentry.minimum;

	XgNumEntrySetValue((Widget)ne, value);
	break;

    }
} 



