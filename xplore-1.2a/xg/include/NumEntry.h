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
 *	Filename: NumEntry.h
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
 *	messner	08/30/93	Original file
 *
 *
 * ---FHDR--- */

#ifndef _XgNumEntry_h
#define _XgNumEntry_h



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



#ifndef XgNtextBackground
#  define XgNtextBackground "textBackground"
#endif


extern WidgetClass			xgNumEntryWidgetClass;
typedef struct _XgNumEntryClassRec	*XgNumEntryWidgetClass;
typedef struct _XgNumEntryRec		*XgNumEntryWidget;


/*
 * Is NumEntry macro
 */
#define XgIsNumEntry(w)	(XtIsSubclass (w, xgNumEntryWidgetClass))


/*
 * Convienience creation function
 */
#ifndef _NO_PROTO
  Widget XgCreateNumEntry(Widget, char *, ArgList, Cardinal);
#else
  Widget XgCreateNumEntry();
#endif


/*
 * Declare the XgNumEntryGet/SetString functions
 */
#ifndef _NO_PROTO
  extern float XgNumEntryGetValue(Widget w);
  extern int XgNumEntrySetValue(Widget w, double value);
  XtArgVal XgNumEntryFloatToXtArgVal(double value);
#else
  extern float XgNumEntryGetValue();
  extern int XgNumEntrySetValue();
  XtArgVal XgNumEntryFloatToXtArgVal();
#endif


/*
 * Declare the children get function and constants
 */
#ifndef _NO_PROTO
  extern Widget XgNumEntryGetChild(Widget w, int child);
#else
  extern Widget XgNumEntryGetChild();
#endif

#define XgNUMENTRY_TEXT		0
#define XgNUMENTRY_UP_ARROW	1
#define XgNUMENTRY_DOWN_ARROW	2


typedef struct {
	int reason;
	XEvent *event;
	double value;
} XgNumEntryCallbackStruct;



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _XgNumEntry_h */
