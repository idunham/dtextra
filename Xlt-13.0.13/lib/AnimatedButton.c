/**
 *
 * $Id: AnimatedButton.c,v 1.7 2001/06/09 18:38:57 amai Exp $
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

#include "../config.h"
#include <AnimatedButtonP.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#undef DEBUG

static const char rcsid[] = "$Id: AnimatedButton.c,v 1.7 2001/06/09 18:38:57 amai Exp $";

/*
   Widget methods, forward declarations
 */

static void class_initialize(void);
static void class_part_initialize(WidgetClass widget_class);
static void initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args);
static void destroy(Widget w);
static Boolean set_values(Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args);

/*
   Helper functions, forward declarations
 */
static void SuspendAnimation(Widget w);

/*
   Widget default resources
 */

#define Offset(field) XtOffsetOf(XltAnimatedButtonRec, animated_button.field)
static XtResource resources[] =
{
#if 0
	{
		XltNbubbleString, XltCBubbleString, XmRXmString,
		sizeof(XmString), Offset(BubbleString),
		XmRImmediate, (XtPointer)NULL
	},
#endif
	{
		XltNshowBubble, XltCShowBubble, XmRBoolean,
		sizeof(Boolean), XtOffsetOf(XltBubbleButtonRec, bubble_button.show_bubble),
		XmRImmediate, (XtPointer)False
	},
	{
		XltNanimate, XltCAnimate, XmRBoolean,
		sizeof(Boolean), Offset(animate),
		XmRImmediate, (XtPointer)False
	},
	{
	    XltNanimationLabels, XltCAnimationLabels, XmRXmStringTable,
	    sizeof(XmStringTable), Offset(animation_labels),
	    XmRStringTable, (XtPointer)NULL
	},
	{
	    XltNanimationLabelCount, XltCAnimationLabelCount, XmRInt,
	    sizeof(int), Offset(animation_label_count),
	    XmRImmediate, (XtPointer)0
	},
	{
	    XltNanimationPixmaps, XltCAnimationPixmaps, XmRPixmap,
	    sizeof(Pixmap), Offset(animation_pixmaps),
	    XmRPixmap, (XtPointer)NULL
	},
	{
	    XltNanimationPixmapCount, XltCAnimationPixmapCount, XmRInt,
	    sizeof(int), Offset(animation_pixmap_count),
	    XmRImmediate, (XtPointer)0
	},
	{
		XltNstopAtFrame, XltCStopAtFrame, XtRInt,
		sizeof(int), Offset(stop_at_frame),
		XtRImmediate, (XtPointer)-1
	},
	{
		XltNframeDelay, XltCFrameDelay, XtRInt,
		sizeof(int), Offset(Delay),
		XtRImmediate, (XtPointer)1000
	}
};

#if 0
static XmSyntheticResource syn_resources[] =
{
	{
		XltNbubbleString,
		sizeof(XmString), Offset(BubbleString),
		_XmExportLabelString, NULL
	}
};
#endif
#undef Offset

/*
   Widget class record
 */

#if 0
static XtActionsRec actions[] =
{
};
#endif

/* *INDENT-OFF* */
XltAnimatedButtonClassRec xrwsAnimatedButtonClassRec = {
    /* Core class part */
    {
	/* superclass            */ (WidgetClass) &xrwsBubbleButtonClassRec,
        /* class_name            */ "XltAnimatedButton",
	/* widget_size           */ sizeof(XltAnimatedButtonRec),
	/* class_initialize      */ class_initialize,
	/* class_part_initialize */ class_part_initialize,
	/* class_inited          */ False,
	/* initialize            */ initialize,
	/* initialize_hook       */ NULL,
	/* realize               */ XtInheritRealize,
	/* actions               */ NULL /*actions*/,
	/* num_actions           */ 0 /*XtNumber(actions)*/,
	/* resources             */ resources,
	/* num_resources         */ XtNumber(resources),
	/* xrm_class             */ NULLQUARK,
	/* compress_motion       */ True,
	/* compress_exposure     */ XtExposeCompressMaximal,
	/* compress_enterleave   */ True,
	/* visible_interest      */ False,
	/* destroy               */ destroy,
	/* resize                */ XtInheritResize,
	/* expose                */ XtInheritExpose,
	/* set_values            */ set_values,
	/* set_values_hook       */ NULL,
	/* set_values_almost     */ XtInheritSetValuesAlmost,
	/* get_values_hook       */ NULL,
	/* accept_focus          */ NULL,
	/* version               */ XtVersion,
	/* callback offsets      */ NULL,
	/* tm_table              */ NULL,
	/* query_geometry        */ XtInheritQueryGeometry,
	/* display_accelerator   */ NULL,
	/* extension             */ (XtPointer)NULL
    },
    /* Primitive Class part */
    {
	/* border_highlight      */ XmInheritBorderHighlight,
       	/* border_unhighlight    */ XmInheritBorderUnhighlight,
       	/* translations          */ XtInheritTranslations,
       	/* arm_and_activate_proc */ XmInheritArmAndActivate,
       	/* synthetic resources   */ NULL /*syn_resources*/, 
        /* num syn res           */ 0 /*XtNumber(syn_resources)*/,
	/* extension             */ (XtPointer)NULL
    },
    /* Label Class part */
    {
        /* setOverrideCallback */ XmInheritSetOverrideCallback,
        /* menuProcs           */ XmInheritMenuProc,
        /* translations        */ XtInheritTranslations,
	/* extension           */ NULL
    },
    /* PushButton Class part */
    {
	/* extension */ NULL
    },
    /* BubbleButton Class part */
    {
	/* leave_time  */ 0,
	/* extension   */ NULL
    },
    /* AnimatedButton Class part */
    {
	/* extension   */ NULL
    }
};
/* *INDENT-ON* */



WidgetClass xrwsAnimatedButtonWidgetClass = (WidgetClass)&xrwsAnimatedButtonClassRec;

/*
   Helper routines
 */
static void
CopyStringTable(Widget w, int numLabels)
{
int i;
XmStringTable old_table;
XmStringTable new_table;

	old_table = AnimatedButton_AnimationLabels(w);
	new_table = (XmStringTable)XtMalloc(AnimatedButton_AnimationLabelCount(w) * sizeof(XmString));
	for (i = 0; i < numLabels; i++)
	{
		new_table[i] = XmStringCopy(old_table[i]);
	}
	AnimatedButton_AnimationLabels(w) = new_table;
}

static void
NextFrame(Widget w, XtIntervalId *Id)
{
	if (Lab_IsText(w))
	{
	    if (AnimatedButton_AnimationLabelCount(w) > 0)
	    {
		XtVaSetValues(w,
		    XmNlabelString, AnimatedButton_AnimationLabels(w)[AnimatedButton_NextFrame(w)],
		    NULL);
	    }
	    AnimatedButton_NextFrame(w)++;
	    if (AnimatedButton_NextFrame(w) >= AnimatedButton_AnimationLabelCount(w))
	    {
		AnimatedButton_NextFrame(w) = 0;
	    }
	}
	else
	{
	    if (AnimatedButton_AnimationPixmapCount(w) > 0)
	    {
		XtVaSetValues(w,
		    XmNlabelPixmap, AnimatedButton_AnimationPixmaps(w)[AnimatedButton_NextFrame(w)],
		    NULL);
	    }
	    AnimatedButton_NextFrame(w)++;
	    if (AnimatedButton_NextFrame(w) >= AnimatedButton_AnimationPixmapCount(w))
	    {
		AnimatedButton_NextFrame(w) = 0;
	    }
	}

	if (AnimatedButton_NextFrame(w) == AnimatedButton_StopAtFrame(w))
	{
	    AnimatedButton_Animate(w) = False;
	    /* stopAtFrameCallback maybe */
	}

	if (AnimatedButton_Animate(w))
	{
	    AnimatedButton_Timer(w) = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
						AnimatedButton_Delay(w),
						(XtTimerCallbackProc)NextFrame,
						w);
	}
	else
	{
		AnimatedButton_Timer(w) = (XtIntervalId)NULL;
	}
}

static void
ResumeAnimation(Widget w)
{
	if (AnimatedButton_Timer(w) == (XtIntervalId)NULL)
	{
		AnimatedButton_Timer(w) = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
						AnimatedButton_Delay(w),
						(XtTimerCallbackProc)NextFrame,
						w);
	}
	AnimatedButton_Animate(w) = True;
}

static void
SuspendAnimation(Widget w)
{
	if (AnimatedButton_Timer(w) != (XtIntervalId)NULL)
	{
		XtRemoveTimeOut(AnimatedButton_Timer(w));
		AnimatedButton_Timer(w) = (XtIntervalId)NULL;
	}
	AnimatedButton_Animate(w) = False;
}
/*
   Widget methods
 */

static void
class_initialize(void)
{
}

static void
class_part_initialize(WidgetClass widget_class)
{
}

static void
initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
	AnimatedButton_Timer(new_w) = (XtIntervalId)NULL;
	AnimatedButton_NextFrame(new_w) = 0;
	if (AnimatedButton_AnimationLabels(new_w))
	{
		CopyStringTable(new_w,
			AnimatedButton_AnimationLabelCount(new_w));
	}
	if (AnimatedButton_Animate(new_w))
	{
		ResumeAnimation(new_w);
	}
}

static void
destroy(Widget w)
{
int i;

	if (AnimatedButton_AnimationLabels(w) != NULL)
	{
	    for (i = 0; i < AnimatedButton_AnimationLabelCount(w); i++)
	    {
		XmStringFree(AnimatedButton_AnimationLabels(w)[i]);
	    }
	    XtFree((void *)AnimatedButton_AnimationLabels(w));
	}
}

static Boolean
set_values(Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
	if (AnimatedButton_AnimationLabels(old) != AnimatedButton_AnimationLabels(new_w))
	{
		AnimatedButton_NextFrame(new_w) = 0;
		CopyStringTable(new_w,
			AnimatedButton_AnimationLabelCount(new_w));
	}
	if (AnimatedButton_Animate(old) != AnimatedButton_Animate(new_w))
	{
	    if (AnimatedButton_Animate(new_w))
	    {
		    ResumeAnimation(new_w);
	    }
	    else
	    {
		    SuspendAnimation(new_w);
	    }
	}
	return (False);
}

/*
   Public functions
 */

Widget
XltCreateAnimatedButton(Widget parent,
	       char *name,
	       Arg *arglist,
	       Cardinal argCount)
{
	return XtCreateWidget(name, xrwsAnimatedButtonWidgetClass, parent, arglist, argCount);
}

