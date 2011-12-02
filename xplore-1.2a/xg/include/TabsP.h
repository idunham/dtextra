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
 *	Filename: TabsP.h
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *		This is the private header file for the Tabs widget.
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	messner	06/14/95	Original file
 *	messner	07/26/95	Added label_pixmaps to XgTabsPart struct
 *
 *
 * ---FHDR--- */


#ifndef _XgTabsP_H_
#define _XgTabsP_H_

/*
 * Super Class include
 */
#include <Xm/PrimitiveP.h>

#include "Tabs.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




typedef struct {
    int dummy;
} XgTabsClassPart;

typedef struct _XgTabsClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XgTabsClassPart tabs_class;
} XgTabsClassRec;

typedef struct {

    /* 
     * public resources
     */
    int curtab;
    int numtabs;
    int focus_tab;

    int tab_shape;
    int tab_location;

    String *tablabels;

    Boolean auto_unmanage;
    WidgetList tabwidgets;

    Pixel tabcolor;
    Pixel selected_color;

    XmFontList font_list;

    XtCallbackList  activateCallback;
    int textmargin;

    /*
     * private resources
     */
    int main_tab_width;
    String *labels;
    GC  textgc;
    GC  topgc;
    GC  bottomgc;
    GC  fillgc;
    XFontStruct *font;

    Pixmap *label_pixmaps;

    Boolean i_have_focus;
} XgTabsPart;

typedef struct _XgTabsRec {
    CorePart core;
    XmPrimitivePart primitive;
    XgTabsPart tabs;
} XgTabsRec;

extern XgTabsClassRec xgTabsClassRec;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _XgTabsP_H_ */
