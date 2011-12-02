/**
 * $Id: AppShell.c,v 1.7 2001/06/09 18:38:57 amai Exp $
 *
 * Copyright (C) 1998 Eric A. Howe
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
 * This widget is loosely based an the AppPlusS widget by John L. Cwikla
 * <cwikla@wri.com>.
 **/
static const char rcsid[] = "$Id: AppShell.c,v 1.7 2001/06/09 18:38:57 amai Exp $";

#include "../config.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include <AppShellP.h>
#include <Xlt.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif


static void class_init(void);
static void init(Widget, Widget, ArgList, Cardinal *);
static void realize(Widget, XtValueMask *, XSetWindowAttributes *);

#define	super_realize \
	xltAppShellWidgetClass->core_class.superclass->core_class.realize
#define	class_colormap \
	xltAppShellClassRec.app_shell_class.colormap
#define	class_visual \
	xltAppShellClassRec.app_shell_class.visual
#define	class_vclass \
	xltAppShellClassRec.app_shell_class.vclass
#define	class_depth \
	xltAppShellClassRec.app_shell_class.depth

#define core_depth(w) \
	((w)->core.depth)
#define core_colormap(w) \
	((w)->core.colormap)

#define	shell_visual(w) \
	(((XltAppShellWidget)(w))->shell.visual)

#define	as_depth(w) \
	(((XltAppShellWidget)(w))->app_shell.depth)
#define	as_vclass(w) \
	(((XltAppShellWidget)(w))->app_shell.vclass)
#define	as_privcmap(w) \
	(((XltAppShellWidget)(w))->app_shell.use_private_colormap)
#define	as_vid(w) \
	(((XltAppShellWidget)(w))->app_shell.vid)
#define	as_editres(w) \
	(((XltAppShellWidget)(w))->app_shell.use_editres)

#define DEFAULT -1

#define OFFSET(field) XtOffset(XltAppShellWidget, app_shell.field)
static XtResource resources[] =
{
	{
		XltNappVisualClass, XltCAppVisualClass,
		XltRAppVisualClass, sizeof(int), OFFSET(vclass),
		XtRImmediate, (XtPointer)DEFAULT
	}, {
		XltNappUsePrivateColormap, XltCAppUsePrivateColormap,
		XtRBoolean, sizeof(Boolean), OFFSET(use_private_colormap),
		XtRImmediate, (XtPointer)False
	}, {
		XltNappVisualID, XltCAppVisualID,
		XltRAppVisualID, sizeof(VisualID), OFFSET(vid),
		XtRImmediate, (XtPointer)0
	}, {
		XltNappDepth, XltCAppDepth,
		XtRInt, sizeof(int), OFFSET(depth),
		XtRImmediate, (XtPointer)DEFAULT
	}, {
		XltNappUseEditres, XltCAppUseEditres,
		XtRBoolean, sizeof(Boolean), OFFSET(use_editres),
		XtRImmediate, (XtPointer)True
	}
};
#undef OFFSET

XltAppShellClassRec xltAppShellClassRec = 
{
	{	/** Core **/
	/* superclass		*/	(WidgetClass)&applicationShellClassRec,
	/* class_name		*/	"XltAppShell",
	/* widget_size		*/	sizeof(XltAppShellRec),
	/* class_initialize	*/	class_init,
	/* class_initialize_hook*/	NULL,
	/* class_inited		*/	False,
	/* initialize		*/	init,
	/* initialize_hook	*/	NULL,
	/* realize		*/	realize,
	/* actions		*/	NULL,
	/* num_actions		*/	0,
	/* resources		*/	resources,
	/* num_resources	*/	XtNumber(resources),
	/* xrm_class		*/	NULLQUARK,
	/* compress_motion	*/	False,
	/* compress_exposures	*/	False,
	/* compress_enterleave	*/	False,
	/* visible_interest	*/	False,
	/* destroy		*/	NULL,
	/* resize		*/	XtInheritResize,
	/* expose		*/	XtInheritExpose,
	/* set_values		*/	NULL,
	/* set_values_hook	*/	NULL,
	/* set_values_almost	*/	XtInheritSetValuesAlmost,
	/* get_values_hook	*/	NULL,
	/* accept_focues	*/	XtInheritAcceptFocus,
	/* version		*/	XtVersion,
	/* callback_private	*/	NULL,
	/* tm_table		*/	NULL,
	/* query_geometry	*/	XtInheritQueryGeometry,	
	/* display_accelerator	*/	XtInheritDisplayAccelerator,
	/* extension		*/	NULL,
	},

	{	/** Composite **/
	/* geometry_manager	*/	XtInheritGeometryManager,
	/* change_managed	*/	XtInheritChangeManaged,
	/* insert_child		*/	XtInheritInsertChild,
	/* delete_child		*/	XtInheritDeleteChild,
	/* extension		*/	NULL,
	},

	{	/** Shell **/
	/* extension		*/	0,
	},

	{	/** WindowManagerShell **/
	/* extension		*/	0,
	},

	{	/** VendorShell **/
	/* extension		*/	NULL,
	},

	{	/** TopLevelShell **/
	/* extension		*/	NULL,
	},

	{	/** ApplicationShell **/
	/* extension		*/	NULL,
	},

	{	/** XltAppShell **/
	/* colormap		*/	(Colormap)None,
	/* visual		*/	NULL,
	/* vclass		*/	0,	
	/* depth		*/	0,
	}
};

WidgetClass xltAppShellWidgetClass = (WidgetClass)&xltAppShellClassRec;

static struct {
	char	*s;	/* string version of visual class	*/
	size_t	n;	/* size of `s'				*/
	int	v;	/* visual class				*/
} vis_names[] = {
#	define	BLECH(a,b)	{a, sizeof(a) - 1, b}
	BLECH("staticgray",	StaticGray),
	BLECH("staticgrey",	StaticGray),
	BLECH("staticcolor",	StaticColor),
	BLECH("pseudocolor",	PseudoColor),
	BLECH("grayscale",	GrayScale),
	BLECH("greyscale",	GrayScale),
	BLECH("truecolor",	TrueColor),
	BLECH("directcolor",	DirectColor)
#	undef	BLECH
};
#define	N_VNAMES	(int)(sizeof(vis_names)/sizeof(vis_names[0]))

static Boolean
string2visualclass(Display *dpy, XrmValue *a, Cardinal *n,
				XrmValue *from, XrmValue *to, XtPointer *data)
{
	static int cls;
	char	*name;
	Boolean found;
	int i;

	if(*n != 0) {
		XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
				"string2visualclass", "wrongParamaters",
				"ResourceError",
				"string2visualclass needs no arguments.",
				NULL, NULL);
	}

	if(to->addr != NULL && to->size < sizeof(cls)) {
		to->size = sizeof(cls);
		return False;
	}

	name = XtNewString((char *)from->addr);
	for(i = strlen(name) - 1; i >= 0; --i)
		name[i] = tolower(name[i]);

	cls  = DEFAULT;
	for(i = 0, found = False; i < N_VNAMES; ++i) {
		if(strncmp(name, vis_names[i].s, vis_names[i].n) != 0)
			continue;
		found = True;
		cls   = vis_names[i].v;
		break;
	}
	XtFree(name);

	if(!found) {
		XtDisplayStringConversionWarning(dpy, from->addr,
							XltRAppVisualClass);
		return False;
	}

	if(to->addr == NULL)
		to->addr = (XtPointer)&cls;
	else
		*(int *)to->addr = cls;
	to->size = sizeof(cls);

	return True;
}

static Boolean
string2visualid(Display *dpy, XrmValue *a, Cardinal *n,
				XrmValue *from, XrmValue *to, XtPointer *data)
{
static	VisualID	vid;
	XVisualInfo	tmp, *v;
	int		n_v;

	if(*n != 0) {
		XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
					"string2visualid", "wrongParamaters",
					"ResourceError",
					"string2visualid needs no arguments.",
					NULL, NULL);
	}

	if(to->addr != NULL && to->size < sizeof(vid)) {
		to->size = sizeof(vid);
		return False;
	}

	vid = (VisualID)strtol(from->addr, NULL, 0);
	tmp.visualid = vid;
	if((v = XGetVisualInfo(dpy, VisualIDMask, &tmp, &n_v)) == NULL) {
		XtDisplayStringConversionWarning(dpy, from->addr, XltRAppVisualID);
		return False;
	}
	XFree((char *)v);

	if(to->addr == NULL)
		to->addr = (XtPointer)&vid;
	else
		*(VisualID *)to->addr = vid;
	to->size = sizeof(vid);

	return True;
}

static void
class_init(void)
{
	XtSetTypeConverter(XtRString, XltRAppVisualClass, string2visualclass,
						NULL, 0, XtCacheAll, NULL);
	XtSetTypeConverter(XtRString, XltRAppVisualID, string2visualid,
						NULL, 0, XtCacheAll, NULL);
}

static void
defaults(Widget w, XVisualInfo *vi)
{
	Display	*dpy = XtDisplay(w);

	vi->depth  = DefaultDepth(dpy, DefaultScreen(dpy));
	vi->visual = DefaultVisual(dpy, DefaultScreen(dpy));
	vi->class  = vi->visual->class;
}

static Boolean
valid_vid(Widget w, XVisualInfo *vi)
{
	XVisualInfo	*vis, tmp;
	int		n;

	if(as_vid(w) == 0)
		return False;

	tmp.visualid = as_vid(w);
	if((vis = XGetVisualInfo(XtDisplay(w), VisualIDMask, &tmp, &n)) == NULL)
		return False;

	/*
	 * We'll assume that there is only one.
	 */
	*vi = *vis;
	XFree((char *)vis);
	return True;
}

static Boolean
use_defaults(Widget w, XVisualInfo *vi)
{
	if(as_depth(w) != DEFAULT || as_vclass(w) != DEFAULT)
		return False;
	defaults(w, vi);

	return True;
}

static void
init(Widget in, Widget out, ArgList a, Cardinal *n)
{
	Display		*dpy;
	int		sn;
	Visual		*def;
	XVisualInfo	vi;

	dpy  = XtDisplay(out);
	sn   = DefaultScreen(dpy);
	def  = DefaultVisual(dpy, sn);

	if(class_colormap != None) {
		shell_visual(out)  = class_visual;
		as_vclass(out)     = class_vclass;
		core_depth(out)    = class_depth;
		core_colormap(out) = class_colormap;
		return;
	}

	if(!valid_vid(out, &vi)
	&& !use_defaults(out, &vi)) {
		if(as_depth(out) == DEFAULT)
			core_depth(out) = DefaultDepth(dpy, sn);
		else
			core_depth(out) = as_depth(out);

		if(as_vclass(out) == DEFAULT)
			as_vclass(out) = def->class;

		/*
		 * Try and match the user's request; if that doesn't work,
		 * try and match the depth; if that doesn't work, try and
		 * match the visual class (with the best depth); if that
		 * doesn't work, we just give up and use the defaults
		 * that Xt has given us.
		 */
		if(XMatchVisualInfo(dpy, sn, core_depth(out),
						as_vclass(out), &vi) == 0) {
			XVisualInfo	tmp;
			XVisualInfo	*v  = NULL;
			int		n_v = 0;
			int		n   = 0;

			if(as_depth(out) != DEFAULT) {
				/*
				 * We'll ignore n_v here to avoid some
				 * hassle, if the user doesn't like the
				 * first visual we get, then they can
				 * always use appVisualID.
				 */
				tmp.depth = core_depth(out);
				v = XGetVisualInfo(dpy, VisualDepthMask, &tmp,
									&n_v);
			}

			if(v != NULL) {
				vi = *v;
			}
			else {
				tmp.class = as_vclass(out);
				v = XGetVisualInfo(dpy, VisualClassMask, &tmp,
									&n_v);
				if(v != NULL) {
					int	i;
					for(i = 1, n = 0; i < n_v; ++i)
						if(v[i].depth > v[n].depth)
							n = i;
					vi = v[n];
				}
			}

			if(v == NULL)
				defaults(out, &vi);
			else
				XFree((char *)v);
		}
	}

	core_depth(out)   = vi.depth;
	shell_visual(out) = vi.visual;
	as_vclass(out)    = vi.class;
	if(shell_visual(out)->visualid == def->visualid
	&& !as_privcmap(out))
		core_colormap(out) = DefaultColormap(dpy, sn);
	else
		core_colormap(out) = XCreateColormap(dpy, RootWindow(dpy, sn), 
						shell_visual(out), AllocNone);

	/*
	 * And finally, cache the results for future use.
	 */
	class_visual   = shell_visual(out);
	class_colormap = core_colormap(out);
	class_vclass   = as_vclass(out);
	class_depth    = core_depth(out);
}

/*
 * LessTif supplies an editres protocol handler as part of VendorShell
 * so we don't have to bother worrying about it.  IMHO, editres should be
 * a required part of X:  editres is just too cool to be passed by.
 */
#if !defined(LesstifVersion) && defined(HAVE_EDITRES) && HAVE_EDITRES
#	include <X11/Xmu/Editres.h>
#	define	add_editres(w) \
			XtAddEventHandler((w), (EventMask)0, True, \
				(XtEventHandler)_XEditResCheckMessages, NULL);
#else
#	define add_editres(w) (void)(w)
#endif

static void
realize(Widget w, XtValueMask *mask, XSetWindowAttributes *attr)
{
	attr->colormap         = core_colormap(w);
	attr->background_pixel = 0;
	attr->border_pixel     = 0;
	*mask |= CWColormap | CWBorderPixel | CWBackPixel;

	super_realize(w, mask, attr);

	if(as_editres(w))
		add_editres(w);
}

Widget
XltAppInitialize(XtAppContext *ac, String cls, XrmOptionDescList options,
		Cardinal num_options, int *argc, String *argv,
		String *fallbacks, ArgList args, Cardinal num_args)
{
	XtAppContext app;
	Widget       shell;
	Display     *dpy;

	XtToolkitInitialize();
	app = XtCreateApplicationContext();
	if(fallbacks != NULL)
		XtAppSetFallbackResources(app, fallbacks);
	dpy = XtOpenDisplay(app, NULL, NULL, cls, options, num_options,
							argc, argv);
	shell = XtAppCreateShell(NULL, cls, xltAppShellWidgetClass, dpy,
							args, num_args);

	if(ac != NULL)
		*ac = app;

	return shell;
}

Widget
XltCreateAppShell(Widget parent, char *name, Arg *al, Cardinal ac)
{
	return XtCreateWidget(name, xltAppShellWidgetClass, parent, al, ac);
}
