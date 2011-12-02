/*
 *  Error() - exits after telling the error message
 *  Warning () - Spits out a warning message
 *
 *  RCS:
 *      $Revision: 1.1 $
 *      $Date: 1995/04/15 19:23:42 $
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
 *      when    who     why
 */

#include "xhead.h"
#include "mxkill.h"


void Error (message, qualifier)
char
    *message;
char
    *qualifier;
{
    (void) fprintf (stderr,"Error:%s, (%s)\n",
        message, qualifier);
    exit(1);

}

void Warning (message, qualifier)
char
    *message,
    *qualifier;
{
    (void) fprintf (stderr, "Warning: %s (%s)\n",
        message, qualifier);
    return;
}
