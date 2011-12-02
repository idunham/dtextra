/*
 *	MXError()	-	catches X errors and handles them
 *
 *	RCS:
 *		$Revision$
 *		$Date$
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		This function catches X -lib errors and handles them.  It forgives
 *		the non-fatal errors.  If no error is handled, the application will
 *		exit with the error
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
 *		0 - if the error is non-fatal
 *		-1 - exits if the error is fatal
 *
 *	Side Effects:
 *		text
 *
 *	Limitations and Comments:
 *		text
 *
 *	Development History:
 *		when	who		why
 *	2/04/94,	mm,		while freeing colors, BadAccess was happening.
 */
#include "xhead.h"
#include "mxkill.h"
#include <X11/Xproto.h>

int MXError (display, error)
Display *display;
XErrorEvent
	*error;
{
	int xerrcode;

	xerrcode = error->error_code;

	if (xerrcode == BadAlloc ||
		  (xerrcode == BadAccess && error->request_code==88)) return (False);
	
	else
	{
        switch (error->request_code)
        {
            case X_GetGeometry:
            {
                if (error->error_code == BadDrawable)
                    return (False);
                break;
            }

            case X_GetWindowAttributes:
            case X_QueryTree:
            {
                if (error->error_code == BadWindow)
                    return (False);
                break;
            }

            case X_QueryColors:
            {
                if (error->error_code == BadValue)
                    return(False);
                break;
            }
        }
	}
    (void) fprintf (stderr," mdgclock Non-fatal XError..Handling!\n");
	return (True);
}
