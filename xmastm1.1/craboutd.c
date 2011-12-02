/*
 *  CreateAboutDialog() - creates the about dialog of xmastm1.1
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
 *      muquit@semcor.com   08-Dec-94   first cut
 */

#include "xhead.h"
#include "defines.h"
#include "xmastm.h"

#include "brain.xbm"


Boolean CreateAboutDialog (parent)
Widget
    parent;
{
    Arg
        args[10];
    int
        n;

    XmString
        xs_pname,
        xs_desc,
        xs_tmp,
        xs_tmp2,
        xs_copyr,
        xs_final,
        xs_email,
        xs_email2,
        xs_femail,
        xs;

    Pixel
        fg,
        bg;

    Pixmap
        pxmp = (Pixmap) NULL;
    
    char
        strbuf[256];

    xs_pname = (XmString) NULL;
    xs_tmp2 = (XmString) NULL;

    xs = XmStringCreateSimple ("About Yrolo");
    n = 0;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg (args[n], XmNdialogTitle, xs); n++;
    GaboutW = XmCreateInformationDialog(parent, "Aboutd",
        args, n);

    XtUnmanageChild (XmMessageBoxGetChild(GaboutW,XmDIALOG_HELP_BUTTON));
    XtUnmanageChild (XmMessageBoxGetChild(GaboutW,XmDIALOG_CANCEL_BUTTON));
    XmStringFree(xs);

    /*
    ** get the fg and bg color of the dialog, we will use them
    ** to properly mask the info pixmap
    */


    XtVaGetValues (GaboutW,
        XmNforeground, &fg,
        XmNbackground, &bg,
        NULL);

    pxmp = XCreatePixmapFromBitmapData(XtDisplay(GaboutW),
        RootWindowOfScreen(XtScreen(GaboutW)),
        mastm_bits,
        mastm_width, 
        mastm_height,
        fg,
        bg,
        DefaultDepthOfScreen(XtScreen(GaboutW)));

    if (pxmp != (Pixmap) NULL)
    {
        XtVaSetValues(GaboutW,
            XmNsymbolPixmap, pxmp,
            NULL);
    }

    sprintf(strbuf, "XMastm $ Revision %s $\n\n%s\n",
        Version,
        "Mastermind in Motif");

    xs_desc = XmStringCreateLtoR(strbuf, CHSET1);

    (void) sprintf(strbuf,"%s",
        "Email: ");
    xs_email = XmStringCreateLtoR(strbuf, CHSET4);

    (void) sprintf(strbuf,"%s",
        "muquit@semcor.com");

    xs_email2=XmStringCreate(strbuf, CHSET5);

    xs_femail = XmStringConcat(xs_email, xs_email2);
/*
    sprintf(strbuf, "\n\n%s\n",
        "(Copyright 1995, Muhammad A Muquit)");
*/
    sprintf(strbuf, "\n\n%s\n",
        "URL: http://www.semcor.com/~muquit/");
    xs_copyr = XmStringCreateLtoR(strbuf, CHSET3);

    xs_tmp = XmStringConcat (xs_desc, xs_femail);
    xs_final = XmStringConcat(xs_tmp, xs_copyr);

    XtVaSetValues(GaboutW,
        XmNmessageString, xs_final,
        NULL);

    XmStringFree(xs_pname);
    XmStringFree(xs_tmp);
    XmStringFree(xs_desc);
    XmStringFree(xs_final);
    XmStringFree(xs_email);
    XmStringFree(xs_email2);
    XmStringFree(xs_femail);
    XmStringFree(xs_tmp2);

    return (True); /* Xt wil remove Work Proc automatically */
}
