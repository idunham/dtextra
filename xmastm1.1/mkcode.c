/*
 *  MakeCode - makes the hidden code
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
 *      muquit@semcor.com   19-Apr-95   first cut
 */

#include "xhead.h"
#include "xmastm.h"
#include "defines.h"

void MakeCode ()
{
    int
        col;

    int
        tmp;
    long
        timefield;

    time (&timefield);
    srand((int) timefield);

    for (col=0; col < Cols; col++)
    {
       CodePeg[col] = rand() % MaxColors;
       (void) fprintf (stderr,"%d\n", CodePeg[col]);
    }
}
