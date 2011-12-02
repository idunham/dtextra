/*
 *  FileMenuCB - callback routine for File menu items
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
 *      muquit@semcor.com   21-Apr-95   first cut
 */

#include "xhead.h"
#include "defines.h"
#include "xmastm.h"

void FileMenuCB (widget, client_data, call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
    switch ((int) client_data)
    {
        case RESTART:
        {
            Restart();
            break;
        }

        case QUIT:
        {
            exit(1);
            break;  /* won't reach */
        }
    }
}
