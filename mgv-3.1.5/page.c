/*
 * page.c
 *	Page menu callbacks for mgv.
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
MU_ID("$Mu: mgv/page.c 1.103 1998/08/16 05:42:17 $")

#include <assert.h>
#include <Xm/List.h>
#include <Xm/ToggleB.h>
#include <Xm/ScrollBar.h>

#include <wlib/wlib.h>
#include <wlib/typesanity.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/page.h>
#include <mine/log.h>
#include <mine/sens.h>
#include <mine/auto.h>
#include <mine/center.h>
#include <mine/uiutil.h>
#include <mine/page_control.h>

/*
 * popup->back, popup->forward
 */
void
mgv_page_backforward(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	USEUP(call);

	if(*XtName(w) == 'b')
		mgv_pgstack_back(m->pgstack);
	else
		mgv_pgstack_forward(m->pgstack);
}

/*
 * page->next
 */
void
mgv_page_next(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m    = (MGV *)closure;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	mgv_pgstack_goto(m->pgstack, m->page + 1);
}

/*
 * page->prev
 */
void
mgv_page_prev(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m    = (MGV *)closure;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	mgv_pgstack_goto(m->pgstack, m->page - 1);
}

/*
 * page list callback
 */
void
mgv_page_goto(Widget w, XtPointer closure, XtPointer call)
{
	XmLSS	*cbs = (XmLSS *)call;
	MGV	*m   = (MGV *)closure;
	USEUP(w);
	assert(MgvOK(m));

	if(m->page == cbs->item_position - 1)
		return;

	/*
	 * Jeez, XmList is one-based (probably so you can use position
	 * zero to indicate the end of the list when using the
	 * XmList.*() convenience functions, -1 would probably be a better
	 * value to indicate "the end"), how silly in a C environment
	 * (even windoze got this one right!).
	 */
	mgv_pgstack_goto(m->pgstack, cbs->item_position - 1);
}

/*
 * show/hide the page labels/numbers
 */
void
mgv_page_labels(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS	*cbs = (XmTBS *)call;
	MGV	*m   = (MGV *)closure;
	Widget	p;
	assert(MgvOK(m));

	if(m->dsc == NULL
	|| m->dsc->pages == NULL)
		return;
	if(strcmp(XtName(w), "showLabels") == 0)
		m->labels = cbs->set;
	else
		m->pgnums = cbs->set;

	if(!m->labels && !m->pgnums) {
		w = wl_find1(m->main, "*popup*hideList");
		XmToggleButtonSetState(w, True, True);
		return;
	}
	if(!m->labels != !m->pgnums
	&& !XtIsManaged(XtParent(m->pagelist))) {
		p = wl_find1(m->main, "*popup*hideList");
		assert(p != NULL);
		XmToggleButtonSetState(p, False, True);
	}

	XmListDeleteAllItems(m->pagelist);
	mgv_list_update(m, -1);

	/*
	 * There doesn't seem to be any simple way to retain
	 * the scroll position.  If you replace in the loop rather
	 * than rebuilding the list, the list won't resize properly
	 * when you the labels go away (i.e. it won't shrink).
	 */
	XmListSetBottomPos(m->pagelist, m->page + 1);

	/*
	 * And the sizing trick (ugh!).
	 */
	p = XtParent(m->pagelist);
	if(!XtIsManaged(p))
		return;
	XtUnmanageChild(p);
	XtVaSetValues(p, XmNrightAttachment, XmATTACH_FORM, NULL);
	XtManageChild(p);
}

/*
 * page->center
 */
void
mgv_page_center(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m   = (MGV *)closure;
	XmTBS	*cbs = (XmTBS *)call;
	USEUP(w);
	assert(MgvOK(m));

	m->center = cbs->set ? mgv_center_center : mgv_center_noop;
	m->center(m);
}

/*
 * page->redisplay
 */
void
mgv_page_redisplay(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	mgv_show(m, m->page);
}

/*
 * page->auto size
 */
void
mgv_page_size(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m   = (MGV *)closure;
	XmTBS	*cbs = (XmTBS *)call;
	MGV_APP	*res = mgv_appres(m->main);
	assert(MgvOK(m));

	if(!cbs->set)
		return;

	/*
	 * "What's in a name...", a bloody hell of a lot I'd say.
	 */
	switch(*XtName(w)) {
	case 's':
		m->autosizer     = mgv_auto_size;
		res->auto_height = True;
		res->auto_width  = True;
		break;
	case 'h':
		m->autosizer     = mgv_auto_height;
		res->auto_height = True;
		res->auto_width  = False;
		break;
	case 'w':
		m->autosizer     = mgv_auto_width;
		res->auto_height = False;
		res->auto_width  = True;
		break;
	case 'n':
		m->autosizer     = mgv_auto_none;
		res->auto_height = False;
		res->auto_width  = False;
		break;
	default:
		assert("unknown toggle!" != NULL);
		break;
	}
	m->autosizer(m);
}
