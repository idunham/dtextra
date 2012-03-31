/*
 * auto.c
 *	Automatic window height/width functions.
 *
 * Copyright (C) 1997 Eric A. Howe
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
MU_ID("$Mu: mgv/auto.c 1.9.2.1 1998/09/27 08:31:46 $")

#include <assert.h>
#include <Xm/Xm.h>
#include <Xm/XmStrDefs.h>

#include <wlib/wlib.h>
#include <wlib/sanity.h>
#include <mine/mgv.h>
#include <mine/auto.h>

/**
 ** Remember that MGV.root_width and MGV.root_height have wmWidth
 ** and wmHeight adjustments built in.
 **/

/*
 * Automatic height and width.
 */
void
mgv_auto_size(MGV *m)
{
	Dimension gw, gh, mw, mh, cw, ch;
	Dimension width, height;
	Arg       a[2];
	Cardinal  n;
	assert(MgvOK(m));

	n = 0;
	XtSetArg(a[n], XmNwidth,  &width);	++n;
	XtSetArg(a[n], XmNheight, &height);	++n;
	ARGSANITY(a, n);

	XtGetValues(m->ghost,           a, n);	gw = width;	gh = height;
	XtGetValues(m->clip,            a, n);	cw = width;	ch = height;
	XtGetValues(XtParent(m->main),  a, n);	mw = width;	mh = height;

	if((mw += gw - cw) > m->root_width
	&& (m->bits & MgvSMARTSIZE))
		mw = m->root_width;
	if((mh += gh - ch) > m->root_height
	&& (m->bits & MgvSMARTSIZE))
		mh = m->root_height;

	n = 0;
	XtSetArg(a[n], XmNheight, mh);	++n;
	XtSetArg(a[n], XmNwidth,  mw);	++n;
	wlSetValues(XtParent(m->main), a, n);
}

/*
 * Automatic width.
 */
void
mgv_auto_width(MGV *m)
{
	Dimension gw, mw, cw;
	Dimension width;
	Arg       a;
	assert(MgvOK(m));

	XtSetArg(a, XmNwidth, &width);
	XtGetValues(m->ghost,          &a, 1);	gw = width;
	XtGetValues(m->clip,           &a, 1);	cw = width;
	XtGetValues(XtParent(m->main), &a, 1);	mw = width;

	if((mw += gw - cw) > m->root_width
	&& (m->bits & MgvSMARTSIZE))
		mw = m->root_width;

	XtSetArg(a, XmNwidth, mw);
	XtSetValues(XtParent(m->main), &a, 1);
}

/*
 * Automatic height.
 */
void
mgv_auto_height(MGV *m)
{
	Dimension gh, mh, ch;
	Dimension height;
	Arg       a;
	assert(MgvOK(m));

	XtSetArg(a, XmNheight, &height);
	XtGetValues(m->ghost,          &a, 1);	gh = height;
	XtGetValues(m->clip,           &a, 1);	ch = height;
	XtGetValues(XtParent(m->main), &a, 1);	mh = height;

	if((mh += gh - ch) > m->root_height
	&& (m->bits & MgvSMARTSIZE))
		mh = m->root_height;

	XtSetArg(a, XmNheight, mh);
	XtSetValues(XtParent(m->main), &a, 1);
}

/*
 * Automatic nothing.  Having an empty auto-sizer saved us from having to
 * litter the code with a bunch of ugly NULL checks.
 */
void
mgv_auto_none(MGV *m)
{
	USEUP(m);
}
