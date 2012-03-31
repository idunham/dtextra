/*
 * stdlib.h
 *	stdlib.h wrapper for dmalloc things.  This is generally included
 *	instead of stdlib.h to get dmalloc support automagically.
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
#ifndef	WLIB_STDLIB_H
#define	WLIB_STDLIB_H
MU_HID(wlib_stdlib_h, "$Mu: wlib/wlib/stdlib.h 1.2 1998/09/02 02:49:48 $")

#include <stdlib.h>
#if USE_DMALLOC
#	include <dmalloc.h>
#endif

#if !HAVE_PUTENV
	/*
	 * I'm just going to assume that you really do have `putenv'
	 * but some kind of macro madness in your header files is
	 * hiding it from us.
	 */
	extern	int	putenv(const char *);
#endif

#endif
