/**
 *
 * $Id: NumEntryP.h,v 1.4 2001/06/09 18:38:57 amai Exp $
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
#ifndef _NUMENTRYP_H
#define _NUMENTRYP_H

#include <Xm/FormP.h>
#include "NumEntry.h"

typedef struct {
	Widget TextField;
	Widget Label;
	Widget RowColumn;
	Widget UpArrow;
	Widget DnArrow;
	String value;
	short columns;
	XtCallbackList ValueChangedCallback;
	XtCallbackList ActivateCallback;
	XtCallbackList LosingFocusCallback;
	XtCallbackList HelpCallback;
	XmString LabelString;
	XtIntervalId TimerId;
	int InitialDelay;
	int Delay;
	double result;
} XltNumEntryPart;

typedef struct _XltNumEntryRec {
	CorePart core;
	CompositePart composite;
	ConstraintPart constraint;
	XmManagerPart manager;
	XmBulletinBoardPart bulletin_board;
	XmFormPart form;
	XltNumEntryPart num_entry;
} XltNumEntryRec;

typedef struct {
	XtPointer extension;
} XltNumEntryClassPart;

typedef struct _XltNumEntryClassRec {
	CoreClassPart core_class;
	CompositeClassPart composite_class;
	ConstraintClassPart constraint_class;
	XmManagerClassPart manager_class;
	XmBulletinBoardClassPart bulletin_board_class;
	XmFormClassPart form_class;
	XltNumEntryClassPart num_entry_class;
} XltNumEntryClassRec;

extern XltNumEntryClassRec xrwsNumEntryClassRec;

#define NumEntry_RowColumn(m) (((XltNumEntryWidget)(m))->num_entry.RowColumn)
#define NumEntry_UpArrow(m) (((XltNumEntryWidget)(m))->num_entry.UpArrow)
#define NumEntry_DnArrow(m) (((XltNumEntryWidget)(m))->num_entry.DnArrow)
#define NumEntry_Label(m) (((XltNumEntryWidget)(m))->num_entry.Label)
#define NumEntry_TextField(m) (((XltNumEntryWidget)(m))->num_entry.TextField)
#define NumEntry_Value(m) (((XltNumEntryWidget)(m))->num_entry.value)
#define NumEntry_Columns(m) (((XltNumEntryWidget)(m))->num_entry.columns)
#define NumEntry_LabelString(m) (((XltNumEntryWidget)(m))->num_entry.LabelString)
#define NumEntry_TimerId(m) (((XltNumEntryWidget)(m))->num_entry.TimerId)
#define NumEntry_InitialDelay(m) (((XltNumEntryWidget)(m))->num_entry.InitialDelay)
#define NumEntry_Delay(m) (((XltNumEntryWidget)(m))->num_entry.Delay)
#define NumEntry_Result(m) (((XltNumEntryWidget)(m))->num_entry.result)

#endif /* _NUMENTRYP_H */
