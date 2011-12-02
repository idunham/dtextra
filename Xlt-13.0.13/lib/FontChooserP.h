/** 
 *
 * $Header: /cvsroot/xlt/Xlt/lib/FontChooserP.h,v 1.7 2001/06/09 18:38:57 amai Exp $
 *
 * Copyright © 1999-2001 by the LessTif developers.
 *
 * This file is part of the GNU LessTif Library.
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


#ifndef _XLT_FONTCHOOSER_P_H
#define _XLT_FONTCHOOSER_P_H

#include <Xm/XmP.h>
#include <Xm/Form.h>
#include <Xm/BulletinBP.h>

/* Define the instance part */
typedef struct {
	Boolean		show_demo;
	String		demo_string;
	String		count_string;
	String		field_names;
	Widget		*menus;
	String		*setting;
	String		font_name;
	String		pattern;
	Widget		demo, count;
	String		sort_criteria;
} XltFontChooserPart;

/* define the full instance record */
typedef struct _XltFontChooserRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmBulletinBoardPart bulletin_board;
    XmMessageBoxPart message_box;
    XltFontChooserPart font_chooser;
} XltFontChooserRec;

/* Define class part structure */
typedef struct {
    XtPointer extension;
} XltFontChooserClassPart;

/* Define the full class record */
typedef struct _XltFontChooserClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmBulletinBoardClassPart bulletin_board_class;
    XmMessageBoxClassPart messagebox_class;
    XltFontChooserClassPart font_chooser_class;
} XltFontChooserClassRec;

extern XltFontChooserClassRec xltFontChooserClassRec;

#define	FC_ShowDemo(w)		(((XltFontChooserWidget)(w))->font_chooser.show_demo)
#define	FC_DemoString(w)	(((XltFontChooserWidget)(w))->font_chooser.demo_string)
#define	FC_CountString(w)	(((XltFontChooserWidget)(w))->font_chooser.count_string)
#define	FC_FieldNames(w)	(((XltFontChooserWidget)(w))->font_chooser.field_names)
#define	FC_Menus(w)		(((XltFontChooserWidget)(w))->font_chooser.menus)
#define	FC_Setting(w)		(((XltFontChooserWidget)(w))->font_chooser.setting)
#define	FC_FontName(w)		(((XltFontChooserWidget)(w))->font_chooser.font_name)
#define	FC_Pattern(w)		(((XltFontChooserWidget)(w))->font_chooser.pattern)

#define	FC_DemoWidget(w)	(((XltFontChooserWidget)(w))->font_chooser.demo)
#define	FC_CountWidget(w)	(((XltFontChooserWidget)(w))->font_chooser.count)
#define	FC_SortCriteria(w)	(((XltFontChooserWidget)(w))->font_chooser.sort_criteria)

#ifndef MGR_NumChildren
#define MGR_NumChildren(m) \
	(((XmManagerWidget)(m))->composite.num_children)
#endif
#ifndef MGR_Children
#define MGR_Children(m) \
	(((XmManagerWidget)(m))->composite.children)
#endif
#ifndef	MB_DialogType
#define	MB_DialogType(w) \
    (((XmMessageBoxWidget)w)->message_box.dialog_type)
#endif

#endif /* _XLT_FONTCHOOSER_P_H */
