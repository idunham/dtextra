/*
 * version.c
 *	Version information generator for wlib.
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
MU_ID("$Mu: wlib/version.c 1.16 1998/09/01 20:49:44 $")

#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include <wlib/stdlib.h>
#include <wlib/build.h>
#include <wlib/wlib.h>

/*
 * Format string for sprintf.
 */
#define FORMAT	"version    = %s, %s\n"		\
		"compile    = %s\n"		\
		"ldflags    = %s\n"		\
		"libs       = %s\n"		\
		"uname      = %s %s %s, %s\n"	\
		"bindir     = %s\n"		\
		"datadir    = %s\n"		\
		"build date = %s"

/*
 * we could get the uname information at build time
 * by consulting uname(1) but this is easier than
 * relying on uname(1) and it tells us what the current
 * values are rather than what they were when mgv
 * was built.  The caller is responsible for the returned
 * memory (use XtFree).
 */
char *
wl_version(void)
{
	struct utsname	uts;
	char  *buf;
	size_t size;

	memset((void *)&uts, '\0', sizeof(uts));
	uname(&uts);

	/*
	 * I'm going to over allocate here because it is
	 * less error-prone than accounting for all of the
	 * %s's and spaces and such.
	 */
	size = strlen(bld_verstring)
	     + strlen(bld_tag)
	     + strlen(bld_compile)
	     + strlen(bld_ldflags)
	     + strlen(bld_libs)
	     + strlen(uts.sysname)
	     + strlen(uts.release)
	     + strlen(uts.version)
	     + strlen(uts.machine)
	     + strlen(bld_bindir)
	     + strlen(bld_datadir)
	     + strlen(bld_date)
	     + sizeof(FORMAT);

	if((buf = XtCalloc(size, 1)) == NULL)
		return buf;

	sprintf(buf, FORMAT,
			bld_verstring, bld_tag,
			bld_compile,
			bld_ldflags,
			bld_libs,
			uts.sysname, uts.release, uts.version, uts.machine,
			bld_bindir,
			bld_datadir,
			bld_date);
	return buf;
}
