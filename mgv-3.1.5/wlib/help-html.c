/*
 * help-html.c
 *	Extra help support for HTML.
 *
 * Copyright (C) 1996  Matthew D. Francey
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
MU_ID("$Mu: wlib/help-html.c 1.41.4.1 1998/09/15 05:12:20 $")

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/wlibP.h>
#include <wlib/sanity.h>

static char *
urlify(char **url, Widget w, WLP_HELP *h)
{
	char  *browser, *prefix;
	size_t length;

	browser = wlp_s(w, WlpShelpBrowser);
	prefix  = wlp_s(w, WlpShelpPrefix);

	/*
	 * "<prefix><chapter>.html"
	 */
	length = strlen(prefix) + strlen(h->chapter) + 5 + 1;

	if(h->section == NULL) {
		*url = XtMalloc(length);
		sprintf(*url, "%s%s.html", prefix, h->chapter);
	}
	else {
		length += strlen(h->section) + 1;
		*url    = XtMalloc(length);
		sprintf(*url, "%s%s.html#%s", prefix, h->chapter, h->section);
	}

	return browser;
}

static pid_t
launch(char **argv)
{
	pid_t	brat;

	brat = fork();
	switch(brat) {
	case 0:
		execvp(*argv, argv);
		fprintf(stderr, "could not execute %s: %s\n",
						*argv, strerror(errno));
		exit(EXIT_FAILURE);
		break;
	case -1:
		break;
	default:
		break;
	}
	return brat;
}

/*
 * plain vanilla HTML help with no browser communication 
 */
Boolean
wlp_help_html(Widget w, WLP_HELP *h)
{
	char	*display, *fmt;
	char	*url, *argv[5];
	int	n;

	n = 0;
	argv[n++] = urlify(&url, w, h);
	if((display = DisplayString(XtDisplay(wl_top(w)))) != NULL) {
		argv[n++] = "-display";
		argv[n++] = display;
	}
	argv[n++] = url;
	argv[n++] = NULL;
	ARGSANITY(argv, n);

	if(launch(&argv[0]) == -1) {
		int e = errno;
		fmt = wlp_s(w, WlpScantBrowse);
		wl_warn(wl_top(w), fmt, argv[0], strerror(e));
	}

	XtFree(url);

	return True;
}

/*
 * X error handler for catching BadWindow
 */
static	int		badwindow      = FALSE;
static	XErrorHandler	default_xerror = NULL;
static int
badwin_catcher(Display *dpy, XErrorEvent *ev)
{
	badwindow = (ev->error_code == BadWindow);
	return badwindow ? 0 : default_xerror(dpy, ev);
}

/*
 * XmuClientWindow replacement.  Many people don't have Xmu
 * for some reason.
 */
static Window
recurse(Display *dpy, Window win, Atom wm_state)
{
	Atom		actual_type = None;
	int		format;
	unsigned long	n_items, after;
	unsigned char	*prop;
	Window		root, found, parent;
	Window		*kids;
	unsigned int	n_kids, i;

	if(!XQueryTree(dpy, win, &root, &parent, &kids, &n_kids))
		return None;
	for(i = 0, found = None; found == None && i < n_kids; ++i) {
		XGetWindowProperty(dpy, kids[i], wm_state, 0, 0, False,
				AnyPropertyType, &actual_type, &format,
				&n_items, &after, &prop);
		if(actual_type != None)
			found = kids[i];
	}

	for(i = 0; found == None && i < n_kids; ++i)
		found = recurse(dpy, kids[i], wm_state);

	if(kids != NULL)
		XFree((void *)kids);
	return found;
}

static Window
client_window(Display *dpy, Window win)
{
	Atom		wm_state, actual_type;
	int		format;
	unsigned long	n_items, after;
	unsigned char	*prop;
	Window		kid;

	wm_state = actual_type = None;

	if((wm_state = XInternAtom(dpy, "WM_STATE", True)) == None)
		return win;
	XGetWindowProperty(dpy, win, wm_state, 0, 0, False, AnyPropertyType,
			&actual_type, &format, &n_items, &after, &prop);
	if(actual_type != None)
		return win;
	return (kid = recurse(dpy, win, wm_state)) == None ? win : kid;
}

static Window
winping(Display *dpy, Window win, char *name, char *classname)
{
	XClassHint	hints = {NULL, NULL};
	int		status;

	if(win == None)
		return None;

	/*
	 * we have to watch for BadWindow errors in here
	 */
	XSync(dpy, False);
	default_xerror = XSetErrorHandler(badwin_catcher);
	win = client_window(dpy, win);
	XSync(dpy, False);
	if(win == None || badwindow) {
		win = None;
		goto Hell;
	}
	status = XGetClassHint(dpy, win, &hints);
	XSync(dpy, False);
	if(status == 0 || badwindow) {
		win = None;
		goto Hell;
	}
	if(strcmp(name, hints.res_name)       != 0
	|| strcmp(classname, hints.res_class) != 0)
		win = None;
	XFree(hints.res_name);
	XFree(hints.res_class);
Hell:
	XSetErrorHandler(default_xerror);
	badwindow = FALSE;
	return win;
}

static Window
winfind(Display *dpy, Window guess, char *name, char *cls)
{
	Window		root, win, *kids;
	unsigned	n_kids, i;

	if((win = winping(dpy, guess, name, cls)) != None)
		return win;
	root = RootWindow(dpy, DefaultScreen(dpy));
	if(XQueryTree(dpy, root, &win, &win, &kids, &n_kids) == 0)
		return None;
	for(i = 0, win = None; win == None && i < n_kids; ++i)
		win = winping(dpy, kids[i], name, cls);
	XFree(kids);

	return win;
}

/*
 * HTML help with netscape's -remote thing
 */
Boolean
wlp_help_remote(Widget w, WLP_HELP *h)
{
static	Window	ns = None;
	Display	*dpy;
	char	*display;
	char	*url, *argv[8];
	char	id[64], *act = NULL;
	int	n;
	pid_t	brat;

	dpy = XtDisplay(wl_top(w));
	n = 0;
	argv[n++] = urlify(&url, w, h);
	if((display = DisplayString(dpy)) != NULL) {
		argv[n++] = "-display";
		argv[n++] = display;
	}
	if((ns = winfind(dpy, ns, "Navigator", "Netscape")) != None) {
		sprintf(id, "%ld", (long)ns);
		act = XtMalloc(strlen(url) + sizeof("openURL()"));
		sprintf(act, "openURL(%s)", url);
		argv[n++] = "-id";
		argv[n++] = &id[0];
		argv[n++] = "-remote";
		argv[n++] = act;
	}
	else {
		argv[n++] = url;
	}
	argv[n++] = NULL;
	ARGSANITY(argv, n);

	if((brat = launch(&argv[0])) == -1) {
		int e = errno;
		wl_warn(wl_top(w), wlp_s(w,WlpScantBrowse),argv[0],strerror(e));
	}
	if(ns != None)
		waitpid(brat, NULL, 0);

	XtFree(url);
	XtFree(act);

	return True;
}
