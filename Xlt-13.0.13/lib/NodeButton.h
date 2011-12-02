/**
 *
 * $Id: NodeButton.h,v 1.2 2001/06/09 18:38:57 amai Exp $
 *
 * Copyright (C) 1996 Free Software Foundation, Inc.
 * Copyright © 1999-2001 by the LessTif developers.
 *
 * This file is part of the GNU LessTif Extension Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **/
#ifndef _NODEBUTTON_H
#define _NODEBUTTON_H

#include <X11/IntrinsicP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XltNODEBUTTON_BUTTON		1
#define XltNODEBUTTON_ARROW		2
#define XltNODEBUTTON_ROWCOLUMN	3

extern WidgetClass xrwsNodeButtonWidgetClass;

typedef struct _XltNodeButtonRec *XltNodeButtonWidget;
typedef struct _XltNodeButtonClassRec *XmNodeButtonWidgetClass;

#define XltIsNodeButton(w) XtIsSubclass((w), xrwsNodeButtonWidgetClass)

extern Widget XltCreateNodeButton(Widget parent,
			     char *name,
			     Arg *arglist,
			     Cardinal argCount);
extern Widget XltNodeButtonGetChild(Widget W, unsigned char child);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
