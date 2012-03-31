/*
 * app.c
 *	Top level application resource handling
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
MU_ID("$Mu: mgv/app.c 1.77 1998/09/01 20:49:37 $")

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <Xm/Xm.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/build.h>
#include <wlib/sanity.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/strdefs.h>
#include <mine/strings.h>
#include <mine/track.h>
#include <mine/page.h>
#include <mine/mark.h>
#include <mine/zoom.h>
#include <mine/scroll.h>
#include <mine/util.h>

/*
 * Possible stat macro brain damage
 */
#if !defined(S_ISDIR)
#	define	S_ISDIR(m)	((m) & S_IFDIR)
#endif

/*
 * P_tmpdir hackery.
 */
#if !defined(P_tmpdir)
#	define	P_tmpdir	"/usr/tmp"
#endif

/*
 * Figure out a default printer name.
 *
 * LPDEST is for SYSV/lp based systems and PRINTER is for BSD/lpr
 * based systems but we'll check both.
 */
static String
defprinter(void)
{
	char	*e;

	if((e = getenv("LPDEST"))  == NULL
	&& (e = getenv("PRINTER")) == NULL)
		e = "ps";
	return e;
}

/*
 * probable print commands
 */
static struct {
	char	*command;
	char	*switches;
} printcmds[] = {
	{"lp",		" -d %s"},
	{"lpr",		" -P%s"},
	{"rlpr",	" -P%s"},
	{NULL}
};
#define N_CMD	(4 + 7)

/*
 * Default PATH setting.  I find the notion of not having a PATH a little
 * strange but I suppose it is possible.  Since we're only looking for
 * the print spooler command, this list should suffice.
 */
#define	PATH	"/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin"

/*
 * Figure out a default print command.
 */
static void
defprintcmd(Widget w, int offset, XrmValue *v)
{
	char	*buf, *s, *p;
	int	i;
	USEUP(offset); USEUP(w);

	if((s = getenv("PATH")) == NULL)
		s = PATH;
	p   = XtNewString(s);
	buf = XtMalloc(strlen(s) + N_CMD + 1);

	v->addr = NULL;
	v->size = 0;
	for(s = strtok(p, ":"); s != NULL; s = strtok(NULL, ":")) {
		for(i = 0; printcmds[i].command != NULL; ++i) {
			sprintf(buf, "%s/%s", s, printcmds[i].command);
			if(access(buf, X_OK) != 0)
				continue;
			strcat(buf, printcmds[i].switches);
			goto The_End;
		}
	}

The_End:
	XtFree(p);
	s = defprinter();
	p = XtCalloc(strlen(buf) + strlen(s) + 1, 1);
	sprintf(p, buf, s);

	v->addr = p;
	v->size = strlen(p);
	XtFree(buf);
}

/*
 * Figure out where to put temp files.
 */
#define	MODEWANT	(S_IRUSR | S_IWUSR | S_IXUSR)
static void
deftempdir(Widget w, int offset, XrmValue *v)
{
	struct stat	st;
	char	*dirs[3];
	int	i;
	USEUP(w); USEUP(offset);

	memset((void *)&st, '\0', sizeof(st));
	dirs[0] = getenv("TMPDIR");
	dirs[1] = getenv("TEMPDIR");
	dirs[2] = P_tmpdir;
	for(i = 0; i < (int)(sizeof(dirs)/sizeof(dirs[0])); ++i) {
		if(dirs[i]                 == NULL
		|| stat(dirs[i], &st)      != 0
		|| (st.st_mode & MODEWANT) != MODEWANT
		|| !S_ISDIR(st.st_mode))
			continue;
		v->size = strlen(dirs[i]);
		v->addr = XtNewString(dirs[i]);
		return;
	}

	v->size = sizeof("/tmp");
	v->addr = XtNewString("/tmp");
}

/*
 * Application resources.  Some of the resources need to have a realized
 * widget to get the correct colormap and what-not so we have to do this
 * in two passes.
 */
#define	OFFSET(f)	XtOffsetOf(MGV_APP, f)
static XtResource res_pass1[] = {
	{
		MgvNantialiasEPS, MgvCAntialiasEPS,
		XtRBoolean, sizeof(Boolean), OFFSET(antialias_eps),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNautoHeight, MgvCAutoHeight,
		XtRBoolean, sizeof(Boolean), OFFSET(auto_height),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNautoWidth, MgvCAutoWidth,
		XtRBoolean, sizeof(Boolean), OFFSET(auto_width),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNbunzip, MgvCBunzip,
		XtRString, sizeof(String), OFFSET(bunzip),
		XtRString, (XtPointer)"bunzip2"
	}, {
		MgvNbzip, MgvCBzip,
		XtRString, sizeof(String), OFFSET(bzip),
		XtRString, (XtPointer)"bzip2 -9"
	}, {
		MgvNcenter, MgvCCenter,
		XtRBoolean, sizeof(Boolean), OFFSET(center),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNcompress, MgvCCompress,
		XtRString, sizeof(String), OFFSET(compress),
		XtRString, (XtPointer)"compress"
	}, {
		MgvNconfirmExit, MgvCConfirmExit,
		XtRBoolean, sizeof(Boolean), OFFSET(confirm_exit),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNcopyStdin, MgvCCopyStdin,
		XtRBoolean, sizeof(Boolean), OFFSET(copy_stdin),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNdecompress, MgvCDecompress,
		XtRString, sizeof(String), OFFSET(decompress),
		XtRString, (XtPointer)"gzip --stdout --decompress"
	}, {
		MgvNdefaultPageMedia, MgvCDefaultPageMedia,
		MgvRPageMedia, sizeof(int), OFFSET(default_page_media),
		XtRString, (XtPointer)"Letter"
	}, {
		MgvNgunzip, MgvCGunzip,
		XtRString, sizeof(String), OFFSET(gunzip),
		XtRString, (XtPointer)"gzip --stdout --decompress"
	}, {
		MgvNgzip, MgvCGzip,
		XtRString, sizeof(String), OFFSET(gzip),
		XtRString, (XtPointer)"gzip -9"
	}, {
		MgvNhideLog, MgvCHideLog,
		XtRBoolean, sizeof(Boolean), OFFSET(hide_log),
		XtRString, (XtPointer)"True"
	}, {
		MgvNhideMenuBar, MgvCHideMenuBar,
		XtRBoolean, sizeof(Boolean), OFFSET(hide_menu),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNhideStatusLine, MgvCHideStatusLine,
		XtRBoolean, sizeof(Boolean), OFFSET(hide_status),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNhideToolBar, MgvCHideToolBar,
		XtRBoolean, sizeof(Boolean), OFFSET(hide_toolbar),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNhidePageList, MgvCHidePageList,
		XtRBoolean, sizeof(Boolean), OFFSET(hide_pglist),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNkillChildrenAtExit, MgvCKillChildrenAtExit,
		XtRBoolean, sizeof(Boolean), OFFSET(kill_kids),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNmagicResistance, MgvCMagicResistance,
		XtRInt, sizeof(int), OFFSET(magic_resistance),
		XtRImmediate, (XtPointer)2
	}, {
		MgvNmagicScrolling, MgvCMagicScrolling,
		XtRBoolean, sizeof(Boolean), OFFSET(magic_scroll),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNmagstep, MgvCMagstep,
		XtRInt, sizeof(int), OFFSET(magstep),
		XtRImmediate, (XtPointer)0
	}, {
		MgvNprintCommand, MgvCPrintCommand,
		XtRString, sizeof(String), OFFSET(printcmd),
		XtRCallProc, (XtPointer)defprintcmd
	}, {
		MgvNreticuleLineWidth, MgvCReticuleLineWidth,
		XtRInt, sizeof(int), OFFSET(reticule_width),
		XtRImmediate, (XtPointer)2
	}, {
		MgvNresetScrollBars, MgvCResetScrollBars,
		MgvRResetScrollBars, sizeof(int), OFFSET(reset_scroll),
		XtRImmediate, (XtPointer)MgvRESETSCROLL
	}, {
		MgvNscrollPercentage, MgvCScrollPercentage,
		XwlRPercentage, sizeof(int), OFFSET(scroll_percentage),
		XtRImmediate, (XtPointer)90
	}, {
		MgvNshowLabels, MgvCShowLabels,
		XtRBoolean, sizeof(Boolean), OFFSET(show_labels),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNshowPageNumbers, MgvCShowPageNumbers,
		XtRBoolean, sizeof(Boolean), OFFSET(show_pgnums),
		XtRImmediate, (XtPointer)True,
	}, {
		MgvNsmartSizing, MgvCSmartSizing,
		XtRBoolean, sizeof(Boolean), OFFSET(smart_sizing),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNsmoothScrolling, MgvCSmoothScrolling,
		XtRBoolean, sizeof(Boolean), OFFSET(smooth_scroll),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNsmoothness, MgvCSmoothness,
		XtRInt, sizeof(int), OFFSET(smoothness),
		XtRImmediate, (XtPointer)25
	}, {
		MgvNtempDir, MgvCTempDir,
		XtRString, sizeof(String), OFFSET(temp_dir),
		XtRCallProc, (XtPointer)deftempdir,
	}, {
		MgvNwmWidth, MgvCWmWidth,
		XtRInt, sizeof(int), OFFSET(wm_width),
		XtRImmediate, (XtPointer)14
	}, {
		MgvNwmHeight, MgvCWmHeight,
		XtRInt, sizeof(int), OFFSET(wm_height),
		XtRImmediate, (XtPointer)32
	}
};
static XtResource res_pass2[] = {
	{
		MgvNreticuleForeground, MgvCReticuleForeground,
		XtRPixel, sizeof(Pixel), OFFSET(reticule_fg),
		XtRString, (XtPointer)"red"
	}
};
#undef OFFSET

/*
 * resource converter for MgvRPageMedia
 */
static Boolean
string2media(Display *dpy, XrmValue *a, Cardinal *n, XrmValue *from,
						XrmValue *to, XtPointer *data)
{
	int	alloced = FALSE;
	char	*lower  = NULL;
	int	i;
	USEUP(a); USEUP(n); USEUP(data);

	/*
	 * Initial sanity checks.
	 */
	if(*n != 0) {
		XtWarningMsg("wrongParameters", "cvtStringToMedia",
			"XtToolkitError",
			"String to Media conversion needs no extra arguments",
			NULL, NULL);
	}
	if(from->addr == NULL)
		return False;
	if(to->addr != NULL && to->size < sizeof(int)) {
		to->size = sizeof(int);
		return False;
	}
	if(to->addr == NULL) {
		to->addr = XtMalloc(sizeof(int));
		to->size = sizeof(int);
		alloced = TRUE;
	}

	lower = lowdup((String)from->addr);
	for(i = 0; dsc_media[i].name != NULL; ++i) {
		if(strcmp(lower, dsc_media[i].name) != 0)
			continue;
		*(int *)(to->addr) = i;
		XtFree(lower);
		return True;
	}

	if(alloced) {
		XtFree(to->addr);
		to->addr = NULL;
	}
	XtFree(lower);
	XtDisplayStringConversionWarning(dpy, (char *)from->addr,MgvRPageMedia);
	return False;
}

/*
 * resource converter for MgvRResetScrollBars
 */
static Boolean
string2resetsb(Display *dpy, XrmValue *a, Cardinal *n, XrmValue *from,
						XrmValue *to, XtPointer *data)
{
	Boolean	alloced = False;
	int	bits    = -1;
	char	*lower  = NULL;
	USEUP(dpy); USEUP(a); USEUP(data);

	/*
	 * Initial sanity checks.
	 */
	if(*n != 0) {
		XtWarningMsg("wrongParameters", "cvtStringToResetScrollBars",
			"XtToolkitError",
			"String to ResetScrollBars conversion needs no extra arguments",
			NULL, NULL);
	}
	if(from->addr == NULL)
		return False;
	if(to->addr != NULL && to->size < sizeof(int)) {
		to->size = sizeof(int);
		return False;
	}
	if(to->addr == NULL) {
		to->addr = XtMalloc(sizeof(int));
		to->size = sizeof(int);
		alloced = TRUE;
	}

	lower = lowdup((String)from->addr);
	if(strcmp(lower, "all")  == 0
	|| strcmp(lower, "both") == 0
	|| strcmp(lower, "true") == 0)
		bits = MgvRESETSCROLL;
	else if(strcmp(lower, "none")  == 0
	     || strcmp(lower, "false") == 0)
		bits = 0;
	else if(strncmp(lower, "hor", 3) == 0)
		bits = MgvRESETSCROLL_H;
	else if(strncmp(lower, "ver", 3) == 0)
		bits = MgvRESETSCROLL_V;
	XtFree(lower);

	if(bits == -1) {
		if(alloced) {
			XtFree(to->addr);
			to->addr = NULL;
		}
		return False;
	}
	*(int *)(to->addr) = bits;

	return True;
}

/*
 * a doubly linked list makes deletion easier
 */
typedef struct MGVL_s {
	MGV		*m;
	struct MGVL_s	*next, *prev;
} MGVL;
static struct {
	/*
	 * Pass zero is unitialized, pass one is when we get most of
	 * the resources (everything that doesn't need a realized (ie a
	 * window, colormap, etc...) widget), and pass two is when we get
	 * the rest of the resources.
	 */
	int	pass;

	int	next;
	MGV_APP	res;
	int	n_mgvs;
	Widget	reaper;
	MGVL	*mgvs;
} app;

static XtActionsRec actions[] = {
	{"mgvMark",	mgv_action_mark},
	{"mgvScroll",	mgv_action_scroll},
	{"mgvZoom",	mgv_action_zoom},
	{"mgvPan",	mgv_action_pan}
};

static void
app_init(Widget w)
{
	if(app.pass >= 2)
		return;

	assert(w != NULL);
	if(app.pass < 1) {
		XtSetTypeConverter(XmRString, MgvRPageMedia,
				string2media, NULL, 0, XtCacheNone, NULL);
		XtSetTypeConverter(XmRString, MgvRResetScrollBars,
				string2resetsb, NULL, 0, XtCacheNone, NULL);
		XtGetApplicationResources(wl_top(w), &app.res,
				&res_pass1[0], XtNumber(res_pass1), NULL, 0);
		XtAppAddActions(XtWidgetToApplicationContext(w), actions,
				XtNumber(actions));
		wl_chkver(w);
		app.next = 1;
		++app.pass;

		/*
		 * Sanity checking.
		 */
		if(app.res.smoothness < 0)
			app.res.smoothness *= -1;
		else if(app.res.smoothness == 0)
			app.res.smoothness = 1;
		if(app.res.magic_resistance < 0)
			app.res.magic_resistance *= -1;
		if(app.res.printcmd == NULL)
			app.res.printcmd = "";

		/*
		 * We may want to change these sometime so we'll make
		 * copies that we know we can free.
		 */
		app.res.bunzip     = XtNewString(app.res.bunzip);
		app.res.bzip       = XtNewString(app.res.bzip);
		app.res.decompress = XtNewString(app.res.decompress);
		app.res.compress   = XtNewString(app.res.compress);
		app.res.gunzip     = XtNewString(app.res.gunzip);
		app.res.gzip       = XtNewString(app.res.gzip);
		app.res.temp_dir   = XtNewString(app.res.temp_dir);
		app.res.printcmd   = XtNewString(app.res.printcmd);
		app.res.printargv  = stoargv(app.res.printcmd);
	}
	else if(XtIsRealized(w)) {
		XtGetApplicationResources(wl_top(w), &app.res,
				&res_pass2[0], XtNumber(res_pass2), NULL, 0);
		++app.pass;
	}
}

static int
app_ok(void)
{
	DIEIF(app.pass   >  2);
	DIEIF(app.n_mgvs <= 0);
	DIEIF(app.mgvs   == NULL);
	DIEIF(app.next   <= 0);
	return TRUE;
}

void
mgv_app_close(Widget w, XtPointer closure, XtPointer call)
{
	struct sigaction sa;
	MGV     *m = (MGV *)closure;
	MGVL    *e;
	Arg      a[2];
	Cardinal n;
	USEUP(w); USEUP(call);

	assert(app_ok());
	assert(MgvOK(m));

	for(e = app.mgvs; e != NULL && e->m != m; e = e->next)
		;
	assert(e != NULL);
	if(e->prev != NULL)
		e->prev->next = e->next;
	else
		app.mgvs = e->next;
	if(e->next != NULL)
		e->next->prev = e->prev;

	mgv_close(m, FALSE, TRUE);

	n = 0;
	XtSetArg(a[n], XmNiconPixmap, None);	++n;
	XtSetArg(a[n], XmNiconMask,   None);	++n;
	wlSetValues(wl_top(m->main), a, n);

	wl_free_pixmap(XtDisplay(m->main), m->icon);
	wl_free_pixmap(XtDisplay(m->main), m->mask);
	mgv_free(m);
	XtFree((void *)e);

	if(--app.n_mgvs == 0) {
		if(app.res.kill_kids) {
			memset((void *)&sa, '\0', sizeof(sa));
			sa.sa_handler = SIG_IGN;
			sigemptyset(&sa.sa_mask);
			sigaction(SIGTERM, &sa, NULL);
			kill(-getpgrp(), SIGTERM);
		}

		/*
		 * Remember that we made copies of these in the
		 * resource initialization phase.
		 */
		XtFree((XtPointer)app.res.bunzip);
		XtFree((XtPointer)app.res.bzip);
		XtFree((XtPointer)app.res.decompress);
		XtFree((XtPointer)app.res.compress);
		XtFree((XtPointer)app.res.gunzip);
		XtFree((XtPointer)app.res.gzip);
		XtFree((XtPointer)app.res.temp_dir);
		XtFree((XtPointer)app.res.printcmd);
		XtFree((XtPointer)*app.res.printargv);
		XtFree((XtPointer)app.res.printargv);

		wl_shutdown();

		exit(EXIT_SUCCESS);
	}
}

static void
app_death(Widget w, XtPointer closure, XtPointer call)
{
	MGVL	*e, *next;

	USEUP(w); USEUP(closure); USEUP(call);
	assert(app_ok());

	/*
	 * The final destroy will exit() without any help.
	 */
	for(e = app.mgvs; e != NULL; e = next) {
		next = e->next;
		XtDestroyWidget(wl_top(e->m->main));
	}
}

static void
confirm_destroy(Widget w, XtPointer closure, XtPointer call)
{
	USEUP(w); USEUP(closure); USEUP(call);
	app.reaper = NULL;
}

static WLW exit_query[] = {
	{"reallyExit",	wlQuestionDialog,	0,	NULL},
	{NULL}
};
static WLC exit_query_cb[] = {
	{"*reallyExit",	XmNunmapCallback,	NULL, wl_dismiss},
	{"*reallyExit",	XmNcancelCallback,	NULL, wl_dismiss},
	{"*reallyExit",	XmNhelpCallback,	NULL, wl_help},
	{"*reallyExit",	XmNokCallback,		NULL, app_death},
	{"*reallyExit",	XmNdestroyCallback,	NULL, confirm_destroy},
	{NULL}
};

void
mgv_app_exit(Widget w, XtPointer closure, XtPointer call)
{
	assert(app_ok());
	if(app.n_mgvs <= 1 || !app.res.confirm_exit) {
		app_death(w, closure, call);
		return;
	}

	if(app.reaper == NULL) {
		app.reaper = wl_create(wl_top(w), exit_query);
		wl_callback(XtParent(app.reaper), &exit_query_cb[0], NULL);
	}
	wl_dlg_popup(app.reaper);
}

MGV_APP *
mgv_appres(Widget w)
{
	app_init(w);
	return &app.res;
}

MGV *
mgv_app_add(MGV *m)
{
	MGVL	*e;

	app_init(m->main);

	if((e = (MGVL *)XtCalloc(1, sizeof(MGVL))) == NULL)
		return NULL;
	e->m = m;
	if((e->next = app.mgvs) != NULL)
		e->next->prev = e;
	app.mgvs = e;
	++app.n_mgvs;
	return m;
}

int
mgv_app_nextinst(void)
{
	return app.next++;
}
