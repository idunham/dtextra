/*
 *  main for mppp
 *
 *  RCS:
 *      $Revision$
 *      $Date$
 *
 *  Description:
 *
 *      Program to make a ppp connection.
 *
 *  Limitations and Comments:
 *
 *  N/A
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-03-1997    first cut
 */


#ifdef HAVE_MOTIF

#include "xhead.h"
#include "xmppp.h"

#endif  /* HAVE_MOTIF */


#define INCLUDED_FROM_MAIN
#include "mppp.h"

int main(int argc,char **argv)
{

    /* initialize globals */
    g_verbose=0;        /* verbose off */
    g_quit=0;           /* quit for making the ppp connection is established*/
    g_quit_time=0;      /* stay up forever */
    *g_configfile='\0'; /* the config file */

#ifdef HAVE_MOTIF

    /* create the user interface */
    DoMotif(argc,argv);

#else
     parseCommandline(argc,argv);
     startPPP();
#endif      /* HAVE_MOTIF */
    exit(0);
}
