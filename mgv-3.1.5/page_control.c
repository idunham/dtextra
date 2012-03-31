/*
 * page_control.c
 *	Page stack control.
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
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/page_control.c 1.16.2.1 1998/09/11 02:41:24 $")

#include <assert.h>
#include <Xm/List.h>

#include <wlib/sanity.h>
#include <mine/mgv.h>
#include <mine/page_control.h>
#include <mine/sens.h>

/*
 * The typedef for this is in mine/mgv.h to avoid #include loops and such.
 */
#define	PGS_MAGIC1	0x50475354	/* 'P' 'G' 'S' 'T' */
#define	PGS_MAGIC2	0x54534750	/* 'T' 'S' 'G' 'P' */
struct MGVP_s {
	long	magic1;
	MGV	*m;
	int	*list;
	int	size;		/* current size			*/
	int	current;	/* current position		*/
	int	used;		/* number of entries used	*/
	long	magic2;
};

/*
 * Initial size and increment.
 */
#define	INCR	10

static int
pgs_ok(MGVP p)
{
	DIEIF(p == NULL);
	DIEIF(p->magic1 != PGS_MAGIC1);
	DIEIF(p->magic2 != PGS_MAGIC2);
	DIEIF(p->m      == NULL);
	return TRUE;
}

/*
 * When the list size (MGVP.size) is this value then
 * there is no list at all.  This will happen with non-DSC
 * documents where we can't switch to arbitrary pages.
 * Basically, this value serves as a flag which says "don't do
 * anything with p->list".
 */
#define	NODSC	-1

/*
 * This is just a nicer way of using NODSC.
 */
#define	haspgstack(p)	((p)->size != NODSC)

static void
show(MGV *m, int page)
{
	int      top, vis;
	Arg      a[2];
	Cardinal n;

	mgv_show(m, page);

	if(m->dsc == NULL)
		return;

	n = 0;
	XtSetArg(a[n], XmNtopItemPosition,  &top);	++n;
	XtSetArg(a[n], XmNvisibleItemCount, &vis);	++n;
	wlGetValues(m->pagelist, a, n);

	/*
	 * Convert to Motif List numbering.
	 */
	++page;

	/*
	 * Check visibility.
	 */
	if(page < top) {
		XmListSetPos(m->pagelist, page);
	}
	else if(page > top + vis - 1) {
		/*
		 * If we're near the end of the list we want to decrease
		 * the top position to keep the list's screen area full of
		 * entries (note:  if we XmListSetPos(m->pagelist,
		 * last_page_pos) the list will end up with one visible
		 * item, we wish to avoid this situation).
		 */
		if(page > m->dsc->n_pages - vis)
			page = m->dsc->n_pages - vis + 1;
		XmListSetPos(m->pagelist, page);
	}
}

MGVP
mgv_pgstack_alloc(MGV *m)
{
	MGVP	p = (MGVP)XtMalloc(sizeof(struct MGVP_s));

	p->magic1 = PGS_MAGIC1;
	p->magic2 = PGS_MAGIC2;

	/*
	 * If there is no DSC information then there's no
	 * point in having a page list.
	 */
	if(m->dsc != NULL) {
		p->size = INCR;
		p->list = (int *)XtCalloc(p->size, sizeof(int));
	}
	else {
		p->size = NODSC;
		p->list = NULL;
	}
	p->used    = 0;
	p->current = -1;
	p->m       = m;

	assert(pgs_ok(p));

	return p;
}

MGVP
mgv_pgstack_free(MGVP p)
{
	if(p == NULL)
		return p;
	assert(pgs_ok(p));
	XtFree((XtPointer)p->list);
	XtFree((XtPointer)p);
	return NULL;
}

static void
resize(MGVP p, int size)
{
	assert(pgs_ok(p));
	assert(haspgstack(p));

	if(p->size >= size + 1)
		return;

	while(p->size <= size)
		p->size += INCR;
	p->list = (int *)XtRealloc((XtPointer)p->list, p->size*sizeof(int));
}

void
mgv_pgstack_forward(MGVP p)
{
	int	sens = p->m->sens;

	assert(pgs_ok(p));
	assert(haspgstack(p));

	if(p->current == p->used - 1) {
		assert("Cannot go forward at the end of the list!" != NULL);
		return;
	}

	++p->current;

	if(p->current == p->used - 1)
		sens &= ~MgvSCANFORWARD;
	sens |= MgvSCANBACK;
	if(p->m->sens != sens)
		mgv_sens(p->m->senscache, p->m->sens = sens);

	show(p->m, p->list[p->current]);
}

void
mgv_pgstack_goto(MGVP p, int page)
{
	if(p == NULL)
		return;

	assert(pgs_ok(p));

	/*,,,
	 * This shouldn't happen but LessTif (as of 1998.08.06) is
	 * activating accelerators and mnemonics on insensitive widgets.
	 * Fear not gentle readers, a patch has been submitted.
	 */
	if(page < 0)
		return;

	if(haspgstack(p)
	&& (p->current == -1 || p->list[p->current] != page)) {
		int	sens = p->m->sens;

		resize(p, p->current + 1);
		p->list[++p->current] = page;
		p->used = p->current + 1;

		if(!(sens & MgvSCANBACK)
		&& p->current != 0)
			sens |= MgvSCANBACK;
		sens &= ~MgvSCANFORWARD;
		if(p->m->sens != sens)
			mgv_sens(p->m->senscache, p->m->sens = sens);
	}

	show(p->m, page);
}

void
mgv_pgstack_back(MGVP p)
{
	int	sens = p->m->sens;

	assert(pgs_ok(p));
	assert(haspgstack(p));

	if(p->current <= 0) {
		assert("Cannot go back at the beginning of the list!" != NULL);
		return;
	}

	--p->current;

	if(p->current == 0)
		sens &= ~MgvSCANBACK;
	sens |= MgvSCANFORWARD;
	if(sens != p->m->sens)
		mgv_sens(p->m->senscache, p->m->sens = sens);

	show(p->m, p->list[p->current]);
}

void
mgv_pgstack_prefill(MGVP p, int pages)
{
	int	i;

	assert(pgs_ok(p));

	if(!haspgstack(p))
		return;

	resize(p, pages);
	p->used = pages;
	for(i = 0; i < pages; ++i)
		p->list[i] = i;
	p->current = 0;
	mgv_sens(p->m->senscache, p->m->sens |= MgvSCANFORWARD);
}
