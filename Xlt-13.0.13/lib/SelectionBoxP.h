/**
 *
 * $Id: SelectionBoxP.h,v 1.2 2001/06/09 18:38:57 amai Exp $
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
#ifndef _SELECTIONBOXP_H
#define _SELECTIONBOXP_H

#include <SelectionBox.h>
#include <Xm/SelectioBP.h>

typedef struct {
	Boolean magic_scroll;
} XltSelectionBoxPart;

typedef struct _XltSelectionBoxRec {
	CorePart core;
	CompositePart composite;
	ConstraintPart constraint;
	XmManagerPart manager;
	XmBulletinBoardPart bulletin_board;
	XmSelectionBoxPart selection_box;
	XltSelectionBoxPart xlt_selection_box;
} XltSelectionBoxRec;

typedef struct {
	XtPointer extension;
} XltSelectionBoxClassPart;

typedef struct _XltSelectionBoxClassRec {
	CoreClassPart core_class;
	CompositeClassPart composite_class;
	ConstraintClassPart constraint_class;
	XmManagerClassPart manager_class;
	XmBulletinBoardClassPart bulletin_board_class;
	XmSelectionBoxClassPart selection_box_class;
	XltSelectionBoxClassPart xlt_selection_box_class;
} XltSelectionBoxClassRec;

extern XltSelectionBoxClassRec xltSelectionBoxClassRec;

#define SelectionBox_MagicScroll(m) (((XltSelectionBoxWidget)(m))->xlt_selection_box.magic_scroll)

#endif
