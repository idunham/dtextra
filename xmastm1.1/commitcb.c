/*
 *  CommitCB - commits a selection
 *
 *  RCS:
 *      $Revision$
 *      $Date$
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      text
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Return Values:
 *      value   description
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      text
 *
 *  Development History:
 *      who                 when        why
 *      muquit@semcor.com   16-Apr-95   first cut
 */

#include "xhead.h"
#include "defines.h"
#include "xmastm.h"

void CommitCB (widget, client_data, call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
    int
        i;

    if ((Gcol1 != 1) ||
        (Gcol2 != 1) ||
        (Gcol3 != 1) ||
        (Gcol4 != 1))
        XBell (XtDisplay(widget), 99);
    else
    {
        Gcol1=0;
        Gcol2=0;
        Gcol3=0;
        Gcol4=0;
       GcurrentCol = 0;
       GcurrentRow++;

        ScoreMove();
        SetScore (GcurrentRow - 1);
    }

    if (GcurrentRow == Rows)
    {
         GcurrentRow = 0;
         return;
    }

/*    for (i=0; i < Cols; i++)
    {
       index = CodePeg[i];

       XtVaSetValues (GhiddenCodeW[i],
            XmNforeground, Gpbg[index],
            XmNbackground, Gpbg[index],
            NULL);
    }
*/

}
