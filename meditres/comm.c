/*
 * $Header: /cvsroot-fuse/meditres/meditres/comm.c,v 1.11 2002/01/15 12:15:14 amai Exp $
 *
 * $XConsortium: comm.c,v 1.23 94/04/17 20:38:51 dave Exp $
 *
 * Copyright (c) 1990  X Consortium
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the X Consortium shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization
 * from the X Consortium.
 *
 */

/*
 * Modifications by Volker Gering, 1997
 *
 * Motif port, MEDITRES project, © 2001 by Danny Backx
 */


/*
 * This file contains the code to communicate with the client that is
 * being edited.
 */

#include "config.h"

#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>	/* Get standard string definitions. */
#include <X11/Xatom.h>
#include <X11/cursorfont.h>	/* For crosshair cursor. */
#include <X11/Xproto.h>
#include <X11/Xos.h>		/* for XtNewString */
#include <X11/keysym.h>         /* defines XK_... */
#include <X11/Xmu/Error.h>
#include <X11/Xmu/WinUtil.h>

#include <Xm/TextF.h>

#include "meditres.h"


/*
 * static Globals.
 */

static Atom atom_comm;
static Atom atom_command;
static Atom atom_resource_editor;
static Atom atom_client_value;
static Atom atom_editres_protocol;

static ResIdent GetNewIdent(void);
static void TellUserAboutMessage(Widget label, ResCommand command);
static void BuildHeader(CurrentClient *client_data);
static void FreeEvent(Event *event);
static Event *BuildEvent(ProtocolStream *stream);
static char *DispatchEvent(Event *event);
static void GetClientValue(Widget w, XtPointer data, Atom *selection,
            Atom *type, XtPointer value, unsigned long *length,  int *format);
static void ClientTimedOut(XtPointer data, XtIntervalId *id);
static void LoseSelection(Widget w, Atom *sel);
static void SelectionDone(Widget w, Atom *sel, Atom *targ);
static Boolean ConvertCommand(Widget w, Atom *selection, Atom *target, Atom *type_ret,
               XtPointer *value_ret, unsigned long *length_ret, int *format_ret);

/* >>>>> HACK for batchres */
static void brLoseSelection(Widget, Atom *);
static void brGetClientValue(Widget w, XtPointer data, Atom *selection, Atom *type,
                 XtPointer value, unsigned long *length, int *format);
/* <<<<< HACK for batchres */


/* extern Widget CM_entries[NUM_CM_ENTRIES]; */
/* extern Widget TM_entries[NUM_TM_ENTRIES]; */


/* Function Name:
 *   ClientTimedOut (XtTimerCallbackProc)
 *
 * Description:
 *   Called if the client takes too long to take our selection.
 *
 * Arguments:
 *   data - The widget that owns the client communication selection.
 *   id - *** NOT USED ***
 *
 * Returns:
 *   <none>
 *
 * Calls:
 *   SetMessage - utils.c
 *
 * Global Data:
 *   global_client
 */
/* ARGSUSED */
static void
ClientTimedOut(XtPointer data, XtIntervalId *id)
{
  Widget w = (Widget) data;


  global_client.ident = NO_IDENT;
  XtDisownSelection(w,
		    global_client.atom, 
		    XtLastTimestampProcessed(XtDisplay(w)));

  /* "It appears that this client does not understand */
  /* the Editres Protocol" */
  SetMessage(global_screen_data.info_label,
	     res_labels[4],
	     "It appears that this client does not understand the Editres Protocol");

}   /* ClientTimedOut() */

/* Function Name:
 *   GetClientWindow
 *
 * Description:
 *   Gets the Client's window by asking the user.
 *
 * Arguments:
 *   w - a widget.
 *
 * Returns:
 *   a clients window, or None.
 *
 * Calls:
 *   SetMessage - utils.c
 *
 * Global Data:
 */
Window 
GetClientWindow(Widget w, int *x, int *y)
{
  int          status;
  Cursor       cursor;
  XEvent       event;
  int          buttons = 0;
  int          keys = 0;
  Display      *dpy = XtDisplayOfObject(w);
  Window       target_win = None;
  Window       root_win = RootWindowOfScreen(XtScreenOfObject(w));
  XtAppContext context = XtWidgetToApplicationContext(w);
  char         buffer[10];
  int          len;
  KeySym       keycode;


  /* >>>>> HACK for batchres */
  if (global_winID != None)
    {
      /* we submit the winID given by parameter only once! */
      /* otherwise there is an infinite loop */
      target_win = global_winID;
      global_winID = None;
      return(target_win);
    }
  /* <<<<< HACK for batchres */

  /* Make the target cursor */
  cursor = XCreateFontCursor(dpy,
			     XC_crosshair);
  
  /* Grab the pointer using target cursor, letting it room all over */
  status = XGrabPointer(dpy,             /* Display */
			root_win,            /* grab_window */
			False,           /* owner_events */
			ButtonPressMask	| ButtonReleaseMask,
			GrabModeSync,    /* pointer_mode */
			GrabModeAsync,   /* keyboard_mode */
			root_win,            /* confine_to */
			cursor,          /* cursor */
			CurrentTime);    /* time, when grab took place */
  if (status != GrabSuccess)
    {
      /* "Can't grab the mouse" */
      SetMessage(global_screen_data.info_label,
		 res_labels[5],
		 "Can't grab the mouse");
      return(None);
    }   /* if (grab of mouse pointer unsuccessful) */

  /* Grab the keyboard, letting it room all over */
  status = XGrabKeyboard(dpy,             /* Display */
			 root_win,            /* grab_window */
			 False,           /* owner_events */
			 GrabModeAsync,   /* pointer_mode */
			 GrabModeAsync,   /* keyboard_mode */
			 CurrentTime);    /* time, when grab took place */
  if (status != GrabSuccess)
    {
      /* "Can't grab the keyboard" */
      SetMessage(global_screen_data.info_label,
		 res_labels[37],
		 "Can't grab the keyboard");
      return(None);
    }   /* if (grab of keyboard unsuccessful) */

  /* Let the user select a window... */
  while ( (target_win == None) || (buttons != 0) )
    {
/* printf("targetwin: 0x%x, buttons: %d\n", target_win, buttons); */
      /* allow one more event */
      XAllowEvents(dpy,
		   SyncPointer,
		   CurrentTime);
      XtAppNextEvent(context,
		     &event);

      switch (event.type)
	{
	case ButtonPress:
/* printf("ButtonWindow 0x%x\n", event.xbutton.window); */
	  if (event.xbutton.window != root_win)
	    {
	      XtDispatchEvent(&event);
	      break;
	    }
      
	  if (target_win == None)
	    {
	      target_win = event.xbutton.subwindow; /* window selected */
	      if (x != NULL)
		{
		  *x = event.xbutton.x_root;
		}
	      if (y != NULL)
		{
		  *y = event.xbutton.y_root;
		}
	    }

	  buttons++;
	  break;   /* ButtonPress */

	case ButtonRelease:
	  if (event.xbutton.window != root_win)
	    {
	      XtDispatchEvent(&event);
	      break;
	    }
      
	  if (buttons > 0) /* There may have been some */
	    {              /* down before we started */
	      buttons--;
	    }

	  break;   /* ButtonRelease */

	case KeyPress:
	  /* <Key>ESC aborts, <Key>Return + <Key>Spacebar select */
	  len = (XLookupString(&(event.xkey),
			       buffer, sizeof(buffer),
			       &keycode,
			       (XComposeStatus*)NULL));
/* printf("keycode: 0x%x\n", keycode); */
	  switch (keycode)
	    {
	    case XK_Return:
	    case XK_space:
	    case XK_Select:
	    case XK_KP_Space:
	    case XK_KP_Enter:
/* printf("Ret, space, Select, ...\n"); */
/* printf("KeyWindow 0x%x\n", event.xkey.window); */
/* 	      if (event.xkey.window != root_win) */
/* 		{ */
/* 		  XtDispatchEvent(&event); */
/* 		  break; */
/* 		} */
/* printf(" window root \n"); */
	      if (target_win == None)
		{
/* printf(" subwindow 0x%x\n", event.xkey.subwindow); */
		  target_win = event.xkey.subwindow; /* window selected */
		  if (x != NULL)
		    {
		      *x = event.xkey.x_root;
		    }
		  if (y != NULL)
		    {
		      *y = event.xkey.y_root;
		    }
		}

	      break;   /* XK_Return  */

	    case XK_Escape:
	    case XK_Cancel:
	    case XK_Break:
	      XUngrabPointer(dpy,
			     CurrentTime);      /* Done with pointer */
	      XUngrabKeyboard(dpy,
			      CurrentTime);      /* Done with keyboard */

	      return ((Window)None);
	      break;   /* XK_Escape */

	    default:
	      fprintf(stderr,
		      "Unknown key! Press <ESC> to abort, <Return> to select\n");
	      break;

	    }   /* switch (keycode) */
	  break;   /* KeyPress */

	default:
	  XtDispatchEvent(&event);
	  break;   /* default */

	}   /* switch (event.type) */

    }   /* while () */

  /* If the XServer supports KeyRelease, we throw it away */
  while ((event.type == KeyRelease) || (event.type == KeyPress))
    {
      XtDispatchEvent(&event);
    }   /* while (discard Keyboard-Events) */

  XUngrabPointer(dpy,
		 CurrentTime);      /* Done with pointer */
  XUngrabKeyboard(dpy,
		 CurrentTime);      /* Done with keyboard */

/* printf("[GetClientWindow] target_win 0x%x, XmuClientWindow 0x%x\n", */
/*        target_win, */
/*        XmuClientWindow(dpy, target_win)); */
  return (XmuClientWindow(dpy,
			  target_win));

}   /* GetClientWindow() */

/* Function Name:
 *   SetCommand
 *
 * Description:
 *   Causes this widget to own the resource editor's command selection.
 *
 * Arguments:
 *   w - the widget that will own the selection.
 *   command - command to send to client.
 *   msg - message to prompt the user to select a client.
 *
 * Returns:
 *   <none>
 *
 * Calls:
 *   SetMessage - utils.c
 *   GetClientWindow - (self)
 *   GetNewIdent -
 *   BuildHeader -
 *   SetCommand -
 *   TellUserAboutMessage - (self)
 *   startWait - utils.c
 *
 * Global Data:
 */
/* ARGSUSED */
void
SetCommand(Widget w, ResCommand command, char *msg)
{
  XClientMessageEvent client_event;
  Display             *dpy = XtDisplay(w);

  if (msg == NULL)
    {
      /* "Click the mouse pointer on any toolkit client" */
      msg = res_labels[6];
    }
  
  SetMessage(global_screen_data.info_label,
	     msg,
	     "Click the mouse pointer on any toolkit client");
  
  if (global_client.window == None) {
      if ( (global_client.window = GetClientWindow(w, NULL, NULL)) == None ) {
	  return;
      }
  }

/* printf("[SetCommand] global_client.window: %d 0x%x\n", */
/*        global_client.window, */
/*        global_client.window); */
       
  global_client.ident = GetNewIdent();

  global_client.command = command;
  global_client.atom = atom_comm;

  BuildHeader(&(global_client)); 

  if (!XtOwnSelection(w,
		      global_client.atom,
		      CurrentTime,
		      ConvertCommand,
		      LoseSelection,
		      SelectionDone))
    {
      /* "Unable to own the Resource/Attribute Selection" */
      SetMessage(global_screen_data.info_label,
		 res_labels[7],
		 "Unable to own the Resource/Attribute Selection");
    }

  client_event.window = global_client.window;
  client_event.type = ClientMessage;
  client_event.message_type = atom_resource_editor;
  client_event.format = EDITRES_SEND_EVENT_FORMAT;
  client_event.data.l[0] = XtLastTimestampProcessed(dpy);
  client_event.data.l[1] = global_client.atom;
  client_event.data.l[2] = (long) global_client.ident;
  client_event.data.l[3] = global_effective_protocol_version;

  global_error_code = NO_ERROR;                 /* Reset Error code. */
  global_old_error_handler = XSetErrorHandler(HandleXErrors);
  global_serial_num = NextRequest(dpy);

  XSendEvent(dpy,
	     global_client.window,
	     FALSE,
	     (long)0, 
	     (XEvent*)&client_event);

  XSync(dpy, FALSE);
  XSetErrorHandler(global_old_error_handler);

  if (global_error_code == NO_WINDOW) {
      char error_buf[BUFSIZ];


      global_error_code = NO_ERROR;	/* Reset Error code. */
      sprintf(error_buf,
	      "The communication window with%s%s.",
	      " application is no longer available\n",
	      "Please select a new widget tree");
    
      global_client.window = None;
      SetCommand(w, LocalSendWidgetTree, error_buf);

      return;
    }   
  
  TellUserAboutMessage(global_screen_data.info_label, command);
  global_client.timeout = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
					  CLIENT_TIME_OUT, 
					  ClientTimedOut,
					  (XtPointer)w);
  startWait(XtWidgetToApplicationContext(w),
	    CLIENT_TIME_OUT / 1000);

}   /* SetCommand() */

/* Function Name:
 *   TellUserAboutMessage
 *
 * Description:
 *   Informs the user that we have sent a message to the client
 *
 * Arguments:
 *   label - the info label.
 *   command - command that we have executed.
 *
 * Returns:
 *   <none>
 *
 * Calls:
 * Global Data:
 */
static void
TellUserAboutMessage(Widget label, ResCommand command)
{
	char msg[BUFSIZ];
	char *str;

  /* >>>>> HACK for batchres */
	if (label == NULL) {
		return;
	}
  /* <<<<< HACK for batchres */

	switch(command) {
	case LocalSendWidgetTree:
		str = " asking for widget tree";
		break;   /* LocalSendWidgetTree */
	case LocalSetValues:
		str = " asking it to perform SetValues()";
		break;   /* LocalSetValues */
	case LocalFlashWidget:
	case LocalGetGeometry:
		str = " asking it to perform GetGeometry()";
		break;   /* LocalFlashWidget or LocalGetGeometry */
	case LocalGetResources:
		str = " asking it to get a widget's resource list";
		break;   /* LocalGetResources */
	case LocalFindChild:
		str = " asking it to find the child Widget.";
		break;   /* LocalFindChild */
	default:
		str = "";
		break;   /* default */
	}   /* switch(command) */

	/* "Message sent to client %s" */
	if (res_labels[8]) {
		sprintf(msg, res_labels[8], str);
	} else {
		sprintf(msg, "Message sent to client %s", str);
	}
	SetMessage(label, msg, "?res_labels[8]");
}   /* TellUserAboutMessage() */

/* Function Name:
 *   ConvertCommand
 *	Description: Converts the command string into a selection that can
 *                   be sent to the client.
 *	Arguments: (see Xt)
 *	Returns: TRUE if we could convert the selection and target asked for.
 */
/* ARGSUSED */
static Boolean
ConvertCommand(Widget w, Atom *selection, Atom *target, Atom *type_ret,
               XtPointer *value_ret, unsigned long *length_ret, int *format_ret)
{
  if ( (*selection != atom_comm) || (*target != atom_command) )
    {
      return(FALSE);
    }

  *type_ret = atom_editres_protocol;
  *value_ret = (XtPointer)global_client.stream.real_top;
  *length_ret = global_client.stream.size + HEADER_SIZE;
  *format_ret = EDITRES_FORMAT;
    
  return(TRUE);

}   /* ConvertCommand() */


/*	Function Name: SelectionDone
 *	Description: done with the selection.
 *	Arguments: *** NOT USED ***
 *	Returns: none.
 */
/* ARGSUSED */
static void
SelectionDone(Widget w, Atom *sel, Atom *targ)
{
  /* Keep the toolkit from automaticaly freeing the selection value */
}   /* SelectionDone() */


/*	Function Name: LoseSelection
 *	Description: Called when we have lost the selection, asks client
 *                   for the selection value.
 *	Arguments: w - the widget that just lost the selection.
 *                 sel - the selection.
 *	Returns: none.
 */
static void
LoseSelection(Widget w, Atom *sel)
{
  if (global_client.timeout != 0)
    {
      XtRemoveTimeOut(global_client.timeout);
      global_client.timeout = 0;

      /* reset the countdown */
      global_countdown = 0;
      if (global_timeout)
	{
	  XtRemoveTimeOut(global_timeout);
	  global_timeout = 0;
	  SetWaitLabel(0);
	}   /* if (old timeout running) */
    }

  XtGetSelectionValue(w,
		      *sel,
		      atom_client_value,
		      GetClientValue,
		      NULL,
		      XtLastTimestampProcessed(XtDisplay(w)));
}   /* LoseSelection() */

/*	Function Name: GetClientValue
 *	Description: Gets the value out of the client, and does good things
 *                   to it.
 *	Arguments: w - the widget that asked for the selection.
 *                 data - client_data *** NOT USED ***.
 *                 sel - the selection.
 *                 type - the type of the selection.
 *                 value - the selection's value.
 *                 length - the length of the selection's value.
 *                 format - the format of the selection.
 *	Returns: none.
 */

static Boolean  reset_protocol_level = True;

/* ARGSUSED */
static void
GetClientValue(Widget w, XtPointer data, Atom *selection, Atom *type,
               XtPointer value, unsigned long *length,  int *format)
{
  Event          *event;
  ProtocolStream alloc_stream;
  ProtocolStream *stream;
  unsigned char  ident;
  unsigned char  error_code;
  char           *error_str;
  char           msg[BUFSIZ];


/* type should be checked if XT_CONVERT_FAIL */

  if (*length == 0)
    {
      return;
    }

  stream = &alloc_stream;	/* easier to think of it this way... */

  stream->current = stream->top = (unsigned char*)value;
  stream->size = HEADER_SIZE;		/* size of header. */

  /*
   * Retrieve the Header.
   */

  if (*length < HEADER_SIZE)
    {
      /* "Incorrectly formatted message from client" */
      SetMessage(global_screen_data.info_label,
		 res_labels[9],
		 "Incorrectly formatted message from client");
      return;
    }

  (void)_XEditResGet8(stream, &ident);
  if (global_client.ident != ident)
    {

#ifdef DEBUG
      if (global_resources.debug)
	{
	  printf("Incorrect ident from client.\n");
	}
#endif  /* DEBUG */

      if (!XtOwnSelection(w,
			  *selection,
			  CurrentTime,
			  ConvertCommand, 
			  LoseSelection,
			  SelectionDone))
	{
	  /* "Unable to own the Resource Editor Command Selection" */
	  SetMessage(global_screen_data.info_label,
		     res_labels[10],
		     "Unable to own the Resource Editor Command Selection");
	}

	return;

    }   /* if (global_client.ident != ident) */

    (void)_XEditResGet8(stream, &error_code);
    (void)_XEditResGet32(stream, &(stream->size));
    stream->top = stream->current;   /* reset stream to top of value.*/

    switch ((int)error_code)
      {
      case PartialSuccess:
        /*****
        if (global_client.command == LocalSendWidgetTree &&
	    global_effective_protocol_version < CURRENT_PROTOCOL_VERSION)
          {
	    ++global_effective_protocol_version;
          }
	*****/
	if ((event = BuildEvent(stream)) != NULL)
	  {
	    error_str = DispatchEvent(event);
	    FreeEvent(event);
	  }
	else
	  {
	    sprintf(msg,
		    "Unable to unpack protocol request.");
	    error_str = XtNewString(msg);
	  }
	break;   /* PartialSuccess */

      case Failure:
	error_str = GetFailureMessage(stream);
	break;   /* Failure */

      case ProtocolMismatch:
	error_str = ProtocolFailure(stream);
	--global_effective_protocol_version;
	/* normaly protocol version is reset to current during a SendWidgetTree
         * request, however, after a protocol failure this is skiped once for
         * a retry.
         */
	reset_protocol_level = False;
	SetCommand(w,
		   LocalSendWidgetTree,
		   NULL);
	break;   /* ProtocolMismatch */

	default:
		/* "Unknown Error code %s" */
		if (res_labels[11]) {
			sprintf(msg, res_labels[11], (int)error_code);
		} else {
			sprintf(msg, "Unknown Error code %i", (int)error_code);
		}
		SetMessage(global_screen_data.info_label, msg, "?res_labels[11]");
		break;    /* default */
	}   /* switch (error_code) */

    if (error_str == NULL)
      {
	WNode *top;


	if (global_tree_info == NULL)
	  {
	    return;
	  }
	
	top = global_tree_info->top_node;
	/* "Widget/Style tree for client %s (%s)" */
	if (res_labels[12])
	  {
	    sprintf(msg,
		    res_labels[12],
		    top->name,
		    top->class);
	  }
	else
	  {
	    sprintf(msg,
		    "Widget/Style tree for client %s (%s)",
		    top->name,
		    top->class);
	  }
	SetMessage(global_screen_data.info_label,
		   msg,
		   "?res_labels[12]");

	return;
      }   /* if (error_str == NULL) */

    SetMessage(global_screen_data.info_label,
	       error_str,
	       "?error_str");
    XtFree(error_str);

}   /* GetClientValue() */

/*	Function Name: BuildHeader
 *	Description: Puts the header into the message.
 *	Arguments: client_data - the client data.
 *	Returns: none.
 */
static void
BuildHeader(CurrentClient *client_data)
{
  unsigned long  old_alloc;
  unsigned long  old_size;
  unsigned char  *old_current;
  EditresCommand command;
  ProtocolStream *stream = &(client_data->stream);


  /*
   * We have cleverly keep enough space at the top of the header
   * for the return protocol stream, so all we have to do is
   * fill in the space.
   */

  /* 
   * Fool the insert routines into putting the header in the right
   * place while being damn sure not to realloc (that would be very bad.
   */

  old_current = stream->current;
  old_alloc = stream->alloc;
  old_size = stream->size;

  stream->current = stream->real_top;
  stream->alloc = stream->size + (2 * HEADER_SIZE);	

  _XEditResPut8(stream, client_data->ident);

  switch(client_data->command)
    {
    case LocalSendWidgetTree: 
      if (reset_protocol_level)
	{
	  global_effective_protocol_version = CURRENT_PROTOCOL_VERSION;
	}
      reset_protocol_level = True;
      command = SendWidgetTree;
      break;   /* LocalSendWidgetTree */

    case LocalSetValues:
      command = SetValues;
      break;   /* LocalSetValues */

    case LocalFlashWidget:
      command = GetGeometry;
      break;   /* LocalFlashWidget */

    case LocalGetResources:
      command = GetResources;
      break;   /* LocalGetResource */

    case LocalFindChild:
      command = FindChild;
      break;   /* LocalFindChild */

    case LocalGetValues:
#ifdef XlibSpecificationRelease

#if XlibSpecificationRelease >= 6 

      command = GetValues;

#else
      printf("GetValues is supported only if libXmu is patched!\n");
      printf("GetValues perhaps supported from X11R6 and higher!\n");
      printf("  This is X11R%d", XlibSpecificationRelease);
#endif   /* XlibSpecificationRelease >= X11R6 */

#else
      /* editres is not available in X11R4 */
#endif   /* XlibSpecificationRelease */
      break;   /* LocalGetValues */

    default:
      command = SendWidgetTree;
      break;   /* default */
    }   /* switch (command) */
				  
  _XEditResPut8(stream, (unsigned char)command);
  _XEditResPut32(stream, old_size);

  stream->alloc = old_alloc;
  stream->current = old_current;
  stream->size = old_size;

}   /* BuildHeader() */

/*	Function Name: BuildEvent
 *	Description: Builds the event structure from the 
 *	Arguments: stream - the protocol data stream.
 *	Returns: event - the event.
 */
static Event
*BuildEvent(ProtocolStream *stream)
{
  int   i;
  Event *event = (Event*)XtCalloc(sizeof(Event), 1);


  /*
   * The return value will be different depending upon the
   * request sent out.
   */

  switch(global_client.command)
    {
    case LocalSendWidgetTree:
      {
	SendWidgetTreeEvent *send_event = (SendWidgetTreeEvent*)event;


	send_event->type = SendWidgetTree;

	if ( !_XEditResGet16(stream, &(send_event->num_entries)) )
	  {
	    goto done;
	  }

	send_event->info = (WidgetTreeInfo*)XtCalloc(sizeof(WidgetTreeInfo),
						     send_event->num_entries);

	for (i = 0; i < (int)send_event->num_entries; i++)
	  {
	    WidgetTreeInfo *info = send_event->info + i;


	    if ( !(_XEditResGetWidgetInfo(stream, &(info->widgets)) &&
		   _XEditResGetString8(stream, &(info->name)) &&
		   _XEditResGetString8(stream, &(info->class)) &&
		   _XEditResGet32(stream, &(info->window)) ) ) 
	      {
		goto done;
	      }
	  }   /* for */

	if (global_effective_protocol_version == CURRENT_PROTOCOL_VERSION)
	  {			/* get toolkit type and reset if necessary */
	    if ( !_XEditResGetString8(stream, &(send_event->toolkit)) )
	      {
 	        goto done;
	      }
	  }   /* if (effective_protocol_version == CURRENT) */

	/* set the command menu entries senitive */
/* 	SetEntriesSensitive(&CM_entries[CM_OFFSET], CM_NUM, True); */
	/* set the tree menu entries senitive */
/* 	SetEntriesSensitive(TM_entries, TM_NUM, True); */

	if (global_effective_protocol_version == CURRENT_PROTOCOL_VERSION)
	  {
	    if (strcmp(send_event->toolkit, "InterViews") == 0) {
	        RebuildMenusAndLabel("iv");
	      }
	  }   /* if (effective_protocol_version == CURRENT) */

	else
	  {
	    RebuildMenusAndLabel("xt");
	  }

	break;
      }   /* case LocalSendWidgetTree: */
    /*---------------------*/

    case LocalSetValues:
      {
	SetValuesEvent *sv_event = (SetValuesEvent*)event;


	sv_event->type = SetValues;

	if (!_XEditResGet16(stream, &(sv_event->num_entries)))
	  {
	    goto done;
	  }
	    
	sv_event->info = (SetValuesInfo *) XtCalloc(sizeof(SetValuesInfo),
						    sv_event->num_entries);

	for (i = 0; i < (int)sv_event->num_entries; i++)
	  {
	    SetValuesInfo * info = sv_event->info + i;
	    if (!(_XEditResGetWidgetInfo(stream, &(info->widgets)) &&
		  _XEditResGetString8(stream, &(info->message))))
	      {
		goto done;
	      }
	  }   /* for */

	break;
      }   /* case LocalSetValues: */
    /*---------------------*/

    case LocalGetResources:
      {
	GetResourcesEvent * res_event = (GetResourcesEvent *) event;


	res_event->type = GetGeometry;

	if (!_XEditResGet16(stream, &(res_event->num_entries)))
	  {
	    goto done;
	  }

	res_event->info = (GetResourcesInfo*)XtCalloc(sizeof(GetResourcesInfo),
						      res_event->num_entries);

	for (i = 0; i < (int)res_event->num_entries; i++)
	  {
	    GetResourcesInfo *res_info = res_event->info + i;


	    if (!(_XEditResGetWidgetInfo(stream, &(res_info->widgets)) &&
		  _XEditResGetBoolean(stream, &(res_info->error))))
	      {
		goto done;
	      }

	    if (res_info->error)
	      {
		if (!_XEditResGetString8(stream, &(res_info->message))) 
		  {
		    goto done;
		  }
	      }   /* if (res_info->error) */

	    else
	      {
		unsigned int j;


		if (!_XEditResGet16(stream, &(res_info->num_resources)))
		  {
		    goto done;
		  }

		res_info->res_info
		  = (ResourceInfo*)XtCalloc(sizeof(ResourceInfo),
					    res_info->num_resources);

		for (j = 0; j < res_info->num_resources; j++)
		  {
		    unsigned char temp;
		    ResourceInfo *info = res_info->res_info + j;


		    if (!(_XEditResGetResType(stream, &(temp)) &&
			  _XEditResGetString8(stream, &(info->name)) &&
			  _XEditResGetString8(stream, &(info->class)) &&
			  _XEditResGetString8(stream, &(info->type))))
		      {
			goto done;
		      }

		    else
		      {
			info->res_type = (ResourceType) temp;
		      }

		  } /* for j */

	      } /* else (NOT res_info->error) */

	  } /* for i */

	break;
      }   /* case LocalGetResources: */
    /*---------------------*/

    case LocalFlashWidget:
    case LocalGetGeometry:
      {
	GetGeomEvent *geom_event = (GetGeomEvent*)event;


	geom_event->type = GetGeometry;

	if (!_XEditResGet16(stream, &(geom_event->num_entries)))
	  {
	    goto done;
	  }
	    
	geom_event->info = (GetGeomInfo *) XtCalloc(sizeof(GetGeomInfo),
						    geom_event->num_entries);

	for (i = 0; i < (int)geom_event->num_entries; i++)
	  {
	    GetGeomInfo *info = geom_event->info + i;


	    if (!(_XEditResGetWidgetInfo(stream, &(info->widgets)) &&
		  _XEditResGetBoolean(stream, &(info->error))))
	      {
		goto done;
	      }

	    if (info->error)
	      {
		if (!_XEditResGetString8(stream, &(info->message)))
		  {
		    goto done;
		  }
	      }   /* if (info->error) */

	    else
	      {
		if (!(_XEditResGetBoolean(stream, &(info->visable)) &&
		      _XEditResGetSigned16(stream, &(info->x)) &&
		      _XEditResGetSigned16(stream, &(info->y)) &&
		      _XEditResGet16(stream, &(info->width)) &&
		      _XEditResGet16(stream, &(info->height)) &&
		      _XEditResGet16(stream, &(info->border_width))))
		  {
		    goto done;
		  }

	      }   /* else (NOT info->error) */

	  }   /* for i */

	break;
      }   /* case LocalFlashWidget:, LocalGetGeometry: */
    /*---------------------*/

    case LocalFindChild:
      {
	FindChildEvent *find_event = (FindChildEvent*)event;


	find_event->type = FindChild;

	if (!_XEditResGetWidgetInfo(stream, &(find_event->widgets)))
	  {
	    goto done;
	  }

	break;
      }   /* case LocalFindChild: */
    /*---------------------*/

    case LocalGetValues: /* This is for REPLY... */
      {
#ifdef XlibSpecificationRelease
#if XlibSpecificationRelease >= 6 
	Arg args[1];
	GetValuesEvent *gv_event = (GetValuesEvent*)event;

	gv_event->type = GetValues;
	if (!_XEditResGet16(stream, &(gv_event->num_entries))) {
	    goto done;
	}

	gv_event->info = (GetValuesInfo*)XtCalloc(sizeof(GetValuesInfo),1);
	{
	  GetValuesInfo *info = gv_event->info;


	  if (!(_XEditResGetString8(stream, &(info->value))))
	    {
	      goto done;
	    }

	  /* set the string value of the asciitext widget. Note that only
	   * one active node is dealt with here.  This is ok because only
	   * one node can be active when the resource box is up.
	   */

	  XtSetArg (args[0], XtNstring, info->value);
	 
/* FIX ME need to fix this for more than one resource box to work */
/* FIX ME */	XmTextFieldSetString(global_tree_info->active_nodes[0]->resources->res_box->value_wid, info->value);

#if 0
#if 0
	fprintf(stderr, "MEDITRES: Got value %s\n", info->value);
#else
	XtSetValues
	    (global_tree_info->active_nodes[0]->resources->res_box->value_wid,
	     args, 1);
#endif
#endif
	  }

#else
      printf("GetValues is supported only if libXmu is patched!\n");
      printf("GetValues perhaps supported from X11R6 and higher!\n");
      printf("  This is X11R%d", XlibSpecificationRelease);
#endif   /* XlibSpecificationRelease >= X11R6 */

#else
      /* editres is not available in X11R4 */
#endif   /* XlibSpecificationRelease */

	break;
      }   /* case LocalGetValues: */
    /*---------------------*/

    default:
      {
	goto done;
      }   /* default: */
    /*---------------------*/

    }   /* switch (global_client.command) */

  return(event);


done:
  FreeEvent(event);
  return(NULL);
}   /* BuildEvent() */

/*	Function Name: FreeEvent
 *	Description: Frees all memory associated with the event. 
 *	Arguments: event - the event.
 *	Returns: none.
 *
 * NOTE: XtFree() returns w/o freeing if ptr is NULL.
 */
static void
FreeEvent(Event *event)
{
  unsigned int i;


  switch(event->any_event.type)
    {
    case SendWidgetTree:
      {
	SendWidgetTreeEvent *send_event = (SendWidgetTreeEvent*)event;
	WidgetTreeInfo      *info = send_event->info;


	if (info != NULL)
	  {
	    for (i = 0; i < send_event->num_entries; i++, info++)
	      {
		XtFree((char *)info->widgets.ids);
		XtFree(info->name);
		XtFree(info->class);
	      }   /* for i */

	    XtFree((char *)send_event->info);
	  }   /* if (info != NULL) */

	break;
      }   /* case SendWidgetTree: */
    /*---------------------*/

    case SetValues:
      {
	SetValuesEvent *sv_event = (SetValuesEvent*)event;
	SetValuesInfo  *info = sv_event->info;


	if (info != NULL)
	  {
	    for (i = 0; i < sv_event->num_entries; i++, info++)
	      {
		XtFree((char *)info->widgets.ids);
		XtFree(info->message);
	      }   /* for i */

	    XtFree((char *)sv_event->info);

	  }   /* if (info != NULL) */

	break;
      }   /* case SetValues: */
    /*---------------------*/

    case GetResources:
      {
	GetResourcesEvent *get_event = (GetResourcesEvent*)event;
	GetResourcesInfo  *info = get_event->info;


	if (info != NULL)
	  {
	    for (i = 0; i < get_event->num_entries; i++, info++)
	      {
		XtFree((char *)info->widgets.ids);

		if (info->error) 
		  {
		    XtFree(info->message);
		  }

		else
		  {
		    unsigned int j;
		    ResourceInfo *res_info = info->res_info;


		    if (res_info != NULL)
		      {
			for (j = 0; j < info->num_resources; j++, res_info++) 
			  {
			    XtFree(res_info->name);
			    XtFree(res_info->class);
			    XtFree(res_info->type);
			  }   /* for j */

			XtFree((char *)info->res_info);

		      }   /* if (res_info != NULL) */

		  }   /* else (NOT res_info != NULL) */

	      }   /* for i */
 
	    XtFree((char *)get_event->info);

	  }   /* if (info != NULL) */

	break;
      }   /* case GetResources: */
    /*---------------------*/

    case GetGeometry:
      {
	GetGeomEvent *geom_event = (GetGeomEvent*)event;
	GetGeomInfo  *info = geom_event->info;


	if (info != NULL)
	  {
	    for (i = 0; i < geom_event->num_entries; i++, info++)
	      {
		XtFree((char *)info->widgets.ids);

		if (info->error) 
		  {
		    XtFree(info->message);
		  }   /* if (info->error) */

	      }   /* for i */

	    XtFree((char *)geom_event->info);

	  }   /* if (info != NULL) */

	break;
      }   /* case GetGeometry: */
    /*---------------------*/

    case FindChild:
      {
	FindChildEvent *find_event = (FindChildEvent*)event;


	XtFree((char *)find_event->widgets.ids);

	break;
      }   /* case FindChild: */

    default:
      break;

    }   /* switch (event->any_event.type) */

}   /* FreeEvent() */


/*	Function Name: DispatchEvent
 *	Description: Handles the event, calling the proper function.
 *	Arguments: event - the event.
 *	Returns: one.
 */
static char *
DispatchEvent(Event *event)
{
	char *error = NULL;
	char msg[BUFSIZ];

	fprintf(stderr, "DispatchEvent: %d\n", global_client.command);
	switch(global_client.command) {
	case LocalSendWidgetTree:
		BuildVisualTree(global_tree_parent, event);
		break;
	case LocalSetValues:
		error = PrintSetValuesError(event);
		break;
	case LocalFlashWidget:
		error = HandleFlashWidget(event);
		break;
	case LocalGetResources:
		error = HandleGetResources(event);
		break;
	case LocalFindChild:
		DisplayChild(event);
		break;
	case LocalGetValues:
		break;
	default:

		sprintf(msg, "Internal error: Unknown command %d.", 
			global_client.command);
		error = XtNewString(msg);
		break;
	}   /* switch (global_client.command) */

	return(error);

}   /* DispatchEvent() */


/*	Function Name: InternAtoms
 *	Description: interns all static atoms.
 *	Arguments: display - the current display.
 *	Returns: none.
 */
void
InternAtoms(Display *dpy)
{
  atom_comm             = XInternAtom(dpy, EDITRES_COMM_ATOM, False);
  atom_command          = XInternAtom(dpy, EDITRES_COMMAND_ATOM, False);
  atom_resource_editor  = XInternAtom(dpy, EDITRES_NAME, False);
  atom_client_value     = XInternAtom(dpy, EDITRES_CLIENT_VALUE, False);
  atom_editres_protocol = XInternAtom(dpy, EDITRES_PROTOCOL_ATOM, False);
}   /* InternAtoms() */


static ResIdent
GetNewIdent(void)
{
  static ResIdent ident = 1;


  return(ident++);
}   /* GetNewIdent() */


/* >>>>> HACK for editres */

/* Function Name:
 *   brSetCommand
 *
 * Description:
 *   Causes this widget to own the resource editor's command selection.
 *
 * Arguments:
 *   w - the widget that will own the selection.
 *   command - command to send to client.
 *   msg - message to prompt the user to select a client.
 *
 * Returns:
 *   <none>
 *
 * Calls:
 *   SetMessage - utils.c
 *   GetClientWindow - (self)
 *   GetNewIdent -
 *   BuildHeader - (self)
 *
 * Global Data:
 */
/* ARGSUSED */
void
brSetCommand(w, command, msg)
     Widget     w;
     ResCommand command;
     char       *msg;
{
  XClientMessageEvent client_event;
  Display             *dpy = XtDisplay(w);


  if (global_client.window == None)
    {
      if ( (global_client.window = GetClientWindow(w,
						   NULL,
						   NULL)) == None ) 
	{
	  return;
	}
    }

  global_client.ident = GetNewIdent();

  global_client.command = command;
  global_client.atom = atom_comm;

  BuildHeader(&(global_client)); 

  if (!XtOwnSelection(w,
		      global_client.atom,
		      CurrentTime,
		      ConvertCommand, 
		      brLoseSelection,   /* send next SetValues packet */
		      SelectionDone))
    {
      /* "Unable to own the Resource/Attribute Selection" */
      SetMessage(global_screen_data.info_label,
		 res_labels[7],
		 "Unable to own the Resource/Attribute Selection");
    }

  client_event.window = global_client.window;
  client_event.type = ClientMessage;
  client_event.message_type = atom_resource_editor;
  client_event.format = EDITRES_SEND_EVENT_FORMAT;
  client_event.data.l[0] = XtLastTimestampProcessed(dpy);
  client_event.data.l[1] = global_client.atom;
  client_event.data.l[2] = (long) global_client.ident;
  client_event.data.l[3] = global_effective_protocol_version;

  global_error_code = NO_ERROR;                 /* Reset Error code. */
  global_old_error_handler = XSetErrorHandler(HandleXErrors);
  global_serial_num = NextRequest(dpy);

  XSendEvent(dpy,
	     global_client.window,
	     FALSE,
	     (long)0, 
	     (XEvent*)&client_event);

  XSync(dpy,
	FALSE);
  XSetErrorHandler(global_old_error_handler);

  if (global_error_code == NO_WINDOW)
    {
      char error_buf[BUFSIZ];


      global_error_code = NO_ERROR;	/* Reset Error code. */
      sprintf(error_buf,
	      "The communication window with%s%s.",
	      " application is no longer available\n",
	      "Please select a new widget tree");
    
      global_client.window = None;
      SetCommand(w,
		 LocalSendWidgetTree,
		 error_buf);

      return;
    }   
  
  global_client.timeout = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
					  CLIENT_TIME_OUT, 
					  ClientTimedOut,
					  (XtPointer)w);
  startWait(XtWidgetToApplicationContext(w),
	    CLIENT_TIME_OUT / 1000);

}   /* brSetCommand() */


/*	Function Name: brLoseSelection
 *	Description: Called when we have lost the selection, asks client
 *                   for the selection value.
 *	Arguments: w - the widget that just lost the selection.
 *                 sel - the selection.
 *	Returns: none.
 *
 * Calls:
 *   brGetClientValue - (self)
 */
static void
brLoseSelection(Widget w, Atom *sel)
{
  if (global_client.timeout != 0)
    {
      XtRemoveTimeOut(global_client.timeout);
      global_client.timeout = 0;

      /* reset the countdown */
      global_countdown = 0;
      if (global_timeout)
	{
	  XtRemoveTimeOut(global_timeout);
	  global_timeout = 0;
	  SetWaitLabel(0);
	}   /* if (old timeout running) */
    }

  XtGetSelectionValue(w,
		      *sel,
		      atom_client_value,
		      brGetClientValue,
		      NULL,
		      XtLastTimestampProcessed(XtDisplay(w)));

}   /* brLoseSelection() */


/*	Function Name: brGetClientValue
 *	Description: Gets the value out of the client, and does good things
 *                   to it.
 *	Arguments: w - the widget that asked for the selection.
 *                 data - client_data *** NOT USED ***.
 *                 sel - the selection.
 *                 type - the type of the selection.
 *                 value - the selection's value.
 *                 length - the length of the selection's value.
 *                 format - the format of the selection.
 *	Returns: none.
 */
/* ARGSUSED */
static void
brGetClientValue(Widget w, XtPointer data, Atom *selection, Atom *type,
                 XtPointer value, unsigned long *length, int *format)
{
  Event          *event;
  ProtocolStream alloc_stream;
  ProtocolStream *stream;
  unsigned char  ident;
  unsigned char  error_code;
  char           *error_str;
  char           msg[BUFSIZ];


/* type should be checked if XT_CONVERT_FAIL */

  if (*length == 0)
    {
      return;
    }

  stream = &alloc_stream;	/* easier to think of it this way... */

  stream->current = stream->top = (unsigned char*)value;
  stream->size = HEADER_SIZE;		/* size of header. */

  /*
   * Retrieve the Header.
   */

  if (*length < HEADER_SIZE)
    {
      /* "Incorrectly formatted message from client" */
      SetMessage(global_screen_data.info_label,
		 res_labels[9],
		 "Incorrectly formatted message from client");
      return;
    }

  (void)_XEditResGet8(stream, &ident);
  if (global_client.ident != ident)
    {

#ifdef DEBUG
      if (global_resources.debug)
	{
	  printf("Incorrect ident from client.\n");
	}
#endif  /* DEBUG */

      if (!XtOwnSelection(w,
			  *selection,
			  CurrentTime,
			  ConvertCommand, 
			  LoseSelection,
			  SelectionDone))
	{
	  /* "Unable to own the Resource Editor Command Selection" */
	  SetMessage(global_screen_data.info_label,
		     res_labels[10],
		     "Unable to own the Resource Editor Command Selection");
	}

      return;

    }   /* if (global_client.ident != ident) */

  (void)_XEditResGet8(stream, &error_code);
  (void)_XEditResGet32(stream, &(stream->size));
  stream->top = stream->current;   /* reset stream to top of value.*/

  switch ((int)error_code)
    {
    case PartialSuccess:
      /*****
      if (global_client.command == LocalSendWidgetTree &&
          global_effective_protocol_version < CURRENT_PROTOCOL_VERSION)
        {
          ++global_effective_protocol_version;
        }
      *****/
      if ((event = BuildEvent(stream)) != NULL)
	{
	  error_str = DispatchEvent(event);
	  FreeEvent(event);
	}
      else
	{
	  /* "Unable to unpack protocol request." */
	  sprintf(msg,
		  res_labels[38]);
	  error_str = XtNewString(msg);
	}
      break;   /* PartialSuccess */

    case Failure:
      error_str = GetFailureMessage(stream);
      break;   /* Failure */

    case ProtocolMismatch:
      error_str = ProtocolFailure(stream);
      --global_effective_protocol_version;
      /* normaly protocol version is reset to current during a SendWidgetTree
       * request, however, after a protocol failure this is skiped once for
       * a retry.
       */
      reset_protocol_level = False;
      SetCommand(w,
		 LocalSendWidgetTree,
		 NULL);
      break;   /* ProtocolMismatch */

    default:
	/* "Unknown Error code %i" */
	if (res_labels[11]) {
		sprintf(msg, res_labels[11], (int)error_code);
	} else {
		sprintf(msg, "Unknown Error code %i", (int)error_code);
	}
	SetMessage(global_screen_data.info_label, msg, "?res_labels[11]");
	break;    /* default */
    }   /* switch (error_code) */

	if (error_str == NULL) {
		WNode *top;

		if (global_tree_info == NULL || global_mode == MODE_BATCHRES) {
			NextLine(NULL);
			return;
		}

	top = global_tree_info->top_node;
	/* "Widget/Style tree for client %s (%s)" */
	if (res_labels[12]) {
		sprintf(msg, res_labels[12], top->name, top->class);
	} else {
		sprintf(msg, "Widget/Style tree for client %s (%s)",
			top->name,
			top->class);
	}
	SetMessage(global_screen_data.info_label, msg, "?res_labels[12]");

	NextLine(NULL);
	return;
	}   /* if (error_str == NULL) */

	SetMessage(global_screen_data.info_label, error_str, "?res_labels[12]");
	XtFree(error_str);

	NextLine(NULL);
}   /* brGetClientValue() */

/* <<<<< HACK for editres */
