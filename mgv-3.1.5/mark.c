/*
 * mark.c
 *	Page marking callbacks for mgv.
 *
 * Copyright (C) 1996  Eric A. Howe
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/mark.c 1.26 1998/08/20 21:57:57 $")

#include <assert.h>
#include <ctype.h>
#include <Xm/List.h>

#include <wlib/wlib.h>
#include <wlib/typesanity.h>
#include <mine/mgv.h>
#include <mine/mark.h>
#include <mine/uiutil.h>

/*
 * mark->mark
 */
void
mgv_mark_mark(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	mgv_mark(m, m->page, !m->dsc->pages[m->page].mark, TRUE);
}

/*
 * mark->mark all
 */
void
mgv_mark_markall(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;
	int  i, max;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	max = (m->dsc == NULL) ? 0 : m->dsc->n_pages;
	for(i = 0; i < max; ++i)
		mgv_mark(m, i, TRUE, FALSE);
	mgv_list_update(m, -1);
}

/*
 * mark->unmark all
 */
void
mgv_mark_unmarkall(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;
	int  i, max;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	max = (m->dsc == NULL) ? 0 : m->dsc->n_pages;
	for(i = 0; i < max; ++i)
		mgv_mark(m, i, FALSE, FALSE);
	mgv_list_update(m, -1);
}

static void
mgv_mark_evenodd(MGV *m, int start)
{
	int	i, max;

	max   = (m->dsc == NULL) ? 0 : m->dsc->n_pages;
	for(i = start; i < max; i += 2)
		mgv_mark(m, i, TRUE, FALSE);
	mgv_list_update(m, -1);
}

/*
 * mark->odd
 */
void
mgv_mark_odd(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	mgv_mark_evenodd(m, 0);
}

/*
 * mark->even
 */
void
mgv_mark_even(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	mgv_mark_evenodd(m, 1);
}

/*
 * mark->invert
 */
void
mgv_mark_invert(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	int	i, n;
	Arg     a;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	XtSetArg(a, XmNitemCount, &n);
	XtGetValues(m->pagelist, &a, 1);

	for(i = 0; i < n; ++i)
		mgv_mark(m, i, !m->dsc->pages[i].mark, FALSE);
	mgv_list_update(m, -1);
}

/*
 * page marking action
 */
void
mgv_action_mark(Widget w, XEvent *ev, String *argv, Cardinal *argc)
{
	int    item, n_items;
	MGV   *m;
	Widget mainwin;
	Arg    a;
	USEUP(argv); USEUP(argc);

	mainwin = wl_findup(w, "mainWindow");
	assert(mainwin != NULL);

	XtSetArg(a, XmNuserData, &m);
	XtGetValues(mainwin, &a, 1);
	assert(MgvOK(m));

	/*
	 * sanity
	 */
	if(w != m->pagelist
	|| ev->type      != ButtonPress
	|| m->dsc        == NULL
	|| m->dsc->pages == NULL) {
		XBell(XtDisplay(mainwin), 100);
		return;
	}

	XtSetArg(a, XmNitemCount, &n_items);
	XtGetValues(w, &a, 1);

	if((item = XmListYToPos(w, ((XButtonEvent *)ev)->y)) > n_items) {
		XBell(XtDisplay(mainwin), 100);
		return;
	}
	mgv_mark(m, item - 1, !m->dsc->pages[item - 1].mark, TRUE);
}
