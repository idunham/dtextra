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



#ifndef ROW_H
#define ROW_H

/* Resources:
 * Name         	Class           RepType		Default Value
 * ----         	-----           -------		-------------
 * orientation		Orientation	Int		XmHORIZONTAL
 * horizontalStretch	Stretch		Boolean		False
 * verticalStretch	Stretch		Boolean		False
 * edgeToEdge		Spacing		Boolean		False
 * alignChildren	Align		Int		XgCENTER_ALIGN
 */



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/* Resource specifications */
#ifndef XgNhorizontalStretch
#  define XgNhorizontalStretch   "horizontalStretch"
#endif
#ifndef XgNverticalStretch
#  define XgNverticalStretch     "verticalStretch"
#endif
#ifndef XgNalignChildren
#  define XgNalignChildren       "alignChildren"
#endif

/* Class Resource specifications */
#ifndef XgCStretch
#  define XgCStretch "Stretch"
#endif
#ifndef XgCSpacing
#  define XgCSpacing "Spacing"
#endif
#ifndef XgCAlign
#  define XgCAlign   "Align"
#endif

#ifndef XgRAlign
#  define XgRAlign "Align"
#endif
typedef int	XgAlign;

/* Resource constants */
#define XgALIGN_TOP_LEFT      0
#define XgALIGN_CENTER        1
#define XgALIGN_BOTTOM_RIGHT  2


extern WidgetClass 		xgRowWidgetClass;
typedef struct _XgRowClassRec 	*XgRowWidgetClass;
typedef struct _XgRowRec      	*XgRowWidget;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*  ROW_H */
