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
 *	Filename: Tabs.h
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *		This is the public header file for the Tabs widget.
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	messner	06/14/95	Original file
 *
 *
 * ---FHDR--- */



#ifndef _XgTabs_H_
#define _XgTabs_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/*
 * New resources for Tabs Widget
 *
 * Re-assigned Xm resources
 */
#ifndef XgNactivateCallback
#  define XgNactivateCallback XmNactivateCallback 
#endif

#ifndef XgNautoUnmanage
#  define XgNautoUnmanage  XmNautoUnmanage
#endif

#ifndef XgNfontList
#  define XgNfontList XmNfontList
#endif

#ifndef XgNhighlightThickness
#  define XgNhighlightThickness  XmNhighlightThickness
#endif

#ifndef XgNnavigationType
#  define XgNnavigationType  XmNnavigationType
#endif


/*
 * Pure Xg resources
 */
#ifndef XgNtabCount
#  define XgNtabCount 	  "tabCount"
#endif
#ifndef XgCTabCount 
#  define XgCTabCount 	  "TabCount"
#endif

#ifndef XgNcurrentTab
#  define XgNcurrentTab   "currentTab"
#endif
#ifndef XgCCurrentTab
#  define XgCCurrentTab   "CurrentTab"
#endif

#ifndef XgNtabShape
#  define XgNtabShape 	  "tabShape"
#endif
#ifndef XgCShape
#  define XgCShape 	  "Shape"
#endif

#ifndef XgNtabLocation
#  define XgNtabLocation  "tabLocation"
#endif
#ifndef XgCLocation
#  define XgCLocation	  "Location"
#endif

#ifndef XgNtabLabels
#  define XgNtabLabels 	  "tabLabels"
#endif
#ifndef XgCTabLabels
#  define XgCTabLabels 	  "TabLabels"
#endif

#ifndef XgNtabWidgets
#  define XgNtabWidgets   "tabWidgets"
#endif
#ifndef XgCTabWidgets
#  define XgCTabWidgets   "TabWidgets"
#endif

#ifndef XgNtabColor
#  define XgNtabColor 	  "tabColor"
#endif

#ifndef XgNselectedTabColor
#  define XgNselectedTabColor "selectedTabColor"
#endif

#ifndef XgNtabMargin
#  define XgNtabMargin 	  "tabMargin"
#endif

#ifndef XgNlabelMargin
#  define XgNlabelMargin  "labelMargin"
#endif

#ifndef XgNactivateCallback
#  define XgNactivateCallback XmNactivateCallback
#endif

#ifndef XgNfocusTab
#  define XgNfocusTab 	  "focusTab"
#endif
#ifndef XgCFocusTab
#  define XgCFocusTab	  "FocusTab"
#endif



/*
 * Types for the Tab Shape
 */
typedef	int			XgTabShapeType;

#ifndef XgRTabShapeType
#  define XgRTabShapeType	"TabShapeType"
#endif 

#ifndef XgSLANTED
#  define XgSLANTED		1
#endif

#ifndef XgCHAMFERRED
#  define XgCHAMFERRED		0
#endif

/*
 * Types for the Tab Location
 */
typedef	int			XgTabLocationType;

#ifndef XgRTabLocationType
#  define XgRTabLocationType	"TabLocationType"
#endif 

#ifndef XgTOP
#  define XgTOP		0
#endif
#ifndef XgBOTTOM
#  define XgBOTTOM	1
#endif
#ifndef XgLEFT
#  define XgLEFT	2
#endif
#ifndef XgRIGHT
#  define XgRIGHT	3
#endif





extern WidgetClass 		xgTabsWidgetClass;
typedef struct _XgTabsClassRec	*XgTabsWidgetClass;
typedef struct _XgTabsRec 	*XgTabsWidget;


/*
 * Convienience creation function
 */
#ifndef _NO_PROTO
  Widget XgCreateTabsWidget(Widget, char *, ArgList, Cardinal);
#else
  Widget XgCreateTabsWidget();
#endif


/*
 * Activate Callback structure for Tabs widget
 */
typedef struct {
	int tab;	      /* Newly selected tab          */
	int old_tab;          /* Last selected tab           */
	Boolean doit;	      /* Switch to the new tab flag  */
	char *tab_label;      /* Label on newly selected tab */
} XgTabsCallbackStruct;
	

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*_XgTabs_H_*/
