/*
    $Id: Stroke.h,v 1.4 2000/09/20 09:38:06 amai Exp $

    libStroke is a library to enable any X application to use Strokes.
    Copyright (C) 1997  Richard Scott <rwscott@magi.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _STROKE_H
#define _STROKE_H

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_STROKE_TRANSLATION "!<Btn3Motion>: Input()\\n\
				    !<Btn3Down>: Input()\\n\
				    !<Btn3Up>: Input()"

typedef struct {
	String Stroke;
	String Action;
} StrokeMap, *StrokeMapPtr;

typedef struct _StrokeStruct {
	Widget widget;
	Boolean InStroke;
	Boolean Debug;
	String translations;
	StrokeMapPtr *Map;
	XPoint *points;
	int *box;
	int npoints;
	int maxpoints;
	int xmin;
	int xmax;
	int ymin;
	int ymax;
	int button;
	int slop;
	GC gc;
	String Sound;
	struct _StrokeStruct *next;
} StrokeState_t, *StrokeStatePtr;

void StrokeInitialize(Widget DrawingArea);

void StrokeSetDebug(Widget W, Boolean Debug);
Boolean StrokeGetDebug(Widget W);

void StrokeSetMapping(Widget W, String Map);
String StrokeGetMapping(Widget W);

void XltHelpOnStrokes(Widget W);

#ifdef __cplusplus
}
#endif

#endif /* _STROKE_H */
