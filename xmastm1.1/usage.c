/*
 *  Usage() -   shows the help
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
 *      muquit@semcor.com   15-Mar-95   first cut
 */


#include "stdio.h"
#include "defines.h"

void Usage(progname)
char
    *progname;
{
    char **p;

char
    *text;
    static char
        *options[] =
        {
            "-fg foreground_color       sets the foreground color",
            "-bg background_color       sets the background color",
            "-i                         starts in iconic form",
            "-iconic                    starts in iconic form",
            "-d displayname             X server to contact",
            "-display displayname       X server to contact",
            "-xrm resourcestring        specifies a resource string",
            "-h                         shows this help",
            "-V                         shows Version",
            "-nopixmaps                 do not use pixmaps for cards",
            (char *) NULL
        };

text = 
"\n--------------------------------------------------------------------------\n\
Note: If no filename is provided at the command line, Yrolo will\n\
open a file called \".yrolo.yr\" at your home directory. If the file\n\
does not exist, it will be created.  \".yrolo.yr\" is the default database\n\
file.  However this can be changed by setting the environment variable\n\
YROLODEX. Please look at the man page or on line help for details.\n\
--------------------------------------------------------------------------";

    (void) fprintf(stderr,
        "\nYrolo Version %s by muquit@semcor.com\n\n",
            Version);
    fprintf (stderr, "Usage: %s   or\n",progname);
    fprintf (stderr, "       %s [options ...]   or\n",progname);
    fprintf (stderr, "\nWhere options include:\n");
 
    for (p=options; *p != (char *) NULL; p++)
        fprintf (stderr, "  %s\n",*p);

    (void) fprintf(stderr,
        "%s\n", text);

    fprintf (stderr, "\n");
    fprintf (stderr, " Options can be specified in any order, except that\n");
    (void) fprintf(stderr," the filename has to be the last argument.\n");
    fprintf (stderr, " If color name has more than one word, use double\n");
    fprintf (stderr, " qoutes around the name\n\n");
    fprintf (stderr, " Examples:\n\n");
    fprintf (stderr, " 1.  %s -fg blue -bg gray75\n\n", progname);
    fprintf (stderr, " 2.  %s -fg \"dark green\" -bg \"light sky blue\"\n\n", progname);
    fprintf (stderr, " 3.  %s file.ntf\n\n", progname);
    fprintf (stderr, " 4.  %s -i file.yr\n\n", progname);
    fprintf (stderr, " 5.  %s -d machine:0 file.yr\n\n", progname);
    fprintf (stderr, " 6.  %s -xrm 'YRolo*menuCursor: cross'\n\n", progname)
;
    fprintf (stderr, " 9. %s -xrm 'YRolo*Menubar*fontList: variable'\n\n", progname);
    exit(1);

}
