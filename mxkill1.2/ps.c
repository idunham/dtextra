/*
 *  ps related stuff
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
 *      muquit@semcor.com   04-Sep-95   first cut
 *                          02-Mar-97   on my home linux box
 *                                      - found out, no header files included
 *                                      - freeing without checking
 */

#include "xhead.h"
#include "mxkill.h"

#define MaxItems 1024
char
    *lines[MaxItems];

char
    **processes = &lines[0];

char
    *psheader=(char *) NULL;

int
    Gmaxline;

int
    pidcol=(-1);
#include "xhead.h"
#include "mxkill.h"

unsigned int GetpsInfo(commandbuf)
char
    *commandbuf;
{
    int
        i=0;

    FILE
        *pp;

    char
        buf[BUFSIZ];

    int
        newpidcol=(-1);

    *buf='\0';


    pp=popen(commandbuf,"r");
    if (pp == (FILE *) NULL)
        Error("Unable to open pipe to ps",(char *) NULL);

    while ((fgets(buf,sizeof(buf),pp) != NULL))
    {
        buf[(int)strlen(buf)-1]='\0';

        if (newpidcol == (-1))
        {
            newpidcol=StrIndex(buf,"  PID");
            if (newpidcol != (-1))
            {
                if (psheader != (char *) NULL)
                    XtFree(psheader);
                psheader=XtNewString(buf);
                continue;
            }
        }
        if (lines[i] != (char *) NULL)
            XtFree(lines[i]);
        lines[i]=XtNewString(buf);
        if (++i == MaxItems)
            break;
    }

    (void) pclose(pp);

    if (newpidcol != (-1))
        pidcol=newpidcol;

    if (i > 0 || (newpidcol != (-1)))
    {
        Gmaxline=i;
        return (True);
    }
    else
        return (False);
}

/*
** from xzap
*/
int StrIndex(s,t)
char
    *s,
    *t;
{
    int
        i,
        n;

    n=(int) strlen(t);

    for (i=0; s[i] != '\0'; i++)
    {
        if (strncmp(s+i,t,n) == 0)
            return (i);
    }
    return (-1);
}
