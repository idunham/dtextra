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



#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_




#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/*
 * New resources
 */
#ifndef XgNhorizontalStretch
#  define XgNhorizontalStretch   "horizontalStretch"
#endif
#ifndef XgNverticalStretch
#  define XgNverticalStretch     "verticalStretch"
#endif

/* Class Resource specifications */
#ifndef XgCStretch
#  define XgCStretch "Stretch"
#endif

#ifndef XgNdropTimeout
#  define XgNdropTimeout 	"dropTimeout"
#endif
#ifndef XgCDropTimeout
#  define XgCDropTimeout	"DropTimeout"
#endif

#ifndef XgNdragCursor
#  define XgNdragCursor		"dragCursor"
#endif

#ifndef XgNtoolPixmap
#  define XgNtoolPixmap		"toolPixmap"
#endif

#ifndef XgNcloneList
#  define XgNcloneList 		"cloneList"
#endif
#ifndef XgCCloneList
#  define XgCCloneList		"CloneList"
#endif

#ifndef XgNhintDelay
#  define XgNhintDelay		"hintDelay"
#endif
#ifndef XgCHintDelay
#  define XgCHintDelay		"HintDelay"
#endif

#ifndef XgNhintBackground
#  define XgNhintBackground	"hintBackground"
#endif
#ifndef XgCHintBackground
#  define XgCHintBackground	"HintBackground"
#endif

#ifndef XgNhintForeground
#  define XgNhintForeground	"hintForeground"
#endif
#ifndef XgCHintForeground
#  define XgCHintForeground	"HintForeground"
#endif

#ifndef XgNhintsEnabled
#  define XgNhintsEnabled	"hintsEnabled"
#endif
#ifndef XgCHintsEnabled
#  define XgCHintsEnabled	"HintsEnabled"
#endif


typedef void (*XgCloneProc)(
#ifndef _NO_PROTO
    Widget 	/* cousin parent */,
    Widget	/* cousin        */,
    XtPointer	/* client data   */
#endif
);


extern WidgetClass 			xgToolBarWidgetClass;
typedef struct _XgToolBarClassRec 	*XgToolBarWidgetClass;
typedef struct _XgToolBarRec      	*XgToolBarWidget;



/*
 * Is ToolBar macro
 */
#define XgIsToolBar(w) (XtIsSubclass (w, xgToolBarWidgetClass))


/*
 * Convienience creation function
 */
#ifndef _NO_PROTO
  Widget XgCreateToolBar(Widget, char *, ArgList, Cardinal);
#else
  Widget XgCreateToolBar();
#endif


/*
 * Data kept for each widget on the toolbar
 */
typedef struct {
	Widget self;
	Widget cloned_from;
	String tool_hint;
} XgToolBarCloneDataStruct;




/*
 * Public routines
 */
#ifndef _NO_PROTO

  Boolean XgToolBarWidgetIsRegistered(Widget widget);

  void XgToolBarRegisterWidget(Widget widget, 
	Widget (*cloner)(Widget, Widget, void *), XtPointer client_data);
  void XgToolBarUnregisterWidget(Widget widget);

  void XgToolBarSetToolHint(Widget widget, String hint);

  Widget XgToolBarDefaultCloner(Widget parent, Widget widget_to_clone,
	XtPointer client_data);
  Widget XgToolBarSpacerCloner(Widget parent, Widget widget_to_clone,
	XtPointer client_data);

  void XgToolBarGetClones(Widget toolbar, String **names, int *count);
  void XgToolBarSetClones(Widget toolbar, String *names, int count);  

  Widget XgToolBarNameToWidget(String name);

  Boolean XgToolBarGetCloneData(Widget toolbar, 
	XgToolBarCloneDataStruct ** data, int *num_clones);

#else

  Boolean XgToolBarWidgetIsRegistered();

  void XgToolBarRegisterWidget();
  void XgToolBarUnregisterWidget();

  void XgToolBarSetToolHint();

  Widget XgToolBarDefaultCloner();
  Widget XgToolBarSpacerCloner();

  void XgToolBarGetClones();
  void XgToolBarSetClones();

  Widget XgToolBarNameToWidget();

  Boolean XgToolBarGetCloneData();

#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* _TOOLBAR_H_ */
