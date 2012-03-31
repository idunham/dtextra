/*
 * build.h
 *	Declarations for variables that are defined via the build system.
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
#ifndef	WLIB_BUILD_H
#define	WLIB_BUILD_H
MU_HID(wlib_build_h, "$Mu: wlib/wlib/build.h 1.14 1998/08/19 03:51:12 $")

/*
 * fallback resources
 */
extern char *bld_fallbacks[];

/*
 * version information
 */
extern char *bld_verstring;
extern char *bld_tag;

/*
 * compiler and linker stuff
 */
extern char *bld_compile;
extern char *bld_ldflags;
extern char *bld_libs;

/*
 * build date (yyyy/mm/dd HH:MM UTC)
 */
extern char *bld_date;

/*
 * Install directories.
 */
extern char *bld_bindir;
extern char *bld_datadir;

/*
 * NULL terminated arrays for "-help" and "-longhelp"
 */
extern char *bld_switch_help[];
extern char *bld_switch_longhelp[];

#endif
