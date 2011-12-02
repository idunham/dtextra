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
 *	Filename: FastLabelP.h
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *		This is the public header file for the fast label widget.
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	messner	01/93	Original file
 *
 *
 * ---FHDR--- */


#ifndef _XgFastLabel_h
#define _XgFastLabel_h



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



extern WidgetClass			xgFastLabelWidgetClass;
typedef struct _XgFastLabelClassRec	*XgFastLabelWidgetClass;
typedef struct _XgFastLabelRec		*XgFastLabelWidget;


#ifndef XgNrotation
#  define XgNrotation	"rotation"
#endif

#ifndef XgCRotation
#  define XgCRotation	"Rotation"
#endif


/*
 * Is FastLabel macro
 */
#define XgIsFastLabel(w)	(XtIsSubclass (w, xgFastLabelWidgetClass))


/*
 * Convienience creation function
 */
#ifndef _NO_PROTO
  Widget XgCreateFastLabel(Widget parent, char *name, ArgList al, Cardinal ac);
#else
  Widget XgCreateFastLabel();
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _XgFastLabel_h */
