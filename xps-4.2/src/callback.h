/* 
  The simpler X callbacks -- called when by various X events 
  (mouse clicks, key presses, expose requests)
  Some crud could be cleaned from this.

  $Id: callback.h,v 1.1 2002/12/01 22:00:56 rockyb Exp $ 
  Copyright (C) 1998 Rocky Bernstein
  
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

/* Set change is to be applied to both windows */
extern Boolean set_both_windows;

extern void Redisplay(Widget w, XEvent *event, String *parms, Cardinal *count);
extern void RedisplayCallback(Widget w, XtPointer client_data, 
			      XtPointer call_data);
extern void Redraw(Widget w, XtPointer client_data, XtPointer call_data);

extern Boolean is_valid_interval(Widget w, long int value);

/**************************************************************************
 Time to refresh our view of the world... set up to 
 come back again a little later. 
**************************************************************************/
extern void Timeout(XtPointer closure, XtIntervalId* id);

/*************************************************************************
  Change the update interval by the amount specified.
*************************************************************************/
extern void ChangeSpeed(Widget w, XEvent *event, String *args, 
			Cardinal *n_args);

/**************************************************************************
 "Quit" action and callback : Close up shop.                                  
***************************************************************************/
extern void Quit(Widget w, XEvent *event, String *args, Cardinal *n_args);
extern void QuitCallback(Widget w, XtPointer clientData, XtPointer callData);

/**************************************************************************
  Toggle showing user process vs. all processes, kernel processes or not.
***************************************************************************/
extern void ToggleProcsCallback(Widget w, XtPointer client_data, 
				XtPointer call_data);
extern void ToggleProcs(Widget w, XEvent *event, String *args, 
			Cardinal *n_args);

extern void ToggleKernel(Widget w, XEvent *event, String *args, 
			 Cardinal *n_args);
/**************************************************************************
 Called when switching between normal and zoomed modes. Switch GC stuff. 
**************************************************************************/
extern void Popup(Widget w, XtPointer client_data, XtPointer call_data);
/***************************************************************************/
/* "Zoom" action: Unmap the active popup, switch to other, and then map it.*/
/***************************************************************************/
extern void Zoom(Widget w, XEvent *event, String *args, Cardinal *n_args);
extern void ZoomCallback(Widget w, XtPointer client_data, XtPointer call_data);
extern void ToggleBeepCallback(Widget w, XtPointer client_data, 
			       XtPointer call_data);
extern void ToggleBeep(Widget w, XEvent *event, String *args, 
		       Cardinal *n_args);

extern void ToggleKillCallback(Widget w, XtPointer client_data, 
			       XtPointer call_data);
extern void ToggleKill(Widget w, XEvent *event, String *args, 
		       Cardinal *n_args);

extern void ToggleBoth(Widget w, XtPointer client_data, XtPointer call_data);

/* Online help from the help button goes here. */
extern void help_callback(Widget w, XtPointer client_data, 
			  XtPointer call_data);

extern void popdown_refresh_rate_dialog(Widget dialog, XtPointer client_data, 
					XtPointer call_data);

/**************************************************************************
Handle visibility and map/unmap events
***************************************************************************/
extern void visibility_eventhandler(Widget w, XtPointer client_data, 
				    XEvent *event, 
				    Boolean *continueToDispatch);

extern void map_eventhandler(Widget w, XtPointer client_data, 
			     XEvent *event, Boolean *continueToDispatch);

/* Generic event handler. For debugging. */
extern void eventhandler(Widget w, XtPointer client_data, 
			 XEvent *event, Boolean *continueToDispatch);

/**************************************************************************
 The shell window has been resized, and since the drawing is contained in 
 a form under the shell, it too will be resized.  
***************************************************************************/
extern void Resize(Widget w, void *client, void *call);


