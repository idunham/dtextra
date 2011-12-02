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
 *	Filename: NumEntryP.h
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
 *	messner 08/30/93	Original file
 *
 *
 * ---FHDR--- */


#ifndef _XgNumEntryP_h
#define _XgNumEntryP_h


/*
 * Super Class include
 */
#if (XmVERSION == 1 && XmREVISION > 1) || XmVERSION > 1
#include	<Xm/ManagerP.h>
#endif
#include        <X11/CompositeP.h>

#include	"NumEntry.h"



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/*
 * New fields for the NumEntry widget class record
 */
typedef struct _XgNumEntryClassPart {
     int empty;   /* No new procedures */
} XgNumEntryClassPart;


/*
 * Class record declaration
 */
typedef struct _XgNumEntryClassRec {
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart     constraint_class;
	XmManagerClassPart	manager_class;
    	XgNumEntryClassPart	numentry_class;
} XgNumEntryClassRec;

extern XgNumEntryClassRec xgNumEntryClassRec;


/*
 * Combo Box instance structure.
 */
typedef struct _XgNumEntryPart {
	/*
	 * Public resources
	 */
	short textColumns;		/* */
	int textMaxLength;		/* */
	Boolean editable;		/* */
	Boolean audible;		/* */
	
	XmFontList font_list;		/* */
	Pixel textBg;

	Dimension margin_height;
	Dimension margin_width;

	float minimum;
	float maximum;
	float value;
	float increment;

	short precision;

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

	Widget text;			/* Widget ID of Text 		*/
	Widget up_arrow;		/* Widget ID of the Up Arrow	*/
	Widget down_arrow;		/* Widget ID of the Down Arrow 	*/

	XtIntervalId timeout;
	XtWorkProcId workproc_id;
	int direction;
	Position y;
	Boolean setting_value;

	Widget focus_widget;
} XgNumEntryPart;



/*
 * Full instance record declaration
 */
typedef struct _XgNumEntryRec {
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	XmManagerPart		manager;
    	XgNumEntryPart		numentry;
} XgNumEntryRec;



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _XgNumEntryP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
