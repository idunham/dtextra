/*
 * help-libhelp.c
 *	Extra help support for the libhelp help system.
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
#include <wlib/rcs.h>
MU_ID("$Mu: wlib/help-libhelp.c 1.22 1998/09/02 02:49:43 $")

#include <stdio.h>
#include <string.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/wlibP.h>

#if HAVE_LIBHELP

#include <help.h>

/*
 * Libhelp initialization.  You'll notice that there are several memory leaks
 * here.  Since libhelp doesn't make copies of its string resources, we have
 * no choice but to let things leak or use static buffers and I prefer the the
 * leaks since this function is only called once. I sent a patch to the
 * libhelp author to fix this bug but I never received a response (the patch
 * also removed the dependence on environment variables) so I'll just have
 * to keep using this kludge.  Alas, the patch has been lost so I'm going
 * to (eventually) switch to an XmHTML implementation.
 */
static int
init(Widget w)
{
	char *indexfile, *path, *cls;	/* these leak, we have no choice */
	char *libhelppath, *prefix, *s, *name;
	int  size;

	/*
	 * We have to set the LIBHELPPATH in the environment since
	 * there is no other way to set the search path.  I could
	 * just assume that people will set LIBHELPPATH but overloading
	 * helpPrefix seems like a sensible thing to do.  I sent a
	 * patch to the libhelp people awhile ago that put this into
	 * the help_set_resource() API but I haven't heard from them.
	 */
	libhelppath = getenv("LIBHELPPATH");
	prefix      = wlp_s(w, WlpShelpPrefix);
	if((s = strchr(prefix, ':')) != NULL
	&& *(s + 1) == '/'
	&& *(s + 2) == '/')
		prefix = s + 3;
	size = sizeof("LIBHELPPATH=") + strlen(prefix);
	if(libhelppath == NULL) {
		path = XtCalloc(1, size);
		s    = "LIBHELPPATH=%s";
	}
	else {
		path = XtCalloc(1, size + strlen(libhelppath) + 1);
		s    = "LIBHELPPATH=%s:%s";
	}
	if(path == NULL)
		return FALSE;
	sprintf(path, s, prefix, libhelppath);
	putenv(path);

	name = cls = NULL;
	wl_appnames(w, &name, &cls);
	indexfile = XtCalloc(strlen(name) + 5 + 1, 1);
	sprintf(indexfile, "%s.html", name);
	XtFree(name);

	help_set_resource(help_class_name, (XtPointer)cls);
	help_set_resource(help_index, (XtPointer)indexfile);
	help_set_resource(help_server, (XtPointer)XMHELP_SERVER);

	return TRUE;
}

/*
 * libhelp stuff
 */
Boolean
wlp_help_libhelp(Widget w, WLP_HELP *h)
{
static	int    initted = FALSE;
	char  *buf  = NULL;
	size_t size = 0;

	if(!initted
	&& !(initted = init(w)))
		return False;

	size = strlen(h->chapter) + 1;
	if(h->section == NULL) {
		buf = XtMalloc(size);
		sprintf(buf, "%s.html", h->chapter);
	}
	else {
		size += strlen(h->section) + 1;
		buf = XtMalloc(size);
		sprintf(buf, "%s.html#%s", h->chapter, h->section);
	}
	get_help(w, (XtPointer)buf, NULL);
	XtFree(buf);
	return True;
}

#else

/*
 * stub for lackers of libhelp
 */
Boolean
wlp_help_libhelp(Widget w, WLP_HELP *h)
{
static	Boolean	warned = False;
	if(!warned) {
		warned = True;
		wl_warn(wl_top(w), wlp_s(w, WlpSnotSupported),"libhelp","HTML");
	}
	return wlp_help_html(w, h);
}

#endif
