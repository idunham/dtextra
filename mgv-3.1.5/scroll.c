/*
 * scroll.c
 *	Page scrolling and panning actions.
 *
 * Copyright (C) 1998 Eric A. Howe
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
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/scroll.c 1.9 1998/09/01 20:49:39 $")

#include <assert.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <Xm/XmStrDefs.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/sanity.h>
#include <mine/mgv.h>
#include <mine/scroll.h>
#include <mine/strings.h>
#include <mine/page_control.h>
#include <mine/sens.h>
#include <mine/util.h>
#include <mine/center.h>

/*
 * Don't look kids, this function is being naughty under the covers.
 *
 * There is some overlap between this and the scrolling for the mgvScroll
 * action but that can't really be helped without making more mess.
 */
static void
pan(MGV *m, int dx, int dy)
{
	WL_SBV   h, v;
	Widget   hw = NULL;
	Widget   vw = NULL;
	Position x, y;
	Arg      a[2];
	Cardinal n;

	hw = wl_getsbvalues(m->main, "*view*HorScrollBar", &h);
	if(h.value + dx < h.min)
		dx = h.min - h.value;
	else if(h.value + dx > h.max - h.slider)
		dx = h.max - h.slider - h.value;

	vw = wl_getsbvalues(m->main, "*view*VertScrollBar", &v);
	if(v.value + dy < v.min)
		dy = v.min - v.value;
	else if(v.value + dy > v.max - v.slider)
		dy = v.max - v.slider - v.value;

	h.value += dx;
	v.value += dy;

	wl_setsbvalues(hw, &h, False);
	wl_setsbvalues(vw, &v, False);

	n = 0;
	XtSetArg(a[n], XmNx, &x);	++n;
	XtSetArg(a[n], XmNy, &y);	++n;
	wlGetValues(m->ghost, a, n);

	XtMoveWidget(m->ghost, x - dx, y - dy);
}

void
mgv_action_pan(Widget w, XEvent *ev, String *argv, Cardinal *argc)
{
	MGV	*m;
	char	*cmd;
	USEUP(argc);

	w = wl_findup(w, "mainWindow");
	XtVaGetValues(w, XmNuserData, &m, NULL);
	assert(MgvOK(m));

	cmd = lowdup(*argv);

	if(strcmp(cmd, "begin") == 0) {
		m->startx = ev->xbutton.x;
		m->starty = ev->xbutton.y;
	}
	else if(strcmp(cmd, "end") == 0) {
		m->startx = m->curx = -1;
		m->starty = m->cury = -1;
	}
	else if(strcmp(cmd, "pan") == 0) {
		m->curx = ev->xbutton.x;
		m->cury = ev->xbutton.y;
		pan(m, m->startx - m->curx, m->starty - m->cury);
	}
	else {
		wl_warn(m->ghost, mgv_s(w, MgvSbadArgument), "mgvPan", *argv);
	}
	XtFree(cmd);
}

/*
 * scrolling action
 * You're not supposed to fiddle with the scrollbars in a scrolled window
 * but it works so screw the OSF.
 */
#define	UP	1
#define	DOWN	2
#define	LEFT	3
#define	RIGHT	4
void
mgv_action_scroll(Widget w, XEvent *ev, String *argv, Cardinal *argc)
{
	MGV	*m;
	char    *action  = NULL;
	char	*magical = NULL;
	int	way, force, value, incr, dir;
	WL_SBV	v;
	USEUP(argc); USEUP(ev);

	w = wl_findup(w, "mainWindow");
	assert(w != NULL);
	XtVaGetValues(w, XmNuserData, &m, NULL);
	assert(MgvOK(m));

	if(*argc >= 1)
		action = lowdup(argv[0]);
	if(*argc >= 2)
		magical = lowdup(argv[1]);
	force = 1;
	if(*argc >= 3) {
		if((force = atoi(argv[2])) < 0)
			force *= -1;
		else if(force == 0)
			force  =  1;
	}

	memset((void *)&v, '\0', sizeof(v));
	if(strcmp(action, "down") == 0) {
		dir =  1;
		way = DOWN;
		w   = wl_getsbvalues(w, "*view*VertScrollBar", &v);
	}
	else if(strcmp(action, "up") == 0) {
		dir = -1;
		way = UP;
		w   = wl_getsbvalues(w, "*view*VertScrollBar", &v);
	}
	else if(strcmp(action, "right") == 0) {
		dir =  1;
		way = RIGHT;
		w   = wl_getsbvalues(w, "*view*HorScrollBar",  &v);
	}
	else if(strcmp(action, "left") == 0) {
		dir = -1;
		way = LEFT;
		w   = wl_getsbvalues(w, "*view*HorScrollBar",  &v);
	}
	else {
		XBell(XtDisplay(w), 100);
		XtFree(action);
		XtFree(magical);
		return;
	}

	/*
	 * Check magic scrolling.
	 * We abuse the MgvS things a little bit here but it gives
	 * a fairly simple solution.  We also query the scrollbar values
	 * again after we move to the new page since the page size can
	 * change.
	 *
	 * This chunk is a little convoluted because of the magic resistance.
	 */
	if((way == UP || way == DOWN)
	&& (m->bits & MgvMAGICSCROLL)
	&& strcmp(magical, "magical") == 0) {
		/*
		 * Some would consider this wonderful little construction
		 * an abuse--I call it a valid and wholesome technique.
		 */
		int hehe = (way == UP)  + 2*(m->last_magic >  0)
					+ 4*(m->last_magic == 0);
		switch(hehe) {
		case 0:	case 4:	m->last_magic -= force;	break;
		case 1:		m->last_magic  = 0;	break;
		case 2:		m->last_magic  = 0;	break;
		case 3: case 5:	m->last_magic += force;	break;
		default:
			/* can't happen (unless your compiler is busted) */
			assert("busted compiler!" != NULL);
			break;
		}

		if(way == UP
		&& v.value == v.min
		&& (m->sens & MgvSNOTFIRST)) {
			if(m->last_magic < m->magic_resistance)
				return;
			mgv_pgstack_goto(m->pgstack, m->page - 1);
			if(m->center == mgv_center_center)
				return;
			v.value = v.max - v.slider;
			wl_setsbvalues(w, &v, True);
			m->last_magic = 0;

			XtFree(action);
			XtFree(magical);
			return;
		}
		if(way == DOWN
		&& v.value + v.slider == v.max
		&& (m->sens & MgvSNOTLAST)) {
			if(m->last_magic > -m->magic_resistance)
				return;
			mgv_pgstack_goto(m->pgstack, m->page + 1);
			if(m->center == mgv_center_center)
				return;
			v.value = 0;
			wl_setsbvalues(w, &v, True);
			m->last_magic = 0;

			XtFree(action);
			XtFree(magical);
			return;
		}
	}
	m->last_magic = 0;

	/*
	 * We move m->scrollfactor of the _slider_ size (the slider
	 * represents one screen full in the scrollbar's coordinate
	 * system).  This makes things easy and sensible for both me,
	 * the programmer, and you, the not-me's (I could have said
	 * "users" but I'm a user too so that would be a misleading
	 * thing to say).
	 *
	 * Once we have the final value (unadjusted for page size
	 * limits), we can compute the individual increments based
	 * on the m->smoothness value (which is the percentage of
	 * the total increment that each little hop should be).
	 */
	value = v.slider * m->scrollfactor;
	if((incr = value / m->smoothness) <= 0)
		incr = 1;
	incr *= dir;
	value = v.value + dir*value;
	if(value < v.min)
		value = v.min;
	else if(value > v.max - v.slider)
		value = v.max - v.slider;

	if((m->bits & MgvSMOOTH) && incr < 0) {
		while((v.value += incr) > value)
			wl_setsbvalues(w, &v, True);
	}
	else if((m->bits & MgvSMOOTH) && incr > 0) {
		while((v.value += incr) < value)
			wl_setsbvalues(w, &v, True);
	}
	v.value = value;
	wl_setsbvalues(w, &v, True);

	XtFree(action);
	XtFree(magical);
}
