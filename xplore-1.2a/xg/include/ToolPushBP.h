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




#ifndef _XgToolPushButtonP_h
#define _XgToolPushButtonP_h

#include "ToolPushB.h"
#include <Xm/PushBP.h>



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/* 
 * ToolPushButton class structure 
 */
typedef struct _XgToolPushButtonClassPart
{
   int foo;
} XgToolPushButtonClassPart;


/*
 * Full class record declaration for ExtPushButton class
 */
typedef struct _XgToolPushButtonClassRec {
    CoreClassPart	  	core_class;
    XmPrimitiveClassPart	primitive_class;
    XmLabelClassPart      	label_class;
    XmPushButtonClassPart 	pushbutton_class;
    XgToolPushButtonClassPart	toolbutton_class;
} XgToolPushButtonClassRec;


externalref XgToolPushButtonClassRec xgToolPushButtonClassRec;

/*
 * ToolPushButton instance record
 */
typedef struct _XgToolPushButtonPart
{
	/*
	 * Public resources
	 */
	Cursor	drag_cursor;
	Pixmap  tool_pixmap;

	void 	(*start_drag_proc)();
} XgToolPushButtonPart;


/*
 * Full instance record declaration
 */
typedef struct _XgToolPushButtonRec {
    CorePart	     		core;
    XmPrimitivePart  		primitive;
    XmLabelPart      		label;
    XmPushButtonPart 		pushbutton;
    XgToolPushButtonPart	toolbtn;
} XgToolPushButtonRec;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _XgToolPushButtonP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
