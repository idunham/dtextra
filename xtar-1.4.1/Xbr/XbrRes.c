/* 
 * Copyright 1993, 1994 Liverpool University Computer Science Department
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of L.U.C.S. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. L.U.C.S. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * FILE NAME:	XbrRes.c
 * CREATED:	Thu 19 Sep 1994
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Routines for handling Motif resources.
 *
 */

#include <Xm/Xm.h>

char *XbrResGetString(Widget, char *);

/*----------------------------------------------------------------------------
  XbrResGetString()
  Convert an XmString to a normal text string.
  
  Widget w	Widget to use as the source
  char *name	Name of resource to grab as an XmString
  ----------------------------------------------------------------------------*/
char *XbrResGetString(Widget w, char *name)
{
    XmString string;
    char *retval;
    
    XtVaGetValues(w, name, &string, NULL);
    
    XmStringGetLtoR(string, XmSTRING_DEFAULT_CHARSET, &retval);
    
    return(retval);
}
