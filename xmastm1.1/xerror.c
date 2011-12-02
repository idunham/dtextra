/*
 *  XError()    -   catches X errors and handles them
 *
 *  RCS:
 *      $Revision$
 *      $Date$
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      This function catches X -lib errors and handles them.  It forgives
 *      the non-fatal errors.  If no error is handled, the application will
 *      exit with the error
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
 *      0 - if the error is non-fatal
 *      -1 - exits if the error is fatal
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      text
 *
 *  Development History:
 *      when    who     why
 *  2/04/94,    mm,     while freeing colors, BadAccess was happening.
 */
#include "xhead.h"
#include "xmastm.h"

int XError (display, error)
Display *display;
XErrorEvent
    *error;
{
    int xerrcode;
    char buf[128];

    xerrcode = error->error_code;

    if (xerrcode == BadAlloc ||
          (xerrcode == BadAccess && error->request_code==88)) return 0;
    
    else
    {
        XGetErrorText (display, xerrcode, buf, 128);
        fprintf (stderr, "Xmastm: X ERRROR: %s\n",buf);
        fprintf (stderr, "MAJOR Code: %d\n", error->request_code);
        exit(-1);
    }
    return 0;
}
