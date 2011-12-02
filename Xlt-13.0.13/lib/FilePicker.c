#undef DEBUG
/**
 *
 * $Id: FilePicker.c,v 1.9 2004/07/16 21:34:22 rwscott Exp $
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

#ifdef DEBUG
#include <stdio.h>
#endif

#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>
#include <Xm/FileSB.h>

#include <FilePickerP.h>
#include <BubbleButton.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static const char rcsid[] = "$Id: FilePicker.c,v 1.9 2004/07/16 21:34:22 rwscott Exp $";

/*
   Widget methods, forward declarations
 */

static void destroy(Widget aw);
static void resize(Widget w);
static void class_initialize(void);
static void class_part_initialize(WidgetClass widget_class);
static void initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args);
static Boolean set_values(Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args);

/*
   Helper functions, forward declarations
 */

/*
static void Help(Widget w, XEvent *event, String *params, Cardinal *num_params);
*/

/*
   Widget default resources
 */

#define Offset(field) XtOffsetOf(XltFilePickerRec, file_picker.field)
static XtResource resources[] =
{
    {
	    XltNfileSelectionDialog, XltCFileSelectionDialog, XmRWidget,
	    sizeof(Widget), Offset(FileSelectionDialog),
	    XmRImmediate, (XtPointer)NULL
    },
    {
	    XmNvalue, XmCValue, XtRString,
	    sizeof(String), Offset(Value),
	    XtRImmediate, NULL
    },
#if 0
    {
	    XmNlabelString, XmCLabelString, XmRXmString,
	    sizeof(XmString), Offset(LabelString),
	    XmRImmediate, (XtPointer)NULL
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
#endif
};

static void FPGetValue(Widget wid, int resource_offset, XtArgVal *value);

static XmSyntheticResource syn_resources[] =
{
    {
	XmNvalue,
	sizeof(String), Offset(Value),
	FPGetValue, NULL
    },
};
#undef Offset

#if 0
static void Up(Widget w, XEvent *event, String *params, Cardinal *num_params);
static void Down(Widget w, XEvent *event, String *params, Cardinal *num_params);
#endif

#if 0
static XtActionsRec actions[] =
{
    {"Help", Help},
};
#endif

static char tm_table[] = "\
	";

/*
   Widget class record
 */
/* *INDENT-OFF* */
XltFilePickerClassRec xltFilePickerClassRec = {
	/* Core Class Part */
	{
	/* superclass            */ (WidgetClass) & xmFormClassRec,
	/* class_name            */ "XltFilePicker",
	/* widget_size           */ sizeof(XltFilePickerRec),
	/* class_initialize      */ class_initialize,
	/* class_part_initialize */ class_part_initialize,
	/* class_inited          */ False,
	/* initialize            */ initialize,
	/* initialize_hook       */ NULL,
	/* realize               */ XtInheritRealize /*realize*/,
	/* actions               */ NULL /*actions*/,
	/* num_actions           */ 0 /*XtNumber(actions)*/,
	/* resources             */ resources,
	/* num_resources         */ XtNumber(resources),
	/* xrm_class             */ NULLQUARK,
	/* compress_motion       */ True,
	/* compress_exposure     */ XtExposeCompressMultiple,
	/* compress_enterleave   */ True,
	/* visible_interest      */ True,
	/* destroy               */ destroy,
	/* resize                */ resize,
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
	/* destroy           */ destroy /*NULL*/,
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
	/* extension                    */ NULL
	},
	/* XmForm part */
	{
	/* extension                    */ NULL
	},
	/* FilePicker Class Part */
	{
	/* file_selection_dialog        */ NULL,
	/* extension                    */ NULL
	}
};
/* *INDENT-ON* */

WidgetClass xltFilePickerWidgetClass = (WidgetClass)&xltFilePickerClassRec;

/*
   Helper routines
 */

static void
BrowseOkCallback(Widget w, Widget fp, XmFileSelectionBoxCallbackStruct *cbs)
{
Widget FSB_TextField;
String Value;

#ifdef DEBUG
    printf("%s:BrowseOkCallback(%d) - %s %s\n", __FILE__, __LINE__, 
    	XtName(w), XtName(fp));
#endif
    FSB_TextField = XmFileSelectionBoxGetChild(w, XmDIALOG_TEXT);
    Value = XmTextFieldGetString(FSB_TextField);
    XmTextFieldSetString(FilePicker_TextField(fp), Value);
    XmTextFieldSetInsertionPosition(FilePicker_TextField(fp), XmTextFieldGetLastPosition(FilePicker_TextField(fp)));
    XtFree(Value);
    XtUnmanageChild(w);
}

/* ******************** */

static void
BrowseCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef DEBUG
    printf("%s:BrowseCallback(%d) - %s\n", __FILE__, __LINE__, XtName(w));
#endif
    if (FilePicker_FileSelectionDialog(XtParent(w)) == NULL)
    {
#ifdef DEBUG
	printf("%s:BrowseCallback(%d) - %s creating FSB\n", __FILE__, __LINE__, XtName(w));
#endif
    	if (FilePickerClass_FileSelectionDialog(XtParent(w)) == NULL)
    	{
    	Widget Shell = XtParent(w);

#ifdef DEBUG
	    printf("%s:BrowseCallback(%d) - %s creating class FSB\n", __FILE__, __LINE__, XtName(w));
#endif
	    while (!XtIsShell(Shell)) Shell = XtParent(Shell);
	    FilePickerClass_FileSelectionDialog(XtParent(w)) = XmCreateFileSelectionDialog(Shell, "FilePickerDialog", NULL, 0);
    	}
    	FilePicker_FileSelectionDialog(XtParent(w)) = FilePickerClass_FileSelectionDialog(XtParent(w));
    }
    XtRemoveAllCallbacks(FilePicker_FileSelectionDialog(XtParent(w)), XmNokCallback);
    XtRemoveAllCallbacks(FilePicker_FileSelectionDialog(XtParent(w)), XmNcancelCallback);
    XtAddCallback(FilePicker_FileSelectionDialog(XtParent(w)), XmNokCallback, (XtCallbackProc)BrowseOkCallback, XtParent(w));
    XtAddCallback(FilePicker_FileSelectionDialog(XtParent(w)), XmNcancelCallback, (XtCallbackProc)XtUnmanageChild, NULL);
    XtManageChild(FilePicker_FileSelectionDialog(XtParent(w)));
}

/* ******************** */

/*
   Widget methods
 */

static void
destroy(Widget aw)
{
#ifdef DEBUG
	printf("%s:destroy(%d) - %s\n", __FILE__, __LINE__,
		XtName(aw));
#endif
}

static void
resize(Widget w)
{
#ifdef DEBUG
    printf("%s:resize(%d) - %s %ix%i %ix%i\n", __FILE__, __LINE__,
		XtName(w),
		XtWidth(w), XtHeight(w),
		XtWidth(FilePicker_TextField(w)), XtHeight(FilePicker_TextField(w)));
#endif
    XtVaSetValues(FilePicker_TextField(w),
	XmNtopOffset, (int)((XtHeight(w) - XtHeight(FilePicker_TextField(w))) / 2),
	NULL);
    XtVaSetValues(FilePicker_Button(w),
	XmNtopOffset, (int)((XtHeight(w) - XtHeight(FilePicker_Button(w))) / 2),
	NULL);
    (XtClass(w)->core_class.superclass->core_class.resize) (w);
}

static void
class_initialize(void)
{
#ifdef DEBUG
    printf("%s:class_initialize(%d)\n", __FILE__, __LINE__);
#endif
    xltFilePickerClassRec.file_picker_class.file_selection_dialog = NULL;
}

static void 
class_part_initialize(WidgetClass widget_class)
{
XltFilePickerWidgetClass wc = (XltFilePickerWidgetClass) widget_class;
XltFilePickerWidgetClass sc = (XltFilePickerWidgetClass) (widget_class->core_class.superclass);

#ifdef DEBUG
    printf("%s:class_part_initialize(%d)\n", __FILE__, __LINE__);
#endif
    if (wc->file_picker_class.file_selection_dialog == XltInheritFileSelectionDialog)
    {
    	wc->file_picker_class.file_selection_dialog = sc->file_picker_class.file_selection_dialog;
    }
}

static void
initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
#ifdef DEBUG
    printf("%s:initialize(%d) - %s %ix%i\n", __FILE__, __LINE__, 
    	XtName(new_w),
    	XtWidth(new_w), XtHeight(new_w));
#endif

    FilePicker_Label(new_w) = XmCreateLabel(new_w, XtName(new_w), args, *num_args);
    FilePicker_TextField(new_w) = XmCreateTextField(new_w, "TextField", args, *num_args);
    FilePicker_Button(new_w) = XltCreateBubbleButton(new_w, "Browse", args, *num_args);
    if (XtHeight(new_w) == 0)
    {
    	XtHeight(new_w) = XtHeight(FilePicker_Label(new_w)) > XtHeight(FilePicker_TextField(new_w)) ? XtHeight(FilePicker_Label(new_w)) : XtHeight(FilePicker_TextField(new_w));
    	XtHeight(new_w) = XtHeight(new_w) > XtHeight(FilePicker_Button(new_w)) ? XtHeight(new_w) : XtHeight(FilePicker_Button(new_w));
    }
    XtVaSetValues(FilePicker_Label(new_w),
    	XmNalignment, XmALIGNMENT_BEGINNING,
    	XmNtopAttachment, XmATTACH_FORM,
    	XmNleftAttachment, XmATTACH_FORM,
    	XmNrightAttachment, XmATTACH_WIDGET,
    	XmNrightWidget, FilePicker_TextField(new_w),
    	XmNbottomAttachment, XmATTACH_FORM,
    	NULL);
    XtVaSetValues(FilePicker_Button(new_w),
    	XmNtopAttachment, XmATTACH_FORM,
    	XmNtopOffset, (int)((XtHeight(new_w) - XtHeight(FilePicker_Button(new_w))) / 2),
    	XmNrightAttachment, XmATTACH_FORM,
    	NULL);
    XtVaSetValues(FilePicker_TextField(new_w),
    	XmNtopAttachment, XmATTACH_FORM,
    	XmNtopOffset, (int)((XtHeight(new_w) - XtHeight(FilePicker_TextField(new_w))) / 2),
    	XmNrightAttachment, XmATTACH_WIDGET,
    	XmNrightWidget, FilePicker_Button(new_w),
    	NULL);
    if (FilePicker_Value(new_w))
    {
    	XmTextFieldSetString(FilePicker_TextField(new_w), FilePicker_Value(new_w));
	XmTextFieldSetInsertionPosition(FilePicker_TextField(new_w), XmTextFieldGetLastPosition(FilePicker_TextField(new_w)));
    }
    XtManageChild(FilePicker_Label(new_w));
    XtManageChild(FilePicker_Button(new_w));
    XtManageChild(FilePicker_TextField(new_w));
    XtAddCallback(FilePicker_Button(new_w), XmNactivateCallback, (XtCallbackProc)BrowseCallback, NULL);
    /*
    XtAddCallback(new_w, XmNhelpCallback, (XtCallbackProc)Help, NULL);
    */
}

static Boolean
set_values(Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
    Boolean refresh = False;

#ifdef DEBUG
    printf("%s:set_values(%d) - %s\n", __FILE__,__LINE__,
		XtName(new_w));
#endif
    if (FilePicker_Value(new_w) != FilePicker_Value(old))
    {
    	XmTextFieldSetString(FilePicker_TextField(new_w), FilePicker_Value(new_w));
	XmTextFieldSetInsertionPosition(FilePicker_TextField(new_w), XmTextFieldGetLastPosition(FilePicker_TextField(new_w)));
    }
    return (refresh);
}

/*
   Synthetic resource hooks
 */

static void 
FPGetValue(Widget wid, int resource_offset, XtArgVal *value)
{
	*value = (XtArgVal)XmTextFieldGetString(FilePicker_TextField(wid));
#ifdef DEBUG
	printf("%s:FPGetValue(%d) - %s\n", __FILE__, __LINE__,
		XtName(wid));
#endif
}

#if 0
static void 
Help(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
static Widget Dialog = NULL;
char HelpText[] =
   "Generic help for XltFilePicker\n"
   "Type in a file name, or click on Browse\n"
   "to search for one."
   ;

    if (Dialog == NULL)
    {
    Widget ParentShell;
    XmString string;

    	ParentShell = w;
    	while (!XtIsApplicationShell(ParentShell)) ParentShell = XtParent(ParentShell);
    	Dialog = XmCreateInformationDialog(ParentShell, "HelpText", NULL, 0);
    	XtUnmanageChild(XmMessageBoxGetChild(Dialog, XmDIALOG_HELP_BUTTON));
    	string = XmStringCreateLtoR(HelpText,XmFONTLIST_DEFAULT_TAG);
    	XtVaSetValues(Dialog,
    		XmNmessageString, string,
    		NULL);
    }
    XtManageChild(Dialog);
}
#endif

/*
   Public functions
 */

Widget
XltCreateFilePicker(Widget parent,
		   char *name,
		   Arg *arglist,
		   Cardinal argCount)
{
	return XtCreateWidget(name, xltFilePickerWidgetClass, parent, arglist, argCount);
}

String
XltFilePickerGetString(Widget w)
{
    return(XmTextFieldGetString(FilePicker_TextField(w)));
}

void
XltFilePickerSetString(Widget w, String string)
{
    XmTextFieldSetString(FilePicker_TextField(w), string);
}

Widget
XltFilePickerGetChild(Widget W, unsigned char child)
{
	if (XltIsFilePicker(W))
	{
	    switch (child)
	    {
	    case XltFILEPICKER_TEXT:
		    return FilePicker_TextField(W);
		    break;
	    case XltFILEPICKER_LAB:
		    return FilePicker_Label(W);
		    break;
	    case XltFILEPICKER_BUTTON:
		    return FilePicker_Button(W);
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
