/*
 *  ScoreMove() -   determine the score for the move
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

void ScoreMove ()
{
    int
        prev_row;

    int
        rc_rip,
        rc_wrp;

    register int
        i,
        j;
    int
        Gp_used[4],
        Cp_used[4];


    rc_rip= 0;
    rc_wrp= 0;
    prev_row = GcurrentRow - 1;
    (void) fprintf (stderr,
        " Score for row: %d\n", prev_row);

    for (i=0; i < Cols; i++)
    {
        Gp_used[i] = Cp_used[i] = Wc_Wp;
    }

    for (i=0; i < Cols; i++)
    {
        if (GuessPeg[i] == CodePeg[i])
        {
            Gp_used[i] = Cp_used[i] = Rc_Rp;
            rc_rip++;
        }
    }
        

    if (rc_rip != Cols)
    {
        for (i=0; i < Cols; i++)
        {
            for (j=0; j < Cols; j++)
            {
                if ((GuessPeg[i] == CodePeg[j]) &&
                    (Cp_used[j] == Wc_Wp) &&
                    (Gp_used[i] == Wc_Wp))
                {
                    Gp_used[i] = Cp_used[j] = Rc_Wp;
                    rc_wrp++;
                }

            }
        } 
    }
    G_rc_rip= rc_rip;
    G_rc_wrp = rc_wrp;
    (void) fprintf (stderr,"rc_rp: %d\n", rc_rip);
    (void) fprintf (stderr,"rc_wp: %d\n", rc_wrp);

    if (G_rc_rip == 4)
    {
        int
            index;
        for (i=0; i < Cols; i++)
        {
           index = CodePeg[i];

           XtVaSetValues (GhiddenCodeW[i],
                XmNforeground, Gpbg[index],
                XmNbackground, Gpbg[index],
                NULL);
        }
            (void) fprintf (stderr,
                "You found the code in %d moves!\n", GcurrentRow);
    }
}
