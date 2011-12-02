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
 *	Filename: ClockLabelP.h
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	messner	01/93   	Original file
 *
 *
 * ---FHDR--- */


#ifndef _XgClockLabelP_h
#define _XgClockLabelP_h

/*
 * Super Class include
 */
#include "FastLabelP.h"

#include "ClockLabel.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/*
 * New fields for the ClockLabel widget class record
 */
typedef struct _XgClockLabelClassPart {
     int empty;   /* No new procedures */
} XgClockLabelClassPart;


/*
 * Class record declaration
 */
typedef struct _XgClockLabelClassRec {
	CoreClassPart		core_class;
	XmPrimitiveClassPart    prim_class;
	XgFastLabelClassPart	fastlabel_class;
    	XgClockLabelClassPart	clocklabel_class;
} XgClockLabelClassRec;

extern XgClockLabelClassRec xgClockLabelClassRec;


/*
 * Fast Label instance structure.
 */
typedef struct _XgClockLabelPart {
	/*
	 * Public resources
	 */
	String		format_string;	/* */

	long		(*time_proc)();	/* */

	int		update_rate;	/* */

	Boolean		use_timezone;	/* */

	Widget		update_widget;



	/*
	 * Private resources
	 */
	String		my_fmt_string;

	XtIntervalId	timer_id;

	long		old_time;


} XgClockLabelPart;



/*
 * Full instance record declaration
 */
typedef struct _XgClockLabelRec {
	CorePart		core;
	XmPrimitivePart		primitive;
	XgFastLabelPart		fastlabel;
    	XgClockLabelPart	clocklabel;
} XgClockLabelRec;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _XgClockLabelP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
