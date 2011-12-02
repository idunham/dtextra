/*
 *  CreateAskDialog() -   create the ask dialog
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
 *      muquit@semcor.com   23-Apr-95   first cut
 */

#include "xhead.h"
#include "defines.h"
#include "xmastm.h"

#include <ElecDisp.h>

void CreateAskDialog2 (parent)
Widget
    parent;
{
    int
        n;

    Arg
        args[20];

    XmString
        xmstr;

    Widget
        formW,
        elecdW;

    xmstr = XmStringCreateSimple ("Sorry!");
    n = 0;
    XtSetArg (args[n], XmNdialogTitle, xmstr);          n++;
    XtSetArg (args[n], XmNdialogStyle,
        XmDIALOG_PRIMARY_APPLICATION_MODAL);            n++;
    XtSetArg (args[n], XmNautoUnmanage, False);         n++;

    Gaskd2W = XmCreatePromptDialog (parent, "askD2",
        args, n);
     XmStringFree (xmstr);

     xmstr = XmStringCreateSimple ("Yes");
     XtVaSetValues (Gaskd2W,
        XmNokLabelString, xmstr,
        NULL);

     XmStringFree (xmstr);

     xmstr = XmStringCreateSimple ("No");
     XtVaSetValues (Gaskd2W,
        XmNcancelLabelString, xmstr,
        NULL);        

     XmStringFree (xmstr);
    
     XtUnmanageChild(XmSelectionBoxGetChild (Gaskd2W,
        XmDIALOG_TEXT));
     XtUnmanageChild(XmSelectionBoxGetChild (Gaskd2W,
        XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild (Gaskd2W,
        XmDIALOG_HELP_BUTTON));

    /*
    ** create the form
    */

    formW = XtVaCreateWidget ("askForm",
        xmFormWidgetClass, Gaskd2W,
        NULL);

    /*
    ** create the electric dialog
    */

    elecdW = XtVaCreateManagedWidget ("elecAsk2",
        xmtdsElecDispWidgetClass, formW,
        XtNinterval, 0,
        XtNpixHeight, 1,
        XtNpixWidth, 1,
        XtNrasterWidth, 200,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);
     
    XmtdsEDStopFunc (elecdW, NULL, 0);
    XmtdsEDClearFunc( elecdW, NULL, 0);


    GelecResp2W = elecdW;

    XtManageChild (formW);
}
