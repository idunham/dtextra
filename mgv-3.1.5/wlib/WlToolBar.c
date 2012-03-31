/*
 * WlToolBar.c
 *	Implementation of the WlToolBar widget.
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
#include <wlib/rcs.h>
MU_ID("$Mu: wlib/WlToolBar.c 1.51 1998/09/04 05:25:08 $")

#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>

#include <wlib/stdlib.h>
#include <wlib/WlToolBarP.h>
#include <wlib/sanity.h>
#include <wlib/wlibP.h>
#include <wlib/WlToolBar-unknown.xpm>
#include <wlib/WlToolBar-iunknown.xpm>
#include <wlib/WlToolBar-unknown.xbm>
#include <wlib/WlToolBar-iunknown.xbm>

static void class_init(void);
static void initialize(Widget, Widget, ArgList, Cardinal *);
static void destroy(Widget);
static void resize(Widget);
static void realize(Widget, XtValueMask *, XSetWindowAttributes *);
static Boolean set_values(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult query_geometry(Widget, XtWidgetGeometry *,
							XtWidgetGeometry *);
static XtGeometryResult geometry_manager(Widget, XtWidgetGeometry *,
							XtWidgetGeometry *);
static void change_managed(Widget);
static void constraint_init(Widget, Widget, ArgList, Cardinal *);
static Boolean constraint_setvalues(Widget, Widget, Widget, ArgList,Cardinal *);
static void constraint_destroy(Widget);

static void enter_action(Widget, XEvent *, String *, Cardinal *);
static void leave_action(Widget, XEvent *, String *, Cardinal *);

#define OFFSET(field) XtOffset(WlToolBarWidget, toolbar.field)
static XtResource resources[] =
{
	{
		XwlNbuttonMarginExtra, XwlCButtonMarginExtra,
		XmRDimension, sizeof(Dimension), OFFSET(margin_extra),
		XmRString, (XtPointer)"1"
	}, {
		XwlNbuttonShadowThickness, XwlCButtonShadowThickness,
		XmRDimension, sizeof(Dimension), OFFSET(button_shadow),
		XmRString, (XtPointer)"1"
	}, {
		XwlNflattenButtons, XwlCFlattenButtons,
		XmRBoolean, sizeof(Boolean), OFFSET(flatten_buttons),
		XmRImmediate, (XtPointer)True
	}, {
		XwlNgroupSpacing, XwlCGroupSpacing,
		XmRDimension, sizeof(Dimension), OFFSET(group_spacing),
		XmRString, (XtPointer)"10"
	}, {
		XmNmarginHeight, XmCMarginHeight,
		XmRDimension, sizeof(Dimension), OFFSET(margin_height),
		XmRString, (XtPointer)"0"
	}, {
		XmNmarginWidth, XmCMarginWidth,
		XmRDimension, sizeof(Dimension), OFFSET(margin_width),
		XmRString, (XtPointer)"0"
	}, {
		XmNorientation, XmCOrientation,
		XmROrientation, sizeof(unsigned char), OFFSET(orientation),
		XmRImmediate, (XtPointer)XmHORIZONTAL
	}, {
		XwlNshowToolTips, XwlCShowToolTips,
		XmRBoolean, sizeof(Boolean), OFFSET(show_tips),
		XmRImmediate, (XtPointer)True
	}, {
		XmNspacing, XmCSpacing,
		XmRDimension, sizeof(Dimension), OFFSET(spacing),
		XmRString, (XtPointer)"0"
	}, {
		XwlNtipBackground, XwlCTipBackground,
		XmRPixel, sizeof(Pixel), OFFSET(tip_background),
		XmRString, (XtPointer)"#ffff00"
	}, {
		XwlNtipBorderWidth, XwlCTipBorderWidth,
		XmRDimension, sizeof(Dimension), OFFSET(tip_border_width),
		XmRString, (XtPointer)"1"
	}, {
		XwlNtipDistance, XwlCTipDistance,
		XmRDimension, sizeof(Dimension), OFFSET(tip_distance),
		XmRString, (XtPointer)"10"
	}, {
		XwlNtipFontList, XwlCTipFontList,
		XmRFontList, sizeof(XmFontList), OFFSET(tip_fontlist),
		XmRImmediate, NULL
	}, {
		XwlNtipForeground, XwlCTipForeground,
		XmRPixel, sizeof(Pixel), OFFSET(tip_foreground),
		XmRString, (XtPointer)"#000000"
	}, {
		XwlNtipDelay, XwlCTipDelay,
		XmRInt, sizeof(int), OFFSET(tip_delay),
		XmRImmediate, (XtPointer)1000
	}, {
		XwlNtipPosition, XwlCTipPosition,
		XwlRTipPosition, sizeof(unsigned char), OFFSET(tip_position),
		XmRImmediate, (XtPointer)XwlTIP_BOTTOM_RIGHT
	}, {
		XwlNuseColorIcons, XwlCUseColorIcons,
		XmRBoolean, sizeof(Boolean), OFFSET(use_color_icons),
		XmRImmediate, (XtPointer)True
	}
};
#undef OFFSET

#define OFFSET(field) XtOffset(WlToolBarConstraint, toolbar.field)
static XtResource constraint_resources[] =
{
	{
		XwlNnewGroup, XwlCNewGroup,
		XmRBoolean, sizeof(Boolean), OFFSET(new_group),
		XmRImmediate, (XtPointer)False
	}, {
		XwlNtoolTip, XwlCToolTip,
		XmRXmString, sizeof(XmString), OFFSET(tooltip),
		XmRImmediate, NULL
	}, {
		XwlNtoolBarIcon, XwlCToolBarIcon,
		XmRString, sizeof(String), OFFSET(icon_name),
		XmRImmediate, NULL
	}
};
#undef OFFSET

static XtActionsRec actions[] = {
	{"EnterAction",	enter_action},
	{"LeaveAction",	leave_action}
};

static char translations[] =
"<Enter>: EnterAction()\n\
<Leave>: LeaveAction()";

WlToolBarClassRec wlToolBarClassRec = 
{
	{	/** Core **/
	/* superclass		*/	(WidgetClass)&xmManagerClassRec,
	/* class_name		*/	"WlToolBar",
	/* widget_size		*/	sizeof(WlToolBarRec),
	/* class_initialize	*/	class_init,
	/* class_initialize_hook*/	NULL,
	/* class_inited		*/	False,
	/* initialize		*/	initialize,
	/* initialize_hook	*/	NULL,
	/* realize		*/	realize,
	/* actions		*/	actions,
	/* num_actions		*/	XtNumber(actions),
	/* resources		*/	resources,
	/* num_resources	*/	XtNumber(resources),
	/* xrm_class		*/	NULLQUARK,
	/* compress_motion	*/	True,
	/* compress_exposures	*/	XtExposeCompressMultiple,
	/* compress_enterleave	*/	False,
	/* visible_interest	*/	False,
	/* destroy		*/	destroy,
	/* resize		*/	resize,
	/* expose		*/	XtInheritExpose,
	/* set_values		*/	set_values,
	/* set_values_hook	*/	NULL,
	/* set_values_almost	*/	XtInheritSetValuesAlmost,
	/* get_values_hook	*/	NULL,
	/* accept_focus		*/	XtInheritAcceptFocus,
	/* version		*/	XtVersion,
	/* callback_private	*/	NULL,
	/* tm_table		*/	translations,
	/* query_geometry	*/	query_geometry,	
	/* display_accelerator	*/	XtInheritDisplayAccelerator,
	/* extension		*/	NULL,
	},

	{	/** Composite **/
	/* geometry_manager	*/	geometry_manager,
	/* change_managed	*/	change_managed,
	/* insert_child		*/	XtInheritInsertChild,
	/* delete_child		*/	XtInheritDeleteChild,
	/* extension            */	NULL,
	},

	{	/** Constraint **/
	/* constraint resource list */	constraint_resources,
	/* size of "                */	XtNumber(constraint_resources),
	/* size of constraint rec   */	sizeof(WlToolBarConstraintRec),
	/* initialization           */	constraint_init,
	/* destroy                  */	constraint_destroy,
	/* setvalues                */	constraint_setvalues,
	/* extension                */	NULL,
	},

	{	/** XmManager **/
	/* translations            */	NULL,
	/* syn_resources           */	NULL,
	/* num_syn_resources       */	0,
	/* syn_constraint_resources*/	NULL,
	/* num_....                */	0,
	/* parent_process          */	XmInheritParentProcess,
	/* extension               */	NULL,
	},

	{	/** WlToolBar **/
	/* extension		*/	NULL,
	}
};

WidgetClass wlToolBarWidgetClass = (WidgetClass)&wlToolBarClassRec;

/*
 * Misc. macros for nicer widget access.  Pretty hideous but better
 * than the alternative.
 */
#define	comp_children(w) \
	(((CompositeRec *)w)->composite.children)
#define	comp_num_children(w) \
	((int)((CompositeRec *)w)->composite.num_children)
#define	core_background(w) \
	((w)->core.background_pixel)
#define	core_border_width(w) \
	((w)->core.border_width)
#define	core_colormap(w) \
	((w)->core.colormap)
#define	core_depth(w) \
	((w)->core.depth)
#define	core_height(w) \
	((w)->core.width)
#define	core_width(w) \
	((w)->core.width)
#define man_traversal_on(w) \
	(((XmManagerRec *)w)->manager.traversal_on)
#define tb_button_shadow(w) \
	(((WlToolBarWidget)w)->toolbar.button_shadow)
#define tb_flatten_buttons(w) \
	(((WlToolBarWidget)w)->toolbar.flatten_buttons)
#define tb_group_spacing(w) \
	(((WlToolBarWidget)w)->toolbar.group_spacing)
#define tb_margin_extra(w) \
	(((WlToolBarWidget)w)->toolbar.margin_extra)
#define tb_margin_height(w) \
	(((WlToolBarWidget)w)->toolbar.margin_height)
#define tb_margin_width(w) \
	(((WlToolBarWidget)w)->toolbar.margin_width)
#define tb_orientation(w) \
	(((WlToolBarWidget)w)->toolbar.orientation)
#define tb_show_tips(w) \
	(((WlToolBarWidget)w)->toolbar.show_tips)
#define tb_spacing(w) \
	(((WlToolBarWidget)w)->toolbar.spacing)
#define tb_tip_background(w) \
	(((WlToolBarWidget)w)->toolbar.tip_background)
#define tb_tip_border_width(w) \
	(((WlToolBarWidget)w)->toolbar.tip_border_width)
#define tb_tip_delay(w) \
	(((WlToolBarWidget)w)->toolbar.tip_delay)
#define tb_tip_distance(w) \
	(((WlToolBarWidget)w)->toolbar.tip_distance)
#define tb_tip_fontlist(w) \
	(((WlToolBarWidget)w)->toolbar.tip_fontlist)
#define tb_tip_foreground(w) \
	(((WlToolBarWidget)w)->toolbar.tip_foreground)
#define tb_tip_position(w) \
	(((WlToolBarWidget)w)->toolbar.tip_position)
#define tb_tip_window(w) \
	(((WlToolBarWidget)w)->toolbar.tip_window)
#define tb_tooltip(w) \
	(((WlToolBarWidget)w)->toolbar.tooltip)
#define tb_tooltip_on(w) \
	(((WlToolBarWidget)w)->toolbar.tooltip_on)
#define tb_use_color_icons(w) \
	(((WlToolBarWidget)w)->toolbar.use_color_icons)

#define tb_images(w) \
	(((WlToolBarWidget)w)->toolbar.images)
#define tb_interval_id(w) \
	(((WlToolBarWidget)w)->toolbar.interval_id)
#define tb_interval_id_on(w) \
	(((WlToolBarWidget)w)->toolbar.interval_id_on)
#define tb_iunknown(w) \
	(((WlToolBarWidget)w)->toolbar.iunknown)
#define tb_layout(w) \
	(((WlToolBarWidget)w)->toolbar.layout)
#define tb_n_images(w) \
	(((WlToolBarWidget)w)->toolbar.n_images)
#define tb_root_height(w) \
	(((WlToolBarWidget)w)->toolbar.root_height)
#define tb_root_width(w) \
	(((WlToolBarWidget)w)->toolbar.root_width)
#define tb_sizer(w) \
	(((WlToolBarWidget)w)->toolbar.sizer)
#define tb_unknown(w) \
	(((WlToolBarWidget)w)->toolbar.unknown)

#define tb_enter_args(w) (((WlToolBarWidget)w)->toolbar.enter_args)
#define	tb_enter_args_n(w)	3
#define tb_leave_args(w) (((WlToolBarWidget)w)->toolbar.leave_args)
#define	tb_leave_args_n(w)	3

#define	tbc_tooltip(x) \
	(((WlToolBarConstraint)((x)->core.constraints))->toolbar.tooltip)
#define	tbc_newgroup(x) \
	(((WlToolBarConstraint)((x)->core.constraints))->toolbar.new_group)
#define	tbc_break_loop(x) \
	(((WlToolBarConstraint)((x)->core.constraints))->toolbar.break_loop)
#define	tbc_icon_name(x) \
	(((WlToolBarConstraint)((x)->core.constraints))->toolbar.icon_name)
#define	tbc_free_pixmap(x) \
	(((WlToolBarConstraint)((x)->core.constraints))->toolbar.free_pixmap)

#define	super_realize \
	(wlToolBarWidgetClass->core_class.superclass->core_class.realize)

/*
 * The event mask for tooltips.
 */
#define	MASK	(EnterWindowMask | LeaveWindowMask)

/**
 ** Internals.
 **/

static char *
fixup(char *s)
{
	char *p;
	int   i;

	for(p = s, i = 0; *p != '\0'; ++p, ++i) {
		if(*p == '-')
			s[i] = '_';
		else
			s[i] = tolower(*p);
	}
	return s;
}

static void
tip_post(XtPointer closure, XtIntervalId *unused)
{
	Widget       tb  = (Widget)closure;
	Window       win = None;
	Window       root;
	Widget       w;
	int          rx, ry, x, y, dx, dy;
	unsigned int mask, width, height, bwidth, depth;
	Dimension    lw, lh, tw, th;
	Arg          a[2];
	Cardinal     n;
	USEUP(unused);

	if(!tb_show_tips(tb))
		return;

	if(!XQueryPointer(XtDisplay(tb), XtWindow(tb), &root, &win,
							&rx, &ry, &x, &y, &mask)
	|| win == None)
		return;

	w = XtWindowToWidget(XtDisplay(tb), win);
	if(w == NULL
	|| XtParent(w) != tb
	|| tbc_tooltip(w) == NULL)
		return;

	dx = rx - x;
	dy = ry - y;

	if(!XGetGeometry(XtDisplay(tb), win, &root, &x, &y,
					&width, &height, &bwidth, &depth))
		return;

	n = 0;
	XtSetArg(a[n], XmNlabelString, tbc_tooltip(w)); ++n;
	wlSetValues(tb_tooltip(tb), a, n);

	n = 0;
	XtSetArg(a[n], XmNwidth,  &lw);	++n;
	XtSetArg(a[n], XmNheight, &lh);	++n;
	wlGetValues(tb_tooltip(tb), a, n);

	switch(tb_orientation(tb)) {
	case XmHORIZONTAL:
		switch(tb_tip_position(tb)) {
		case XwlTIP_BOTTOM_LEFT:
			dx += width/2 - lw;
			dy += height + tb_tip_distance(tb);
			break;
		case XwlTIP_BOTTOM_RIGHT:
			dx += width/2;
			dy += height + tb_tip_distance(tb);
			break;
		case XwlTIP_TOP_LEFT:
			dx += width/2 - lw;
			dy -= lh + tb_tip_distance(tb);
			break;
		case XwlTIP_TOP_RIGHT:
			dx += width/2;
			dy -= lh + tb_tip_distance(tb);
			break;
		default:
			/* can't happen */
			return;
		}
		break;
	case XmVERTICAL:
		switch(tb_tip_position(tb)) {
		case XwlTIP_BOTTOM_LEFT:
			dx -= lw + tb_tip_distance(tb);
			dy += height/2;
			break;
		case XwlTIP_BOTTOM_RIGHT:
			dx += width + tb_tip_distance(tb);
			dy += height/2;
			break;
		case XwlTIP_TOP_LEFT:
			dx -= lw + tb_tip_distance(tb);
			dy += height/2 - lh;
			break;
		case XwlTIP_TOP_RIGHT:
			dx += width + tb_tip_distance(tb);
			dy += height/2 - lh;
			break;
		default:
			/* can't happen */
			return;
		}
		break;
	default:
		/* can't happen */
		return;
	}

	/*
	 * Adjust for sanity.
	 */
	n = 0;
	XtSetArg(a[n], XmNwidth,  &tw);	++n;
	XtSetArg(a[n], XmNheight, &th);	++n;
	wlGetValues(tb_tip_window(tb), a, n);
	x += dx; y += dy;
	if(x < 0)
		x = 0;
	else if(x + tw > tb_root_width(tb))
		x = tb_root_width(tb) - tw;
	if(y < 0)
		y = 0;
	else if(y + th > tb_root_height(tb))
		y = tb_root_height(tb) - th;

	n = 0;
	XtSetArg(a[n], XmNx, (Position)x);	++n;
	XtSetArg(a[n], XmNy, (Position)y);	++n;
	wlSetValues(tb_tip_window(tb), a, n);

	XtPopup(tb_tip_window(tb), XtGrabNone);
	tb_tooltip_on(tb)     = True;
	tb_interval_id_on(tb) = False;
}

static void
tip_off(Widget w)
{
	if(tb_tooltip_on(w))
		XtPopdown(tb_tip_window(w));
	if(tb_interval_id_on(w))
		XtRemoveTimeOut(tb_interval_id(w));

	tb_tooltip_on(w)     = False;
	tb_interval_id_on(w) = False;
}

static void
tip_on(Widget w)
{
	tip_off(w);
	tb_interval_id(w) = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			tb_tip_delay(w), tip_post, (XtPointer)w);
	tb_interval_id_on(w) = True;
}

static void
tip_replace(Widget w)
{
	XtPopdown(tb_tip_window(w));
	tip_post((XtPointer)w, NULL);
}

static void
enter_leave(Widget w, XtPointer closure, XEvent *event, Boolean *cont)
{
	Widget          tb = (Widget)closure;
	XCrossingEvent *ev = (XCrossingEvent *)event;
	USEUP(w);

	*cont = True;

	if(ev->detail != NotifyNonlinear
	&& ev->detail != NotifyNonlinearVirtual)
		return;

	if(ev->type == EnterNotify) {
		if(!tb_interval_id_on(tb))
			tip_replace(tb);
	}
	else {
		tip_off(tb);
	}
}

static void
horizontal_size(Widget w, XtWidgetGeometry *g)
{
	int    i, kids;
	Widget kid;

	g->width  = 2*tb_margin_width(w);
	g->height = tb_margin_height(w);

	for(i = 0, kids = 0; i < comp_num_children(w); ++i) {
		kid = comp_children(w)[i];
		if(!XtIsManaged(kid))
			continue;

		++kids;
		if(core_height(kid) + 2*tb_margin_height(w) > g->height)
			g->height = core_height(kid) + 2*tb_margin_height(w);

		g->width += core_width(kid);
		g->width += (tbc_newgroup(kid) && kids != 1)
			? tb_group_spacing(w)
			: tb_spacing(w);
	}
}

static void
vertical_size(Widget w, XtWidgetGeometry *g)
{
	int    i, kids;
	Widget kid;

	g->width  = tb_margin_width(w);
	g->height = 2*tb_margin_height(w);

	for(i = 0, kids = 0; i < comp_num_children(w); ++i) {
		kid = comp_children(w)[i];
		if(!XtIsManaged(kid))
			continue;

		++kids;
		if(core_width(kid) + 2*tb_margin_width(w) > g->width)
			g->width = core_width(kid) + 2*tb_margin_width(w);

		g->height += core_height(kid);
		g->height += (tbc_newgroup(kid) && kids != 1)
			? tb_group_spacing(w)
			: tb_spacing(w);
	}
}

static void
find_biggest(Widget w, int start, int check, Dimension *wdt, Dimension *hgt)
{
	XtWidgetGeometry g;
	Dimension        max, width, height;
	int              i, checked;
	Widget           kid;

	max = 0;
	for(i = start, checked = 0; i < comp_num_children(w); ++i) {
		kid = comp_children(w)[i];
		if(!XtIsManaged(kid))
			continue;

		memset((void *)&g, '\0', sizeof(g));
		XtQueryGeometry(kid, NULL, &g);

		width  = (g.request_mode & CWWidth)
			? g.width
			: core_width(kid);
		height = (g.request_mode & CWHeight)
			? g.height
			: core_height(kid);

		if(width > max)
			max = width;
		if(height > max)
			max = height;

		if(check != 0
		&& ++checked == check)
			break;
	}

	if(wdt != NULL)
		*wdt = max;
	if(hgt != NULL)
		*hgt = max;
}

static void
horizontal_layout(Widget w)
{
	Dimension width, height;
	Position  x, y;
	Widget    kid;
	int       i, kids;

	find_biggest(w, 0, 0, &width, &height);

	x = tb_margin_width(w);
	y = tb_margin_height(w);

	for(i = kids = 0; i < comp_num_children(w); ++i) {
		if(!XtIsManaged(w))
			continue;

		kid = comp_children(w)[i];
		++kids;

		if(kids != 1) {
			x += tbc_newgroup(kid)
				? tb_group_spacing(w)
				: tb_spacing(w);
		}

		XtConfigureWidget(kid, x, y, width, height, 
				core_border_width(kid));

		x += core_width(kid);
	}
}

static void
vertical_layout(Widget w)
{
	Dimension width, height;
	Position  x, y;
	Widget    kid;
	int       i, kids;

	find_biggest(w, 0, 0, &width, &height);

	x = tb_margin_width(w);
	y = tb_margin_height(w);

	for(i = kids = 0; i < comp_num_children(w); ++i) {
		if(!XtIsManaged(w))
			continue;

		kid = comp_children(w)[i];
		++kids;

		if(kids != 1) {
			y += tbc_newgroup(kid)
				? tb_group_spacing(w)
				: tb_spacing(w);
		}

		XtConfigureWidget(kid, x, y, width, height, 
				core_border_width(kid));

		y += core_width(kid);
	}
}

static void
layout(Widget w)
{
	XtWidgetGeometry g;

	tb_layout(w)(w);
	memset((void *)&g, '\0', sizeof(g));
	tb_sizer(w)(w, &g);
	XtMakeResizeRequest(w, g.width, g.height, NULL, NULL);
}

static void
set_geom_methods(Widget w)
{
	if(tb_orientation(w) == XmHORIZONTAL) {
		tb_sizer(w)  = horizontal_size;
		tb_layout(w) = horizontal_layout;
	}
	else {
		tb_sizer(w)  = vertical_size;
		tb_layout(w) = vertical_layout;
	}
}

#define	BUTTON_MASK ( \
	Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask \
)

static void
shadow_trick(Widget w, XtPointer closure, XEvent *event, Boolean *cont)
{
	Widget          tb = (Widget)closure;
	XCrossingEvent *ev = (XCrossingEvent *)event;

	*cont = True;

	if((ev->state & BUTTON_MASK)
	|| !tb_flatten_buttons(tb))
		return;

	if(ev->type == EnterNotify)
		XtSetValues(w, tb_enter_args(tb), tb_enter_args_n(tb));
	else
		XtSetValues(w, tb_leave_args(tb), tb_leave_args_n(tb));
}

/**
 ** Actions.
 **/

static void
enter_action(Widget w, XEvent *ev, String *argv, Cardinal *argc)
{
	USEUP(argv); USEUP(argc);

	if(tb_show_tips(w)) {
		switch(ev->xcrossing.detail) {
		case NotifyVirtual:		tip_on(w);	break;
		case NotifyNonlinearVirtual:	tip_on(w);	break;
		case NotifyInferior:		tip_off(w);	break;
		}
	}
}

static void
leave_action(Widget w, XEvent *ev, String *argv, Cardinal *argc)
{
	USEUP(argv); USEUP(argc);

	if(tb_show_tips(w)) {
		switch(ev->xcrossing.detail) {
		case NotifyVirtual:		tip_off(w);	break;
		case NotifyNonlinearVirtual:	tip_off(w);	break;
		case NotifyInferior:		tip_on(w);	break;
		}
	}
}

/**
 ** Type converters.
 **/

static struct {
	char         *string;
	unsigned char pos;
} pos_names[] = {
	{"bottom_left",		XwlTIP_BOTTOM_LEFT},
	{"tip_bottom_left",	XwlTIP_BOTTOM_LEFT},
	{"wltip_bottom_left",	XwlTIP_BOTTOM_LEFT},
	{"xwltip_bottom_left",	XwlTIP_BOTTOM_LEFT},
	{"xmtip_bottom_left",	XwlTIP_BOTTOM_LEFT},

	{"bottom_right",	XwlTIP_BOTTOM_RIGHT},
	{"tip_bottom_right",	XwlTIP_BOTTOM_RIGHT},
	{"wltip_bottom_right",	XwlTIP_BOTTOM_RIGHT},
	{"xwltip_bottom_left",	XwlTIP_BOTTOM_LEFT},
	{"xmtip_bottom_right",	XwlTIP_BOTTOM_RIGHT},

	{"top_left",		XwlTIP_TOP_LEFT},
	{"tip_top_left",	XwlTIP_TOP_LEFT},
	{"wltip_top_left",	XwlTIP_TOP_LEFT},
	{"xwltip_bottom_left",	XwlTIP_BOTTOM_LEFT},
	{"xmtip_top_left",	XwlTIP_TOP_LEFT},

	{"top_right",		XwlTIP_TOP_RIGHT},
	{"tip_top_right",	XwlTIP_TOP_RIGHT},
	{"wltip_top_right",	XwlTIP_TOP_RIGHT},
	{"xwltip_top_right",	XwlTIP_TOP_RIGHT},
	{"xmtip_top_right",	XwlTIP_TOP_RIGHT}
};
#define	N_POS	((int)(sizeof(pos_names)/sizeof(pos_names[0])))

static Boolean
string2tippos(Display *dpy, XrmValue *a, Cardinal *n,
				XrmValue *from, XrmValue *to, XtPointer *data)
{
static	unsigned char pos;
	String        position;
	Boolean       found;
	int           i;
	USEUP(data); USEUP(a);

	if(*n != 0) {
		XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
			"string2tippos", "wrongParameters",
			"ResourceError",
			"string2tippos needs no arguments",
			NULL, NULL);
	}

	if(to->addr != NULL && to->size < sizeof(unsigned char)) {
		to->size = sizeof(unsigned char);
		return False;
	}

	position = fixup(XtNewString(from->addr));
	for(i = 0, found = False; i < N_POS; ++i) {
		if(strcmp(position, pos_names[i].string) != 0)
			continue;
		found = True;
		pos   = pos_names[i].pos;
		break;
	}

	if(!found) {
		XtDisplayStringConversionWarning(dpy, from->addr,
							XwlRTipPosition);
		return False;
	}

	if(to->addr == NULL)
		to->addr = (XtPointer)&pos;
	else
		*(unsigned char *)to->addr = pos;
	to->size = sizeof(unsigned char);

	return True;
}

static void
get_icons_int(Widget w, char *name, Pixel fg, Pixel bg, Pixmap *p, Pixmap *ip)
{
	WlToolBarImage *e;
	int i;

	*p = *ip = None;

	for(i = 0, e = NULL; i < tb_n_images(w); ++i) {
		if(strcmp(tb_images(w)[i].name, name) != 0)
			continue;
		e = &tb_images(w)[i];
		break;
	}
	if(e == NULL)
		return;

	if(tb_use_color_icons(w)) {
		*p  = wlp_xpm_from_data(w, XtDisplay(w), bg, e->xpm);
		*ip = wlp_xpm_from_data(w, XtDisplay(w), bg, e->ixpm);
	}

	if(*p == None)
		*p  = wlp_xbm_from_data(w, XtDisplay(w), fg, bg,
					(char *)e->bits, e->width, e->height);
	if(*ip == None)
		*ip = wlp_xbm_from_data(w, XtDisplay(w), fg, bg,
					(char *)e->ibits, e->width, e->height);
}

static void
get_icons_ext(Widget w, char *name, Pixel fg, Pixel bg, Pixmap *p, Pixmap *ip)
{
	int  len     = strlen(name);
	char *fname  = XtMalloc(len + 5);
	char *ifname = XtMalloc(len + 5 + 2);
	char *ext    = fname  + len + 1;
	char *iext   = ifname + len + 1 + 2;
	USEUP(fg);

	strcat(strcpy(fname,  name), ".");
	strcat(strcpy(ifname, name), "-i.");

	if(tb_use_color_icons(w)) {
		strcpy(ext,  "xpm");
		*p = wlp_xpm_from_file(w, XtDisplay(w), bg, fname);

		strcpy(iext, "xpm");
		*ip = wlp_xpm_from_file(w, XtDisplay(w), bg, ifname);
	}

	if(*p == None) {
		strcpy(ext, "xbm");
		*p = wlp_xbm_from_file(w, XtDisplay(w), fname);
	}
	if(*ip == None) {
		strcpy(iext, "xbm");
		*ip = wlp_xbm_from_file(w, XtDisplay(w), ifname);
	}

	XtFree(fname);
	XtFree(ifname);
}

#define	INTERNAL "INTERNAL::"
#define	N_INTERNAL	(sizeof(INTERNAL) - 1)

static void
get_icons(Widget w, char *name, Pixel fg, Pixel bg, Pixmap *p, Pixmap *ip)
{
	if(strncmp(name, INTERNAL, N_INTERNAL) == 0)
		get_icons_int(w, name + N_INTERNAL, fg, bg, p, ip);
	else
		get_icons_ext(w, name, fg, bg, p, ip);
}

static void
get_unknowns(Widget w, Pixel fg, Pixel bg, Pixmap *p, Pixmap *ip)
{
	*p = *ip = None;

	if(tb_use_color_icons(w)) {
		*p  = wlp_xpm_from_data(w, XtDisplay(w), bg, tb_unknown_xpm);
		*ip = wlp_xpm_from_data(w, XtDisplay(w), bg, tb_iunknown_xpm);
	}

	if(*p == None)
		*p  = wlp_xbm_from_data(w, XtDisplay(w), fg, bg,
					(char *)tb_unknown_bits,
					tb_unknown_width, tb_unknown_height);
	if(*ip == None)
		*ip = wlp_xbm_from_data(w, XtDisplay(w), fg, bg,
					(char *)tb_iunknown_bits,
					tb_iunknown_width, tb_iunknown_height);
}

static void
change_flattness(Widget w)
{
	Widget kid;
	int    i;

	for(i = 0; i < comp_num_children(w); ++i) {
		kid = comp_children(w)[i];
		if(!XmIsPushButton(kid) && !XmIsPushButtonGadget(w))
			continue;
		if(tb_flatten_buttons(w))
			XtSetValues(kid, tb_leave_args(w), tb_leave_args_n(w));
		else
			XtSetValues(kid, tb_enter_args(w), tb_enter_args_n(w));
	}
}

/**
 ** Methods: Core
 **/

static void
class_init(void)
{
	XtSetTypeConverter(XmRString, XwlRTipPosition, string2tippos,
				NULL, 0, XtCacheAll, NULL);
}

static void
initialize(Widget inw, Widget outw, ArgList argv, Cardinal *argc)
{
	Arg      a[4];
	Cardinal n;
	USEUP(inw); USEUP(argv); USEUP(argc);

	tb_images(outw)   = NULL;
	tb_n_images(outw) = 0;

	tb_unknown(outw) = tb_iunknown(outw) = None;

	tb_root_width(outw)  = WidthOfScreen(XtScreen(outw));
	tb_root_height(outw) = HeightOfScreen(XtScreen(outw));

	tb_interval_id_on(outw) = False;
	tb_tooltip_on(outw)     = False;
	man_traversal_on(outw)  = False;

	n = 0;
	XtSetArg(a[n], XmNallowShellResize, True);			++n;
	XtSetArg(a[n], XmNborderWidth,      tb_tip_border_width(outw));	++n;
	XtSetArg(a[n], XmNborderColor,      tb_tip_foreground(outw));	++n;
	XtSetArg(a[n], XmNbackground,       tb_tip_background(outw));	++n;
	ARGSANITY(a, n);

	tb_tip_window(outw) = XtCreatePopupShell("TipWindow",
					overrideShellWidgetClass, outw, a, n);

	n = 0;
	XtSetArg(a[n], XmNbackground, tb_tip_background(outw));	++n;
	XtSetArg(a[n], XmNforeground, tb_tip_foreground(outw));	++n;
	if(tb_tip_fontlist(outw) != NULL) {
		XtSetArg(a[n], XmNfontList, tb_tip_fontlist(outw));	++n;
	}
	ARGSANITY(a, n);

	tb_tooltip(outw) = XtCreateManagedWidget("Tip", xmLabelWidgetClass,
					tb_tip_window(outw), a, n);

	XtSetArg(tb_enter_args(outw)[0], XmNshadowThickness,
				tb_button_shadow(outw));
	XtSetArg(tb_enter_args(outw)[1], XmNmarginWidth,
				tb_margin_extra(outw));
	XtSetArg(tb_enter_args(outw)[2], XmNmarginHeight,
				tb_margin_extra(outw));

	XtSetArg(tb_leave_args(outw)[0], XmNshadowThickness, 0);
	XtSetArg(tb_leave_args(outw)[1], XmNmarginWidth,
				tb_button_shadow(outw) + tb_margin_extra(outw));
	XtSetArg(tb_leave_args(outw)[2], XmNmarginHeight,
				tb_button_shadow(outw) + tb_margin_extra(outw));

	set_geom_methods(outw);
}

static void
destroy(Widget w)
{
	if(tb_tip_window(w) != NULL)
		XtDestroyWidget(tb_tip_window(w));
	wl_free_pixmap(XtDisplay(w), tb_unknown(w));
	wl_free_pixmap(XtDisplay(w), tb_iunknown(w));
}

static void
resize(Widget w)
{
	layout(w);
}

static void
realize(Widget w, XtValueMask *mask, XSetWindowAttributes *attr)
{
#	define CANT_LOAD "Could not load icon %s%s for %s"
	int      i;
	Widget   kid;
	Pixel    fg, bg;
	Pixmap   p, ip;
	Arg      c[2], pix[4];
	Cardinal n_c, n_pix;

	/*
	 * Wimp out and let the super-class make the window.  We really
	 * don't want to make our own window, we just want to be around
	 * when it comes up so that we can get the colormap and such.
	 */
	super_realize(w, mask, attr);

	/*
	 * Now that we have a window, we can go about building the various
	 * icons for the child buttons.  We need these little tricks to get
	 * the correct background colors for the XPM versions.
	 */

	n_c = 0;
	XtSetArg(c[n_c], XmNforeground, &fg);	++n_c;
	XtSetArg(c[n_c], XmNbackground, &bg);	++n_c;

	for(i = 0; i < comp_num_children(w); ++i) {
		kid = comp_children(w)[i];
		if(tbc_icon_name(kid) == NULL)
			continue;

		wlGetValues(kid, c, n_c);

		if(i == 0)
			get_unknowns(w, fg, bg, &tb_unknown(w),&tb_iunknown(w));

		if(!XmIsLabel(kid) && !XmIsLabelGadget(kid))
			continue;

		get_icons(w, tbc_icon_name(kid), fg, bg, &p, &ip);

		if(p == None) {
			String buf = XtMalloc(strlen(CANT_LOAD)
						- 2 - 2 - 2 + 1
						+ strlen(tbc_icon_name(kid))
						+ strlen(XtName(kid)));
			sprintf(buf, CANT_LOAD, tbc_icon_name(kid), "",
						XtName(kid));
			XtAppWarningMsg(XtWidgetToApplicationContext(w),
				XtName(kid), "missing image", "ResourceError",
				buf, NULL, NULL);
			XtFree(buf);
			p = tb_unknown(w);
		}
		if(ip == None) {
			String buf = XtMalloc(strlen(CANT_LOAD)
						- 2 - 2 + 1
						+ strlen(tbc_icon_name(kid))
						+ strlen(XtName(kid)));
			sprintf(buf, CANT_LOAD, tbc_icon_name(kid), "-i",
						XtName(kid));
			XtAppWarningMsg(XtWidgetToApplicationContext(w),
				XtName(kid), "missing image", "ResourceError",
				buf, NULL, NULL);
			ip = tb_iunknown(w);
		}

		n_pix = 0;
		XtSetArg(pix[n_pix], XmNrecomputeSize, True);		++n_pix;
		XtSetArg(pix[n_pix], XmNlabelType, XmPIXMAP);		++n_pix;
		XtSetArg(pix[n_pix], XmNlabelPixmap, p);		++n_pix;
		XtSetArg(pix[n_pix], XmNlabelInsensitivePixmap, ip);	++n_pix;
		wlSetValues(kid, pix, n_pix);
		tbc_free_pixmap(kid) = True;
	}

	/*
	 * And force a resize to get some sanity.
	 * Surely there is a better way to do this.
	 */
	XtUnmanageChild(w);
	XtManageChild(w);

#	undef CANT_LOAD
}

static Boolean
set_values(Widget cur, Widget req, Widget neww, ArgList a, Cardinal *n)
{
	Arg      at[3], al[3];
	Cardinal nt, nl;
	USEUP(req); USEUP(a); USEUP(n);

	if(tb_orientation(cur) != tb_orientation(neww))
		set_geom_methods(neww);

	if(tb_orientation(cur)   != tb_orientation(neww)
	|| tb_spacing(cur)       != tb_spacing(neww)
	|| tb_group_spacing(cur) != tb_group_spacing(neww)
	|| tb_margin_width(cur)  != tb_margin_width(neww)
	|| tb_margin_height(cur) != tb_margin_height(neww))
		layout(neww);

	nt = nl = 0;

	if(tb_tip_foreground(cur) != tb_tip_foreground(neww)) {
		XtSetArg(at[nt], XmNforeground, tb_tip_foreground(neww));
		++nt;
		XtSetArg(al[nl], XmNforeground, tb_tip_foreground(neww));
		++nl;
	}
	if(tb_tip_background(cur) != tb_tip_background(neww)) {
		XtSetArg(at[nt], XmNbackground, tb_tip_background(neww));
		++nt;
		XtSetArg(al[nl], XmNbackground, tb_tip_background(neww));
		++nl;
	}
	if(tb_tip_border_width(cur) != tb_tip_border_width(neww)) {
		XtSetArg(at[nt], XmNborderWidth, tb_tip_border_width(neww));
		++nt;
	}
	if(tb_tip_fontlist(cur) != tb_tip_fontlist(neww)) {
		XtSetArg(al[nl], XmNfontList, tb_tip_fontlist(neww));
		++nl;
	}
	if(!tb_show_tips(neww)
	&& tb_tooltip_on(neww))
		tip_off(neww);

	if(tb_flatten_buttons(cur) != tb_flatten_buttons(neww))
		change_flattness(neww);

	wlSetValues(tb_tip_window(neww), at, nt);
	wlSetValues(tb_tooltip(neww), al, nl);

	if(tb_tip_position(cur) != tb_tip_position(neww)
	|| tb_tip_distance(cur) != tb_tip_distance(neww))
		tip_replace(neww);

	return False;
}

static XtGeometryResult
query_geometry(Widget w, XtWidgetGeometry *req, XtWidgetGeometry *reply)
{
	XtGeometryResult answer = XtGeometryNo;

	reply->request_mode = 0;

	if(req->request_mode & CWX) {
		reply->x = req->x;
		reply->request_mode |= CWX;
		answer = XtGeometryYes;
	}

	if(req->request_mode & CWY) {
		reply->y = req->y;
		reply->request_mode |= CWY;
		answer = XtGeometryYes;
	}

	if(req->request_mode & CWBorderWidth) {
		reply->border_width = req->border_width;
		reply->request_mode |= CWBorderWidth;
		answer = XtGeometryYes;
	}

	if(req->request_mode & CWSibling) {
		reply->sibling = req->sibling;
		reply->request_mode |= CWSibling;
		answer = XtGeometryYes;
	}

	if(req->request_mode & CWStackMode) {
		reply->stack_mode = req->stack_mode;
		reply->request_mode |= CWStackMode;
		answer = XtGeometryYes;
	}

	if(req->request_mode & (CWWidth | CWHeight))
		tb_sizer(w)(w, reply);

	if(req->request_mode & CWWidth) {
		reply->request_mode |= CWWidth;
		answer = (req->width >= reply->width)
			? XtGeometryYes
			: XtGeometryNo;
	}

	if(req->request_mode & CWHeight) {
		reply->request_mode |= CWHeight;
		answer = (req->height >= reply->height)
			? XtGeometryYes
			: XtGeometryNo;
	}

	return answer;
}

/**
 ** Methods:  Composite
 **/

static XtGeometryResult
geometry_manager(Widget w, XtWidgetGeometry *req, XtWidgetGeometry *reply)
{
	Dimension        width  = core_width(w);
	Dimension        height = core_height(w);
	Dimension        border = core_border_width(w);
	XtGeometryResult answer = XtGeometryNo;
	Widget           tb     = XtParent(w);

	reply->request_mode = 0;

	if(req->request_mode & CWBorderWidth) {
		border = reply->border_width = req->border_width;
		reply->request_mode |= CWBorderWidth;
		answer = XtGeometryYes;
	}

	if(req->request_mode & CWSibling) {
		reply->sibling = req->sibling;
		reply->request_mode |= CWSibling;
		answer = XtGeometryYes;
	}

	if(req->request_mode & CWStackMode) {
		reply->stack_mode = req->stack_mode;
		reply->request_mode |= CWStackMode;
		answer = XtGeometryYes;
	}

	if(req->request_mode & CWWidth) {
		width = reply->width = req->width;
		reply->request_mode |= CWWidth;
		answer = XtGeometryYes;
	}

	if(req->request_mode & CWHeight) {
		height = reply->height = req->height;
		reply->request_mode |= CWHeight;
		answer = XtGeometryYes;
	}

	if(req->request_mode & (CWWidth | CWHeight | CWBorderWidth)) {
		XtResizeWidget(w, width, height, border);
		layout(tb);
	}

	if(req->request_mode & (CWX | CWY))
		answer = XtGeometryNo;

	return answer;
}

static void
change_managed(Widget w)
{
	layout(w);
}

/**
 ** Methods: Constraint
 **/

/*
 * We can't play with toolBarIcon here because no one will be realized and
 * we need a window.  We actually have the same problem with all of the
 * contraint_* functions, hence, we have to do some fiddling around the
 * the toolbar realize method.
 */
static void
constraint_init(Widget reqw, Widget neww, ArgList a, Cardinal *n)
{
	Widget tb = XtParent(neww);
	USEUP(reqw); USEUP(a); USEUP(n);

	if(tbc_tooltip(neww) != NULL)
		tbc_tooltip(neww) = XmStringCopy(tbc_tooltip(neww));
	tbc_break_loop(neww)  = False;
	tbc_free_pixmap(neww) = False;
	XtAddEventHandler(neww, MASK, False, enter_leave, tb);
	if(XmIsPushButton(neww) || XmIsPushButtonGadget(neww)) {
		if(tb_flatten_buttons(tb))
			XtSetValues(neww,tb_leave_args(tb),tb_leave_args_n(tb));
		else
			XtSetValues(neww,tb_enter_args(tb),tb_enter_args_n(tb));
		XtAddEventHandler(neww, MASK, False, shadow_trick, tb);
	}
}

static Boolean
constraint_setvalues(Widget current, Widget req, Widget neww,
							ArgList a, Cardinal *n)
{
	Widget tb = XtParent(neww);
	USEUP(a); USEUP(n); USEUP(req);

	if(!XmStringCompare(tbc_tooltip(neww), tbc_tooltip(current))) {
		XmStringFree(tbc_tooltip(current));
		tbc_tooltip(current) = NULL;
		tbc_tooltip(neww) = XmStringCopy(tbc_tooltip(neww));
	}

	/*
	 * This "break_loop" business is pretty sick but it can't be
	 * helped.  Without this trick, the XtSetValues call will trigger a
	 * contstraint_setvalues call and the obvious death will happen
	 * shortly.  An alternative approach would be to check the
	 * margin and shadow settings on 'neww' to see if we need to set
	 * them, however, I don't know what order the constraint and standard
	 * widget setvalues methods are called in so this other trick may
	 * not work.
	 *
	 * At least we can hide break_loop in the constraint record.
	 *
	 * This whole thing is here to get the leave_args set when a button
	 * becomes insensitive.
	 */
	if(!tbc_break_loop(neww)
	&& tb_flatten_buttons(tb)
	&& !XtIsSensitive(neww)
	&& (XmIsPushButton(neww) || XmIsPushButtonGadget(neww))) {
		tbc_break_loop(neww) = True;
		XtSetValues(neww, tb_leave_args(tb), tb_leave_args_n(tb));
		tbc_break_loop(neww) = False;
	}

	return False;
}

static void
constraint_destroy(Widget w)
{
	Widget   tb = XtParent(w);
	Pixmap   p, ip;
	Arg      a[2];
	Cardinal n;

	if(tbc_tooltip(w) != NULL)
		XmStringFree(tbc_tooltip(w));

	if(XmIsPushButton(w) || XmIsPushButtonGadget(w))
		XtRemoveEventHandler(w, MASK, False, shadow_trick, tb);
	XtRemoveEventHandler(w, MASK, False, enter_leave, tb);

	/*
	 * This really is cheating but I can't ignore such convenience.
	 */
	if(!tbc_free_pixmap(w) || !XmIsLabel(w) || !XmIsLabelGadget(w))
		return;

	p = ip = None;

	n = 0;
	XtSetArg(a[n], XmNlabelPixmap,            &p);		++n;
	XtSetArg(a[n], XmNlabelInsensitivePixmap, &ip);		++n;
	wlGetValues(w, a, n);

	n = 0;
	XtSetArg(a[n], XmNlabelPixmap,            None);	++n;
	XtSetArg(a[n], XmNlabelInsensitivePixmap, None);	++n;
	wlSetValues(w, a, n);

	if(p  != None && p  != tb_unknown(tb))
		XFreePixmap(XtDisplay(tb), p);
	if(ip != None && ip != tb_iunknown(tb))
		XFreePixmap(XtDisplay(tb), ip);
}

/**
 ** Convenience functions.
 **/

void
WlToolBarSetInternalImages(Widget w, WlToolBarImage *imgs, size_t n_imgs)
{
	tb_images(w)   = imgs;
	tb_n_images(w) = (int)n_imgs;
}
