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
 *	Filename: ComboBoxP.h
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
 *	messner 09/30/93	Original file
 *
 *
 * ---FHDR--- */


#ifndef _XgComboBoxP_h
#define _XgComboBoxP_h


/*
 * Super Class include
 */
#if (XmVERSION == 1 && XmREVISION > 1) || XmVERSION > 1
#include	<Xm/ManagerP.h>
#endif

#include "ComboBox.h"



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/*
 * New fields for the ComboBox widget class record
 */
typedef struct _XgComboBoxClassPart {
     int empty;   /* No new procedures */
} XgComboBoxClassPart;


/*
 * Class record declaration
 */
typedef struct _XgComboBoxClassRec {
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	XmManagerClassPart	manager_class;
    	XgComboBoxClassPart	combobox_class;
} XgComboBoxClassRec;

extern XgComboBoxClassRec xgComboBoxClassRec;


/*
 * Combo Box instance structure.
 */
typedef struct _XgComboBoxPart {
	/*
	 * Public resources
	 */
	short textColumns;		/* */
	int textMaxLength;		/* */
	Boolean editable;		/* */
	Boolean audible;		/* */
	
	int visibleItems;		/* */
	int listCount;			/* */
	XmStringTable listTable;	/* */	

	XmFontList font_list;		/* */

	Dimension margin_width;		/* */
	Dimension margin_height;	/* */

	Boolean autoFillIn;		/* */
	Boolean expandListWidth;	/* */

	XtCallbackList focus_list;
	XtCallbackList losing_focus_list;
	XtCallbackList value_changed;
	XtCallbackList activate;

	Pixel textFg, listFg, textBg, listBg;

	/*
	 * Private widget resources
	 */
	int dont_reenter;		/* */
	char *saved_text;		/* */
	Boolean initializing;		/* */

	Dimension minWidth;		/* */
	Dimension minHeight;		/* */	

	Widget text;			/* Widget ID of Text 		*/
	Widget list;			/* Widget ID of List 		*/
	Widget popup;			/* Widget ID of the List Popup	*/
	Widget arrow;			/* Widget ID of the Arrow 	*/

	XtWorkProcId workproc_id;
	Widget focus_widget;
	Boolean i_have_focus;

	int saved_list_index;
} XgComboBoxPart;



/*
 * Full instance record declaration
 */
typedef struct _XgComboBoxRec {
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	XmManagerPart		manager;
    	XgComboBoxPart		combobox;
} XgComboBoxRec;


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* _XgComboBoxP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
