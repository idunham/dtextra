/*
 *  function_name() -   description
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

int AskCB (widget, client_data, call_data)
Widget
    widget;
XtPointer
    client_data,
    call_data;
{
    char
        elecbuf[MaxTextLength*2];

    register int
        i;

   static int
        answer;


    XtAppContext
        app;

   *elecbuf = '\0';
   answer = 0;
   app = XtWidgetToApplicationContext(widget);

/*
   (void) sprintf (elecbuf, "%s", "set-font(-schumacher-*-*-*-*-*-16-*-*-*-*-*-*-*)");
   (void) sprintf (elecbuf, "%s%s",elecbuf, "speed(500)");
   (void) sprintf (elecbuf, "%s%s", elecbuf,"message(50,0,Congratulations!)");
   (void) sprintf (elecbuf, "%s%s", elecbuf,"clear()");
   (void) sprintf (elecbuf, "%s%s", elecbuf,"message(50,0,Congratulations!)");
   (void) sprintf (elecbuf, "%s%s", elecbuf,"clear()");
   (void) sprintf (elecbuf, "%s%s", elecbuf,"message(50,0,Congratulations!)");
   (void) sprintf (elecbuf, "%s%s", elecbuf,"clear()");
   (void) sprintf (elecbuf, "%s%s", elecbuf, "set-font(fixed)");
   (void) sprintf (elecbuf, "%s%s", elecbuf, "speed(10)");
   (void) sprintf (elecbuf, "%s%s", elecbuf, 
        "message(40,0, Do\\\\ You\\\\ Want\\\\ To\\\\ Play\\\\ Again?)");

   for (i=0; i < 200; i++)
        (void) sprintf(elecbuf,"%s%s", elecbuf, "scroll-left()");

   (void) fprintf (stderr,"length: %d\n", (int) strlen(elecbuf));
   (void) sprintf (elecbuf, "%s%s", elecbuf, "clear()");
   (void) fprintf (stderr,"%s\n",elecbuf);
 

   XtVaSetValues (GelecRespW,
        XtNprogram, elecbuf,
        NULL);
*/

   switch ((int) client_data)
   {
        case HappyFace:
        {
            XmtdsEDRestartFunc (GelecRespW, NULL, 0);
            break;
        }

        case SadFace:
        {
            XmtdsEDRestartFunc (GelecResp2W, NULL, 0);
            break;
        }
   }
   XtManageChild (widget);

   XtAddCallback (widget, XmNokCallback, (XtCallbackProc) Response,
        &answer);
   
   XtAddCallback (widget, XmNcancelCallback, (XtCallbackProc) Response,
        &answer);

   while (answer == 0 ||
        XtAppPending(app))
    XtAppProcessEvent(app, XtIMAll);

    if ((int) client_data == HappyFace)
        XmtdsEDStopFunc (GelecRespW, NULL, 0);
    else
        XmtdsEDClearFunc (GelecResp2W, NULL, 0);

    XtUnmanageChild (widget);
    return (answer);
}

void Response (widget,ans, cbs)
Widget
    widget;

XtPointer
    ans;
   
XmAnyCallbackStruct
    *cbs;
{
    int
        *answer;

    answer = (int *) ans;
  
    switch (cbs->reason)
    {
        case XmCR_OK:
            *answer = YES;
            break;
  
        case XmCR_CANCEL:
            *answer = NO;
            break;
    }
}

