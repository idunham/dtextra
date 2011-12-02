/*
 * $Header: /cvsroot-fuse/meditres/meditres/utils.c,v 1.15 2002/01/16 20:05:43 amai Exp $
 *
 * $XConsortium: utils.c,v 1.22 94/04/17 20:38:57 hersh Exp $
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
 * Motif port, MEDITRES project: © 2001, Danny Backx.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Error.h>

#include <Xm/Text.h>
#include <Xm/FileSB.h>

#include "meditres.h"


/* local functions */
static WidgetResources *ParseResources();
static int             CompareResourceEntries();
static void            FreeResources();
static void            AddResource();
static WNode           *FindWidgetFromWindowGivenNode();
static void            decCountdown();
static  void  _PopdownFileDialog();


/* Function Name:
 *   SetMessage(w, str, fallback)
 *
 * Description:
 *   Shows the message to the user.
 *
 * Arguments:
 *   w        - a label widget to show the message in.
 *   str      - the string to show. 
 *   fallback - fallback string, if str == NULL
 *
 * Returns:
 *   <void>
 *
 * Global data:
 *   <none>
 *
 * Calls:
 *   SetMessage - (recursive)
 */
void
SetMessage(Widget w, char *str, char *fallback)
{
  char	*ptr;
  int	l;

  if (w == NULL)
      return;
  if (MessageArea == NULL)
	return;
  if (str == 0)
	ptr = fallback;
  else
	ptr = str;

  XmTextInsert(MessageArea, XmTextGetLastPosition(MessageArea), ptr);

  /* Append newline when necessary */
  l = strlen(ptr);
  if (ptr[l-1] != '\n')
	XmTextInsert(MessageArea, XmTextGetLastPosition(MessageArea), "\n");
}


/* Function Name:
 *   GetAllStrings
 *
 * Description:
 *   Returns a list of strings that have been borken up by
 *   the character specified.
 *
 * Arguments:
 *   in  - the string to parse.
 *   sep - the separator character.
 *   out - the strings to send out.
 *   num - the number of strings in out.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   XtMalloc - X Toolkit
 */
void
GetAllStrings(char *in, char sep, char ***out, int  *num)
{
  int size;
  int i;
  char *ptr;


  if (*in == sep)		/* jump over first char if it is the sep. */
    {
      in++;
    }

  /*
   * count the number of strings.
   */

  for (*num = 1, ptr = in;  (ptr = strchr(ptr, sep)) != NULL;  (*num)++)
    {
      ptr++;
    }   /* for */

  /*
   * Create Enough space for pointers and string.
   */

  size = (sizeof(char*) * *num) + (sizeof(char) * (strlen(in) + 1));
  *out = (char **) XtMalloc( (Cardinal) size);

  ptr = (char*) (*out + *num);
  strcpy(ptr, in);

  /*
   * Change all `sep' characters to '\0' and stuff the pointer into
   * the next pointer slot.
   */

  i = 1;
  (*out)[0] = ptr;
  while (TRUE)
    {
      if ((ptr = strchr(ptr, sep)) == NULL)
	{
	  break;
	}

      *ptr++ = '\0';
      (*out)[i++] = ptr;

    }   /* while (TRUE) */

  /*
   * If last string is empty then strip it off.
   */

  if ( *((*out)[i - 1]) == '\0' )
    {
      (*num)--;
    }

}   /* GetAllStrings() */


/* Function Name:
 *   AddString
 *
 * Description:
 *   Mallocs and strcats the string onto the end of
 *   the given string.
 *
 * Arguments:
 *   str - string to add on to.
 *   add - string to add.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   XtRealloc - X Toolkit
 */
void
AddString(char **str, char *add)
{
  int  len_str;
  int  len_add;
  char *ptr;


  len_str = ((*str) ? strlen(*str) : 0);
  len_add = strlen(add);

  *str = XtRealloc(*str, sizeof(char) * (len_str + len_add + 1));
  ptr = *str + len_str;
  strcpy(ptr, add);

}   /* AddString() */

 
/* Function Name:
 *   FindNode
 *
 * Description:
 *   Finds a node give the top node, and a node id number.
 *
 * Arguments:
 *   top_node - the top node.
 *   id       - the node id.
 *   number   - number of given ids
 *
 * Returns:
 *   node found or NULL
 *
 * Calls:
 *
 */
WNode*
FindNode(WNode *top_node, unsigned long *ids, Cardinal number)
{
  int   i;
  int   j;
  WNode *node;


  if (top_node == NULL || ids == NULL)
    {
      return(NULL);
    }

  if (ids[0] != top_node->id)
    {
      return(NULL);
    }

  for (node = top_node, i = 1 ; i < number; i++)
    {
      Boolean found_it = FALSE;

      for (j = 0; j < node->num_children; j++)
	{
	  if (node->children[j]->id == ids[i])
	    {
	      node = node->children[j];
	      found_it = TRUE;
	      break;
	    }    /* if (children[j] == ids[i]) */

	}   /* for (children of node) */

      if (!found_it)
	{
	  return(NULL);
	}

    }	/* for (all nodes) */

  return(node);

}   /* FindNode() */

WNode *
FindNodeById(WNode *top, unsigned long id)
{
	int	i;
	WNode	*n;

	if (top->id == id)
		return top;
	for (i=0; i<top->num_children; i++) {
		n = FindNodeById(top->children[i], id);
		if (n)
			return n;
	}
	return NULL;
}


/* Function Name:
 *   FindWidgetFromWindow
 *
 * Description:
 *   finds a widget in the current tree given its window id.
 *
 * Arguments:
 *   tree_info - information about this tree.
 *   win       - window to search for.
 *
 * Returns:
 *   node - the node corrosponding to this widget.
 *
 * Calls:
 *   FindWidgetFromWindowGivenNode - (self)
 */

WNode*
FindWidgetFromWindow(TreeInfo *tree_info, Window win)
{
  if (tree_info == NULL)
    {
      return(NULL);
    }

    return(FindWidgetFromWindowGivenNode(tree_info->top_node,
					 win));

}   /* FindWidgetFromWindow() */


/* Function Name:
 *   FindWidgetFromWindowGivenNode
 *
 * Description:
 *   finds a widget in the current tree given its window id.
 *
 * Arguments:
 *   node - current node.
 *   win  - window to search for.
 *
 * Returns:
 *   node - the node corrosponding to this widget.
 *
 * Calls:
 *   FindWidgetFromWindowGivenNode - (recursive)
 */
static WNode*
FindWidgetFromWindowGivenNode(WNode *node, Window win)
{
  int   i;
  WNode *ret_node;


  if (node->window == win)
    {
      return(node);
    }

  for (i = 0; i < node->num_children; i++)
    {
      ret_node = FindWidgetFromWindowGivenNode(node->children[i],
					       win);
      if (ret_node != NULL)
	{
	  return(ret_node);
	}

    }   /* for (node children) */

    return(NULL);

}   /* FindWidgetFromWindowGivenNode() */


/* Function Name:
 *   HandleXErrors
 *
 * Description:
 *   Handles error codes from the server.
 *
 * Arguments:
 *   display - the display.
 *   error   - error information.
 *
 * Returns:
 *   0 - OK
 *
 * Calls: 
 *   XmuPrintDefaultErrorMessage - X Misc Utilites
 *
 * This function exits with EXIT_FAILURE if XmuPrintDefaultErrorMessage fails.
 */
/* ARGSUSED */
int
HandleXErrors(Display *display, XErrorEvent *error)
{
  if (error->serial != global_serial_num)
    {
      (*global_old_error_handler) (display, error);
      return(0);
    }   /* if (serial != global_serial) */

  if (error->error_code == BadWindow)
    {
      global_error_code = NO_WINDOW;    
    }   /* if (error == BadWindow) */

  else
    {
      if (XmuPrintDefaultErrorMessage(display, error, stderr) != 0)
	{
	  exit(EXIT_FAILURE);
	}

    }   /* else (error != BadWindow) */

    return(0);

}   /* HandleXErrors() */


/* Function Name:
 *   _DumpTreeToFile
 *
 * Description:
 *   Dumps the widget tree to a file
 *
 * Arguments:
 *   w        - a random widget in the application on the
 *              currently active display
 *   tree_ptr - pointer to the widget tree info.
 *   filename - name of the file.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   SetMessage - (self)
 *   PerformTreeToFileDump -
 */

/* ARGSUSED */
void
_DumpTreeToFile(Widget w, XtPointer tree_ptr, char *filename)
{
  TreeInfo *tree_info = (TreeInfo *) tree_ptr;
  FILE     *fp; 


fprintf(stderr, "_DumpTreeToFile(,,%s)\n", filename);
  if (tree_info == NULL)
    {
      /* "No widget tree is available" */
      SetMessage(global_screen_data.info_label,
		 res_labels[17],
		 "No widget tree is available");
      return;
    }

  if ( (fp = fopen(filename, "w")) == NULL )
    {
      char buf[BUFSIZ];

      /* "Unable to open the file %s for writing" */
      if (res_labels[24])
	{
	  sprintf(buf,
		  res_labels[24],
		  filename);
	}
      else
	{
	  sprintf(buf,
		  "Unable to open the file %s for writing",
		  filename);
	}
      SetMessage(global_screen_data.info_label,
		 buf,
		 "?res_labels[24]");
      return;
    }

  PerformTreeToFileDump(tree_info->top_node,
			0,
			fp);
  fclose(fp);

}   /* _DumpTreeToFile() */


/* Function Name:
 *   startWait
 *
 * Description:
 *   Start the timercallback for the countdown in the status box
 *
 * Arguments:
 *   time   - countdown in seconds
 *
 * Returns:
 *   <void>
 *
 * Global data:
 *   global_countdown
 *
 * Calls:
 *   decCountdown - (self)
 *   SetWaitLabel - widgets.c
 */
void
startWait(XtAppContext app_con, int time)
{
  if (global_countdown < time)
    {
      /* another countdown is smaller */
      /* or no other countdown active */
      SetWaitLabel(time);

      if (time > 0)
	{
	  global_countdown = time;
	  if (global_timeout)
	    {
	      XtRemoveTimeOut(global_timeout);
	    }   /* if (old timeout running) */

	  global_timeout = XtAppAddTimeOut(app_con,
					   1000,
					   decCountdown,
					   (XtPointer)app_con);
	}
    }

}   /* startWait() */


/* Function Name:
 *   decCountdown
 *
 * Description:
 *   Shows the aktual countdown value and restarts the timer callback
 *
 * Arguments:
 *   context  - the applicaton context
 *   id       - (XtIntervalId)
 *
 * Returns:
 *   <void>
 *
 * Global data:
 *   global_countdown
 *
 * Calls:
 *   SetWaitLabel - widgets.c
 */
static void
decCountdown(XtPointer context, XtIntervalId *id)
{
  global_countdown--;
  SetWaitLabel(global_countdown);

  if (global_countdown > 0)
    {
      if (global_timeout)
	{
	  XtRemoveTimeOut(global_timeout);
	}   /* if (old timeout running) */

      global_timeout = XtAppAddTimeOut(context,
				       1000,
				       decCountdown,
				       (XtPointer)context);
    }

}   /* decCountdown() */


/************************************************************
 * 
 * The file dialog boxes are handled with this code.
 *
 * It automatically calls the function specified when the
 * user selects okay, or hits <CR>.
 *
 * A translation is required in the app-defaults file.
 *
 ************************************************************/

/* Function Name:
 *   _PopupFileDialog
 *
 * Description:
 *   Puts up a dialog box to get the filename.
 *
 * Arguments:
 *   str           - message.
 *   default_value - the default value of the filename;
 *   func          - function to call when filename has been entered.
 *   data          - generic data to pass to func.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   SetMessage - (self)
 *   PopupCentered - (self)
 *   XUniqueContext, XSaveContext
 *   XawDialogAddButton
 *   Xt-stuff
 */

static XContext  file_dialog_context = None;

typedef struct _FileDialogInfo
{
  void      (*func)();
  XtPointer data;
} FileDialogInfo;


void
_PopupFileDialog(Widget w, String str, String default_value,
                 void (*func)(), XtPointer data)
{
  FileDialogInfo *file_info;
  Widget         dialog;
  Arg            args[2];
  Cardinal       num_args;


  if (file_dialog_context == None)
    {
      file_dialog_context = XUniqueContext();
    }

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNlabel, str);
  num_args++;
  XtSetArg(args[num_args],
	   XtNvalue, default_value);
  num_args++;
  dialog = XmCreateFileSelectionDialog (XtParent(w), "dialog", args, num_args);
  XtAddCallback (dialog, XmNokCallback, (XtCallbackProc)_PopdownFileDialog, (XtPointer)TRUE);
  XtAddCallback (dialog, XmNcancelCallback,  (XtCallbackProc)_PopdownFileDialog, (XtPointer)FALSE);
  XtManageChild (dialog);

  file_info = XtNew(FileDialogInfo);

  file_info->func = func;
  file_info->data = data;

  if  (XSaveContext(XtDisplay(dialog),
		    (Window) dialog,
		    file_dialog_context, 
		    (XPointer) file_info) != 0)
    {
      SetMessage(global_screen_data.info_label,
		 res_labels[40],
		 "Error while trying to save Context\nAborting file dialog popup.");
      XtDestroyWidget(dialog);
      return;
    }   /* if (XSaveContext != 0) */

/*  PopupCentered(NULL, dialog, XtGrabNone); */
}   /* _PopupFileDialog() */


/* Function Name:
 *   PopupCentered
 *
 * Description:
 *   Pops up the window specified under the location passed
 *   in the event, or under the cursor.
 *
 * Arguments:
 *   event - the event that we should use.
 *   w     - widget to popup.
 *   mode  - mode to pop it up in.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   XQueryPointer
 *   Xt-stuff
 */
void
PopupCentered(XEvent *event, Widget w, XtGrabKind mode)
{
  Boolean   get_from_cursor = FALSE;
  Arg       args[3];
  Cardinal  num_args;
  Dimension width;
  Dimension height;
  Dimension b_width;
  int       x;
  int       y;
  int       max_x;
  int       max_y;


  XtRealizeWidget(w);

  if (event == NULL)
    {
      get_from_cursor = TRUE;
    }   /* if (event == NULL) */

  else
    {
      switch (event->type)
	{
	case ButtonPress:
	case ButtonRelease:
	  x = event->xbutton.x_root;
	  y = event->xbutton.y_root;
	  break;

	case KeyPress:
	case KeyRelease:
	  x = event->xkey.x_root;
	  y = event->xkey.y_root;
	  break;

	default:
	  get_from_cursor = TRUE;
	  break;

	}   /* switch (event->type) */

    }   /* else (event != NULL) */

  if (get_from_cursor)
    {
      Window       root;
      Window       child;
      int          win_x;
      int          win_y;
      unsigned int mask;
	

      XQueryPointer(XtDisplay(w),
		    XtWindow(w),
		    &root,
		    &child,
		    &x, &y,
		    &win_x, &win_y,
		    &mask);
    }   /* if (get_from_cursor) */

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNwidth, &width);
  num_args++;
  XtSetArg(args[num_args],
	   XtNheight, &height);
  num_args++;
  XtSetArg(args[num_args],
	   XtNborderWidth, &b_width);
  num_args++;
  XtGetValues(w, args, num_args);

  width += 2 * b_width;
  height += 2 * b_width;

  /* Keep x between 0 and (ScreenWidth - WindowWidth) */
  x -= ((int) width/2);
  if (x < 0) 
    {
      x = 0;
    }

  max_x = (int) (XtScreen(w)->width - width);
  if ( x > max_x )
    {
      x = max_x;
    }

  /* Keep y between 0 and (ScreenHeight - WindowHeight) */
  y -= ( (int) height/2 );
  if (y < 0)
    {
      y = 0;
    }

  max_y = (int) (XtScreen(w)->height - height);
  if ( y > max_y )
    {
      y = max_y;
    }
  
  num_args = 0;
  XtSetArg(args[num_args],
	   XtNx, x);
  num_args++;
  XtSetArg(args[num_args],
	   XtNy, y);
  num_args++;
  XtSetValues(w,
	      args, num_args);

  XtPopup(w, mode);

}   /* PopupCentered() */


/* Function Name:
 *   _PopdownFileDialog
 *
 * Description:
 *   Destroys the file dialog, and calls the correct function.
 *
 * Arguments:
 *   w           - a child of the dialog widget.
 *   client_data - TRUE if command was sucessful.
 *   junk        - ** UNUSED **.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   SetMessage - (self)
 *   XFindContext, XDeleteContext
 *   XawDialogGetValueString
 *   Xt-stuff
 */
/* ARGSUSED */
void 
_PopdownFileDialog(Widget w, XtPointer client_data, XtPointer junk)
{

  Widget         dialog = w;
  XPointer       file_info_ptr;
  FileDialogInfo *file_info;

fprintf(stderr, "_PopdownFileDialog\n");

  if (XFindContext(XtDisplay(dialog),
		   (Window) dialog,
		   file_dialog_context,
		   &file_info_ptr) == XCNOENT)
    {
      SetMessage(global_screen_data.info_label,
		 res_labels[41],
		 "Error while trying to find Context\n Aborting...");	
    }  /* if (XFindContext == XC NO ENTRY) */

  file_info = (FileDialogInfo*) file_info_ptr;
  (void) XDeleteContext(XtDisplay(dialog),
			(Window)dialog, 
			file_dialog_context);

  if ( ((Boolean)client_data) == TRUE )
    {
        char *filename=malloc(MAXPATHLEN);
        XmFileSelectionBoxCallbackStruct *cbs =
                          (XmFileSelectionBoxCallbackStruct *)junk;

        XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &filename);
 
        (*file_info->func)(w, file_info->data, filename); /* call handler */
        free(filename);
    }   /* if (client_data == TRUE) */

  XtFree( (XtPointer) file_info); /* Free data. */

#if 0
  XtPopdown(XtParent(dialog));
  XtDestroyWidget(XtParent(dialog)); /* Remove file dialog. */
#else
  XtDestroyWidget(dialog); /* Remove file dialog. */
#endif
}   /* _PopdownFileDialog() */


/************************************************************
 *
 * Functions for dealing with the Resource Box.
 *
 ************************************************************/

/* Function Name:
 *   GetNamesAndClasses
 *
 * Description:
 *   Gets a list of names and classes for this widget.
 *
 * Arguments:
 *   node           - this widget's node.
 *   names, classes - list of names and classes. ** RETURNED **
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   XtMalloc
 */
void
GetNamesAndClasses(WNode *node, char  ***names, char  ***classes)
{
  int   i;
  int   total_widgets;
  WNode *temp = node;


  for (total_widgets = 1 ; temp->parent != NULL ; total_widgets++)
    {
      temp = temp->parent;
    }   /* for (counting nodes) */

  *names = (char **) XtMalloc( sizeof(char*) * (total_widgets + 1) );
  *classes = (char **) XtMalloc( sizeof(char*) * (total_widgets + 1) );

  (*names)[total_widgets] = NULL;
  (*classes)[total_widgets] = NULL;

  for ( i = (total_widgets - 1); i >= 0 ; node = node->parent, i--)
    {
      (*names)[i] = node->name;
      (*classes)[i] = node->class;
    }   /* for (fill arrays with names) */

}   /* GetNamesAndClasses() */


/* 
 * This is called by DispatchEvent() to handle a reply.
 *
 * Function Name:
 *   HandleGetResources
 *
 * Description:
 *   Gets the resources.
 *
 * Arguments:
 *   event - the information from the client.
 *
 * Returns:
 *   an error message to display.
 *
 * Calls:
 *   FindNode -
 *   AddString - (self)
 *   FreeResources - (self)
 *   ParseResources - (self)
 *   CreateResourceBox - (self)
 */
char*
HandleGetResources(Event *event)
{
	GetResourcesEvent *get_event = (GetResourcesEvent*) event;
	char              buf[BUFSIZ];
	char              *errors = NULL;
	int               i;
	WNode             *node;

	for (i = 0; i < (int)get_event->num_entries; i++) {
		node = FindNode(global_tree_info->top_node,
			get_event->info[i].widgets.ids, 
			get_event->info[i].widgets.num_widgets);

		if (node == NULL) {
			/* "Editres Internal Error: Unable to FindNode" */
			if (res_labels[16] != NULL) {
				sprintf(buf, res_labels[16]);
			} else {
				sprintf(buf, "Editres Internal Error: Unable to FindNode");
			}
			AddString(&errors, buf); 
			continue;
		}   /* if (node == NULL) */
		if (node->resources != NULL) {
			FreeResources(node->resources);
		} /* if (node->ressources != NULL) */
		if (!get_event->info[i].error) {
			node->resources = ParseResources(get_event->info + i, &errors);
			CreateResourceBox(node, &errors);

			/* This is support for opening up multiple resource boxes */
			OpenAdditionalResourceBoxes();
		} else  {
			AddString(&errors, get_event->info[i].message);
			AddString(&errors, "\n");
		}   /* else (info[i].error) */
	}   /* for (all event entries) */

	return(errors);
}   /* HandleGetResources() */

/* Function Name:
 *   CreateResourceBox
 *
 * Description:
 *   Creates a resource box for the widget specified.
 *
 * Arguments:
 *   node   - the node of the widget in question.
 *   errors - an error string.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   AddString - (self)
 *   XtMalloc -
 *   CreateResourceBoxWidgets - (self)
 */
void
CreateResourceBox(WNode *node, char **errors)
{
  WidgetResources *resources = node->resources;
  char            **names;
  char            **cons_names;
  int             i;

  /* Get names of normal resources */
  if (resources->num_normal > 0) {
      names = (char**)XtMalloc(sizeof(char*) * (resources->num_normal + 1));

      for (i = 0 ; i < resources->num_normal ; i++) {
	  names[i] = resources->normal[i].name;
      }   /* for (all normal resources) */

      names[i] = NULL;
  } else { /* if (there are normal resources) */
	names = NULL;
  }   /* else (no normal resources) */

  /* Get names of constraint resources */
  if (resources->num_constraint > 0) {
      cons_names = (char**)XtMalloc(sizeof(char*)
				    * (resources->num_constraint + 1));
	
      for (i = 0 ; i < resources->num_constraint ; i++) 
	{
	  cons_names[i] = resources->constraint[i].name;
	}   /* for (all constraint resources) */

	cons_names[i] = NULL;
    }   /* if (there are constraint resources) */

  else
    {
      cons_names = NULL;
    }   /* else (no constraint resources) */

    CreateResourceBoxWidgets(node, names, cons_names);
}   /* CreateResourceBox() */


/* Function Name:
 *   ParseResources
 *
 * Description:
 *   Parses the resource values returned from the client
 *   into a resources structure.
 *
 * Arguments:
 *   info  - info about a widget's resources.
 *   error - where to place error info.
 *
 * Returns:
 *   The resource information.
 *
 * Calls:
 *   AddResource -
 *   AddString -
 *   XtMalloc
 */
static WidgetResources*
ParseResources(info, error)
     GetResourcesInfo *info;
     char             **error;
{
  WidgetResources    *resources;
  WidgetResourceInfo *normal;
  int                i;


  resources = (WidgetResources *) XtMalloc(sizeof(WidgetResources));
  
  /*
   * Allocate enough space for both the normal and constraint resources,
   * then add the normal resources from the top, and the constraint resources
   * from the bottom.  This assures that enough memory is allocated, and
   * that there is no overlap.
   */

  resources->normal = (WidgetResourceInfo*)XtMalloc(sizeof(WidgetResourceInfo)
						    * info->num_resources);

  normal = resources->normal;
  resources->constraint = resources->normal + info->num_resources - 1;

  resources->num_constraint = 0;
  resources->num_normal = 0;

  /* Get number of normal and constraint resources */
  for (i = 0; i < (int)info->num_resources; i++)
    {
      switch( (int)info->res_info[i].res_type )
	{
	case NormalResource:
	  resources->num_normal++;
	  AddResource(info->res_info + i,
		      normal++);
	  break;

	case ConstraintResource:
	  resources->num_constraint++;
	  AddResource(info->res_info + i,
		      resources->constraint--);
	  break;

	default:
	  {
	    char buf[BUFSIZ];


	    sprintf(buf,
		    "Unknown resource type %d\n", 
		    info->res_info[i].res_type);
	    AddString(error,
		      buf);
	  }
	  break;

	}   /* switch(info->res_info[i].res_type) */

    }   /* for (all resources) */

  /*
   * Sort the resources alphabetically. 
   */

  if (resources->num_normal > 0)
    {
      qsort(resources->normal,
	    resources->num_normal,
	    sizeof(WidgetResourceInfo),
	    CompareResourceEntries);
    }   /* if (there are normal resources) */

  else
    {
      resources->constraint = NULL;
    }   /* else (no normal resources) */

  if (resources->num_constraint > 0)
    {
      resources->constraint++;
      qsort(resources->constraint,
	    resources->num_constraint,
	    sizeof(WidgetResourceInfo),
	    CompareResourceEntries);
    }   /* if (there are constraint resources) */

  else
    {
      resources->constraint = NULL;
    }   /* else (no constraint resources) */

  return(resources);

}   /* ParseResources() */


/* Function Name:
 *   CompareResourceEntries
 *
 * Description:
 *   Compares two resource entries.
 *
 * Arguments:
 *   e1, e2 - the entries to compare.
 *
 * Returns:
 *   an integer >, < or = 0.
 *
 * Calls:
 *
 */
static int 
CompareResourceEntries(e1, e2) 
     WidgetResourceInfo *e1;
     WidgetResourceInfo *e2;
{
  return (strcmp(e1->name,
		 e2->name));
}   /* CompareResourceEntries() */


/* Function Name:
 *   AddResource
 *
 * Description:
 *   Parses the resource string a stuffs in individual
 *   parts into the resource info struct.
 *
 * Arguments:
 *   res_info - the resource info from the event.
 *   resource - location to stuff the resource into.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *
 */
static void
AddResource(res_info, resource) 
     ResourceInfo       *res_info;
     WidgetResourceInfo *resource;
{
  resource->name = res_info->name;
  res_info->name = NULL;	/* Keeps it from being deallocated. */
  resource->class = res_info->class;
  res_info->class = NULL;	/* Keeps it from being deallocated. */
  resource->type = res_info->type;
  res_info->type = NULL;	/* Keeps it from being deallocated. */
}   /* AddResource() */


/* Function Name:
 *   FreeResources
 *
 * Description:
 *   frees the resource information.
 *
 * Arguments:
 *   resources.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   XtFree
 */
static void
FreeResources(resources) 
     WidgetResources *resources;
{
  int i;


  if (resources->num_normal > 0)
    {
      for (i = 0; i < resources->num_normal; i++)
	{
	  XtFree(resources->normal[i].name);
	  XtFree(resources->normal[i].class);
	  XtFree(resources->normal[i].type);
	}

      XFree((char*)resources->normal);
    }

  if (resources->num_constraint > 0)
    {
      for (i = 0; i < resources->num_constraint; i++)
	{
	  XtFree(resources->constraint[i].name);
	  XtFree(resources->constraint[i].class);
	  XtFree(resources->constraint[i].type);
	}

      XFree((char*)resources->constraint);
    }

  XFree((char*)resources);

}   /* FreeResources() */
	

/* Function Name:
 *   CheckDatabase
 *
 * Description:
 *   Checks to see if the node is in the database.
 *
 * Arguments:
 *   db - the db to check
 *   names, clases - names and clases, represented as quarks.
 *
 * Returns:
 *   True if this entry is found.
 *
 * Calls:
 *   XrmQGetResource
 */
Boolean
CheckDatabase(XrmDatabase db, XrmQuarkList names, XrmQuarkList classes)
{
  XrmRepresentation junk;
  XrmValue          garbage;


  return(XrmQGetResource(db, names, classes, &junk, &garbage));

}   /* CheckDatabase() */


/* Function Name:
 *   Quarkify
 *
 * Description:
 *   Quarkifies the string list specifed.
 *
 * Arguments:
 *   list - list of strings to quarkify
 *   ptr  - an additional string to quarkify.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   XrmStringToQuark
 *   XtMalloc
 */
XrmQuarkList
Quarkify(char **list, char *ptr)
{
  int          i;
  char         **tlist;
  XrmQuarkList quarks;
  XrmQuarkList tquarks;


  i = 0;
  for (tlist = list; *tlist != NULL; tlist++)
    {
      i++;
    }   /* for (all strings) */

  if (ptr != NULL)
    {
      i++;
    }

  i++;			/* leave space for NULLQUARK */

  quarks = (XrmQuarkList)XtMalloc(sizeof(XrmQuark) * i);

  tquarks = quarks;
  for (tlist = list; *tlist != NULL; tlist++) 
    {
      *tquarks = XrmStringToQuark(*tlist);
      tquarks++;
    }   /* for (all strings) */

  if (ptr != NULL) 
    {
      *tquarks++ = XrmStringToQuark(ptr);
    }
	
  *tquarks = NULLQUARK;

  return(quarks);

}   /* Quarkify() */


/* Function Name:
 *   ExecuteOverAllNodes
 *
 * Description:
 *   Executes the given function over all nodes.
 *
 * Arguments:
 *   top_node - top node of the tree.
 *   func     - the function to execute.
 *   data     - a data pointer to pass to the function.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   ExecuteOverAllNodes - (recursive)
 */
void
ExecuteOverAllNodes(WNode *top_node, void (*func)(), XtPointer data)
{
	int i;

	(*func)(top_node, data);

	for (i = 0; i < top_node->num_children; i++) {
		ExecuteOverAllNodes(top_node->children[i], func, data);
	}   /* for (all children of top_node) */
}   /* ExecuteOverAllNodes() */


/* Function Name:
 *   InsertWidgetFromNode
 *
 * Description:
 *   Inserts the widget info for this widget represented
 *   by this node.
 *
 * Arguments:
 *   stream - the stream to insert it info into.
 *   none   - the widget node to insert.
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   XtMalloc, XtFree
 *   XEditRes-stuff
 */
void
InsertWidgetFromNode(ProtocolStream *stream, WNode *node)
{
  WNode         *temp;
  unsigned long *widget_list;
  register int  i;
  register int  num_widgets;


  i = 0;
  for (temp = node; temp != 0; temp = temp->parent)
    {
      i++;
    }

  num_widgets = i;
  widget_list = (unsigned long*)XtMalloc(sizeof(unsigned long) * num_widgets);

  /*
   * Put the widgets into the list.
   * Make sure that they are inserted in the list from parent -> child.
   */

  for (i--, temp = node; temp != 0; temp = temp->parent, i--) 
    {
      widget_list[i] = temp->id;
    }
	
  _XEditResPut16(stream, num_widgets);	/* insert number of widgets. */
  for (i = 0; i < num_widgets; i++) 	/* insert Widgets themselves. */
    {
      _XEditResPut32(stream, widget_list[i]);
    }
    
  XtFree((char*)widget_list);

}   /* InsertWidgetFromNode() */


/* Function Name:
 *   GetFailureMesssage
 *
 * Description:
 *   returns the message returned from a failed request.
 *
 * Arguments:
 *   stream - the protocol stream containing the message.
 *
 * Returns:
 *   message to show.
 *
 * Calls:
 *   XtNewString
 *   XEditRes-stuff
 */
char*
GetFailureMessage(ProtocolStream *stream)
{
  char *return_str;
  char *fb_label35 = "Unable to unpack protocol request";


  if (_XEditResGetString8(stream, &return_str)) 
    {
      return(return_str);
    }

  /* "Unable to unpack protocol request" */
  if (res_labels[35] != NULL)
    {
      return(XtNewString(res_labels[35]));
    }
  else
    {
      return(XtNewString(fb_label35));
    }

}   /* GetFailureMessage() */


/* Function Name:
 *   ProtocolFailure
 *
 * Description:
 *   Gets the version of the protocol the client is
 *   willing to speak.
 *
 * Arguments:
 *   stream - the protocol stream containing the message.
 *
 * Returns:
 *   message to show.
 *
 * Calls:
 *   XEditRes-stuff
 *   XtNewString
 */
char*
ProtocolFailure(ProtocolStream *stream)
{
	char          buf[BUFSIZ];
	unsigned char version;
	char          *old_version_string;
	char          *fb_label35 = "Unable to unpack protocol request";


	if (!_XEditResGet8(stream, &version)) {
		/* "Unable to unpack protocol request" */
		if (res_labels[35] != NULL) {
			return(XtNewString(res_labels[35]));
		} else {
			return(XtNewString(fb_label35));
		}
	}

	switch ((int)version) {
	case PROTOCOL_VERSION_ONE_POINT_ZERO:
		old_version_string = "1.0";
		break;
	default:
		old_version_string = "1.0";
	}   /* switch (version) */

	/* "This version of editres uses protocol version %s, */
	/* but the client speaks version %s" */
	if (res_labels[36] != NULL) {
		sprintf(buf, res_labels[36], 
			CURRENT_PROTOCOL_VERSION_STRING,
			old_version_string);
	} else {
		sprintf(buf,
			"This version of editres uses protocol version %s,\n"
			"but the client speaks version %s",
			CURRENT_PROTOCOL_VERSION_STRING,
			old_version_string);
	}

	return(XtNewString(buf));
}   /* ProtocolFailure() */
