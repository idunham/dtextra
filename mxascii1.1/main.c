/*
 *  main -   main for mxascii
 *
 *  RCS:
 *      $Revision$
 *      $Date$
 *
 *  Description:
 *      mxascii talks to diskmond to a specific port and displays
 *      the disk usage graphically
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
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Jun-18-96  first cut
 */

#include <xhead.h>
#define INCLUDED_FROM_MAIN
#include <mxascii.h>

/*
** fallback resources
*/
static char *app_defaults[]=
{
#include "appdef.h"
NULL
};

int main(int argc,char **argv)
{
    XtAppContext
        app;

    int
        i;

    /*
    ** initialize globals --starts
    */
    GtoplevelW=(Widget) NULL;
    GmainwW=(Widget) NULL;

    for (i=0; i < 6; i++)
    {
        Gvalue_labelW[i] = (Widget) NULL;
        Gascii_labelW[i] = (Widget) NULL;
    }

    for (i=0; i < 3; i++)
        Gtoggle_menuW[i] = (Widget) NULL;

    /*
    Gleft_label_1W = (Widget) NULL;
    Gleft_label_2W = (Widget) NULL;
    Gmiddle_label_1W = (Widget) NULL;
    Gmiddle_label_2W = (Widget) NULL;
    Glast_label_1W = (Widget) NULL;
    Glast_label_2W = (Widget) NULL;
    */



    /*
    ** initialize globals --ends
    */

    /*
    ** create toplevel widget
    */

    GtoplevelW=XtVaAppInitialize(&app,"MXascii",
        NULL,0,&argc,argv,app_defaults,NULL);

    XtVaSetValues(GtoplevelW,
        XmNallowShellResize, True,
        NULL);

    /*
    ** create the table
    */
    MakeTable ();

    /*
    ** create the user interface
    */
    CreateUserInterface (GtoplevelW);
    CreateAboutD (GtoplevelW);

    
    SetCb ((Widget) NULL,(XtPointer) 0,(XtPointer) NULL);
    XtRealizeWidget(GtoplevelW);

    XtAppMainLoop(app);
    return (0);
}
