/*
 * sanity.h
 *	Misc convenience functions/macros for sanity checks.
 *
 * Copyright (C) 1997 Eric A. Howe
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
#ifndef	WLIB_SANITY_H
#define	WLIB_SANITY_H
MU_HID(wlib_sanity_h, "$Mu: wlib/wlib/sanity.h 1.6 1998/08/27 03:22:41 $")

#include <assert.h>

#define	ARGSANITY(a,n)	assert((size_t)(n) <= sizeof(a)/sizeof((a)[0]))

/*
 * How many stack overruns do we have to see before people smarten up?
 * How many "Arg a[100];" declarations do we have to see before people
 * learn to count?
 */
#define wlGetValues(w,a,n) {		\
	ARGSANITY((a), (n));		\
	XtGetValues((w), (a), (n));	\
}
#define wlSetValues(w,a,n) {		\
	ARGSANITY((a), (n));		\
	XtSetValues((w), (a), (n));	\
}

/*
 * This is handy for assert(some_checks(x)) type things.
 */
#define	DIEIF(x)				\
	if(x) {					\
		fprintf(stderr, #x "\n");	\
		fflush(stderr);			\
		return FALSE;			\
	}

CDECLS_BEGIN
/*
 * This is the same as XFreePixmap() but it can deal with None pixmaps.
 * I got sick of all the silly "if this isn't dead, kill it" code, if
 * something is already dead, then killing it again should be very
 * easy so why should I have to check for life-signs all the time?
 * For convenience, this function always returns "None" so that you can
 * do
 *
 *	p = wl_free_pixmap(dpy, p);
 *
 * instead of
 *
 *	wl_free_pixmap(dpy, p);
 *	p = None;
 */
extern	Pixmap	wl_free_pixmap(Display *, Pixmap);
CDECLS_END

#endif
