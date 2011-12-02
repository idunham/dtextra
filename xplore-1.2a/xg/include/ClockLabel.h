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
 *	Filename: ClockLabel.h
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
 *	GMM	11/09/93	Original file
 *
 *
 * ---FHDR--- */

#ifndef _XgClockLabel_h
#define _XgClockLabel_h

#include "FastLabel.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/*
 * New resources for the ClockLabel widget
 */
#ifndef XgNformatString
#  define XgNformatString	"formatString"
#endif
#ifndef XgCFormatString
#  define XgCFormatString 	"FormatString"
#endif

#ifndef XgNtimeProc
#  define XgNtimeProc		"timeProc"
#endif
#ifndef XgCTimeProc
#  define XgCTimeProc		"TimeProc"
#endif

#ifndef XgNupdateRate
#  define XgNupdateRate		"updateRate"
#endif
#ifndef XgCUpdateRate
#  define XgCUpdateRate		"Updaterate"
#endif

#ifndef XgNuseTimeZone
#  define XgNuseTimeZone	"useTimeZone"
#endif
#ifndef XgCUseTimeZone
#  define XgCUseTimeZone	"UseTimeZone"
#endif

#ifndef XgNupdateWidget
#  define XgNupdateWidget	"updateWidget"
#endif


extern WidgetClass			xgClockLabelWidgetClass;
typedef struct _XgClockLabelClassRec	*XgClockLabelWidgetClass;
typedef struct _XgClockLabelRec		*XgClockLabelWidget;



/*
 *
 */
#define XgIsClockLabel(w)	(XtIsSubclass (w, xgClockLabelWidgetClass))


/*
 * Convienience creation function
 */
#ifndef _NO_PROTO
  Widget XgCreateClockLabel(Widget, char *, ArgList, Cardinal);
#else
   Widget XgCreateClockLabel();
#endif


#ifndef _NO_PROTO
	extern long XgClockLabelGetTime(Widget w);
	extern void XgClockLabelForceUpdate(Widget w);
#else
	extern long XgClockLabelGetTime();
	extern void XgClockLabelForceUpdate();
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* _XgClockLabel_h */
