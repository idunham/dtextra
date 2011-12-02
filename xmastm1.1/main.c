/*
 *  main.c  - main module for Xmastm
 *
 *  RCS:
 *      $Revision$
 *      $Date$
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      int argc;
 *      char **argv;
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      None
 *
 *  Return Values:
 *      value   description
 *      none
 *
 *  Side Effects:
 *      None
 *
 *  Limitations and Comments:
 *      needs Motif and X library to compile
 *
 *  Development History:
 *      when    who     why
 *  10/28/94    muquit  first cut
 */

#include "xhead.h"

#include "defines.h"
#include "errcds.h"
#include "mastm.xbm"

#include <ElecDisp.h>

#define MAIN
#include "xmastm.h"


typedef struct
{
    String
        ScrollbarBg;
    Boolean
        UseCardPixmaps;
} YAppResources, *YAppResourcesP;

#define YOffset(x) XtOffset(YAppResourcesP, x)

static XtResource resources[] =
{

    {"ScrollbarBg", "ScrollbarBg", XtRString, sizeof(String),
        YOffset(ScrollbarBg), XtRString, "Red"},
    {"UseCardPixmaps", "UseCardPixmaps", XtRBoolean, sizeof(Boolean),
        YOffset(UseCardPixmaps), XtRImmediate, (XtPointer) False},
};

/*
** define fallback resources
*/

static char *app_defaults[] =
{
#include "appdef.h"
    NULL
};

void main (argc, argv)
int
    argc;
char
    **argv;
{

    char
        elec_buf[100];

    char
        *elec_args[5];

    int
        n;

    int
        depth;


    Pixmap
        iconPixmap;

    int
        screen;

    XtAppContext
        app;
    
    register int
        i;
        


    Display
        *display;

    static Widget
        toplevelW;

    char
        *fname = (char *) NULL;
    
    int
        ishelp = 0;

    int
        isver = 0;

    int
        isfile = 0;
        
    char
        *xmastmessg;
    int
        length;

    *elec_buf = '\0';

    length = (int) strlen ("Xmastm by muquit@semcor.com") +
        (int) strlen(Version) + 15;

    xmastmessg = (char *) malloc (length*sizeof(char));

    if (xmastmessg == (char *) NULL)
        Error ("Memory Allocation Failed!", (char *) NULL);

    (void) sprintf(xmastmessg, "Xmastm - Version %s by muquit@semcor.com",
        Version);
/*
** Initalize Globals ---Starts---
*/

    for (i=0; i < 6; i++)
        Gpbg[i] = (Pixel) 0;

    for (i=0; i < Cols; i++)
        GhiddenCodeW[i] = (Widget) NULL;

    for (i=0; i < (Rows*Cols)+1; i++)
        w_special[i] = (Widget) NULL;

    for (i=0; i < (Rows*2)+1; i++)
        GscoreLabW[i] = (Widget) NULL;

    for (i=0; i < Cols; i++)
    {
        CodePeg[i] = (int) 0;
        GuessPeg[i] = (int) 0;
    }

    G_rc_rip=0;
    G_rc_wrp = 0;

    GlabW = (Widget) NULL;
    GelecLogoW = (Widget) NULL;
    GaskdW = (Widget) NULL;
    Gaskd2W = (Widget) NULL;
    GelecRespW = (Widget) NULL;
    GelecResp2W = (Widget) NULL;

    GcurrentRow = 0;
    GcurrentCol = 0;

    Gcol1=0;
    Gcol2=0;
    Gcol3=0;
    Gcol4=0;
/*
** Initalize Globals ---Ends---
*/

    /*
    ** parse the command line
    */

    for (i=1; i <argc; i++)
    {
        if (!strncmp(argv[i],"-fg",3))
        {
            i++;
            continue;
        }
    
        if (!strncmp(argv[i],"-bg",3))
        {
            i++;
            continue;
        }
    
        if (!strncmp (argv[i], "-i",2))
        {
            continue;
        }
    
        if (!strncmp (argv[i], "-iconic",7))
        {
            continue;
        }
    
        if (!strncmp (argv[i], "-d", 2))
        {
            i++;
            continue;
        }

        if (!strncmp (argv[i], "-display",8))
        {
            i++;
            continue;
        }

        if (!strncmp (argv[i], "-xrm", 4))
        {
            i++;
            continue;
        }

        if (!strncmp (argv[i], "-synchronous", 12))
        {
            continue;
        }

        if (!strncmp (argv[i], "-h", 2))
        {
            ishelp++;
            continue;
        }

        if (!strncmp (argv[i], "-V", 2))
        {
            isver++;
            continue;
        }

        if (!strncmp (argv[i], "-nopixmaps", (int) strlen("-nopixmaps")))
        {
            GnoPixmaps = True;
            continue;
        }

        if (argv[i][0] != '-' ) /* the file name */
        {
            fname = argv[i];
            isfile = 1;
            continue;
        }

        Usage(argv[0]);
    }

    if (isver)
    {
        (void) fprintf(stderr,
            "\nyrolo $ Release %s $\n\n", Version);
        exit(1);
    }

    if (ishelp)
    {
        Usage (argv[0]);
    }
    /*
    ** create toplevel widget
    */

    toplevelW =  XtVaAppInitialize (&app, "XMastm",
        NULL, 0, &argc, argv, app_defaults, NULL);
    
    XtVaSetValues (toplevelW, 
        XmNallowShellResize, True,
        NULL);
    
    GtoplevelW= toplevelW;


    /*
    ** set the icon
    */
    display = XtDisplay(toplevelW);
    depth = XDefaultDepth (display, XDefaultScreen (display));

    iconPixmap = XCreatePixmapFromBitmapData (XtDisplay(toplevelW),
        DefaultRootWindow (XtDisplay(toplevelW)),
        mastm_bits,
        mastm_width,
        mastm_height,
        BlackPixelOfScreen (XtScreen(toplevelW)),
        WhitePixelOfScreen (XtScreen(toplevelW)),
        depth);

    
    XtVaSetValues (toplevelW,
        XmNiconPixmap, iconPixmap,
        NULL);



    /*
    ** create main window Widget
    */
    (void) CreateMainWindow (toplevelW);

    /*
    ** create the ask electric dialog
    */

    CreateAskDialog (toplevelW);
    CreateAskDialog2 (toplevelW);

    /*
    ** create dialogs
    */

/*    CreateDialogs (toplevelW);*/


    /*
    ** realize toplvel
    */
    XtRealizeWidget (toplevelW);

    screen = DefaultScreen (display);
    black_pixel = BlackPixel(display, screen);
    white_pixel = WhitePixel (display, screen);


    XSetErrorHandler (XError);

    /*
    ** allocate guess and make the code
    */

    /*Gguess = AllocateGuess();

    if (Gguess == (Guess *) NULL)
    {
        (void) fprintf (stderr,
            "Memory allocation failed!\n");
        exit(1);
    }
*/

    MakeCode ();

#if defined (sgi)
	n = 0;
	elec_args[n] = "6x10"; n++;
	XmtdsEDSetFontFunc (GelecLogoW, elec_args, n);
#endif

    n = 0;
    elec_args[n] = "0"; n++;

#if defined(sgi)
    elec_args[n] = "3"; n++;
#else
    elec_args[n] = "0"; n++;
#endif
    (void) sprintf (elec_buf, "%s", "    X Mastermind 1.1");
    elec_args[n] = elec_buf; n++;
    XmtdsEDMessageFunc (GelecLogoW,
        elec_args, n);


    XtAppMainLoop (app);

}

void SetResources(widget)
Widget
    widget;
{
    YAppResources
        appResources;
    
    XtGetApplicationResources(widget, (XtPointer) &appResources,
        resources, XtNumber(resources), NULL, 0);
    
    scb_bgcolor = mystrdup(appResources.ScrollbarBg);

#ifdef DEBUG
    (void) fprintf (stderr, "Scrollbar BG: %s\n",
        scb_bgcolor);
#endif

    GuseFolderPixmaps = appResources.UseCardPixmaps;

#ifdef DEBUG
(void) fprintf(stderr,
    "Use FolderPixmaps: %d\n", GuseFolderPixmaps);
#endif

}

