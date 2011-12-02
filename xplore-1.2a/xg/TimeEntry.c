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
 *	Filename: TimeEntry.c
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *		This is the source file for a Motif Time Entry widget.
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	GMM	08/12/94	Original file
 *
 *
 * ---FHDR--- */


#include	<stdio.h>
#include	<time.h>
#include	<stdlib.h>
#include	<ctype.h>

/*
 * These includes are needed to describe the widget
 */
#include 	<Xm/XmP.h>
#include	<Xm/XmStrDefs.h>

#include	<Xm/Label.h>

#include	<Xm/RowColumn.h>
#include        <X11/CompositeP.h>

/*
 * These includ file are for the elements of the widget
 */
#include	<Xm/Text.h>
#include	<Xm/ArrowB.h>

#include 	"TimeEntryP.h"



/*
 * defines for allowable minimum and maximum values
 *
 *	86400 = 00:00 2 Jan 1970
 *	86400 * 365 * 60 = 00:00 1 Jan 1970 + 60 years
 */
#define MIN_TIME_T	86400
#define MAX_TIME_T	86400 * 365 * 60


#ifndef RES_CONVERT
#	define RES_CONVERT(r, v)   XtVaTypedArg, r, XmRString, v, strlen(v) + 1
#endif


static int	days_in_month[13] = {
	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static char textTranslations[] =
    "<Key>osfUp:	    TimeEntry-Manager(up)	 \n\
     <Key>osfDown:	    TimeEntry-Manager(down)	 \n\
     <Key>osfPageUp:	    TimeEntry-Manager(page-up)	 \n\
     <Key>osfPageDown:	    TimeEntry-Manager(page-down) \n\
     Ctrl<Key>osfUp:        traverse-next()              \n\
     Ctrl<Key>osfDown:      traverse-prev()              \n\
     Ctrl<Key>osfRight:	    traverse-next()		 \n\
     Ctrl<Key>osfLeft:      traverse-prev()              \n\
     <Btn1Down>:	    TimeEntry-Manager(mouse-down) grab-focus()  \n\
     <Btn1Motion>:	    TimeEntry-Manager(mouse-motion) extend-end() ";



static char noEditTextTranslations[] = 
    "<Key>osfRight:	    traverse-next()\n\
     <Key>osfLeft:	    traverse-prev()";

static void TimeEntryActions();

static XtActionsRec actions[] = {
    { "TimeEntry-Manager", TimeEntryActions },
    { NULL, NULL }
};

static XtTranslations newTextTranslations, newNoEditTextTranslations;


/*
 * Declaration of methodsm and internal use functions
 */
static void Destroy();
static void ClassInitialize();
static void Initialize();
static void Resize();
static void ComputeSize();
static XtGeometryResult QueryGeometry();
static XtGeometryResult GeometryManager();
static Boolean SetValues();
static void InsertChild();
static void SetTextFocus();
static void ArrowCB();
static void ModifyVerifyCB();
static void ActivateCallback();
static void CreateDateWidgets(), CreateTimeWidgets(), CreateArrowWidgets();
static Boolean CheckFocus();
static void FocusChangeHandler();
static void CopyBg(), CopyFg();


#define AddFocusChangeHandler(w) \
	XtAddEventHandler(w, FocusChangeMask, False, FocusChangeHandler, NULL)\



/*
 * Macros for child widget access
 */
#define MONTCHILD(w)		\
	((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.month))
#define DATESLASHCHILD(w)	\
	((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.month_day_slash))
#define DAYCHILD(w)		\
	((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.day))
#define YEARSLASHCHILD(w)	\
	((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.day_year_slash))
#define YEARCHILD(w)		\
	((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.year))

#define HOURSCHILD(w)		\
  ((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.hours))
#define MINUTESCOLONCHILD(w)	\
  ((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.hours_minutes_colon))
#define MINUTESCHILD(w)		\
  ((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.minutes))
#define SECONDSCOLONCHILD(w)	\
  ((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.minutes_seconds_colon))
#define SECONDSCHILD(w)		\
  ((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.seconds))

#define UPARROWCHILD(w)		\
	((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.arrow_up))
#define DOWNARROWCHILD(w)	\
	((CompositeWidget)(((XgTimeEntryWidget)w)->timeentry.arrow_down))

/*
 * New resources 
 */
static XtResource resources[] = {
    {
	XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension, 
     	sizeof(Dimension), XtOffsetOf(XgTimeEntryRec, timeentry.margin_height),
	XmRImmediate, (XtPointer)5 
    },
    {
	XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension, 
	sizeof(Dimension), XtOffsetOf(XgTimeEntryRec, timeentry.margin_width),
	XmRImmediate, (XtPointer)3 
    },
    {
	XmNeditable, XmCEditable, XmRBoolean, sizeof(Boolean),
       	XtOffsetOf(XgTimeEntryRec, timeentry.editable),
	XmRImmediate, (XtPointer) True 
    },
    {
	XmNverifyBell, XmCVerifyBell, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XgTimeEntryRec, timeentry.audible),
	XmRImmediate, (XtPointer) False 
    },
    {
	XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
	XtOffsetOf(XgTimeEntryRec, timeentry.font_list),
	XmRImmediate, (XtPointer) NULL 
    },
    {
	XmNminimum, XmCMinimum, XmRInt, sizeof(long),
	XtOffsetOf(XgTimeEntryRec, timeentry.minimum),
	XmRImmediate, (XtPointer) 0
    },
    {
	XmNmaximum, XmCMaximum, XmRInt, sizeof(long),
	XtOffsetOf(XgTimeEntryRec, timeentry.maximum),
	XmRImmediate, (XtPointer) 0 
    },
    {
	XgNshowDate, XgCShowDate, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XgTimeEntryRec, timeentry.show_date),
	XmRImmediate, (XtPointer)True 
    },
    {
	XgNshowSeconds, XgCShowSeconds, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XgTimeEntryRec, timeentry.show_seconds),
	XmRImmediate, (XtPointer)True 
    },
    {
	XgNshowTime, XgCShowTime, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XgTimeEntryRec, timeentry.show_time),
	XmRImmediate, (XtPointer)True 
    },
    {
	XmNorientation, XmCOrientation, XmROrientation, sizeof(unsigned char),
	XtOffsetOf(XgTimeEntryRec, timeentry.orientation),
	XmRImmediate, (XtPointer)XmHORIZONTAL 
    },
    {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension),
	XtOffsetOf(XgTimeEntryRec, timeentry.spacing),
	XmRImmediate, (XtPointer)8
    },
    { 
	XgNtextBackground, XmCBackground, XmRPixel, sizeof(Pixel),
       	XtOffsetOf(XgTimeEntryRec, timeentry.textBg),
	XtRCallProc, (XtPointer)CopyBg 
    },
    { 
	XgNtextForeground, XmCBackground, XmRPixel, sizeof(Pixel),
       	XtOffsetOf(XgTimeEntryRec, timeentry.textFg),
	XtRCallProc, (XtPointer)CopyFg 
    },
    {
	XmNvalue, XmCValue, XmRInt, sizeof(long),
	XtOffsetOf(XgTimeEntryRec, timeentry.value),
	XmRImmediate, (XtPointer)-1 
    },
    {
	XgNuseTimeZone, XgCUseTimeZone, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XgTimeEntryRec, timeentry.use_timezone),
	XmRImmediate, (XtPointer)True
    },
    { 
	XmNfocusCallback, XmCCallback, XmRCallback, sizeof(XtPointer),
	XtOffsetOf( XgTimeEntryRec, timeentry.focus_list ), 
	XmRImmediate, (XtPointer) NULL 
    },
    { 
	XmNlosingFocusCallback, XmCCallback, XmRCallback, sizeof(XtPointer),
	XtOffsetOf( XgTimeEntryRec, timeentry.losing_focus_list ), 
	XmRImmediate, (XtPointer) NULL 
    },
    {
	XmNactivateCallback, XmCCallback, XmRCallback, sizeof(XtPointer),
	XtOffsetOf( XgTimeEntryRec, timeentry.activate ), 
	XmRImmediate, (XtPointer) NULL 
    },
    {
	XmNvalueChangedCallback, XmCCallback, XmRCallback, sizeof(XtPointer),
	XtOffsetOf( XgTimeEntryRec, timeentry.value_changed ), 
	XmRImmediate, (XtPointer) NULL 
    }

   /* {
	XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     	sizeof(Dimension), XtOffsetOf(XmManagerRec, manager.shadow_thickness),
     	XmRImmediate, (XtPointer)0
    } */
};


XgTimeEntryClassRec XgtimeEntryClassRec = {
    {
    /* core_class fields */
	/* superclass		*/ (WidgetClass) &xmManagerClassRec,
	/* class_name		*/ "XgTimeEntry",
	/* widget_size		*/ sizeof(XgTimeEntryRec),
	/* class_initialize	*/ ClassInitialize,
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

    /* TimeEntry fields */
    {
	0
    }
};


WidgetClass xgTimeEntryWidgetClass = (WidgetClass) &XgtimeEntryClassRec;



void SetManagedTimeWidgets(w)
XgTimeEntryWidget	w;
{
	if ( w->timeentry.show_time )
	{
		XtManageChild(w->timeentry.hours);
		XtManageChild(w->timeentry.hours_minutes_colon);
		XtManageChild(w->timeentry.minutes);
		if (  w->timeentry.show_seconds )
		{
			XtManageChild(w->timeentry.minutes_seconds_colon);
			XtManageChild(w->timeentry.seconds);
		}
		else
		{
			XtUnmanageChild(w->timeentry.minutes_seconds_colon);
			XtUnmanageChild(w->timeentry.seconds);
		}
	}
	else
	{
		XtUnmanageChild(w->timeentry.hours);
		XtUnmanageChild(w->timeentry.hours_minutes_colon);
		XtUnmanageChild(w->timeentry.minutes);
		XtUnmanageChild(w->timeentry.minutes_seconds_colon);
		XtUnmanageChild(w->timeentry.seconds);
	}
	
}


void SetManagedDateWidgets(w)
XgTimeEntryWidget	w;
{
	if ( w->timeentry.show_date )
	{
		XtManageChild(w->timeentry.month);
		XtManageChild(w->timeentry.month_day_slash);
		XtManageChild(w->timeentry.day);
		XtManageChild(w->timeentry.day_year_slash);
		XtManageChild(w->timeentry.year);
	}
	else
	{
		XtUnmanageChild(w->timeentry.month);
		XtUnmanageChild(w->timeentry.month_day_slash);
		XtUnmanageChild(w->timeentry.day);
		XtUnmanageChild(w->timeentry.day_year_slash);
		XtUnmanageChild(w->timeentry.year);
	}
}


static XgTimeEntryWidget
GetTimeEntryWidget(w)
Widget w;
{
XgTimeEntryWidget	te;

	te = (XgTimeEntryWidget)w;
	while ( !XgIsTimeEntry((Widget)te) )
		te = (XgTimeEntryWidget)XtParent((Widget)te);

	return te;
}



static void
ClassInitialize()
{
    	newTextTranslations = 
	    	XtParseTranslationTable(textTranslations);
}

static void 
CopyFg(tew, offset, value)
XgTimeEntryWidget tew;
int  offset;
XrmValue * value;
{
    value->addr = (XtPointer) &tew->manager.foreground;
}


static void 
CopyBg(tew, offset, value)
XgTimeEntryWidget tew;
int  offset;
XrmValue * value;
{
    value->addr = (XtPointer) &tew->core.background_pixel;
}


/* +++PHDR+++
 *
 * Function:	Initialize()
 *
 * Scope:	static
 *
 * Description:
 *	This function is the Initialize() method for the XgTimeEntry widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * request	XgTimeEntryWidget	Widget filled in from resources
 * new		XgTimeEntryWidget	Copy of request widget that has
 *					  been potentialy altered by 
 *					  XgTimeEntry's superclasses
 *
 *
 * Returns:	void
 *
 *
 * ---PHDR--- */

static void 
Initialize(request, new, args, num_args)
XgTimeEntryWidget	request, new;
ArgList			args;
Cardinal		*num_args;
{
Dimension 	width, height;


	new->timeentry.initializing = True;

        /*
         * Add the NumEntry's actions
         */
        XtAppAddActions(XtWidgetToApplicationContext((Widget) new),
                actions, XtNumber(actions));


	new->timeentry.resized = False;

	new->timeentry.timeout = None;
	new->timeentry.value_changed = new->timeentry.activate = NULL;
	new->timeentry.focus_widget = NULL;
	new->timeentry.workproc_id = None;


	/*
	 * Create the date part of the TimeEntry Widget
	 */
	CreateDateWidgets(new);

	/*
	 * Create the time part of the TimeEntry Widget
	 */
	CreateTimeWidgets(new);

	/*
 	 * Create the arrow button part of the time entry widget
	 */
	CreateArrowWidgets(new);
	

	/*
	 *  Set the widget to it's computed size, or resize it to the 
	 * passed size
	 */
	ComputeSize(new, &width, &height, NULL, NULL);

	if ( width > new->core.width )
		new->core.width = width;

	if ( height > new->core.height )
		new->core.height = height;

	Resize(new);


	/*
	 * Put the original value in the text field
	 */
	XgTimeEntrySetValue((Widget)new, new->timeentry.value);


	new->timeentry.text_focus = NULL;
	if ( new->timeentry.show_date )
		SetTextFocus(new->timeentry.month);
	else
		SetTextFocus(new->timeentry.hours);
	

	new->timeentry.timeout = None;
	new->timeentry.initializing = False;
}

/* +++PHDR+++
 *
 * Function:	CreateDateWidgets()
 *
 * Scope:	static
 *
 * Description:
 *	This function
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		XgTimeEntryWidget	
 *
 *
 * Returns:	void
 *
 *
 * ---PHDR--- */

static void
CreateDateWidgets(w)
XgTimeEntryWidget 	w;
{
Boolean	editable;
Widget	box, text;
Position	x;


	editable = w->timeentry.editable;
	
	x = 0;
	w->timeentry.month = text =
	    XtVaCreateWidget(NULL,
		xmTextWidgetClass,
		(Widget)w,
		XmNx, x,
		XmNy, 0,
		XmNeditable, editable,
		XmNverifyBell, w->timeentry.audible,
		XmNcolumns, 2,
		XmNfontList, w->timeentry.font_list,
		XmNmaxLength, 3,
		XmNcursorPositionVisible, False,
		XmNmarginHeight, w->timeentry.margin_height,
		XmNmarginWidth, w->timeentry.margin_width,
		XmNresizeWidth, False,
		XmNresizeHeight, False,
		XmNtraversalOn, True,
		XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		XmNnavigationType, XmNONE,
		XmNshadowThickness, 0,
		NULL);

        /*
         * Now overridw the Text widgets translations with our actions
         */
        XtOverrideTranslations(text, newTextTranslations);


	if ( w->timeentry.font_list == NULL )
		XtVaGetValues(w->timeentry.month, 
			XmNfontList, &w->timeentry.font_list, NULL);

	x += MONTCHILD(w)->core.width;
	w->timeentry.month_day_slash = XtVaCreateWidget(NULL,
		xmLabelWidgetClass,
		(Widget)w,
		XmNx, x,
		XmNy, 0,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		RES_CONVERT(XmNlabelString, "/"),
		XmNfontList, w->timeentry.font_list,
		XmNmarginHeight, w->timeentry.margin_height,
		XmNmarginWidth, 0,
		NULL);

	x += DATESLASHCHILD(w)->core.width;
	w->timeentry.day = text = XtVaCreateWidget(NULL,
		xmTextWidgetClass,
		(Widget)w,
		XmNx, x,
		XmNy, 0,
		XmNeditable, editable,
		XmNverifyBell, w->timeentry.audible,
		XmNcursorPositionVisible, False,
		XmNcolumns, 2,
		XmNfontList, w->timeentry.font_list,
		XmNmaxLength, 2,
		XmNmarginHeight, w->timeentry.margin_height,
		XmNmarginWidth, w->timeentry.margin_width,
		XmNresizeWidth, False,
		XmNresizeHeight, False,
		XmNtraversalOn, True,
		XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		XmNnavigationType, XmNONE,
		XmNshadowThickness, 0,
		NULL);

        /*
         * Now overridw the Text widgets translations with our actions
         */
        XtOverrideTranslations(text, newTextTranslations);

	
	x += DAYCHILD(w)->core.width;
	w->timeentry.day_year_slash = XtVaCreateWidget(NULL,
		xmLabelWidgetClass,
		(Widget)w,
		XmNx, x,
		XmNy, 0,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		RES_CONVERT(XmNlabelString, "/"),
		XmNmarginHeight, w->timeentry.margin_height,
		XmNmarginWidth, 0,
		XmNfontList, w->timeentry.font_list,
		NULL);

	x += YEARSLASHCHILD(w)->core.width;
	w->timeentry.year = text = XtVaCreateWidget(NULL,
		xmTextWidgetClass,
		(Widget)w,
		XmNx, x,
		XmNy, 0,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		XmNeditable, editable,
		XmNverifyBell, w->timeentry.audible,
		XmNcursorPositionVisible, False,
		XmNcolumns, 2,
		XmNfontList, w->timeentry.font_list,
		XmNmaxLength, 2,
		XmNmarginHeight, w->timeentry.margin_height,
		XmNmarginWidth, w->timeentry.margin_width,
		XmNresizeWidth, False,
		XmNresizeHeight, False,
		XmNtraversalOn, True,
		XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNnavigationType, XmNONE,
		XmNshadowThickness, 0,
		NULL);

        /*
         * Now overridw the Text widgets translations with our actions
         */
        XtOverrideTranslations(text, newTextTranslations);

	/*
	 * Add callbacks for focus (so we know what text widget to act
	 * on when one of the arrow buttons is pressed), and modifyVerify
	 * (to verify the entry).
	 */
	AddFocusChangeHandler(w->timeentry.month);
	AddFocusChangeHandler(w->timeentry.day);
	AddFocusChangeHandler(w->timeentry.year);

	XtAddCallback(w->timeentry.month,
		XmNmodifyVerifyCallback, ModifyVerifyCB, NULL);
	XtAddCallback(w->timeentry.month,
		XmNactivateCallback, ActivateCallback, NULL);

	XtAddCallback(w->timeentry.day,
		XmNmodifyVerifyCallback, ModifyVerifyCB, NULL);
	XtAddCallback(w->timeentry.day,
		XmNactivateCallback, ActivateCallback, NULL);

	XtAddCallback(w->timeentry.year,
		XmNmodifyVerifyCallback, ModifyVerifyCB, NULL);
	XtAddCallback(w->timeentry.year,
		XmNactivateCallback, ActivateCallback, NULL);

	SetManagedDateWidgets(w);
}


/* +++PHDR+++
 *
 * Function:	CreateTimeWidgets()
 *
 * Scope:	static
 *
 * Description:
 *	This function 
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		XgTimeEntryWidget	
 *
 *
 * Returns:	void
 *
 *
 * ---PHDR--- */

static void
CreateTimeWidgets(w)
XgTimeEntryWidget 	w;
{
Boolean	editable;
Widget	box, text;


	editable = w->timeentry.editable;

	w->timeentry.hours = text =
	    XtVaCreateWidget(NULL,
		xmTextWidgetClass,
		(Widget)w,
		XmNeditable, editable,
		XmNverifyBell, w->timeentry.audible,
		XmNcursorPositionVisible, False,
		XmNcolumns, 2,
		XmNfontList, w->timeentry.font_list,
		XmNmaxLength, 2,
		XmNmarginHeight, w->timeentry.margin_height,
		XmNmarginWidth, w->timeentry.margin_width,
		XmNresizeWidth, False,
		XmNresizeHeight, False,
		XmNtraversalOn, True,
		XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		XmNnavigationType, XmNONE,
		XmNshadowThickness, 0,
		NULL);

        /*
         * Now overridw the Text widgets translations with our actions
         */
        XtOverrideTranslations(text, newTextTranslations);


	w->timeentry.hours_minutes_colon = XtVaCreateWidget(NULL,
		xmLabelWidgetClass,
		(Widget)w,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		RES_CONVERT(XmNlabelString, ":"),
		XmNfontList, w->timeentry.font_list,
		XmNmarginWidth, 0,
		XmNmarginHeight, w->timeentry.margin_height,
		NULL);

	w->timeentry.minutes = text = XtVaCreateWidget(NULL,
		xmTextWidgetClass,
		(Widget)w,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		XmNeditable, editable,
		XmNverifyBell, w->timeentry.audible,
		XmNcursorPositionVisible, False,
		XmNcolumns, 2,
		XmNfontList, w->timeentry.font_list,
		XmNmaxLength, 2,
		XmNmarginHeight, w->timeentry.margin_height,
		XmNmarginWidth, w->timeentry.margin_width,
		XmNresizeWidth, False,
		XmNresizeHeight, False,
		XmNtraversalOn, True,
		XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNnavigationType, XmNONE,
		XmNshadowThickness, 0,
		NULL);

        /*
         * Now overridw the Text widgets translations with our actions
         */
        XtOverrideTranslations(text, newTextTranslations);


	w->timeentry.minutes_seconds_colon = XtVaCreateWidget(NULL,
		xmLabelWidgetClass,
		(Widget)w,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		RES_CONVERT(XmNlabelString, ":"),
		XmNfontList, w->timeentry.font_list,
		XmNmarginWidth, 0,
		XmNmarginHeight, w->timeentry.margin_height,
		NULL);

	w->timeentry.seconds = text = XtVaCreateWidget(NULL,
		xmTextWidgetClass,
		(Widget)w,
		XmNeditable, editable,
		XmNverifyBell, w->timeentry.audible,
		XmNcursorPositionVisible, False,
		XmNcolumns, 2,
		XmNfontList, w->timeentry.font_list,
		XmNmaxLength, 2,
		XmNmarginHeight, w->timeentry.margin_height,
		XmNmarginWidth, w->timeentry.margin_width,
		XmNresizeWidth, False,
		XmNresizeHeight, False,
		XmNtraversalOn, True,
		XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNnavigationType, XmNONE,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		XmNshadowThickness, 0,
		NULL);

        /*
         * Now overridw the Text widgets translations with our actions
         */
        XtOverrideTranslations(text, newTextTranslations);

	/*
	 * Add callbacks for focus (so we know what text widget to act
	 * on when one of the arrow buttons is pressed), and modifyVerify
	 * (to verify the entry).
	 */
	AddFocusChangeHandler(w->timeentry.hours);
	AddFocusChangeHandler(w->timeentry.minutes);
	AddFocusChangeHandler(w->timeentry.seconds);
	
	XtAddCallback(w->timeentry.hours,
		XmNmodifyVerifyCallback, ModifyVerifyCB, NULL);
	XtAddCallback(w->timeentry.hours,
		XmNactivateCallback, ActivateCallback, NULL);

	XtAddCallback(w->timeentry.minutes,
		XmNmodifyVerifyCallback, ModifyVerifyCB, NULL);
	XtAddCallback(w->timeentry.minutes,
		XmNactivateCallback, ActivateCallback, NULL);

	XtAddCallback(w->timeentry.seconds,
		XmNmodifyVerifyCallback, ModifyVerifyCB, NULL);
	XtAddCallback(w->timeentry.seconds,
		XmNactivateCallback, ActivateCallback, NULL);

	SetManagedTimeWidgets(w);
}


static void
CreateArrowWidgets(w)
XgTimeEntryWidget       w;
{
Widget box;

	/*
	 * Create the Arrow button paraent and the buttons
	 */
	w->timeentry.arrow_up = XtVaCreateManagedWidget(NULL,
		xmArrowButtonWidgetClass,
		(Widget)w,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		XmNheight, HOURSCHILD(w)->core.height / 2,
		XmNhighlightThickness, 0,
		XmNtraversalOn, False,
		XmNarrowDirection, XmARROW_UP,
		XmNshadowThickness, 0,
		XmNx, 0,
		XmNy, 0,
		NULL);

	w->timeentry.arrow_down = XtVaCreateManagedWidget(NULL,
		xmArrowButtonWidgetClass,
		(Widget)w,
		XmNforeground, w->timeentry.textFg,
		XmNbackground, w->timeentry.textBg,
		XmNheight, HOURSCHILD(w)->core.height / 2,
		XmNhighlightThickness, 0,
		XmNtraversalOn, False,
		XmNarrowDirection, XmARROW_DOWN,
		XmNshadowThickness, 0,
		XmNx, 0,
		XmNy, 0,
		NULL);


	/*
	 * Add callbacks for the Arrow Buttons
	 */
	XtAddCallback(w->timeentry.arrow_up, 
		XmNarmCallback, ArrowCB, (XtPointer)1);
	XtAddCallback(w->timeentry.arrow_up,
		XmNdisarmCallback, ArrowCB, (XtPointer)1);

	XtAddCallback(w->timeentry.arrow_down,
		XmNarmCallback, ArrowCB, (XtPointer)-1);
	XtAddCallback(w->timeentry.arrow_down,
		XmNdisarmCallback, ArrowCB, (XtPointer)-1);
}


static void
ComputeSize(w, all_width, all_height, date_width, date_height)
XgTimeEntryWidget	w;
Dimension		*all_width;
Dimension		*all_height;
Dimension		*date_width;
Dimension		*date_height;
{
Dimension	width, height;

	if ( w->timeentry.show_date == True )
	{
		width = MONTCHILD(w)->core.width + 
			DATESLASHCHILD(w)->core.width +
			DAYCHILD(w)->core.width +
			YEARSLASHCHILD(w)->core.width +
			YEARCHILD(w)->core.width;

		height = MONTCHILD(w)->core.height;

		if ( date_width != NULL )
			*date_width = width;
		
		if ( date_height != NULL )
			*date_height = height;

		if ( w->timeentry.show_time == True )
		{
			if ( w->timeentry.orientation == XmHORIZONTAL )
			{
				width += HOURSCHILD(w)->core.width + 
					MINUTESCOLONCHILD(w)->core.width +
					MINUTESCHILD(w)->core.width +
					w->timeentry.spacing;

				if ( w->timeentry.show_seconds == True )
					width +=  
					    (SECONDSCOLONCHILD(w)->core.width +
						SECONDSCHILD(w)->core.width) ;
			}
			else
			{
				height += UPARROWCHILD(w)->core.height +
					DOWNARROWCHILD(w)->core.height +
					w->timeentry.spacing;
			}
		}
	}
	else if ( w->timeentry.show_time == True )
	{
		width = HOURSCHILD(w)->core.width + 
			MINUTESCOLONCHILD(w)->core.width +
			MINUTESCHILD(w)->core.width;

		if ( w->timeentry.show_seconds == True )
			width +=  (SECONDSCOLONCHILD(w)->core.width +
				SECONDSCHILD(w)->core.width) ;

		height = UPARROWCHILD(w)->core.height +
			DOWNARROWCHILD(w)->core.height;
	}
	else
		width = height = 10;

	width += UPARROWCHILD(w)->core.width + 1;

	if ( all_width != NULL )
		*all_width = width;

	if ( all_height != NULL )
		*all_height = height;

}


static Boolean
CheckFocus(te)
XgTimeEntryWidget        te;
{
XgTimeEntryCallbackStruct        cbs;

        te->timeentry.workproc_id = None;

        /*
         * See if we've lost or gained focus
         */
        if ( te->timeentry.focus_widget == NULL )
        {
		if ( te->manager.has_focus == False )
			return True;

		te->manager.has_focus = False;

                if ( te->timeentry.losing_focus_list == NULL )
                        return True;

                cbs.reason = XmCR_LOSING_FOCUS;
                cbs.event = NULL;
                cbs.value = te->timeentry.value;
                XtCallCallbacks((Widget)te, XmNlosingFocusCallback, &cbs);
        }
        else if ( te->manager.has_focus == False )
        {
		te->manager.has_focus = True;

                if ( te->timeentry.focus_list == NULL )
                        return True;

                cbs.reason = XmCR_FOCUS;
                cbs.event = NULL;
                cbs.value = te->timeentry.value;
                XtCallCallbacks((Widget)te, XmNfocusCallback, &cbs);
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
XgTimeEntryWidget       te = GetTimeEntryWidget(w);

        if ( event->type == FocusIn )
        {
		SetTextFocus(w);
	

                if ( te->timeentry.workproc_id == None )
		{
			te->timeentry.workproc_id = XtAppAddWorkProc(
				XtWidgetToApplicationContext(w),
				(XtWorkProc)CheckFocus, (XtPointer)te);
		}

		te->timeentry.focus_widget = w;
	}

	if ( event->type == FocusOut )
	{
		if ( te->timeentry.workproc_id == None )
		{
			te->timeentry.focus_widget = NULL;
			te->timeentry.workproc_id = XtAppAddWorkProc(
				XtWidgetToApplicationContext(w),
				(XtWorkProc)CheckFocus, (XtPointer)te);
		}
		
	}	
}


static void
Resize(w)
XgTimeEntryWidget	w;
{
Dimension	width, height, time_width;
Position	time_x, time_y;


	/*
	 * First get the width of the time widgets
	 */
	time_width = HOURSCHILD(w)->core.width +
		MINUTESCOLONCHILD(w)->core.width +
		MINUTESCHILD(w)->core.width;
	if ( w->timeentry.show_time && w->timeentry.show_seconds )
		time_width += SECONDSCOLONCHILD(w)->core.width +
			SECONDSCHILD(w)->core.width;


	/*
	 *
	 */
	if ( w->timeentry.show_date == True )
	{
		ComputeSize(w, NULL, NULL, &width, &height);
		
		if (  w->timeentry.show_date == True )
		{
			if ( w->timeentry.orientation == XmHORIZONTAL )
			{
				time_x = width + w->timeentry.spacing;
				time_y = 0;
			}
			else
			{
				time_y = height + w->timeentry.spacing;
				time_x = 0;
			}
		}
		else
		{
			time_x = width;
			time_y = 0;
			time_width = 0;
		}
	}
	else if ( w->timeentry.show_date == True )
	{
		time_x = 0;
		time_y = 0;
	}
	

	/*
	 * Move and size the arrow widgets
	 */
	height = HOURSCHILD(w)->core.height / 2;
	XtVaSetValues(w->timeentry.arrow_up, 
		XmNheight, height,
		XmNx, time_x + time_width + 1,
		XmNy, time_y, NULL);
	XtVaSetValues(w->timeentry.arrow_down, 
		XmNheight, height, 
		XmNx, time_x + time_width + 1,
		XmNy, time_y + UPARROWCHILD(w)->core.height, NULL);


	/*
	 * Now move the time widgets
	 */
	XtMoveWidget(w->timeentry.hours, time_x, time_y);
	time_x += HOURSCHILD(w)->core.width;
	XtMoveWidget(w->timeentry.hours_minutes_colon, time_x, time_y);
	time_x += MINUTESCOLONCHILD(w)->core.width;
	XtMoveWidget(w->timeentry.minutes, time_x, time_y);
	time_x += MINUTESCHILD(w)->core.width;
	XtMoveWidget(w->timeentry.minutes_seconds_colon, time_x, time_y);
	time_x += SECONDSCOLONCHILD(w)->core.width;
	XtMoveWidget(w->timeentry.seconds, time_x, time_y);


	/*
	 * Set the height of each label widget
	 */
	height = HOURSCHILD(w)->core.height;
	XtVaSetValues(
		w->timeentry.month_day_slash, XmNheight, height, NULL);
	XtVaSetValues(
		w->timeentry.day_year_slash, XmNheight, height, NULL);
	XtVaSetValues(
		w->timeentry.hours_minutes_colon, XmNheight, height, NULL);
	XtVaSetValues(
		w->timeentry.minutes_seconds_colon, XmNheight, height, NULL);


	/*
	 * Make sure the arrow buttons are on top
	 */
	if ( XtWindow(w->timeentry.arrow_up) != None )
		XRaiseWindow(XtDisplay(w->timeentry.arrow_up),
			XtWindow(w->timeentry.arrow_up));

	if ( XtWindow(w->timeentry.arrow_down) != None )
		XRaiseWindow(XtDisplay(w->timeentry.arrow_down),
			XtWindow(w->timeentry.arrow_down));
}


static XtGeometryResult
QueryGeometry(w, proposed, desired)
XgTimeEntryWidget w;
XtWidgetGeometry *proposed, *desired;
{
Dimension	width, height;

#define Set(bit) (proposed->request_mode & bit)

	ComputeSize(w, &width, &height, NULL, NULL);
	desired->width = width;
	desired->height = height;

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


static XtGeometryResult
GeometryManager(w, request, reply)
Widget		w;
XtWidgetGeometry	*request;
XtWidgetGeometry	*reply;
{
XgTimeEntryWidget	te = GetTimeEntryWidget(w);

	if ( !(request->request_mode & (CWX|CWY|CWWidth|CWHeight)) )
		return XtGeometryNo;

	return XtGeometryYes;
}



static void
InsertChild(w)
Widget w;
{
XgTimeEntryWidget	te = GetTimeEntryWidget(w);

	if ( te->timeentry.initializing != True ) 
	{
	        XtAppWarningMsg(XtWidgetToApplicationContext(w),
                        "insertChild", "badChild", "XgTimeEntry",
                        "XgTimeEntry: Cannot add children.",
                        (String *)NULL, (Cardinal *)NULL);
        	return;
    	}

    	(*((CompositeWidgetClass)
           (xgTimeEntryWidgetClass->core_class.superclass))->composite_class.
     	    insert_child) (w);
}

static void
Destroy(w)
XgTimeEntryWidget	w;
{
	/*
	 * Make sure this is a TimeEntry widget
	 */
	if ( !XgIsTimeEntry((Widget)w) )
		return;

	if ( w->timeentry.timeout != None )
		XtRemoveTimeOut(w->timeentry.timeout);

	if ( w->timeentry.workproc_id != None )
		XtRemoveWorkProc(w->timeentry.workproc_id);

	/*
	 * Remove the callbacks, if any installed
	 */
	if ( w->timeentry.focus_list != NULL )
		XtRemoveCallbacks( (Widget)w, XmNfocusCallback,
			w->timeentry.focus_list );

	if ( w->timeentry.losing_focus_list != NULL )
		 XtRemoveCallbacks( (Widget)w, XmNlosingFocusCallback,
			w->timeentry.losing_focus_list );

	if ( w->timeentry.value_changed != NULL )
		XtRemoveCallbacks( (Widget)w, XmNvalueChangedCallback,
			w->timeentry.value_changed );
	
	if ( w->timeentry.activate != NULL )
		XtRemoveCallbacks( (Widget)w, XmNactivateCallback,
			w->timeentry.activate );

}


SetWidgetsValue(neww, resource, value)
XgTimeEntryWidget	neww;
String			resource;
XtArgVal		value;
{
	XtVaSetValues((Widget)neww, resource, value, NULL);
	XtVaSetValues(neww->timeentry.month, resource, value, NULL);
	XtVaSetValues(neww->timeentry.month_day_slash, resource, value, NULL);
	XtVaSetValues(neww->timeentry.day, resource, value, NULL);
	XtVaSetValues(neww->timeentry.day_year_slash, resource, value, NULL);
	XtVaSetValues(neww->timeentry.year, resource, value, NULL);

	XtVaSetValues(neww->timeentry.hours, resource, value, NULL);
	XtVaSetValues(neww->timeentry.hours_minutes_colon, 
		resource, value, NULL);
	XtVaSetValues(neww->timeentry.minutes, resource, value, NULL);
	XtVaSetValues(neww->timeentry.minutes_seconds_colon, 
		resource, value, NULL);
	XtVaSetValues(neww->timeentry.seconds, resource, value, NULL);

	XtVaSetValues(neww->timeentry.arrow_up, resource, value, NULL);
	XtVaSetValues(neww->timeentry.arrow_down, resource, value, NULL);
}



SetTextWidgetsValue(neww, resource, value)
XgTimeEntryWidget	neww;
String			resource;
XtArgVal		value;
{
	XtVaSetValues(neww->timeentry.month, resource, value, NULL);
	XtVaSetValues(neww->timeentry.day, resource, value, NULL);
	XtVaSetValues(neww->timeentry.year, resource, value, NULL);
	XtVaSetValues(neww->timeentry.hours, resource, value, NULL);
	XtVaSetValues(neww->timeentry.minutes, resource, value, NULL);
	XtVaSetValues(neww->timeentry.seconds, resource, value, NULL);
}



/* +++PHDR+++
 *
 * Function:	SetValues()
 *
 * Scope:	static	
 *
 * Description:
 *	SetValues() method for the XgTimeEntry widget. 
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
XgTimeEntryWidget	cur = (XgTimeEntryWidget)current,
		        req = (XgTimeEntryWidget)request,
		        neww = (XgTimeEntryWidget)new;
Boolean			redraw = False;

	/* 
	 * Check for changed margin height
	 */
	if ( cur->timeentry.margin_height != neww->timeentry.margin_height && 
		neww->timeentry.seconds != NULL )
	{
		SetTextWidgetsValue(neww, XmNmarginHeight,
			(XtArgVal)neww->timeentry.margin_height);

		redraw = True;
	}


	/* 
	 * Check for changed margin width
	 */
	if ( cur->timeentry.margin_width != neww->timeentry.margin_width && 
		neww->timeentry.seconds != NULL )
	{
		SetTextWidgetsValue(neww, XmNmarginWidth, 
			(XtArgVal)neww->timeentry.margin_width);

		redraw = True;
	}


	/* 
	 * Check for changed editable
	 */
	if ( cur->timeentry.editable != neww->timeentry.editable && 
		neww->timeentry.seconds != NULL )
	{
		SetTextWidgetsValue(neww, XmNeditable, 
			(XtArgVal)neww->timeentry.editable);
	}


	/* 
	 * Check for changed verifyBell 
	 */
	if ( cur->timeentry.audible != neww->timeentry.audible && 
		neww->timeentry.seconds != NULL )
	{
		SetTextWidgetsValue(neww,
			XmNverifyBell, (XtArgVal)neww->timeentry.audible);
	}


	/* 
	 * Check for changed font_list
	 */
	if ( cur->timeentry.font_list != neww->timeentry.font_list )
	{
		SetWidgetsValue(neww, XmNfontList,
			(XtArgVal)neww->timeentry.font_list);

		redraw = True;
	}

	/*
	 * Did the foreground color change?
	 */
	if ( neww->manager.foreground != cur->manager.foreground )
	{
		SetWidgetsValue(neww, XmNforeground,
			(XtArgVal)neww->manager.foreground);

		redraw = True;
	}



	/*
	 * Did the background color change?
	 */
	if ( neww->core.background_pixel != cur->core.background_pixel )
	{
		SetWidgetsValue(neww, XmNbackground, 
			(XtArgVal)neww->core.background_pixel);

		redraw = True;
	}


	/*
	 * Did one of the show flags change?
	 */
	if ( neww->timeentry.show_date != cur->timeentry.show_date )
	{
		SetManagedDateWidgets(neww);
		redraw = True;
	}

	
	if ( neww->timeentry.show_time != cur->timeentry.show_time ||
		 neww->timeentry.show_seconds != cur->timeentry.show_seconds )
	{
		SetManagedTimeWidgets(neww);
		redraw = True;
	}


	/*
	 * Did the show_seconds, spacing or orientation change ?
	 */
        if ( neww->timeentry.show_seconds != cur->timeentry.spacing ||
                neww->timeentry.orientation != cur->timeentry.orientation )
	{
		Resize(neww);
                redraw = True;
	}

        /*
         * Check for changed textBackground
         */
        if ( cur->timeentry.textBg!= neww->timeentry.textBg )
        {
	    SetTextWidgetsValue(neww, XmNbackground, neww->timeentry.textBg);
        }



	return redraw;
}



static void
SetTextFocus(w)
Widget	w;
{
XgTimeEntryWidget	te = GetTimeEntryWidget(w);

	if ( !XmIsText(w) )
		return;

	if ( te->timeentry.text_focus == w || te->timeentry.editable == False )
		return;

	if ( te->timeentry.text_focus != NULL )
		XtVaSetValues(te->timeentry.text_focus, 
			XmNcursorPositionVisible, False, NULL);

	te->timeentry.text_focus = w;

	XtVaSetValues(te->timeentry.text_focus, 
		XmNcursorPositionVisible, True, NULL);
}



static long
GetTimeValue(te)
XgTimeEntryWidget	te;
{
struct tm	ts;
long		time_value;
#ifdef __OpenBSD__
extern long tz;
#else
extern long timezone;
#endif

	ts.tm_mon = te->timeentry.month_value - 1;
	ts.tm_mday = te->timeentry.day_value;

	if ( te->timeentry.year_value < 70 )
		ts.tm_year = te->timeentry.year_value + 100;
	else
		ts.tm_year = te->timeentry.year_value;

	ts.tm_hour = te->timeentry.hours_value;
	ts.tm_min = te->timeentry.minutes_value;
	ts.tm_sec = te->timeentry.seconds_value;

	ts.tm_wday = 0;
	ts.tm_yday = 0;
	ts.tm_isdst = -1;

	time_value = (long)mktime(&ts);

	/*
	 * If we are not Rusing timezone then compensate for it
	 */
	if ( te->timeentry.use_timezone == False )
	{
#ifdef __OpenBSD__
		time_value -= tz;
#else
		time_value -= timezone;
#endif

		/*
		 * Adjust for Daylight Savings Time
		 */
		if ( ts.tm_isdst > 0 )
			time_value += 60 * 60;
	}
		

	if ( te->timeentry.minimum != te->timeentry.maximum )
	{
		if ( time_value < te->timeentry.minimum ||
			time_value > te->timeentry.maximum )
				return -1;
	}

	return time_value;
}


static void
GetMinMaxAndPointer(te, w, minimum, maximum, ptr)
XgTimeEntryWidget	te;
Widget			w;
int			*minimum;
int			*maximum;
int			**ptr;
{
	

	if ( w == te->timeentry.month )
	{
		*minimum = 1;
		*maximum = 12;
		*ptr = &te->timeentry.month_value;
	}
	else if ( w == te->timeentry.day )
	{
	int	year = te->timeentry.year_value;
		*minimum = 1;
		*maximum = days_in_month[te->timeentry.month_value];

		if ( te->timeentry.month_value == 2 && 
			(year == 0 || (year % 4) == 0) )
				*maximum++;
			
		*ptr = &te->timeentry.day_value;
	}
	else if ( w == te->timeentry.year )
	{
		*minimum = 0;
		*maximum = 99;
		*ptr = &te->timeentry.year_value;
	}
	else if ( w == te->timeentry.hours )
	{
		*minimum = 0;
		*maximum = 23;
		*ptr = &te->timeentry.hours_value;
	}
	else if ( w == te->timeentry.minutes )
	{
		*minimum = 0;
		*maximum = 59;
		*ptr = &te->timeentry.minutes_value;
	}
	else if ( w == te->timeentry.seconds )
	{
		*minimum = 0;
		*maximum = 59;
		*ptr = &te->timeentry.seconds_value;
	}
}


static void
ActivateCallback(w, client_data, cb)
Widget			w;
XtPointer		client_data;
XmAnyCallbackStruct	*cb;
{
XgTimeEntryWidget		te = GetTimeEntryWidget(w);
XgTimeEntryCallbackStruct	cbs;

	if ( te->timeentry.activate != NULL )
	{
		cbs.reason = cb->reason;
		cbs.event = cb->event;
		cbs.value = te->timeentry.value;

		XtCallCallbacks((Widget)te, XmNactivateCallback, &cbs);
	}
}



static void
ModifyVerifyCB(w, client_data, cb)
Widget		w;
XtPointer	client_data;
XmTextVerifyPtr	cb;
{
char			*ptr, *text, digit_char, buf[16];
int			textLen;
int			old_value, value, *value_ptr, minimum, maximum;
long			time_value;
XmTextBlock		textBlock;
XgTimeEntryWidget	te = GetTimeEntryWidget(w);
XgTimeEntryCallbackStruct	cbs;


	if ( te->timeentry.setting_value == True )
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
	if ( !isdigit(digit_char) )
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

	GetMinMaxAndPointer(te, w, &minimum, &maximum, &value_ptr);
	old_value = *value_ptr;

	buf[textLen] = digit_char;
	buf[textLen + 1] = '\0';

	value = (int)strtol(buf, &ptr, 10);

	if ( value == 0 && ptr == buf )
	{
		cb->doit = False;
		return;
	}

	if ( value < minimum || value > maximum )
	{
		cb->doit = False;
		return;
	}

	*value_ptr = value;

	time_value = GetTimeValue(te);

	if ( time_value == -1 )
	{
		*value_ptr = old_value;
		cb->doit = False;
		return;
	}

	if ( time_value < MIN_TIME_T || time_value > MAX_TIME_T )
	{
		*value_ptr = old_value;
		cb->doit = False;
		return;
	}


	te->timeentry.value = time_value;

	if ( te->timeentry.value_changed != NULL )
	{
		cbs.reason = XmCR_VALUE_CHANGED;
		cbs.event = cb->event;
		cbs.value = time_value;

		XtCallCallbacks((Widget)te, XmNvalueChangedCallback, &cbs);
	}
}


static void
arrow_timeout(te)
XgTimeEntryWidget        te;
{
int	old_value, minimum, maximum, *value_ptr;
long	time_value;


	GetMinMaxAndPointer(te, te->timeentry.text_focus,
		&minimum, &maximum, &value_ptr);


	old_value = *value_ptr;
	*value_ptr += te->timeentry.direction;

	time_value = GetTimeValue(te);

	/*
	 * Make sure we are within range
	 */
	if ( time_value < MIN_TIME_T || time_value > MAX_TIME_T )
	{
		*value_ptr = old_value;
		return;
	}


	
	if ( XgTimeEntrySetValue((Widget)te, time_value) == FALSE )
	{
		if ( te->timeentry.timeout != None )
			XtRemoveTimeOut(te->timeentry.timeout);

		*value_ptr = old_value;

		te->timeentry.timeout = None;
		return;
	}

	te->timeentry.timeout = XtAppAddTimeOut(
		XtWidgetToApplicationContext((Widget)te),
		50, (XtTimerCallbackProc)arrow_timeout, (XtPointer)te);
}

	
/* +++PHDR+++
 *
 * Function:	ArrowCB()
 *
 * Scope:	static
 *
 * Description:
 *	This function is called when one of the arrow buttons are pressed
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
ArrowCB(arrow, direction, cbd)
Widget 			arrow;
int			direction;
XmAnyCallbackStruct 	*cbd;
{
XgTimeEntryWidget       te = GetTimeEntryWidget(arrow); 
int			old_value, minimum, maximum, *value_ptr;
long			time_value;

	if ( cbd->reason == XmCR_DISARM )
	{
	XgTimeEntryCallbackStruct	cbs;

		if ( te->timeentry.timeout != None )
			XtRemoveTimeOut(te->timeentry.timeout);
			
		te->timeentry.timeout = None;

		if ( te->timeentry.value_changed != NULL )
		{
			cbs.reason = XmCR_VALUE_CHANGED;
			cbs.event = cbd->event;
			cbs.value = te->timeentry.value;

			XtCallCallbacks((Widget)te, XmNvalueChangedCallback, 
				(XmAnyCallbackStruct *)&cbs);
		}

		return;
	}


	if ( te->timeentry.workproc_id == None )
	{
		te->timeentry.workproc_id = XtAppAddWorkProc(
			XtWidgetToApplicationContext(arrow),
			(XtWorkProc)CheckFocus, (XtPointer)te);
	}

	te->timeentry.focus_widget = arrow;


	GetMinMaxAndPointer(te, te->timeentry.text_focus,
		&minimum, &maximum, &value_ptr);

	old_value = *value_ptr;
	*value_ptr += direction;
	
	time_value = GetTimeValue(te);

	/*
	 * Make sure we are within range
	 */
	if ( time_value < MIN_TIME_T || time_value > MAX_TIME_T )
	{
		if ( te->timeentry.timeout != None )
			XtRemoveTimeOut(te->timeentry.timeout);

		*value_ptr = old_value;
			
		te->timeentry.timeout = None;
		return;
	}


	if ( XgTimeEntrySetValue((Widget)te, time_value) == FALSE )
	{
		if ( te->timeentry.timeout != None )
			XtRemoveTimeOut(te->timeentry.timeout);

		*value_ptr = old_value;
			
		te->timeentry.timeout = None;
		return;
	}


	te->timeentry.direction = direction;
	te->timeentry.timeout = 
		XtAppAddTimeOut( XtWidgetToApplicationContext(arrow),
		500, (XtTimerCallbackProc)arrow_timeout, (XtPointer)te);
}

 
/* +++PHDR+++
 *
 * Function:	XgTimeEntryGetChild()
 *
 * Scope:	extern
 *
 * Description:
 *	This function returns a child widget of the passed XgTimeEntry widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgTimeEntry
 * child	unsigned char		Constant for child to return
 *
 * child constants:
 *
 *
 * Returns:	Widget id of child widget, NULL if error
 *
 *
 * ---PHDR--- */


Widget 
XgTimeEntryGetChild(w, child)
Widget w; 
int child;
{
XgTimeEntryWidget	te = (XgTimeEntryWidget)w;
	/*
	 * Make sure this is a TimeEntry widget
	 */
	if ( !XgIsTimeEntry(w) )
		return NULL;

	switch ( (int) child )
	{
	  case XgTIMEENTRY_MONTH:
		return te->timeentry.month;

	  case XgTIMEENTRY_DAY_SLASH:
		return te->timeentry.month_day_slash;

	  case XgTIMEENTRY_DAY:
		return te->timeentry.day;

	  case XgTIMEENTRY_YEAR_SLASH:
		return te->timeentry.day_year_slash;

	  case XgTIMEENTRY_YEAR:
		return te->timeentry.year;

	  case XgTIMEENTRY_HOURS:
		return te->timeentry.hours;

	  case XgTIMEENTRY_MINUTES_COLON:
		return te->timeentry.hours_minutes_colon;

	  case XgTIMEENTRY_MINUTES:
		return te->timeentry.minutes;

	  case XgTIMEENTRY_SECONDS_COLON:
		return te->timeentry.minutes_seconds_colon;

	  case XgTIMEENTRY_SECONDS:
		return te->timeentry.seconds;

	  case XgTIMEENTRY_UP_ARROW:
		return te->timeentry.arrow_up;

	  case XgTIMEENTRY_DOWN_ARROW:
		return te->timeentry.arrow_down;

	  default:
		return NULL;
	}


	return NULL;
}


/* +++PHDR+++
 *
 * Function:	XgTimeEntryGetValue()
 *
 * Scope:	extern
 *
 * Description:
 *	This function returns a the 
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgTimeEntry
 *
 *
 * Returns: 	long
 *
 *
 * ---PHDR--- */


long
XgTimeEntryGetValue(w)
Widget w; 
{
XgTimeEntryWidget	te = (XgTimeEntryWidget)w;


	/*
	 * Make sure this is a TimeEntry widget
	 */
	if ( !XgIsTimeEntry(w) )
		return 0;


	return te->timeentry.value;
}


/* +++PHDR+++
 *
 * Function:	XgTimeEntrySetValue()
 *
 * Scope:	extern
 *
 * Description:
 *	This function sets the date and time into the text portions of the 
 *	TimeEntry widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgTimeEntry
 * value	long			seconds since epoch
 *
 *
 * Returns: 	void
 *
 *
 * ---PHDR--- */

#ifndef _NO_PROTO
int XgTimeEntrySetValue(Widget w, long value)
#else
int
XgTimeEntrySetValue(w, value)
Widget 	w; 
long	value;
#endif
{
char			buf[8];
XgTimeEntryWidget	te = (XgTimeEntryWidget) w;
struct tm		*tmptr;

	/*
	 * Make sure this is a TimeEntry widget
	 */
	if ( !XgIsTimeEntry(w) )
		return FALSE;

	/*
	 * Put the current time in the widget
	 */
	if ( value == -1 )
	{
		value = (long)time(NULL);
		if ( te->timeentry.minimum != te->timeentry.maximum )
			if ( value < te->timeentry.minimum ||
				value > te->timeentry.maximum )
					value = te->timeentry.minimum;
	}
	else
	{
		if ( te->timeentry.minimum != te->timeentry.maximum )
			if ( value < te->timeentry.minimum ||
				value > te->timeentry.maximum )
					return FALSE;
	}


	/*
	 * Make sure it is within range
	 */
	if ( value > MAX_TIME_T ||  value < MIN_TIME_T )
		return FALSE;


	/*
	 * Fill out the tm struct with the passed time
	 */
	if ( te->timeentry.use_timezone == True )
		tmptr = localtime((time_t *)&value);
	else
		tmptr = gmtime((time_t *)&value);

	/*
	 * Now set the int values ofg each of the fields
	 */
	te->timeentry.month_value = tmptr->tm_mon + 1;
	te->timeentry.day_value = tmptr->tm_mday;

	if ( tmptr->tm_year > 99 )
		te->timeentry.year_value = tmptr->tm_year - 100;
	else
		te->timeentry.year_value = tmptr->tm_year;
	te->timeentry.hours_value = tmptr->tm_hour;
	te->timeentry.minutes_value = tmptr->tm_min;
	te->timeentry.seconds_value = tmptr->tm_sec;

	/*
	 * Save this time value
	 */
	te->timeentry.value = value;

	/*
	 * Do this so the ModifyVerifyCB simply returns
	 */
	te->timeentry.setting_value = True;

	/*
	 * And put the values in each of the text fields
	 */
	sprintf(buf, "%-2.2d", te->timeentry.month_value);
	XmTextSetString(te->timeentry.month, buf);

	sprintf(buf, "%-2.2d", te->timeentry.day_value);
	XmTextSetString(te->timeentry.day, buf);

	sprintf(buf, "%-2.2d", te->timeentry.year_value);
	XmTextSetString(te->timeentry.year, buf);

	sprintf(buf, "%-2.2d", te->timeentry.hours_value);
	XmTextSetString(te->timeentry.hours, buf);

	sprintf(buf, "%-2.2d", te->timeentry.minutes_value);
	XmTextSetString(te->timeentry.minutes, buf);

	sprintf(buf, "%-2.2d", te->timeentry.seconds_value);
	XmTextSetString(te->timeentry.seconds, buf);

	te->timeentry.setting_value = False;

	return TRUE;
}




static void TimeEntryActions(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
XgTimeEntryWidget	te = GetTimeEntryWidget(w);
int			old_value, *value_ptr, minimum, maximum, increment;
long			time_value;

        /*
         * See which action to take
         */
        switch ( *(params[0]) )
        {
          case 'u': /* up                */
        	increment = 1;
        	break;

          case 'd': /* down              */
        	increment = -1;
        	break;

          case 'p': /* page-up/page-down */

	        if ( *((char *)params[0] + 5) == 'u' )
	        	increment = 4;
        	else
        		increment = -4;
        	break;

      	  case 'm': /* mouse             */

	        /* mouse-down */
       		if ( *((char *)params[0] + 6) == 'd' )
        	{
	     		te->timeentry.y = ((XButtonEvent *)event)->y;
			return;
		}

		/* mouse-motion */
		increment = (te->timeentry.y - ((XButtonEvent *)event)->y) / 
			(HeightOfScreen(XtScreen(w)) / 512);

		te->timeentry.y = ((XButtonEvent *)event)->y;

        	break;

	  default:
	  	return;

        }
    
        GetMinMaxAndPointer(te, w, &minimum, &maximum, &value_ptr);

        old_value = *value_ptr;
	*value_ptr += increment;
	
	time_value = GetTimeValue(te);

	/*
	 * Make sure we are within range
	 */
	if ( time_value < MIN_TIME_T || time_value > MAX_TIME_T )
	{
		if ( te->timeentry.timeout != None )
			XtRemoveTimeOut(te->timeentry.timeout);

		*value_ptr = old_value;
	}


	if ( XgTimeEntrySetValue((Widget)te, time_value) == FALSE )
		*value_ptr = old_value;
} 


void XgTimeEntrySetFocus(w, widget_id)
Widget w;
int widget_id;
{
	Widget text;

	switch ( widget_id )
	{
	  case XgTIMEENTRY_MONTH:
		text = (Widget)MONTCHILD(w);
		break;

	  case XgTIMEENTRY_DAY:
		text = (Widget)DAYCHILD(w);
		break;

	  case XgTIMEENTRY_YEAR:
		text = (Widget)YEARCHILD(w);
		break;

	  case XgTIMEENTRY_HOURS:
		text = (Widget)HOURSCHILD(w);
		break;

	  case XgTIMEENTRY_MINUTES:
		text = (Widget)MINUTESCHILD(w);
		break;

	  case XgTIMEENTRY_SECONDS:
		text = (Widget)SECONDSCHILD(w);
		break;

	  default:
		return;
	}

	/*
	 * Traverse to the specified widget
	 */
	XmProcessTraversal(text, XmTRAVERSE_CURRENT);
	SetTextFocus(text);
}
