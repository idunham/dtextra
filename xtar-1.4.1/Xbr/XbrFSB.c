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
 * FILE NAME:	XbrFSB.c
 * CREATED:	Fri Oct 29 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Routines for handling the Motif XmFileSelectionBox widget.
 *
 */

#include <Xm/Xm.h>
#include <Xm/FileSB.h>

char *XbrFSBSelection(Widget);

/* XbrFSBSelection:***********************************************************/
/* XbrFSBSelection: Get the selected file name out of the File Selection Box */
/* XbrFSBSelection:***********************************************************/

char *XbrFSBSelection(Widget w)
{
    char *filename;
    XmString MyString;

    XtVaGetValues(w, XmNdirSpec, &MyString, NULL);
    XmStringGetLtoR(MyString, XmSTRING_DEFAULT_CHARSET, &filename);

    return(filename);
}
