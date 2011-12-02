/*
 * $Header: /cvsroot-fuse/meditres/meditres/wtree.c,v 1.19 2002/01/16 20:05:43 amai Exp $
 *
 * $XConsortium: wtree.c,v 1.18 94/04/17 20:38:59 rws Exp $
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
 * Motif port, MEDITRES project, © 2001, 2002 by Danny Backx
 */

 
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <X11/StringDefs.h>

#include <Xm/Xm.h>
#include <Xm/Container.h>
#include <Xm/IconG.h>

#include "meditres.h"


/* left-over from Xaw code ... */
#ifndef ONE
#define ONE	((Cardinal)1)
#endif


/* >>>>> HACK for batchres */
static void LoadNewResFile (XtPointer *, XtIntervalId);
static Boolean isodigit(int c);
Boolean iswhitespace (int c);
/* <<<<< HACK for batchres */


static void   AddChild (WNode *parent, WNode *child);
static void   FillNode (WidgetTreeInfo *info, WNode *node, TreeInfo *tree_info);
static void   AddNode(WNode **top_node, WidgetTreeInfo *info, TreeInfo *tree_info);
static void   AddNodeToActiveList (WNode *node);
static void   fillSubTree (WNode *node, ViewTypes type);

/*
 * internal function declaration.
 */

void
no_printf(const char *format, ...)
{
}   /* no_printf() */


/* Function Name:
 *   BuildVisualTree
 *
 * Description:
 *   Creates the Tree and shows it.
 *
 * Arguments:
 *   tree_parent - parent of the tree widget.
 *   event       - the event that caused this action.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   CreateTree - (self)
 *   fillDepth - (self)
 *   AddTreeNode - (self)
 *   _TreeRelabe - (self)
 *   SetMessage - utils.c
 *
 * Global variables:
 *   global_tree_info
 *   global_screen_data.info_label
 ******************************************************************/
/* ARGSUSED */
void
BuildVisualTree(Widget tree_parent, Event *event)
{
	WNode		*top;
	char		msg[BUFSIZ];

LOG "BuildVisualTree\n" END
	/* closes the print dialog */
	if (global_screen_data.tree_dialog) {
		XtPopdown(global_screen_data.tree_dialog);
	}

	if (global_tree_info != NULL) {
		WidgetTreeRemoveChildren(global_tree_info->tree_widget, NULL);
		XtFree ((char *) global_tree_info->active_nodes);
		XtFree((char *) global_tree_info);
	}

	global_tree_info = CreateTree(event);
	top = global_tree_info->top_node;

	/* filling the fields "num_parents" and "view" in nodes (recursive) */
	/* the top node has 0 parents */
#if 0
	fillDepth (global_tree_info->top_node, /* top node */
		0,                          /* number of parents */
		TRUE,                       /* view subtree */
		FALSE);                     /* member of subtree to show */
#endif
	global_tree_info->tree_widget = global_tree_parent;

	if (top == NULL) {
		/* "There is no widget/style tree to display" */
		SetMessage(global_screen_data.info_label,
			res_labels[27],
			"There is no widget/style tree to display");
		return;
	}

	global_tree_info->num_shown = 0;
	AddTreeNode(global_tree_info->tree_widget, top);
	SetShownWidLabel(global_tree_info->num_shown);
#if 0
	if (XtIsRealized(tree_parent)) {	/* hack around problems in Xt. */
		XtRealizeWidget (global_tree_info->tree_widget);
	}
#endif
//	XtManageChild (global_tree_info->tree_widget);

	_TreeRelabel (global_tree_info, NameLabel);
	global_tree_info->viewing = NameLabel;

#if 0
	/* "Widget/Style tree for client %s(%s)" */
	if (res_labels[12]) {
		sprintf (msg, res_labels[12], top->name, top->class);
	} else {
		sprintf(msg, "Widget/Style tree for client %s(%s)",
			top->name, top->class);
	}
	SetMessage (global_screen_data.info_label, msg, "?res_labels[12]");

	/* >>>>> HACK for batchres */
	if (global_resfile_buffer != NULL) {
		printf("\nLoad new resource file \"%s\" into client\n",
			global_resources.resFile);

		/* start to load the resource file after
		 * all current events are processed */
		XtAppAddTimeOut(XtWidgetToApplicationContext(tree_parent),
			100, LoadNewResFile,
			(XtPointer)global_resfile_buffer);
	}
	/* <<<<< HACK for batchres */
#endif
}   /* BuildVisualTree() */


/* Function Name:
 *   AddTreeNode
 *
 * Description:
 *   Adds all nodes below this to the Tree widget.
 *
 * Arguments:
 *   parent - parent of the tree widget.
 *   top - the top node of the tree.
 *
 * Returns:
 *   the tree widget.
 *
 * Calls:
 *   AddTreeNode - (recursive)
 *   SetMessage - utils.c
 *
 * Callbacks installed:
 *   TreeToggle - (self)
 *
 * NOTE: This is a recursive function.
 */
void
AddTreeNode(Widget tree, WNode *top)
{
  int      i;
  Arg      al[5];
  Cardinal ac;
  char     msg[BUFSIZ];
  char     ch;

#if 0
  if ( (top->view != NotShownAfter) && (top->view != NotShownBeside) ) {
      /* we cannot abort, because there are children to display */
      if (top->view != NotShownBefore) {
#endif

	global_tree_info->num_shown++;
	ac = 0;
	if (top->parent) {
		XtSetArg(al[ac], XmNentryParent, top->parent->widget); ac++;
	}

	XtSetArg(al[ac], XmNoutlineState, XmEXPANDED); ac++;
	XtSetArg(al[ac], XmNuserData, (XtPointer)top->id); ac++;
	top->widget = XmCreateIconGadget(tree, top->name, al, ac);
	XtManageChild(top->widget);

	for (i = 0; i < top->num_children; i++) {
		AddTreeNode (tree, top->children[i]);
	}
}   /* AddTreeNode() */

/* Function Name:
 *   TreeToggle
 *
 * Description:
 *   Callback: Called whenever a tree node is toggled.
 *
 * Arguments:
 *   w - the tree widget.
 *   node_ptr - pointer to this node's information.
 *   state_ptr - state of the toggle.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   AddNodeToActiveList - (self)
 *   RemoveNodeFromActiveList - (self)
 *   SetSelWidLabel - widgets.c
 */
/* ARGSUSED */
void
TreeToggle (w, node_ptr, state_ptr)
     Widget    w;
     XtPointer node_ptr;
     XtPointer state_ptr;
{
#if 0
  Boolean state;
  WNode   *node = (WNode*)node_ptr;


LOG "TreeToggle: %s\n", node->name  END
  /* The original version of Xaw defines the callback data "state_ptr"
   * as an (Boolean), asuming that sizeof(Boolean) <= sizeof(XtPointer).
   * But some versions of Xaw3d "corrected" this as (Boolean*).
   */
  if ( ((Boolean)state_ptr == FALSE) || ((Boolean)state_ptr == TRUE) )
    {
      /* original version style */
      state = (Boolean)state_ptr;
    }
  else
    {
      /* unofficial version style */
      /* if "state_ptr" isn't \000 (FALSE) or \001 (TRUE) it must be a pointer */
      state = *(Boolean*)state_ptr;
    }

  if (state)
    {
      AddNodeToActiveList (node);
    }
  else
    {
      RemoveNodeFromActiveList (node);
    }

  /* update number of selected widgets */
  SetSelWidLabel (global_tree_info->num_active);
#endif
}   /* TreeToggle() */


/* Function Name:
 *   AddNodeToActiveList
 *
 * Description:
 *   Adds this node to the list of active toggles.
 *
 * Arguments:
 *   node - node to add.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   IsActiveNode - (self)
 */
static void
AddNodeToActiveList (WNode *node)
{
  TreeInfo *info = node->tree_info;


  if ( (notShown(node))
       || (IsActiveNode (node)) )
    {
      return;
    }	/* if (node not shown or already active) */

  if (info->num_active >= info->alloc_active)
    {
      info->alloc_active += NUM_INC;
      info->active_nodes = (WNode **)XtRealloc ((XtPointer) info->active_nodes,
						sizeof (WNode *) *
						info->alloc_active);
    }

  info->active_nodes[info->num_active++] = node;

}   /* AddNodeToActiveList()  */


/* Function Name:
 *   RemoveNodeFromActiveList
 *
 * Description:
 *   Removes a node from the active list.
 *
 * Arguments:
 *   node - node to remove.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   IsActiveNode - (self)
 */
void
RemoveNodeFromActiveList (WNode *node)
{
  TreeInfo *info      = node->tree_info;
  Boolean  found_node = FALSE;
  int      i;


  if (!IsActiveNode (node))
    {
      return;
    }	/* if (This node is not active) */

  for (i = 0; i < info->num_active; i++)
    {
      if (found_node)
	{
	  /* remove found node by copying following nodes 1 back */
	  info->active_nodes[i-1] = info->active_nodes[i];
	}   /* if (node already found) */

      else if (info->active_nodes[i] == node)
	{
	  found_node = TRUE;
	}   /* elsif (node to remove found) */

      /* else */
	/* {} */

    }   /* for (all nodes) */

  info->num_active--;

}   /* RemoveNodeFromActiveList() */


/* Function Name:
 *   IsActiveNode
 *
 * Description:
 *   returns TRUE is this node is on the active list.
 *
 * Arguments:
 *   node - node to check.
 *
 * Returns:
 *   see above.
 *
 * Calls:
 *   None!
 */
Boolean
IsActiveNode (node)
     WNode *node;
{
  TreeInfo *info = node->tree_info;
  int      i;


  for (i = 0; i < info->num_active; i++)
    {
      if (info->active_nodes[i] == node)
	{
	  return (TRUE);
	}   /* if (node found) */

    }   /* for (all nodes) */

  return (FALSE);
}   /* IsActiveNode() */


/* Function Name:
 *   CreateTree
 *
 * Description:
 *   Creates a widget tree give a list of names and classes.
 *
 * Arguments:
 *   event - the information from the client.
 *
 * Returns:
 *   The tree_info about this new tree.
 *
 * Calls:
 *   AddNode - (self)
 */
TreeInfo *
CreateTree(Event *event)
{
	SendWidgetTreeEvent *send_event = (SendWidgetTreeEvent*)event;
	int                 i;

	TreeInfo            *tree_info;

	tree_info = (TreeInfo*)XtMalloc ((Cardinal)sizeof (TreeInfo));

	tree_info->tree_widget = NULL;
	tree_info->top_node = NULL;
	tree_info->active_nodes = NULL;
	tree_info->num_active = 0;
	tree_info->alloc_active = 0;
	tree_info->flash_widgets = NULL;
	tree_info->num_flash_widgets = tree_info->alloc_flash_widgets = 0;

	for (i = 0; i < (int) send_event->num_entries; i++) {
		AddNode (&(tree_info->top_node),
			(send_event->info + i),
			tree_info);
	}   /* for (number of nodes from SendWidgetTreeEvent) */

	tree_info->num_nodes = (int)send_event->num_entries;
	SetNumWidLabel (tree_info->num_nodes);
	SetSelWidLabel (tree_info->num_active);
	tree_info->newtop_node = tree_info->top_node;
	tree_info->newtop_depth = 0;

	return (tree_info);
}   /* CreateTree() */


/* Function Name:
 *   fillDepth
 *
 * Description:
 *   Calculating the depth of each node (number of parents)
 *   and setting the view status.
 *
 * Arguments:
 *   node - the node we are looking at
 *   depth - the number of parents for this node
 *   viewSub - show subtree of this node?
 *   childOfNew - member of subtree to show?
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   fillDepth - (recursive)
 */
void
fillDepth (node, depth, viewSub, childOfNew)
     WNode    *node;
     Cardinal depth;
     Boolean  viewSub;
     Boolean  childOfNew;
{
  WNode *parent;
  int   i;


LOG "fillDepth: %s\n", node->name  END
  node->num_parents = depth;
  node->childrenFolded = FALSE;
  if (viewSub)
    {
      if (depth < global_tree_info->newtop_depth)
	{
	  /* this could be NotShownBefore, but we correct the path from */
	  /* FirstShown to root later with NotShownBefore */
	  node->view = NotShownBeside;
	}

      else if (node == global_tree_info->newtop_node)
	/* test this before depth == newtop_depth! */
        {
          node->view = FirstShown;
	  childOfNew = TRUE;

	  parent = node->parent;
	  while (parent != NULL)
	    {
	      parent->view = NotShownBefore;
	      parent = parent->parent;
	    }   /* while (path to root: all parents) */
        }

      else if ( (global_maxdepth > 0)
		&& (depth == global_tree_info->newtop_depth + global_maxdepth) )
        {
          node->view = LastShown;
	  viewSub = FALSE;
        }

      else if (node->num_children == 0)
        {
          node->view = LastShown;
	  viewSub = FALSE;
        }

      else if (depth == global_tree_info->newtop_depth)
	/* this can't be the new top! */
	{
	  node->view = NotShownBeside;
	  viewSub = FALSE;
	  childOfNew = FALSE;
	}

      else
        {
          node->view = Shown;
        }

      /* if global_maxdepth == 0 and FirstShown:  */
      if (depth >= global_tree_info->newtop_depth + global_maxdepth)
        {
          viewSub = FALSE;
        }

    }   /* if (view member of this subtree) */

  else
    {
      if (childOfNew)
        {
          node->view = NotShownAfter;
	  if (node->parent)
	    {
	      node->parent->childrenFolded = TRUE;
	    }
        }   /* if (a parent is new top node to show)*/
      else
        {
	  node->view = NotShownBeside;
        }
    }   /* else (do not show member of this subtree) */

  depth++;
     
  i = node->num_children;
  while (i-- > 0)
    {
      if ( (global_maxchilds > 0)
	   && (node->num_children - i > global_maxchilds) )
	{
	  viewSub = FALSE;
	}

      fillDepth (node->children[i],
		 depth,
		 viewSub,
		 childOfNew);

    }   /* while (there are children) */
}   /* fillDepth() */


/* Function Name:
 *   PrintNodes
 *
 * Description:
 *   Prints all nodes.
 *
 * Arguments:
 *   top - the top node.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   PrintNodes - (recursive)
 */
void
PrintNodes (top)
     WNode *top;
{
  int i;


  if (top->parent == NULL)
    {
      printf ("Top of Tree, Name: %10s, ID: %10ld, Class: %10s\n",
	      top->name,
	      top->id,
	      top->class);
    }   /* if (top of tree) */

  else
    {
      printf ("Parent %10s, Name: %10s, ID: %10ld, Class: %10s\n",
	      top->parent->name,
	      top->name,
	      top->id,
	      top->class);
    }   /* else (not top of tree) */

  for (i = 0; i < top->num_children; i++)
    {
      PrintNodes (top->children[i]);
    }   /* for (all children of parameter top) */

}   /* PrintNodes() */


/* Function Name:
 *   _TreeRelabel
 *
 * Description:
 *   Modifies the selected elements of the tree
 *
 * Arguments:
 *   tree_info - the tree we are working on.
 *   type - type of selection to perform
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   SetMessage - utils.c
 *   PrepareToLayoutTree - widgets.c
 *   LayoutTree - widgets.c
 *   _TreeRelabelNode - (self)
 */
void
_TreeRelabel (TreeInfo *tree_info, LabelTypes type)
{
  WNode *top;


  if (tree_info == NULL)
    {
      /* "No widget/style tree is available" */
      SetMessage (global_screen_data.info_label,
		  res_labels[17],
		  "No widget/style tree is available");
      return;
    }   /* if (error: no tree_info) */

  top = tree_info->top_node;

  PrepareToLayoutTree (tree_info->tree_widget);
  _TreeRelabelNode (top,
		    type,
		    TRUE);
  LayoutTree (tree_info->tree_widget);

  global_tree_info->viewing = type;

}   /* _TreeRelabel() */


/* Function Name:
 *   _TreeSelect
 *
 * Description:
 *   Activates relatives of the active nodes, as specified
 *    by type, or Selects all nodes as specified by type.
 *
 * Arguments:
 *   tree_info - information about the tree to work on.
 *   type - type of activate to invode.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   SetMessage - utils.c
 *   _TreeSelectNode - (self)
 *   CopyActiveNodes - (self)
 *   _TreeActivateNode - (self)
 */
void
_TreeSelect (TreeInfo *tree_info, SelectTypes type)
{
  WNode    **active_nodes;
  Cardinal num_active_nodes;
  int      i;


  if (tree_info == NULL)
    {
      /* "No widget/style tree is available" */
      SetMessage (global_screen_data.info_label,
		  res_labels[17],
		  "No widget/style tree is available");
      return;
    }   /* if (error: no tree_info) */

  switch (type)
    {
    case SelectNone:
    case SelectAll:
    case SelectInvert:
      _TreeSelectNode (tree_info->top_node,
		       type,
		       TRUE);
      return;

    default:
      break;			/* otherwise continue. */
    }   /* switch() */

  if (tree_info->num_active == 0)
    {
      /* "There are no active nodes" */
      SetMessage (global_screen_data.info_label,
		  res_labels[18],
		  "There are no active nodes");
      return;
    }

  active_nodes = CopyActiveNodes (tree_info);
  num_active_nodes = tree_info->num_active;

  for (i = 0; i < num_active_nodes; i++)
    {
      _TreeActivateNode (active_nodes[i],
			 type);
    }   /* for (all active nodes) */

  XtFree ((XtPointer) active_nodes);

}   /* _TreeSelect() */


/* Function Name:
 *   _TreeSelectNode
 *
 * Description:
 *   Modifies the state of a node and all its decendants.
 *
 * Arguments:
 *   node - node to operate on.
 *   type - type of selection to perform.
 *   recurse - whether to continue on down the tree.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   SetMessage - utils.c
 *   notShown - (self)
 *   TreeToggle - (self)
 *   RemoveNodeFromActiveList - (self)
 *   _TreeSelectNode - (recursive)
 */
void
_TreeSelectNode(WNode *node, SelectTypes type, Boolean recurse)
{
  int     i;
  Arg     args[1];
  Boolean state;

LOG "TreeSelectNode: %s ", node->name  END
  /* if this node and its subtree is not shown and type isn't SelectNone, */
  /* we can return */
  if ( (node->widget == NULL) || (notShown(node))) {
LOG "-\n"  END
      if (type == SelectNone)
	{
	  /* no toggle to change state, but RemoveNodeFromActiveList */
	  RemoveNodeFromActiveList (node);
	}

      if ( (recurse) && (node->view == NotShownBefore) )
	{
	  for (i = 0; i < node->num_children; i++)
	    {
	      _TreeSelectNode (node->children[i], type, recurse);
	    }   /* for (all children of this node) */
	}

      return;
  }

  switch (type) {
    case SelectAll:
LOG "SelectAll\n"  END
      state = TRUE;
      break;

    case SelectNone:
LOG "SelectNone\n"  END
      state = FALSE;
      break;

    case SelectInvert:
LOG "SelectInvert\n"  END
#if 0
      XtSetArg (args[0],
		XtNstate, &state);
#endif
      XtGetValues (node->widget,
		   args, ONE);

      state = !state;
      break;

    default:
      /* "Editres Internal Error: Unknown select type" */
      SetMessage (global_screen_data.info_label,
		  res_labels[31],
		  "Editres Internal Error: Unknown select type");
      return;
  }   /* switch() */

#if 0
  XtSetArg(args[0], XtNstate, state);
  XtSetValues (node->widget, args, ONE);
#endif
  TreeToggle(node->widget, (XtPointer)node, (XtPointer)state);

  if (!recurse) {
      return;
  }

  for (i = 0; i < node->num_children; i++) {
      _TreeSelectNode (node->children[i], type, recurse);
  }   /* for (all children of this node) */
}   /* _TreeSelectNode() */


/* Function Name:
 *   _TreeRelabelNode
 *
 * Description:
 *   Modifies the node and all its decendants label.
 *
 * Arguments:
 *   node - node to operate on.
 *   type - type of selection to perform.
 *   recurse - whether to continue on down the tree.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   SetMessage - utils.c
 *   _TreeRelabelNode - (recursive)
 *   notShown - (self)
 */
void
_TreeRelabelNode (WNode *node, LabelTypes type, Boolean recurse)
{
#define  MAX_NAME_LEN  30
  int  i;
  Arg  args[1];
  char buf[MAX_NAME_LEN + 7];      /* Maximum: "...name>..+" or "(name>)?" */
  char tmpbuf[MAX_NAME_LEN + 7];   /* Maximum: "...name>..+" or "(name>)?" */
  char *strptr;
  char *label;


  /* if this node is not shown, we can return */
  if ( (node->widget == NULL)
#ifndef DEBUG
       || (notShown(node))
#endif   /* DEBUG */
       )
    {
      /* we must relabel children, if this node keeps the subtree to show */
      if ( (recurse) && (node->view == NotShownBefore) )
	{
	  for (i = 0; i < node->num_children; i++)
	    {
	      _TreeRelabelNode (node->children[i],
				type,
				recurse);
	    }   /* for (all children of this node) */

	}

      return;
    }

  switch (type)
    {
    case ClassLabel:
      strptr = node->class;
      break;

    case NameLabel:
      strptr = node->name;
      break;

    case IDLabel:
      sprintf (tmpbuf, "id: 0x%lx",
	       node->id);
      strptr = tmpbuf;
      break;

    case WindowLabel:
      if (node->window == EDITRES_IS_UNREALIZED)
	{
	  strptr = "unrealized widget";
	}
      else if (node->window == EDITRES_IS_OBJECT)
	{
	  strptr = "non windowed object";
	}
      else
	{
	  sprintf (tmpbuf,
		   "win: 0x%lx",
		   node->window);
	  strptr = tmpbuf;
	}

      break;

    case ToggleLabel:
      XtSetArg (args[0],
		XtNlabel, &label);
      XtGetValues (node->widget,
		   args, ONE);
      if (label && !strstr (label, node->name))
	{
	  strptr = node->class;
	}
      else
	{
	  strptr = node->name;
	}
      break;

    case ViewLabel:
      switch (node->view)
	{
	case NotShownBefore:
	  strptr = "Before";
	  break;

	case FirstShown:
	  strptr = "First";
	  break;

	case Shown:
	  strptr = "shown";
	  break;

	case LastShown:
	  strptr = "Last";
	  break;

	case NotShownAfter:
	  strptr = "After";
	  break;

	case NotShownBeside:
	  strptr = "Beside";
	  break;

	default:
	  strptr = "?unknown?";
	  fprintf(stderr,
		  "internal error: unknown view type\n");
	}   /* switch (node->view) */

      break;   /* case ViewLabel: */

    default:
      /* "Internal Error: Unknown select type" */
      SetMessage (global_screen_data.info_label,
		  res_labels[32],
		  "Internal Error: Unknown select type");
      return;
    }   /* switch() */

  strncpy(buf,
	  strptr,
	  MAX_NAME_LEN-1);
  buf[MAX_NAME_LEN-1] = '\0';
  if (strlen(strptr) > strlen(buf))
    {
      strcat(buf, ">");
    }

  if ( (node->view == FirstShown)
       && (node != global_tree_info->top_node) )
    {
      sprintf(tmpbuf,
	      "...%s",
	      buf);
      strcpy(buf, tmpbuf);
    }

  if ( (node->view == LastShown) && (node->num_children > 0) )
    {
      strcat(buf, "..");
    }

  if (node->childrenFolded)
    {
      strcat(buf, "+");
    }

  if (notShown(node))
    {
      sprintf(tmpbuf,
	      "(%s)?",
	      buf);
      strcpy(buf, tmpbuf);
    }
  XtSetArg (args[0], XtNlabel, buf);
  XtSetValues(node->widget, args, ONE);
  node->label = XtRealloc(node->label, strlen(buf)+1);
  strcpy(node->label, buf);

  if (!recurse)
    {
      return;
    }

  for (i = 0; i < node->num_children; i++)
    {
      _TreeRelabelNode (node->children[i],
			type,
			recurse);
    }   /* for (all children of this node) */

}   /* _TreeRelabelNode() */


/* Function Name:
 *   _TreeActivateNode
 *
 * Description:
 *   Activates relatives of the node specfied, as specified by type.
 *
 * Arguments:
 *   node - node to opererate on.
 *   type - type of activate to invode.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   AddNodeToActiveList - (self)
 *   _TreeActivateNode - (self)
 *   SetMessage - utils.c
 *   SetSelWidLabel - widgets.c
 */
void
_TreeActivateNode (WNode *node, SelectTypes type)
{
  Arg args[1];
  int i;

#if 0
  XtSetArg (args[0],
	    XtNstate, TRUE);
#endif

  if ((type == SelectParent) || (type == SelectAncestors))
    {
      node = node->parent;
      if ( (node == NULL) || (node->widget == NULL)
#ifndef DEBUG
	   || (notShown(node))
#endif   /* DEBUG */
	   )
	{
	  return;
	}

      XtSetValues (node->widget,
		   args, ONE);
      AddNodeToActiveList (node);

      if (type == SelectAncestors)
	{
	  _TreeActivateNode (node, type);
	}
    }   /* if (SelectParent || SelectAncestors) */

  else if ((type == SelectChildren) || (type == SelectDescendants))
    {
      for (i = 0; i < node->num_children; i++)
	{
	  AddNodeToActiveList (node->children[i]);
	  if (node->children[i]->widget != NULL)
	    {
	      XtSetValues (node->children[i]->widget,
			   args, ONE);

	      if (type == SelectDescendants)
		{
		  _TreeActivateNode (node->children[i], type);
		}

	    }   /* if (toggle widget exists: node shown) */

	}   /* for (all children of parameter node) */
    }   /* if (SelectChildren ||  SelectDescendants) */

  else
    {
      /* "Internal Error: Unknown acctivate type" */
      SetMessage (global_screen_data.info_label,
		  res_labels[33],
		  "Internal Error: Unknown acctivate type");
    }

  /* update number of selected widgets */
  SetSelWidLabel (global_tree_info->num_active);

}   /* _TreeActivateNode() */


/* Function Name:
 *   CopyActiveNodes
 *
 * Description:
 *   returns a copy of the currently selected nodes.
 *
 * Arguments:
 *   tree_info - the tree info struct.
 *
 * Returns:
 *   a copy of the selected nodes.
 *
 * Calls:
 *   XtMalloc - Xtoolkit
 */
WNode **
CopyActiveNodes (TreeInfo *tree_info)
{
  WNode **list;
  int   i;


  if ((tree_info == NULL) || (tree_info->num_active == 0))
    {
      return (NULL);
    }

  list = (WNode**)XtMalloc (sizeof(WNode*) * tree_info->num_active);

  for (i = 0; i < tree_info->num_active; i++)
    {
      list[i] = tree_info->active_nodes[i];
    }

  return (list);
}   /* CopyActiveNodes() */


/* Function Name:
 *   _newTopParent
 *
 * Description:
 *   The parent of the actual root of the tree becomes the new top of the tree.
 *   Only the children of the new top, which haven't been shown, are shown,
 *   not their subtrees.
 *
 * Arguments:
 *   recursive - repeat up to the root node
 *
 * Returns:
 *   none
 *
 * Calls:
 *   notShown - (self)
 *   _treeFold - (self)
 *   _newTopParent - (recursive)
 */
void
_newTopParent(Boolean recursive)
{
  WNode   *top = global_tree_info->newtop_node;
  int i;


LOG "_newTopParent: %s\n", top->name  END
  if (top->parent)
    {
      /* becomes the actual top LastShown? */
      if (top->childrenFolded)
	{
	  /* if all children folded this node is LastShown */
	  i = 0;
	  while ( (i < top->num_children)
		  && notShown(top->children[i]) )
	    {
	      i++;
	    }   /* while (children not shown) */
	  if (i < top->num_children)
	    {
	      top->view = Shown;
	    }
	  else
	    {
	      top->view = LastShown;
	    }

	}   /* if (some children folded) */

      /* has the actual top children? */
      else if (top->num_children == 0)
	{
	  top->view = LastShown;
	}   /* elsif (no children) */

      else
	{
	  top->view = Shown;
	}   /* else (all children shown) */

      for (i=0; i < top->parent->num_children; i++)
	{
	  if (top->parent->children[i] != top)
	    {
	      _treeFold(top->parent->children[i],
			Unfold1Selected);
	    }
	}   /* for (all children) */

      top = top->parent;
      top->view = FirstShown;
      global_tree_info->newtop_node = top;
      global_tree_info->newtop_depth = top->num_parents;

      if (recursive)
	{
	  _newTopParent(recursive);
	}

    }   /* if (parent exists) */

}   /* _newTopParent() */


/* Function Name:
 *   _newTopSub
 *
 * Description:
 *   Define the parameter node as the new root of the tree.
 *   The new top node must be below the actual root of the tree and must
 *   have the view set to "FirstShown".
 *
 * Arguments:
 *   node - a node below the new top node.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   _newTopSub - (recursive)
 */
void
_newTopSub(node)
     WNode *node;
{
  WNode *parent;
  Arg   args[1];     /* set XtNtreeParent to NULL */
  int   i;


LOG "_newTopSub: %s\n", node->name  END
  if (node->widget)
    {
#if 0
      XtSetArg (args[0],
		XtNtreeParent, NULL);
      XtSetValues (node->widget,
		   args, ONE);
#endif
    }

  if (node->view == FirstShown)
    {
      /* end the recursion */
      global_tree_info->newtop_node = node;
      global_tree_info->newtop_depth = node->num_parents;

      /* all parents get NotShownBefore */
      parent = node->parent;
      while (parent != NULL)
	{
	  parent->view = NotShownBefore;
	  parent = parent->parent;
	}   /* while (path of parents to root node) */

    }   /* if (we reached the newtop) */

  else
    {
      node->view = NotShownBeside;

      for (i=0; i < node->num_children; i++)
	{
	  _newTopSub (node->children[i]);
	}   /* for (all children) */

    }   /* else (we still haven't reached the new top) */

}   /* _newTopSub() */


/* Function Name:
 *   _treeFold
 *
 * Description:
 *   This functions sets a new viewing status of this node or subtree.
 *
 * Arguments:
 *   node - the node that becomes a new viewing status
 *   type - the type of folding (FoldSelected, Unfold1Selected,
 *                               UnfoldSelected, UnfoldAll)
 *
 * Returns:
 *   none
 *
 * Calls:
 *   notShown - (self)
 *   fillSubTree - (self)
 *   _treeFold - (recursive)
 *   AddTreeNode - (self)
 */
void
_treeFold(WNode *node, FoldTypes type)
{
  int i;


LOG "_treeFold: %s\n", node->name  END
  switch (type)
    {
    case FoldSelected:
      if ( (node->view == FirstShown) || (node->parent == NULL) )
	{
	  fprintf(stderr,
		  "error: already at top node. Node %s can't be folded!\n",
		  node->name);
	  return;
	}   /* if (FirstShown) */

      if (notShown(node))
	{
	  return;   /* already not shown */
	}   /* if (not shown) */

      fillSubTree(node, NotShownAfter);

      /* the parent must be Shown or FirstShown */
      node->parent->childrenFolded = TRUE;
      if (node->parent->view == Shown)
	{
	  /* if all children are notShown, node->parent gets LastShown */
	  for (i=0; i < node->parent->num_children; i++)
	    {
	      if (!notShown(node->parent->children[i]))
		{
		  /* if one children is shown, we can leave this */
		  /* node->parent to Shown */
		  return;
		}
	    }   /* for (all children) */

	  node->parent->view = LastShown;

	}   /* if (parent Shown) */
      break;

    case Unfold1Selected:
      if (notShown(node))
	{
	  /* node must be the children of selected = the last node to show */
	  if (node->view != FirstShown)
	    {
	      node->view = LastShown;
	    }   /* if (this isn't the actual newtop) */

	  if (node->num_children > 0)
	    {
	      node->childrenFolded = TRUE;
	      for (i=0; i < node->num_children; i++)
		{
		  fillSubTree (node->children[i],
			       NotShownAfter);
		}   /* for (all children) */
	    }
	  else
	    {
	      node->childrenFolded = FALSE;
	    }   /* else (no children) */

	}   /* if (subtree is folded) */
      break;

    case UnfoldSelected:
      fillSubTree (node, Shown);
      AddTreeNode (global_tree_info->tree_widget,
		   node);
      break;

    case UnfoldAll:
      if (notShown(node))
	{
	  _treeFold (node,
		     UnfoldSelected);
	}   /* if (node not shown) */

      else
	{
	  node->childrenFolded = FALSE;
	  if ( (node->view != FirstShown)
	       && (node->num_children > 0) )
	    {
	      node->view = Shown;
	    }   /* if (!FirstNode and Has Children) */

	  for (i=0; i < node->num_children; i++)
	    {
	      _treeFold (node->children[i],
			 UnfoldAll);
	    }   /* for (all children) */
	}   /* else (node shown: check children recursively) */
      break;

    default:
      fprintf(stderr,
	      "internal error: unknown foldType!\n");
      return;
    }   /* switch (type) */

}   /* _treeFold() */


/* Function Name:
 *   displayNewTree
 *
 * Description:
 *   Creates a new tree widget with the actual viewing status of each node
 *
 * Arguments:
 *   node.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   SetMessage - utils.c
 *   AddTreeNode - (self)
 *   SetShownWidLabel -
 *   _TreeRelabel - (self)
 *   clearNodesWidget - (self)
 */
void
displayNewTree ()
{
	fprintf(stderr, "DisplayNewTree (#if-ed out)\n");
#if 0
  Widget tree_parent;


LOG "displayNewTree\n"  END
  if (!global_tree_info->tree_widget)
    {
      /* "No widget/style tree is available" */
      SetMessage(global_screen_data.info_label,
		 res_labels[17],
		 "No widget/style tree is available");
      return;
    }

  /* closes the print dialog */
  if (global_screen_data.tree_dialog)
    {
      XtPopdown(global_screen_data.tree_dialog);
    }

  tree_parent = XtParent(global_tree_info->tree_widget);
  XtDestroyWidget (global_tree_info->tree_widget);
  clearNodesWidget (global_tree_info->top_node);

  global_tree_info->num_active = 0;
  SetSelWidLabel (0);

  global_tree_info->tree_widget = XtCreateWidget ("tree",
						  treeWidgetClass,
						  tree_parent,
						  NULL, ZERO);

  global_tree_info->num_shown = 0;
  AddTreeNode (global_tree_info->tree_widget,
	       global_tree_info->top_node);
  SetShownWidLabel (global_tree_info->num_shown);

  if (XtIsRealized (tree_parent))   /* hack around problems in Xt. */
    {
      XtRealizeWidget (global_tree_info->tree_widget);
    }

  XtManageChild (global_tree_info->tree_widget);

  _TreeRelabel (global_tree_info,
		global_tree_info->viewing);
#endif
}   /* displayNewTree() */


/************************************************************

 * Non - Exported Functions. 
 *
 ************************************************************/


/* Function Name:
 *   AddNode
 *
 * Description:
 *   Adds a node to the widget tree (internal data).
 *
 * Arguments:
 *   top_node - a pointer to the current top node.
 *   info - the info from the client about the widget tree.
 *   tree_info - global information on this tree.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   FindNode - (self)
 *   FillNode - (self)
 *   AddChild - (self)
 */
static void
AddNode(WNode **top_node, WidgetTreeInfo *info, TreeInfo *tree_info)
{
  WNode    *node;
  WNode    *parent;
  Boolean  early_break = FALSE;
  Cardinal number = info->widgets.num_widgets;

#if 0
  if (top_node && *top_node)
	fprintf(stderr, "AddNode(%s, %d children, wid %p)\n",
		(*top_node)->name, (*top_node)->num_children,
		(*top_node)->widget);
#endif

  if ((node = FindNode (*top_node, info->widgets.ids, number)) == NULL)
    {
      node = (WNode*)XtCalloc (sizeof (WNode), ONE);

      node->id = info->widgets.ids[number - 1];
      FillNode (info,
		node,
		tree_info);

      for (number--; number > 0; number--, node = parent) {
	  parent = FindNode(*top_node, info->widgets.ids, number);
	  if (parent == NULL) {
	      parent = (WNode*)XtCalloc (sizeof (WNode), ONE);
	      parent->id = info->widgets.ids[number - 1];
	  } else {
	      early_break = TRUE;
	  }

	  AddChild (parent, node);

	  if (early_break) {
	      break;
	  }
	}   /* for (number of widgets in info: search parent) */

      if (!early_break)	{
	  if (node->parent == NULL) {
	      *top_node = node;
	  } else {
	      *top_node = node->parent;
	  }
	}   /* if (!early_break) */

    }   /* if (node not exist) */
  else
    {
      FillNode(info, node, tree_info);
    }

}   /* AddNode() */


/* Function Name:
 *   FillNode
 *
 * Description:
 *   Fills in everything but the node id and the depth in the node.
 *
 * Arguments:
 *   info - the info from the client.
 *   node - node to fill.
 *   tree_info - global information on this tree.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   (none)
 */
static void
FillNode (WidgetTreeInfo *info, WNode *node, TreeInfo *tree_info)
{
  node->class = info->class;
  info->class = NULL;		/* keeps it from deallocating. */
  node->name = info->name;
  info->name = NULL;
  node->window = info->window;
  node->label = NULL;
  node->tree_info = tree_info;
}   /* FillNode() */


/* Function Name:
 *   AddChild
 *
 * Description:
 *   Adds a child to an existing node.
 *
 * Arguments:
 *   parent - parent node.
 *   child - child node to add.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   XtRealloc - Xtoolkit
 */
static void
AddChild (WNode *parent, WNode *child)
{
  if (parent->num_children >= parent->alloc_children)
    {
      parent->alloc_children += NUM_INC;
      parent->children = (WNode**)XtRealloc ((char*)parent->children,
				    sizeof(WNode*) * parent->alloc_children);
    }

  parent->children[parent->num_children] = child;
  (parent->num_children)++;

  child->parent = parent;
}   /* AddChild() */


/************************************************************
 *
 *  Functions that operate of the current tree.
 * 
 ************************************************************/

/* Function Name:
 *   SetAndCenterTreeNode
 *
 * Description:
 *   Deactivates all nodes, activates the one specified, and
 *   and moves the tree to be centered on the current node.
 *
 * Arguments:
 *   node - node to use.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   _TreeSelect - (self)
 *   _TreeSelectNode - (self)
 */
void
SetAndCenterTreeNode (WNode *node)
{
  Arg       args[5];
  Cardinal  num_args;
  Position  node_x;
  Position  node_y;
  Dimension port_width;
  Dimension port_height;
  Dimension node_width;
  Dimension node_height;
  Dimension node_bw;


  /* if this node is not shown, we can return */
  if ( (node->widget == NULL)
#ifndef DEBUG
       || (notShown(node))
#endif   /* DEBUG */
       )
    {
      return;
    }

  _TreeSelect (node->tree_info, SelectNone);	/* Unselect all nodes */
  _TreeSelectNode (node, SelectAll, FALSE);	/* Select this node */

  /*
   * Get porthole dimensions.
   */

  num_args = 0;
  XtSetArg (args[num_args],
	    XtNwidth, &port_width);
  num_args++;
  XtSetArg (args[num_args],
	    XtNheight, &port_height);
  num_args++;
  XtGetValues (XtParent (node->tree_info->tree_widget),
	       args, num_args);

  /*
   * Get node widget dimensions.
   */

  num_args = 0;
  XtSetArg (args[num_args],
	    XtNwidth, &node_width);
  num_args++;
  XtSetArg (args[num_args],
	    XtNheight, &node_height);
  num_args++;
  XtSetArg (args[num_args],
	    XtNborderWidth, &node_bw);
  num_args++;
  XtSetArg (args[num_args],
	    XtNx, &node_x);
  num_args++;
  XtSetArg (args[num_args],
	    XtNy, &node_y);
  num_args++;
  XtGetValues (node->widget,
	       args, num_args);

  /*
   * reset the node x and y location to be the new x and y location of
   * the tree relative to the porthole.
   */

  node_x = (port_width / 2) - (node_x + node_width / 2 + node_bw);
  node_y = (port_height / 2) - (node_y + node_height / 2 + node_bw);

  num_args = 0;
  XtSetArg (args[num_args],
	    XtNx, node_x);
  num_args++;
  XtSetArg (args[num_args],
	    XtNy, node_y);
  num_args++;
  XtSetValues (node->tree_info->tree_widget,
	       args, num_args);
}   /* SetAndCenterTreeNode() */


/* Function Name:
 *   PerformTreeToFileDump
 *
 * Description:
 *   Dumps the contents of the current widget tree to the file specified. 
 *
 * Arguments:
 *   node - node to dump.
 *   num_tabs - number of spaces to indent.
 *   fp - pointer to the file to write to.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   PerformTreeToFileDump - (recursive)
 */
void
PerformTreeToFileDump (WNode *node, int num_tabs, FILE  *fp)
{
  int i;


  for (i = 0; i < num_tabs; i++)
    {
      fprintf (fp, "\t");
    }

  fprintf (fp,
	   "%s (%s)\n",
	   node->name,
	   node->class);

  num_tabs++;
  for (i = 0; i < node->num_children; i++)
    {
      PerformTreeToFileDump (node->children[i],
			     num_tabs,
			     fp);
    }

}   /* PerformTreeToFileDump() */


/* Function Name:
 *   fillSubTree
 *
 * Description:
 *   Sets the node->view to parameter <type> of all children (recursive)
 *   and set node->childrenFolded. Shown is possibly modified to LastShown,
 *   if it is the last node to show. It also unmanages the unshown widgets.
 *
 * Arguments:
 *   node - the node that becomes a new viewing status
 *   type - the type of view (NotShownBefore, NotShownBeside, NotShownAfter,
 *                            FirstShown, Shown, LastShown)
 *
 * Returns:
 *   none
 *
 * Calls:
 *   fillSubTree - (recursive)
 */
static void
fillSubTree (WNode *node, ViewTypes type)
{
  Arg      args[1];
  Cardinal num_args = 0;
  int      i;


LOG "fillSubTree: %s ", node->name  END
switch (type)
  {
  case NotShownBefore: LOG "NotShownBefore\n"  END break;
  case NotShownBeside: LOG "NotShownBeside\n"  END break;
  case NotShownAfter: LOG "NotShownAfter\n"  END break;
  case Shown: LOG "Shown\n"  END break;
  case FirstShown: LOG "FirstShown\n"  END break;
  case LastShown: LOG "LastShown\n"  END break;
  }

  for (i=0; i < node->num_children; i++)
    {
      fillSubTree (node->children[i], type);
    }   /* for (all children) */

  if (type == Shown)
    {
      /* the FirstShown node must be unchanged! */
      if (node->view != FirstShown)
	{
	  if (node->num_children == 0)
	    {
	      node->view = LastShown;
	    }   /* if (no children) */
	  else
	    {
	      node->view = Shown;
	    }
	}   /* if (node != FirstShown) */
      node->childrenFolded = FALSE;
    }
  else
    {
      node->view = type;
    }

#ifndef DEBUG
  if (notShown(node))
    {
      if (node->widget)
	{
#if 0
	  XtSetArg (args[num_args],
		    XtNtreeParent, NULL);
	  num_args++;
#endif
	  XtSetValues (node->widget,
		       args, num_args);

	  if (XtIsManaged(node->widget))
	    {
	      XtUnmanageChild(node->widget);
	      global_tree_info->num_shown--;

	    }   /* if (widget is still managed) */
	}   /* if (widget created) */
    }   /* if (remove this node) */
#endif   /* DEBUG */

  if ( (type == NotShownAfter)
       && (node->num_children > 0) )
    {
      node->childrenFolded = TRUE;
    }

}   /* fillSubTree() */


/* Function Name:
 *   clearNodesWidget
 *
 * Description:
 *   Sets the node->widget to NULL, because it has been destroyed
 *
 * Arguments:
 *   top - the node of the subtree to clear the node->widget
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   fillWidget - (recursive)
 */
void
clearNodesWidget (top)
     WNode *top;
{
  int i;


LOG "clearNodesWidget: %s\n", top->name  END
  top->widget = NULL;
  for (i=0; i < top->num_children; i++)
    {
      clearNodesWidget(top->children[i]);
    }   /* for (all children) */

}   /* clearNodesWidget() */


/* Function Name:
 *   notShown
 *
 * Description:
 *   Determins wether this node has viewing status of (NotShown*) or not
 *
 * Arguments:
 *   node - the node to examine
 *
 * Returns:
 *   TRUE, if node->view is one of (NotShownBefore, NotShownBeside, NotShownAfter)
 *
 * Calls:
 *   none.
 */
Boolean
notShown (node)
     WNode *node;
{
  if ( (node->view == NotShownBefore)
       || (node->view == NotShownBeside)
       || (node->view == NotShownAfter) )
    {
      return(TRUE);
    }   /* if (notShown) */
  else
    {
      return(FALSE);
    }

}   /* notShown() */


/* Function Name:
 *   checkName
 *
 * Description:
 *   Returns the illigal character in the parameter widget name
 *
 * Arguments:
 *   s - name of a widget
 *
 * Returns:
 *   '\0' if s contains no illegal characters for widget name,
 *   else the first illegal character
 *
 * Calls:
 *   (none)
 */
char
checkName (s)
     char *s;
{
  char *ptr;


  ptr = s;
  while (*ptr != '\0')
    {
      if ((*ptr >= 'a' && *ptr <= 'z')
	  || (*ptr >= 'A' && *ptr <= 'Z')
	  || (*ptr >= '0' && *ptr <= '9')
	  || *ptr == '_'
	  || *ptr == '-')
	{
	  ptr++;
	  continue;
	}
      return(*ptr);
    }

  return(*ptr);

}   /* checkName() */



/* >>>>> HACK fuer batchres */

#define BUF_LEN 200      /* must be longer than the longest resource name */

/* Function Name:
 *   LoadNewResFile
 *
 * Description:
 *   Start loading the resources of parameter 'resFile' into the
 *   connected client. This is a timeout procedure.
 *
 * Arguments:
 *   res_file - pointer to the buffer of the loaded resource file (client_data)
 *   timerID - *** NOT USED ***
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   NextLine - (self)
 */
void
LoadNewResFile (XtPointer *res_file, XtIntervalId timerID)
{
  /* return immediately if no client connected */
  if (global_tree_info->num_nodes <= 0)
    {
      return;
    }
  if (res_file == NULL)
    {
      fprintf(stderr,
	      "Parameter error in LoadNewResFile()!\n");
      return;
    }

  global_mode = MODE_BATCHRES;
  XtUnmanageChild(global_screen_data.info_label);

  NextLine((char *)res_file);

}   /* LoadNewResFile() */


/* Function Name:
 *   NextLine
 *
 * Description:
 *   Get the next resource line from the buffer which is loaded from the
 *   resource file given in parameter 'resFile' and set the value in the client.
 *   The first call should give the pointer to the buffer, subsequent calls
 *   should be called with NULL to continue with the next line in the buffer.
 *   The buffer must be finished with '\0'. After reaching the end, the buffer
 *   is freed.
 *
 * Arguments:
 *   res_file - pointer to the buffer of the loaded resource file
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   brApplyResources - handler.c
 */
void
NextLine (char *res_file)
{
  static char  *bufptr = NULL;
  static int   line = 0;
  char         *ptr;
  char         *res_path;
  char         res_name[BUF_LEN];
  char         *res_value;
  size_t       len_value;
  int          special;
  int          onumber;
  char         *src = NULL;


  if (res_file != NULL)
    {
      bufptr = res_file;
      line = 1;
    }

  if (bufptr == NULL)
    {
      fprintf(stderr, "Internal Error! No buffer to work with.\n");
      return;
    }

  /* ignore comment lines and blank lines, remove leading whitespaces */
  while ( ( (*bufptr == '!')
	    || (iswhitespace(*bufptr))
	    || (*bufptr == '\n') )
	  && (*bufptr != '\0') )
    {
      if (*bufptr == '!')
	{
	  line++;
	  while (*bufptr != '\n')
	    {
	      bufptr++;
	    }
	}
      else
	{
	  if (*bufptr == '\n')
	    {
	      line++;
	    }
	  bufptr++;
	}
    }

  if (*bufptr == '\0')
    {
      /* finished working on buffer */
      global_mode = MODE_INTERACTIVE;
      XtFree(global_resfile_buffer);
      global_resfile_buffer = NULL;
      XtManageChild(global_screen_data.info_label);

printf("Resource file loaded into client.\n");

      if (global_resources.terminate)
	{
	  exit(EXIT_SUCCESS);
	}

      return;
    }   /* if (end of buffer) */

  else
    {
      /* resource path and name */
      ptr = bufptr;
      while ( (*ptr != ':')
	      && (*ptr != '\0') )
	{
	  ptr++;
	}
      if (*ptr == '\0')
	{
	  fprintf(stderr,
		  "ERROR parsing resource file: Early end of file (%s)\n",
		  "no resource specification with ':' found");
	  fprintf(stderr,
		  "line %d..%d: %s\n",
		  line,
		  line + global_resfile_wraplines,
		  bufptr);

	  global_mode = MODE_INTERACTIVE;
	  XtFree(global_resfile_buffer);
	  global_resfile_buffer = NULL;
	  XtManageChild(global_screen_data.info_label);

	  if (global_resources.terminate)
	    {
	      exit(EXIT_FAILURE);
	    }

	  return;
	}   /* if (ERROR: early EOF) */

      res_path = bufptr;
      bufptr = ptr + 1;        /* start position for resource value */

      /* remove tailing blanks from resource specification */
      ptr--;
      while ( (iswhitespace(*ptr))
	      && (ptr > global_resfile_buffer)
	      && (*(ptr-1) != '\0') )
	{
/* 	  if (*ptr == '\n') */
/* 	    { */
/* 	      line++; */
/* 	    } */
	  ptr--;
	}
      if ( (ptr <= global_resfile_buffer)
	   || (*(ptr-1) == '\0') )
	{
	  fprintf(stderr,
		  "ERROR parsing resource file: No resource specification (%s)\n",
		  "no resource path or name before ':' found");
	  fprintf(stderr,
		  "line %d..%d: %s\n",
		  line,
		  line + global_resfile_wraplines,
		  ptr);

	  global_mode = MODE_INTERACTIVE;
	  XtFree(global_resfile_buffer);
	  global_resfile_buffer = NULL;
	  XtManageChild(global_screen_data.info_label);

	  if (global_resources.terminate)
	    {
	      exit(EXIT_FAILURE);
	    }

	  return;
	}

      *(ptr+1) = '\0';

      /* resource name */
      while ( (*ptr != '.')
	      && (*ptr != '*')
	      && (ptr > global_resfile_buffer)
	      && (*ptr != '\0') )
	{
	  ptr--;
	}
      if ( ((ptr <= global_resfile_buffer) && (*ptr != '*') && (*ptr != '.'))
	   || (*ptr == '\0') )
	{
	  fprintf(stderr,
		  "ERROR parsing resource file: No binding before name(%s)\n",
		  "no '.' or '*' before resource name found");
	  fprintf(stderr,
		  "line %d..%d: %s\n",
		  line,
		  line + global_resfile_wraplines,
		  ptr);

	  global_mode = MODE_INTERACTIVE;
	  XtFree(global_resfile_buffer);
	  global_resfile_buffer = NULL;
	  XtManageChild(global_screen_data.info_label);

	  if (global_resources.terminate)
	    {
	      exit(EXIT_FAILURE);
	    }

	  return;
	}

      ptr++;
      strcpy(res_name,
	     ptr);
      *ptr = '\0';

      /* resource value */
      while ( (iswhitespace(*bufptr))
	      && (bufptr != '\0') )
	{
	  bufptr++;
	}
      /* "" is a valid value, so '\n' here is no error */
      if (*bufptr == '\0')
	{
	  fprintf(stderr,
		  "ERROR parsing resource file: Early end of file (%s)\n",
		  "no resource value after ':' found");
	  fprintf(stderr,
		  "line %d..%d: %s%s:\n",
		  line,
		  line + global_resfile_wraplines,
		  res_path,
		  res_name);

	  global_mode = MODE_INTERACTIVE;
	  XtFree(global_resfile_buffer);
	  global_resfile_buffer = NULL;
	  XtManageChild(global_screen_data.info_label);

	  if (global_resources.terminate)
	    {
	      exit(EXIT_FAILURE);
	    }

	  return;
	}

      ptr = bufptr;
      res_value = ptr;
      len_value = (size_t)0;
      special = 0;                 /* number of sequences escaped by '\' */
      while ( (*ptr != '\n')
	      && (*ptr != '\0') )
	{
	  if (*ptr == '\n')
	    {
	      line++;
	    }
	  if (*ptr == '\\')
	    {
	      ptr++;
	      special++;
	      len_value++;
	      if (isodigit(*ptr))
		{
		  if ( (!isodigit(*(ptr+1)))
		       || (!isodigit(*(ptr+2))) )
		    {
		      fprintf(stderr,
			      "ERROR parsing resource file: No octal number (%s)\n",
			      "octal number must be '\\###' with # between 0 and 7");
		      fprintf(stderr,
			      "line %d..%d: %s%s:%s\n",
			      line,
			      line + global_resfile_wraplines,
			      res_path,
			      res_name,
			      bufptr);

		      global_mode = MODE_INTERACTIVE;
		      XtFree(global_resfile_buffer);
		      global_resfile_buffer = NULL;
		      XtManageChild(global_screen_data.info_label);

		      if (global_resources.terminate)
			{
			  exit(EXIT_FAILURE);
			}

		      return;
		    }
		  /* '\###' with octal number n found */
		  ptr += 2;
		  len_value += 2;
		}
	    }
	  ptr++;
	}
      line++;

      if (*ptr != '\0')
	{
	  /* there are more lines to work with */
	  bufptr = ptr + 1;
	  *ptr = '\0';
	}
      else
	{
	  /* this is the last line of the buffer */
	  bufptr = ptr;
	}

      if (special > 0)
	{
	  /* substitute special chars by '\' */
	  ptr = res_value;
	  while ((*ptr != '\0')
		 && (ptr <= bufptr))
	    {
	      if (*ptr == '\\')
		{
		  switch (*(ptr+1))
		    {
		    case ' ':
		      src = ptr + 1;
		      break;

		    case '\t':
		      src = ptr + 1;
		      break;

		    case 'n':
		      *(ptr+1) = '\n';
		      src = ptr + 1;
		      break;

		    case '\\':
		      src = ptr + 1;
		      break;

		    case '0':
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		      if ( (isodigit(*(ptr+2))) && (isodigit(*(ptr+3))) )
			{
			  fprintf(stderr,
			    "Warning parsing resource file: No octal number (%s)\n",
			    "octal number must be '\\###' with # between 0 and 7");
			  fprintf(stderr,
				  "line %d..%d: %s%s:%s\n",
				  line,
				  line + global_resfile_wraplines,
				  res_path,
				  res_name,
				  res_value);
			}   /* if (incorrect "\###" sequence) */
		      else
			{
			  onumber = (*(ptr+1)) * 64 + (*(ptr+2)) * 8 + (*(ptr+3));
			  if (onumber >= 256)
			    {
			      fprintf(stderr,
				"Warning parsing resource file: Octal number out %s\n",
				"of range (must be <= 399)");
			      fprintf(stderr,
				      "line %d..%d: %s%s:%s\n",
				      line,
				      line + global_resfile_wraplines,
				      res_path,
				      res_name,
				      res_value);
			      src = NULL;
			    }
			  else
			    {
			      *(ptr+3) = (char)onumber;
			      src = ptr + 3;
			    }   /* else ("\###" <= "\399") */
			}
		      break;

		    default:
		      fprintf(stderr,
			      "Warning parsing resource file: unknown '\\-%s' (%c)\n",
			      "sequence",
			      *(ptr-1));
		      fprintf(stderr,
			      "  allowed are only: %s\n",
			      "\\<blank>, \\t, \\n, \\\\, \\###, # between 0 and 7");
		      fprintf(stderr,
			      "line %d..%d: %s%s:%s\n",
			      line,
			      line + global_resfile_wraplines,
			      res_path,
			      res_name,
			      res_value);
		      src = NULL;
		      break;
		    }   /* switch (char behind '\') */

		  if (src != NULL)
		    {
		      memmove(ptr,
			      src,
			      (bufptr-1) - ptr);
		      *(bufptr-1) = '\0';
		    }
		}   /* if '\' found */

	      ptr++;
	    }   /* while (replace sequences with '\...') */

	}   /* if (res_value contains '\'-sequences) */

      SetWorkLabel(line*100 / global_resfile_lines);

      brApplyResource(res_path, res_name, res_value);

    }   /* else (not end of buffer) */

}   /* NextLine() */


/* Function Name:
 *   isodigit
 *
 * Description:
 *   Returns TRUE if parameter c is an octal digit
 *
 * Arguments:
 *   c - character or EOF
 *
 * Returns:
 *   TRUE if c is an octal digit.
 *
 * Calls:
 *   (none)
 */
Boolean
isodigit (int c)
{
  if ( (c >= (int)'0') && (c <= (int)'7') )
    {
      return(TRUE);
    }
  else
    {
      return(FALSE);
    }
}   /* isodigit() */


/* Function Name:
 *   iswhitespace
 *
 * Description:
 *   Returns TRUE if parameter c is <blank> or <tab>
 *
 * Arguments:
 *   c - character or EOF
 *
 * Returns:
 *   TRUE if c is a WhiteSpace for resource files (blank or tab)
 *
 * Calls:
 *   (none)
 */
Boolean
iswhitespace (int c)
{
  if ( (c == (int)' ') || (c == (int)'\t') )
    {
      return(TRUE);
    }
  else
    {
      return(FALSE);
    }
}   /* iswhitespace() */

/* <<<<< HACK fuer batchres */
