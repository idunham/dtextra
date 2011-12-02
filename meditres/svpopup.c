/*
 * $Header: /cvsroot-fuse/meditres/meditres/svpopup.c,v 1.6 2002/01/16 20:05:43 amai Exp $
 *
 * $XConsortium: svpopup.c,v 1.17 94/04/17 21:45:21 rws Exp $
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
#include <X11/StringDefs.h>	/* Get standard string definations. */
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Shell.h>

#include "meditres.h"


static void _SetField();
static void CreateSetValuesPopup();
static void DoSetValues();
static void CancelSetValues();


/* Function Name:
 *   PopupSetValues
 *
 * Description:
 *   This function pops up the setvalues dialog
 *
 * Arguments:
 *   parent - the parent of the setvalues popup.
 *   event - the event that caused this popup, or NULL.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   SetMessage - utils.c
 *   CreateSetValuesPopup - (self)
 *   SetField - (self)
 *   PopupCentered -
 */
/* ARGSUSED */
void
PopupSetValues(Widget parent, XEvent *event)
{
#if 0
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

  if (global_screen_data.set_values_popup == NULL)
    {
      CreateSetValuesPopup(parent, &global_screen_data);
    }

  /*
   * Clear out the old strings, and set the active widget to the name widget.
   */

  XtSetArg(args[0],
	   XtNstring, "");
  XtSetValues(global_screen_data.res_text,
	      args, ONE);
  XtSetValues(global_screen_data.val_text,
	      args, ONE);

  _SetField(global_screen_data.res_text,
	    global_screen_data.val_text);

  /*
   * Pop it up.
   */

  PopupCentered(event, global_screen_data.set_values_popup, XtGrabNone);
#endif
}   /* PopupSetValues() */


/* Function Name:
 *   ModifySVEntry
 *
 * Description:
 *   Action routine that can be bound to the set values 
 *   dialog box's Text Widget that will send input to the field specified.
 *
 * Arguments:
 *   (Standard Action Routine args) 
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   SetMessage -
 *   _SetField - (self)
 */
/* ARGSUSED */
void 
ModifySVEntry(Widget w, XEvent  *event, String *params, Cardinal *num_params)
{
#if 0
  Widget new;
  Widget old;


  if (*num_params != 1)
    {
      SetMessage(global_screen_data.info_label,
		 res_labels[21],
		 "Error: SVActiveEntry Action must have exactly one argument");
      return;
    }   /* if (num_params != 1) */
    
  switch (params[0][0])
    {
    case 'r':
    case 'R':
      new = global_screen_data.res_text;
      old = global_screen_data.val_text;
      break;

    case 'v':
    case 'V':
      new = global_screen_data.val_text;
      old = global_screen_data.res_text;
      break;

    default:
      SetMessage(global_screen_data.info_label,
		 res_labels[22],
		 "Error: SVActiveEntry Action's first Argument must be either 'Resource' or 'Value'");
      return;

    }   /* switch (params[0,0]) */
    
  _SetField(new, old);
#endif
}   /* ModifySVEntry() */


/************************************************************
 *
 * Private Functions
 *
 ************************************************************/

/* Function Name:
 *   _SetField
 *
 * Description:
 *   Sets the current text entry field.
 *
 * Arguments:
 *   new, old - new and old text fields.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   Xlib- and Xt-stuff
 */
static void
_SetField(Widget new, Widget old)
{
#if 0
  Arg   args[2];
  Pixel new_border;
  Pixel old_border;
  Pixel old_bg;


  if (!XtIsSensitive(new))
    {
      XBell(XtDisplay(old), 0); /* Don't set field to an inactive Widget. */
      return;
    }   /* if (Widget new unsensitive) */

  XtSetKeyboardFocus(XtParent(new),
		     new); 

  /* change the border color: switch between new and old */
  XtSetArg(args[0],
	   XtNborderColor, &old_border);
  XtSetArg(args[1],
	   XtNbackground, &old_bg);
  XtGetValues(new,
	      args, TWO);

  XtSetArg(args[0],
	   XtNborderColor, &new_border);
  XtGetValues(old,
	      args, ONE);

  if (old_border != old_bg)	/* Colors are already correct, return. */
    {
      return;
    }

  XtSetArg(args[0], XtNborderColor, old_border);
  XtSetValues(old, args, ONE);

  XtSetArg(args[0], XtNborderColor, new_border);
  XtSetValues(new, args, ONE);
#endif
}   /* _SetField() */


/* Function Name:
 *   CreateSetValuesPopup
 *
 * Description:
 *   Creates the setvalues popup.
 *
 * Arguments:
 *   parent - the parent of the popup.
 *   scr_data - the data about this screen.
 *
 * Returns:
 *   the set values popup in the global variable
 *   "scr_data->set_values_popup"
 *
 * Calls:
 *   Xt-stuff
 *
 * Callbacks installed:
 *   DoSetValues - (self)
 *   CancelSetValues - (self)
 */
static void
CreateSetValuesPopup(Widget parent, ScreenData *scr_data)
{
#if 0
  Widget     form;
  Widget     cancel;
  Widget     do_it;
  Widget     label;
  Widget     res_label;
  Widget     val_label;
  Dimension  resLabelWidth;
  Dimension  valLabelWidth;
  Arg        args[10];
  Cardinal   num_args;

    
  scr_data->set_values_popup = XtCreatePopupShell("svPopup", 
						  transientShellWidgetClass, 
						  parent,
						  NULL, ZERO);

  form = XtCreateManagedWidget("svForm",
			       formWidgetClass, 
			       scr_data->set_values_popup,
			       NULL, ZERO);

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNborderWidth, 0);
  num_args++;
  label = XtCreateManagedWidget("svLabel",
				labelWidgetClass,
				form,
				args, num_args);


  num_args = 0;
  XtSetArg(args[num_args],
	   XtNfromVert, label);
  num_args++;
  XtSetArg(args[num_args],
	   XtNborderWidth, 0);
  num_args++;
  res_label = XtCreateManagedWidget("svResourceLabel",
				    labelWidgetClass,
				    form,
				    args, num_args);

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNfromVert, label);
  num_args++;
  XtSetArg(args[num_args],
	   XtNeditType, XawtextEdit);
  num_args++;
  XtSetArg(args[num_args],
	   XtNresize, XawtextResizeWidth);
  num_args++;
  scr_data->res_text = XtCreateManagedWidget("svResourceText", 
					     asciiTextWidgetClass,
					     form,
					     args, num_args);

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNfromVert, res_label);
  num_args++;
  XtSetArg(args[num_args],
	   XtNborderWidth, 0);
  num_args++;
  val_label = XtCreateManagedWidget("svValueLabel",
				    labelWidgetClass,
				    form,
				    args, num_args);

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNfromVert, scr_data->res_text);
  num_args++;
  XtSetArg(args[num_args],
	   XtNeditType, XawtextEdit);
  num_args++;
  XtSetArg(args[num_args],
	   XtNresize, XawtextResizeWidth);
  num_args++;
  scr_data->val_text = XtCreateManagedWidget("svValueText", 
					     asciiTextWidgetClass,
					     form,
					     args, num_args);

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNfromVert, scr_data->val_text);
  num_args++;
  do_it = XtCreateManagedWidget("svSetValues",
				commandWidgetClass, 
				form,
				args, num_args);

  num_args = 0;
  XtSetArg(args[num_args],
	   XtNfromVert, scr_data->val_text);
  num_args++;
  XtSetArg(args[num_args],
	   XtNfromHoriz, do_it);
  num_args++;
  cancel = XtCreateManagedWidget("svCancel",
				 commandWidgetClass,
				 form,
				 args, num_args);

  /* horizontal alignment of text fields */
  XtSetArg(args[0],
	   XtNwidth, &resLabelWidth);
  XtGetValues(res_label,
	      args, ONE);
  XtSetArg(args[0],
	   XtNwidth, &valLabelWidth);
  XtGetValues(val_label,
	      args, ONE);

  if (valLabelWidth < resLabelWidth)
    {
      XtSetArg(args[0],
	       XtNfromHoriz, res_label);
    }   /* if (svResourceLabel is wider) */
  else
    {
      XtSetArg(args[0],
	       XtNfromHoriz, val_label);
    }   /* else (svValueLabel ist wider) */
  XtSetValues(scr_data->res_text,
	      args, ONE);
  XtSetValues(scr_data->val_text,
	      args, ONE);

  XtAddCallback(do_it,
		XtNcallback,
		DoSetValues,
		NULL);
  XtAddCallback(cancel,
		XtNcallback,
		CancelSetValues,
		NULL);

  /*
   * Initialize the text entry fields.
   */
  {
    Pixel color;


    num_args = 0;
    XtSetArg(args[num_args],
	     XtNbackground, &color);
    num_args++;
    XtGetValues(scr_data->val_text,
		args, num_args);

    num_args = 0;
    XtSetArg(args[num_args],
	     XtNborderColor, color);
    num_args++;
    XtSetValues(scr_data->val_text,
		args, num_args);

    XtSetKeyboardFocus(form,
		       scr_data->res_text);
  }   /* initialize the text entry fields */
#endif
}   /* CreateSetValuesPopup() */


/* Function Name:
 *   DoSetValues
 *
 * Description:
 *   Performs a SetValues.
 *
 * Arguments:
 *   w - the widget that called this.
 *   junk - ** UNUSED **
 *   garbage - ** UNUSED **.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   SetMessage - utils.c
 *   _XEditRes... - (Xmu library)
 *   InsertWidgetFromNode -
 *   SetCommand -
 */
/* ARGSUSED */
static void
DoSetValues(Widget w, XtPointer junk, XtPointer garbage)
{
#if 0
  ProtocolStream *stream = &(global_client.stream);
  char           *res_name;
  char           *res_value;
  Arg            args[1];
  Cardinal       i;


  if (global_tree_info->num_active == 0)
    {
      SetMessage(global_screen_data.info_label,
		 res_labels[23],
		 "There are no currently active widgets/styles");
      return;
    }

  XtSetArg(args[0],
	   XtNstring, &res_name);
  XtGetValues(global_screen_data.res_text,
	      args, ONE);

  XtSetArg(args[0],
	   XtNstring, &res_value);
  XtGetValues(global_screen_data.val_text,
	      args, ONE);

  _XEditResResetStream(stream);
  _XEditResPutString8(stream,
		      res_name);
  _XEditResPutString8(stream,
		      XtRString);
  _XEditResPutString8(stream,
		      res_value);
  _XEditResPut16(stream,
		 global_tree_info->num_active);

  for (i = 0; i < global_tree_info->num_active; i++) 
    {
      InsertWidgetFromNode(stream, global_tree_info->active_nodes[i]);
    }   /* for (all tree nodes) */

  SetCommand(w, LocalSetValues, NULL);
#endif
}   /* DoSetValues() */


/* Function Name:
 *   CancelSetValues
 *
 * Description:
 *   Pops down the setvalues popup.
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
CancelSetValues(Widget w, XtPointer junk, XtPointer garbage)
{
#if 0
  XtPopdown(XtParent(XtParent(w))); 
#endif
}   /* CancelSetValues() */
