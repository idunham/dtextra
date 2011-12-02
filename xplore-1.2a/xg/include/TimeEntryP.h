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
 *	Filename: TimeEntryP.h
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.4 26 Feb 1997
 *
 *	Description:
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	messner	07/30/94	Original file
 *
 *
 * ---FHDR--- */


#ifndef _XgTimeEntryP_h
#define _XgTimeEntryP_h


/*
 * Super Class include
 */
#if (XmVERSION == 1 && XmREVISION > 1) || XmVERSION > 1
#  include	<Xm/ManagerP.h>
#endif

#include	"TimeEntry.h"



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/*
 * New fields for the TimeEntry widget class record
 */
typedef struct _XgTimeEntryClassPart {
     int empty;   /* No new procedures */
} XgTimeEntryClassPart;


/*
 * Class record declaration
 */
typedef struct _XgTimeEntryClassRec {
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart     constraint_class;
	XmManagerClassPart	manager_class;
    	XgTimeEntryClassPart	timeentry_class;
} XgTimeEntryClassRec;

extern XgTimeEntryClassRec xgTimeEntryClassRec;


/*
 * Combo Box instance structure.
 */
typedef struct _XgTimeEntryPart {
	/*
	 * Public resources
	 */
	Boolean editable;		/* */
	Boolean audible;		/* */

	Boolean show_time;
	Boolean show_date;
	Boolean show_seconds;

	int date_location;
	unsigned char orientation;
	
	XmFontList font_list;		/* */
	Pixel textBg;
	Pixel textFg;

	Dimension spacing;
	Dimension margin_height;
	Dimension margin_width;

	long minimum;
	long maximum;
	long value;

	Boolean use_timezone;

	XtCallbackList focus_list;
	XtCallbackList losing_focus_list;

	XtCallbackList value_changed;
	XtCallbackList activate;



	/*
	 * Private widget resources
	 */
	Boolean initializing;		/* */
	Dimension minWidth;		/* */
	Dimension minHeight;		/* */	

	Position y;

	Widget month;
	Widget month_day_slash;
	Widget day;
	Widget day_year_slash;
	Widget year;

	int month_value;
	int day_value;
	int year_value;

	long timezone_offset;

	Widget hours;
	Widget hours_minutes_colon;
	Widget minutes;
	Widget minutes_seconds_colon;
	Widget seconds;

	Widget arrow_up;
	Widget arrow_down;

	int hours_value;
	int minutes_value;
	int seconds_value;

	XtIntervalId timeout;
	int direction;
	int which_section;
	Boolean setting_value;

	Boolean resized;

	XtWorkProcId workproc_id;
	Widget focus_widget;
	Widget text_focus;
} XgTimeEntryPart;



/*
 * Full instance record declaration
 */
typedef struct _XgTimeEntryRec {
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	XmManagerPart		manager;
    	XgTimeEntryPart		timeentry;
} XgTimeEntryRec;


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* _XgTimeEntryP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
