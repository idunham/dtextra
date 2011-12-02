/*
 *  create the interface
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
 */

#include "xhead.h"
#include "mxkill.h"
#include "menu.h"
#include "regex.h"

#include "converter.h"
#include "bitmaps.h"
#include "mxkicon.xbm"

static Pixmap
    kpix[6];

static Widget
        aboutdW,
        xpsheadtextW,
        xpslistW,
        xpstextW,
        xzapPbW,
        xregextextW;

static Widget
    SmsgtextW;

void CreateKillPixmaps(fg,bg)
Pixel
    fg,
    bg;
{
     int
        i;


     for (i=0; i < 6; i++)
        kpix[i] = (Pixmap) NULL;

     /*
     ** terminate (15)
     */
     kpix[0]=XCreatePixmapFromBitmapData(Gdisplay,
        Grootwindow,
        skull_bits,
        skull_width,
        skull_height,
        fg,
        bg,
        Gdepth); 
    
     /*
     ** quit (3)
     */

     kpix[1]=XCreatePixmapFromBitmapData(Gdisplay,
        Grootwindow,
        skull1_bits,
        skull1_width,
        skull1_height,
        fg,
        bg,
        Gdepth); 
  
     /* 
     ** kill (9)
     */ 
 
     kpix[2]=XCreatePixmapFromBitmapData(Gdisplay, 
        Grootwindow, 
        skull2_bits, 
        skull2_width, 
        skull2_height, 
        fg, 
        bg,
        Gdepth);

     /* 
     ** hangup (1)
     */ 
 
     kpix[3]=XCreatePixmapFromBitmapData(Gdisplay, 
        Grootwindow, 
        phone_bits, 
        phone_width, 
        phone_height, 
        fg, 
        bg,
        Gdepth);

     /* 
     **  stop (17)
     */ 
 
     kpix[4]=XCreatePixmapFromBitmapData(Gdisplay, 
        Grootwindow, 
        stillman_bits, 
        stillman_width, 
        stillman_height, 
        fg, 
        bg,
        Gdepth);

     /* 
     ** continue (19)
     */ 
 
     kpix[5]=XCreatePixmapFromBitmapData(Gdisplay, 
        Grootwindow, 
        runman_bits, 
        runman_width, 
        runman_height, 
        fg, 
        bg,
        Gdepth);

    for(i=0; i < 6; i++)
    {
        if (kpix[i] == (Pixmap) NULL)
            Error("Unable to create kill pixmaps",(char *) NULL);
    }
}


void CreateMainWindow(parent)
Widget
    parent;
{
    Arg 
        args[20];

    int
        n;

    Widget
        xmainwW,
        xmenubarW,
        xhelpW,
        xformW,
        xpsPbW,
        xframeW,
        xrcW,
        xregexformW,
        xregexLabW,
        xregexframeW,
        xregexrcW;

    Widget
        msgframeW,
        msgrcW;
        
    extern MenuItem
        file_items[],
        signal_items[],
        ps_items[],
        help_items[];

    Pixel
        fg,
        bg;

    Widget
        vsbW,
        hsbW;

    Pixmap
        pix=(Pixmap) NULL;
    /*
    ** create the main window
    */
    xmainwW=XtVaCreateWidget("xmainw",
        xmMainWindowWidgetClass,parent,
        NULL);

    XtVaGetValues(xmainwW,
        XmNforeground,&fg,
        XmNbackground,&bg,
        NULL);
    CreateKillPixmaps(fg,bg);

    /*
    ** create menubar
    */
    xmenubarW=XmCreateMenuBar(xmainwW,"menuBar",NULL,0);
    (void) MBuildPulldownMenu(xmenubarW,"File",'F',file_items);
    (void) MBuildPulldownMenu(xmenubarW,"Signal",'S',signal_items);
    (void) MBuildPulldownMenu(xmenubarW,"ps Options",'O',ps_items);
    xhelpW=MBuildPulldownMenu(xmenubarW,"Help",'H',help_items);
    XtVaSetValues(xmenubarW,
        XmNmenuHelpWidget,
        xhelpW,
        NULL);
        

    /*
    ** create a form
    */
    xformW=XtVaCreateWidget("xform",
        xmFormWidgetClass, xmainwW,
        NULL);
    

    /*
    ** create a frame
    */
    xframeW=XtVaCreateManagedWidget("xframe",
        xmFrameWidgetClass,   xformW,
        XmNtopAttachment,     XmATTACH_FORM,
        XmNleftAttachment,    XmATTACH_FORM,
        XmNrightAttachment,   XmATTACH_FORM,
        NULL);
    /*
    ** create a rc on frame
    */
    xrcW=XtVaCreateWidget("xrc",
        xmRowColumnWidgetClass, xframeW,
        XmNorientation,XmHORIZONTAL,
        NULL);

    /*
    ** create zap pushbutton
    */
    xzapPbW=XtVaCreateManagedWidget("xzappb",
        xmPushButtonWidgetClass,    xrcW,
        NULL);
    SetKillPixmap(9);

    XtAddCallback(xzapPbW,XmNactivateCallback,
        (XtCallbackProc) KillProcessCb,NULL);

    /*
    ** create the ps pushbutton
    */
    xpsPbW=XtVaCreateManagedWidget("xpspb",
        xmPushButtonWidgetClass,    xrcW,
        NULL);

    XtAddCallback(xpsPbW, XmNactivateCallback,
        (XtCallbackProc) PsCb, (XtPointer) NULL);

    pix=XCreatePixmapFromBitmapData(Gdisplay,
        Grootwindow,
        ps_bits,
        ps_width,
        ps_height,
        fg,
        bg,
        Gdepth);
        
    if(pix != (Pixmap) NULL)
        XtVaSetValues(xpsPbW,
            XmNlabelType, XmPIXMAP,
            XmNlabelPixmap, pix,
            NULL);

    /*
    ** create text widget for ps options
    */
    xpstextW=XtVaCreateManagedWidget("xpstext",
        xmTextFieldWidgetClass, xrcW,
        NULL);
    XmTextSetString(xpstextW,GpsdefOption);
    XtAddCallback(xpstextW,XmNactivateCallback,
        (XtCallbackProc) psOptionTextCb, NULL);

    /*
    ** create the text widget to put psheader
    */
    xpsheadtextW=XtVaCreateManagedWidget("xpsheadtext",
        xmTextWidgetClass, xformW,
        XmNleftAttachment,    XmATTACH_FORM,
        XmNtopAttachment,     XmATTACH_WIDGET,
        XmNtopWidget,         xframeW,
        XmNrightAttachment,   XmATTACH_FORM,
        NULL);


    /*
    ** create the 
    /*
    ** create another form
    */
    xregexformW=XtVaCreateWidget("xregform",
        xmFormWidgetClass, xformW,
        XmNleftAttachment,  XmATTACH_FORM,
        XmNbottomAttachment,XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);
    /*
    ** create a regex frame
    */
    xregexframeW=XtVaCreateManagedWidget("xwcframe",
        xmFrameWidgetClass,xregexformW,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment,XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);
    /*
    ** create a rc on the form below list widget
    */
    xregexrcW=XtVaCreateWidget("xwcrc",
        xmRowColumnWidgetClass,xregexframeW,
        XmNorientation,XmHORIZONTAL,
        NULL);

    /*
    ** create the regex label
    */
    xregexLabW=XtVaCreateManagedWidget("xwclab",
        xmLabelWidgetClass,xregexrcW,
        NULL);

    /*
    ** create the regex entering text widget
    */
    xregextextW=XtVaCreateManagedWidget("xwctext",
        xmTextFieldWidgetClass,xregexrcW,
        NULL);
    XtAddCallback(xregextextW,XmNactivateCallback,
        (XtCallbackProc) PatternMatchCb,(XtPointer) NULL);

    /*
    ** create the message frame -------------------------------
    */
    msgframeW=XtVaCreateManagedWidget("msgframew",
         xmFrameWidgetClass,xformW,
         XmNleftAttachment,  XmATTACH_FORM,
         XmNrightAttachment, XmATTACH_FORM,
         XmNbottomAttachment,XmATTACH_WIDGET,
         XmNbottomWidget,xregexformW,
         NULL);

    /*
    ** creaet the msgrcW
    */
    msgrcW=XtVaCreateWidget("msgrcw",
        xmRowColumnWidgetClass,msgframeW,
        NULL);
    /*
    ** crate the msgtext w
    */
    SmsgtextW=XtVaCreateManagedWidget("msgtextw",
        xmTextWidgetClass,msgrcW,
        XmNeditable,                False,
        XmNcursorPositionVisible,   False,
        XmNshadowThickness,         0,
        NULL);

    /*
    ** create the ps list W -----------------------------------
    */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,XmATTACH_WIDGET);        n++;
    XtSetArg(args[n], XmNtopWidget,xpsheadtextW);               n++;
    XtSetArg(args[n], XmNleftAttachment,XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNrightAttachment,XmATTACH_FORM);        n++;
    XtSetArg(args[n], XmNbottomAttachment,XmATTACH_WIDGET);     n++;
    /*
    XtSetArg(args[n], XmNbottomWidget,xregexformW);             n++;
    */
    XtSetArg(args[n], XmNbottomWidget,msgframeW);             n++;
    XtSetArg(args[n], XmNbottomOffset, 5);                      n++;
    /*
    XtSetArg(args[n], XmNscrollBarPlacement,XmBOTTOM_RIGHT);    n++;
    */
    XtSetArg(args[n], XmNlistSizePolicy,XmCONSTANT);            n++;
    XtSetArg(args[n], XmNselectionPolicy,XmMULTIPLE_SELECT);    n++;
    XtSetArg(args[n], XmNlistMarginWidth, 2);                   n++;
    XtSetArg(args[n], XmNlistMarginHeight, 2);                  n++;

    xpslistW=XmCreateScrolledList(xformW,"xpslist",
        args,n);

    /*
    ** set the color of the scrollbar
    */
    XtVaGetValues (XtParent(xpslistW),
        XmNverticalScrollBar, &vsbW,
        XmNhorizontalScrollBar, &hsbW,
        NULL);

    bg=(Pixel) AllocateNamedColor(Gdisplay,"#b2c3df");
    SetScrollbarColors(vsbW,hsbW,bg);

    /*
    ** Manage Widgets
    */
    XtManageChild(msgrcW);
    XtManageChild(xpslistW);
    XtManageChild(xregexrcW);
    XtManageChild(xregexformW);
    XtManageChild (xrcW);
    XtManageChild (xformW);
    XtManageChild(xmenubarW);
    XtManageChild(xmainwW);
}

/*
** Null callback
*/
void NullCb(widget,client_data,call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
    (void) fprintf (stderr," I am Null callback, I do nothing!\n");
}

/*
** Quit callback
*/
void QuitCb(widget,client_data,call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
    FreeKillPixmaps();
    exit(0);
}

/*
** set the signal number from Signal menu
*/

void SignalSetCb(widget,client_data,call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
    int
        sig_no=(int) client_data;
    register int
        i;

    Gsignal_no=sig_no;
    
    for (i=0; i < 6; i++)
    {
        XtVaSetValues(Gtoggle_menuW[i],
            XmNset, False,
            NULL);
    }
    XtVaSetValues(widget,
        XmNset, True,
        NULL);

    SetKillPixmap(Gsignal_no);
#ifdef DEBUG
    (void) fprintf (stderr," Signal: %d\n",Gsignal_no);
#endif
}

void FreeKillPixmaps()
{
    int
        i;

    for (i=0; i < 6; i++)
    {
        if (kpix[i] != (Pixmap) NULL)
            XFreePixmap(Gdisplay,kpix[i]);
    }
}

void SetKillPixmap(sig_no)
int
    sig_no;
{
    switch(sig_no)
    {
        case 15:
        {
            SetPixmap(kpix[0],xzapPbW);
            break;
        }

        case 3:
        {
            SetPixmap(kpix[1],xzapPbW);
            break;
        }
        
        case 9:
        {
            SetPixmap(kpix[2],xzapPbW);
            break;
        }

        case 1:
        {
            SetPixmap(kpix[3],xzapPbW);
            break;
        }

        case 17:
        {
            SetPixmap(kpix[4],xzapPbW);
            break;
        }

        case 19:
        {
            SetPixmap(kpix[5],xzapPbW);
            break;
        }
    }
}

void SetPixmap(pixm,widget)
Pixmap
    pixm;
Widget
    widget;
{
    XtVaSetValues(widget,
        XmNlabelType, XmPIXMAP,
        XmNlabelPixmap,pixm,
        NULL);
}


/*
** ps pushbutton callback
*/

void PsCb(widget,client_data,call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
    char
        *psops;

    extern char
        *psheader;

    unsigned int
        status;
    char
        commandbuf[1024];
     
    SetbusyCursorState(GtoplevelW,1);
    psops=GetpsOptions();
    *commandbuf='\0';

    (void) sprintf(commandbuf,"%s -%s",GpsCommand,GpsdefOption);
    if (psops != (char *) NULL)
        (void) sprintf(commandbuf,"%s -%s",GpsCommand,psops); 
    
    status=GetpsInfo(commandbuf);

#ifdef DEBUG
    (void) fprintf (stderr," ps command: %s\n",commandbuf);
    (void) fprintf (stderr," ps status: %d\n",status);
    (void) fprintf (stderr," Gmaxline: %d\n", Gmaxline);
    (void) fprintf (stderr," psheader:%s\n",psheader);
#endif
    if (status)
    {
        if (psheader != (char *) NULL)
            XmTextSetString(xpsheadtextW,psheader);
        PutListOnScreen();
    }
    SetbusyCursorState(GtoplevelW,0);
    WriteMessage("",0);
}

/*
** see if there is any ps option
*/
char *GetpsOptions()
{
    char
        *s=(char *) NULL;

    s=XmTextGetString(xpstextW);

    if ((int) strlen(s) != 0)
        return (s);

return ((char *) NULL);
}

/*
** callback for the ps option text widget
**
**  line was getting freed, actually there was no need for it,8/30/1997
*/

void psOptionTextCb(widget,client_data,call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
    PsCb((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);
}

void PutListOnScreen()
{
    extern int
        Gmaxline;

    extern char
        *lines[];

    XmString
        *xmtable;

    register int
        i;

    int
        n;
    n=Gmaxline;
    xmtable=(XmString *) calloc(n,sizeof(XmString));
    for (i=0; i < Gmaxline; i++)
    {
        xmtable[i]=XmStringCreateSimple(lines[i]);
    }
    XtUnmapWidget(xpslistW);
    XmListDeleteAllItems(xpslistW);
    XmListAddItems(xpslistW,xmtable,n,0);
    XtMapWidget(xpslistW);

    for (i=0; i < n; i++)
        XmStringFree (xmtable[i]);
}

/*
** kill callback
*/

void KillProcessCb(widget,client_data,call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
    GetListPositions();
}

/*
** multiple selection callback
*/

void GetListPositions()
{
    Boolean
        status;

    int
        *position_list;

    int
        position_count;

    extern char
        *lines[];
    extern int
        pidcol;
    int
        i;

    int
        rc;
    int
        pid;

    register char
        *p;

    status=XmListGetSelectedPos(xpslistW,&position_list,&position_count);

    if (status)
    {
        for(i=0; i < position_count; i++)
        {
            p=lines[position_list[i]-1];
            p += pidcol;
        
            rc=sscanf(p,"%d",&pid);
            if (rc != 1)
            {
                XBell(Gdisplay,0);
                 (void) fprintf (stderr," Problem finding PID\n");
                 break;
            }
            if (pid > 0) 
            {
                rc=kill(pid,Gsignal_no);
                if (rc != 0)
                    (void) fprintf (stderr," Problem killing pid: %d\n",pid);
            }
        }
        (void) free ((char *) position_list);
        PsCb((Widget) NULL, (XtPointer) NULL, (XtPointer) NULL);
    }
    else
        XBell(Gdisplay,0);

}

/*
** callback routine for pattern match
*/
void PatternMatchCb(widget,client_data,call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
    Widget
        listw=xpslistW;

    char
        *s_text,
        *text,
        *newtext,
        *exp;

    XmString
        *strlist,
        *selectlist;

    int
        cnt,
        matched,
        j;

    char
        buf[128];

    j=0;
    selectlist=(XmString *) NULL;
    newtext=XmTextFieldGetString(widget);

    if (!newtext || !*newtext)
    {
        XtFree(newtext);
        return;
    }


    s_text=XtNewString(newtext);

    /*
    ** compile expression into pattern matching library
    */

    if (exp=re_comp(newtext))
    {
        (void) fprintf (stderr," Error with re_comp(%s): %s\n",
            newtext,exp);
        XtFree(newtext);
        return;
    }

    XtVaGetValues(listw,
        XmNitemCount, &cnt,
        XmNitems, &strlist,
        NULL);

    while(cnt--)
    {
        if (!XmStringGetLtoR(strlist[cnt],XmFONTLIST_DEFAULT_TAG,&text))
            break;

        matched=re_exec(text) > 0;
        if(matched)
        {
            selectlist=(XmString *) XtRealloc((XmString *) selectlist,
                (j+1)*(sizeof(XmString *)));

            selectlist[j++]=XmStringCopy(strlist[cnt]);
        }
        XtFree(text);
    }

    XtFree(newtext);
    XtVaSetValues(listw,
        XmNselectedItems,     selectlist,
        XmNselectedItemCount, j,
        NULL);

    if (j)
    {
        if (j > 1)
        (void) sprintf(buf,"%d matching processes found for string: %s",j,s_text);
        else
        (void) sprintf(buf,"%d matching process found for string: %s",j,s_text);

        WriteMessage(buf,0);
    }
    else
    {
        (void) sprintf(buf,"No matching process found for string: %s",s_text);
        WriteMessage(buf,1);
    }
    while (j--)
        XmStringFree(selectlist[j]);

    XtFree(s_text);
    /*XmTextFieldSetString(widget,"");*/
}

void psOptionCb(widget,client_data,call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
    switch((int) client_data)
    {
        case 0:
        {
            XmTextSetString(xpstextW,GpsdefOption);
            PsCb((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);
            break;
        }
        case 1:
        {
            XmTextSetString(xpstextW,GpsAllOption);
            PsCb((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);
            break;
        }
        case 2:
        {
            XmTextSetString(xpstextW,GpsAllbyUserOption);
            PsCb((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);
            break;
        }
        case 3:
        {
            XmTextSetString(xpstextW,GpsAllbyPidOption);
            PsCb((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);
            break;
        }
    }
}    


/*
 *  CreateAboutD - crea the about dialog
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
 *      muquit@semcor.com   02-Sep-95   first cut
 */



void CreateAboutD(parent)
Widget
    parent;
{
    Arg
        args[10];

    int
        n;

    XmString
        xmstr;

    Pixmap
        apix=(Pixmap) NULL;

    Pixel
        fg,
        bg;

    XtAppContext app=XtWidgetToApplicationContext(GtoplevelW);
    XitAddXmStringConverter(app);

    xmstr=XmStringCreateSimple("About mdgclock");
    n=0;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg (args[n], XmNdialogTitle, xmstr);      n++;

    aboutdW=XmCreateInformationDialog(parent,"aboutd",
        args,n);

    XmStringFree(xmstr);

    XtUnmanageChild(XmMessageBoxGetChild(aboutdW,XmDIALOG_HELP_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(aboutdW,XmDIALOG_CANCEL_BUTTON));

    XtVaGetValues(aboutdW,
        XmNforeground,&fg,
        XmNbackground,&bg,
        NULL);

    apix=XCreatePixmapFromBitmapData(Gdisplay,
        Grootwindow,
        mxkicon_bits,
        mxkicon_width,
        mxkicon_height,
        bg,
        fg,
        Gdepth);


    if (apix != (Pixmap) NULL)
        XtVaSetValues(aboutdW,
            XmNsymbolPixmap,apix,
            NULL);
}

void AboutdCb(widget,client_data,call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
        if (!XtIsManaged(aboutdW))
        {
            XtManageChild(aboutdW);
            XtVaSetValues(aboutdW,
                XmNx, ((GdisplayWidth>>1)-50),
                XmNy, ((GdisplayHeight>>1)-50),
                NULL);
        }
}
/*
**  WriteMessage()
**  writes status message to the text widget above the regex label
**
**  RCS
**      $Revision$
**      $Date$
**  Return Values:
**      none
**
**  Parameters:
**      message     the message to write
*       bell        1 for beep, 0 for quiet
**
**  Side Effects:
**      none
**
**  Limitations and Comments:
**      none
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Aug-30-1997    first cut
*/

void WriteMessage(message,bell)
char
    *message;
int
    bell;
{
    if (bell == 1)
        XBell(Gdisplay,0);
    XmTextSetString(SmsgtextW,message);
    XmUpdateDisplay(SmsgtextW);
}
