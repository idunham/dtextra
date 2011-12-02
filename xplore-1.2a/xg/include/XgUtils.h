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
 *	Filename   : XgUtils.h
 *	Module     : Xg WIdget Set
 *	Version ID : 1.14 26 Feb 1997
 *
 *	Description:
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	-------	---------------	---------------------------------------------
 *	messner	07/07/96	Original file
 *
 *
 * ---FHDR--- */


#ifndef _XgUtils_h_
#define _XgUtils_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <Xm/Xm.h>

#ifdef USE_XM_MWMUTIL_H
#  include <Xm/Protocols.h>
#  include <Xm/MwmUtil.h>
#else
#  ifdef USE_X11_MWMUTIL_H
#    include <X11/Protocols.h>
#    include <X11/MwmUtil.h>
#  endif
#endif

#if NeedFunctionPrototypes || defined(__cplusplus)
	extern char *XgConvertXmStringToString(XmString fromMe);
	extern Widget XgGetShellWidget(Widget w);
	extern Widget XgGetShellChildWidget(Widget w);
	extern void XgCenterShellOverParent(Widget pShell, Widget pParent);
	extern void XgUpdateLabel(Widget pLabel);
	extern void XgSetOlwmDecor(Widget widget, int decor);
	extern int XgRemoveDecorationAndFunction(
			Widget pShell, int iDecorations, int iFunctions);
	extern void XgSetCloseCallback(Widget pShell, 
			XtCallbackProc pCB, XtPointer pClientData);
	extern void XgSetListStrings(Widget list, char ** strings, int count);
	extern String XgWidgetToName(Widget w);
	extern Boolean XgScrollToPosition(Widget w, int pos, int item_height);
	extern Pixel XgGetClosestColor(Display * display,
			XColor *xcolor, XColor * xcolors, int max_colors);
	extern XRectangle * XgGrabArea(Display *dpy, Window confine_to,
		Window convert_to);
#else
	extern char *XgConvertXmStringToString();
	extern Widget XgGetShellWidget();
	extern Widget XgGetShellChildWidget();
	extern void XgCenterShellOverParent();
	extern void XgUpdateLabel();
	extern void XgSetOlwmDecor();
	extern int XgRemoveDecorationAndFunction();
	extern void XgSetCloseCallback();
	extern void XgSetListStrings();
	extern String XgWidgetToName();
	extern Boolean XgScrollToPosition();
	extern Pixel XgGetClosestColor();
	extern XRectangle * XgGrabArea();
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* _XgUtils_h_ */


