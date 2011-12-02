/* Motif process menu and menu handling 

  $Id: process.c,v 1.2 2003/04/24 00:21:29 rockyb Exp $
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
#include <sys/types.h>
#include <Xm/Xm.h>
#include <Xm/Scale.h>
#include <Xm/List.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/PushB.h>
#include <Xm/MessageB.h>
#include <Xm/ToggleB.h>
#include <Xm/ScrolledW.h>
#include <Xm/Separator.h>

#if defined(HAVE_KILL) && defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif

#include "xstuff.h"
#include "common.h"
#include "dialog.h"
#include "process.h"
#ifdef HAVE_KILL
#include "sigmap.h"
const int sigmap_elts = sizeof(signal_map) / sizeof(sigmap_el);
#endif
#include "askquestion.h"
#include "callback.h"

#ifdef HAVE_SYS_RESOURCE_H
#include <errno.h>
#include <sys/resource.h>
#endif

/* Variables local to the process dialog menu and its actions */

int priority=5;
Boolean do_process_group = False;

static int   last_global_signal_pos  = XPS_DEFAULT_QUICK_KILL_SIGNAL-1;
static int   dialog_signal_pos;

static Widget       
              process_form,          /* process form */
              process_dialog,        /* process dialog */
              ps_info,               /* Process id info           */
              priority_slider,       /* scale for process priority */
              process_group_button;  /* button doing on process group */
#ifdef HAVE_SETPRIORITY
Boolean do_priority = False;
static  Widget priority_button;      /* button doing on priority group */
#endif
#ifdef HAVE_KILL
static Widget signal_list;           /* scrollbar of list of signal names */
static void   SignalListCallback ( Widget w, XtPointer client_data, 
				   XtPointer call_data);
#endif

#ifdef HAVE_KILL
static void 
do_kill(Widget w, pid_t pid, signal_t signal_value)
{
  if (base.debugging & DBG_MENU) 
    printf("kill %ld %d\n", (long int) pid, signal_value);
#ifdef RUN_SUID_ROOT
  seteuid(real_uid);
#endif    
  if (kill(pid, signal_value) == -1) {
    /* beep if we can't deliver signal to process */
    XBell(XtDisplay(w),0);  
    {
#ifdef HAVE_STRERROR      
      char buf[300];
      sprintf(buf, "kill signal error:\n%s", strerror(errno));
      ShowDialog(w, XmDIALOG_ERROR, buf);
#else       
      ShowDialog(w, XmDIALOG_ERROR, "kill signal error.");
#endif /*HAVE_STRERROR*/
    }
  }
#ifdef RUN_SUID_ROOT
  seteuid(0);
#endif    
}
#endif /* HAVE_KILL */


/***************************************************************************
  Deliver the requested signal to the process, or process group.
  Or change the priority -- not done yet.
  Beep if we fail.         
***************************************************************************/
static void 
do_process_action_callback(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
  signal_t signal_value;
  int *signal_pos = (int *) client_data;

#ifdef HAVE_SETPRIORITY
  if (do_priority) {
    int priority_type = (do_process_group) ? PRIO_PGRP : PRIO_PROCESS;
    errno = 0;
#ifdef RUN_SUID_ROOT
    seteuid(real_uid);
#endif  /*RUN_SUID_ROOT*/
    if (setpriority(priority_type, base.selected_pid, priority) == -1 
	&& errno !=0 ) {
      /* beep if we can't set the priority. */
      XBell(XtDisplay(w),0);  
      { 
#ifdef HAVE_STRERROR      
	char buf[300];
	sprintf(buf, "Can't set priority:\n%s", strerror(errno));
	ShowDialog(w, XmDIALOG_ERROR, buf);
#else       
	ShowDialog(w, XmDIALOG_ERROR, "Can't set priority.");
#endif /*HAVE_STRERROR*/
      }
    };
#ifdef RUN_SUID_ROOT
    seteuid(0);
#endif    
  }
#endif /*HAVE_SETPRIORITY*/

  signal_value = signal_map[*signal_pos].sig;
  if (do_process_group) {
    signal_value = -signal_value;
  }

  if (base.debugging & DBG_MENU) 
    printf("priority: %d, do_process_group %d, sig %d\n", 
	   priority, do_process_group, signal_value);

  if (signal_value != 0 ) {
    do_kill(w, base.selected_pid, signal_value);
  }
  base.selected_pid = BOGUS_PID; 
  ForceRedisplay();

}

static void 
do_process_cancel_callback(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
  base.selected_pid = -5;
  ForceRedisplay();
}

static void 
toggle_button_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
  Boolean *toggle = (Boolean *) client_data;
  *toggle = ! *toggle;
}

#ifdef HAVE_KILL
static void 
SignalListCallback ( Widget w, XtPointer client_data, XtPointer call_data)
{
  XmListCallbackStruct *cbs = (XmListCallbackStruct * ) call_data;
  int *signal_pos           = (int *) client_data;
  *signal_pos               = cbs->item_position-1;
}
#endif

/*************************************************************************
  Callback to priority value on process popup menu. 
  The actual setting is done in do_process_action_callback().
*************************************************************************/
static void
set_priority_callback(Widget scale_w, XtPointer client_data, 
		      XtPointer call_data)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  if (cbs->value <= PRIO_MAX && cbs->value >= PRIO_MIN ) {
    priority = cbs->value;
  }
}

#define MAXLINE 512
#define MAXSTATUS 256

/***************************************************************************
  This routine is called to pop up the process dialog/form.
  This is done only after obtaining information about the process
  and setting various fields.
  
  The form can be used to shows process information, set process
  priority and/or send a signal to the process or process group.

  If want_kill is True and active.quick_killis True, this routine instead
  sends the signal last selected to the process.
***************************************************************************/
void 
Handle_Process(Widget w, pid_t pid, XEvent *event, Boolean want_kill)
{

  char line[MAXLINE];
  FILE *fp;
  
  XmProcessTraversal(active.canvas, XmTRAVERSE_CURRENT);
#ifdef HAVE_KILL
  if (active.quick_kill && want_kill) {
    if (last_global_signal_pos>=0 && last_global_signal_pos<sigmap_elts) {
      do_kill(w, pid, signal_map[last_global_signal_pos+1].sig);
      ForceRedisplay(); 
    } else {
      char buf[300];
      sprintf(buf, "Invalid signal position in signal list: %d\n", 
	      last_global_signal_pos+1);
      ShowDialog(w, XmDIALOG_ERROR, buf);
    }
    return;
  }
#endif

  base.selected_pid = pid;
  ForceRedisplay(); 

  /* Set priority to current priority value for the process */
#ifdef HAVE_GETPRIORITY
  errno = 0;
  priority = getpriority(PRIO_PROCESS, base.selected_pid);
  if ( priority == -1 && errno != 0 ) {
      /* beep if we can't get the priority. */
      XBell(XtDisplay(active.shell),0);  
  } else {
    XmScaleSetValue(priority_slider, priority);
  }
#endif
  
#ifdef HAVE_KILL
  /* Unless we have selected quick kill, make sure position 1 -- no
     action is selected for the signal value.  */
  if (active.quick_kill) {
    /* Off by one because we have "NO ACTION" while other menu doesn't */
    XmListSelectPos(signal_list, last_global_signal_pos+1+1, False);
  } else {
    XmListSelectPos(signal_list, 1, False);
  }
#endif
  
  /* process group button is not pushed/selected */
  do_process_group = False;
  XmToggleButtonSetState(process_group_button, do_process_group, False);

#ifdef HAVE_SETPRIORITY
  do_priority = False;
  XmToggleButtonSetState(priority_button, do_priority, False);
#endif
  
  /* Set process command info . */
  sprintf(line, base.ps_cmd_info, base.selected_pid); 
  XmTextRemove(ps_info);
  /*XmScrollBarSetValues(ps_info, 1, 4, 1, 4, False);*/
  fp = popen(line, "r");
  if (fp != NULL) {
    int pos;
    XmTextSetString(ps_info, "");
    while(fgets(line, MAXLINE, fp) != NULL) {
      pos = XmTextGetLastPosition(ps_info);
      XmTextInsert(ps_info, pos, line);
    }
  }
  pclose(fp);

  /* Set to show full command-line invocation. */
  sprintf(line, base.ps_cmdline, base.selected_pid); 
  fp = popen(line, "r");
  if (fp != NULL) {
    int pos;
    while(fgets(line, MAXLINE, fp) != NULL) {
      pos = XmTextGetLastPosition(ps_info);
      XmTextInsert(ps_info, pos, line);
    }
    pclose(fp);
  }

  XtManageChild(ps_info);
  XtManageChild(process_dialog);
  /* By itself the popup appears at 0,0. I don't know why 
     XMenuPosition doesn't do better. */
  /*
    XmMenuPosition(process_dialog, (XButtonPressedEvent *) event);
  */
#ifdef TESTING
  switch (event->type) {
  case ButtonPress:
    XtVaSetValues(process_dialog, 
		  XmNx, event->xbutton.x,
		  XmNy, event->xbutton.y,
		  NULL);
    break;
    
  case KeyPress:
    XtVaSetValues(process_dialog, 
		  XmNx, event->xkey.x,
		  XmNy, event->xkey.y,
		  NULL);
    break;
  default:
  }
#endif
}

/***************************************************************************
  Creates the process message dialog and callbacks.  Although many
  static items like signal names are set up, values for various fields
  like the priority and and process information are reassigned only
  after a particular process is selected.
***************************************************************************/
Widget
create_process_menu(Widget parent, char *name)
{
  Widget form;

#include "help/process_window.h"
#ifdef HAVE_KILL
  int i;
  XmStringTable xmstr;

  xmstr = ( XmString * ) XtMalloc (sizeof ( XmString ) * sigmap_elts ) ;
  for  ( i = 0; i < sigmap_elts; i++ ) {
    xmstr[i] = XmStringCreateLtoR ( signal_map[i].signal_name, 
				    XmFONTLIST_DEFAULT_TAG );
  }
#endif

  /* Create the dialog widget. */
  process_dialog = XmCreateMessageDialog ( parent, name, NULL, 0 );

  XtAddCallback(process_dialog, XmNokCallback, do_process_action_callback, 
		&dialog_signal_pos);

  XtAddCallback(process_dialog, XmNcancelCallback, do_process_cancel_callback, 
		NULL);

  XtAddCallback(process_dialog, XmNhelpCallback, help_callback, 
		process_window_help);

  /* 
     Remove unneeded buttons and children. 
   */

  /* This is something along the top line */
  XtUnmanageChild( XmMessageBoxGetChild ( process_dialog, 
					  XmDIALOG_SYMBOL_LABEL ) );
  XtUnmanageChild( XmMessageBoxGetChild ( process_dialog, 
					  XmDIALOG_MESSAGE_LABEL ) );
  /* Create a manager widget as a child of the dialog, to be used
     as the work area of the dialog, replacing the label and icon. 
  */

  process_form = XtVaCreateManagedWidget( "form", xmFormWidgetClass, 
					  process_dialog, NULL ) ;

  form = 
    XtVaCreateManagedWidget("processForm", xmFormWidgetClass,
			    process_form,
			    XmNorientation,       XmVERTICAL,
			    XmNtopAttachment,     XmATTACH_NONE,
			    XmNleftAttachment,    XmATTACH_FORM,
			    XmNrightAttachment,   XmATTACH_FORM,
			    XmNbottomAttachment,  XmATTACH_FORM,
			    NULL);

  /* Widget for ps info. XmCreateText doesn't do variable args.
     So set up a local temp argument array. Uck! Motif sucks. */
  {
    Arg args[20];
    int n = 0;
    
    XtSetArg(args[n], XmNeditable, False); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    XtSetArg(args[n], XmNscrollHorizontal, True); n++;
    XtSetArg(args[n], XmNscrollVertical, True); n++;
    XtSetArg(args[n], XmNtraversalOn, True); n++;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,       form); n++;
    /* done as X resource, but must be done. 
    XtSetArg(args[n], XmNwidth,              8000); n++;
    XtSetArg(args[n], XmNheight,             1000); n++; 
    XtSetArg(args[n], XmNunitType,           Xm100TH_FONT_UNITS); n++; 
    */
    XtSetArg(args[n], XmNvisualPolicy,       XmVARIABLE); n++;
    XtSetArg(args[n], XmNscrollingPolicy,    XmAUTOMATIC); n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED); n++;
    
    ps_info = XmCreateScrolledText(process_form, "PsInfo", args, n);
    XmTextSetString(ps_info, "");
  }
    
#ifdef HAVE_KILL
  signal_list = XmCreateScrolledList( form, "signal_list", NULL, 0 );

  XtManageChild(signal_list);
  XtVaSetValues(signal_list, 
		 XmNitems, xmstr,
		 XmNitemCount, sigmap_elts,
		 XmNvisibleItemCount, 6, 
		 XmNselectionPolicy,   XmSINGLE_SELECT, 
		 XmNtopAttachment,     XmATTACH_WIDGET,
		 XmNtopWidget,         ps_info, 
		 XmNtopAttachment,     XmATTACH_NONE,
		 XmNleftAttachment,    XmATTACH_FORM,
		 XmNrightAttachment,   XmATTACH_NONE,
		 XmNbottomAttachment,  XmATTACH_FORM,
		 NULL );

  for  ( i = 0; i < sigmap_elts; i++ ) {
    XmStringFree ( xmstr[i] );
  }
  XtFree((char *) xmstr);
  
  XtAddCallback( signal_list, XmNdefaultActionCallback, 
		 SignalListCallback, &dialog_signal_pos);
  XtAddCallback( signal_list, XmNsingleSelectionCallback, 
		 SignalListCallback, &dialog_signal_pos);

#endif
  
  /* Priority slider */
  priority_slider = 
    XtVaCreateManagedWidget ("priority", xmScaleWidgetClass, form,
			     XtVaTypedArg, XmNtitleString, 
			     XmRString, "priority",
			     10,
			     XmNmaximum, PRIO_MAX,
			     XmNminimum, PRIO_MIN,
			     XmNvalue, 10,  /* Will be reset later */
			     XmNshowValue,         True,
			     XmNorientation,       XmVERTICAL,
			     XmNtopAttachment,     XmATTACH_FORM,
			     XmNbottomAttachment,  XmATTACH_FORM,
			     XmNleftAttachment,    XmATTACH_WIDGET,

   /* We use XtParent() below because of some (all?) Lesstif 
      complaints. XmCreateScrolledList seems to creates
      a scrolled widgets parent and a list widget child and returns
      the list widget. */
#ifdef HAVE_KILL
			     XmNleftWidget,        XtParent(signal_list),
#endif

			     XmNrightAttachment,   XmATTACH_NONE,
			     XmNbottomAttachment,  XmATTACH_NONE,
			     NULL);

  XtAddCallback(priority_slider, XmNdragCallback, 
		set_priority_callback,  NULL);
  XtAddCallback(priority_slider, XmNvalueChangedCallback, 
		set_priority_callback,  NULL);

  /* Process group identifier */
  process_group_button = 
      XtVaCreateManagedWidget ("process group", xmToggleButtonWidgetClass, 
			       form, 
			       XmNtopAttachment,     XmATTACH_FORM,
			       XmNleftAttachment,    XmATTACH_WIDGET,
			       XmNleftWidget,        priority_slider,
			       XmNrightAttachment,   XmATTACH_NONE,
			       XmNbottomAttachment,  XmATTACH_NONE,
			       NULL);
  XtAddCallback(process_group_button, XmNvalueChangedCallback, 
		toggle_button_callback, &do_process_group);

#ifdef HAVE_SETPRIORITY
  priority_button = 
      XtVaCreateManagedWidget ("set priority", xmToggleButtonWidgetClass, 
			       form, 
			       XmNtopAttachment,     XmATTACH_WIDGET,
			       XmNtopWidget,         process_group_button,
			       XmNleftAttachment,    XmATTACH_WIDGET,
			       XmNleftWidget,        priority_slider,
			       XmNrightAttachment,   XmATTACH_NONE,
			       XmNbottomAttachment,  XmATTACH_FORM,
			       NULL);
  XtAddCallback(priority_button, XmNvalueChangedCallback, 
		toggle_button_callback, &do_priority);
#endif
    
  return(process_dialog);
  
}

#ifdef HAVE_KILL
static void
set_kill_signal_callback( Widget button, XtPointer client_data,
			XtPointer call_data)
{
   int *pos = (int *) client_data;
   last_global_signal_pos = *pos;
}

/* Routine to set the default signal used by "Quick Kill", or
   Point and Kill. 
*/ 
void
popup_default_kill_dialog( Widget button, XtPointer client_data,
			   XtPointer call_data)
{
  Widget dialog_box, signal_list;
  int i;
  XmStringTable xmstr;
  
#include "help/kill_signal.h"

  /* Create the user name dialog */
  dialog_box = NewDialog(button, "set_kill_signal", "Set Kill Signal", 
			 True,  help_callback           , kill_signal_help,
			 True,  destroy_widget_callback , NULL,
			 True,  set_kill_signal_callback, &dialog_signal_pos);


  dialog_signal_pos = last_global_signal_pos;
  xmstr = ( XmString * ) XtMalloc (sizeof ( XmString ) * sigmap_elts ) ;

  /* Create list skipping over the 0th item -- no action. */
  for  ( i = 1; i < sigmap_elts; i++ ) {
    xmstr[i-1] = XmStringCreateLtoR ( signal_map[i].signal_name, 
				      XmFONTLIST_DEFAULT_TAG );
  }

  signal_list = XmCreateScrolledList( dialog_box, "signal_list", NULL, 0 );
  XtManageChild(signal_list);
  XtVaSetValues(signal_list, 
		 XmNitems, xmstr,
		 XmNitemCount, sigmap_elts-1,
		 XmNvisibleItemCount, 6, 
		 XmNselectionPolicy,   XmSINGLE_SELECT, 
		 NULL );

  for  ( i = 0; i < sigmap_elts-1; i++ ) {
    XmStringFree ( xmstr[i] );
  }
  XtFree((char *) xmstr);

  XtAddCallback( signal_list, XmNdefaultActionCallback, 
		 set_kill_signal_callback, &dialog_signal_pos);
  XtAddCallback( signal_list, XmNsingleSelectionCallback, 
		 SignalListCallback, &dialog_signal_pos);


  XmListSelectPos(signal_list, last_global_signal_pos+1, False);

  XtManageChild(dialog_box);
}
#endif /*HAVE_KILL*/

