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
 *	Filename: x_rotate.c
 *
 *	Description:
 *		This is the header file for X rotation functions.
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	GMM	07/10/95	Original file.
 *
 *
 * ---FHDR--- */


#ifndef _X_ROTATE_H_
#define _X_ROTATE_H_



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




#ifndef XgALIGN_RIGHT
#  define XgALIGN_RIGHT		1
#endif
#ifndef XgALIGN_CENTER
#  define XgALIGN_CENTER	0
#endif
#ifndef XgALIGN_LEFT
#  define XgALIGN_LEFT		2
#endif



#ifndef _NO_PROTO
  extern int XgDrawRotatedText(
	Display  *dpy,
	Drawable drawable,
	GC gc,
	int x,
	int y,
	int alignment,
	int degrees,
	char *text,
	int len);
#else
  extern int XgDrawRotatedText();
#endif


#ifndef _NO_PROTO 
  extern int XgRotateXImage(XImage *image, int degrees);
#else
  extern int XgRotateXImage();
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif  /* _X_ROTATE_H_ */
