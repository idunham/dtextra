/*
 * $Header: /cvsroot-fuse/meditres/meditres/geometry.c,v 1.5 2002/01/16 20:05:43 amai Exp $
 *
 * $XConsortium: geometry.c,v 1.16 94/04/17 20:38:55 dave Exp $
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
 *
 * Author:  Chris D. Peterson, MIT X Consortium
 */

/*
 * Modifications by Volker Gering, 1997
 *
 * Motif port, MEDITRES project, © 2001 by Danny Backx
 */

#include "config.h"

#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include "meditres.h"


/* local functions */
static WNode *FindWidgetFromWindow();
static WNode *FindWidgetFromWindowGivenNode();
static void  CreateFlashWidget();
static void  FlashWidgets();
static void  AddToFlashList();
static void  _AddToFlashList();
static void  FlashWidgetsOn();
static void  FlashWidgetsOff();
static void  FlashWidgetsCleanup();


/* Function Name:
 *   _FindWidget
 *
 * Description:
 *   Finds a widget in the tree and shows it to the user.
 *
 * Arguments:
 *   w - any widget in the application.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   SetMessage - utils.c
 *   GetClientWindow -
 *   FindWidgetFromWindow -
 *   InsertWidgetFromNode -
 *   SetCommand -
 */
void 
_FindWidget(Widget w)
{
  WNode  *node;
  Window win;
  Window GetClientWindow();
  int    x;    		/* location of event ... */
  int    y;             /* ... in root coordinates. */


  /* "Click on any widget/style in the client. */
  /* Editres will select that widget/style in the tree display" */
  SetMessage(global_screen_data.info_label,
	     res_labels[14],
	     "Click on any widget/style in the client.\nEditres will select that widget/style in the tree display");

  if ( (win = GetClientWindow(w,
			      &x,
			      &y)) != None)
    {
      node = FindWidgetFromWindow(global_tree_info,
				  win);
      if (node != NULL)
	{
	  ProtocolStream *stream = &(global_client.stream);	    


	  _XEditResResetStream(stream);
	  InsertWidgetFromNode(stream,
			       node);
	  _XEditResPut16(stream,
			 (short)x);
	  _XEditResPut16(stream,
			 (short)y);
	  SetCommand(w,
		     LocalFindChild,
		     NULL);

	  return;
	}   /* if (node != NULL) */

    }   /* if (GetClientWindow OK) */

  /* "That window does not appear to be */
  /* in the currently displayed client" */
  SetMessage(global_screen_data.info_label, 
	     res_labels[15],
	     "That window does not appear to be in the currently displayed client");

}   /* _FindWidget() */


/* Function Name:
 *   FindWidgetFromWindow
 *
 * Description:
 *   finds a widget in the current tree given its window id.
 *
 * Arguments: tree_info - information about this tree.
 *   win - window to search for.
 *
 * Returns:
 *   node - the node corrosponding to this widget.
 *
 * Calls:
 *   FindWidgetFromWindowGivenNode - (self)
 */
static WNode
*FindWidgetFromWindow(TreeInfo *tree_info, Window win)
{
  if (tree_info == NULL)
    {
      return(NULL);
    }

  return (FindWidgetFromWindowGivenNode(tree_info->top_node,
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
 *   win - window to search for.
 *
 * Returns:
 *   node - the node corrosponding to this widget.
 *
 * Calls:
 *   FindWidgetFromWindowGivenNode - (recursive)
 */
static WNode
*FindWidgetFromWindowGivenNode(WNode *node, Window win)
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

    }   /* for */

  return(NULL);

}   /* FindWidgetFromWindowGivenNode() */


/*	Function Name: DisplayChild
 *	Description: Displays the child node returned by the client
 *	Arguments: event - the event from the client.
 *	Returns: none.
 */
void
DisplayChild(Event *event)
{
  FindChildEvent *find_event = (FindChildEvent*)event;
  WNode          *node;
  char           msg[BUFSIZ];

  node = FindNode(global_tree_info->top_node,
		  find_event->widgets.ids,
		  find_event->widgets.num_widgets);

  if (node == NULL)
    {
      /* "Welcome to the X Resource Editor version 1.1" */
      SetMessage(global_screen_data.info_label,
		 res_labels[13],
		 "Welcome to the X Resource Editor version 1.1");
      return;	
    }

  SetAndCenterTreeNode(node);

  node = node->tree_info->top_node;

  /* "Widget/Style tree for client %s (%s)" */
  if (res_labels[12])
    {
      sprintf(msg,
	      res_labels[12],
	      node->name,
	      node->class);
    }
  else
    {
      sprintf(msg,
	      "Widget/Style tree for client %s (%s)",
	      node->name,
	      node->class);
    }
  SetMessage(global_screen_data.info_label,
	     msg,
	     "?res_labels[12]");

  _FlashActiveWidgets(global_tree_info);

}   /* DisplayChild() */


/*	Function Name: _FlashActiveWidgets
 *	Description: Highlights all active widgets in the tree.
 *	Arguments: tree_info - information about the current tree.
 *	Returns: none.
 */
void
_FlashActiveWidgets(TreeInfo *tree_info)
{
  int            i;
  ProtocolStream *stream = &(global_client.stream);


  if (tree_info == NULL)
    {
      /* "No Widget/Style tree is available" */
      SetMessage(global_screen_data.info_label,
		 res_labels[17],
		 "No Widget/Style tree is available");
      return;
    }   /* if (no tree_info) */

  if (tree_info->num_active == 0)
    {
      /* "There are no active nodes" */
      SetMessage(global_screen_data.info_label,
		 res_labels[18],
		 "There are no active nodes");
      return;
    }   /* if (no widgets selected) */
	
  _XEditResResetStream(stream); 
  /*
   * Insert the number of widgets. 
   */
  _XEditResPut16(stream,
		 (unsigned short)tree_info->num_active);

  for (i = 0; i < tree_info->num_active; i++) {
		InsertWidgetFromNode(stream, tree_info->active_nodes[i]);
  }   /* for (number of widgets) */

  SetCommand(tree_info->tree_widget, LocalFlashWidget, NULL);

}   /* _FlashActiveWidgets() */


/*	Function Name: HandleFlashWidget
 *	Description: Is called when client has returned geometry of all widget
 *                   to flash.
 *	Arguments: event - the event containing the client info.
 *	Returns: none.
 */
char
*HandleFlashWidget(Event *event)
{
  GetGeomEvent *geom_event = (GetGeomEvent *) event;
  char         *errors = NULL;
  int          i;


  for (i = 0; i < (int)geom_event->num_entries; i++) 
    {
      AddToFlashList(global_tree_info, geom_event->info + i, &errors);
    }   /* for */

  FlashWidgets(global_tree_info);

  return(errors);

}   /* HandleFlashWidget() */


/*	Function Name: AddWidgetToFlashList
 *	Description: Adds a widget to the list of widget to flash.
 *	Arguments: tree_info - info about this tree.
 *                 geom_info - the info from the client about this widget.
 *                 errors - a string containing the errors.
 *	Returns: none
 */
static void
AddToFlashList(TreeInfo *tree_info, GetGeomInfo *geom_info, char **errors)
{
  WNode *node;
  char  buf[BUFSIZ];


  node = FindNode(tree_info->top_node, 
		  geom_info->widgets.ids,
		  geom_info->widgets.num_widgets);

  if (node == NULL)
    {
      sprintf(buf,
	      "Editres Internal Error: Unable to FindNode.\n");
      AddString(errors,
		buf); 
      return;	
    }   /* if (node not found) */

  if (geom_info->error)
    {
      AddString(errors,
		geom_info->message); 
      return;	
    }

  if (!geom_info->visable)
    {
      sprintf(buf,
	      "%s(0x%lx) - This widget is not mapped\n",
	      node->name,
	      node->id);
      AddString(errors,
		buf); 
      return;
    }

  _AddToFlashList(tree_info,
		  errors,
		  node, 
		  geom_info->x,
		  geom_info->y, 
		  geom_info->width + geom_info->border_width, 
		  geom_info->height + geom_info->border_width);

}   /* AddToFlashList() */


/*	Function Name: _AddToFlashList
 *	Description: adds the window to the current client's flash list.
 *	Arguments: errors - a string to stuff any errors encountered.
 *                 node - the node associated with this object.
 *                 x, y - location of the flash widget in root coords.
 *                 width, height - size of the flash widget.
 *	Returns: none.
 */
static void
_AddToFlashList(TreeInfo *tree_info, char **errors, WNode *node,
                int x, int y, unsigned int width, unsigned int height)
{
  Display           *dpy = XtDisplay(tree_info->tree_widget);
  Window            window = (Window)node->window;
  XWindowAttributes attrs;
  char              msg[BUFSIZ];


  if (window == EDITRES_IS_OBJECT)
    {
      window = node->parent->window;
    }

  if (window == EDITRES_IS_UNREALIZED)
    {
      if (node->window == EDITRES_IS_OBJECT)
	{
	  sprintf(msg,
		  "%s(0x%lx) - This object's parent is unrealized\n", 
		  node->name,
		  node->id);	    
	}
      else
	{
	  sprintf(msg,
		  "%s(0x%lx) - This widget is unrealized\n", 
		  node->name,
		  node->id);
	}

      AddString(errors,
		msg); 
      return;

    }   /* if (window == UNREALIZED) */

  global_error_code = NO_ERROR;                 /* Reset Error code. */
  global_old_error_handler = XSetErrorHandler(HandleXErrors);
  global_serial_num = NextRequest(dpy);

  XGetWindowAttributes(dpy,
		       window,
		       &attrs);

  XSync(dpy,
	FALSE);
  XSetErrorHandler(global_old_error_handler);
  if (global_error_code == NO_WINDOW)
    {
      sprintf(msg,
	      "%s(0x%lx) - This widget's window no longer exists.\n", 
	      node->name,
	      node->id);
      AddString(errors,
		msg); 
      return;

    }   /* if (error_code == NO_WINDOW) */

  if (attrs.map_state != IsViewable)
    {
      sprintf(msg,
	      "%s(0x%lx) - This widget is not mapped.\n",
	      node->name,
	      node->id);
      AddString(errors,
		msg);
      return;

    }   /* if (widget not viewable) */

  CreateFlashWidget(tree_info,
		    x,
		    y,
		    width,
		    height);

}   /* _AddToFlashList() */


/*	Function Name: CreateFlashWidget
 *	Description: Creates a widget of the size specified that
 *                   will flash on the display, and adds it to the list
 *                   of widgets to flash.
 *	Arguments: tree_info - the tree information structure.
 *                 x,y,width, height - size and location of the flash widget.
 *	Returns: none.
 */
    
#define MORE_FLASH_WIDGETS 5

static void
CreateFlashWidget(TreeInfo *tree_info, int x, int y,
                  unsigned int width, unsigned int height)
{
  Widget    shell;
  Arg       args[3];
  Cardinal  num = 0;
  Dimension bw;


  /* Create an overrideShell covering the area of the widget to flash */
  XtSetArg(args[num],
	   XtNx, x);
  num++;
  XtSetArg(args[num],
	   XtNy, y);
  num++;
  XtSetArg(args[num],
	   XtNbackground, global_resources.flash_color);
  num++;

  shell = XtCreatePopupShell("flash",
			     overrideShellWidgetClass, 
			     tree_info->tree_widget,
			     args, num);

  /* Correct the dimensions by the XtNborderWidth */
  num = 0;
  XtSetArg(args[num],
	   XtNborderWidth, &bw);
  num++;
  XtGetValues(shell,
	      args, num);

  bw *= 2;

  num = 0;
  XtSetArg(args[num],
	   XtNwidth, (width - bw));
  num++;
  XtSetArg(args[num],
	   XtNheight, (height - bw));
  num++;
  XtSetValues(shell,
	      args, num);    

  if (tree_info->num_flash_widgets + 1 > tree_info->alloc_flash_widgets)
    {
      tree_info->alloc_flash_widgets += MORE_FLASH_WIDGETS;
      tree_info->flash_widgets =
	(Widget*)XtRealloc ((char*)tree_info->flash_widgets,
			    sizeof(Widget) * tree_info->alloc_flash_widgets);
    }   /* if (we had to get more memory for the new elements) */

  tree_info->flash_widgets[tree_info->num_flash_widgets] = shell;
  tree_info->num_flash_widgets++;

}   /* CreateFlashWidget() */


/*	Function Name: FlashWidgets
 *	Description: Starts the widgets flashing.
 *	Arguments: tree_info - the info about the tree (contains flash list)
 *	Returns: none
 */
static void
FlashWidgets(TreeInfo *tree_info)
{
  int           i;
  unsigned long wait;
  unsigned long half_flash;
  XtAppContext  ac = XtWidgetToApplicationContext(tree_info->tree_widget);


  if (tree_info->flash_widgets == NULL)   /* no widgets to flash. */
    {
      return;
    }

  wait = half_flash = global_resources.flash_time/2;

  for (i = 1; i < global_resources.num_flashes; i++)
    {
      (void)XtAppAddTimeOut(ac,
			    wait,
			    FlashWidgetsOff,
			    (XtPointer)tree_info);
      wait += half_flash;
      (void)XtAppAddTimeOut(ac,
			    wait,
			    FlashWidgetsOn,
			    (XtPointer)tree_info);
      wait += half_flash;
    }

  wait += half_flash;
  (void)XtAppAddTimeOut(ac,
			wait,
			FlashWidgetsCleanup,
			(XtPointer)tree_info);
  startWait(ac,
	    (wait / 1000));

  FlashWidgetsOn ((XtPointer)tree_info,
		  (XtIntervalId*)NULL);

}   /* FlashWidgets() */

    
/*	Function Name: FlashWidgetsOn
 *	Description: Turns on all the Flash Widgets.
 *	Arguments: info_ptr - pointer to the tree info.
 *                 id - *** UNUSED ***.
 *	Returns: none
 */
/* ARGSUSED */
static void
FlashWidgetsOn(XtPointer info_ptr, XtIntervalId *id)
{
  int      i;
  TreeInfo *tree_info = (TreeInfo *) info_ptr;


  for (i = 0; i < tree_info->num_flash_widgets; i++)
    {
      XtRealizeWidget (tree_info->flash_widgets[i]);
      XMapRaised (XtDisplay(tree_info->flash_widgets[i]),
		  XtWindow(tree_info->flash_widgets[i]));
    }   /* for (all flash_widgets) */

}   /* FlashWidgetsOn() */


/*	Function Name: FlashWidgetsOff
 *	Description: Turns off all the Flash Widgets.
 *	Arguments: info_ptr - pointer to the tree info.
 *                 id - *** UNUSED ***.
 *	Returns: none
 */
/* ARGSUSED */
static void
FlashWidgetsOff(XtPointer info_ptr, XtIntervalId *id)
{
  int      i;
  TreeInfo *tree_info = (TreeInfo *) info_ptr;


  for (i = 0; i < tree_info->num_flash_widgets; i++)
    {
      XtUnmapWidget(tree_info->flash_widgets[i]);
    }   /* for (all flash_widgets) */

}   /* FlashWidgetsOff() */


/*	Function Name: FlashWidgetsCleanup
 *	Description: Destroys all the Flash Widgets.
 *	Arguments: info_ptr - pointer to the tree info.
 *                 id - *** UNUSED ***.
 *	Returns: none
 */
/* ARGSUSED */
static void
FlashWidgetsCleanup(XtPointer info_ptr, XtIntervalId *id)
{
  int      i;
  TreeInfo *tree_info = (TreeInfo *) info_ptr;

  /*
   * Unmap 'em first for consistency.
   */
    
  for (i = 0; i < tree_info->num_flash_widgets; i++)
    {
      XtUnmapWidget(tree_info->flash_widgets[i]);
    }   /* for (all flash_widgets) */

  XFlush(XtDisplay(tree_info->tree_widget));

  for (i = 0; i < tree_info->num_flash_widgets; i++) 
    {
      XtDestroyWidget(tree_info->flash_widgets[i]);
    }   /* for (all flash_widgets) */

  XtFree ((char*)tree_info->flash_widgets);
  tree_info->flash_widgets = NULL;
  tree_info->num_flash_widgets = tree_info->alloc_flash_widgets = 0;

}   /* FlashWidgetsCleanup() */
