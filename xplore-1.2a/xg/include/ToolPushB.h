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


#ifndef _ToolPushButton_h
#define _ToolPushButton_h



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/*
 * New resources for the Tool Pushbutton widget
 */
#ifndef XgNdragCursor
#  define XgNdragCursor		"dragCursor"
#endif

#ifndef XgNtoolPixmap
#  define XgNtoolPixmap		"toolPixmap"
#endif

#ifndef XgNstartDragProc
#  define XgNstartDragProc	"startDragProc"
#endif
#ifndef XgCStartDragProc
#  define XgCStartDragProc	"StartDragProc"
#endif



/*
 * Is ToolPushButton macro
 */
#define XgIsToolPushButton(w) XtIsSubclass(w, xgToolPushButtonWidgetClass)


/*
 * Convienience creation function
 */
#ifndef _NO_PROTO
  Widget XgCreateToolPushButtonWidget(Widget, char *, ArgList, Cardinal);
#else
  Widget XgCreateToolPushButtonWidget();
#endif



extern WidgetClass 			 xgToolPushButtonWidgetClass;
typedef struct _XgToolPushButtonClassRec *XgToolPushButtonWidgetClass;
typedef struct _XgToolPushButtonRec      *XgToolPushButtonWidget;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _XgToolPushButton_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
