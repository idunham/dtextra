/*
 * $Header: /cvsroot-fuse/meditres/meditres/actions.c,v 1.8 2002/01/16 14:35:20 amai Exp $
 *
 * $XConsortium: actions.c,v 1.15 94/04/17 20:38:50 rws Exp $
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
 * Port to Motif and MEDITRES, Danny Backx, 2001.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>     /* Standard cursor font */
#include <X11/Xmu/CharSet.h>

#include "meditres.h"


/*
 * Private data.
 */

struct ActionValues
{
  String name;
  int    type;
};

static struct ActionValues select_values[] =
{
  { "widget",      (int) SelectWidget },
  { "all",         (int) SelectAll },
  { "nothing",     (int) SelectNone },
  { "invert",      (int) SelectInvert },
  { "children",    (int) SelectChildren },
  { "descendants", (int) SelectDescendants },
  { "parent",      (int) SelectParent },
  { "ancestors",   (int) SelectAncestors }
};

static struct  ActionValues label_values[] =
{
  { "name",   (int) NameLabel },
  { "class",  (int) ClassLabel },
  { "id",     (int) IDLabel },
  { "window", (int) WindowLabel },
  { "toggle", (int) ToggleLabel },
  { "view",   (int) ViewLabel }
};

static struct  ActionValues fold_values[] =
{
  { "this",    (int) FoldSelected },
  { "subtree", (int) FoldChildSelected }
};

static struct  ActionValues unfold_values[] =
{
  { "this",     (int) UnfoldSelected },
  { "children", (int) Unfold1Selected },
  { "all",      (int) UnfoldAll }
};

static WNode   *FindTreeNodeFromWidget();
static Boolean CheckAndFindEntry();


/* Function Name:
 *   EnableGetVal
 *
 * Description:
 *   Sets a global variable to notify the Notify action
 *   for the resource list widet to do GetValues.
 *
 * Arguments:
 *   w - any widget in the widget tree.
 *   event - ** NOT USED **
 *   params - ** NOT USED **
 *   num_params - ** NOT USED **
 *
 * Calls:
 *   <none>
 *
 * Global data:
 *   do_get_values
 */

Boolean do_get_values = False;

/* ARGSUSED */
static void
EnableGetVal(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  do_get_values = True;
}   /* EnableGetVal() */


/*	Function Name: SelectAction
 *	Description: 
 *      Arguments: w - any widget in the widget tree.
 *                 event - NOT USED.
 *                 params, num_params - the parameters paseed to the action
 *                                      routine. 
 *
 * params[0] - One of "nothing", "parent", "children", "ancestors",
 *                    "descendants", "invert", "all"
 * num_params - must be 1.
 */
/* ARGSUSED */
static void
SelectAction(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  WNode *node;
  int   type;


  if (!CheckAndFindEntry("Select",
			 params, *num_params, 
			 select_values, XtNumber(select_values),
			 &type))
    {
      return;
    }

  switch(type)
    {
    case SelectAll:
    case SelectNone:
    case SelectInvert:
	_TreeSelect(global_tree_info, type);
	break;

    case SelectWidget:
	_FindWidget(XtParent(w));
	break;

    default:
	node = FindTreeNodeFromWidget(w);
	if (node)
	    _TreeActivateNode(node, type);	
	else
	    _TreeSelect(global_tree_info, type);
	break;

    }   /* switch(type) */

}   /* SelectAction() */


/*	Function Name: RelabelAction
 *	Description: 
 *      Arguments: w - any widget in the widget tree.
 *                 event - NOT USED.
 *                 params, num_params - the parameters paseed to the action
 *                                      routine. 
 *
 * params[0] - One of "name", "class", "id", "win"
 * num_params - must be 1.
 */
/* ARGSUSED */
static void
RelabelAction(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  WNode *node;
  int   type;


  if (!CheckAndFindEntry("Relabel",
			 params, *num_params, 
			 label_values, XtNumber(label_values),
			 &type))

    {
      return;
    }

  if ((node = FindTreeNodeFromWidget(w)) == NULL) 
    {
      _TreeRelabel(global_tree_info, type);
    }

  else
    {
      PrepareToLayoutTree(global_tree_info->tree_widget); 
      _TreeRelabelNode(node, type, FALSE);
      LayoutTree(global_tree_info->tree_widget); 
    }   /* else (node found) */

}   /* RelabelAction() */


/*	Function Name: PopdownFileDialogAction
 *	Description: Pops down the file dialog widget.
 *                   and calls the approipriate handler.
 *	Arguments: w - any child of the dialog widget.
 *                 event - the event that caused this action.
 *                 params, num_params - params passed to the action routine.
 * RETURNED        none.
 *
 * params[0] - One of "cancel" or "okay".
 * num_params - must be 1.
 */
/* ARGSUSED */
static void
PopdownFileDialogAction(w, event, params, num_params)
     Widget   w;
     XEvent   *event;
     String   *params;
     Cardinal *num_params;
{
	fprintf(stderr, "PopdownFileDialogAction\n");
#if 0
  char    buf[BUFSIZ];
  Boolean val;

  if (*num_params != 1)
    {
      /* "Action %s must have exactly one argument" */
      if (res_labels[2])
	{
	  sprintf(buf,
		  res_labels[2], 
		  "PopdownFileDialog");
	}
      else
	{
	  sprintf(buf,
		  "Action %s must have exactly one argument"
		  "PopdownFileDialog");
	}
      SetMessage(global_screen_data.info_label,
		 buf,
		 "?buf");
      return;
    }   /* if (num_params != 1) */

  XmuCopyISOLatin1Lowered(buf, params[0]);

  if (strcmp(buf, "cancel") == 0) {
      val = FALSE;
    }   /* if ("cancel") */

  else if (strcmp(buf, "okay") == 0) {
      val = TRUE;
    }   /* elsif ("okay") */

  else
    {
      /* "Action %s's argument must be either `%s' or `%s'" */
      if (res_labels[1])
	{
	  sprintf(buf,
		  res_labels[1],
		  "PopdownFileDialog",
		  "cancel",
		  "okay");
	}
      else
	{
	  sprintf(buf,
		  "Action %s's argument must be either `cancel' or `okay'",
		  "PopdownFileDialog");
	}
      SetMessage(global_screen_data.info_label,
		 buf,
		 "?buf");
      return;
    }   /* else (NOT "cancel" or "okay") */

    _PopdownFileDialog(w, (XtPointer)val, NULL);
#endif
}   /* PopdownFileDialogAction() */

/*	Function Name: ActionQuit
 *	Description: This function prints a message to stdout.
 *	Arguments: w - ** UNUSED **
 *                 call_data - ** UNUSED **
 *                 client_data - ** UNUSED **
 *	Returns: none
 */
/* ARGSUSED */
static void
ActionQuit(w, event, params, num_params)
     Widget   w;
     XEvent   *event;
     String   *params;
     Cardinal *num_params;
{
#if 0
  if ( (w==global_toplevel)
       || (XtIsSubclass(w, portholeWidgetClass))
       || (w=global_tree_info->tree_widget)
       || (XtIsSubclass(w, toggleWidgetClass)) )
    {
      XtDestroyApplicationContext(XtWidgetToApplicationContext(w));
      exit(EXIT_SUCCESS);
    }

  else
    {
      if (strcmp(XtName(w), RESOURCE_BOX) == 0)	{
	  global_resource_box_up = FALSE;
      }
      XtPopdown(w);
      XtDestroyWidget(w);
    }
#endif
}   /* ActionQuit() */


/* Function Name:
 *   TreeMoveAction
 *
 * Description:
 *
 * Arguments:
 *   w - the tree widget
 *   event - the event that caused this action
 *   params - params passed to the action routine
 *   num_params - number of params
 *
 * params[0] - Could be one of "north", "northeast", "east", "southeast",
 *             "south", "southwest", "west", "northwest", "n", "ne", "e",
 *             "se", "s", "sw", "w", "nw". Mandantory if activated from keyboard.
 * params[1] - Could be a number, which could be followed by a unit "p" (Page),
 *             "x" (Pixel). Optional.
 * num_params - must be 0 if activated by pointer operation,
 *              must be 1 or 2 if activated from keyboard
 *
 * Returns:
 *   <none>
 *
 * Global Data:
 *
 * Calls:
 */
/* ARGSUSED */
void
TreeMoveAction (w, event, params, num_params)
     Widget   w;
     XEvent   *event;
     String   *params;
     Cardinal *num_params;
{
  XButtonEvent    *ev = (XButtonEvent*)event;
  int             diffx;
  int             diffy;
  Arg             args[4];
  int             num_args = 0;
  Dimension       tWidth;
  Dimension       tHeight;
  Dimension       pWidth;
  Dimension       pHeight;
  static Position startx = 0;
  static Position starty = 0;
  static Position posx = 0;
  static Position posy = 0;
  static Position minx = 0;
  static Position miny = 0;
  static Cursor   moveCursor = (XID)0;


  if (event->type == ButtonPress)
    {
      num_args = 0;
      XtSetArg (args[num_args],
		XtNx, &posx);
      num_args++;
      XtSetArg (args[num_args],
		XtNy, &posy);
      num_args++;
      XtSetArg (args[num_args],
		XtNwidth, &tWidth);
      num_args++;
      XtSetArg (args[num_args],
		XtNheight, &tHeight);
      num_args++;
      XtGetValues (w,
		   args, num_args);

      num_args = 0;
      XtSetArg (args[num_args],
		XtNwidth, &pWidth);
      num_args++;
      XtSetArg (args[num_args],
		XtNheight, &pHeight);
      num_args++;
      XtGetValues (XtParent(w),
		   args, num_args);

      minx = tWidth - pWidth;
      miny = tHeight - pHeight;
      startx = ev->x_root;
      starty = ev->y_root;
      
      if (moveCursor == (XID)0)
	{
	  moveCursor = XCreateFontCursor(XtDisplay(w),
					 XC_fleur);
	}

      XDefineCursor(XtDisplay(w),
		    XtWindow(w),
		    moveCursor);
    }   /* if (Button pressed: start moving) */

  else if (event->type == MotionNotify)
    {
      diffx = global_movespeed * (((XMotionEvent*)event)->x_root-startx);
      diffy = global_movespeed * (((XMotionEvent*)event)->y_root-starty);

      num_args = 0;
      if (posx + diffx > 0)
	{
	  startx = startx + posx + diffx;
	}
      else if (posx + diffx < -minx)
	{
          startx = startx + posx + diffx + minx;
	}
      else
	{
	  XtSetArg (args[num_args],
		    XtNx, posx + diffx);
	  num_args++;
	}

      if (posy + diffy > 0)
	{
	  starty = starty + posy + diffy;
	}
      else if (posy + diffy < -miny)
	{
          starty = starty + posy + diffy + miny;
	}
      else
	{
	  XtSetArg (args[num_args],
		    XtNy, posy + diffy);
	  num_args++;
	}

      XtSetValues (w,
		   args, num_args);
    }   /* elsif (ButtonMotion) */

  else if (event->type == ButtonRelease)
    {
      XUndefineCursor(XtDisplay(w),
		      XtWindow(w));
      /* we keep the cursor for the next movement, so we only need to */
      /* create it once */
      /*    XFreeCursor(XtDisplay(w), */
      /* 	        moveCursor); */
    }   /* elsif (ButtonRelease) */

  else
    {
      printf("unknown Event type\n");
    }

}   /* TreeMoveAction() */


/* Function Name:
 *   FlashAction
 *
 * Description:
 *   Flashes nodes: If action is called from a toggleWidget only the
 *   corresponding nodes flashes, otherwise all selected nodes flashes.
 *
 * Arguments:
 *   w - the tree widget or any client widget (toggle button)
 *   event - the event that caused this action
 *   params - params passed to the action routine
 *   num_params - number of params
 *
 * Returns:
 *   <none>
 *
 * Global Data:
 *
 * Calls:
 *   _FlashActiveWidgets - geometry.c
 */
/* ARGSUSED */
void
FlashAction (w, event, params, num_params)
     Widget   w;
     XEvent   *event;
     String   *params;
     Cardinal *num_params;
{
#if 0
  WNode          *node;
  ProtocolStream *stream = &(global_client.stream);


  if (global_tree_info == NULL)
    {
      /* "No Widget/Style tree is available" */
      SetMessage(global_screen_data.info_label,
		 res_labels[17],
		 "No Widget/Style tree is available");
      return;
    }   /* if (no tree_info) */

  if (XtIsSubclass(w, toggleWidgetClass))
    {
      /* flash only the widget named by the toggle */

      if ( (node = FindTreeNodeFromWidget(w)) == NULL)
	{
	  fprintf(stderr,
		  "internal Error: cannot find node of this toggleWidget\n");
	}
      else
	{
	  _XEditResResetStream(stream); 
	  /*
	   * flash only one widget
	   */
	  _XEditResPut16(stream,
			 (unsigned short)1);

	  InsertWidgetFromNode(stream,
			       node);

	  SetCommand(global_tree_info->tree_widget,
		     LocalFlashWidget,
		     NULL);
	}   /* else (node of toggleWidget found) */

    }   /* if (Action from ToggleWidget) */

  else
    {
      _FlashActiveWidgets(global_tree_info);
    }   /* else (not activated from ToggleWidget) */
#endif      
}   /* FlashAction() */


/* Function Name:
 *   SendTreeAction
 *
 * Description:
 *   Ask a new client for it's widget tree.
 *
 * Arguments:
 *   w - the tree widget, the parent (porthole) or any client widget (toggle)
 *   event - the event that caused this action
 *   params - params passed to the action routine:
 *            "new": ask a new client for it's widget tree
 *            "refresh": refresh current widget tree
 *   num_params - number of params
 *
 * params[0] - One of "new" or "refresh".
 * num_params - must be 1.
 *
 * Returns:
 *   <none>
 *
 * Global Data:
 *
 * Calls:
 */
/* ARGSUSED */
void
SendTreeAction(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  char    buf[BUFSIZ];
  Boolean val;

  fprintf(stderr, "SendTreeAction\n");

  if (*num_params != 1)
    {
      /* "Action %s must have exactly one argument" */
      if (res_labels[2])
	{
	  sprintf(buf,
		  res_labels[2], 
		  "SendTreeAction");
	}
      else
	{
	  sprintf(buf,
		  "Action %s must have exactly one argument",
		  "SendTreeAction");
	}
      SetMessage(global_screen_data.info_label,
		 buf,
		 "?buf");
      return;
    }   /* if (num_params != 1) */

  XmuCopyISOLatin1Lowered(buf, params[0]);

  if (strcmp(buf, "new") == 0)
    {
      val = TRUE;
    }   /* if (get tree from new client) */

  else if (strcmp(buf, "refresh") == 0)
    {
      val = FALSE;
    }   /* elsif (refresh current tree) */

  else
    {
      /* "Action %s's argument must be either `%s' or `%s'" */
      if (res_labels[1])
	{
	  sprintf(buf,
		  res_labels[1],
		  "SendTreeAction",
		  "new",
		  "refresh");
	}
      else
	{
	  sprintf(buf,
		  "Action %s's argument must be either `new' or `refresh'",
		  "SendTreeAction");
	}
      SetMessage(global_screen_data.info_label,
		 buf,
		 "?buf");
      return;
    }   /* else (NOT "new" or "refresh") */

  SendTree (w, (XtPointer)val, NULL);
}   /* SendTreeAction() */


/* Function Name:
 *   DumpTreeAction
 *
 * Description:
 *   Dump tree as intended list to text file.
 *
 * Arguments:
 *   w - the tree widget, the parent (porthole) or any client widget (toggle)
 *   event - the event that caused this action
 *   params - ** NOT USED **
 *   num_params - ** NOT USED **
 *
 * Returns:
 *   <none>
 *
 * Global Data:
 *
 * Calls:
 *   DumpTreeToFile - handler.c
 */
/* ARGSUSED */
void
DumpTreeAction (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  DumpTreeToFile (w,
		  NULL,
		  NULL);

}   /* DumpTreeAction() */


/* Function Name:
 *   PrintAction
 *
 * Description:
 *
 * Arguments:
 *   w - the tree widget, the parent (porthole) or any client widget (toggle)
 *   event - the event that caused this action
 *   params - ** NOT USED **
 *   num_params - ** NOT USED **
 *
 * Returns:
 *   <none>
 *
 * Global Data:
 *
 * Calls:
 *   printTree - print.c
 */
/* ARGSUSED */
void
PrintAction (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  printTree (w, 
   	     NULL,
   	     NULL);
  
}   /* DumpTreeAction() */


/* Function Name:
 *   ResBoxAction
 *
 * Description:
 *
 * Arguments:
 *   w - the tree widget, the parent (porthole) or any client widget (toggle)
 *   event - the event that caused this action
 *   params - ** NOT USED **
 *   num_params - ** NOT USED **
 *
 * Returns:
 *   <none>
 *
 * Calls:
 *   GetResourceList -
 *
 * Global Data:
 */
/* ARGSUSED */
void
ResBoxAction (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  /* don't know why clientData = FALSE ? */
  GetResourceList (w,
		  (XtPointer)FALSE,
		  NULL);

}   /* ResBoxAction() */


/* Function Name:
 *   FoldAction
 *
 * Description:
 *   If widget is a toggle widget fold only this node,
 *   else fold the selected nodes. Parameter "this" is FoldSelected,
 *   "subtree" is FoldChildSelected.
 *
 * Arguments:
 *   w - any widget in the widget tree.
 *   event - ** NOT USED **
 *   params, num_params - the parameters paseed to the action routine. 
 *
 * params[0] - One of "this" (default), "subtree"
 * num_params - must be 1 or 0.
 *
 * Returns:
 *
 * Calls:
 *   treeFold - handler.c
 *
 * Global data:
 */
/* ARGSUSED */
static void
FoldAction(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
#if 0
  WNode *node;
  int   type;
  int   i;


  /* if no parameter is given, set default */
  if (*num_params < 1)
    {
      type = (int)FoldSelected;
    }   /* if (no parameter given) */

  else
    {
      if (!CheckAndFindEntry("Fold",
			     params, *num_params, 
			     fold_values, XtNumber(fold_values),
			     &type))
	{
	  return;
	}   /* if (unknown parameter) */

    }   /* else (analyse parameter) */

  if (XtIsSubclass(w, toggleWidgetClass))
    {
      /* fold only the widget named by the toggle */
      if ( (node = FindTreeNodeFromWidget(w)) == NULL)
	{
	  fprintf(stderr,
		  "internal Error: cannot find node of this toggleWidget\n");
	}
      else
	{
	  /* closes the print dialog */
	  if (global_screen_data.tree_dialog)
	    {
	      XtPopdown(global_screen_data.tree_dialog);
	    }

	  XtUnmapWidget (global_tree_info->tree_widget);

	  /* special treatment for FoldTypeSelected */
	  if (type = (int)FoldChildSelected)
	    {
	      for (i=0; i < node->num_children; i++)
		{
		  _treeFold(node->children[i],
			    FoldSelected);
		}   /* for (all children of this node) */
	    }   /* if (FoldTypeSelected) */
	  else
	    {
	      _treeFold(node,
			(FoldTypes)type);
	    }   /* else (not FoldTypeSelected) */

	  SetShownWidLabel (global_tree_info->num_shown);
	  _TreeRelabel (global_tree_info,
			global_tree_info->viewing);   /* includes LayoutTree() */

	}   /* else (node found) */

    }   /* if (toggleWidget) */

  else
    {
      treeFold(w,
	       (XtPointer)type,
	       NULL);
    }   /* else (not activated from toggleWidget) */
#endif  
}   /* FoldAction() */


/* Function Name:
 *   UnfoldAction
 *
 * Description:
 *
 * Arguments:
 *   w - any widget in the widget tree.
 *   event - ** NOT USED **
 *   params, num_params - the parameters paseed to the action routine. 
 *
 * params[0] - One of "this", "children" (default), "all"
 * num_params - must be 1 or 0.
 */
/* ARGSUSED */
static void
UnfoldAction(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
#if 0
  WNode *node;
  int   type;
  int   i;


  /* if no parameter is given, set default */
  if (*num_params < 1)
    {
      type = (int)Unfold1Selected;
    }   /* if (no parameter given) */

  else
    {
      if (!CheckAndFindEntry("Unfold",
			     params, *num_params, 
			     unfold_values, XtNumber(unfold_values),
			     &type))
	{
	  return;
	}   /* if (unknown parameter) */

    }   /* else (analyse parameter) */

  if (XtIsSubclass(w, toggleWidgetClass))
    {
      /* fold only the widget named by the toggle */

      if ( (node = FindTreeNodeFromWidget(w)) == NULL)
	{
	  fprintf(stderr,
		  "internal Error: cannot find node of this toggleWidget\n");
	}
      else
	{
	  /* closes the print dialog */
	  if (global_screen_data.tree_dialog)
	    {
	      XtPopdown(global_screen_data.tree_dialog);
	    }

	  XtUnmapWidget (global_tree_info->tree_widget);

	  /* special treatment for Unfold1Selected */
	  if (type = (int)Unfold1Selected)
	    {
	      if ((node) && (node->view != FirstShown))
		{
		  node->view = Shown;
		  node->childrenFolded = FALSE;
		}   /* if (node != FirstShown) */

	      for (i=0; i < node->num_children; i++)
		{
		  _treeFold(node->children[i],
			    Unfold1Selected);
		}   /* for (all children of this node) */

	      AddTreeNode (global_tree_info->tree_widget,
			   node);

	    }   /* if (Unfold1Selected) */
	  else
	    {
	      _treeFold(node,
			(FoldTypes)type);
	    }   /* else (not Unfold1Selected) */

	  SetShownWidLabel (global_tree_info->num_shown);
	  _TreeRelabel (global_tree_info,
			global_tree_info->viewing);   /* includes LayoutTree() */

	}   /* else (node found) */

    }   /* if (toggleWidget) */

  else
    {
      treeFold(w,
	       (XtPointer)type,
	       NULL);
    }   /* else (not activated from toggleWidget) */
#endif  
}   /* UnfoldAction() */


/* Function Name:
 *   NewTopAction
 *
 * Description:
 *
 * Arguments:
 *   w - the tree widget, the parent (porthole) or any client widget (toggle)
 *   event - the event that caused this action
 *   params - ** NOT USED **
 *   num_params - ** NOT USED **
 *
 * Returns:
 *   <none>
 *
 * Global Data:
 *
 * Calls:
 *   newTop - handler.c
 */
/* ARGSUSED */
void
NewTopAction (w, event, params, num_params)
     Widget   w;
     XEvent   *event;
     String   *params;
     Cardinal *num_params;
{
#if 1
  WNode *node;


#if 0
  if (XtIsSubclass(w, toggleWidgetClass))
#else
  if (1)
#endif
    {
      /* new top is the widget named by the toggle */
      if ( (node = FindTreeNodeFromWidget(w)) == NULL)
	{
	  fprintf(stderr,
		  "internal Error: cannot find node of this toggleWidget\n");
	}
      else
	{
	  global_tree_info->newtop_node->view = NotShownBefore;
	  node->view = FirstShown;

	  _newTopSub (global_tree_info->newtop_node);
	  displayNewTree();
	}   /* else (node found) */

    }   /* if (toggleWidget) */

  else
    {
      /* new top is the selected toggle */
      newTop (w,
	      (XtPointer)NewSelected,
	      NULL);
    }   /* else (not toggleWidget) */
#endif  
}   /* NewTopAction() */


/* Function Name:
 *   RestoreParentAction
 *
 * Description:
 *
 * Arguments:
 *   w - any widget in the widget tree.
 *   event - NOT USED.
 *   params, num_params - the parameters paseed to the action routine. 
 *
 * params[0] - either "root" or <none>
 * num_params - must be 1 or 0.
 */
/* ARGSUSED */
static void
RestoreParentAction(w, event, params, num_params)
     Widget   w;
     XEvent   *event;
     String   *params;
     Cardinal *num_params;
{
  char    buf[BUFSIZ];


  if (*num_params == 0)
    {
      newTop (w,
	      (XtPointer)NewParent,
	      NULL);
    }   /* if (no params: restore parent) */

  else
    {
      XmuCopyISOLatin1Lowered(buf, params[0]);

      if (strcmp(buf, "root") == 0)
	{
	  newTop (w,
		  (XtPointer)NewRoot,
		  NULL);
	}   /* if (restore root) */

      else
	{
	  /* "Action %s's argument must be either `%s' or `%s'" */
	  if (res_labels[1])
	    {
	      sprintf(buf,
		      res_labels[1],
		      "RestoreParent",
		      "",
		      "root");
	    }
	  else
	    {
	      sprintf(buf,
		      "Action %s's argument must be either `' or `root'"
		      "RestoreParent", buf);
	    }
	  SetMessage(global_screen_data.info_label,
		     buf,
		     "?buf");
	}   /* else (NOT "root") */

    }   /* else (parameter exists) */

}   /* RestoreParentAction() */


/* Function Name:
 *   ShowAllAction
 *
 * Description:
 *   Show the whole tree.
 *
 * Arguments:
 *   w - the tree widget, the parent (porthole) or any client widget (toggle)
 *   event - the event that caused this action
 *   params - ** NOT USED **
 *   num_params - ** NOT USED **
 *
 * Returns:
 *   <none>
 *
 * Global Data:
 *
 * Calls:
 *   ShowAll - handler.c
 */
/* ARGSUSED */
void
ShowAllAction (w, event, params, num_params)
     Widget   w;
     XEvent   *event;
     String   *params;
     Cardinal *num_params;
{
  showAll (w,
	   NULL,
	   NULL);

}   /* ShowAllAction() */


/* Function Name:
 *   SetApplicationActions
 *
 * Description:
 *   Sets my application actions.
 *
 * Arguments:
 *   app_con - the application context.
 *
 * Returns:
 *   <none>
 *
 * Calls:
 *
 * Global data:
 */

static XtActionsRec actions[] =
{
  {"EnableGetVal",      EnableGetVal},
  {"Select",            SelectAction},
  {"SVActiveEntry",     ModifySVEntry},
  {"SearchPopup",       SearchPopup},
  {"FindEntry",         FindEntry},
  {"Relabel",      	RelabelAction}, 
  {"PopdownFileDialog", PopdownFileDialogAction},
  {"TreeMove",          TreeMoveAction},
  {"Flash",             FlashAction},
  {"SendTree",          SendTreeAction},
  {"DumpTree",          DumpTreeAction},
  {"Print",             PrintAction},
  {"ResBox",            ResBoxAction},
  {"Fold",              FoldAction},
  {"Unfold",            UnfoldAction},
  {"NewTop",            NewTopAction},
  {"RestoreParent",     RestoreParentAction},
  {"ShowAll",           ShowAllAction},
  {"quit",              ActionQuit}
};

void
SetApplicationActions(XtAppContext app_con)
{
  XtAppAddActions(app_con,
		  actions, XtNumber(actions));
}   /* SetApplicationActions() */


/************************************************************
 *
 * Private functions	
 *
 ************************************************************/

/*	Function Name: CheckAndFindEntry
 *	Description: Checks the args to make sure they are valid,
 *                   then parses the arg list to find the correct action
 *                   to take.
 *	Arguments: action_name - name of the action (for error messages).
 *                 params, num_params - params passed to the action routine.
 *                 table, num_table - table to check the parameters against.
 * RETURNED        type - info about the action to take.
 *	Returns: TRUE if the arguments are okay.
 */
static Boolean
CheckAndFindEntry(action_name, params, num_params, table, num_table, type)
     String              *params;
     String              action_name;
     Cardinal            num_params;
     Cardinal            num_table;
     struct ActionValues *table;
     int                 *type;
{
  char buf[BUFSIZ];
  int  i;


  if (num_params != 1)
    {
      /* "Action %s must have exactly one argument" */
      if (res_labels[2])
	{
	  sprintf(buf,
		  res_labels[2], 
		  action_name);
	}
      else
	{
	  sprintf(buf,
		  "Action %s must have exactly one argument",
		  action_name);
	}
      SetMessage(global_screen_data.info_label,
		 buf,
		 "?buf");
      return(FALSE);
    }   /* if (num_params != 1) */
	
  XmuCopyISOLatin1Lowered(buf, params[0]);
  for ( i = 0 ; i < num_table; i++ ) 
    {
      if (strcmp(buf, table[i].name) == 0)
	{
	  *type = table[i].type;
	  return(TRUE);
	}
    }   /* for */

  /* "Unknown parameter to action %s must be one of:\n" */
  if (res_labels[3])
    {
      sprintf(buf,
	      res_labels[3], 
	      action_name);
    }
  else
    {
      sprintf(buf,
	      "Unknown parameter to action %s must be one of:\n",
	      action_name);
    }

  for (i = 0; i < num_table; )
    {
      strcat(buf, table[i++].name);
	
      if (i == (num_table - 1))
	{
	  strcat(buf, ", or ");
	}

      else if (i < num_table)
	{
	  strcat(buf, ", ");
	}

    }   /* for (all possible parameter of <action_name>) */
    
  SetMessage(global_screen_data.info_label,
	     buf,
	     "?buf");
  return(FALSE);
}   /* CheckAndFindEntry() */


/*	Function Name: FindTreeNodeFromWidget
 *	Description: finds the tree node associated with a widget.
 *	Arguments: w - widget to check.
 *	Returns: the node associated with this widget, or NULL.
 */
static WNode *
FindTreeNodeFromWidget(w)
     Widget w;
{
  int      ret_val;
  XPointer data_return;


  /*
   * Yes, I really am casting a widget to a window  ** TRUST ME ***
   */

  ret_val = XFindContext(XtDisplay(w),
			 (Window)w,
			 NODE_INFO,
			 &data_return);

  if (ret_val == 0)
    {
      return((WNode*)data_return);
    }

  return(NULL);

}   /* FindTreeNodeFromWidget() */

