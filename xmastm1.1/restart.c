/*
 *  Restart() -  restarts Mastermind
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

void Restart ()
{
    Pixel
        fg,
        bg;

    register int
        i;

    XmString
        xmstr;

    Gcol1=0;
    Gcol2=0;
    Gcol3=0;
    Gcol4=0;
    GcurrentCol = 0;
    GcurrentRow = 0;
    G_rc_rip=0;
    G_rc_wrp=0;

    XtVaGetValues (GtoplevelW,
        XmNbackground, &bg,
        XmNforeground, &fg,
        NULL);

    for (i=0; i < Rows*Cols; i++)
    {
        XtVaSetValues (w_special[i],
            XmNbackground, bg,
            NULL);
    }

    xmstr = XmStringCreateSimple("??");

    for (i=0; i < Cols; i++)
    {
        XtVaSetValues (GhiddenCodeW[i],
            XmNbackground, bg,
            XmNforeground, BlackPixelOfScreen (XtScreen(GtoplevelW)),
            NULL);

        XtVaSetValues (GhiddenCodeW[i],
            XmNlabelString, xmstr,
            NULL);        
    }
    XmStringFree (xmstr);

    xmstr = XmStringCreateSimple("");
    for (i=0; i < Rows*2; i++)
        XtVaSetValues (GscoreLabW[i],
            XmNlabelString, xmstr,
            NULL);
    XmStringFree (xmstr);

    /*
    ** generate new code
    */

    MakeCode ();
}
