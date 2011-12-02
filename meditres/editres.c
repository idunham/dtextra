/*
 * $Header: /cvsroot-fuse/meditres/meditres/editres.c,v 1.9 2002/01/15 12:15:14 amai Exp $
 *
 * $XConsortium: editres.c,v 1.18 94/04/17 20:38:52 rws Exp $
 *
 * Copyright (c) 1989  X Consortium
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the X Consortium shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from the X Consortium.
 */

/*
 * Modifications by Volker Gering, 1997
 *
 * Motif port + MEDITRES project, © 2001 Danny Backx.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>     /* stat() */

/* X11 is not limited to Unix. Some operating systems don't have */
/* signal handling */
#include <unistd.h>       /* stat() for linux, Signal handling */
#include <signal.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Editres.h>

#include "meditres.h"

/*
 * Global variables. 
 */

/* array of toolkit dependent labels taken from the resource file */
String         res_labels[NUM_RES_LABELS];

/* decremented if the target client does not speak the current version */
int            global_effective_protocol_version = CURRENT_PROTOCOL_VERSION;

/* toolkit type of client whose "resources" we are currently editing */
char           *global_effective_toolkit         = "xt";

int            global_error_code;
unsigned long  global_serial_num;
int            (*global_old_error_handler) ();

Boolean        global_resource_box_up            = FALSE;
TreeInfo       *global_tree_info                 = NULL;
CurrentClient  global_client;
ScreenData     global_screen_data;
Widget         global_tree_parent;
Widget         global_paned = NULL;   /* named after toolkit */
Widget         global_toplevel;
AppResources   global_resources;
FindTypes      global_findType;
GC             global_draw_gc;
GC             global_fill_gc;
XFontStruct    *global_draw_font;
int            global_print_width;
/* >>>>> HACK for batchres */
Window         global_winID;
int            global_mode                       = MODE_INTERACTIVE;
char           *global_resfile_buffer            = NULL;
Cardinal       global_resfile_wraplines          = 0;
Cardinal       global_resfile_lines              = 0;
/* <<<<< HACK for batchres */

/* Widgets in the status area */
Widget         global_numWid;         /* number of nodes */
Widget         global_shownWid;       /* number of toggels shown */
Widget         global_selWid;         /* number of selected/active widgets */
Widget         global_wait;           /* countdown */
Widget         global_zoom;
Widget         global_work;           /* work in progress */
Widget         global_rcv;            /* receiving data */

int            global_countdown;      /* actual countdown value */
XtIntervalId   global_timeout;        /* id for countdown timer */

Cardinal       global_maxdepth = MAXDEPTH;
Cardinal       global_maxchilds = MAXCHILDS;
Cardinal       global_movespeed = MOVESPEED;


#define Offset(field)   (XtOffsetOf(AppResources, field))

static XtResource  editres_resources[] =
{
  {"debug",            "Debug",             XtRBoolean, sizeof (Boolean),
   Offset (debug), XtRString, (XtPointer) "false"},
  {"numFlashes",       "NumFlashes",        XtRInt, sizeof (int),
   Offset (num_flashes), XtRImmediate, (XtPointer) NUM_FLASHES},
  {"flashTime",        "FlashTime",         XtRInt, sizeof (int),
   Offset (flash_time), XtRImmediate, (XtPointer) FLASH_TIME},
  {"flashColor",       XtCForeground,       XtRPixel, sizeof (Pixel),
   Offset (flash_color), XtRImmediate, (XtPointer) XtDefaultForeground},
  {"saveResourceFile", "SaveResourcesFile", XtRString, sizeof (String),
   Offset (save_resources_file), XtRString, (XtPointer) ""},
  {"msgWiget",        "MsgWidget",         XtRBoolean, sizeof (Boolean),
   Offset (msgWidget), XtRString, (XtPointer) "true"},
  {"msgStderr",        "MsgStderr",         XtRBoolean, sizeof (Boolean),
   Offset (msgStderr), XtRString, (XtPointer) "false"},
  {"msgBoth",          "MsgBoth",           XtRBoolean, sizeof (Boolean),
   Offset (msgBoth), XtRString, (XtPointer) "false"},
  /* >>>>> HACK for batchres */
  {"winID",            "WinID",             XtRString, sizeof (String),
   Offset (winID), XtRString, (XtPointer) ""},
  {"resFile",          "ResFile",           XtRString, sizeof (String),
   Offset (resFile), XtRString, (XtPointer) ""},
  {"terminate",        "Terminate",         XtRBoolean, sizeof (Boolean),
   Offset (terminate), XtRString, (XtPointer) "false"}
  /* <<<<< HACK for batchres */
};

static XrmOptionDescRec options[] =
{
  {"-debug",            ".debug", XrmoptionNoArg, (XtPointer)"true"},
  {"-numFlashes",       ".numFlashes", XrmoptionSepArg, (XtPointer)NULL},
  {"-flashTime",        ".flashTime", XrmoptionSepArg, (XtPointer)NULL},
  {"-flashColor",       ".flashColor", XrmoptionSepArg, (XtPointer)NULL}, 
  {"-saveResourceFile", ".saveResourceFile", XrmoptionSepArg, (XtPointer)NULL},
  {"-msgWidget",        ".msgWidget", XrmoptionNoArg, (XtPointer)"true"},
  {"-msgStderr",        ".msgStderr", XrmoptionNoArg, (XtPointer)"true"},
  {"-msgBoth",          ".msgBoth", XrmoptionNoArg, (XtPointer)"true"},
  /* >>>>> HACK for batchres */
  {"-winID",            ".winID", XrmoptionSepArg, (XtPointer)NULL},
  {"-resFile",          ".resFile", XrmoptionSepArg, (XtPointer)NULL},
  {"-terminate",        ".terminate", XrmoptionNoArg, (XtPointer)"true"}
  /* <<<<< HACK for batchres */
};

Atom wm_delete_window;


static void usage (XtAppContext app_con, char *call);
static Boolean readResFile(const char *filename);
static void InitSignalHandler (void);
static void hangup(int signo);
static void cleanup (int signo);
static void DestroyNodes(WNode *node);


/*
 *
 * internal function declaration.
 *
 */

int
main(int argc, char **argv)
{
	XtAppContext app_con;

	global_toplevel = XtAppInitialize(&app_con, "meditres",
		options, XtNumber(options),
		&argc, argv,
		fallback,
		NULL, 0);

	if (argc != 1) {
		usage(app_con, argv[0]);
	}

	/* new signalhandler for HUP, INT, ALRM, TERM, USR1, USR2 */
	InitSignalHandler();

	SetApplicationActions (app_con);
	XtGetApplicationResources (global_toplevel,
		(XtPointer) &global_resources,
		editres_resources, XtNumber(editres_resources),
		NULL, (Cardinal) 0);
	global_resources.allocated_save_resources_file = FALSE;

	if (global_resources.msgWidget) {
		global_resources.msgStderr = FALSE;
	}
	if (global_resources.msgStderr) {
		global_resources.msgWidget = FALSE;
	}
	if (global_resources.msgBoth) {
		global_resources.msgWidget = TRUE;
		global_resources.msgStderr = TRUE;
	}

	XtOverrideTranslations (global_toplevel,
		XtParseTranslationTable("<Message>WM_PROTOCOLS: quit()") );

	/* build tree for Xt intrinsics */
	BuildWidgetTree (global_toplevel);

	SetMessage(global_screen_data.info_label,
		res_labels[13],
		"Welcome to the Motif X Resource Editor version " VERSION);

	global_screen_data.set_values_popup = NULL;
	global_screen_data.find_popup = NULL;
	global_draw_gc = NULL;
	global_fill_gc = NULL;
	global_draw_font = NULL;

	InternAtoms (XtDisplay (global_toplevel));

	XtRealizeWidget (global_toplevel);

	wm_delete_window = XInternAtom(XtDisplay(global_toplevel),
		"WM_DELETE_WINDOW", False);
	(void)XSetWMProtocols(XtDisplay(global_toplevel),
		XtWindow (global_toplevel), &wm_delete_window, 1);

	/* >>>>> HACK for batchres */

	/* try to connect to given client */
	if (global_resources.winID != NULL
			&& strlen(global_resources.winID) != 0) {
		while (isspace((int)global_resources.winID[0])
				&& strlen(global_resources.winID) > 0) {
			global_resources.winID++;
		}

		global_winID = strtoul(global_resources.winID,
			NULL,     /* no error checking */
			0);       /* determine base automatically */

		/* try to connect to client specified by parameter winID */
		(void)XtAppAddTimeOut(app_con, 100, brConnectToClient, NULL);
		/* XtUnmanageChild(global_toplevel); */
	} else {
		global_winID = None;
	}

	/* try to load given resource file */
	if (global_resources.resFile != NULL
			&& strlen(global_resources.resFile) != 0) {
		if (!readResFile(global_resources.resFile)) {
			fprintf(stderr, 
				"Resource file %s could not be loaded!\n",
				global_resources.resFile);
			global_resources.resFile = NULL;
			global_resfile_buffer = NULL;
		}
	}

	/* <<<<< HACK for batchres */

#ifndef	LESSTIF_VERSION
	/*
	 * Add event handler for Editres protocol, unless we use LessTif.
	 * LessTif does this by itself.
	 */
	XtAddEventHandler(global_toplevel, (EventMask)0, True,
		(XtEventHandler)_XEditResCheckMessages, NULL);
#endif

	XtAppMainLoop (app_con);
	exit(0);
}   /* main() */


/* Function Name:
 *   usage
 *
 * Description:
 *   Prints a the calling Syntax for this function to stdout.
 *
 * Arguments:
 *   app_con - the application context.
 *   call - the name of the application.
 *
 * Returns:
 *   none - exits tho.
 *
 * Calls:
 *   XtDestroyApplicationContext - Xtoolkit
 */
static void
usage (XtAppContext app_con, char *call)
{
  XtDestroyApplicationContext (app_con);
  fprintf (stderr,
	   "Usage: %s [Options]\n\n",
	   call);
  fprintf (stderr, "Possible Options are:\n");
  fprintf (stderr, "  -debug\n");
  /* to implementate: */
  /*   fprintf (stderr, "  -msgWidget, -msgStderr, -msgBoth\n"); */
  /*   fprintf (stderr, "  (default is to write messages to widget)\n"); */
  /*   fprintf (stderr, "  (-msgBoth has hightest priority)\n"); */
  fprintf (stderr, "  -numFlashes <count>\n");
  fprintf (stderr, "  -flashTime <time in ms>\n");
  fprintf (stderr, "  -flashColor <color>\n");
  fprintf (stderr, "  -saveResourceFile <file>\n\n");
  /* >>>>> HACK fuer batchres */
  fprintf (stderr, "  -winID <windowID of client>\n");
  fprintf (stderr, "  -resFile <file to load>\n");
  fprintf (stderr, "  -terminate\n\n");
  /* <<<<< HACK fuer batchres */
  fprintf (stderr, "... and all the other standard command-line options of Xt.\n");

  exit (EXIT_FAILURE);

}   /* usage() */



/* Function Name:
 *   InitSignalHandler
 *
 * Description:
 *   Setup an new signal handler for signals HUP, INT, ALRM, TERM, USR1, USR2
 *
 * Arguments:
 *   (none)
 *
 * Returns:
 *   (none)
 *
 * Calls:
 *   sigemptyset
 *   sigaction
 *
 * Callbacks installed:
 *   cleanup - self
 */
static void
InitSignalHandler (void)
{
  struct sigaction newAction;
  sigset_t         emptyMask;


  sigemptyset (&emptyMask);

  newAction.sa_handler = hangup;
  newAction.sa_mask = emptyMask;
  newAction.sa_flags = 0;

  if ( sigaction(SIGHUP, &newAction, (struct sigaction*)NULL) == -1 )
    {
      perror ("sigaction()");
      exit (EXIT_FAILURE);
    }   /* if (error installing new sigaction for SIGHUP) */

  newAction.sa_handler = cleanup;

  if ( sigaction(SIGINT, &newAction, (struct sigaction*)NULL) == -1 )
    {
      perror ("sigaction()");
      exit (EXIT_FAILURE);
    }   /* if (error installing new sigaction for SIGINT) */

  if ( sigaction(SIGALRM, &newAction, (struct sigaction*)NULL) == -1 )
    {
      perror ("sigaction()");
      exit (EXIT_FAILURE);
    }   /* if (error installing new sigaction for SIGALRM) */

  if ( sigaction(SIGTERM, &newAction, (struct sigaction*)NULL) == -1 )
    {
      perror ("sigaction()");
      exit (EXIT_FAILURE);
    }   /* if (error installing new sigaction for SIGTERM) */

  if ( sigaction(SIGUSR1, &newAction, (struct sigaction*)NULL) == -1 )
    {
      perror ("sigaction()");
      exit (EXIT_FAILURE);
    }   /* if (error installing new sigaction for SIGUSR1) */

  if ( sigaction(SIGUSR2, &newAction, (struct sigaction*)NULL) == -1 )
    {
      perror ("sigaction()");
      exit (EXIT_FAILURE);
    }   /* if (error installing new sigaction for SIGUSR2) */

}   /* InitSignalHandler() */


/* Function Name:
 *   hangup
 *
 * Description:
 *   Signal handler: Reinitialize editres
 *
 * Arguments:
 *   signo - number of the signal raised (should be SIGHUP)
 *
 * Returns:
 *   (none)
 *
 * Calls:
 *   RemoveNodeFromActiveList -
 *   DestroyNodes - (self)
 */
static void
hangup(int signo)
{
	sigset_t	new, old;
	int		i;

	/* block all signals */
	sigemptyset(&new);
	sigprocmask(SIG_SETMASK, &new, &old);

	if (signo != SIGHUP) {
		fprintf(stderr, "\nWarning: signal (%d) expected, but (%d) arrived!\n",
			SIGHUP, signo);
		fprintf(stderr, "Now destroying all internal data!\n");
	}   /* if (wrong signal) */

	/*
	 * reset global variables.
	 */

	/* decremented if the target client does not speak the current version */
	global_effective_protocol_version = CURRENT_PROTOCOL_VERSION;

	/* toolkit type of client whose "resources" we are currently editing */
	global_effective_toolkit = "xt";

	/* force to select a new client if use "Refresh Tree..." */
	global_client.window = None;

	/* the resource box is destroyed by cleaning up the node_info tree */
	if (global_resource_box_up) {
		global_resource_box_up = FALSE;
	}

	/* if window "Set Values" is popped up, destroy it */
	if (global_screen_data.set_values_popup != NULL) {
		XtDestroyWidget (global_screen_data.set_values_popup);
		global_screen_data.set_values_popup = NULL;
	}

	/* if window "Find Dialog" is popped up, destroy it */
	if (global_screen_data.find_popup != NULL) {
		XtDestroyWidget (global_screen_data.find_popup);
		global_screen_data.find_popup = NULL;
	}

  /* if we allocated an own string for save_resource_file: free it */
  if (global_resources.allocated_save_resources_file) 
    {
      XtFree(global_resources.save_resources_file);
    }
  global_resources.allocated_save_resources_file = FALSE;

  global_paned = NULL;   /* named after toolkit is known */

  global_maxdepth = MAXDEPTH;
  global_maxchilds = MAXCHILDS;
  global_movespeed = MOVESPEED;

  /* if a resource file is loaded: free the buffer */
  if (global_resfile_buffer != NULL)
    {
      XtFree(global_resfile_buffer);
      global_resfile_buffer = NULL;
    }

  /* try to reload given resource file */
  if ( global_resources.resFile != NULL
       && strlen(global_resources.resFile) != 0 )
    {
      if (!readResFile(global_resources.resFile))
	{
	  fprintf(stderr,
		  "Resource file %s could not be loaded!\n",
		  global_resources.resFile);
	  global_resources.resFile = NULL;
	  global_resfile_buffer = NULL;
	}
    }


  /* NOW it't time to destroy the widgets and the WNode tree */
  for (i=0; i < global_tree_info->num_active; i++)
    {
      RemoveNodeFromActiveList (global_tree_info->active_nodes[i]);
    }   /* for (all selected widgets) */
  XtFree((char*)(global_tree_info->active_nodes));
  XtFree((char*)(global_tree_info->flash_widgets));

  DestroyNodes(global_tree_info->top_node);
  XtDestroyWidget(global_tree_info->tree_widget);

  XtFree((char*)global_tree_info);
  global_tree_info = NULL;

  /* update status area */
  SetNumWidLabel (0);
  SetSelWidLabel (0);
  SetShownWidLabel (0);
  SetWaitLabel (0);
  SetZoomLabel (0);
  SetWorkLabel (0);
  SetRcvLabel (FALSE);

  /* restore old sigmask */
  sigprocmask (SIG_SETMASK,
	       &old,
	       &new);

  /* if we grabbed the Xserver we must release it */
  XUngrabPointer (XtDisplay (global_toplevel), CurrentTime);
  XUngrabKeyboard (XtDisplay (global_toplevel), CurrentTime);
  XUngrabServer (XtDisplay (global_toplevel));
  
}   /* hangup() */


/* Function Name:
 *   cleanup
 *
 * Description:
 *   Signal handler: cleaning up and exit process
 *
 * Arguments:
 *   signo - number of the signal raised
 *
 * Returns:
 *   (none)
 *
 * Calls:
 *
 */
static void
cleanup (int signo)
{
  sigset_t new;
  sigset_t old;


  /* block all signals */
  sigemptyset (&new);
  sigprocmask (SIG_SETMASK,
	       &new,
	       &old);

  fprintf(stderr, "\nsignal (%d) arrived!\n", signo);
  fprintf(stderr, "Editres cleans up ...\n");

  /* restore old sigmask */
  sigprocmask (SIG_SETMASK,
	       &old,
	       &new);

  /* if we grabbed the Xserver we must release it */
  XUngrabPointer (XtDisplay (global_toplevel), CurrentTime);
  XUngrabKeyboard (XtDisplay (global_toplevel), CurrentTime);
  XUngrabServer (XtDisplay (global_toplevel));
  
  exit (EXIT_FAILURE);

}   /* cleanup() */


/* Function Name:
 *   DestroyNodes
 *
 * Description:
 *   This function frees the memory allocated by the WNode tree
 *
 * Arguments:
 *   node - the top node of the tree
 *
 * Returns:
 *   (none)
 *
 * Calls:
 *    DestroyNodes - (reccursive)
 */
static void
DestroyNodes(WNode *node)
{
  int      i;
  NameInfo *nameInfo;
  NameInfo *this;


  /* do we have to free the strings "name" and "class"? Don't know */

  for (i=0; i < node->num_children; i++)
    {
      DestroyNodes(node->children[i]);
    }   /* for (all children) */

  if (node->alloc_children > 0)
    {
      XtFree((char*)(node->children));
    }   /* if (free list of children) */

  if (node->resources != NULL)
    {
      if (node->resources->res_box != NULL)
	{
	  nameInfo = node->resources->res_box->name_info;
	  while (nameInfo != NULL)
	    {
	      this = nameInfo;
	      nameInfo = nameInfo->next;
	      XtFree((char*)this);
	    }
	}   /* if (resource box exists) */

      if (node->resources->normal != NULL)
	{
	}
      if (node->resources->constraint != NULL)
	{
	}

      XtDestroyWidget(node->resources->res_box->shell);

    }   /* if (resources allocated) */

  XtFree((char*)node);

}   /* DestroyNodes() */



/* >>>>> HACK for batchres */

/* Function Name:
 *   readResFile
 *
 * Description:
 *   Try to read the resource file given to parameter "-resFile" into a
 *   buffer. The global variable 'global_resfile_buffer' points to the
 *   buffer if reading succeded. This function uses 'XtMalloc()' to create
 *   the buffer. It must be freed by 'XtFree()' if it's no longer useful.
 *
 * Arguments:
 *   filename - Name of the resource file to load into the client.
 *
 * Returns:
 *   TRUE if file could be loaded
 *
 * Calls:
 *    XtResolvePathname - (X Toolkit)
 */
Boolean
readResFile(const char *filename)
{
  FILE         *file;
  Boolean      ret = FALSE;
  struct stat  statbuf;
  char         *ptr;


  if (stat(filename, &statbuf) == -1)
    {
      perror ("stat new resource file");
      ret = FALSE;
    }
  else
    {
      if (S_ISREG(statbuf.st_mode))
	{

#ifdef S_IRUSR
	  /* user has read permissions */
	  if ((statbuf.st_mode & S_IRUSR) != 0)
	    {
#endif   /* S_IRUSR */

	      if ((global_resfile_buffer
		   = (char*)XtMalloc(statbuf.st_size)) == NULL)
		{
		  fprintf(stderr,
			  "ERROR: XtMalloc failed for size %d.\n",
			  statbuf.st_size);
		}
	      else
		{
		  ret = TRUE;
		}

#ifdef S_IRUSR
	    }
	  else
	    {
	      fprintf(stderr,
		      "ERROR: user has no read permission on file %s.\n",
		      filename);
	      ret = FALSE;
	    }
#endif   /* S_IRUSR */

	}
      else
	{
	  fprintf(stderr,
		  "ERROR: resource file %s is not a regular file!\n",
		  filename);
	  ret = FALSE;
	}
    }

  if (ret)
    {
      if ((file = fopen(filename, "r")) != NULL)
	{
	  ptr = global_resfile_buffer;
	  global_resfile_lines = 0;
	  while (fgets(ptr, statbuf.st_size, file) != NULL)
	    {
	      ptr = &global_resfile_buffer[strlen(global_resfile_buffer)];
	    }
	  ret = TRUE;

	  /* now we have to replace the '\<return>' */
	  ptr = global_resfile_buffer;
	  global_resfile_wraplines = 0;
	  while (*ptr != '\0')
	    {
	      if (*ptr == '\\' && *(ptr+1) == '\n')
		{
		  memmove(ptr, ptr+2, strlen(ptr+2) + 1);
		  global_resfile_wraplines++;
		}
	      if (*ptr == '\n')
		{
		  global_resfile_lines++;
		}
	      ptr++;
	    }   /* while () */

	}   /* if (open(filename) OK) */

      else
	{
	  perror ("fopen new resource file");
	  ret = FALSE;
	}
    }

  return (ret);

}   /* readResFile() */

/* <<<<< HACK for batchres */
