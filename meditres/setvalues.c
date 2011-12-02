/*
 * $Header: /cvsroot-fuse/meditres/meditres/setvalues.c,v 1.6 2002/01/16 14:35:20 amai Exp $
 *
 * $XConsortium: setvalues.c,v 1.6 94/04/17 20:38:57 hersh Exp $
 *
 Copyright (c) 1989  X Consortium

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 Except as contained in this notice, the name of the X Consortium shall not be
 used in advertising or otherwise to promote the sale, use or other dealings
 in this Software without prior written authorization from the X Consortium.
 *
 * Author:  Chris D. Peterson, MIT X Consortium
 */

/*
 * Modifications by Volker Gering, 1997
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xresource.h>
#include <X11/Xfuncs.h>
#include <X11/Xos.h>

#include <Xm/TextF.h>

#include "meditres.h"


#define RESOURCE_NAME  ("name")
#define RESOURCE_CLASS ("Class")

/* Function Name:
 *   PrintSetValuesError
 *
 * Description:
 *   Allow the SetValues error to be printed.
 *
 * Arguments:
 *   event - the set values call that caused this event.
 *
 * Returns:
 *   str - a string contining the errors.
 *
 * Calls:
 *   FindNode - utils.c
 *   AddString - utils.c
 */
char
*PrintSetValuesError(Event *event)
{
  char           *errors = NULL;
  WNode          *node;
  int            i;
  SetValuesEvent *sv_event = (SetValuesEvent*)event;
  char           buf[BUFSIZ];


  if (sv_event->num_entries == 0)
    {
      /* "SetValues was Successful." */
      if (global_mode == MODE_BATCHRES)
	{
	  return(NULL);
	}
      else
	{
	  if (res_labels[39])
	    return(XtNewString(res_labels[39]));
	  else
	    return(XtNewString("SetValues was Successful."));
	}
    }

  for (i = 0 ; i < (int)sv_event->num_entries ; i++)
    {
      node = FindNode(global_tree_info->top_node,
		      sv_event->info[i].widgets.ids, 
		      sv_event->info[i].widgets.num_widgets);

      if (node == NULL)
	{
	  if (res_labels[16])
	    {
	      sprintf(buf,
		      res_labels[16]);
	    }
	  else
	    {
	      sprintf(buf,
		      "Editres Internal Error: Unable to FindNode.\n");
	    }
	  AddString(&errors,
		    buf); 
	  continue;
	}   /* if (node == NULL) */

      sprintf(buf,
	      "%s(0x%lx) - %s\n",
	      node->name, node->id,
	      sv_event->info[i].message);
      AddString(&errors,
		buf);
    }   /* for (all selected nodes) */

    return(errors);

}   /* PrintSetValuesError()  */


/* Function Name:
 *   GetResourceValueForSetValues
 *
 * Description:
 *   Returns the value that should be sent to SetValues.
 *
 * Arguments:
 *   node - the node which contains the resource box.
 *   size - length of the string returned
 *
 * Returns:
 *   value - allocated value.
 *
 * Calls:
 *   Xrm- and Xt-stuff
 */
char
*GetResourceValueForSetValues(WNode *node, unsigned short *size)
{
  Arg         args[1];
  char        *ptr;
  char        *temp;
  XrmDatabase db = NULL;
  XrmValue    value;

  ptr = XmTextFieldGetString(node->resources->res_box->value_wid);

  /*
   * This makes sure that exactly the same thing happens during a set
   * values, that would happen if we were to insert this value into
   * the resource database.
   */

  temp = XtMalloc(sizeof(char) * (strlen(ptr) + strlen(RESOURCE_NAME) + 2));
  sprintf(temp, "%s:%s", RESOURCE_NAME, ptr);

  XrmPutLineResource(&db, temp);
  XtFree(temp);
  XtFree(ptr);

  XrmGetResource(db,
		 RESOURCE_NAME,
		 RESOURCE_CLASS,
		 &temp,
		 &value);

    ptr = XtMalloc(sizeof(char) * value.size);
    memmove (ptr, value.addr, value.size);
    XrmDestroyDatabase(db);
    
    *size = (unsigned short) value.size;

    return(ptr);
}   /* GetResourceValueForSetValues() */


/* >>>>> HACK for batchres */

/* Function Name:
 *   brGetResourceValueForSetValues
 *
 * Description:
 *   Returns the value that should be sent to SetValues.
 *
 * Arguments:
 *   res_value - value to set to the resource
 *   size - length of the string returned
 *
 * Returns:
 *   value - allocated value.
 *
 * Calls:
 *   Xrm- and Xt-stuff
 */
char
*brGetResourceValueForSetValues(char *res_value, unsigned short *size)
{
  static char      *temp = NULL;
  static Cardinal  tempSize = 0;
  static char      *ptr = NULL;
  static Cardinal  ptrSize = 0;

  Arg              args[1];
  XrmDatabase      db = NULL;
  XrmValue         value;
  char             *type;


  /*
   * This makes sure that exactly the same thing happens during a set
   * values, that would happend of we were to insert this value into
   * the resource database.
   */
  if (tempSize < sizeof(char) * (strlen(res_value) + strlen(RESOURCE_NAME) + 2))
    {
      tempSize = (sizeof(char) * (strlen(res_value) + strlen(RESOURCE_NAME) + 2));
      if ( (temp = XtRealloc(temp, tempSize)) == NULL )
	{
	  fprintf(stderr, "XtRealloc failed!\n");
	  exit(EXIT_FAILURE);
	}
    }

  sprintf(temp,
	  "%s:%s",
	  RESOURCE_NAME,
	  res_value);

  XrmPutLineResource(&db,
		     temp);

  XrmGetResource(db,
		 RESOURCE_NAME,
		 RESOURCE_CLASS,
		 &type,
		 &value);

  if (ptrSize < sizeof(char) * value.size)
    {
      ptrSize = sizeof(char) * value.size;
      if ( (ptr = XtRealloc(ptr, ptrSize)) == NULL )
	{
	  fprintf(stderr, "XtRealloc failed!\n");
	  exit(EXIT_FAILURE);
	}
    }

  memmove (ptr,
	   value.addr,
	   value.size);
  XrmDestroyDatabase(db);

  *size = (unsigned short) value.size;

  return(ptr);

}   /* brGetResourceValueForSetValues() */

/* <<<<< HACK for batchres */
