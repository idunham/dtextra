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



#ifndef _TOOLBARP_H_
#define _TOOLBARP_H_

#if XmVersion > 1001
#  include    <Xm/ManagerP.h>
#else
#endif

#include    "ToolBar.h"



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



typedef struct {
	String tool_hint;
	Widget tool;
} ToolBarHintData_t;

typedef struct {
	Widget		widget;
	String		name;
	String		tool_hint;
	Widget		(*clone_proc)();
	XtPointer       client_data;
} RegisteredWidget_t;


typedef struct _XgToolBarClassPart {
          Widget	     drop_widget;
	  XgToolBarWidget    *toolbar_list;
	  int		     num_toolbars;
	  int		     drop_timeout;
	  Cursor	     drag_cursor;
	  XtIntervalId	     drop_timer_id;
	  int		     num_registered;
	  RegisteredWidget_t *registered_widgets;
} XgToolBarClassPart;


typedef struct _XgToolBarClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart  manager_class;
    XgToolBarClassPart  toolbar_class;
} XgToolBarClassRec;


extern XgToolBarClassRec XgtoolBarClassRec;

typedef struct {
    unsigned char     	orientation;
    Boolean     	hstretch,
                	vstretch;
    Dimension		spacing;
    Dimension		margin_height,
			margin_width;

    String		*clone_list;

    int 		hint_delay;
    Pixel		hint_bg;
    Pixel		hint_fg;
    Boolean		hints_enabled;

    XmFontList		font_list;

    Boolean		editable;


    /*
     * private resources
     */
    XgToolBarCloneDataStruct   *clones;    
    int 	     num_clones;

    Widget 	     hint_window;
    XtIntervalId     hint_timer_id;


} XgToolBarPart;


typedef struct _XgToolBarRec {
    CorePart        core;
    CompositePart   composite;
    ConstraintPart  constraint;
    XmManagerPart   manager;
    XgToolBarPart   toolbar;
}  XgToolBarRec;


/*
 * Private routines
 */
#ifndef _NO_PROTO
  int _XgToolBarStartDrop(Widget w, XEvent *event);
#else
  int _XgToolBarStartDrop();
#endif



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _TOOLBARP_H_ */
