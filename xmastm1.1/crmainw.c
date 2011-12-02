/*
 *  CreateMainWindow()  -   creates main window for xmastm
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
 *      when    who                 why
 *  10/29/94    muquit@semcor.com   first cut
 */

#include "xhead.h"
#include "xmastm.h"
#include "defines.h"
#include "mqtools.h"
#include <ElecDisp.h>

#include "point.xbm"
#include "brect.xbm"

static char *Pick[]=
{
    "Red",
    "Green",
    "Blue",
    "Yellow",
    "Cyan",
    "Magenta",
};

Widget CreateMainWindow (parent)
Widget
    parent;
{
    Arg
        args[20];

	char
		*elec_args[4];
    Widget
        menubarW,
        mainwW,
        frameW,
        rframeW,
        rformW,
        formW,
        rowcolW,
        pick_rcW,
        pick_rc2,
        rowcol2,
        rowcol3,
        rowcol4,
        rowcol5,
        rowcol6,
        pushbW,
        labelW,
        sframeW,
        srowcolW,
        srowcol2W,
        slabel1W,
        slabel2W;

    extern MENUITEM
        file_items[],
        help_items[];

    Pixel
        bg_pixel;

    XmString
        xmstr;

    register int
        col,
        row;

    Widget
        hwidget,
        helpW;

    Display
        *display;

    int
        index;

    Pixmap
        point,
        bpeg,
        wpeg;

    Pixel
        fg,
        bg;

    char
        elec_buf[100];

    int
        n;

    *elec_buf = '\0';
    display = XtDisplay(parent);

    /*
    ** create main window
    */

    mainwW = XtVaCreateWidget ("MainW",
        xmMainWindowWidgetClass, parent,
        XmNshowSeparator, True,
        NULL);

    XtVaGetValues (mainwW,
        XmNforeground, &fg,
        XmNbackground, &bg,
        NULL);

    /*
    ** create menubar
    */

    menubarW = XmCreateMenuBar (mainwW, "Menubar",
        NULL, 0);

    (void) MBuildPulldownMenu (menubarW, "File", 'F', file_items);
    helpW = MBuildPulldownMenu (menubarW, "Help", 'H', help_items);

    XtVaSetValues (menubarW,
        XmNmenuHelpWidget, helpW,
        NULL);

    point = XCreatePixmapFromBitmapData (display,
         RootWindowOfScreen(XtScreen(parent)),
         point_bits,
         point_width,
         point_height,
         fg,
         bg,
         DefaultDepthOfScreen(XtScreen(parent)));   

    /*
    ** create the black peg for scorin
    */

    bpeg = XCreatePixmapFromBitmapData (display,
        RootWindowOfScreen(XtScreen(parent)),
        brect_bits,
        brect_width,
        brect_height,
        BlackPixelOfScreen (XtScreen(parent)),
        bg,
        DefaultDepthOfScreen(XtScreen(parent)));
    wpeg = XCreatePixmapFromBitmapData (display,
        RootWindowOfScreen(XtScreen(parent)),
        brect_bits,
        brect_width,
        brect_height,
        WhitePixelOfScreen (XtScreen(parent)),
        bg,
        DefaultDepthOfScreen(XtScreen(parent)));

    /*
    ** create a frame
    */

    frameW = XtVaCreateManagedWidget("Frame",
        xmFrameWidgetClass, mainwW,
        XmNtraversalOn, False,
        NULL);

    /*
    ** create a form on the frame
    */

    formW = XtVaCreateWidget ("FormW",
        xmFormWidgetClass, frameW,
        NULL);

    /*
    ** create the electric display
    */

    GelecLogoW = XtVaCreateManagedWidget("Electric",
        xmtdsElecDispWidgetClass, formW,
        XtNinterval, 0,
        XtNpixHeight, 1,
        XtNpixWidth, 1,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    XmtdsEDStopFunc (GelecLogoW, NULL, 0);
    XmtdsEDClearFunc(GelecLogoW, NULL, 0);


/*
    n = 0;
    elec_args[n] = "15"; n++;
    elec_args[n] = "2"; n++;
    (void) sprintf (elec_buf, "%s", "X Mastermind 1.1");
    elec_args[n] = elec_buf; n++;
    XmtdsEDMessageFunc (GelecLogoW,
        elec_args, n);

*/

    /*
    ** create the row col for picking colors
    */

    pick_rcW = XtVaCreateWidget ("Pickrc",
        xmRowColumnWidgetClass, formW,
        XmNorientation, XmVERTICAL,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, GelecLogoW,
        NULL);

    index = 0;
    for (row=0; row < 6; row++)
    {
        pick_rc2 = XtVaCreateWidget ("Pickrc2",
            xmRowColumnWidgetClass, pick_rcW,
            XmNorientation, XmHORIZONTAL,
            NULL);
        
        Gpbg[row] = (Pixel) AllocateNamedColor(display, Pick[row]);
        for (col=0; col < Cols; col++)
        {
            frameW = XtVaCreateManagedWidget("Frame",
                xmFrameWidgetClass, pick_rc2,
                XmNwidth, 40,
                XmNheight, 30,
                NULL);

            xmstr = XmStringCreateSimple("");
            pushbW = XtVaCreateManagedWidget("pb",
                xmPushButtonWidgetClass, frameW,
                XmNlabelString, xmstr,
                XmNbackground, Gpbg[row],
                NULL);
            XmStringFree (xmstr);

            XtAddCallback (pushbW, XmNactivateCallback,
                (XtCallbackProc) SetColor, (XtPointer) index);
            index++;
        }
        XtManageChild (pick_rc2);
        
    }

    /*
    ** create row col for the pointer bitmaps
    */

    rowcol3 = XtVaCreateWidget ("Pointrc",
        xmRowColumnWidgetClass, formW,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, pick_rcW,
        XmNorientation, XmVERTICAL,
        NULL);


    for (col=0; col < 1; col++)
    {
       rowcol4 = XtVaCreateWidget("rocol4",
        xmRowColumnWidgetClass, rowcol3,
        XmNorientation, XmHORIZONTAL,
        NULL);

       for (col=0; col < Cols; col++)
       {
            frameW = XtVaCreateManagedWidget("Frame",
                xmFrameWidgetClass, rowcol4,
                XmNwidth, 40,
                XmNheight, 20,
                NULL);

            labelW = XtVaCreateManagedWidget("Pointlab",
                xmLabelWidgetClass, frameW,
                XmNwidth, 40,
                XmNheight, 20,
                XmNlabelType, XmPIXMAP,
                XmNlabelPixmap, point,
                NULL);
        }
    }

    /*
    ** create a frame to hold the scores etc
    */
    sframeW = XtVaCreateManagedWidget("Frame",
        xmFrameWidgetClass, formW,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, rowcol3,
        XmNleftOffset, 4,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, pick_rcW,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    srowcolW = XtVaCreateWidget ("Scrowcol",
        xmRowColumnWidgetClass, sframeW,
        XmNorientation, XmVERTICAL,
        NULL);

    for (col=0; col < 1; col++)
    {
        srowcol2W = XtVaCreateWidget ("Scrowcol2",
            xmRowColumnWidgetClass, srowcolW,
            XmNorientation, XmHORIZONTAL,
            NULL);
        for (col=0; col < 2; col++)
        {
            frameW = XtVaCreateManagedWidget("Frame",
                xmFrameWidgetClass, srowcol2W,
                XmNshadowThickness, 0,
                XmNwidth, 40,
                XmNheight,20,
                NULL);

            switch (col)
            {
                case 0:
                {
                    slabel1W = XtVaCreateManagedWidget("Pointlab",
                        xmLabelWidgetClass, frameW,
                        XmNwidth, 40,
                        XmNheight,20,
                        XmNlabelType, XmPIXMAP,  
                        XmNlabelPixmap, bpeg,
                        XmNrecomputeSize, False,
                        NULL);

                    break;
                }

                case 1:
                {
                    slabel2W = XtVaCreateManagedWidget("Pointlab",
                        xmLabelWidgetClass, frameW,
                        XmNwidth, 40,
                        XmNheight,20,
                        XmNlabelType, XmPIXMAP,  
                        XmNlabelPixmap, wpeg,
                        XmNrecomputeSize, False,
                        NULL);
                    break;
                }
            }
        }
        XtManageChild (srowcol2W);
    }

    /*
    ** create the score labels
    */

    index = 0;
    for (row=0; row < Rows; row++)
    {
        rowcol6 = XtVaCreateWidget ("Rowcol6",
            xmRowColumnWidgetClass, srowcolW,
            XmNorientation, XmHORIZONTAL,
            NULL);

        for (col=0; col < 2; col++)
        {
            frameW = XtVaCreateManagedWidget("Frame",
                xmFrameWidgetClass, rowcol6,
                XmNshadowType, XmSHADOW_IN,
                XmNwidth, 40,
                XmNheight, 20,
                NULL);
            xmstr = XmStringCreateSimple("");
            GscoreLabW[index] = XtVaCreateManagedWidget("scoreLab",
                xmLabelWidgetClass, frameW,
                XmNwidth, 40,
                XmNheight,20,
                XmNlabelString, xmstr,
                XmNrecomputeSize, False,
                NULL);
            XmStringFree (xmstr);
            index++;
        }
        XtManageChild (rowcol6);
    }        
        /*
        ** create a row col widget
        */

    rowcolW = XtVaCreateWidget ("Rowcol",
        xmRowColumnWidgetClass, formW,
        XmNorientation, XmVERTICAL,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, rowcol3,
        NULL);

    index = 0;
    for (row=0; row < Rows; row++)
    {
            rowcol2 = XtVaCreateWidget ("Rowcol2",
                xmRowColumnWidgetClass, rowcolW,
                XmNorientation, XmHORIZONTAL,
                NULL);
        for (col=0; col < Cols; col++)
        {

            frameW = XtVaCreateManagedWidget("Frame",
                xmFrameWidgetClass, rowcol2,
                XmNshadowType, XmSHADOW_OUT,
                XmNshadowThickness, 3,
                XmNwidth, 40,
                XmNheight, 20,
                NULL);

            xmstr = XmStringCreateSimple("");
            w_special[index] = XtVaCreateManagedWidget("RED",
                xmLabelWidgetClass, frameW,
                XmNlabelString, xmstr,
                NULL);
            XmStringFree (xmstr);
            index++;
        }
        XtManageChild (rowcol2);

    }

    /*
    ** create a frame for holding the hidden code
    */

    frameW = XtVaCreateManagedWidget ("Frame",
        xmFrameWidgetClass, rowcolW,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, rowcolW,
        NULL);

    rowcol5 = XtVaCreateWidget ("Rowcol",
        xmRowColumnWidgetClass, frameW,
        XmNorientation, XmHORIZONTAL,
        NULL);

    for (col=0; col < Cols; col++)
    {
        frameW = XtVaCreateManagedWidget("Frame",
            xmFrameWidgetClass, rowcol5,
            XmNshadowType, XmSHADOW_OUT,
            XmNshadowThickness, 1,
            XmNwidth, 40,
            XmNheight, 20,
            NULL);

        xmstr = XmStringCreateSimple("??");
        GhiddenCodeW[col] = XtVaCreateManagedWidget("Hcode",
            xmLabelWidgetClass, frameW,
            XmNlabelString, xmstr,
            NULL);
        XmStringFree (xmstr);            
    }
    /*
    ** create a frame at the right side
    */

    rframeW = XtVaCreateManagedWidget ("Rframe",
        xmFrameWidgetClass, formW,
        XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, GelecLogoW,
        XmNtopOffset, 6,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, rowcolW,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);


    /*
    ** create a form
    */

    rformW = XtVaCreateWidget ("Rform",
        xmFormWidgetClass, rframeW,
        NULL);

    /*
    ** create commit push button
    */

    pushbW = XtVaCreateManagedWidget ("commit",
        xmPushButtonWidgetClass, rformW,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    XtAddCallback (pushbW, XmNactivateCallback,
        (XtCallbackProc) CommitCB,
        NULL);

    XtManageChild (rformW);
    XtManageChild (rowcol5);
    XtManageChild (rowcolW);
    XtManageChild (rowcol4);
    XtManageChild (srowcolW);
    XtManageChild (rowcol3);
    XtManageChild (pick_rcW);
    XtManageChild (formW);
    XtManageChild (menubarW);
    XtManageChild (mainwW);
    return (mainwW);
}
