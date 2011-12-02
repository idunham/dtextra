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


#ifndef _ExtPushButton_h
#define _ExtPushButton_h



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/*
 * New resources for the Extended Pushbutton widget
 */
typedef	int			XgExtLabelType;

#ifndef XgRExtLabelType
#  define XgRExtLabelType       "ExtLabelType"
#endif 

#ifndef XgSTRING
#  define XgSTRING		0
#endif
#ifndef XgPIXMAP_DATA
#  define XgPIXMAP_DATA		1
#endif
#ifndef XgPIXMAP_FILE
#  define XgPIXMAP_FILE		2
#endif
#ifndef XgPIXMAP_PIXMAP
#  define XgPIXMAP_PIXMAP	3
#endif
#ifndef XgBITMAP_FILE
#  define XgBITMAP_FILE		4
#endif


#ifndef XgNextString
#  define XgNextString		"extString"
#endif
#ifndef XgCExtString
#  define XgCExtString		"ExtString"
#endif

#ifndef XgNextPixmap
#  define XgNextPixmap		"extPixmap"
#endif
#ifndef XgCExtPixmap
#  define XgCExtPixmap		"ExtPixmap"
#endif

#ifndef XgNshift
#  define XgNshift		"shift"
#endif
#ifndef XgCShift 
#  define XgCShift		"Shift"
#endif

#ifndef XgNlabelLocation
#  define XgNlabelLocation      "labelLocation"
#endif
#ifndef XgCLocation
#  define XgCLocation	        "Location"
#endif

#ifndef XgNextLabelType 
#  define XgNextLabelType	"extLabelType"
#endif
#ifndef XgCExtLabelType
#  define XgCExtLabelType	"ExtLabelType"
#endif

#ifndef XgRExtLabelType
#  define XgRExtLabelType	"ExtLabelType"
#endif


/*
 * Constants for labelLocation
 */
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
#ifndef XgNONE
#  define XgNONE	-1
#endif




/*
 * Is ExtPushButton macro
 */
#define XgIsExtPushButton(w) XtIsSubclass(w, xgExtPushButtonWidgetClass)


/*
 * Convienience creation function
 */
#ifndef _NO_PROTO
  Widget XgCreateExtPushButtonWidget(Widget, char *, ArgList, Cardinal);
#else
  Widget XgCreateExtPushButtonWidget();
#endif


extern WidgetClass 			xgExtPushButtonWidgetClass;
typedef struct _XgExtPushButtonClassRec *XgExtPushButtonWidgetClass;
typedef struct _XgExtPushButtonRec      *XgExtPushButtonWidget;



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* _XgExtPushButton_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
