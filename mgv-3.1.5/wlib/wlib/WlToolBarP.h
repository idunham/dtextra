/*
 * WlToolBarP.h
 *	Private header for the WlToolBar widget.
 *
 * Copyright (C) 1998 Eric A. Howe
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#ifndef WLIB_WLTOOLBARP_H
#define WLIB_WLTOOLBARP_H
MU_HID(wlib_wltoolbarp_h, "$Mu: wlib/wlib/WlToolBarP.h 1.24 1998/08/27 03:22:41 $")

#include <Xm/XmP.h>
#include <Xm/ManagerP.h>

#include <wlib/WlToolBar.h>

typedef struct {
	XtPointer	extension;
} WlToolBarClassPart;

typedef struct {
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	XmManagerClassPart	manager_class;
	WlToolBarClassPart	toolbar_class;
} WlToolBarClassRec, *WlToolBarWidgetClass;

extern WlToolBarClassRec wlToolBarClassRec;

typedef struct {
	/**
	 ** Resources.
	 **/

	unsigned char	orientation;
	Pixel		tip_foreground, tip_background;
	XmFontList	tip_fontlist;
	Dimension	tip_border_width;
	unsigned char	tip_position;
	Dimension	tip_distance;
	int		tip_delay;
	Dimension	group_spacing;
	Dimension	margin_height, margin_width;
	Dimension	spacing;
	Boolean		show_tips;
	Boolean		flatten_buttons;
	Dimension	button_shadow;
	Dimension	margin_extra;
	Boolean		use_color_icons;

	/**
	 ** Privates.
	 **/

	XtIntervalId interval_id;
	Boolean      interval_id_on;
	Boolean      tooltip_on;
	Widget	     tip_window, tooltip;
	Arg          enter_args[3];
	Arg          leave_args[3];

	/*
	 * These are used if we can't load a toolbar icon.
	 */
	Pixmap unknown, iunknown;

	/*
	 * These are semi-private, the only access is via
	 * a "convenience" function.
	 */
	WlToolBarImage	*images;
	int		n_images;

	void (*sizer)(Widget, XtWidgetGeometry *);
	void (*layout)(Widget);

	/*
	 * You might think that these could be placed in the ClassPart
	 * but that might not work in a multi-headed configuration where
	 * the heads are not the same size.
	 */
	int          root_width, root_height;
} WlToolBarPart;

typedef struct {
	CorePart	core;
	CompositePart	composite;
	ConstraintPart	constraint;
	XmManagerPart	manager;
	WlToolBarPart	toolbar;
} WlToolBarRec, *WlToolBarWidget;

typedef struct {
	Boolean		new_group;
	XmString	tooltip;
	String		icon_name;

	/*
	 * See the WlToolBar constraint_setvalues for an explanation.
	 */
	Boolean		break_loop;

	Boolean		free_pixmap;
} WlToolBarConstraintPart;

typedef struct {
	XmManagerConstraintPart	manager;
	WlToolBarConstraintPart	toolbar;
} WlToolBarConstraintRec, *WlToolBarConstraint;
 
#endif
