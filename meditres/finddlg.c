/*
 * $Header: /cvsroot-fuse/meditres/meditres/finddlg.c,v 1.7 2002/01/16 20:05:43 amai Exp $
 *
 * This is part of the program "editres2",
 * based on "editres" from Chris D. Peterson, MIT X Consortium
 *
 * Volker Gering, 1997
 */

#include "config.h"

#include <stdio.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>	/* Get standard string definations. */
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Shell.h>

#include "meditres.h"


static void CreateFindDlg(Widget parent, ScreenData *scr_data);


/* Function Name:
 *   PopupFindDialog
 *
 * Description:
 *   This function pops up the find dialog
 *
 * Arguments:
 *   parent - the parent of the find popup.
 *   event - the event that caused the action.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   SetMessage - utils.c
 *   CreateFindDlg - (self)
 */
/* ARGSUSED */
void
PopupFindDialog(Widget parent, XEvent *event)
{
  Arg args[1];


  if (global_tree_info == NULL)
    {
      /* "No widget/style tree is available" */
      SetMessage(global_screen_data.info_label,
		 res_labels[17],
		 "No widget/style tree is available");
      return;
    }

  /* 
   * Check and possibly create the popup.
   */
  if (global_screen_data.find_popup == NULL)
    {
      CreateFindDlg(parent, &global_screen_data);
    }

  /*
   * Pop it up.
   */
  PopupCentered(event, global_screen_data.find_popup, XtGrabNone);

}   /* PopupFindDialog() */


/* Function Name:
 *   FindEntry
 *
 * Description:
 *   Action routine that can be bound to the find dialog
 *   to search for an entry
 *
 * Arguments:
 *   (Standard Action Routine args) 
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   SetMessage - utils.c
 */
/* ARGSUSED */
void 
FindEntry(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
#if 0
  FindTypes type;


  if (*num_params > 1)
    {
      SetMessage(global_screen_data.info_label,
		 res_labels[43],
		 "Error: Search Action must have zero or one argument");
      return;
    }   /* if (num_params > 1) */

  if (*num_params == 0)
    {
      DoFind(NULL,    /* dummy widget */
	     NULL,    /* use global_findType */
	     NULL);   /* ** UNUSED ** */
    }

  else
    {
      if (!strcasecmp(params[0], "All"))
	{
	  type = FindTypeAll;
	}   /* if (All) */

      else if (!strcasecmp(params[0], "NameOrClass"))
	{
	  type = FindTypeNameOrClass;
	}   /* if (NameOrClass) */

      else if (!strcasecmp(params[0], "Class"))
	{
	  type = FindTypeName;
	}   /* if (Name) */

      else if (!strcasecmp(params[0], "Name"))
	{
	  type = FindTypeClass;
	}   /* if (Class) */

      else if (!strcasecmp(params[0], "AnyID"))
	{
	  type = FindTypeAnyID;
	}   /* if (AnyID) */

      else if (!strcasecmp(params[0], "WinID"))
	{
	  type = FindTypeWinID;
	}   /* if (WinID) */

      else if (!strcasecmp(params[0], "WidID"))
	{
	  type = FindTypeWidID;
	}   /* if (WidID) */

      else
	{
	  SetMessage(global_screen_data.info_label,
		     res_labels[44],
		     "Error: Search Actions's first Argument must be 'All', 'NameOrClass', 'Name', 'Class', 'AnyID', 'WinID' or 'WidID'");
	  return;
	}

      DoFind (NULL, &type, NULL);
    }
#endif    
}   /* FindEntry() */


/* Function Name:
 *   SearchPopup
 *
 * Description:
 *   Action routine that shows the find dialog.
 *
 * Arguments:
 *   (Standard Action Routine args) 
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   PopupFindDialog - (self)
 */
/* ARGSUSED */
void 
SearchPopup(Widget w, XEvent *event, String *params, Cardinal *num_params)
{

  PopupFindDialog(w, event);
    
}   /* FindEntry() */


/************************************************************
 *
 * Private Functions
 *
 ************************************************************/

/* Function Name:
 *   CreateFindDlg
 *
 * Description:
 *   Creates the find popup.
 *
 * Arguments:
 *   parent - the parent of the popup.
 *   scr_data - the data about this screen.
 *
 * Returns:
 *   the find popup in the global variable
 *   "scr_data->find_popup"
 *
 * Calls:
 *   Xt-stuff
 *
 * Callbacks installed:
 *   ChangeFindType - handler.c
 *   DoFind - (self)
 *   CancelFind - (self)
 */
static void
CreateFindDlg(Widget parent, ScreenData *scr_data)
{
#if 0
  Widget     form;
  Widget     label;
  Widget     toggle;
  Widget     do_it;
  Widget     cancel;
  Arg        args[10];
  Cardinal   num_args;

    
  scr_data->find_popup = XtCreatePopupShell("findPopup", 
					    transientShellWidgetClass, 
					    parent,
					    NULL, ZERO);

  form = XtCreateManagedWidget("findForm",
			       formWidgetClass, 
			       scr_data->find_popup,
			       NULL, ZERO);

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNborderWidth, 0);
  num_args++;
  label = XtCreateManagedWidget("findLabel",
				labelWidgetClass,
				form,
				args, num_args);


  num_args = 0;
  XtSetArg(args[num_args],
	   XtNtranslations, XtParseTranslationTable(
             "<EnterWindow>:         highlight(Always) \n\
              <LeaveWindow>:         unhighlight() \n\
              <Btn1Down>, <Btn1Up>:  set() notify() "));
  num_args++;
  XtSetArg(args[num_args],
	   XtNfromVert, label);
  num_args++;
  XtSetArg(args[num_args],
	   XtNvertDistance, 20);
  num_args++;
  XtSetArg(args[num_args],
	   XtNstate, (XtArgVal) TRUE);
  num_args++;
  toggle = XtCreateManagedWidget("findTypeAll",
				 toggleWidgetClass,
				 form,
				 args, num_args);
  XtAddCallback (toggle,
		 XtNcallback,
		 ChangeFindType,
		 (XtPointer)FindTypeAll);

  num_args = 1;   /* keep the XtNtranslation */
  XtSetArg(args[num_args],
	   XtNfromVert, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNradioGroup, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNhorizDistance, 20);
  num_args++;
  toggle = XtCreateManagedWidget("findTypeNameOrClass",
				 toggleWidgetClass,
				 form,
				 args, num_args);
  XtAddCallback (toggle,
		 XtNcallback,
		 ChangeFindType,
		 (XtPointer)FindTypeNameOrClass);

  num_args = 1;   /* keep the XtNtranslation */
  XtSetArg(args[num_args],
	   XtNfromVert, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNradioGroup, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNhorizDistance, 40);
  num_args++;
  toggle = XtCreateManagedWidget("findTypeName",
				 toggleWidgetClass,
				 form,
				 args, num_args);
  XtAddCallback (toggle,
		 XtNcallback,
		 ChangeFindType,
		 (XtPointer)FindTypeName);

  num_args = 1;   /* keep the XtNtranslation */
  XtSetArg(args[num_args],
	   XtNfromVert, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNradioGroup, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNhorizDistance, 40);
  num_args++;
  toggle = XtCreateManagedWidget("findTypeClass",
				 toggleWidgetClass,
				 form,
				 args, num_args);
  XtAddCallback (toggle,
		 XtNcallback,
		 ChangeFindType,
		 (XtPointer)FindTypeClass);

  num_args = 1;   /* keep the XtNtranslation */
  XtSetArg(args[num_args],
	   XtNfromVert, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNradioGroup, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNhorizDistance, 20);
  num_args++;
  toggle = XtCreateManagedWidget("findTypeAnyID",
				 toggleWidgetClass,
				 form,
				 args, num_args);
  XtAddCallback (toggle,
		 XtNcallback,
		 ChangeFindType,
		 (XtPointer)FindTypeAnyID);

  num_args = 1;   /* keep the XtNtranslation */
  XtSetArg(args[num_args],
	   XtNfromVert, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNradioGroup, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNhorizDistance, 40);
  num_args++;
  toggle = XtCreateManagedWidget("findTypeWinID",
				 toggleWidgetClass,
				 form,
				 args, num_args);
  XtAddCallback (toggle,
		 XtNcallback,
		 ChangeFindType,
		 (XtPointer)FindTypeWinID);

  num_args = 1;   /* keep the XtNtranslation */
  XtSetArg(args[num_args],
	   XtNfromVert, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNradioGroup, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNhorizDistance, 40);
  num_args++;
  toggle = XtCreateManagedWidget("findTypeWidID",
				 toggleWidgetClass,
				 form,
				 args, num_args);
  XtAddCallback (toggle,
		 XtNcallback,
		 ChangeFindType,
		 (XtPointer)FindTypeWidID);

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNfromVert, toggle);
  num_args++;
  XtSetArg(args[num_args],
	   XtNvertDistance, 20);
  num_args++;
  XtSetArg(args[num_args],
	   XtNeditType, XawtextEdit);
  num_args++;
  XtSetArg(args[num_args],
	   XtNresizable, True);
  num_args++;
  XtSetArg(args[num_args],
	   XtNresize, XawtextResizeWidth);
  num_args++;
  scr_data->find_text = XtCreateManagedWidget("findText", 
					      asciiTextWidgetClass,
					      form,
					      args, num_args);

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNfromVert, scr_data->find_text);
  num_args++;
  do_it = XtCreateManagedWidget("searchNext",
				commandWidgetClass, 
				form,
				args, num_args);

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNfromVert, scr_data->find_text);
  num_args++;
  XtSetArg(args[num_args],
	   XtNfromHoriz, do_it);
  num_args++;
  cancel = XtCreateManagedWidget("findCancel",
				 commandWidgetClass,
				 form,
				 args, num_args);

  XtAddCallback(do_it,
		XtNcallback,
		DoFind,
		NULL);
  XtAddCallback(cancel,
		XtNcallback,
		CancelFind,
		NULL);

  /*
   * Initialize the text entry field
   */
  XtSetKeyboardFocus(form,
		     scr_data->find_text);

  /*
   * initialize global var. findType
   */
  global_findType = FindTypeAll;

#endif
}   /* CreateFindDlg() */


#if 0
/* Function Name:
 *   DoFind
 *
 * Description:
 *   Performs a search.
 *
 * Arguments:
 *   w - the widget that called this (** UNUSED **).
 *   ftype - the type of search ((FindTypes*) or global_findType if NULL)
 *   garbage - ** UNUSED **.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   SetMessage - utils.c
 *   _findNode - (self)
 *   SetAndCenterTreeNode - wtree.c
 */
/* ARGSUSED */
static void
DoFind(Widget w, XtPointer ftype, XtPointer garbage)
{
  char           *find_text;
  FindTypes      type;
  Arg            args[1];
  Cardinal       i;


  if (global_screen_data.find_text == NULL)
    {
      SetMessage (global_screen_data.info_label,
		  res_labels[46],
		  "Editres Internal Error: Parameter NULL");
      return;
    }

  /* which text should be found? */
  XtSetArg(args[0],
	   XtNstring, &find_text);
  XtGetValues(global_screen_data.find_text,
	      args, ONE);

  /* get the type of information we are looking for */
  if (ftype != NULL)
    {
      type = *(FindTypes*)ftype;
    }
  else
    {
      type = global_findType;
    }

  /* deselect all */
  _TreeSelect(global_tree_info, SelectNone);

  _searchNode(global_tree_info->top_node,
	    find_text,
	    TRUE);

  /* if only 1 found: Center toggle and flash in client */
  if (global_tree_info->num_active == 1)
    {
      SetAndCenterTreeNode(global_tree_info->active_nodes[0]);
      _FlashActiveWidgets(global_tree_info);
    }

}   /* DoFind() */


/* Function Name:
 *   CancelFind
 *
 * Description:
 *   Pops down the find dialog.
 *
 * Arguments:
 *   w - any grandchild of the popup.
 *   junk - ** UNUSED **
 *   garbage - ** UNUSED **.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 */
/* ARGSUSED */
static void
CancelFind(Widget w, XtPointer junk, XtPointer garbage)
{
  XtPopdown(XtParent(XtParent(w))); 
}   /* CancelSetValues() */


/* Function Name:
 *   _searchNode
 *
 * Description:
 *   Searches a node by name in given node an all descendants.
 *
 * Arguments:
 *   node - node to operate on.
 *   name - name of node to search for.
 *   recurse - whether to continue on down the tree.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   SetMessage - utils.c
 *   notShown - wtree.c
 *   _TreeSelectNode - wtree.c
 */
static void
_searchNode (WNode *node, char *name, Boolean recurse)
{
  int           i;
  Arg           args[1];
  Boolean       found = FALSE;
  unsigned long id;
  char          *check;


  if (node == NULL || name == NULL)
    {
      /* "Editres Internal Error: Paramter NULL" */
      SetMessage (global_screen_data.info_label,
		  res_labels[46],
		  "Editres Internal Error: Parameter NULL");
      return;
    }

  /* if this node and its subtree is not shown and type isn't SelectNone, */
  /* we can return */
  if ( (node->widget == NULL)
#ifndef DEBUG
       || (notShown(node))
#endif   /* DEBUG */
       )
    {
      if ( (recurse) && (node->view == NotShownBefore) )
	{
	  for (i = 0; i < node->num_children; i++)
	    {
	      _searchNode (node->children[i], name, recurse);
	    }   /* for (all children of this node) */
	}

      return;
    }

  switch (global_findType)
    {
    case FindTypeAll:
      id = strtoul(name,
		   &check,    /* with error checking */
		   0);        /* determine base automatically */

      found = (strcmp(node->name, name) == 0
	       || strcmp(node->class, name) == 0
	       || (node->window == id && check != name)
	       || (node->id == id && check != name));
      break;

    case FindTypeNameOrClass:
      found = (strcmp(node->name, name) == 0
	       || strcmp(node->class, name) == 0);
      break;

    case FindTypeName:
      found = (strcmp(node->name, name) == 0);
      break;

    case FindTypeClass:
      found = (strcmp(node->class, name) == 0);
      break;

    case FindTypeAnyID:
      id = strtoul(name,
		   &check,    /* with error checking */
		   0);        /* determine base automatically */

      found = ((node->window == id && check != name)
	       || (node->id == id && check != name));
      break;

    case FindTypeWinID:
      id = strtoul(name,
		   &check,     /* with error checking */
		   0);         /* determine base automatically */

      found = (node->window == id && check != name);
      break;

    case FindTypeWidID:
      id = strtoul(name,
		   &check,     /* with error checking */
		   0);         /* determine base automatically */

      found = (node->id == id && check != name);
      break;

    default:
      /* "Editres Internal Error: Illegal find type" */
      SetMessage (global_screen_data.info_label,
		  res_labels[45],
		  "Editres Internal Error: Illegal find type");
      return;
    }   /* switch(FindType) */

  if (found)
    {
      _TreeSelectNode (node,
		       (XtPointer)SelectAll,
		       (XtPointer)FALSE);   /* not recursive */
    }

  if (!recurse)
    {
      return;
    }

  for (i = 0; i < node->num_children; i++)
    {
      _searchNode (node->children[i], name, recurse);
    }   /* for (all children of this node) */

}   /* _searchNode() */
#endif
