/*
 * $Header: /cvsroot-fuse/meditres/meditres/handler.c,v 1.19 2002/01/16 20:05:43 amai Exp $
 *
 * $XConsortium: handler.c,v 1.29 94/04/17 20:38:56 dave Exp $
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
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xfuncs.h>
#include <X11/Xos.h>		/* for W_OK def */

#include <Xm/Xm.h>
#include <Xm/Container.h>

#include "meditres.h"

/*
 * Function Definitions.
 */

static void SetOnlyMatchingWidgets(WNode *node, XtPointer info_ptr);
static void CreateSetValuesCommand(WNode *node, XtPointer info_ptr);
static void ObtainResource(XtPointer node_ptr);
static void _drawNode(Widget wid, WNode *node);

static int counter=0;


/* Function Name:
 *   Quit
 *
 * Description:
 *   This function destroys the context and exits the program
 *   with success.
 *
 * Arguments:
 *   w           - ** NOT USED **
 *   call_data   - ** NOT USED **
 *   client_data - ** NOT USED **
 *
 * Returns:
 *   none
 *
 * Calls:
 *   Xt-stuff
 */

/* ARGSUSED */
void
Quit(Widget w, XtPointer client, XtPointer call)
{
  XtDestroyApplicationContext(XtWidgetToApplicationContext (w));
  exit(EXIT_SUCCESS);
}   /* Quit() */


/* Function Name:
 *   SendTree
 *
 * Description:
 *    This function initiates the client communication.
 *    by getting the resource tree.
 *
 * Arguments:
 *    w - the widget that made the selection.
 *    value - a boolean value stored as a pointer.
 *            if True then get a new client, otherwise
 *            refresh the current client.
 *    call_data - ** NOT USED **
 *
 * Returns:
 *   none
 *
 * Calls:
 *   _XEditResResetStream - (Xmu library)
 *   SetCommand - utils.c
 */

/* ARGSUSED */
void
SendTree(Widget w, XtPointer value, XtPointer call_data)
{
  if (value) {
      global_client.window = None;
  }

  if (!XtIsWidget(w)) {     /* Make sure that we use a "Real" widget here. */
      w = XtParent(w);
  }

  _XEditResResetStream (&(global_client.stream));   /* an empty message. */

  SetCommand(w, LocalSendWidgetTree, NULL);
}   /* SendTree() */


/* Function Name:
 *   FindWidget
 *
 * Description:
 *   Maps a widget in the client to one in the currently
 *   displayed widget tree.
 *
 * Arguments:
 *   w - the widget that invoked this action.
 *   call_data - ** NOT USED **
 *   client_data - ** NOT USED **
 *
 * Returns:
 *   none
 *
 * Calls:
 *   _FindWidget -
 */

/* ARGSUSED */
void
FindWidget(Widget w, XtPointer client_data, XtPointer call_data)
{
	void _FindWidget();

	/* Use parent since it is a "real" widget not a rect_obj. */
	_FindWidget(XtParent(w));
}   /* FindWidget() */


/* Function Name:
 *   InitSetValues
 *
 * Description:
 *   This function pops up the setvalues dialog
 *
 * Arguments:
 *   w - the widget caused this action.
 *   call_data - ** NOT USED **
 *   client_data - ** NOT USED **
 *
 * Returns:
 *   none
 *
 * Calls:
 */

/* ARGSUSED */
void
InitSetValues(Widget w, XtPointer call_data, XtPointer client_data)
{
	if (!XtIsWidget(w)) {     /* Make sure that we use a "Real" widget here. */
		w = XtParent(w);
	}

	PopupSetValues(w, NULL);
}   /* InitSetValues() */


/* Function Name:
 *   InitFindDlg
 *
 * Description:
 *   This function pops up the find dialog
 *
 * Arguments:
 *   w - the widget caused this action.
 *   call_data - ** NOT USED **
 *   client_data - ** NOT USED **
 *
 * Returns:
 *   none
 *
 * Calls:
 *   PopupFindDlg - finddlg.c
 */

/* ARGSUSED */
void
InitFindDlg(Widget w, XtPointer call_data, XtPointer client_data)
{
  if (!XtIsWidget(w))     /* Make sure that we use a "Real" widget here. */
    {
      w = XtParent(w);
    }

  PopupFindDialog(w,
		  NULL);  /* Position from mouse cursor */

}   /* InitFindDlg() */


/* Function Name:
 *   TreeSelect
 *
 * Description:
 *   Selects all widgets.
 *
 * Arguments:
 *   w - the widget caused this action, ** NOT USED **
 *   call_data - ** NOT USED **
 *   client_data - The type of thing to select.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   _TreeSelect - wtree.c
 */

/* ARGSUSED */
void
TreeSelect(w, client_data, call_data)
     Widget    w;
     XtPointer call_data;
     XtPointer client_data;
{
  SelectTypes  type = (SelectTypes) client_data;


  _TreeSelect(global_tree_info, type);
}   /* TreeSelect() */


/* Function Name:
 *   TreeRelabel
 *
 * Description:
 *   Relabels a tree to the type specified.
 *
 * Arguments:
 *   w - the widget caused this action.
 *   call_data - ** NOT USED **
 *   client_data - the type of label to assign to each node.
 *
 * Returns:
 *   none
 *
 * Calls:
 */

/* ARGSUSED */
void
TreeRelabel(w, client_data, call_data)
     Widget    w;
     XtPointer call_data;
     XtPointer client_data;
{
  LabelTypes  type = (LabelTypes) client_data;


  _TreeRelabel(global_tree_info, type);
}   /* TreeRelabel() */


/* Function Name:
 *   PannerCallback
 *
 * Description:
 *   called when the panner has moved.
 *
 * Arguments:
 *   panner - the panner widget.
 *   closure - *** NOT USED ***.
 *   report_ptr - the panner record.
 *
 * Returns:
 *   none.
 *
 * Calls:
 */

/* ARGSUSED */
void
PannerCallback(w, closure, report_ptr)
     Widget    w;
     XtPointer closure;
     XtPointer report_ptr;
{
}   /* PannerCallback() */


/* Function Name:
 *   PortholeCallback
 *
 * Description:
 *   called when the porthole or its child has changed
 *
 * Arguments:
 *   porthole - the porthole widget.
 *   panner_ptr - the panner widget.
 *   report_ptr - the porthole record.
 *
 * Returns:
 *   none.
 *
 * Calls:
 */

/* ARGSUSED */
void
PortholeCallback(w, panner_ptr, report_ptr)
     Widget    w;
     XtPointer panner_ptr;
     XtPointer report_ptr;
{
}   /* PortholeCallback() */


/* Function Name:
 *   FlashActiveWidgets (X callback)
 *
 * Description:
 *   called to flass all active widgets in the display.
 *
 * Arguments:
 *   *** NOT USED ***
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   _FlashActiveWidgets         (geometry.c)
 */

/* ARGSUSED */
void
FlashActiveWidgets(Widget w, XtPointer junk, XtPointer garbage)
{
  _FlashActiveWidgets(global_tree_info);
}   /* FlashActiveWidgets() */


/* Function Name:
 *   GetResourceList
 *
 * Description:
 *   Gets the resources lists of all active widgets.
 *
 * Arguments:
 *   ** NOT USED **
 *
 * Returns:
 *   none
 *
 * Calls:
 *
 * Gets called from Ctrl<Key>R on the XmContainer, but the list
 * of selected widgets is already in global_tree_info->active_nodes.
 */

/* ARGSUSED */
void
GetResourceList(Widget w, XtPointer junk, XtPointer garbage)
{
	WNode		*node;
	ProtocolStream	*stream = &(global_client.stream);
	int		i, num_active;

	if (global_tree_info == NULL) {
		/* "No widget/style tree is available" */
		SetMessage(global_screen_data.info_label, res_labels[17],
			"No widget/style tree is available");
		return;
	}   /* if (no widget tree) */

#if 1
	/*
	 * There's a concurrency problem here, we can currently
	 * only run one query at a time.
	 */
	if (global_tree_info->num_active==0) {
  	   fprintf(stderr, "GetResourceList: no active nodes\n");
           /* counter=1; */
           return;
        }
#else
	fprintf(stderr, "GetResourceList: %d active nodes\n",
		global_tree_info->num_active);
	num_active = global_tree_info->num_active;
#endif
	for (i=0; i<num_active; i++) {
		node = global_tree_info->active_nodes[0];
		if (node->resources != NULL) {
			char *errors = NULL;

			CreateResourceBox(node, &errors);
			if (errors != NULL) {
				SetMessage(global_screen_data.info_label,
					errors, "?errors");
				XtFree(errors);
			}   /* if (error from CreateRessourceBox() ) */
		} else {   /* if (we have already resources for this node) */
		/*
		 * No resources, fetch them from the client.
		 */
		_XEditResResetStream (stream);
		_XEditResPut16 (stream, (unsigned short) 1);
		InsertWidgetFromNode (stream, node);
		SetCommand(global_tree_info->tree_widget,
			LocalGetResources, NULL);
		}   /* else (get resources from client) */
	} /* for */
	counter = 1;
}   /* GetResourceList() */


void OpenAdditionalResourceBoxes(void)
{
	WNode		*node;
	ProtocolStream	*stream = &(global_client.stream);

/* where is counter ever re-set, or decremented?? */
	if (counter >= global_tree_info->num_active)
		return;

	node = global_tree_info->active_nodes[counter++];
	if (node == NULL)
		return;

	if (node->resources != NULL) {
		char *errors = NULL;

		CreateResourceBox(node, &errors);
		if (errors != NULL) {
			SetMessage(global_screen_data.info_label,
				errors, "?errors");
			XtFree(errors);
		}   /* if (error from CreateRessourceBox() ) */
	} else {   /* if (we have already resources for this node) */
		/*
		 * No resources, fetch them from the client.
		 */
		_XEditResResetStream (stream);
		_XEditResPut16 (stream, (unsigned short) 1);
		InsertWidgetFromNode (stream, node);
		SetCommand(global_tree_info->tree_widget,
			LocalGetResources, NULL);
	}   /* else (get resources from client) */
}

/* Function Name:
 *   DumpTreeToFile
 *
 * Description:
 *   Dumps all widgets in the tree to a file.
 *
 * Arguments:
 *   w - the widget that activated this callback.
 *   junk - ** NOT USED **
 *   garbage - ** NOT USED **
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   _PopupFileDialog -
 */

/* ARGSUSED */
void
DumpTreeToFile(w, junk, garbage)
     Widget    w;
     XtPointer junk;
     XtPointer garbage;
{
fprintf(stderr, "DumpTreeToFile() called\n");
  _PopupFileDialog(XtParent(w),
		   "Enter the filename:",
		   "",
		   _DumpTreeToFile,
		   (XtPointer)global_tree_info);
}   /* DumpTreeToFile() */


/* Function Name:
 *   newTop
 *
 * Description:
 *   Callback for setting the new root of the tree.
 *
 * Arguments:
 *   w - the widget that activated this callback.
 *   newtopType - one of (NewRoot, NewParent, NewSelected)
 *   garbage - ** NOT USED **
 *
 * Returns:
 *   none (XtCallbackProc)
 *
 * Calls:
 *   _newTopParent - wtree.c
 *   _newTopSub - wtree.c
 *   AddTreeNode - wtree.c
 *   displayNewTree - wtree.c
 *   SetMessage - utils.c
 */
void
newTop(w, newTopType, garbage)
     Widget    w;
     XtPointer newTopType;
     XtPointer garbage;
{
  if (global_tree_info == NULL)
    {
      /* "No widget/style tree is available" */
      SetMessage(global_screen_data.info_label,
		 res_labels[17],
		 "No widget/style tree is available");
      return;
    }   /* if (no widget tree) */

  if (global_tree_info->top_node == NULL)
    {
      fprintf(stderr,
	      "internal error: no global_tree_info->top_node\n");
      return;
    }
  if (global_tree_info->newtop_node == NULL)
    {
      fprintf(stderr,
	      "internal error: no global_tree_info->newtop_node\n");
      return;
    }

  /* closes the print dialog */
  if (global_screen_data.tree_dialog)
    {
      XtPopdown(global_screen_data.tree_dialog);
    }

  switch ((NewtopTypes)newTopType)
    {
    case NewRoot:
      _newTopParent(TRUE);
      break;

    case NewParent:
      _newTopParent(FALSE);
      break;

    case NewSelected:
      if (global_tree_info->num_active != 1)
	{
	  /* "This function requires exactly one (1) widget/style to be selected" */
	  SetMessage(global_screen_data.info_label,
	    res_labels[19],
	    "This function requires exactly one (1) widget/style to be selected");
	  return;
	}   /* if (too much nodes selected) */

      global_tree_info->newtop_node->view = NotShownBefore;
      global_tree_info->active_nodes[0]->view = FirstShown;

      _newTopSub (global_tree_info->newtop_node);
      break;

    default:
      fprintf(stderr,
	      "internal error: unknown newTopType!\n");
      return;
    }   /* switch (newTopType) */

  displayNewTree();

}   /* newTop() */


/* Function Name:
 *   treeFold
 *
 * Description:
 *   Callback for setting the viewing status of some nodes.
 *
 * Arguments:
 *   w - the widget that activated this callback.
 *   foldType - one of (FoldSelected, FoldChildSelected,
 *                      Unfold1Selected, UnfoldSelected, UnfoldAll)
 *   garbage - ** NOT USED **
 *
 * Returns:
 *   none (XtCallbackProc)
 *
 * Calls:
 *   _treeFold - wtree.c
 *   CopyActiveNodes - wtree.c
 *   SetMessage - utils.c
 *   PrepareToLayoutTree - widgets.c
 *   LayoutTree - widgets.c
 *   SetShownWidLabel -
 */
void
treeFold(w, foldType, garbage)
     Widget    w;
     XtPointer foldType;
     XtPointer garbage;
{
  WNode    **list;
  WNode    *node;
  Cardinal num_list;
  int      i;
  int      j;


  if (global_tree_info == NULL)
    {
      /* "No widget/style tree is available" */
      SetMessage(global_screen_data.info_label,
		 res_labels[17],
		 "No widget/style tree is available");
      return;
    }   /* if (no widget tree) */

  /* only UnfoldAll needs no selected nodes */
  if ( ((FoldTypes)foldType != UnfoldAll)
       && (global_tree_info->num_active == 0) )
    {
      /* "There are no currently active widgets/styles." */
      SetMessage(global_screen_data.info_label,
		 res_labels[23],
		 "There are no currently active widgets/styles.");
      return;
    }   /* if (no widgets selected) */

  /* closes the print dialog */
  if (global_screen_data.tree_dialog)
    {
      XtPopdown(global_screen_data.tree_dialog);
    }

  XtUnmapWidget (global_tree_info->tree_widget);

  switch ((FoldTypes)foldType)
    {
    case FoldSelected:
      list = CopyActiveNodes (global_tree_info);
      num_list = global_tree_info->num_active;

      /* deselect all nodes */
      _TreeSelect(global_tree_info,
		  SelectNone);

      for (i=0; i < num_list; i++)
	{
	  _treeFold(list[i],
		    FoldSelected);
	}   /* for (all active nodes) */

      XtFree ((XtPointer)list);
      break;

    case FoldChildSelected:
      for (i=0; i < global_tree_info->num_active; i++)
	{
	  node = global_tree_info->active_nodes[i];

	  for (j=0; j < node->num_children; j++)
	    {
	      _treeFold(node->children[j],
			FoldSelected);
	    }   /* for (all children of active nodes) */

	}   /* for (all active nodes) */
      break;

    case Unfold1Selected:
      for (i=0; i < global_tree_info->num_active; i++)
	{
	  node = global_tree_info->active_nodes[i];

	  if ((node) && (node->view != FirstShown))
	    {
	      node->view = Shown;
	      node->childrenFolded = FALSE;
	    }   /* if (node != FirstShown) */

	  for (j=0; j < node->num_children; j++)
	    {
	      _treeFold(node->children[j],
			Unfold1Selected);
	    }   /* for (all children of active nodes) */

	  AddTreeNode (global_tree_info->tree_widget,
		       node);

	}   /* for (all active nodes) */
      break;

    case UnfoldSelected:
      for (i=0; i < global_tree_info->num_active; i++)
	{
	  _treeFold(global_tree_info->active_nodes[i],
		    UnfoldSelected);
	}   /* for (all active nodes) */
      break;

    case UnfoldAll:
      _treeFold(global_tree_info->newtop_node,
		UnfoldAll);
      break;

    default:
      fprintf(stderr,
	      "internal error: unknown foldType!\n");
      return;
    }   /* switch (foldType) */

  SetShownWidLabel (global_tree_info->num_shown);
  _TreeRelabel (global_tree_info,
		global_tree_info->viewing);   /* includes LayoutTree() */

}  /* treeFold() */


/* Function Name:
 *   showAll
 *
 * Description:
 *   Callback for setting the viewing status of all nodes to Shown.
 *
 * Arguments:
 *   w - the widget that activated this callback.
 *   junk - ** NOT USED **
 *   garbage - ** NOT USED **
 *
 * Returns:
 *   none (XtCallbackProc)
 *
 * Calls:
 *   _newTopParent - wtree.c
 *   _treeFold - wtree.c
 *   displayNewTree - wtree.c
 *   SetMessage - utils.c
 */
void
showAll(w, junk, garbage)
     Widget    w;
     XtPointer junk;
     XtPointer garbage;
{
  if (global_tree_info == NULL)
    {
      /* "No widget/style tree is available" */
      SetMessage(global_screen_data.info_label,
		 res_labels[17],
		 "No widget/style tree is available");
      return;
    }   /* if (no widget tree) */

  /* closes the print dialog */
  if (global_screen_data.tree_dialog)
    {
      XtPopdown(global_screen_data.tree_dialog);
    }

  /* Restore Root */
  _newTopParent(TRUE);

  /* Unfold All */
  _treeFold(global_tree_info->top_node,
	    UnfoldAll);

  displayNewTree();

}   /* showAll() */


/* Function Name:
 *   ChangeFindType
 *
 * Description:
 *   Sets the new type for the search activities:
 *   call_data has the new type of search
 *
 * Arguments:
 *   w - the widget that activated this.
 *   type - type of search:
 *               FindTypeAll, FindTypeNameOrClass, FindTypeName, FindTypeClass,
 *               FindTypeAnyID, FindTypeWinID, FindTypeWidID
 *   state_ptr - state of the toggle
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   none.
 */
void
ChangeFindType(Widget w, XtPointer type, XtPointer state_ptr)
{
#if 0
	Boolean   state = (Boolean) state_ptr;
	Arg       args[1];
	Cardinal  num_args;

	/* The original version of Xaw defines the callback data "state_ptr"
	 * as an (Boolean), asuming that sizeof(Boolean) <= sizeof(XtPointer).
	 * But some versions of Xaw3d "corrected" this as (Boolean*).
	 */
	if ((((Boolean)state_ptr) == FALSE) || (((Boolean)state_ptr) == TRUE)) {
		/* original version style */
		state = (Boolean)state_ptr;
	} else {
		/* unofficial version style */
		/* if "state_ptr" isn't \000 (FALSE) or \001 (TRUE) it must be a pointer */
		state = *(Boolean*)state_ptr;
	}

	if (state) {
		global_findType = (FindTypes)type;
	}
#endif
}   /* ChangeFindType() */


/************************************************************
 *
 * Callbacks for the Resource Box.
 *
 ************************************************************/


/* Function Name:
 *   AnyChosen
 *
 * Description:
 *   Callback that is called when the "any" widget is activated.
 *
 * Arguments:
 *   w - the "any" widget that activated this callback.
 *   any_info_ptr - pointer to struct containing
 *                  dot and star widgets to lock.
 *   state_ptr - state of the any toggle.
 *
 * Returns:
 *   none.
 *
 * Calls:
 */

/* ARGSUSED */
void
AnyChosen(w, any_info_ptr, state_ptr)
     Widget    w;
     XtPointer any_info_ptr;
     XtPointer state_ptr;
{
#if 0
  AnyInfo *any_info = (AnyInfo*)any_info_ptr;
  Boolean state;
  Arg     args[1];


  /* The original version of Xaw defines the callback data "state_ptr"
   * as an (Boolean), asuming that sizeof(Boolean) <= sizeof(XtPointer).
   * But some versions of Xaw3d "corrected" this as (Boolean*).
   */
  if ( ((Boolean)state_ptr == FALSE)
       || ((Boolean)state_ptr == TRUE) )
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
      if (any_info->left_count == 0)
	{
	  XtSetSensitive(any_info->left_dot, FALSE);
	  XtSetSensitive(any_info->left_star, FALSE);
#if 0
	  XtSetArg(args[0], XtNstate, TRUE);
	  XtSetValues(any_info->left_star, args, ONE);
#endif
	}

      if ( (any_info->right_count == NULL) || (*any_info->right_count == 0) )
	{
	  XtSetSensitive(any_info->right_dot, FALSE);
	  XtSetSensitive(any_info->right_star, FALSE);

#if 0
	  XtSetArg(args[0], XtNstate, TRUE);
	  XtSetValues(any_info->right_star, args, 1);
#endif
	}

      any_info->left_count++;

      if (any_info->right_count != NULL)
	{
	  (*any_info->right_count)++;
	}
    }   /* if (state == TRUE) */

    else
      {			/* state == 0 */
	if (any_info->left_count > 0)
	  {
	    any_info->left_count--;
	  }
	if ((any_info->right_count != NULL)&&(*any_info->right_count > 0))
	  {
	    (*any_info->right_count)--;
	  }

	if (any_info->left_count == 0)
	  {
	    XtSetSensitive(any_info->left_dot, TRUE);
	    XtSetSensitive(any_info->left_star, TRUE);

#if 0
	    XtSetArg(args[0], XtNstate, TRUE);
	    XtSetValues(any_info->left_dot, args, 1);
#endif
	  }

	if ((any_info->right_count == NULL)||(*any_info->right_count == 0))
	  {
	    XtSetSensitive(any_info->right_dot, TRUE);
	    XtSetSensitive(any_info->right_star, TRUE);

#if 0
	    XtSetArg(args[0], XtNstate, TRUE);
	    XtSetValues(any_info->right_dot, args, 1);
#endif
	  }
      }

  SetResourceString(NULL, (XtPointer)any_info->node, NULL);
  ActivateResourceWidgets(NULL, (XtPointer)any_info->node, NULL);
#endif
}   /* AnyChosen() */


/* Function Name:
 *   GetResourceName
 *
 * Description:
 *   Gets the name of the current resource.
 *
 * Arguments:
 *   res_box - the resource box.
 *
 * Returns:
 *   the name of the currently selected resource.
 *
 * Calls:
 */
static char
*GetResourceName(ResourceBoxInfo *res_box)
{
#if 1
	return NULL;
#else
  XawListReturnStruct *list_info;
  char                *result;


  list_info = XawListShowCurrent(res_box->norm_list);

  if ((list_info->list_index == XAW_LIST_NONE)
      && (res_box->cons_list != NULL))
    {
      list_info = XawListShowCurrent(res_box->cons_list);
    }

  if (list_info->list_index == XAW_LIST_NONE)
    {
      result = "unknown";
    }

  else
    {
      result = list_info->string;
    }

    return(result);
#endif
}   /* GetResourceName() */


/* Function Name:
 *   ActivateWidgetsAndSetResourceString
 *
 * Description:
 *   Sets the new resources string, then
 *   activates all widgets that match this resource,
 *
 * Arguments:
 *   w - the widget that activated this.
 *   node_ptr - the node that owns this resource box.
 *   call_data - passed on to other callbacks.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   SetResourceString - (self)
 *   ActivateResourceWidgets -
 *
 * NOTE: I cannot just have two callback routines, since I care which
 *       order that these are executed in, sigh...
 */
void
ActivateWidgetsAndSetResourceString(w, node_ptr, call_data)
     Widget    w;
     XtPointer node_ptr;
     XtPointer call_data;
{
  SetResourceString(w, node_ptr, call_data);
  ActivateResourceWidgets(w, node_ptr, call_data);
}   /* ActivateWidgetsAndSetResourceString() */


/* Function Name:
 *   SetResourceString
 *
 * Description:
 *   Sets the resource label to correspond to the currently chosen string.
 *
 * Arguments:
 *   w - The widget that invoked this callback, or NULL.
 *   node_ptr - pointer to widget node contating this res box.
 *   call_data - The call data for the action that invoked this callback.
 *
 * Returns:
 *   none.
 *
 * Calls:
 */
void
SetResourceString(Widget w, XtPointer node_ptr, XtPointer call_data)
{
	fprintf(stderr, "SetResourceString(%s)\n", XtName(w));
#if 0
  static char     *malloc_string;   /* These are both inited to zero. */
  static Cardinal malloc_size;

  WNode           *node = (WNode*)node_ptr;
  ResourceBoxInfo *res_box = node->resources->res_box;
  char            *temp;
  char            buf[BUFSIZ * 10];	/* here's hoping it's big enough. */
  NameInfo        *name_node = res_box->name_info;
  Arg             args[1];
  int             len;
  Boolean         state;


  if ((w != NULL) && XtIsSubclass(w, toggleWidgetClass))
    {
      /*
       * Only set resources when toggles are activated, not when they are
       * deactivated.
       */

      /* The original version of Xaw defines the callback data "state_ptr"
       * as an (Boolean), asuming that sizeof(Boolean) <= sizeof(XtPointer).
       * But some versions of Xaw3d "corrected" this as (Boolean*).
       */
      if ( ((Boolean)call_data == FALSE)
	   || ((Boolean)call_data == TRUE) )
	{
	  /* original version style */
	  state = (Boolean)call_data;
	}
      else
	{
	  /* unofficial version style */
	  /* if "call_data" isn't \000 (FALSE) or \001 (TRUE) it must be a pointer */
	  state = *(Boolean*)call_data;
	}

      if (!state)
	{
	  return;
	}
    }   /* if (it's really a toggleWidget) */

  buf[0] = '\0';		/* clear out string. */

  /*
   * Get the widget name/class info.
   */

  if ( (temp = (char*)XawToggleGetCurrent(name_node->sep_leader)) != NULL )
    {
      strcat(buf, temp);
    }

  while (name_node->next != NULL)
    {
      temp = (char*)XawToggleGetCurrent(name_node->name_leader);

      if ( (temp != NULL) && strcmp(temp, ANY_RADIO_DATA) != 0)
	{
	  strcat(buf, temp);
	  temp = (char*)XawToggleGetCurrent(name_node->next->sep_leader);

	  if (temp == NULL)
	    {
	      strcat(buf, "!");
	    }
	  else
	    {
	      strcat(buf, temp);
	    }

	}

      name_node = name_node->next;

    }   /* while() */
		
  strcat(buf, GetResourceName(res_box));
  len = strlen(buf) + 2;   /* Leave space for ':' and '\0' */

#ifdef notdef
  XtSetArg(args[0], XtNstring, &temp);
  XtGetValues(res_box->value_wid,
	      args, ONE);
  len += strlen(temp);
#endif

  if (len > malloc_size)
    {
      malloc_string = XtRealloc(malloc_string, sizeof(char) * len);
      malloc_size = len;
    }

  strcpy(malloc_string, buf);
  strcat(malloc_string, ":");
#ifdef notdef
  strcat(malloc_string, temp);
#endif

  XtSetArg(args[0],
	   XtNlabel, malloc_string);
  XtSetValues(res_box->res_label,
	      args, ONE);
#endif
}   /* SetResourceString() */


/* Function Name:
 *   ResourceListCallback
 *
 * Description:
 *   Callback functions for the resource lists.  This routine is
 *   essentialy called by the list widgets Notify action. If
 *   action EnableGetVal has been invoked,  ResourceListCallback
 *   will perform a GetValues protocol request.
 *
 * Arguments:
 *   list - the list widget that we are dealing with.
 *   node_ptr - pointer to widget node contating this res box.
 *   junk - ** NOT USED **
 *
 * Returns:
 *   none
 *
 * Calls:
 */

void
ResourceListCallback(Widget list, XtPointer node_ptr, XtPointer junk)
{
  Widget          o_list;
  WNode           *node = (WNode*)node_ptr;
  ResourceBoxInfo *res_box = node->resources->res_box;


  if (list == res_box->norm_list)
    {
      o_list = res_box->cons_list;
    }
  else
    {
      o_list = res_box->norm_list;
    }

  if (o_list != NULL)
    {
/*      XawListUnhighlight(o_list); */
    }

  SetResourceString(list,
		    node_ptr,
		    junk);

  /* get the resource value from the application */
  if (global_effective_protocol_version >= PROTOCOL_VERSION_ONE_POINT_ONE
      && do_get_values)
    {
      ObtainResource(node_ptr);
      do_get_values = False;
    }

}   /* ResourceListCallback() */

void GetResource(Widget w, XtPointer client, XtPointer call)
{
	XmString		*items;
	WNode			*node = (WNode *)client;
	ProtocolStream		*stream = &(global_client.stream);
	char			*resnm;
	int			count;
	Widget			list;

	list = XtNameToWidget(XtParent(w), "*resourcelist");

	XtVaGetValues(list,
		XmNselectedItems, &items,
		XmNselectedItemCount, &count,
		NULL);
	XmStringGetLtoR(items[0], XmSTRING_DEFAULT_CHARSET, &resnm);

	_XEditResResetStream(stream);
	_XEditResPutString8(stream, resnm);   /* insert name */

	/* Insert the widget count, always 1 */
	_XEditResPut16(stream, 1);

	InsertWidgetFromNode(stream, node);

	SetCommand(node->tree_info->tree_widget, LocalGetValues, NULL);
}

/* Function Name:
 *   PopdownResBox
 *
 * Description:
 *   Pops down the resource box.
 *
 * Arguments:
 *   w - ** NOT USED **
 *   shell_ptr - pointer to the shell to pop down.
 *   junk - ** NOT USED **
 *
 * Returns:
 *   none
 *
 * Calls:
 */
/* ARGSUSED */
void
PopdownResBox(Widget w, XtPointer shell_ptr, XtPointer junk)
{
  Widget  shell = (Widget)shell_ptr;


  XtPopdown(shell);
  XtDestroyWidget(shell);
}   /* PopdownResBox() */


/* Function Name:
 *   _AppendResourceString
 *
 * Description:
 *   Append resource string to resource file
 *
 * Arguments:
 *   w -
 *   res_box_ptr -
 *   filename_ptr -
 *
 * Returns:
 *   none
 *
 * Calls:
 */
/* ARGSUSED */
static void
_AppendResourceString(Widget w, XtPointer res_box_ptr, XtPointer filename_ptr)
{
  Arg             args[1];
  FILE            *fp;
  char            buf[BUFSIZ];
  char            *resource_string;
  char            *filename = (char*)filename_ptr;
  ResourceBoxInfo *res_box = (ResourceBoxInfo*)res_box_ptr;
  char            *value_ptr;


    if (filename != NULL)
      {
	if (global_resources.allocated_save_resources_file)
	  {
	    XtFree(global_resources.save_resources_file);
	  }
	else
	  {
	    global_resources.allocated_save_resources_file = TRUE;
	  }
	
	global_resources.save_resources_file = XtNewString(filename);
      }   /* if (filename exists) */

    if ( (fp = fopen(global_resources.save_resources_file, "a+")) == NULL )
      {
	sprintf(buf,
		"Unable to open this file for writing, would %s",
		"you like to try again?");
	_PopupFileDialog(global_toplevel,
			 buf,
			 global_resources.save_resources_file,
			 _AppendResourceString, res_box_ptr);
	return;
      }   /* if (fopen fails) */

    XtSetArg(args[0], XtNlabel, &resource_string);
    XtGetValues(res_box->res_label, args, 1);

    XtSetArg(args[0], XtNstring, &value_ptr);
    XtGetValues(res_box->value_wid, args, 1);

    fprintf(fp, "%s %s\n", resource_string, value_ptr);

    fclose(fp);

}   /* _AppendResourceString() */


/* Function Name:
 *   SaveResource
 *
 * Description:
 *   Save the current resource to your resource file
 *
 * Arguments:
 *   w - any widget in the application.
 *   res_box_ptr - the resource box info.
 *   junk - NOT USED.
 *
 * Returns:
 *   none
 *
 * Calls:
 */
/* ARGSUSED */
void
SaveResource(Widget w, XtPointer res_box_ptr, XtPointer junk)
{
  /*
   * If there is no filename the ask for one, otherwise just save to
   * current file.
   */

  if (strcmp(global_resources.save_resources_file, "") == 0) {
      _PopupFileDialog(XtParent(w),
		       "Enter file to dump resources into:",
		       global_resources.save_resources_file,
		       _AppendResourceString,
		       res_box_ptr);
    }   /* if (no filename given since now) */
  else
    {
      _AppendResourceString(w, res_box_ptr, NULL);
    }   /* else (filename for save resources known) */

}   /* SaveResource() */


/* Function Name:
 *   _SetResourcesFile
 *
 * Description:
 *   Sets the filename of the file to save the resources to.
 *
 * Arguments:
 *   w - ** NOT USED **
 *   junk - ** NOT USED **
 *   filename_ptr - a pointer to the filename;
 *
 * Returns:
 *   none
 *
 * Calls:
 */
/* ARGSUSED */
static void
_SetResourcesFile(Widget w, XtPointer junk, XtPointer filename_ptr)
{
  char *filename = (char *) filename_ptr;


  if (global_resources.allocated_save_resources_file)
    {
      XtFree(global_resources.save_resources_file);
    }
  else
    {
      global_resources.allocated_save_resources_file = TRUE;
    }

  global_resources.save_resources_file = XtNewString(filename);

}   /* _SetResourceFile() */


/* Function Name:
 *   SetFile
 *
 * Description:
 *   Changes the current save file
 *
 * Arguments:
 *   w - ** NOT USED **
 *   res_box_ptr - ** NOT USED **
 *   junk - ** NOT USED **
 *
 * Returns:
 *   none
 *
 * Calls:
 */
/* ARGSUSED */
void
SetFile(Widget w, XtPointer junk, XtPointer garbage)
{
  /*
   * If there is no filename the ask for one, otherwise just save to
   * current file.
   */

  _PopupFileDialog(XtParent(w),
		   "Enter a new filename to dump resources into:",
		   global_resources.save_resources_file,
		   _SetResourcesFile,
		   NULL);
}   /* SetFile() */


/* Function Name:
 *   ApplyResource
 *
 * Description:
 *   Apply the current resource to the running application.
 *
 * Arguments:
 *   w - any widget in the application.
 *   node_ptr - a pointer to the node containing the current resouce box.
 *   junk - ** NOT USED **
 *
 * Returns:
 *   none
 *
 * Calls:
 *   GetResourceValueForSetValues - setValues.c
 */
/* ARGSUSED */
void
ApplyResource(Widget w, XtPointer node_ptr, XtPointer junk)
{
}   /* ApplyResource() */


/* Function Name:
 *   ObtainResource
 *
 * Description:
 *   Obtain the current resource from the running application.
 *
 * Arguments:
 *   node_ptr - a pointer to the node containing the current resouce box.
 *
 * Returns:
 *   none
 *
 * Calls:
 */
/* ARGSUSED */
static void
ObtainResource(XtPointer node_ptr)
{
  ProtocolStream      *stream = &(global_client.stream);
  ObtainResourcesInfo info;
  WNode               *node = (WNode*)node_ptr;	
  char                *value;
  Arg                 args[1];


  info.name = GetResourceName(node->resources->res_box);
  info.class = "IGNORE_ME";	/* Not currently used.  */
  info.stream = stream;
  info.count = 1;

  XtSetArg(args[0], XtNlabel, &value);
  XtGetValues(node->resources->res_box->res_label, args, 1); 
  info.database = NULL;
  XrmPutLineResource(&(info.database), value);

  _XEditResResetStream(stream);
  _XEditResPutString8(stream, info.name);   /* insert name */

  /*
   * Insert the widget count, always 1
   */

  _XEditResPut16(stream, 1);

  /*CreateGetValuesCommand(node, (XtPointer)&info);  Inserts widget */

  /* Insert widget */
  /* _XEditResPut16(stream, 1); */
  /* _XEditResPut32(stream, node->id); */
  InsertWidgetFromNode(stream, node);

  SetCommand(node->tree_info->tree_widget, LocalGetValues, NULL);

}   /* ObtainResource() */


/* Function Name:
 *   CreateSetValuesCommand
 *
 * Description:
 *   Creates the SetValues command if this widget
 *   matches the resource string in the database.
 *
 * Arguments:
 *   node - the current node.
 *   info_ptr - the pointer to the apply info.
 *
 * Returns:
 *   none
 *
 * Calls:
 */
static void
CreateSetValuesCommand(WNode *node, XtPointer info_ptr)
{
	ApplyResourcesInfo *info = (ApplyResourcesInfo*)info_ptr;
	XrmNameList        name_quarks;
	XrmClassList       class_quarks;
	char               **names, **classes;

	GetNamesAndClasses(node, &names, &classes);
	name_quarks = (XrmNameList)Quarkify(names, info->name);
	class_quarks = (XrmNameList)Quarkify(classes, info->class);

	if (CheckDatabase(info->database, name_quarks, class_quarks)) {
		InsertWidgetFromNode(info->stream, node);
		info->count++;
	}

	XtFree((char*)names);
 	XtFree((char*)classes);
	XtFree((char*)name_quarks);
	XtFree((char*)class_quarks);
}   /* CreateSetValuesCommand() */

#if 0
/* Function Name:
 *   CreateGetValuesCommand
 *
 * Description:
 *   Creates the GetValues command.
 *
 * Arguments:
 *   node - the current node.
 *   info_ptr - the pointer to the apply info.
 *
 * Returns:
 *   none
 *
 * Calls:
 */
static void
CreateGetValuesCommand(WNode *node, XtPointer info_ptr)
{
	ApplyResourcesInfo *info = (ApplyResourcesInfo*)info_ptr;
	XrmNameList        name_quarks;
	XrmClassList       class_quarks;
	char               **names;
	char               **classes;

	GetNamesAndClasses(node, &names, &classes);
	name_quarks = (XrmNameList)Quarkify(names, info->name);
	class_quarks = (XrmNameList)Quarkify(classes, info->class);

	if (CheckDatabase(info->database, name_quarks, class_quarks)) {
	InsertWidgetFromNode(info->stream, node);
		info->count++;
	}

	XtFree ((char *)names);
	XtFree ((char *)classes);
	XtFree ((char *)name_quarks);
	XtFree ((char *)class_quarks);
}   /* CreateGetValuesCommand() */
#endif

/* Function Name:
 *   ActivateResourceWidgets
 *
 * Description:
 *   Activates all widgets that match this resource.
 *
 * Arguments:
 *   w - ** NOT USED **
 *   node_ptr - the node that owns this resource box.
 *   junk - ** NOT USED **
 *
 * Returns:
 *   none.
 *
 * Calls:
 */
/* ARGSUSED */
void
ActivateResourceWidgets(Widget w, XtPointer node_ptr, XtPointer junk)
{
  WNode              *node = (WNode*)node_ptr;	
  ApplyResourcesInfo info;
  char               *line;
  Arg                args[1];


  info.name = GetResourceName(node->resources->res_box);
  info.class = "IGNORE_ME";	/* Not currently used.  */

  /*
   * Unused fields.
   */

  info.count = 0;
  info.stream = NULL;

  XtSetArg(args[0], XtNlabel, &line);
  XtGetValues(node->resources->res_box->res_label, args, 1);

  info.database = NULL;
  XrmPutLineResource(&(info.database),
		     line);


  ExecuteOverAllNodes(node->tree_info->top_node,
		      SetOnlyMatchingWidgets,
		      (XtPointer)&info);

  XrmDestroyDatabase(info.database);

}   /* ActivateResourceWidgets() */


/* Function Name:
 *   SetOnlyMatchingWidgets
 *
 * Description:
 *   Activates all widgets in the tree that match this
 *   resource specification.
 *
 * Arguments:
 *   node - the current node.
 *   info_ptr - the pointer to the apply info.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   notShown - wtree.c
 *   GetNamesAndClasses -
 *   CheckDatabase -
 *   Quarkify -
 *   TreeToggle -
 */
static void
SetOnlyMatchingWidgets(WNode *node, XtPointer info_ptr)
{
#if 0
  ApplyResourcesInfo *info = (ApplyResourcesInfo*)info_ptr;
  XrmNameList        name_quarks;
  XrmClassList       class_quarks;
  char               **names;
  char               **classes;
  Boolean            state;
  Arg                args[1];


  /* if this node is not shown, we can return */
  if ( (node->widget == NULL) || (notShown(node)) )
    {
      return;
    }

  GetNamesAndClasses(node,
		     &names,
		     &classes);
  name_quarks = (XrmNameList)Quarkify(names,
				      info->name);
  class_quarks = (XrmNameList)Quarkify(classes,
				       info->class);

  state = CheckDatabase(info->database,
			name_quarks,
			class_quarks);

#if 0
  XtSetArg(args[0], XtNstate, state);
  XtSetValues(node->widget, args, 1);
#endif
  TreeToggle(node->widget, (XtPointer)node, (XtPointer)state);

  XtFree ((char *)names);
  XtFree ((char *)classes);
  XtFree ((char *)name_quarks);
  XtFree ((char *)class_quarks);
#endif
}   /* SetOnlyMatchingWidgets() */


/* Function Name:
 *   printExpose
 *
 * Description:
 *   Event handler for the expose of the graphical tree to print
 *
 * Arguments:
 *   (normal event handler parameters)
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   _drawNode - (self)
 */
void
printExpose(Widget w, XtPointer client_data, XEvent *event, Boolean *cont)
{
  XGCValues gcVal;
  XColor    col;


  /* the GC could first be created if the window is created (w is realized) */
  if (global_draw_gc == NULL)
    {
      gcVal.foreground = BlackPixel(XtDisplay(w), 0);
      gcVal.font = global_draw_font->fid;
      global_draw_gc = XCreateGC(XtDisplay(w),
				 XtWindow(w),
				 GCForeground | GCFont,
				 &gcVal);
    }
  if (global_fill_gc == NULL)
    {
      /* 80% gray: rgb:cccccc */
      col.red = 204;
      col.green = 204;
      col.blue = 204;
      col.flags = DoRed | DoGreen | DoBlue;
      if ((XAllocColor(XtDisplay(w),
		       DefaultColormap(XtDisplay(w), 0),
		       &col) == 0)
	  || col.red != 204
	  || col.green != 204
	  || col.blue != 204)
	
	{
	  /* Fallback */
	  gcVal.foreground = WhitePixel(XtDisplay(w), 0);
	}
      else
	{
	  gcVal.foreground = col.pixel;
	}

      global_fill_gc = XCreateGC(XtDisplay(w),
				 XtWindow(w),
				 GCForeground,
				 &gcVal);
    }

  _drawNode(w, global_tree_info->newtop_node);

}   /* printExpose() */


/* Function Name:
 *   _drawNode
 *
 * Description:
 *   Draws the given node for printing.
 *
 * Arguments:
 *   wid  - the widget to draw into
 *   node - the node we are looking at
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   _drawNode - (recursive)
 */
static void
_drawNode(Widget wid, WNode *node)
{
  Window win = XtWindow(wid);
  int    i;


  if (IsActiveNode(node))
    {
      /* to get the full rectangle filled we must add 1 to the dimensions */
      XFillRectangle (XtDisplay(wid),
		      win,
		      global_fill_gc,
		      node->item.x,
		      node->item.y,
		      node->item.width + 1,
		      node->item.height + 1);
    }
  else
    {
      XDrawRectangle (XtDisplay(wid),
		      win,
		      global_draw_gc,
		      node->item.x,
		      node->item.y,
		      node->item.width,
		      node->item.height);
    }

  /* draw the label of the node in the rectangle */
  if (node->label != NULL)
    {
      XDrawString (XtDisplay(wid),
		   win,
		   global_draw_gc,
		   node->item.x,
		   node->item.y
		   + node->item.height - global_draw_font->max_bounds.descent,
		   node->label,
		   strlen(node->label));
    }
  else
    {
      XDrawString (XtDisplay(wid),
		   win,
		   global_draw_gc,
		   node->item.x,
		   node->item.y
		   + node->item.height - global_draw_font->max_bounds.descent,
		   "(NULL)?",
		   strlen("(NULL)?"));
    }

  for (i=0; i < node->num_children; i++)
    {
      if (!notShown(node->children[i]))
	{
	  XDrawLine(XtDisplay(wid),
		    win,
		    global_draw_gc,
		    node->item.x + node->item.width,
		    node->item.y + (node->item.height / 2),
		    node->children[i]->item.x,
		    node->children[i]->item.y
		    + (node->children[i]->item.height / 2));

	  _drawNode(wid, node->children[i]);
	}
    }   /* for (all children) */

}   /* _drawNode() */


/* >>>>> HACK for batchres */

/* Function Name:
 *   brApplyResource
 *
 * Description:
 *   Apply the current resource to the running application.
 *
 * Arguments:
 *   resPath - Path to the resource (i.e. ".MyClass*myDlg*", "*WMShell.")
 *   resName - Name of the resource
 *   resValue - Value to set
 *
 * Returns:
 *   none
 *
 * Calls:
 *   brGetResourceValueForSetValues - setValues.c
 *   brSetComamnd - comm.c
 *   CutSpace - utils.c
 *   NextLine - wtree.c
 */
/* ARGSUSED */
void
brApplyResource(char *resPath, char *resName, char *resValue)
{
  static char        *value = NULL;
  static Cardinal    valSize = 0;

  ProtocolStream     *stream = &(global_client.stream);
  ApplyResourcesInfo info;
  unsigned short     size;
  unsigned short     i;
  long               len;
  char               msg[BUFSIZ];
  char               *val;


  info.name = resName;
  info.class = "IGNORE_ME";	/* Not currently used.  */
  info.stream = stream;
  info.count = 0;

  if (valSize < sizeof(char) * (strlen(resPath) + strlen(resName) + 2))
    {
      valSize = (sizeof(char) * (strlen(resPath) + strlen(resName) + 2));
      if ( (value = XtRealloc(value, valSize)) == NULL )
	{
	  fprintf(stderr, "XtRealloc failed!\n");
	  exit(EXIT_FAILURE);
	}
    }

  sprintf(value,
	  "%s%s:",
	  resPath,
	  resName);

  info.database = NULL;      /* create a new database */
  XrmPutLineResource(&(info.database),
		     value);

  _XEditResResetStream(stream);
  _XEditResPutString8(stream, info.name);   /* Insert name */
  _XEditResPutString8(stream, XtRString);   /* insert type */

  /*
   * Insert value.
   */

  val = brGetResourceValueForSetValues(resValue,
				       &size);
  _XEditResPut16(stream, size);
  for (i = 0; i < size; i++)
    {
      _XEditResPut8(stream, val[i]);
    }   /* for */

  len = stream->current - stream->top;

  /*
   * Insert the widget count, overriden later.
   */

  _XEditResPut16(stream, 0);

  ExecuteOverAllNodes(global_tree_info->top_node,
		      CreateSetValuesCommand,
		      (XtPointer)&info);

  if (info.count > 0)
    {
      *(stream->top + len++) = info.count >> XER_NBBY;   /* Set the correct */
      *(stream->top + len) = info.count;                 /* count. */

      brSetCommand(global_tree_info->tree_widget,
		   LocalSetValues,
		   NULL);
    }
  else
    {
      /* "ApplyResource: found no matches for %s%s:%s" */
      if (res_labels[20])
	{
	  sprintf(msg,
		  res_labels[20],
		  resPath,
		  resName,
		  resValue);
	}
      else
	{
	  sprintf(msg,
		  "ApplyResource: found no matches for %s%s:%s",
		  resPath,
		  resName,
		  resValue);
	}
      SetMessage(global_screen_data.info_label,
		 msg,
		 "?res_labels[20]");
      NextLine(NULL);
    }
	
  XrmDestroyDatabase(info.database);

}   /* brApplyResource() */


/* Function Name:
 *   brConnectToClient
 *
 * Description:
 *
 * Arguments:
 *   client_data - *** NOT USED ***
 *   timerID - ID of TimerCallback
 *
 * Returns:
 *   none
 *
 * Calls:
 *   SendTree - (self)
 */
/* ARGSUSED */
void
brConnectToClient(XtPointer client_data, XtIntervalId *timerID)
{

  SendTree (global_toplevel,
	    (XtPointer)FALSE,  /* do not ask for a new client */
	    NULL);             /* not used */

}   /* brConnectToClient() */

/* <<<<< HACK for batchres */
