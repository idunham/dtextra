/*
 * rcs.h
 *	RCS keyword imbedding macros for wlib.
 *
 * Copyright (C) 1996  Eric A. Howe
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#ifndef	WLIB_RCS_H
#define	WLIB_RCS_H

/*
 * A tricky way to get a string in a compiled file without polluting the
 * global name space.  The little mess will also keep 'gcc -Wall' quiet
 * (thus allowing you to use '-Werror' to be a bastard _and_ get your
 * version strings in the compiled file).  Note that we prefix a "@(#)"
 * on the version strings so that what(1) (from SCCS) can find the
 * version strings too.
 *
 * Use MU_ID() for C files and MU_HID() for headers
 *
 * Things like:
 *
 *	static const char rcsid[] = ......
 *
 * will also work with gcc but I know that this version should work
 * everywhere.
 */
#define	MU_IDBASE(s,x) \
		static char	Mu_iD_##s[]     = "@(#)" x; \
		static int	Mu_iD_trick_##s = sizeof(Mu_iD_trick_##s) \
						+ sizeof(Mu_iD_##s);
#if defined(NORCS)
#	define	MU_ID(x)
#else
#	define	MU_ID(x)	MU_IDBASE(SoUrCeFiLe, x)
#endif

/*
 * Almost never want these.
 */
#define	NOHRCS
#if defined(NOHRCS)
#	define	MU_HID(s, x)
#else
#	define	MU_HID(s, x)	MU_IDBASE(s, x)
#endif

MU_HID(wlib_rcs_h, "$Mu: wlib/wlib/rcs.h 1.16 1998/08/27 03:22:41 $")

/*
 * Some compilers complain about function parameters that don't
 * get used (c89 on Sinix and "gcc -Wall -W" come to mind).  Rather than
 * turn off the warnings or trying to just ignore them, use this simple little
 * macro to "use up" parameters.
 */
#if !defined(__cplusplus)
#	define	USEUP(x)	(void)x
#endif

/*
 * These macros are in this file because everyone includes it and
 * many files need them.
 */
#if !defined(TRUE)
#	define	TRUE	1
#endif
#if !defined(FALSE)
#	define	FALSE	0
#endif

/*
 * Standard C++ things for the heretics :)
 */
#if defined(__cplusplus)
#	define	CDECLS_BEGIN	extern "C" {
#	define	CDECLS_END	}
#else
#	define	CDECLS_BEGIN
#	define	CDECLS_END
#endif

#endif
