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



#ifndef _XgExtPushButtonP_h
#define _XgExtPushButtonP_h

#include "ExtPushB.h"
#include <Xm/PushBP.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* 
 * ExtPushButton class structure 
 */
typedef struct _XgExtPushButtonClassPart
{
   Pixmap stipple_bitmap;
} XgExtPushButtonClassPart;


/*
 * Full class record declaration for ExtPushButton class
 */
typedef struct _XgExtPushButtonClassRec {
    CoreClassPart	  	core_class;
    XmPrimitiveClassPart	primitive_class;
    XmLabelClassPart      	label_class;
    XmPushButtonClassPart 	pushbutton_class;
    XgExtPushButtonClassPart	extpushbutton_class;
} XgExtPushButtonClassRec;


externalref XgExtPushButtonClassRec xgExtPushButtonClassRec;

/*
 * ExtPushButton instance record
 */
typedef struct _XgExtPushButtonPart
{
	/*
	 * Public resources
	 */
	Boolean	shift;
	int	  label_type;
	String	  label_pixmap;
	String	  label_string;
	Dimension spacing;
	int	  label_location;

	/*
	 * Private resources
	 */
        Pixmap  pix;
        Pixmap  arm_pix;
        Pixmap  insen_pix;

	int	doing_setvalues;
	int	doing_destroy;
} XgExtPushButtonPart;


/*
 * Full instance record declaration
 */
typedef struct _XgExtPushButtonRec {
    CorePart	     	core;
    XmPrimitivePart  	primitive;
    XmLabelPart      	label;
    XmPushButtonPart 	pushbutton;
    XgExtPushButtonPart	extpushbutton;
} XgExtPushButtonRec;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _XgExtPushButtonP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
