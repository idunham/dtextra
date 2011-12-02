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
 *	Filename: ComboBox.h
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

#ifndef _XgComboBox_h
#define _XgComboBox_h


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/*
 * New resources for the ComboBox
 */
#ifndef XgNautoFillIn
#  define XgNautoFillIn		"autoFillIn"
#endif
#ifndef XgCAautoFillIn
#  define XgCAautoFillIn	"AutoFillIn"	
#endif

#ifndef XgNexpandListWidth
#  define XgNexpandListWidth	"expandListWidth"
#endif
#ifndef XgCExpandListWidth
#  define XgCExpandListWidth	"ExpandListWidth"
#endif

#ifndef XgNlistStrings
#  define XgNlistStrings	"listStrings"
#endif
#ifndef XgCListStrings
#  define XgCListStrings	"ListStrings"
#endif

#ifndef XgNlistForeground
#  define XgNlistForeground	"listForeground"
#endif

#ifndef XgNtextForeground
#  define XgNtextForeground	"textForeground"
#endif

#ifndef XgNlistBackground
#  define XgNlistBackground	"listBackground"
#endif

#ifndef XgNtextBackground
#  define XgNtextBackground	"textBackground"
#endif


extern WidgetClass			xgComboBoxWidgetClass;
typedef struct _XgComboBoxClassRec	*XgComboBoxWidgetClass;
typedef struct _XgComboBoxRec		*XgComboBoxWidget;


/*
 * Is ComboBox macro
 */
#define XgIsComboBox(w)	(XtIsSubclass (w, xgComboBoxWidgetClass))


/*
 * Convienience creation function
 */
#ifndef _NO_PROTO
  Widget XgCreateComboBox(Widget, char *, ArgList, Cardinal);
#else
  Widget XgCreateComboBox();
#endif

/*
 * Declare the XgComboBoxGet/SetString functions
 */
#ifndef _NO_PROTO
  extern char *XgComboBoxGetString(Widget w);
  extern void XgComboBoxSetString(Widget w, char *string, Boolean notify);
  extern void XgComboBoxSetItems(Widget w, XmString *list, 
  	int list_count, int selected_pos, Boolean notify);
  extern void XgComboBoxSetList(Widget w, char **list, int list_count, 
  	int selected_pos, Boolean notify);
  extern void XgComboBoxSelectPos(Widget w, int selected_pos, Boolean notify);
  extern int XgComboBoxGetSelectedPos(Widget w, char ** szValue);
  extern void XgComboBoxAddItem(Widget w , String str, 
  	int pos, Boolean select_it);
  extern void XgComboBoxChangeItem(Widget w , String str, int pos);
  extern void XgComboBoxDeletePos(Widget w, int pos);
#else
  extern char *XgComboBoxGetString();
  extern void XgComboBoxSetString();
  extern void XgComboBoxSetItems();
  extern void XgComboBoxSetList();
  extern void XgComboBoxSelectPos();
  extern int XgComboBoxGetSelectedPos();
  extern void XgComboBoxAddItem();
  extern void XgComboBoxChangeItem();
  extern void XgComboBoxDeletePos();
#endif


/*
 * Declare the children get function and constants
 */
#ifndef _NO_PROTO
  extern Widget XgComboBoxGetChild(Widget w, int child);
#else
  extern Widget XgComboBoxGetChild();
#endif

#define XgCOMBOBOX_TEXT		XmDIALOG_TEXT
#define XgCOMBOBOX_LIST		XmDIALOG_LIST
#define XgCOMBOBOX_ARROW	XmDIALOG_APPLY_BUTTON



typedef struct {
	int reason;
	XEvent *event;
	char *value;
	int list_pos;
	char *old_value;
} XgComboBoxCallbackStruct;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _XgComboBox_h */


