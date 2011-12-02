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

void CreateAskDialog (parent)
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

    xmstr = XmStringCreateSimple ("Congrats!");
    n = 0;
    XtSetArg (args[n], XmNdialogTitle, xmstr);          n++;
    XtSetArg (args[n], XmNdialogStyle,
        XmDIALOG_PRIMARY_APPLICATION_MODAL);            n++;
    XtSetArg (args[n], XmNautoUnmanage, False);         n++;

    GaskdW = XmCreatePromptDialog (parent, "askD",
        args, n);
    XmStringFree (xmstr);

     xmstr = XmStringCreateSimple ("Yes");
     XtVaSetValues (GaskdW,
        XmNokLabelString, xmstr,
        NULL);

     XmStringFree (xmstr);

     xmstr = XmStringCreateSimple ("No");
     XtVaSetValues (GaskdW,
        XmNcancelLabelString, xmstr,
        NULL);        

     XmStringFree (xmstr);
    
     XtUnmanageChild(XmSelectionBoxGetChild (GaskdW,
        XmDIALOG_TEXT));
     XtUnmanageChild(XmSelectionBoxGetChild (GaskdW,
        XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild (GaskdW,
        XmDIALOG_HELP_BUTTON));

    /*
    ** create the form
    */

    formW = XtVaCreateWidget ("askForm",
        xmFormWidgetClass, GaskdW,
        NULL);

    /*
    ** create the electric dialog
    */

    elecdW = XtVaCreateManagedWidget ("elecAsk",
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


    GelecRespW = elecdW;

    XtManageChild (formW);
}
