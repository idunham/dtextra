/*
 *  main for mdgclock
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
 *      muquit@semcor.com   31-Aug-95   first cut
 */

#include "xhead.h"

#define __Main__
#include "mxkill.h"

#include "mxkicon.xbm"

typedef struct 
{
    String
        psCommand;

    String
        psdefOption; 

    String
        psAllOption;

    String
        psAllbyUserOption;

    String
        psAllbyPidOption;

} mresources, *mresourcesP;


#define moffset(x)   XtOffset(mresourcesP,x)

static XtResource resources[]=
{
    {"psCommand","psCommand",XtRString,sizeof(String),
        moffset(psCommand),XtRString,"/bin/ps"},
    {"psdefOption","psdefOption",XtRString,sizeof(String),
        moffset(psdefOption),XtRString,"x"},
    {"psAllOption","psAllOption",XtRString,sizeof(String),
        moffset(psAllOption),XtRString,"aux"},
    {"psAllbyUserOption","psAllbyUserOption",XtRString,sizeof(String),
        moffset(psAllbyUserOption),XtRString,"aux|sort +0 -1"},
    {"psAllbyPidOption","psAllbyPidOption",XtRString,sizeof(String),
        moffset(psAllbyPidOption),XtRString,"aux|sort +0 -1"},
};

/*
** fallback resources
*/
static char *app_defaults[]=
{
#include "MXkill.ad.h"
    NULL
};
void main(argc,argv)
int
    argc;
char
    *argv[];
{

    XtAppContext
        app;

    int
        screen;

    int
        i;

    Pixmap
        iconpix;

    int
        stderr_pipe[2];

    /*
    ** initialize globals-----starts------
    */

    GtoplevelW=(Widget) NULL;
    Gdisplay=(Display *) NULL;
    Gcolormap=(Colormap) NULL;
    Gdepth=8;
    Grootwindow=(Window) NULL;
    Gwhitepixel=0;
    Gblackpixel=1;
    GdisplayWidth=0;
    GdisplayHeight=0;
    Gsignal_no=9;
    for(i=0; i < 6; i++)
        Gtoggle_menuW[i]=(Widget)NULL;

    *GpsCommand='\0';
    *GpsdefOption='\0';
    *GpsAllOption='\0';
    *GpsAllbyUserOption='\0';
    *GpsAllbyPidOption='\0';
    Giconic_state=False;


    /*
    ** initialize globals-----ends------
    */

    /*
    ** create toplevelW
    */

    XtSetLanguageProc (NULL, NULL, NULL);
    GtoplevelW=XtVaAppInitialize(&app,"MXkill",
        NULL,0,&argc,argv,app_defaults,NULL);
    

    /* 
    ** add the event handler for detecting iconify/deiconify states
    ** this time I'm doing with StructureNofify mask.
    */
    XtAddEventHandler(GtoplevelW,
        StructureNotifyMask,
        False,
        (XtEventHandler) HandleIconicStates,
        NULL);

    XtVaGetValues(GtoplevelW,
        XmNiconic, &Giconic_state,
        NULL);

    /*
    ** set the forgiving error handler
    */
    XSetErrorHandler(MXError);
    SetResources(GtoplevelW);

    Gdisplay=XtDisplay(GtoplevelW);
    screen=XDefaultScreen(Gdisplay);
    Gcolormap=XDefaultColormap(Gdisplay,screen);
    Ggc=XDefaultGC(Gdisplay,screen);
    Gblackpixel=BlackPixel(Gdisplay,screen);
    Gwhitepixel=WhitePixel(Gdisplay,screen);
    Grootwindow=XDefaultRootWindow(Gdisplay);
    Gdepth=XDefaultDepth(Gdisplay,screen);
    GdisplayWidth=XDisplayWidth(Gdisplay,screen);
    GdisplayHeight=XDisplayHeight(Gdisplay,screen);

    /*
    ** create the icon pixmap
    */

    iconpix=XCreatePixmapFromBitmapData(Gdisplay,
        Grootwindow,
        mxkicon_bits,
        mxkicon_width,
        mxkicon_height,
        Gblackpixel,
        Gwhitepixel,
        Gdepth);

    if (iconpix != (Pixmap) NULL)
        XtVaSetValues(GtoplevelW,
            XmNiconPixmap, iconpix,
            NULL);


    /*
    ** create the UI
    */
    CreateMainWindow(GtoplevelW);
    CreateAboutD(GtoplevelW);

    XtRealizeWidget(GtoplevelW);

    /*
    ** show the first ps output with default command
    */
/*    if (Giconic_state == False)
        PsCb((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);*/

    /*
    ** catch stderr and display the message in a dialog
    */

    pipe(stderr_pipe);
    dup2(stderr_pipe[1],2);
    XtAppAddInput(app,stderr_pipe[0],
        (XtPointer) XtInputReadMask,
        (XtInputCallbackProc) DoStderr,
        (XtPointer) stderr_pipe[0]);

    XtAppMainLoop(app);
}

/*
** load resources
*/
void SetResources(widget)
Widget
    widget;
{
    mresources
        app_resources;
 
    XtGetApplicationResources(widget,(XtPointer) &app_resources,
        resources,XtNumber(resources),NULL,0);

    (void) sprintf(GpsCommand,"%s",app_resources.psCommand);
    (void) sprintf(GpsdefOption,"%s",app_resources.psdefOption);
    (void) sprintf(GpsAllOption,"%s",app_resources.psAllOption);
    (void) sprintf(GpsAllbyUserOption,"%s",app_resources.psAllbyUserOption);
    (void) sprintf(GpsAllbyPidOption,"%s",app_resources.psAllbyPidOption);
}

char *mystrdup(s)
  char *s;
{
  char *t;
  
  if ( s == (char*)NULL )
    return (char*)NULL;

  t=(char *) malloc(strlen(s)+1);
  if (t == (char *) NULL)
    return (char*)NULL;

  (void) strcpy(t,s);
  return t;
}

/*
** Handle iconic states
*/

void HandleIconicStates(widget,client_data,event)
Widget
    widget;
XtPointer
    client_data;
XEvent
    *event;
{
   switch(event->type)
   {
        case MapNotify:
        {
            PsCb((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);
            break;
        }

        default:
            break;
   }
}

void DoStderr(client_data,source,id)
XtPointer
    client_data;
int
    source;
XtInputId
    *id;
{
    int
        fd=(int) client_data;

    char
        buf[512];

    int
        n;

    Arg
        args[10];

    static Widget
        messagedW = (Widget) NULL;

    XmString
        xmstr;

    n=read(fd,buf,512);
    buf[n]='\0';

    if (!n)
        return;

    XBell(Gdisplay,0);

    if (messagedW == (Widget) NULL)
    {
        n=0;
        XtSetArg(args[n],XmNtitle,"mxkill Error!"); n++;
        XtSetArg(args[n],XmNdialogStyle,
            XmDIALOG_PRIMARY_APPLICATION_MODAL);    n++;
        messagedW=XmCreateErrorDialog(GtoplevelW,"Error",args,n);
    }
    xmstr=XmStringCreateLtoR(buf,XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(messagedW,
        XmNmessageString,xmstr,
        NULL);
    XmStringFree(xmstr);
    XtManageChild(messagedW);
    XtPopup(XtParent(messagedW),XtGrabNone);
}
