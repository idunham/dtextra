/**
 *
 * $Id: NodeButtonP.h,v 1.2 2001/06/09 18:38:57 amai Exp $
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
#ifndef _NODEBUTTONP_H
#define _NODEBUTTONP_H

#include <NodeButton.h>
#include <Xm/FormP.h>
#include <Xm/RowColumn.h>

typedef struct {
	Widget Button;
	Widget Arrow;
	Widget RowColumn;
	Widget Form;
	XtCallbackList ActivateCallback;
	XtCallbackList HelpCallback;
	XmString LabelString;
	Boolean InitMe;
} XltNodeButtonPart;

typedef struct _XltNodeButtonRec {
	CorePart core;
	CompositePart composite;
	ConstraintPart constraint;
	XmManagerPart manager;
	XmBulletinBoardPart bulletin_board;
	XmFormPart form;
	XltNodeButtonPart node_button;
} XltNodeButtonRec;

typedef struct {
	XtPointer extension;
} XltNodeButtonClassPart;

typedef struct _XltNodeButtonClassRec {
	CoreClassPart core_class;
	CompositeClassPart composite_class;
	ConstraintClassPart constraint_class;
	XmManagerClassPart manager_class;
	XmBulletinBoardClassPart bulletin_board_class;
	XmFormClassPart form_class;
	XltNodeButtonClassPart node_button_class;
} XltNodeButtonClassRec;

extern XltNodeButtonClassRec xrwsNodeButtonClassRec;

#define NodeButton_RowColumn(m) (((XltNodeButtonWidget)(m))->node_button.RowColumn)
#define NodeButton_Button(m) (((XltNodeButtonWidget)(m))->node_button.Button)
#define NodeButton_Arrow(m) (((XltNodeButtonWidget)(m))->node_button.Arrow)
#define NodeButton_LabelString(m) (((XltNodeButtonWidget)(m))->node_button.LabelString)
#define NodeButton_Form(m) ((XltNodeButtonWidget)(m))->node_button.Form)
#define NodeButton_InitMe(m) (((XltNodeButtonWidget)(m))->node_button.InitMe)

#endif
