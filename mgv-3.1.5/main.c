/*
 * main.c
 *	Initialization and entry to mgv.
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
/*
 * application initialization
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/main.c 1.188 1998/09/01 20:49:38 $")

#include <signal.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/build.h>
#include <wlib/icons.h>
#include <wlib/WlAppShell.h>
#include <wlib/WlSplashScreen.h>
#include <wlib/WlToolBar.h>
#include <mine/mgv.h>
#include <mine/help.h>
#include <mine/strdefs.h>
#include <mine/strings.h>
#include <mine/page_control.h>
#include <lib/icon.xpm>
#include <lib/icon.xbm>
#include <lib/mask.xbm>

static XrmOptionDescRec options[] = {
	/*
	 * MGv resources
	 */
	{"-aliaseps",       "*antialiasEPS",        XrmoptionNoArg,  "True"},
	{"-autoheight",     "*autoHeight",          XrmoptionNoArg,  "True"},
	{"-autowidth",      "*autoWidth",           XrmoptionNoArg,  "True"},
	{"-bunzip",         "*bunzip",              XrmoptionSepArg, NULL},
	{"-bzip",           "*bzip",                XrmoptionSepArg, NULL},
	{"-center",         "*center",              XrmoptionNoArg,  "True"},
	{"-compress",       "*compress",            XrmoptionSepArg, NULL},
	{"-confirmexit",    "*confirmExit",         XrmoptionNoArg,  "True"},
	{"-copystdin",      "*copyStdin",           XrmoptionNoArg,  "True"},
	{"-decompress",     "*decompress",          XrmoptionSepArg, NULL},
	{"-gunzip",         "*gunzip",              XrmoptionSepArg, NULL},
	{"-gzip",           "*gzip",                XrmoptionSepArg, NULL},
	{"-killkids",       "*killChildrenAtExit",  XrmoptionNoArg,  "True"},
	{"-labels",         "*showLabels",          XrmoptionNoArg,  "True"},
	{"-log",            "*hideLog",             XrmoptionNoArg,  "False"},
	{"-magicresistance","*magicResistance",     XrmoptionSepArg, NULL},
	{"-magicscroll",    "*magicScrolling",      XrmoptionNoArg,  "True"},
	{"-magstep",        "*magstep",             XrmoptionSepArg, NULL},
	{"-media",          "*defaultPageMedia",    XrmoptionSepArg, NULL},
	{"-menubar",        "*hideMenuBar",         XrmoptionNoArg,  "False"},
	{"-noaliaseps",     "*antialiasEPS",        XrmoptionNoArg,  "False"},
	{"-noautoheight",   "*autoHeight",          XrmoptionNoArg,  "False"},
	{"-noautowidth",    "*autoWidth",           XrmoptionNoArg,  "False"},
	{"-nocenter",       "*center",              XrmoptionNoArg,  "False"},
	{"-noconfirmexit",  "*confirmExit",         XrmoptionNoArg,  "False"},
	{"-nocopystdin",    "*copyStdin",           XrmoptionNoArg,  "False"},
	{"-nokillkids",     "*killChildrenAtExit",  XrmoptionNoArg,  "False"},
	{"-nolabels",       "*showLabels",          XrmoptionNoArg,  "False"},
	{"-nolog",          "*hideLog",             XrmoptionNoArg,  "True"},
	{"-nomagicscroll",  "*magicScrolling",      XrmoptionNoArg,  "True"},
	{"-nomenubar",      "*hideMenuBar",         XrmoptionNoArg,  "True"},
	{"-nopagelist",     "*hidePageList",        XrmoptionNoArg,  "True"},
	{"-nopgnums",       "*showPageNumbers",     XrmoptionNoArg,  "False"},
	{"-nosmartsizing",  "*smartSizing",         XrmoptionNoArg,  "False"},
	{"-nosmooth",       "*smoothScrolling",     XrmoptionNoArg,  "False"},
	{"-nostatus",       "*hideStatusLine",      XrmoptionNoArg,  "True"},
	{"-notoolbar",      "*hideToolBar",         XrmoptionNoArg,  "True"},
	{"-pagelist",       "*hidePageList",        XrmoptionNoArg,  "False"},
	{"-paper",          "*defaultPageMedia",    XrmoptionSepArg, NULL},
	{"-pgnums",         "*showPageNumbers",     XrmoptionNoArg,  "True"},
	{"-printcommand",   "*printCommand",        XrmoptionSepArg, NULL},
	{"-resetboth",      "*resetScrollBars",     XrmoptionNoArg,  "both"},
	{"-resethor",       "*resetScrollBars",     XrmoptionNoArg,  "hor"},
	{"-resetnone",      "*resetScrollBars",     XrmoptionNoArg,  "none"},
	{"-resetver",       "*resetScrollBars",     XrmoptionNoArg,  "ver"},
	{"-reticulefg",     "*reticuleForeground",  XrmoptionSepArg, NULL},
	{"-reticulewidth",  "*reticuleLineWidth",   XrmoptionSepArg, NULL},
	{"-scrollpercent",  "*scrollPercentage",    XrmoptionSepArg, NULL},
	{"-smartsizing",    "*smartSizing",         XrmoptionNoArg,  "True"},
	{"-smooth",         "*smoothScrolling",     XrmoptionNoArg,  "True"},
	{"-smoothness",     "*smoothness",          XrmoptionSepArg, NULL},
	{"-status",         "*hideStatusLine",      XrmoptionNoArg,  "False"},
	{"-temp",           "*tempDir",             XrmoptionSepArg, NULL},
	{"-toolbar",        "*hideToolBar",         XrmoptionNoArg,  "False"},
	{"-wmheight",       "*wmHeight",            XrmoptionSepArg, NULL},
	{"-wmwidth",        "*wmWidth",             XrmoptionSepArg, NULL},
	{"-zoomfg",         "*reticuleForeground",  XrmoptionSepArg, NULL},
	{"-zoomwidth",      "*reticuleLineWidth",   XrmoptionSepArg, NULL},

	/*
	 * wlib resources
	 */
	{"-browser",	"*helpBrowser",		XrmoptionSepArg, NULL},
	{"-helptype",	"*helpType",		XrmoptionSepArg, NULL},
	{"-prefix",	"*helpPrefix",		XrmoptionSepArg, NULL},
	{"-noicon",     "*wlInstallIcon",	XrmoptionNoArg,  "False"},
	{"-installicon","*wlInstallIcon",	XrmoptionNoArg,  "True"},
	{"-coloricon",	"*wlUseColorIcon",	XrmoptionNoArg,  "True"},
	{"-monoicon",	"*wlUseColorIcon",	XrmoptionNoArg,  "False"},
	{"-nocoloricon","*wlUseColorIcon",	XrmoptionNoArg,  "False"},

	/*
	 * WlAppShell
	 */
	{"-depth",	"*wlDepth",		XrmoptionSepArg, NULL},
	{"-editres",	"*wlUseEditRes",	XrmoptionNoArg,  "True"},
	{"-install",	"*wlUsePrivateColormap",XrmoptionNoArg,	"True"},
	{"-noeditres",	"*wlUseEditRes",	XrmoptionNoArg,  "False"},
	{"-noinstall",	"*wlUsePrivateColormap",XrmoptionNoArg,	"True"},
	{"-noprivatecm","*wlUsePrivateColormap",XrmoptionNoArg,  "False"},
	{"-privatecm",	"*wlUsePrivateColormap",XrmoptionNoArg,  "True"},
	{"-visual",	"*wlVisualClass",	XrmoptionSepArg, NULL},
	{"-visualid",	"*wlVisualID",		XrmoptionSepArg, NULL},

	/*
	 * WlSplashScreen
	 */
	{"-colorsplash","*wlUseMonoSplash",	XrmoptionNoArg,  "False"},
	{"-monosplash",	"*wlUseMonoSplash",	XrmoptionNoArg,  "True"},
	{"-nosplash",	"*wlShowSplashScreen",	XrmoptionNoArg,  "False"},
	{"-splash",	"*wlShowSplashScreen",	XrmoptionNoArg,  "True"},
	{"-splashxbm",	"*wlSplashBitmap",	XrmoptionSepArg, NULL},
	{"-splashxpm",	"*wlSplashPixmap",	XrmoptionSepArg, NULL},

	/*
	 * WlToolBar
	 */
	{"-noflatten",    "*flattenButtons",	XrmoptionNoArg,  "False"},
	{"-flatten",      "*flattenButtons",	XrmoptionNoArg,  "True"},
	{"-notooltips",   "*showToolTips",	XrmoptionNoArg,  "False"},
	{"-tooltips",     "*showToolTips",	XrmoptionNoArg,  "True"},
	{"-colortoolbar", "*useColorIcons",	XrmoptionNoArg,  "True"},
	{"-monotoolbar",  "*useColorIcons",	XrmoptionNoArg,  "False"},

	/*
	 * Ghostview resources
	 */
#	define	SGV(x)	"*Ghostview." #x
	{"-antialias",	SGV(antialias),		XrmoptionNoArg,  "True"},
	{"-arguments",	SGV(arguments),		XrmoptionSepArg, NULL},
	{"-color",	SGV(palette),		XrmoptionNoArg,  "Color"},
	{"-dpi",	SGV(Resolution),	XrmoptionSepArg, NULL},
	{"-grayscale",	SGV(palette),		XrmoptionNoArg,  "Grayscale"},
	{"-greyscale",	SGV(palette),		XrmoptionNoArg,  "Grayscale"},
	{"-gslocale",	SGV(gsLocale),		XrmoptionSepArg, NULL},
	{"-interpreter",SGV(interpreter),	XrmoptionSepArg, NULL},
	{"-landscape",	SGV(orientation),	XrmoptionNoArg,  "landscape"},
	{"-monochrome",	SGV(palette),		XrmoptionNoArg,  "Monochrome"},
	{"-noantialias",SGV(antialias),		XrmoptionNoArg,  "False"},
	{"-noquiet",	SGV(quiet),		XrmoptionNoArg,  "False"},
	{"-nosafer",	SGV(safer),		XrmoptionNoArg,  "False"},
	{"-portrait",	SGV(orientation),	XrmoptionNoArg,  "portrait"},
	{"-quiet",	SGV(quiet),		XrmoptionNoArg,  "True"},
	{"-resolution",	SGV(Resolution),	XrmoptionSepArg, NULL},
	{"-safer",	SGV(safer),		XrmoptionNoArg,  "True"},
	{"-seascape",	SGV(orientation),	XrmoptionNoArg,  "seascape"},
	{"-upsidedown",	SGV(orientation),	XrmoptionNoArg,  "upsidedown"},
	{"-xdpi",	SGV(xdpi),		XrmoptionSepArg, NULL},
	{"-ydpi",	SGV(ydpi),		XrmoptionSepArg, NULL},
#	undef SGV
};

static int
helpem_long(char *me, int ret)
{
	int	i;

	printf("%s [options] [files...]\n", me);
	for(i = 0; bld_switch_longhelp[i] != NULL; ++i)
		printf("%s\n", bld_switch_longhelp[i]);
	return ret;
}

#define	LINELEN	75
static int
helpem(char *me, int ret)
{
	int	i, len, slen;

	printf("%s [options] [files...]", me);
	for(i = 0, len = LINELEN + 100; bld_switch_help[i] != NULL; ++i) {
		slen = strlen(bld_switch_help[i]);
		if(len + slen + 1 > LINELEN) {
			printf("\n\t");
			len = 8;
		}
		printf("%s ", bld_switch_help[i]);
		len += slen + 1;
	}
	printf("\n");
	return ret;
}

static int
version(void)
{
	char *buf;

	printf("%s\n\n", buf = wl_version());
	printf("Copyright (c) 1996-1998 Eric A. Howe, Matthew D. Francey\n");
	printf("This is free software with ABSOLUTELY NO WARRANTY.  ");
	printf("See COPYING for details.\n");
	XtFree(buf);
	return EXIT_SUCCESS;
}

/*
 * POSIX doesn't say that fileno() is safe in a signal handler
 * so we grab the file descriptor of stderr (POSIX doesn't appear
 * to force it be 2 even though we all think it is).
 *
 * Okay, okay, we're only using this in a signal handler for SIGSEGV
 * and friends so who cares about safety?  I do, if I'm going to screw up,
 * I'd like to do it properly.
 */
static	int	fd_stderr;

/*
 * Some people don't have SIGBUS but I don't want to put in more
 * than one ifdef to handle it.  Similar problems seem to affect SIGTRAP.
 */
#if !defined(SIGBUS)
#	define	SIGBUS	0
#endif
#if !defined(SIGTRAP)
#	define	SIGTRAP	0
#endif

static void flaming_death(int);
#define	DEATH(sig)	{sig, flaming_death, #sig, sizeof(#sig)}
/*
 * the name field, and hence the n_name field, is only needed if
 * the handler is set to flaming_death
 */
static struct {
	int	sig;
	void	(*handler)(int);
	char	*name;
	int	n_name;
} sigs[] = {
	DEATH(SIGABRT),
	DEATH(SIGBUS),
	DEATH(SIGFPE),
	DEATH(SIGILL),
	DEATH(SIGSEGV),
	DEATH(SIGTRAP),

	{SIGCHLD, SIG_DFL},
	{SIGPIPE, SIG_DFL},
	{SIGTERM, SIG_DFL},
};
#define	N_SIGS	(int)(sizeof(sigs)/sizeof(sigs[0]))

/*
 * Just because I screwed up doesn't mean I have to be
 * messy about it!
 */
static char swansong[] =
	"\tSomething in mgv broke!\n"
	"\tYou found a bug in mgv--I'm trying to die gracefully--if you\n"
	"\tcan reproduce the bug, please send a bug report to mu@trends.net\n";
static void
flaming_death(int sig)
{
	int	i;

	for(i = 0; i < N_SIGS; ++i)
		if(sigs[i].sig == sig)
			break;
	if(i != N_SIGS)
		write(fd_stderr, (void *)sigs[i].name, sigs[i].n_name);
	write(fd_stderr, (void *)swansong, sizeof(swansong));

	/*
	 * hey man, I'm not goin' alone!
	 */
	kill(-getpgrp(), SIGTERM);
	exit(1);
}

static void
sig_init(void)
{
	struct sigaction sa;
	int	i;

	/*
	 * We want to lead our process group so that flaming_death
	 * will do the right thing.  All of this may seem a little
	 * excessive just to deal with SIGSEGV and SIGABRT but I'm
	 * sick of writting little awk one-liners to clean up the
	 * left over gs processes when I find a bug in mgv.
	 *
	 * Besides, I thought it was really cool when I trapped
	 * my first seg fault and died gracefully.
	 *
	 * I suppose I could keep a list of all the child processes
	 * that have been made (and maintain this list) but that is
	 * just a big pain in the ass; using process groups is just
	 * too cute and convenient a trick to not use it.
	 */
	setpgid(getpid(), getpid());

	fd_stderr = fileno(stderr);
	memset((void *)&sa, '\0', sizeof(sa));
	sigemptyset(&sa.sa_mask);
	for(i = 0; i < N_SIGS; ++i) {
		if(sigs[i].sig == 0)
			continue;
		sa.sa_handler = sigs[i].handler;
		sigaction(sigs[i].sig, &sa, NULL);
	}
}

static char **
extract_switches(int *ac, char **argv)
{
	int	i, j;
	int	argc      = *ac;
	char	**missing = NULL;
	int	n_missing = 0;

	for(i = 1; i < argc; ) {
		if(strcmp(argv[i], "--") == 0)
			break;
		if(*argv[i] != '-') {
			++i;
			continue;
		}

		missing = (char **)XtRealloc((XtPointer)missing,
						(n_missing + 1)*sizeof(char *));
		missing[n_missing++] = argv[i];

		for(j = i; j < argc; ++j)
			argv[j] = argv[j + 1];
		--argc;
	}
	*ac = argc;

	/*
	 * And terminate the list if necessary.
	 */
	if(missing != NULL) {
		missing = (char **)XtRealloc((XtPointer)missing, 
						(n_missing + 1)*sizeof(char *));
		missing[n_missing++] = NULL;
	}

	return missing;
}

static void
report_badswitches(Widget w, char **unknown)
{
	int   i, size;
	char *msg;
	char *sep     = "\n    ";
	int   sepsize = strlen(sep);

	if(unknown == NULL)
		return;

	/*
	 * Spin through once to get the buffer size.
	 */
	for(i = size = 0; unknown[i] != NULL; ++i)
		size += strlen(unknown[i]) + sepsize;
	size += 3;
	msg = XtCalloc(size, 1);

	/*
	 * And again to fill things in.
	 */
	for(i = 0; unknown[i] != NULL; ++i)
		strcat(strcat(msg, sep), unknown[i]);
	strcat(msg, "\n");

	wl_warn(w, mgv_s(w, MgvSunknownSwitch), msg);

	XtFree((XtPointer)unknown);
}

static void
nonx_options(char *me, char **argv)
{
	for(++argv; *argv != NULL; ++argv) {
		if(strcmp(*argv, "-help") == 0
		|| strcmp(*argv, "--help")    == 0
		|| strcmp(*argv, "-H")    == 0
		|| strcmp(*argv, "-?")    == 0)
			exit(helpem(me, EXIT_SUCCESS));
		if(strcmp(*argv, "-longhelp") == 0)
			exit(helpem_long(me, EXIT_SUCCESS));
		if(strcmp(*argv, "-version")  == 0
		|| strcmp(*argv, "--version") == 0)
			exit(version());
	}
}

static WL_ICONS icons = {
	cicon,
	(char *)micon_bits, micon_width, micon_height,
	(char *)mask_bits,  mask_width,  mask_height
};

int
main(int argc, char **argv)
{
	MGV	*m;
	char	*me;
	Widget	top, splash;
	Display	*dpy;
	char	**unknown;

	/*
	 * prepare for the worst
	 */
	sig_init();

	if((me = strrchr(argv[0], '/')) == NULL)
		me = argv[0];
	else
		++me;

	/*
	 * Yank out -help and friends before anything else
	 * happens.  There's really no reason to crank up all of
	 * Xt just to show some command-line only information.
	 */
	nonx_options(me, argv);

	/*
	 * Crank Xt into life.
	 *
	 * We don't use the Xt convenience functions since we
	 * want several top level shells.
	 */
	dpy = wl_init(&argc, argv, "MGv", &options[0], XtNumber(options),
							&bld_fallbacks[0]);
	if(dpy == NULL) {
		fprintf(stderr, "mgv: could not initialize!\n");
		return EXIT_FAILURE;
	}
	wl_addclass("Ghostview", ghostviewWidgetClass, NULL, FALSE);
	wl_addclass("WlToolBar", wlToolBarWidgetClass, NULL, FALSE);
	wl_icons_init(&icons);

	unknown = extract_switches(&argc, argv);

	splash = XtAppCreateShell(NULL, "MGv", wlSplashScreenWidgetClass,
								dpy, NULL, 0);

	/*
	 * Here we go...
	 */
	++argv;
	do {
		char *file = *argv;

		/*
		 * Fix up the standard "-" -> stdin naming.
		 */
		if(file != NULL
		&& strcmp(file, "-") == 0)
			file = NULL;

		top = XtAppCreateShell(NULL, "MGv", wlAppShellWidgetClass,
								dpy, NULL, 0);

		if((m = mgv_create(top, file)) == NULL) {
			fprintf(stderr, "%s: could not create top level window"
					"for %s.\n", me,
					file == NULL ? "(stdin)" : file);
			XtDestroyWidget(top);
			top = NULL;
		}
		else {
			mgv_pgstack_goto(m->pgstack, 0);
		}
	} while(*argv != NULL && *++argv != NULL);

	XtDestroyWidget(splash);

	if(top == NULL) {
		fprintf(stderr, "%s: could not create any top level windows\n"
							"\texitting.\n", me);
		exit(EXIT_FAILURE);
	}

	report_badswitches(top, unknown);

	XtAppMainLoop(XtWidgetToApplicationContext(top));

	/*
	 * Sacrifice for the angry compiler gods.
	 */
	return 0;
}
