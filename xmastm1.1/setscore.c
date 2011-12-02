/*
 *  SetScore() -  score the move
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
 *      muquit@semcor.com   20-Apr-95   first cut
 */

#include "xhead.h"
#include "defines.h"
#include "xmastm.h"

void SetScore (row)
int
    row;
{
   int
       rc;
   char
       tmpbuf[1024];

    char
        buf[4];

    XmString
        xmstr;

    *buf = '\0';

    (void) fprintf (stderr," Row: %d\n", row);

    *tmpbuf = '\0';

    if (row > 1)
        row = row<<1;

    if (row == 1)
        row++;




    (void) fprintf (stderr," After inc: %d\n", row);

    (void) sprintf(buf,"%d", G_rc_rip);
    xmstr = XmStringCreateSimple (buf);
    XtVaSetValues (GscoreLabW[row],
        XmNlabelString, xmstr,
        NULL);
    XmStringFree (xmstr);

    (void) sprintf(buf, "%d", G_rc_wrp);
    xmstr = XmStringCreateSimple (buf);
    XtVaSetValues (GscoreLabW[row+1],
        XmNlabelString, xmstr,
        NULL);
    XmStringFree (xmstr);

    if (G_rc_rip == 4)
    {

        (void) sprintf(tmpbuf, "%s%d%s", "Solved in ",GcurrentRow," moves!");
        XBell (XtDisplay(GaskdW), 99);
        XBell (XtDisplay(GaskdW), 99);
        xmstr = XmStringCreateSimple (tmpbuf);
        XtVaSetValues (GaskdW,
            XmNdialogTitle, xmstr,
            NULL);
        XmStringFree (xmstr);
        rc = AskCB (GaskdW, (XtPointer) HappyFace, (XtPointer) NULL);

        switch (rc)
        {
            case YES:
            {
                Restart();
                break;
            }
            
            case NO:
            {
                exit(1);
            }
        }
    }

    if ((GcurrentRow == 10) && (G_rc_rip != 4))
    {
        int
            index;
        int
            col;

        for (col=0; col < Cols; col++)
        {
            index = CodePeg[col];

            XtVaSetValues (GhiddenCodeW[col],
                XmNforeground, Gpbg[index],
                XmNbackground, Gpbg[index],
                NULL);
        }
/*        
        (void) sprintf(tmpbuf, "%s", "Sorry!\n");
        (void) sprintf(tmpbuf, "%s%s",
            tmpbuf, "Guess limit is 10!\n\n");
        (void) sprintf (tmpbuf, "%s%s", tmpbuf, "Do you want to try again?");
        rc = AskUser (GtoplevelW, tmpbuf, SadFace);
*/
        XBell (XtDisplay(Gaskd2W), 99);
        XBell (XtDisplay(Gaskd2W), 99);
        rc = AskCB (Gaskd2W, (XtPointer) SadFace, (XtPointer) NULL);

        switch(rc)
        {
            case YES:
            {
                Restart ();
                break;
            }

            case NO:
            {
                exit(1);
            }
        }

    }
}
