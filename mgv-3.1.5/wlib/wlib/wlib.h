/*
 * wlib.h
 *	Interface to wlib.
 *
 * Copyright (C) 1996 Matthew D. Francey
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
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#ifndef	WLIB_H
#define	WLIB_H
MU_HID(wlib_h, "$Mu: wlib/wlib/wlib.h 1.48 1998/08/27 04:22:37 $")

#include <X11/Intrinsic.h>

#include <wlib/strdefs.h>

/*
 * widget creation convenience function
 */
CDECLS_BEGIN
typedef Widget (*WL_CONVFUNC)(Widget, String, ArgList, Cardinal);
CDECLS_END

/*
 * callback lists
 */
typedef struct {
	char		*name;
	char		*call;
	XtPointer	closure;
	void		(*func)(Widget, XtPointer, XtPointer);
} WLC;

/*
 * Widget lists.
 * In most cases, the "type" will be the name of the widget's public
 * header (without the .h), for dialogs, use "ErrorDialog", "FileSBDialog",...
 * If you've added a widget, then you already know what "type" to use.
 *
 * For convenience (and some extra compile time checking), constants
 * for the standard Motif 1.2 classes are provided in wlib/strdefs.h.
 */
#define	WL_MANAGE	(1 << 0)
#define	WL_UNMANAGE	(1 << 1)
typedef struct wl_widget_s WLW;
struct wl_widget_s {
	char	*name;
	char	*type;
	int	flags;	/* WL_MANAGE, WL_UNMANAGE */
	WLW	*kids;
};

/*
 * these currently only exists for completeness
 */
#define	wl_find1	XtNameToWidget
#define	wl_destroy	XtDestroyWidget

/*
 * Values struct for wl_[gs]etsbvalues().  You can deal with most
 * of this stuff with XmScrollBar[GS]etValues() but you can't get/set
 * XmNminimum or XmNmaxiumum (and you'll almost always want the bounds
 * to ensure sanity on the slider position).
 */
typedef struct {
	int	value;		/* slider position			*/
	int	slider;		/* slider size				*/
	int	incr;		/* increment/decrement amount		*/
	int	pincr;		/* page increment/decrement amount	*/
	int	max;		/* XmNmaximum				*/
	int	min;		/* XmNminimum				*/
} WL_SBV;

/*
 * prototypes
 * Functions that return ints generally return zero for success
 * and an errno value for failure
 */
CDECLS_BEGIN
extern	Display *wl_init(int *, char **, String, XrmOptionDescList, Cardinal, char **);
extern	XtAppContext wl_app(void);

extern	int	wl_addclass(char *, WidgetClass, WL_CONVFUNC, int);
extern	int	wl_appnames(Widget, char **, char **);
extern	char	*wl_appname(Widget);
extern	char	*wl_appclass(Widget);
extern	Widget	wl_create(Widget, WLW *);
extern	void	wl_dismiss(Widget, XtPointer, XtPointer);
extern	void	wl_callback(Widget, WLC *, XtPointer);
extern	Widget	wl_error(Widget, char *, ...);
extern	Widget	wl_warn(Widget, char *, ...);
extern	Widget	wl_msg(Widget, char *, ...);
extern	void	wl_help(Widget, XtPointer, XtPointer);
extern	void	wl_help_context(Widget, XtPointer, XtPointer);
extern	void	wl_find(Widget, ...);
extern	Widget	wl_findup(Widget, char *);
extern	Widget	wl_top(Widget);
extern	Widget	wl_shell(Widget);
extern	void	wl_shutdown(void);
extern	Widget	wl_focus(Widget);

extern	Widget	wl_getsbvalues(Widget, char *, WL_SBV *);
extern	void	wl_setsbvalues(Widget, WL_SBV *, Boolean);

extern	char	*wl_version(void);

extern	void	wl_toggle_sync(Widget, XtPointer, XtPointer);

extern	int	wl_chkver(Widget);

extern	void	wl_dlg_popup(Widget);
extern	void	wl_dlg_popdown(Widget);

CDECLS_END

#endif
