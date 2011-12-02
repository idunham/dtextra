/*
 *  CreateUserInterface - routines to create user interface
 *
 *  RCS:
 *      $Revision$
 *      $Date$
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
 *      who                  when       why
 *      MA_Muquit@fccc.edu   30-May-96  first cut
 */

#include "xhead.h"
#include "mxascii.h"
#include "menu.h"

void CreateUserInterface(Widget parent)
{
    Widget
        frameW,
        main_formW,
        menubarW,

        col1_rcW,
        col2_rcW,

        col3_rcW,
        col4_rcW,

        col5_rcW,
        col6_rcW,

        col7_rcW,
        col8_rcW,

        col9_rcW,
        col10_rcW,
        
        col11_rcW,
        col12_rcW,

        helpW;

    extern MenuItem
        file_items[],
        options_items[],
        help_items[];

    Display
        *display;

    XmFontList
        fontlist;

    XFontStruct
        *font;

    display=XtDisplay (parent);
    font = XLoadQueryFont (display,
   "-b&h-lucidatypewriter-medium-r-normal-sans-14-140-75-75-m-90-iso8859-1");

    if (font == (XFontStruct *) NULL)
    {
        (void) fprintf (stderr," Failed to load font,using fixed!\n");
        font=XLoadQueryFont (display,"fixed");
    }
    else
    {
        fontlist=XmFontListCreate (font,"charset1");

    }
    
    /*
    ** create the main window W
    */
    GmainwW=XtVaCreateManagedWidget("mainww",
        xmMainWindowWidgetClass,parent,
        NULL); 

   /*
   ** create the menubar
   */
   menubarW=XmCreateMenuBar(GmainwW,"menuBar",NULL,0);
   (void) MBuildPulldownMenu(menubarW,"File",'F',file_items);
   (void) MBuildPulldownMenu(menubarW,"Options",'p',options_items);

   helpW=MBuildPulldownMenu(menubarW,"Help",'e',help_items);
   XtVaSetValues(menubarW,
        XmNmenuHelpWidget, helpW,
        NULL);

   XtManageChild(menubarW);

   /*
   ** create a frame for ornamentatl reason
   */
   frameW=XtVaCreateManagedWidget ("framew",
       xmFrameWidgetClass,GmainwW,
       NULL);

   /*
   ** create the main formW
   */
   main_formW=XtVaCreateWidget("mformw",
        xmFormWidgetClass,frameW,
        NULL);
    
    /*
    ** create col1,2 rc widget
    */
    col1_rcW=XtVaCreateWidget ("lrcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL);

    col2_rcW=XtVaCreateWidget ("lrcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget,col1_rcW,
        XmNleftOffset,10,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL);

    /*
    ** create the col1,2 labels
    */
    Gvalue_labelW[0]=XtVaCreateManagedWidget ("label",
        xmLabelWidgetClass,col1_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);

     Gascii_labelW[0]=XtVaCreateManagedWidget ("label2",
        xmLabelWidgetClass,col2_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);

    /*
    ** create 3,4 rc widgets
    */
     col3_rcW=XtVaCreateWidget ("rcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget,col2_rcW,
        XmNleftOffset,15,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL); 

     col4_rcW=XtVaCreateWidget ("rcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget, col3_rcW,
        XmNleftOffset,10,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL);

    /*
    ** create the 3,4 labels
    */

     Gvalue_labelW[1]=XtVaCreateManagedWidget ("label",
        xmLabelWidgetClass,col3_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);
     Gascii_labelW[1]=XtVaCreateManagedWidget ("label2",
        xmLabelWidgetClass,col4_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);

    /*
    ** create the 5,6 rc widgets
    */
     col5_rcW=XtVaCreateWidget ("rcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget,col4_rcW,
        XmNleftOffset,15,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL); 

     col6_rcW=XtVaCreateWidget ("rcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget, col5_rcW,
        XmNleftOffset,10,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL);

    /*
    ** create the 5,6 labels
    */

    Gvalue_labelW[2]=XtVaCreateManagedWidget ("label",
        xmLabelWidgetClass,col5_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);
     Gascii_labelW[2]=XtVaCreateManagedWidget ("label2",
        xmLabelWidgetClass,col6_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);


    /*
    ** create the col 7,8 rc widgets
    */

     col7_rcW=XtVaCreateWidget ("rcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget,col6_rcW,
        XmNleftOffset,15,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL); 

     col8_rcW=XtVaCreateWidget ("rcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget, col7_rcW,
        XmNleftOffset,10,
        XmNtopAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL);   

    /*
    ** create col 7,8 labels
    */
    Gvalue_labelW[3]=XtVaCreateManagedWidget ("label",
        xmLabelWidgetClass,col7_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);
     Gascii_labelW[3]=XtVaCreateManagedWidget ("label2",
        xmLabelWidgetClass,col8_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);

    /*
    ** create the col 9,10 rc widgets
    */

     col9_rcW=XtVaCreateWidget ("rcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget,col8_rcW,
        XmNleftOffset,15,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL); 

     col10_rcW=XtVaCreateWidget ("rcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget, col9_rcW,
        XmNleftOffset,10,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL);   

    /*
    ** create col 9,10 labels
    */
    Gvalue_labelW[4]=XtVaCreateManagedWidget ("label",
        xmLabelWidgetClass,col9_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);
     Gascii_labelW[4]=XtVaCreateManagedWidget ("label2",
        xmLabelWidgetClass,col10_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);

    /*
    ** create the col 11,12 rc widgets
    */

     col11_rcW=XtVaCreateWidget ("rcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget,col10_rcW,
        XmNleftOffset,15,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL); 

     col12_rcW=XtVaCreateWidget ("rcw",
        xmRowColumnWidgetClass,main_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget, col11_rcW,
        XmNleftOffset,10,
        XmNtopAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNorientation, XmVERTICAL,
        NULL);   

    /*
    ** create col 11,12 labels
    */
    Gvalue_labelW[5]=XtVaCreateManagedWidget ("label",
        xmLabelWidgetClass,col11_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);
     Gascii_labelW[5]=XtVaCreateManagedWidget ("label2",
        xmLabelWidgetClass,col12_rcW,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNfontList,fontlist,
        NULL);

    /*
    ** manage the widget not managed yet
    */
    XtManageChild (col12_rcW);
    XtManageChild (col11_rcW);

    XtManageChild (col10_rcW);
    XtManageChild (col9_rcW);

    XtManageChild (col8_rcW);
    XtManageChild (col7_rcW);

    XtManageChild (col6_rcW);
    XtManageChild (col5_rcW);

    XtManageChild (col4_rcW);
    XtManageChild (col3_rcW);

    XtManageChild (col2_rcW);
    XtManageChild (col1_rcW);

    XtManageChild (main_formW);
}

/*
** quit callback routine
*/

void QuitCb (Widget w, XtPointer cli_data,XtPointer call_data)
{
    exit (0);
}

/*
** create the about dialog
*/

#include "converter.h"

static Widget
    aboutdW;

void CreateAboutD(Widget parent)
{
    Arg
        args[10];
            
    int
        n;
         
    XmString   
        xmstr;
            
    /****

    Pixmap
        apix=(Pixmap) NULL;
         
    Pixel
        fg,
        bg; 

    *******/

    XtAppContext app=XtWidgetToApplicationContext(GtoplevelW);
    XitAddXmStringConverter(app);
  
    xmstr=XmStringCreateSimple("About mxascii");
    n=0;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg (args[n], XmNdialogTitle, xmstr);      n++;
 
    aboutdW=XmCreateInformationDialog(parent,"aboutd",
        args,n);

    XmStringFree(xmstr);
 
    XtUnmanageChild(XmMessageBoxGetChild(aboutdW,XmDIALOG_HELP_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(aboutdW,XmDIALOG_CANCEL_BUTTON));
 
#ifdef DO_ICON
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
#endif

}

void AboutdCb(widget,client_data,call_data)
Widget
    widget;
XtPointer  
    client_data,
    call_data;
{
        if (!XtIsManaged(aboutdW))
            XtManageChild(aboutdW);
}

