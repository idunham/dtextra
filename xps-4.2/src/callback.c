/* 
  The simpler X callbacks and event handlers. Called via various X events 
  (mouse clicks, key presses, expose requests)
  Some crud could be cleaned from this.

  $Id: callback.c,v 1.2 2003/04/24 00:21:29 rockyb Exp $
  Copyright (C) 1998,99 Rocky Bernstein
 
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
#include "xstuff.h"
#include "callback.h"
#include "common.h"
#include "askquestion.h"
#include <Xm/DialogS.h>
#include <Xm/ToggleB.h>

Boolean set_both_windows;

Boolean 
is_valid_interval(Widget w, long int value) 
{
  if (value >=XPS_MIN_REFRESH_RATE && value <= XPS_MAX_REFRESH_RATE) {
    return True;
  } else {
    char buf[80];
    sprintf(buf, 
    "Invalid refresh rate interval %ld.\nValue should be between %d and %d.", 
     value, XPS_MIN_REFRESH_RATE, XPS_MAX_REFRESH_RATE);
    ShowDialog(w, XmDIALOG_ERROR, buf );
    return False;
  }
}

/* 
  Common "toggle" routine to pick out that argument is empty or is one
  argument with a value 0, 1, or 2. This returns -1 if error.  
*/
static int 
parse_toggle_arg(const Widget w, const char *fn_name, 
		 const String *args, const Cardinal *n_args) 
{
  char buf[300];
  if (base.debugging & DBG_CALLBACK) {
    if (*n_args) {
      printf("%s: %d parameters, 1st one: %s\n", fn_name, *n_args, *args);
    } else {
      printf("%s: no parameters.\n", fn_name);
    }
  }

  if (0 == *n_args) return 2;
  if (1 == *n_args) {
    switch(*args[0]) {
    case '0': 
    case '1': 
    case '2': 
      return (*args[0]-'0');
      break;
    default:
      sprintf(buf, 
      "%s: invalid argument %s passed.\nShould be an empty, 0,1 or 2.",
	      fn_name, *args);
      ShowDialog( w, XmDIALOG_ERROR, buf);
      return -1;
    }
  } else {
    sprintf(buf, 
	    "%s: invalid argument count %d passed.\nShould be 1.",
	    fn_name, *n_args);
    ShowDialog( w, XmDIALOG_ERROR, buf);
    return -1;
  }

}

/**************************************************************************
 User forces a redisplay.
***************************************************************************/

void 
Redisplay(Widget w, XEvent *event, String *parms, Cardinal *count)
{
  ForceRedisplay();
}

void 
RedisplayCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
  ForceRedisplay();
}

/*************************************************************************
  The window has been exposed, force a redraw even if nothing has changed
*************************************************************************/
void
Redraw(Widget w, XtPointer client_data, XtPointer call_data)
{
  if (base.debugging & DBG_CALLBACK) printf ("Redraw callback called.\n"); 
  ForceRedisplay();
}

/**************************************************************************
 Time to refresh our view of the world... set up to 
 come back again a little later. 
**************************************************************************/
void 
Timeout(XtPointer closure, XtIntervalId* id)
{
  if (base.visible) {
    display(False,XtDisplay(active.canvas),XtWindow(active.canvas));
  }
  XtAppAddTimeOut(base.app_con, base.interval, Timeout, NULL);
}

/*************************************************************************
  Change the update interval by the amount specified.
*************************************************************************/
void
ChangeSpeed(Widget w, XEvent *event, String *args, Cardinal *n_args)
{
  long int increment;
  long int interval = base.interval / XPS_TICK;
  
  if (base.debugging & DBG_CALLBACK) {
    if (*n_args) {
      printf("ChangeSpeed: %d parameters, 1st one: %s\n", *n_args, *args);
    } else {
      printf("ChangeSpeed: no parameters.\n");
    }
  }

  if (*n_args) {
    int count = sscanf(*args, "%ld", &increment);
    if (count != 1) {
      char buf[300];
      sprintf(buf, 
      "change_speed: invalid argument %s passed.\nShould be an integer.",
	      *args);
      ShowDialog( w, XmDIALOG_ERROR, buf);
    }
  } else {
    ShowDialog( w, XmDIALOG_ERROR, 
		"change_speed: no arguments passed.\nNothing done." );
  }

  interval += increment;
  if (!is_valid_interval(w, interval)) {
    return;
  }
  
  base.interval = interval * XPS_TICK;
  updateMessage();
  Timeout(NULL, NULL);
}

/**************************************************************************
 "Quit" action and callback : Close up shop.                                  
***************************************************************************/
static inline void 
QuitCommon(Widget w)
{
  XtUnmapWidget(active.shell);
  XCloseDisplay(XtDisplay(w));
  exit(0);
}

void 
Quit(Widget w, XEvent *event, String *args, Cardinal *n_args)
{
  QuitCommon(w);
}

void 
QuitCallback(Widget w, XtPointer clientData, XtPointer callData)
{
  QuitCommon(w);
}

/**************************************************************************
  Toggle showing user process vs. all processes.
***************************************************************************/
static void 
ToggleProcsCommon(int action)
{
  switch(action) {
  case 1: 
    base.showKernel = True;
    active.allprocs = True;
    break;
  case 2:
    base.showKernel = False;
    active.allprocs = True;
    break;
  case 3:
    base.showKernel = False;
    active.allprocs = False;
    break;
  case 4:
    active.allprocs = !active.allprocs;
    break;
  case 5:
    base.showKernel = !base.showKernel;
    break;
  default:
    printf("Error in passing callback value: %d. Nothing done.\n", 
	   action);
  }

  if (base.debugging & DBG_CALLBACK)
    printf("Showing procs is %d\n", active.allprocs);
  ForceRedisplay();
}

void 
ToggleProcsCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
  int which = (int) client_data;
  XmToggleButtonCallbackStruct *state = 
    (XmToggleButtonCallbackStruct *) call_data;

  if (state->set) {
    ToggleProcsCommon(which);
  }
  
}

void 
ToggleProcs(Widget w, XEvent *event, String *args, Cardinal *n_args)
{
  ToggleProcsCommon( 4 );
#ifdef XPS_HAVE_KERNEL_PROCS 
  XmToggleButtonSetState(active.kernel_button, 
			 active.allprocs && base.showKernel, False);
  XmToggleButtonSetState(active.procs_button, 
			 active.allprocs && !base.showKernel, False);
#else
  XmToggleButtonSetState(active.procs_button, 
			 active.allprocs, True);
#endif
  XmToggleButtonSetState(active.user_button,  !active.allprocs, False);
}

void 
ToggleKernel(Widget w, XEvent *event, String *args, Cardinal *n_args)
{
  ToggleProcsCommon( 5 );
#ifdef XPS_HAVE_KERNEL_PROCS 
  XmToggleButtonSetState(active.kernel_button, 
			 active.allprocs && base.showKernel, False);
  XmToggleButtonSetState(active.procs_button, 
			 active.allprocs && !base.showKernel, False);
#else
  XmToggleButtonSetState(active.procs_button, 
			 active.allprocs, True);
#endif
  XmToggleButtonSetState(active.user_button,  !active.allprocs, False);
}

/**************************************************************************
 Called when switching between normal and zoomed modes. Switch GC stuff. 
**************************************************************************/
void
Popup(Widget w, XtPointer client_data, XtPointer call_data)
{
  XSetForeground(XtDisplay(w),base.lineStyle,active.foreground);
  XSetFont(XtDisplay(w),base.nodeStyle,active.font->fid);
}

/***************************************************************************/
/* "Zoom" action: Unmap the active popup, switch to other, and then map it.*/
/***************************************************************************/
static inline void 
ZoomCommon(void)
{
  XtPopdown(active.shell);
  base.zoomed = !base.zoomed;
  XmToggleButtonSetState(active.zoom_button, base.zoomed, False);
  updateMessage();
  XtPopup(active.shell,XtGrabNone);
  /* Give focus to the canvas as this is the least annoying thing. */
  XmProcessTraversal(active.canvas, XmTRAVERSE_CURRENT);
}

void 
Zoom(Widget w, XEvent *event, String *args, Cardinal *n_args)
{
  ZoomCommon();
}

void 
ZoomCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
  ZoomCommon();
}

static void 
ToggleCommon(int action, Boolean *toggle, Widget w)
{
  switch(action) {
  case 0: case 1:
    *toggle = action;
    break;
  case 2:
    *toggle = !*toggle;
    break;
  default:
    printf("Error in passing callback value: %d. Nothing done.\n", 
	   action);
  }

  if (base.debugging & DBG_CALLBACK) {
    printf("Toggled %s: value: %d\n", XtName(w), *toggle);
  }
  
  XmToggleButtonSetState(w, *toggle, False);

}

void 
ToggleBeepCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
  ToggleCommon( (int) client_data, &active.beep, active.beep_button );
}

void 
ToggleKillCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
  ToggleCommon( (int) client_data, &active.quick_kill, active.kill_button );
}

void 
ToggleBeep(Widget w, XEvent *event, String *args, Cardinal *n_args)
{
  int toggle=parse_toggle_arg(w, "ToggleBeep", args, n_args);
  if (toggle != -1) 
  ToggleCommon( toggle, &active.beep, active.beep_button );
}

void 
ToggleKill(Widget w, XEvent *event, String *args, Cardinal *n_args)
{
  int toggle=parse_toggle_arg(w, "ToggleKill", args, n_args);
  if (toggle != -1) 
    ToggleCommon( toggle, &active.quick_kill, active.kill_button );
}

void 
ToggleBoth(Widget w, XtPointer client_data, XtPointer call_data)
{
  ToggleCommon( 2, &set_both_windows, w );
}

/* Online help from the help button goes here. */
void
help_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
  char *message = (char *) client_data;
  ShowDialog( w, XmDIALOG_INFORMATION, message );
}

void
popdown_refresh_rate_dialog(Widget dialog, XtPointer client_data, 
			    XtPointer call_data) 
{
  XtDestroyWidget(dialog);
  ForceRedisplay();
}


void
visibility_eventhandler(Widget w, XtPointer client_data, 
			XEvent *event, Boolean *continueToDispatch)
{
  switch( ((XVisibilityEvent *) event)->state ) {
  case VisibilityUnobscured: 
    if (base.debugging & DBG_CALLBACK) 
      printf("Event: Unobscured\n");
    base.visible = True;
    break;
  case VisibilityPartiallyObscured: 
    if (base.debugging & DBG_CALLBACK) 
      printf("Event: Partially obscured\n");
    base.visible = True;
    break;
  case VisibilityFullyObscured: 
    if (base.debugging & DBG_CALLBACK) 
      printf("Event: Fully obscured\n");
    base.visible = False;
    break;
  default:
    if (base.debugging & DBG_CALLBACK) 
      printf("Unknown event %d\n", event->type);
    break;
  }
}

void
map_eventhandler(Widget w, XtPointer client_data, 
		 XEvent *event, Boolean *continueToDispatch)
{
  switch( event->type ) {
  case MapNotify: 
    if (base.debugging & DBG_CALLBACK) 
      printf("Event: MapNotify\n");
    base.visible = True;
    break;
  case UnmapNotify: 
    if (base.debugging & DBG_CALLBACK) 
      printf("Event: UnmapNotify\n");
    base.visible = False;
    break;
  case ConfigureNotify: 
    if (base.debugging & DBG_CALLBACK) 
      printf("Event: ConfigureNotify\n");
    break;
  case CreateNotify: 
    if (base.debugging & DBG_CALLBACK) 
      printf("Event: CreateNotify\n");
    break;
  case ReparentNotify: 
    if (base.debugging & DBG_CALLBACK) 
      printf("Event: ReparentNotify\n");
    break;
  case ResizeRequest: 
    if (base.debugging & DBG_CALLBACK) 
      printf("Event: ResizeRequest\n");
    break;
  default:
    if (base.debugging & DBG_CALLBACK) 
      printf("Unknown map/unmap event %d\n", event->type);
    break;
  }
  *continueToDispatch=True;
}

void
eventhandler(Widget w, XtPointer client_data, 
		      XEvent *event, Boolean *continueToDispatch)
{
  printf("Handler called %d\n", event->type);
}

/***************************************************************************
 The shell window has been resized, and since the drawing is contained in 
 a form under the shell, it too will be resized.  
***************************************************************************/
extern void 
Resize(Widget w, void *client, void *call)
 {

   /*  active.width  = 750;
       active.height = 338; */

  /***************************************************************************/
  /* It is possible that a resize will NOT cause an exposure (eg window is   */
  /* made smaller), so we may need to call for a redisplay from here. Note   */
  /* resize() may be called during widget creation but before the shell is   */
  /* realized. If it has been, and there is no expose pending, we'll redraw. */
  /* This saves some flickering over always calling for a redraw from here.  */
  /*                                                                         */
  /* 05/26/90: I just found out that part of the reason why we may not get   */
  /*           an exposure as well is that Motif 1.0 XmDrawingArea widget    */
  /*           calls this callback from its resize method, and NOT as a      */
  /*           result of a ConfigureNotify, which it will be in Motif 1.1.   */
  /*           Thus, in 1.1, the code below will be spurious.                */
  /***************************************************************************/

  if (XtIsRealized(w)) {
    XEvent event;

    if (!XCheckTypedWindowEvent(XtDisplay(w),XtWindow(w),Expose,&event) ||
        event.xexpose.count == 0) {
      ForceRedisplay();
    }
  }
}
