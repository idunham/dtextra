/**
 *
 * $Id: AnimatedButton.h,v 1.3 2001/06/09 18:38:57 amai Exp $
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
#ifndef _ANIMATEDBUTTON_H
#define _ANIMATEDBUTTON_H

#include <X11/IntrinsicP.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XltNframeDelay
#define XltNframeDelay "frameDelay"
#endif
#ifndef XltCFrameDelay
#define XltCFrameDelay "FrameDelay"
#endif
#ifndef XltNanimationLabels
#define XltNanimationLabels "animationLabels"
#endif
#ifndef XltCAnimationLabels
#define XltCAnimationLabels "AnimationLabels"
#endif
#ifndef XltNanimationPixmaps
#define XltNanimationPixmaps "animationPixmaps"
#endif
#ifndef XltCAnimationPixmaps
#define XltCAnimationPixmaps "AnimationPixmaps"
#endif
#ifndef XltNanimationLabelCount
#define XltNanimationLabelCount "animationLabelCount"
#endif
#ifndef XltCAnimationLabelCount
#define XltCAnimationLabelCount "AnimationLabelCount"
#endif
#ifndef XltNanimationPixmapCount
#define XltNanimationPixmapCount "animationPixmapCount"
#endif
#ifndef XltCAnimationPixmapCount
#define XltCAnimationPixmapCount "AnimationPixmpaCount"
#endif
#ifndef XltNanimate
#define XltNanimate "animate"
#endif
#ifndef XltCAnimate
#define XltCAnimate "Animate"
#endif
#ifndef XltNstopAtFrame
#define XltNstopAtFrame "stopAtFrame"
#endif
#ifndef XltCStopAtFrame
#define XltCStopAtFrame "StopAtFrame"
#endif

extern WidgetClass xrwsAnimatedButtonWidgetClass;

typedef struct _XltAnimatedButtonRec *XltAnimatedButtonWidget;
typedef struct _XltAnimatedButtonClassRec *XltAnimatedButtonWidgetClass;

#define XltIsAnimatedButton(w) XtIsSubclass((w), xrwsAnimatedButtonWidgetClass)

extern Widget XltCreateAnimatedButton(Widget parent,
			     char *name,
			     Arg *arglist,
			     Cardinal argCount);
#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
