/* Most of the common Motif and -related routines for xps.
  $Id: xstuff.c,v 1.3 2003/04/24 00:21:29 rockyb Exp $
  Copyright (C) 1998,1999 Rocky Bernstein
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
*/
#include "user_config.h"
#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/ScrolledW.h>
#include <Xm/Scale.h>
#include <Xm/Separator.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include "xstuff.h"
#include "common.h"
#include "dialog.h"
#include "xps.h"
#include "process.h"
#include "callback.h"
#include "MenuSupport.h"
#include "askquestion.h"
#include "getpixelbyname.h"

#ifdef HAVE_X11_XMU_EDITRES_H
#include <X11/Xmu/Editres.h>
#endif

static Widget dialog_user_widget; /* Widget for last user set in a dialog */
static Widget dialog_color_widget; /* Widget for last color set in a dialog */
static Widget dialog_pscmd_widget; /* Widget for last pscmd set in a dialog */
static Widget dialog_debug_widget; /* Widget for last debug set in a dialog */
 
static XtAppContext CreateWindows(int argc, char *argv[]);

ApplInfo base;

Widget   toplevel;
Widget   frame;

int
main(int argc, char *argv[])
{
  
  common_init(argc, argv);
  machine_specific_init();
  
  /* create windows, get arguments, etc, etc... */
  
  base.app_con = CreateWindows(argc,argv);
  if (base.debugging) {
     printf("%s: version %s\n", base.program, VERSION);
     printf("%s: interval %ld, showkernel %d, allprocs %d\n", 
	    base.program, base.interval, base.showKernel, active.allprocs);
     printf("%s: zoomed %d, debugging %d\n", 
	    base.program, base.zoomed, base.debugging);
  }
  
  /* run */
  XtAppMainLoop(base.app_con);
  return 0;
}

/**************************************************************************
Forces a recalculation of everything and update of the display.
Useful when we know something has changed.
***************************************************************************/
void 
ForceRedisplay(void)
{
  /* No recursive calls, please. */
  static Boolean inForceRedisplay=False;
  if (!inForceRedisplay && base.visible) {
    inForceRedisplay=True;
    display(True,XtDisplay(active.canvas),XtWindow(active.canvas));
    inForceRedisplay=False;
  }
}

/***************************************************************************
   Change the name or label of a widget, such as a button or message. 
***************************************************************************/
extern void
set_label (char *name, Widget w)
{
  /* It is disgusting that you gotta do all of this too do something trivial.
     Shouldn't this be part of the standard Motif toolkit? 
   */
  Arg args[1];
  XmString xmstr=XmStringCreateLtoR (name, XmFONTLIST_DEFAULT_TAG);

  XtSetArg (args[0], XmNlabelString, xmstr);
  XtSetValues (w, args, 1);
  XmStringFree(xmstr);
}

/***************************************************************************
 Return the name assigned to a widget. There appears to be 
 no other way to get the information.  Used only on the toplevel widget,
 whose name is not necessarily argv[0] (cf -name toolkit option).       
***************************************************************************/
static inline String 
XtWidgetToName(Widget w)
{
  return w->core.name;
}

/***************************************************************************
  Set how often a refresh of the display occurs.
***************************************************************************/
static void
set_refresh_rate(Widget w, XtPointer client_data, XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  if (is_valid_interval(w, cbs->value)) {
    base.interval = cbs->value * XPS_TICK;
  }
  
  updateMessage();
  /* Let the usual timeout mechanism cause an update to occur.
     We don't want to force a timeout here or else we may have multiple
     timers running which will surely eat up CPU. 
  */
  /*Timeout(NULL, NULL); */
}

#include "help/interval.h"
static void
popup_refresh_rate_dialog(Widget button, XtPointer client_data, 
			  XtPointer call_data)
{
  Widget dialog_box, refresh_rate_slider, form;

  /* Create the dialog as a child of the menu button */
  dialog_box = NewDialog(button,  "set_refresh_rate", "Set Refresh Rate Menu", 
			 True,  help_callback, interval_help,
			 False, (XtCallbackProc) NULL, NULL,
			 True,  popdown_refresh_rate_dialog, NULL);

  form = XtVaCreateManagedWidget ("form", xmFormWidgetClass, 
				dialog_box,
				NULL);
  refresh_rate_slider = 
    XtVaCreateManagedWidget ("interval", xmScaleWidgetClass, form,
			     XtVaTypedArg, XmNtitleString, 
			     XmRString, "Refresh Rate",
			     10,
			     XmNmaximum, XPS_MAX_REFRESH_RATE,
			     XmNminimum, XPS_MIN_REFRESH_RATE,
			     XmNvalue, base.interval / XPS_TICK,
			     XmNshowValue, True,
			     XmNorientation, XmHORIZONTAL,
			     XmNtopAttachment,     XmATTACH_FORM,
			     XmNleftAttachment,    XmATTACH_FORM,
			     XmNrightAttachment,   XmATTACH_FORM,
			     XmNbottomAttachment,  XmATTACH_FORM,

			     NULL);

  XtAddCallback(refresh_rate_slider, XmNdragCallback, 
		set_refresh_rate,  NULL);
  XtAddCallback(refresh_rate_slider, XmNvalueChangedCallback, 
		set_refresh_rate,  NULL);

  XtManageChild(dialog_box);
}

static void
set_debug_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
  char *int_str = XmTextFieldGetString(dialog_debug_widget);
  int debug_value;
  Boolean revise=False;
  
  if (sscanf(int_str, "%d", &debug_value)==1) {
    base.debugging = debug_value;
    if (base.debugging & DBG_CALLBACK)
      printf ("debug reset to %d\n", base.debugging);
  } else {
    /* Debug value not an integer */
    revise = AskQuestion ( w, "Debug value not a number.");
  }
}

static void
popup_debug_dialog( Widget button, XtPointer client_data,
			XtPointer call_data)
{
  Widget dialog_box, label, input, rc;
  char numstr[10];
#include "help/debug.h"
  sprintf(numstr, "%d", base.debugging);
    
  /* Create the user name dialog */
  dialog_box = NewDialog(button,  "Debugging", "Set Debugging",
			 True,  help_callback          , debug_help,
			 True,  destroy_widget_callback, NULL,
			 True,  set_debug_callback, NULL);

  rc = XtVaCreateManagedWidget( "form", xmRowColumnWidgetClass, 
				dialog_box,
				XmNnumColumns,        2,
				XmNorientation,       XmHORIZONTAL,
				XmNpacking,           XmPACK_TIGHT,
				XmNtopAttachment,     XmATTACH_FORM,
				XmNleftAttachment,    XmATTACH_FORM,
				XmNrightAttachment,   XmATTACH_NONE,
				NULL );

  /* Create label(s) */
  label = 
    XtVaCreateManagedWidget("debug",  xmLabelWidgetClass, rc, 
			    XmNtraversalOn, False,
			    NULL);
  /* Create text input */
  dialog_debug_widget = 
  XtVaCreateManagedWidget( "input", 
			 xmTextFieldWidgetClass, rc, 
			 XmNcolumns, 10, 
			 XmNvalue, numstr, 
			 NULL);

  XtAddCallback ( dialog_debug_widget, XmNactivateCallback, 
		  set_debug_callback, 
		  NULL ) ;
  
  XtManageChild(dialog_box);
  /* Focus should be on input area. */
  XmProcessTraversal(dialog_debug_widget, XmTRAVERSE_CURRENT);
}

static void
set_pscmd_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
  char *str = XmTextFieldGetString(dialog_pscmd_widget);
  base.ps_cmdline = str;
}

static void
popup_pscmd_dialog( Widget button, XtPointer client_data,
		    XtPointer call_data)
{
  Widget dialog_box, label, input, rc;
  unsigned int len = 1.1*strlen(base.ps_cmdline);
  #include "help/pscmdline.h"

  if (len < 10) len = 10;
    
  /* Create the user name dialog */
  dialog_box = NewDialog(button,  "PsCmd", "Set ps command",
			 True,  help_callback          , pscmdline_help,
			 True,  destroy_widget_callback, NULL,
			 True,  set_pscmd_callback, NULL);

  rc = XtVaCreateManagedWidget( "form", xmRowColumnWidgetClass, 
				dialog_box,
				XmNnumColumns,        2,
				XmNorientation,       XmHORIZONTAL,
				XmNpacking,           XmPACK_TIGHT,
				XmNtopAttachment,     XmATTACH_FORM,
				XmNleftAttachment,    XmATTACH_FORM,
				XmNrightAttachment,   XmATTACH_NONE,
				NULL );

  /* Create label(s) */
  label = 
    XtVaCreateManagedWidget("pscmd",  xmLabelWidgetClass, rc, 
			    XmNtraversalOn, False,
			    NULL);
  /* Create text input */
  dialog_pscmd_widget = 
  XtVaCreateManagedWidget( "input", 
			   xmTextFieldWidgetClass, rc, 
			   XmNcolumns, len,
			   XmNvalue, base.ps_cmdline, 
			   NULL);

  XtAddCallback ( dialog_pscmd_widget, XmNactivateCallback, 
		  set_pscmd_callback, NULL ) ;
  
  XtManageChild(dialog_box);
  /* Focus should be on input area. */
  XmProcessTraversal(dialog_pscmd_widget, XmTRAVERSE_CURRENT);
}

#ifdef HAVE_REGEX
static Boolean user_is_pattern;
#endif

static void
set_user_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
  char *user_p = XmTextFieldGetString(dialog_user_widget);
  unsigned int len=strlen(user_p);
  uid_t uid; 
  Boolean okay=True;
  char buf[80];
#ifdef HAVE_REGEX
  regex_t re_pat;
  
  if (user_is_pattern) {
    int regerr = regcomp(&re_pat, user_p, REG_NOSUB|REG_EXTENDED);
    if (regerr) {
      char buf2[80];
#ifdef HAVE_REGERROR
      regerror(regerr, &re_pat,  buf2, sizeof(buf));
      sprintf(buf, "Error parsing user pattern: %s.",  buf2);
#else 
      sprintf(buf, "Error parsing user pattern; error code %d.",  regerror);
#endif
      okay=False;
    }
  } else {
#endif	/* HAVE_REGEX */
    if (!usertoID(user_p, &uid)) {
      /* Invalid user name given */
      sprintf(buf, "Userid for %s not found.", user_p);
      okay=False;
    }
#ifdef HAVE_REGEX
  }
#endif

  if (okay) {
    int b, start, finish;
    if (set_both_windows) {
      start= 0; finish=1;
    } else {
      finish = start = (short int) base.zoomed;
    }
    
    for (b=start; b<=finish; b++) {
      free(base.info[b].user_name);
      base.info[b].user_name = (char *) malloc(sizeof(char) * len+1);
      strncpy(base.info[b].user_name, user_p, len+1);
      base.info[b].uid_displayed = uid;
#ifdef HAVE_REGEX
      base.info[b].match_regexp = user_is_pattern;
      if (user_is_pattern) {
	regfree(&base.info[b].re_pat);
	base.info[b].re_pat = re_pat;
      }
#endif
    }
    ForceRedisplay();
  } else {
    ShowDialog ( w, XmDIALOG_ERROR, buf );
  }

  if (base.debugging & DBG_CALLBACK)
    printf ("text entered = %s, id %ld\n", 
	    active.user_name, (long) active.uid_displayed);

  XtDestroyWidget ( GetTopShell(w) );
}

#ifdef HAVE_REGEX
void 
toggle_pattern(Widget w, XtPointer client_data, XtPointer call_data)
{
  user_is_pattern = !user_is_pattern;
}
#endif

#include "help/watch_user.h"
static void
popup_set_user_dialog( Widget button, XtPointer client_data,
		       XtPointer call_data)
{
  Widget dialog_box, user_label, set_both_button, rc;

  set_both_windows = False;
  
  /* Create the user name dialog */
  dialog_box = NewDialog(button, "set_watch_user", "Set Watch User", 
			 True,  help_callback          , watch_user_help,
			 True,  destroy_widget_callback, NULL,
			 True,  set_user_callback      , NULL);

  rc = XtVaCreateManagedWidget( "rc", xmRowColumnWidgetClass, 
				dialog_box,
				XmNnumColumns,        2,
				XmNorientation,       XmHORIZONTAL,
				XmNpacking,           XmPACK_COLUMN,
				XmNtopAttachment,     XmATTACH_FORM,
				XmNleftAttachment,    XmATTACH_FORM,
				NULL );

  /* Create label(s) */
  user_label = 
    XtVaCreateManagedWidget("user_name",    xmLabelWidgetClass, rc, 
			    XmNtraversalOn, False,
			    NULL);
  /* Create text input */
  dialog_user_widget = 
  XtVaCreateManagedWidget( "input", 
			 xmTextFieldWidgetClass, rc, 
			 XmNcolumns, 10, 
			 XmNvalue, active.user_name, 
			 NULL);

  /* Create toggle buttons to allow setting both windows,
     and to match agains exact strings or patterns.
   */
  set_both_button = 
  XtVaCreateManagedWidget( "both windows", 
			   xmToggleButtonWidgetClass, rc,
			   NULL);
  XtAddCallback(set_both_button, XmNvalueChangedCallback, 
		ToggleBoth, (XtPointer) 2);

#ifdef HAVE_REGEX    
  {
    Widget pattern_button = 
      XtVaCreateManagedWidget( "pattern", 
			       xmToggleButtonWidgetClass, rc,
			       NULL);
    user_is_pattern = active.match_regexp;
    XmToggleButtonSetState(pattern_button, active.match_regexp, False);
    XtAddCallback(pattern_button, XmNvalueChangedCallback, 
		  toggle_pattern, (XtPointer) NULL); 
  }
#endif
    
  XtVaSetValues ( dialog_box, 
		  XmNinitialFocus, dialog_user_widget,
		  NULL );
  XtManageChild(dialog_box);
}

static void
popdown_usercolor_callback(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
  uid_t uid; 
  char *user_p = XmTextFieldGetString(dialog_user_widget);
  char *color_p = XmTextFieldGetString(dialog_color_widget);
    
  if (usertoID(user_p, &uid)) {
    Pixel pixel;
    if (base.debugging & DBG_CALLBACK) 
      printf ("user %s, id %ld\n", user_p, (long) uid);
    if (!GetPixelByName(w, color_p, &pixel)) {
      char buf[80];
      sprintf(buf, "Can't parse color %s\n", color_p);
      ShowDialog(w, XmDIALOG_ERROR, buf);
    } else {
      user_color_index i = find_uid_color_index(uid);
      
      if (i == NUM_USER_COLORS) i = 0;
      base.uid[i] = uid;
      base.userColor[i]=pixel;
      
      /* Galling GetUID_Color is bogus. It needs to be done because
	 We cache the color of the last uid. A better thing
	 would be perhaps to have a separate function for this.
       */
      GetUID_Color(uid+1);
      ForceRedisplay();
    }
  } else {
    char buf[80];
    sprintf(buf, "Userid for %s not found.", user_p);
    ShowDialog ( w, XmDIALOG_ERROR, buf );
  }
  
  XtDestroyWidget( GetTopShell(w) );
}

static void
popup_set_user_color( Widget button, XtPointer client_data,
			XtPointer call_data)
{
  Widget dialog_box, user_label, color_label, rc;
#include "help/user_color.h"
    
  /* Create the user name dialog */
  dialog_box = NewDialog(button, "set_user_color", "Set User Color",
			 True,  help_callback             , user_color_help,
			 True,  destroy_widget_callback   , NULL,
			 True,  popdown_usercolor_callback, NULL);
  
  rc = XtVaCreateManagedWidget( "rc", xmRowColumnWidgetClass, 
				dialog_box,
				XmNnumColumns,        2,
				XmNorientation,       XmHORIZONTAL,
				XmNpacking,           XmPACK_COLUMN,
				XmNtopAttachment,     XmATTACH_FORM,
				XmNleftAttachment,    XmATTACH_FORM,
				NULL );

  /* Create label(s) */
  user_label = 
    XtVaCreateManagedWidget("user_name",    xmLabelWidgetClass, rc, 
			    XmNtraversalOn, False,
			    NULL);
  /* Create text input */
  dialog_user_widget =
  XtVaCreateManagedWidget( "user_input", 
			 xmTextFieldWidgetClass, rc, 
			 XmNcolumns, 10, 
			 NULL);

  /* Create label(s) */
  color_label = 
    XtVaCreateManagedWidget("color_name", xmLabelWidgetClass, rc, 
			    XmNtraversalOn, False,
			    NULL);
  /* Create text input */
  dialog_color_widget = 
  XtVaCreateManagedWidget( "color_input", 
			 xmTextFieldWidgetClass, rc, 
			 XmNcolumns, 10, 
			 NULL);

  XtManageChild(dialog_box);
  /* Focus should be on input area. */
  XmProcessTraversal(dialog_user_widget, XmTRAVERSE_CURRENT);

}

/***************************************************************************/
/* Popup Process action: 

   Pop up a window to show information about a selected process.
   This can be used later do something on the process such as 
   send a kill signal or renice it.

   Since we draw on the window and don't use windows for detection, we
   have to calculate which node is at the click position. 
*/
/***************************************************************************/
static void 
ShowProcess(w, event, parms, count)
  Widget w;
  XEvent *event;
  String *parms;
  Cardinal *count;
{
  int x,y;
  pid_t pid;
  Boolean want_kill = False;

  if (base.debugging & DBG_CALLBACK) {
    if (*count) {
      printf("ShowProcess: %d parameters, 1st one: %s\n", *count, *parms);
    } else {
      printf("ShowProcess: no parameters.\n");
    }
  }
  
  if (*count && strlen(*parms)>= 0 && ('K' == (*parms)[0])) {
    want_kill = True;
  }

  switch (event->type) {
    case ButtonPress:
         x = event->xbutton.x;
         y = event->xbutton.y;
         break;

    case KeyPress:
         x = event->xkey.x;
         y = event->xkey.y;
         break;

    default:
         XBell(XtDisplay(w),0);    /* other event types not supported */
         return;
  }

  if (find_pid_under_mouse(XtDisplay(w),XtWindow(w),x,y,&pid)) {
    Handle_Process(w, pid, event, want_kill);
  }
  else
    XBell(XtDisplay(w),0);
}

/* 
   Set up layout of all the kinds of windows that are to be used.
   In particular The process tree window (with command status sections)
   and and the process window which shows info about the process and
   a list of signals that could be sent it.
*/
XtAppContext
CreateWindows(argc,argv)
  int argc;
  char *argv[];
{
  /***************************************************************************/
  /* Command line options.  Must remap -fn/font and -geometry under *normal. */
  /***************************************************************************/

  static XrmOptionDescRec opts[] = {
    { "-help",          "*help",                XrmoptionNoArg,       XtEon },
    { "-version",       "*version",             XrmoptionNoArg,       XtEon },
    { "-metoo",         "*metoo",               XrmoptionNoArg,       XtEon},
    { "-interval",      "*interval",            XrmoptionSepArg,      NULL },
    { "-kernel",        "*kernel",              XrmoptionNoArg,       XtEon},
    { "-allprocs",      "*allprocs",            XrmoptionNoArg,       XtEon},
    { "-iconic",        "*iconic",              XrmoptionNoArg,       XtEon},
    { "-beep",          "*beep",                XrmoptionNoArg,       XtEon},
    { "-zoomed",        "*zoomed",              XrmoptionNoArg,       XtEon},
#ifdef HAVE_KILL
    { "-kill",          "*quickkill",           XrmoptionNoArg,       XtEon},
#endif
    { "-debugging",     "*debugging",           XrmoptionSepArg,      NULL },
    { "-zfont",         "*zoomed.font",         XrmoptionSepArg,      NULL },
    { "-zfn",           "*zoomed.font",         XrmoptionSepArg,      NULL },
    { "-zgeometry",     "*zoomed.geometry",     XrmoptionSepArg,      NULL },
    { "-geometry",      "*normal.geometry",     XrmoptionSepArg,      NULL },
    { "-waiting",       "*waiting",             XrmoptionSepArg,      NULL },
    { "-active",        "*active",              XrmoptionSepArg,      NULL },
    { "-selected",      "*selected",            XrmoptionSepArg,      NULL },
    { "-running",       "*running",             XrmoptionSepArg,      NULL },
    { "-zombie",        "*zombie",              XrmoptionSepArg,      NULL },
    { "-idle",          "*idle",                XrmoptionSepArg,      NULL },
    { "-stopped",       "*stopped",             XrmoptionSepArg,      NULL },
    { "-pscmdline",     "*pscmdline",           XrmoptionSepArg,      NULL },
    { "-pscmdinfo",     "*pscmdinfo",           XrmoptionSepArg,      NULL },
  };

  /*
   * The following is from G. Ferguson's xarchie;
   * Widget and non-widget resources if the application defaults
   * file can't be found.
   * Generated automatically from Xps.ad by "ad2c".
   * Comment out the include line (but not the NULL) if you don't want
   * any resources compiled in.
   */
  static String fallbackResources[] = {
#include "Xps.ad.h"
    NULL
  };

  /***************************************************************************/
  /* Resources that pertain to the application, not to either of the shells. */
  /***************************************************************************/

  static XtResource applResources[] = {
    { "interval",       "Interval",     XtRInt,      sizeof(int),
      XtOffset(ApplInfo *,interval),    XtRString,   XPS_DEFAULT_REFRESH_RATE},
    { "kernel",         "Kernel",       XtRBoolean,  sizeof(Boolean),
      XtOffset(ApplInfo *,showKernel),  XtRString,   XtEoff },
    { "zoomed",         "Zoomed",       XtRBoolean,  sizeof(Boolean),
      XtOffset(ApplInfo *,zoomed),      XtRString,   XtEoff },
    { "debugging",      "Debugging",    XtRInt,      sizeof(int),
      XtOffset(ApplInfo *,debugging),   XtRString,   "0" },
    { "beepup",         "BeepUp",       XtRString,   sizeof(XtRString),
      XtOffset(ApplInfo *,beep_sound_up), XtRString, "50 50 100 100 200 200"},
    { "beepdown",       "BeepDown",     XtRString,   sizeof(XtRString),
      XtOffset(ApplInfo *,beep_sound_down), 
                                        XtRString,   "200 200 100 100 50 50"},
    { "pscmdinfo",       "PsCmdInfo",   XtRString,   sizeof(XtRString),
      XtOffset(ApplInfo *, ps_cmd_info),XtRString,   PSCMD},
    { "pscmdline",        "PsCmdline",  XtRString,   sizeof(XtRString),
      XtOffset(ApplInfo *, ps_cmdline), XtRString,   PS_CMDLINE},
    { "swap",           XtCForeground,  XtRPixel,    sizeof(Pixel),
      XtOffset(ApplInfo *,nodeColor[SSWAP]),XtRString, "black" },
    { "waiting",        XtCForeground,  XtRPixel,    sizeof(Pixel),
      XtOffset(ApplInfo *,nodeColor[SWAIT]),XtRString,  "black" },
    { "active",        XtCForeground,  XtRPixel,     sizeof(Pixel),
      XtOffset(ApplInfo *,nodeColor[SACTIVE]),XtRString,   "white" },
    { "selected",        XtCForeground,  XtRPixel,   sizeof(Pixel),
      XtOffset(ApplInfo *,selected_color),XtRString, "cyan" },
    { "running",        XtCForeground,  XtRPixel,    sizeof(Pixel),
      XtOffset(ApplInfo *,nodeColor[SRUN]),XtRString,   "white" },
    { "zombie",         XtCForeground,  XtRPixel,    sizeof(Pixel),
      XtOffset(ApplInfo *,nodeColor[SZOMB]),XtRString,  "black" },
    { "idle",           XtCForeground,  XtRPixel,    sizeof(Pixel),
      XtOffset(ApplInfo *,nodeColor[SIDL]),XtRString,   "black" },
    { "stopped",        XtCForeground,  XtRPixel,    sizeof(Pixel),
      XtOffset(ApplInfo *,nodeColor[SSTOP]),XtRString,  "black" },
    { "uid1",            XtCForeground,  XtRInt,     sizeof(int),
      XtOffset(ApplInfo *,uid[1]),       XtRString,  "1" },
    { "usercolor1",      XtCForeground,  XtRPixel,   sizeof(Pixel),
      XtOffset(ApplInfo *,userColor[1]), XtRString,  "black" },
    { "uid2",            XtCForeground,  XtRInt,     sizeof(int),
      XtOffset(ApplInfo *,uid[2]),       XtRString,  "2" },
    { "usercolor2",      XtCForeground,  XtRPixel,   sizeof(Pixel),
      XtOffset(ApplInfo *,userColor[2]), XtRString,  "black" },
    { "uid3",            XtCForeground,  XtRInt,     sizeof(int),
      XtOffset(ApplInfo *,uid[3]),       XtRString,  "3" },
    { "usercolor3",      XtCForeground,  XtRPixel,   sizeof(Pixel),
      XtOffset(ApplInfo *,userColor[3]), XtRString,  "black" },
    { "uid4",            XtCForeground,  XtRInt,     sizeof(int),
      XtOffset(ApplInfo *,uid[4]),       XtRString,  "4" },
    { "usercolor4",      XtCForeground,  XtRPixel,   sizeof(Pixel),
      XtOffset(ApplInfo *,userColor[4]), XtRString,  "black" },
    { "uid5",            XtCForeground,  XtRInt,     sizeof(int),
      XtOffset(ApplInfo *,uid[5]),       XtRString,  "5" },
    { "usercolor5",      XtCForeground,  XtRPixel,   sizeof(Pixel),
      XtOffset(ApplInfo *,userColor[5]), XtRString,  "black" },
    { "uid6",            XtCForeground,  XtRInt,     sizeof(int),
      XtOffset(ApplInfo *,uid[6]),       XtRString,  "6" },
    { "usercolor6",      XtCForeground,  XtRPixel,   sizeof(Pixel),
      XtOffset(ApplInfo *,userColor[6]), XtRString,  "black" },
    { "uid7",            XtCForeground,  XtRInt,     sizeof(int),
      XtOffset(ApplInfo *,uid[7]),       XtRString,  "7" },
    { "usercolor7",      XtCForeground,  XtRPixel,   sizeof(Pixel),
      XtOffset(ApplInfo *,userColor[7]), XtRString,  "black" },
    { "uid8",            XtCForeground,  XtRInt,     sizeof(int),
      XtOffset(ApplInfo *,uid[8]),       XtRString,  "8" },
    { "usercolor8",      XtCForeground,  XtRPixel,   sizeof(Pixel),
      XtOffset(ApplInfo *,userColor[8]), XtRString,  "black" },
    { "xterm",          XtCForeground,   XtRPixel,   sizeof(Pixel),
      XtOffset(ApplInfo *,xterm),        XtRString,  "black" },
  };

  /***************************************************************************/
  /* Resources that pertain to the shell popups themselves.                  */
  /***************************************************************************/

  static XtResource shellResources[] = {
    { XtNfont,           XtCFont,       XtRFontStruct,  sizeof(XFontStruct *),
      XtOffset(ShellInfo *,font),       XtRString,      "fixed" },

    { XtNforeground,     XtCForeground, XtRPixel,       sizeof(Pixel),
      XtOffset(ShellInfo *,foreground), XtRString,      "black" },

    { "allprocs",       "Allprocs",     XtRBoolean,     sizeof(Boolean),
      XtOffset(ShellInfo *,allprocs),   XtRString,      XtEoff },

    { "beep",           "Beep",         XtRBoolean,     sizeof(Boolean),
      XtOffset(ShellInfo *,beep),       XtRString,      XtEoff },

#ifdef HAVE_KILL
    { "quickkill",      "QuickKill",    XtRBoolean,     sizeof(Boolean),
      XtOffset(ShellInfo *,quick_kill), XtRString,      XtEoff },
#endif

  };

  /***************************************************************************/
  /* Resources that really are just special command-line options             */
  /***************************************************************************/

  static XtResource cmdResources[] = {
    { "help",           "Help",        XtRBoolean,     sizeof(Boolean),
      XtOffset(cmd_opt *,help),        XtRString,      XtEoff },

    { "version",        "Version",     XtRBoolean,     sizeof(Boolean),
      XtOffset(cmd_opt *,version),     XtRString,      XtEoff },

    { "metoo",          "MeToo",       XtRBoolean,     sizeof(Boolean),
      XtOffset(cmd_opt *,me_too),      XtRString,      XtEoff },
  };

  /***************************************************************************/
  /* Actions defined by the application.                                     */
  /***************************************************************************/

  static XtActionsRec actions[] = {
    { "quit",         Quit },
    { "zoom",         Zoom },
    { "changespeed",  ChangeSpeed },
    { "togglebeep",   ToggleBeep },
    { "redisplay",    Redisplay },
    { "togglekernel", ToggleKernel },
    { "togglekill",   ToggleKill },
    { "toggleprocs",  ToggleProcs },
    { "showprocess",  ShowProcess },
  };

  /* A translation table only to make the process dialog pop up inside 
     the process-tree canvas */
  static char process_tree_trans[] = 
     "<Btn1Down>: showprocess()\n\
      <Btn2Down>: showprocess()\n\
      <Btn3Down>: showprocess(K)\n\
      <Key>Return: showprocess()\n\
      <Key>space: showprocess()"
      ;

#include "help/general.h"
#include "help/button.h"
#include "help/process_tree.h"
#include "help/keyfns.h"
#include "help/keymouse.h"
#include "help/warranty.h"

  static MenuDescription help_submenu[] = {
    { BUTTON,   "Overall layout",       help_callback, general_help},
    { BUTTON,   "Toggle/Radio buttons", help_callback, button_help},
    { BUTTON,   "Process tree",         help_callback, process_tree_help},
    { BUTTON,   "Key/Mouse binding",    help_callback, keymouse_help},
    { BUTTON,   "Key/Mouse actions",    help_callback, keyfns_help},
    { END }
  };
  
  static MenuDescription set_submenu[] = {
    { BUTTON,   "SetUser",     popup_set_user_dialog         },
    { BUTTON,   "UserColor",   popup_set_user_color          },
#ifdef HAVE_KILL
    { BUTTON,   "Kill",        popup_default_kill_dialog     },
#endif
    { BUTTON,   "RefreshRate", popup_refresh_rate_dialog     },
    { BUTTON,   "PsCmdline",   popup_pscmd_dialog            },
    { BUTTON,   "Debugging",   popup_debug_dialog            },
    { END }
  };
  
  /* Method for creating menus from Doug Young's Motif book. */
  static MenuDescription commandDesc[] = {
    /*  Widget type   Widget name   callback proc  callback data */        
    { PULLDOWN,      "Help",        NULL, NULL, help_submenu      },
    { PULLDOWN,      "Set",         NULL, NULL, set_submenu       },
    { BUTTON,        "NoWarranty",  help_callback, warranty_help  },
    { BUTTON,        "Config",      help_callback, config_info    },
    { SEPARATOR                                                   },
    { BUTTON,        "Quit",        QuitCallback                  },
    { END }
  };

  static MenuDescription menubar_desc[] = {
    /*  Widget type   Widget name   callback proc callback data  submenu */    
    { PULLDOWN,       "Commands",   NULL,         NULL,          commandDesc },
    { END }
  };

  char window_name[80];  /* Window -name option or name resource */
  char window_title[80]; /* Window -title option or title resource */

  /***************************************************************************/
  /* Create the toplevel shell.  This shell is never realized, instead it    */
  /* just serves as a common parent for the "normal" and "zoomed" windows,   */
  /* which are implemented as child popups.  I (smj) could not get zoomed    */
  /* mode to work by having only one window and resizing it (mwm ignored the */
  /* resize request, aixwm worked though).                                   */
  /***************************************************************************/

  toplevel = XtAppInitialize(&base.app_con, "Xps", opts, XtNumber(opts), 
			     &argc, argv, fallbackResources, NULL, 0);

  /* Register the functions that can be called on mouse/keystroke events.
     Must be done before XtGetApplicationResources.
   */
  XtAppAddActions(base.app_con, actions, XtNumber(actions));

  XtGetApplicationResources(toplevel,&cmd_options, cmdResources,
                            XtNumber(cmdResources), NULL, 0);

  frame = XtVaCreateManagedWidget( "mainWindow", xmMainWindowWidgetClass,
				   toplevel, 
				   NULL);
					
  base.program = (String) XtWidgetToName(toplevel);

  /* Figure out title and name use. Take the program name 
     as a default, but respect any options or resources that have been
     given. We take advantage of XtGetValues property of leaving the
     address unmodified if the property is not found. 
  */

  sprintf(window_title, "%s@%s", base.program, nodename);
  sprintf(window_name,  "%s@%s", base.program, nodename);

  if (cmd_options.version) {
    show_version();
  }
  

  if (cmd_options.help) {
    usage(0);
  }

  /* Extraneous arguments or -help. */  
  if (argc != 1) usage(2);

  /* 
     Set variables in base structure from the X application resources. 
     This is done before working on the main windows so we can set
     buttons based on the global variables such as "showKernel."
   */

  XtGetApplicationResources(toplevel, &base, applResources,
                            XtNumber(applResources), NULL, 0);

  /***************************************************************************
    Create both normal/zoomed shells exactly the same way, 
    differing only in name. 
  ***************************************************************************/

  for (base.zoomed=False; base.zoomed<=True; base.zoomed++) {
    Widget form, radio_box, radio_box_frame;
    
    active.shell = XtVaCreateManagedWidget(base.zoomed ? "zoomed" : "normal",
					   applicationShellWidgetClass,
					   frame,
					   XmNtitle, window_name,
					   XmNiconName, window_title, 
					   NULL);
    
    XtAddCallback(active.shell,XmNpopupCallback,Popup,NULL);

#ifdef DO_XEDITRESCHECKMESSAGES
    XtAddEventHandler(active.shell, (EventMask) 0, True, 
		      _XEditResCheckMessages, NULL);
#endif

    XtAddEventHandler(active.shell, 
		      (1L<<MapNotify), 
		      True, map_eventhandler, NULL);

    /* The outside form off of which every thing is hung from. */
    form = XtVaCreateManagedWidget("form", xmFormWidgetClass,
				   active.shell, 
				   NULL);

    /* The top line has a command menu and a bunch of buttons. */
    active.menu_bar = 
      XtVaCreateManagedWidget ("topline", xmRowColumnWidgetClass,
			       form,
			       XmNnumColumns,        4,
			       XmNorientation,       XmHORIZONTAL,
			       XmNtopAttachment,     XmATTACH_FORM,
			       XmNleftAttachment,    XmATTACH_FORM,
			       XmNrightAttachment,   XmATTACH_FORM,
			       XmNbottomAttachment,  XmATTACH_NONE,
			       NULL);

    XtAddCallback(active.menu_bar, XmNhelpCallback, help_callback, 
		  button_help);
    

    CreateMenu ( MENUBAR, "menubar", active.menu_bar, menubar_desc, NULL );
    
    /* Now add the button items */
    active.zoom_button = 
      XtVaCreateManagedWidget ("Zoom", xmToggleButtonWidgetClass, 
			       active.menu_bar, NULL);
    XtAddCallback(active.zoom_button, XmNvalueChangedCallback, 
		  ZoomCallback, NULL);
    
    active.beep_button  = 
      XtCreateManagedWidget ("Beep",
			     xmToggleButtonWidgetClass, 
			     active.menu_bar, NULL, 0);
    XtAddCallback(active.beep_button, XmNvalueChangedCallback, 
		  ToggleBeepCallback, (XtPointer) 2);

#ifdef HAVE_KILL    
    active.kill_button  = 
      XtCreateManagedWidget ("Quick Kill",
			     xmToggleButtonWidgetClass, 
			     active.menu_bar, NULL, 0);
    XtAddCallback(active.kill_button, XmNvalueChangedCallback, 
		  ToggleKillCallback, (XtPointer) 2);
#endif
    
    radio_box_frame = XtVaCreateManagedWidget("radio_box_frame", 
					      xmFrameWidgetClass,
					      active.menu_bar, 
					      NULL, 0);
    radio_box = XmCreateRadioBox(radio_box_frame, "radio_box", NULL, 0);
    XtVaSetValues ( radio_box, 
		    XmNorientation, XmHORIZONTAL,
		    XmNpacking, XmPACK_TIGHT,
		    XmNradioBehavior, True,
		    NULL );

#ifdef XPS_HAVE_KERNEL_PROCS
    active.kernel_button =
      XtCreateManagedWidget ("Kernel",
			     xmToggleButtonWidgetClass,
			     radio_box, NULL, 0);

    XtAddCallback(active.kernel_button, XmNvalueChangedCallback, 
		  ToggleProcsCallback, (XtPointer) 1);
#endif
    
    active.procs_button =
      XtCreateManagedWidget ("All",
			     xmToggleButtonWidgetClass,
			     radio_box, NULL, 0);

    XtAddCallback(active.procs_button, XmNvalueChangedCallback, 
		  ToggleProcsCallback, (XtPointer) 2);
    
    active.user_button =
      XtCreateManagedWidget ("User",
			     xmToggleButtonWidgetClass,
			     radio_box, NULL, 0);

    XtAddCallback(active.user_button, XmNvalueChangedCallback, 
		  ToggleProcsCallback, (XtPointer) 3);

    XtManageChild(radio_box);
    
    /* Put a message status at the bottom. */
    active.message2 = XtVaCreateManagedWidget( "message2", xmLabelWidgetClass,
				       form,
				       XmNtraversalOn,       True,
                                       XmNtopAttachment,     XmATTACH_NONE,
				       XmNleftAttachment,    XmATTACH_FORM,
				       XmNrightAttachment,   XmATTACH_NONE,
                                       XmNbottomAttachment,  XmATTACH_FORM,
				       NULL );

    XtAddCallback(active.message2, XmNhelpCallback, help_callback, 
		  config_info);

    /* Another message status right abouve this. */
    active.message = XtVaCreateManagedWidget( "message", xmLabelWidgetClass,
				       form,
				       XmNtraversalOn,       False,
                                       XmNtopAttachment,     XmATTACH_NONE,
				       XmNleftAttachment,    XmATTACH_FORM,
				       XmNrightAttachment,   XmATTACH_NONE,
                                       XmNbottomAttachment,  XmATTACH_WIDGET,
			               XmNbottomWidget,      active.message2,
				       NULL );

    /* A scrollable window for the drawing area. It is 
       in the middle between the command bar and the status area. */
    active.scroll = 
      XtVaCreateManagedWidget ("scrolledWindow", 
			       xmScrolledWindowWidgetClass, 
			       form, 
			       XmNtopAttachment,      XmATTACH_WIDGET,
			       XmNleftAttachment,     XmATTACH_FORM,
			       XmNrightAttachment,    XmATTACH_FORM,
			       XmNbottomAttachment,   XmATTACH_WIDGET,
			       XmNtopWidget,          active.menu_bar,
			       XmNbottomWidget,       active.message,
			       XmNscrollingPolicy,    XmAUTOMATIC,
			       XmNscrollBarDisplayPolicy, XmSTATIC,
			       NULL );

    /* The main tree graph is in scrolling region. */
    active.canvas = 
      XtVaCreateManagedWidget ("canvas",
			       xmDrawingAreaWidgetClass,
			       active.scroll,
			       XmNwidth,  200,
			       XmNheight, 100,
			       XmNunitType, Xm100TH_FONT_UNITS,
			       XmNtraversalOn, True,
			       NULL );

    /* Not sure how to set this in an X Resources, file it's so done here.
       If you know how to do properly, let me know. */
    XtOverrideTranslations(active.canvas,
			   XtParseTranslationTable (process_tree_trans));
    
    XtAddEventHandler(active.canvas, 
		      VisibilityChangeMask,
		      True, visibility_eventhandler, NULL);

    XtAddEventHandler(active.canvas, 
		      (1L<<UnmapNotify),
		      True, map_eventhandler, NULL);

    XtAddCallback(active.canvas, XmNexposeCallback,   Redraw,      NULL);
    XtAddCallback(active.canvas, XmNresizeCallback,   Resize,      NULL);
    XtAddCallback(active.canvas, XmNhelpCallback,     help_callback, 
		  process_tree_help);
    
    XtGetApplicationResources(active.shell, &active, shellResources,
                              XtNumber(shellResources), NULL, 0);

    /* fontHeight is used in determining overall height when tree is 
       built. */
    active.fontHeight = active.font->max_bounds.ascent
                      + active.font->max_bounds.descent;

    {
      Arg args[2];
      XtSetArg(args[0], XmNwidth,  &active.width);
      XtSetArg(args[1], XmNheight, &active.height);
      /* Set the canvas as the initial focus buttons aren't highlighted
         and so one can select with the space bar a process. */
      XtGetValues(frame, args, 2);
      XtSetValues(active.canvas, args, 2); 
      XtSetArg(args[0], XmNinitialFocus, &active.canvas);
      XtSetValues(frame, args, 1); 
    }

    XmToggleButtonSetState(active.zoom_button, base.zoomed, False);
    XmToggleButtonSetState(active.kill_button, active.quick_kill, False);
#ifdef XPS_HAVE_KERNEL_PROCS
    if (active.allprocs) {
	if (base.showKernel) 
	  XmToggleButtonSetState(active.kernel_button, True, False);
	else 
	  XmToggleButtonSetState(active.procs_button, True, False);
    }
#else 
    XmToggleButtonSetState(active.procs_button, active.allprocs, False);
#endif
    XmToggleButtonSetState(active.user_button, !active.allprocs, False);
    XmToggleButtonSetState(active.beep_button, active.beep, False);
    
  } /*for*/


  /* This is done above we get a core dump in XtPopup if not done
     here. Don't know why. If you do, let me know. */
  XtGetApplicationResources(toplevel, &base, applResources,
                            XtNumber(applResources), NULL, 0);

  base.lineStyle = XCreateGC(XtDisplay(frame),
                             DefaultRootWindow(XtDisplay(frame)),
                             0,(XGCValues *) 0);

  base.nodeStyle = XCreateGC(XtDisplay(frame),
                             DefaultRootWindow(XtDisplay(frame)),
                             0,(XGCValues *) 0);

  XtPopup(active.shell, XtGrabNone);
  /* Give focus to the canvas as this is the least annoying thing. */
  XmProcessTraversal(active.canvas, XmTRAVERSE_CURRENT);

  create_process_menu( frame, "process_menu" );

  /* Are we visible? Set based in initial value of iconic.  */
  {
      Arg args[1];
      Boolean iconic; /* Wouldn't need if we had coded base.Invisible */
      XtSetArg(args[0], XtNiconic,  &iconic);
      XtGetValues(active.shell, args, 1);
      base.visible = !iconic;
  }

  /* Check that value set for interval is valid.  */
  if (!is_valid_interval(active.shell, base.interval)) {
    long int interval;
    int count = sscanf(XPS_DEFAULT_REFRESH_RATE, "%ld", &interval);
    if (count != 1) interval = 1;
    base.interval = interval;
  }
  
  base.interval *= XPS_TICK;
  Timeout(NULL, NULL);

  return(base.app_con);
}
