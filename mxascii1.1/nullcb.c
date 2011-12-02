/*
 *  function_name() -   description
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
 *      muquit@semcor.com   17-Oct-95   first cut
 */

#include "xhead.h"
#include "mxascii.h"

void NullCb(Widget widget,XtPointer client_data,XtPointer call_data)
{
    (void) fprintf (stderr,
        "I am NULL callback routine..I do nothing!\n");
}
