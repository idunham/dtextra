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
 *		This is the private header file for the fast label widget.
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


#ifndef _XgFastLabelP_h
#define _XgFastLabelP_h

/*
 * Super Class include
 */
#if XmVersion > 1001
#include <Xm/PrimitiveP.h>
#endif

#include "FastLabel.h"



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/*
 * New fields for the FastLabel widget class record
 */
typedef struct _XgFastLabelClassPart {
     int empty;
} XgFastLabelClassPart;


/*
 * Class record declaration
 */
typedef struct _XgFastLabelClassRec {
	CoreClassPart		core_class;
	XmPrimitiveClassPart    prim_class;
    	XgFastLabelClassPart	fastlabel_class;
} XgFastLabelClassRec;

extern XgFastLabelClassRec xgFastLabelClassRec;


/*
 * Fast Label instance structure.
 */
typedef struct _XgFastLabelPart {
	/*
	 * Public resources
	 */
	unsigned char	alignment;	/* */
	int		rotation;	/* */

	XmFontList	font_list;
	String		label_string;	/* */

	Boolean		recompute_size;	/* */

	Dimension	margin_width;	/* */
	Dimension	margin_height;	/* */


	/*
	 * Private resources
	 */
	GC		text_gc;
	XFontStruct	*font;
	String		my_string;

	Pixmap		pix;
	Dimension	pix_width;
	Dimension	pix_height;

	Dimension	desired_pix_width;
	Dimension	desired_pix_height;

} XgFastLabelPart;



/*
 * Full instance record declaration
 */
typedef struct _XgFastLabelRec {
	CorePart	core;
	XmPrimitivePart	primitive;
    	XgFastLabelPart	fastlabel;
} XgFastLabelRec;



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* _XgFastLabelP_h */
/* DON'T ADD STUFF AFTER THIS #endif */

