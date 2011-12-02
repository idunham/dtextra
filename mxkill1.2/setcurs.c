/*
 *	SetCursorState()	- sets and unsets busy cursor states on the managed
 *						  dialogs
 *
 *	RCS:
 *		$Revision$
 *		$Date$
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		text
 *
 *	Input Parameters:
 *		type	identifier	description
 *
 *		text
 *
 *	Output Parameters:
 *		type	identifier	description
 *
 *		text
 *
 *	Return Values:
 *		value	description
 *
 *	Side Effects:
 *		text
 *
 *	Limitations and Comments:
 *		text
 *
 *	Development History:
 *		who					when		why
 *		muquit@semcor.com	12-Dec-94	first cut
 */

#include "xhead.h"
#include "mxkill.h"

void SetbusyCursorState(w,state)
Widget
    w;
int
    state;
{
      static Cursor cursor;
 
    if (state)
    {
      if (w != NULL)
      {
          if (!cursor)
          cursor = XCreateFontCursor(XtDisplay(w), XC_watch);
          XDefineCursor(XtDisplay(w), XtWindow(w), cursor);
          XFlush(XtDisplay(w));
      }
    }
    else
    {
        if (w != NULL)
        XUndefineCursor (XtDisplay(w), XtWindow(w));
    }
}

