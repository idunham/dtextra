/**
 *
 * $Id: AnimatedButtonP.h,v 1.2 2001/06/09 18:38:57 amai Exp $
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
#ifndef _ANIMATEDBUTTONP_H
#define _ANIMATEDBUTTONP_H

#include <AnimatedButton.h>
#include <BubbleButtonP.h>

typedef struct {
	XtIntervalId Timer;
	int Delay;
	int next_frame;
	Boolean animate;
	XmStringTable animation_labels;
	int animation_label_count;
	Pixmap *animation_pixmaps;
	int animation_pixmap_count;
	int stop_at_frame;
} XltAnimatedButtonPart;

#define AnimatedButton_Timer(w) (((XltAnimatedButtonWidget)w)->animated_button.Timer)
#define AnimatedButton_Delay(w) (((XltAnimatedButtonWidget)w)->animated_button.Delay)
#define AnimatedButton_NextFrame(w) (((XltAnimatedButtonWidget)w)->animated_button.next_frame)
#define AnimatedButton_Animate(w) (((XltAnimatedButtonWidget)w)->animated_button.animate)
#define AnimatedButton_AnimationLabelCount(w) (((XltAnimatedButtonWidget)w)->animated_button.animation_label_count)
#define AnimatedButton_AnimationLabels(w) (((XltAnimatedButtonWidget)w)->animated_button.animation_labels)
#define AnimatedButton_AnimationPixmapCount(w) (((XltAnimatedButtonWidget)w)->animated_button.animation_pixmap_count)
#define AnimatedButton_AnimationPixmaps(w) (((XltAnimatedButtonWidget)w)->animated_button.animation_pixmaps)
#define AnimatedButton_StopAtFrame(w) (((XltAnimatedButtonWidget)w)->animated_button.stop_at_frame)

typedef struct _XltAnimatedButtonRec {
	CorePart core;
	XmPrimitivePart primitive;
	XmLabelPart label;
	XmPushButtonPart pushbutton;
	XltBubbleButtonPart bubble_button;
	XltAnimatedButtonPart animated_button;
} XltAnimatedButtonRec;

typedef struct {
	XtPointer extension;
} XltAnimatedButtonClassPart;

typedef struct _XltAnimatedButtonClassRec {
	CoreClassPart core_class;
	XmPrimitiveClassPart primitive_class;
	XmLabelClassPart label_class;
	XmPushButtonClassPart pushbutton_class;
	XltBubbleButtonClassPart bubble_button_class;
	XltAnimatedButtonClassPart animated_button_class;
} XltAnimatedButtonClassRec;

extern XltAnimatedButtonClassRec xrwsAnimatedButtonClassRec;
#endif
