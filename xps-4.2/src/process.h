/* Motif process menu and menu handling  

  $Id: process.h,v 1.1 2002/12/01 22:00:56 rockyb Exp $
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

/***************************************************************************
  Creates the process message dialog and callbacks.  Although many
  static items like signal names are set up, values for various fields
  like the priority and and process information are reassigned only
  after a particular process is selected.
***************************************************************************/
extern Widget create_process_menu(Widget parent, char *name);

/***************************************************************************
  This routine is called to pop up the process dialog/form.
  This is done only after obtaining information about the process
  and setting various fields.
  
  The form can be used to shows process information, set process
  priority and/or send a signal to the process or process group.

  If want_kill is True and base.quick_kill is True, this routine instead
  sends the signal last selected to the process.
***************************************************************************/
extern void Handle_Process(Widget w, pid_t pid, XEvent *event, 
			   Boolean want_kill);

/* Routine to set the default signal used by "Quick Kill", or
   Point and Kill. 
*/ 
extern void popup_default_kill_dialog(Widget button, XtPointer client_data,
				      XtPointer call_data);
