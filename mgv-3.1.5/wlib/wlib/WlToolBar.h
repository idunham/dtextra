/*
 * WlToolBar.h
 *	Public header for the WlToolBar widget.
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
#ifndef WLIB_WLTOOLBAR_H
#define WLIB_WLTOOLBAR_H
MU_HID(wlib_wltoolbar_h, "$Mu: wlib/wlib/WlToolBar.h 1.11 1998/08/27 03:22:41 $")

/*
 * Constraint resources.
 */

#define	XwlNtoolTip	"toolTip"
#define	XwlCToolTip	"ToolTip"

#define	XwlNnewGroup	"newGroup"
#define	XwlCNewGroup	"NewGroup"

#define	XwlNtoolBarIcon	"toolBarIcon"
#define	XwlCToolBarIcon	"ToolBarIcon"

/*
 * Widget resources.
 */

#define	XwlNshowToolTips	"showToolTips"
#define	XwlCShowToolTips	"ShowToolTips"

#define	XwlNtipForeground	"tipForeground"
#define	XwlCTipForeground	"TipForeground"

#define	XwlNtipBackground	"tipBackground"
#define	XwlCTipBackground	"TipBackground"

#define	XwlNtipFontList	"tipFontList"
#define	XwlCTipFontList	"tipFontList"

#define	XwlNtipBorderWidth	"tipBorderWidth"
#define	XwlCTipBorderWidth	"TipBorderWidth"

#define	XwlNtipPosition	"tipPosition"
#define	XwlCTipPosition	"TipPosition"
#define	XwlRTipPosition	"TipPosition"

#define	XwlNgroupSpacing	"groupSpacing"
#define	XwlCGroupSpacing	"GroupSpacing"

#define	XwlNtipDistance	"tipDistance"
#define	XwlCTipDistance	"TipDistance"

#define	XwlNtipDelay	"tipDelay"
#define	XwlCTipDelay	"TipDelay"

#define	XwlNflattenButtons	"flattenButtons"
#define	XwlCFlattenButtons	"FlattenButtons"

#define	XwlNbuttonShadowThickness	"buttonShadowThickness"
#define	XwlCButtonShadowThickness	"ButtonShadowThickness"

#define	XwlNbuttonMarginExtra	"buttonMarginExtra"
#define	XwlCButtonMarginExtra	"ButtonMarginExtra"

#define	XwlNuseColorIcons	"useColorIcons"
#define	XwlCUseColorIcons	"UseColorIcons"

typedef struct {
	char	*name;
	char	**xpm, **ixpm;
	int	width, height;
	unsigned char *bits, *ibits;
} WlToolBarImage;

enum {
	XwlTIP_BOTTOM_RIGHT,
	XwlTIP_BOTTOM_LEFT,
	XwlTIP_TOP_RIGHT,
	XwlTIP_TOP_LEFT
};

extern WidgetClass wlToolBarWidgetClass;

#define XwlIsWlToolBar(a) (XtIsSubclass(a, wlToolBarWidgetClass))

CDECLS_BEGIN
extern	void	WlToolBarSetInternalImages(Widget, WlToolBarImage *, size_t);
CDECLS_END

#endif
