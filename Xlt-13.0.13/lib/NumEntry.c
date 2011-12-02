/**
 *
 * $Id: NumEntry.c,v 1.18 2005/09/03 12:50:17 rwscott Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/ArrowB.h>
#include <Xm/RowColumn.h>
#include <Xm/MessageB.h>
#include <NumEntryP.h>

#include "calc.h"


#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#undef DEBUG

static const char rcsid[] = "$Id: NumEntry.c,v 1.18 2005/09/03 12:50:17 rwscott Exp $";

/*
   Widget methods, forward declarations
 */

static void destroy(Widget aw);
static void initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args);
static Boolean set_values(Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args);

/*
   Helper functions, forward declarations
 */

static void Help(Widget w, XEvent *event, String *params, Cardinal *num_params);

/*
   Widget default resources
 */

#define Offset(field) XtOffsetOf(XltNumEntryRec, num_entry.field)
static XtResource resources[] =
{
    {
	XmNlabelString, XmCLabelString, XmRXmString,
	sizeof(XmString), Offset(LabelString),
	XmRImmediate, (XtPointer)NULL
    },
    {
	XmNvalue, XmCValue, XtRString,
	sizeof(String), Offset(value),
	XtRString, ""
    },
    {
	XmNcolumns, XmCColumns, XmRShort,
	sizeof(short), Offset(columns),
	XtRImmediate, (XtPointer)20
    },
    {
	XmNvalueChangedCallback, XtCCallback, XtRCallback,
	sizeof(XtCallbackList), Offset(ValueChangedCallback),
	XtRCallback, NULL
    },
    {
	XmNactivateCallback, XtCCallback, XtRCallback,
	sizeof(XtCallbackList), Offset(ActivateCallback),
	XtRCallback, NULL
    },
    {
	XmNlosingFocusCallback, XtCCallback, XtRCallback,
	sizeof(XtCallbackList), Offset(LosingFocusCallback),
	XtRCallback, NULL
    },
    {
	XmNhelpCallback, XtCCallback, XtRCallback,
	sizeof(XtCallbackList), Offset(HelpCallback),
	XtRCallback, NULL
    },
    {
	XltNinitialDelay, XltCInitialDelay, XtRInt,
	sizeof(int), Offset(InitialDelay),
	XtRImmediate, (XtPointer)1000
    },
    {
	XltNdelay, XltCDelay, XtRInt,
	sizeof(int), Offset(Delay),
	XtRImmediate, (XtPointer)100
    },
};

static void NEGetValue(Widget wid, int resource_offset, XtArgVal *value);
static void NEGetColumns(Widget wid, int resource_offset, XtArgVal *value);

static XmSyntheticResource syn_resources[] =
{
    {
	XmNvalue,
	sizeof(String), Offset(value),
	NEGetValue, NULL
    },
    {
	XmNcolumns,
	sizeof(short), Offset(columns),
	NEGetColumns, NULL
    }
};
#undef Offset

static void KeyPad(Widget w, XEvent *event, String *params, Cardinal *num_params);
static void Up(Widget w, XEvent *event, String *params, Cardinal *num_params);
static void Down(Widget w, XEvent *event, String *params, Cardinal *num_params);

static XtActionsRec actions[] =
{
    {"KeyPad", KeyPad},
    {"Up", Up},
    {"Down", Down},
    {"Help", Help},
};

static char tm_table[] = "\
	<Key>osfDown: Down()\n\
	<Key>osfUp: Up()\n\
	!<Btn4Down>: Up()\n\
	!<Btn5Down>: Down()";

static char text_tm_table[] = "\
	<Key>KP_Insert: KeyPad()\n\
	<Key>KP_Delete: KeyPad()\n\
	<Key>KP_Enter: KeyPad()\n\
	<Key>KP_End: KeyPad()\n\
	<Key>KP_Down: KeyPad()\n\
	<Key>KP_Next: KeyPad()\n\
	<Key>KP_Left: KeyPad()\n\
	<Key>KP_Begin: KeyPad()\n\
	<Key>KP_Right: KeyPad()\n\
	<Key>KP_Home: KeyPad()\n\
	<Key>KP_Up: KeyPad()\n\
	<Key>KP_Prior: KeyPad()\n\
	<Key>KP_Add: KeyPad()\n\
	<Key>KP_Divide: KeyPad()\n\
	<Key>KP_Multiply: KeyPad()\n\
	<Key>KP_Subtract: KeyPad()";

/*
   Widget class record
 */
/* *INDENT-OFF* */
XltNumEntryClassRec xrwsNumEntryClassRec = {
    /* Core Class Part */
    {
    /* superclass            */ (WidgetClass) & xmFormClassRec,
    /* class_name            */ "XltNumEntry",
    /* widget_size           */ sizeof(XltNumEntryRec),
    /* class_initialize      */ NULL /*class_initialize*/,
    /* class_part_initialize */ NULL /*class_part_initialize*/,
    /* class_inited          */ False,
    /* initialize            */ initialize,
    /* initialize_hook       */ NULL,
    /* realize               */ XtInheritRealize /*realize*/,
    /* actions               */ actions,
    /* num_actions           */ XtNumber(actions),
    /* resources             */ resources,
    /* num_resources         */ XtNumber(resources),
    /* xrm_class             */ NULLQUARK,
    /* compress_motion       */ True,
    /* compress_exposure     */ XtExposeCompressMultiple,
    /* compress_enterleave   */ True,
    /* visible_interest      */ True,
    /* destroy               */ destroy,
    /* resize                */ XtInheritResize,
    /* expose                */ XtInheritExpose,
    /* set_values            */ set_values,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL /*get_values_hook*/,
    /* accept_focus          */ NULL,
    /* version               */ XtVersion,
    /* callback offsets      */ NULL,
    /* tm_table              */ tm_table /*XtInheritTranslations*/,
    /* query_geometry        */ XtInheritQueryGeometry,
    /* display_accelerator   */ XtInheritDisplayAccelerator,
    /* extension             */ NULL
    },
    /* Composite Class Part */
    {
    /* geometry manager */	XtInheritGeometryManager,
    /* change_managed   */	XtInheritChangeManaged,
    /* insert_child     */	XtInheritInsertChild,
    /* delete_child     */	XtInheritDeleteChild,
    /* extension        */	NULL,
    },
    /* Constraint Class Part */
    {
    /* subresources      */ NULL,
    /* subresource_count */ 0,
    /* constraint_size   */ sizeof(XmFormConstraintRec),
    /* initialize        */ NULL /*constraint_initialize*/,
    /* destroy           */ NULL,
    /* set_values        */ NULL /*constraint_set_values*/,
    /* extension         */ NULL,
    },
    /* Manager Class Part */
    {
    /* translations           */ XmInheritTranslations,
    /* syn_resources          */ syn_resources,
    /* num_syn_resources      */ XtNumber(syn_resources),
    /* syn_constraint_res     */ NULL,
    /* num_syn_constraint_res */ 0,
    /* parent_process         */ XmInheritParentProcess,
    /* extension              */ NULL
    },
    /* XmBulletinBoard part */
    {
    /* always_install_accelerators  */ False,
    /* geo_matrix_create            */ NULL,
    /* focus_moved_proc             */ XmInheritFocusMovedProc,
    /* extension */ NULL
    },
    /* XmForm part */
    {
    /* extension */ NULL
    },
    /* NumEntry Class Part */
    {
    /* extension */ NULL
    }
};
/* *INDENT-ON* */





WidgetClass xrwsNumEntryWidgetClass = (WidgetClass)&xrwsNumEntryClassRec;

/*
   Helper routines
 */

static void
_AutoRepeat(Widget w)
{
    Widget Entry = w;

    while (!XltIsNumEntry(Entry))
	Entry = XtParent(Entry);
    if (XtIsSensitive(Entry))
    {
	NumEntry_TimerId(Entry) = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
						  NumEntry_Delay(Entry),
						  (XtTimerCallbackProc)_AutoRepeat,
						  w);
	XtCallCallbacks(w, XmNactivateCallback, NULL);
    }
    else
    {
	NumEntry_TimerId(Entry) = (XtIntervalId)NULL;
    }
}

static void
AutoRepeat(Widget w, Boolean On)
{
    Widget button = w;

    while (!XltIsNumEntry(w))
	w = XtParent(w);
    if (On)
    {
	if (!NumEntry_TimerId(w))
	{
	    NumEntry_TimerId(w) = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
						  NumEntry_InitialDelay(w),
						  (XtTimerCallbackProc)_AutoRepeat,
						  button);
	}
    }
    else
    {
	if (NumEntry_TimerId(w))
	{
	    XtRemoveTimeOut(NumEntry_TimerId(w));
	    NumEntry_TimerId(w) = (XtIntervalId)NULL;
	}
    }
}

static void
DoMath(Widget W)
{
    String value;
    double result;
    Position Place;

#ifdef DEBUG
    printf("DoMath(%s)\n", XtName(W));
#endif
    value = XmTextFieldGetString(NumEntry_TextField(W));
    Place = XmTextFieldGetInsertionPosition(NumEntry_TextField(W));
    result = XltCalc(value);
    if (result != NumEntry_Result(W))
    {
	char buf[1024];
	XltNumEntryVerifyCallbackStruct cbs;

	NumEntry_Result(W) = result;
	sprintf(buf, "%.*g", NumEntry_Columns(W), result);
	cbs.reason = XmCR_VALUE_CHANGED;
	cbs.event = NULL;
	cbs.doit = True;
	cbs.value = XtNewString(buf);
	cbs.dvalue = NumEntry_Result(W);
	XtCallCallbacks(W, XmNvalueChangedCallback, &cbs);
	if (cbs.doit)
	{
	    XtFree(NumEntry_Value(W));
	    NumEntry_Value(W) = cbs.value;
	    NumEntry_Result(W) = cbs.dvalue;
	}
	else
	{
	    XtFree(cbs.value);
	}
    }
    XmTextFieldSetString(NumEntry_TextField(W), NumEntry_Value(W));
    XmTextFieldSetInsertionPosition(NumEntry_TextField(W), Place);
    XtFree(value);
}

/* ******************** */

static void
LosingFocus(Widget W, XtPointer client_data, XtPointer call_data)
{
#ifdef DEBUG
    printf("NumEntry - LosingFocus(%s) - >%s<\n", XtName(W), NumEntry_Value(XtParent(W)));
#endif
    AutoRepeat(W, False);
    DoMath(XtParent(W));
    XtCallCallbacks(XtParent(W), XmNlosingFocusCallback, call_data);
}

/* ******************** */

static void
Activate(Widget W, XtPointer client_data, XtPointer call_data)
{
#ifdef DEBUG
    printf("NumEntry - Activate(%s) - %s\n", XtName(W), XtName(XtParent(W)));
#endif
    DoMath(XtParent(W));
    XtCallCallbacks(XtParent(W), XmNactivateCallback, call_data);
}

/* ******************** */

static void
ValueChange(Widget W, int a, int b)
{
#ifdef DEBUG
    printf("NumEntry - ValueChange(%s) - >%s<\n", XtName(W), NumEntry_Value(XtParent(W)));
#endif
}

/* ******************** */

static char *
IncDigit(char *buf, Position *Place, int a)
{
    if (*Place < 0)
    {
	if (a > 0)
	{
	    buf = XtRealloc(buf, strlen(buf) + 2);
	    memmove(&buf[1], &buf[0], strlen(buf) + 1);
	    buf[0] = '0';
	    *Place += 1;
	    buf = IncDigit(buf, Place, a);
	}
	else
	{
	    buf[*Place + 1] = '0';
	}
	return (buf);
    }
    if ((buf[*Place] >= '0') && (buf[*Place] <= '9'))
    {
	buf[*Place] = buf[*Place] + a;
	if (buf[*Place] > '9')
	{
	    if (*Place < 0 || buf[*Place - 1] == '-')
	    {
		buf = XtRealloc(buf, strlen(buf) + 2);
		memmove(&buf[*Place + 1], &buf[*Place], strlen(buf) + 1);
		buf[*Place] = '0';
		*Place += 1;
		buf = IncDigit(buf, Place, a);
	    }
	    buf[*Place] = '0';
	    if (buf[*Place - 1] != '.')
	    {
		*Place -= 1;
		buf = IncDigit(buf, Place, a);
		*Place += 1;
	    }
	    else
	    {
		*Place -= 2;
		buf = IncDigit(buf, Place, a);
		*Place += 2;
	    }
	}
	if (buf[*Place] < '0')
	{
	    buf[*Place] = '9';
	    if (buf[*Place - 1] != '.')
	    {
		*Place -= 1;
		buf = IncDigit(buf, Place, a);
		*Place += 1;
	    }
	    else
	    {
		*Place -= 2;
		buf = IncDigit(buf, Place, a);
		*Place += 2;
	    }
	}
	if (buf[*Place] == '0' && *Place == 0)
	{
	    *Place -= 1;
	}
    }
    else
    {
    }
    return (buf);
}

/* ******************** */

static void
StepValue(Widget W, int a, int b)
{
    Widget Rc = XtParent(XtParent(W));
    Position Place;
    char *buf;

#ifdef DEBUG
    printf("NumEntry - StepValue(%s) - %s\n", XtName(W), XtName(Rc));
#endif
    buf = XmTextFieldGetString(NumEntry_TextField(Rc));
    Place = XmTextFieldGetInsertionPosition(NumEntry_TextField(Rc));
    if (buf[Place] != '.')
    {
	buf = IncDigit(buf, &Place, a);
	XmTextFieldSetString(NumEntry_TextField(Rc), buf);
	XmTextFieldSetInsertionPosition(NumEntry_TextField(Rc), Place);
	DoMath(Rc);
    }
    XtFree(buf);
}

/* ******************** */

/*
   Widget methods
 */

static void
destroy(Widget aw)
{
#ifdef DEBUG
    printf("NumEntry - destroy(%s)\n", XtName(aw));
#endif
    XtFree(NumEntry_Value(aw));
}

static void
initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
    Dimension h;

#ifdef DEBUG
    printf("NumEntry - initialize(%s) - %i args\n", XtName(new_w), *num_args);
#endif
    XtHeight(new_w) = 0;
    XtWidth(new_w) = 0;
    NumEntry_Value(new_w) = XtNewString(NumEntry_Value(new_w));
    NumEntry_Label(new_w) = XmCreateLabel(new_w, "Label", NULL, 0);
    if (NumEntry_LabelString(new_w) == NULL)
    {
	NumEntry_LabelString(new_w) = XmStringCreateSimple(XtName(new_w));
    }
    XtVaSetValues(NumEntry_Label(new_w),
		  XmNlabelString, NumEntry_LabelString(new_w),
		  NULL);
    XtOverrideTranslations(NumEntry_Label(new_w), XtParseTranslationTable(tm_table));
    XtManageChild(NumEntry_Label(new_w));
    XtHeight(new_w) = XtHeight(new_w) > XtHeight(NumEntry_Label(new_w)) ? XtHeight(new_w) : XtHeight(NumEntry_Label(new_w));
    XtWidth(new_w) += XtWidth(NumEntry_Label(new_w));

    NumEntry_TextField(new_w) = XmCreateTextField(new_w, "Text", NULL, 0);
    XtAddCallback(NumEntry_TextField(new_w), XmNvalueChangedCallback, (XtCallbackProc)ValueChange, (XtPointer)NULL);
    XtAddCallback(NumEntry_TextField(new_w), XmNactivateCallback, (XtCallbackProc)Activate, (XtPointer)NULL);
    XtAddCallback(NumEntry_TextField(new_w), XmNlosingFocusCallback, (XtCallbackProc)LosingFocus, (XtPointer)NULL);
    XtVaSetValues(NumEntry_TextField(new_w),
		  XmNvalue, NumEntry_Value(new_w),
		  XmNcolumns, NumEntry_Columns(new_w),
		  NULL);
    XtOverrideTranslations(NumEntry_TextField(new_w), XtParseTranslationTable(tm_table));
    XtOverrideTranslations(NumEntry_TextField(new_w), XtParseTranslationTable(text_tm_table));
    XtManageChild(NumEntry_TextField(new_w));
    XtHeight(new_w) = XtHeight(new_w) > XtHeight(NumEntry_TextField(new_w)) ? XtHeight(new_w) : XtHeight(NumEntry_TextField(new_w));
    XtWidth(new_w) += XtWidth(NumEntry_TextField(new_w));
    h = XtHeight(new_w);
    h = (h * 2) / 3;
    NumEntry_RowColumn(new_w) = XmCreateRowColumn(new_w, "RowCol", NULL, 0);
    XtVaSetValues(NumEntry_RowColumn(new_w),
		  XmNtraversalOn, False,
		  XmNpacking, XmPACK_COLUMN,
		  XmNorientation, XmHORIZONTAL,
		  XmNnumColumns, 1,
		  NULL);
    XtOverrideTranslations(NumEntry_RowColumn(new_w), XtParseTranslationTable(tm_table));
    NumEntry_UpArrow(new_w) = XmCreateArrowButton(NumEntry_RowColumn(new_w), "UpArrow", NULL, 0);
    XtAddCallback(NumEntry_UpArrow(new_w), XmNactivateCallback, (XtCallbackProc)StepValue, (XtPointer)1);
    XtAddCallback(NumEntry_UpArrow(new_w), XmNarmCallback, (XtCallbackProc)AutoRepeat, (XtPointer)True);
    XtAddCallback(NumEntry_UpArrow(new_w), XmNdisarmCallback, (XtCallbackProc)AutoRepeat, (XtPointer)False);
    XtVaSetValues(NumEntry_UpArrow(new_w),
		  XmNwidth, h,
		  XmNarrowDirection, XmARROW_UP,
		  NULL);
    XtOverrideTranslations(NumEntry_UpArrow(new_w), XtParseTranslationTable(tm_table));
    XtManageChild(NumEntry_UpArrow(new_w));
    NumEntry_DnArrow(new_w) = XmCreateArrowButton(NumEntry_RowColumn(new_w), "DnArrow", NULL, 0);
    XtAddCallback(NumEntry_DnArrow(new_w), XmNactivateCallback, (XtCallbackProc)StepValue, (XtPointer)-1);
    XtAddCallback(NumEntry_DnArrow(new_w), XmNarmCallback, (XtCallbackProc)AutoRepeat, (XtPointer)True);
    XtAddCallback(NumEntry_DnArrow(new_w), XmNdisarmCallback, (XtCallbackProc)AutoRepeat, (XtPointer)False);
    XtVaSetValues(NumEntry_DnArrow(new_w),
		  XmNwidth, h,
		  XmNarrowDirection, XmARROW_DOWN,
		  NULL);
    XtOverrideTranslations(NumEntry_DnArrow(new_w), XtParseTranslationTable(tm_table));
    XtManageChild(NumEntry_DnArrow(new_w));
    XtVaSetValues(NumEntry_RowColumn(new_w),
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_NONE,
		  XmNrightAttachment, XmATTACH_FORM,
		  NULL);
    XtVaSetValues(NumEntry_TextField(new_w),
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_NONE,
		  XmNrightAttachment, XmATTACH_WIDGET,
		  XmNrightWidget, NumEntry_RowColumn(new_w),
		  NULL);
    XtVaSetValues(NumEntry_Label(new_w),
		  XmNalignment, XmALIGNMENT_BEGINNING,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_WIDGET,
		  XmNrightWidget, NumEntry_TextField(new_w),
		  NULL);
    {
    XtWidgetGeometry pref;

	XtQueryGeometry(NumEntry_RowColumn(new_w), NULL, &pref);
	XtWidth(new_w) += pref.width;
    }
    XtManageChild(NumEntry_RowColumn(new_w));
    XtAddCallback(new_w, XmNhelpCallback, (XtCallbackProc)Help, NULL);
    NumEntry_TimerId(new_w) = (XtIntervalId)NULL;
    NumEntry_Result(new_w) = XltCalc(NumEntry_Value(new_w));
}

static Boolean
set_values(Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
    Boolean refresh = False;

#ifdef DEBUG
    printf("NumEntry - set_values(%s) - >%s< >%s<\n", XtName(new_w), NumEntry_Value(old), NumEntry_Value(new_w));
#endif
    if (XtIsSensitive(old) != XtIsSensitive(new_w) && !XtIsSensitive(new_w))
    {
	if (NumEntry_TimerId(new_w))
	{
	    XtRemoveTimeOut(NumEntry_TimerId(new_w));
	    NumEntry_TimerId(new_w) = (XtIntervalId)NULL;
	}
    }
    if (NumEntry_LabelString(old) != NumEntry_LabelString(new_w))
    {
	NumEntry_LabelString(new_w) = XmStringCopy(NumEntry_LabelString(new_w));
	XtVaSetValues(NumEntry_Label(new_w),
		      XmNlabelString, NumEntry_LabelString(new_w),
		      NULL);
	if (NumEntry_LabelString(old))
	{
	    XmStringFree(NumEntry_LabelString(old));
	}
    }
    if (NumEntry_Value(old) != NumEntry_Value(new_w))
    {
	char *tmp;
	Position Place;
#ifdef DEBUG
	printf("NumEntry - set_values(%s) - value old >%s< new >%s<\n", XtName(new_w),
	       NumEntry_Value(old), NumEntry_Value(new_w));
#endif
	tmp = NumEntry_Value(new_w);
	NumEntry_Value(new_w) = XtNewString(tmp);
	Place = XmTextFieldGetInsertionPosition(NumEntry_TextField(new_w));
	XmTextFieldSetString(NumEntry_TextField(new_w), NumEntry_Value(new_w));
	XmTextFieldSetInsertionPosition(NumEntry_TextField(new_w), Place);
	if (NumEntry_Value(old))
	{
	    XtFree(NumEntry_Value(old));
	}
	refresh = True;
	DoMath(new_w);
    }
    if (NumEntry_Columns(old) != NumEntry_Columns(new_w))
    {
    Dimension old_width = XtWidth(NumEntry_TextField(new_w));

	XtVaSetValues(NumEntry_TextField(new_w),
		      XmNcolumns, NumEntry_Columns(new_w),
		      NULL);
	XtWidth(new_w) -= (old_width - XtWidth(NumEntry_TextField(new_w)));
	refresh = True;
    }
    return (refresh);
}

/*
   Synthetic resource hooks
 */

static void
NEGetValue(Widget wid, int resource_offset, XtArgVal *value)
{
    *value = (XtArgVal)XmTextFieldGetString(NumEntry_TextField(wid));
#ifdef DEBUG
    printf("NumEntry - NEGetValue(%s) - >%s< >%s<\n", XtName(wid), *value, NumEntry_Value(wid));
#endif
}

static void
NEGetColumns(Widget wid, int resource_offset, XtArgVal *value)
{
    XtVaGetValues(NumEntry_TextField(wid),
		  XmNcolumns, value,
		  NULL);
#ifdef DEBUG
    printf("NumEntry - NEGetColumns(%s)\n", XtName(wid));
#endif
}

static void
KeyPad(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    Widget Entry;

    Entry = w;
    while (!XltIsNumEntry(Entry))
	Entry = XtParent(Entry);
    event->xkey.state |= Mod2Mask;
    XtCallActionProc(NumEntry_TextField(Entry), "self-insert", event, params, *num_params);
}

static void
Up(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    Widget Entry;

    Entry = w;
    while (!XltIsNumEntry(Entry))
	Entry = XtParent(Entry);
    XtCallActionProc(NumEntry_UpArrow(Entry), "ArmAndActivate", event, params, *num_params);
}

static void
Down(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    Widget Entry;

    Entry = w;
    while (!XltIsNumEntry(Entry))
	Entry = XtParent(Entry);
    XtCallActionProc(NumEntry_DnArrow(Entry), "ArmAndActivate", event, params, *num_params);
}

static void
Help(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    static Widget Dialog = NULL;
    char HelpText[] =
    "BASIC-style expression evaluator which understands the following\n"
    "operators (all trigonometrical functions use degrees):\n\n"
    "+,-,*,/,^   Addition, subtraction, multiplication, division and power.\n"
    "()          Brackets (overrides operator precedence).\n"
    "ACS expr    Arccosine.\n"
    "ASN expr    Arcsine.\n"
    "ATN expr    Arctangent.\n"
    "COS expr    Cosine.\n"
    "EXP expr    Exponential power.\n"
    "FACT expr   Factorial (expr must be between 0 and 147).\n"
    "LN expr     Natural log (expr must be positive).\n"
    "LOG expr    Log to base 10 (expr must be positive).\n"
    "RND expr    Random number generator.\n"
    "            If expr<0, then expr is used to seed the random number generator.\n"
    "            If 0<=expr<=1, then returns a real number between 0 and 1.\n"
    "            If expr>1, then returns an integer between 1 and expr.\n"
    "SIN expr    Sine.\n"
    "SQRT expr   Square root.\n"
    "TAN expr    Tangent.\n"
    "TIME        Returns the number of seconds since 1st January 1970.\n"
    "\nderived from:\n"
    "calc.c - Floating-point calculator V1.00 (C) Richard K. Lloyd 1992\n"
    "rkl@csc.liv.ac.uk"
     ;

    if (Dialog == NULL)
    {
	Widget ParentShell;
	XmString string;

	ParentShell = w;
	while (!XtIsApplicationShell(ParentShell))
	    ParentShell = XtParent(ParentShell);
	Dialog = XmCreateInformationDialog(ParentShell, "HelpText", NULL, 0);
	string = XmStringCreateLtoR(HelpText, XmFONTLIST_DEFAULT_TAG);
	XtVaSetValues(Dialog,
		      XmNmessageString, string,
		      NULL);
	XmStringFree(string);
    }
    XtManageChild(Dialog);

}

/*
   Public functions
 */

Widget
XltCreateNumEntry(Widget parent,
		  char *name,
		  Arg *arglist,
		  Cardinal argCount)
{
    return XtCreateWidget(name, xrwsNumEntryWidgetClass, parent, arglist, argCount);
}

Widget
XltVaCreateNumEntry(Widget parent, char *name,...)
{
    Widget w;
    va_list args;

    va_start(args, name);
    w = XtVaCreateWidget(name, xrwsNumEntryWidgetClass, parent, args);
    va_end(args);
    return (w);
}

Widget
XltNumEntryGetChild(Widget W, unsigned char child)
{
    if (XltIsNumEntry(W))
    {
	switch (child)
	{
	case XltNUMENTRY_TEXT:
	    return NumEntry_TextField(W);
	    break;
	case XltNUMENTRY_LAB:
	    return NumEntry_Label(W);
	    break;
	case XltNUMENTRY_UP:
	    return NumEntry_UpArrow(W);
	    break;
	case XltNUMENTRY_DN:
	    return NumEntry_DnArrow(W);
	    break;
	default:
	    return NULL;
	    break;
	}
    }
    else
    {
	return NULL;
    }
}
