/*
 *  SetScrollbarColors()    -   set scrollbar colors
 *
 *  RCS:
 *      $Revision: 1.1 $
 *      $Date: 1995/05/07 19:34:10 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      This function determines the best color for the vertical scrollbar
 *      based on the color which will be it's background color.
 *      We have to do this way, because we did not create the scrollbar, we
 *      got it from scrolled window
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
 *      None
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      passed backgroung color is already allocated in the called routine
 *
 *  Development History:
 *      when    who     why
 *  08/22/94    muquit  first cut
 */

#include "xhead.h"
#include "mxkill.h"


void SetScrollbarColors(vsbW, hsbW, bgcolor)
Widget
    vsbW;

Widget
    hsbW;

Pixel
    bgcolor;
{
    Colormap
        colormap;

    Pixel 
        scb_bg,
        scb_topshadow,
        scb_bottomshadow,
        scb_select,
        scb_border;

    /*
    ** get the colormap
    */

    scb_bg = bgcolor;

    XtVaGetValues (vsbW,
        XmNcolormap, &colormap,
        NULL);
    
    /*
    ** let motif calculate the new colors based on the bg color
    */

    XtVaSetValues (vsbW,
        XmNbackground,          scb_bg,
        NULL);
    
    if (hsbW != (Widget) NULL)
    {
        XtVaSetValues(hsbW,
            XmNbackground, scb_bg,
            NULL);
    }


    XmGetColors (XtScreen(vsbW),
        colormap, scb_bg,
        &scb_border,
        &scb_topshadow,
        &scb_bottomshadow,
        &scb_select);   /* trough color, selec_color will be for toggle b*/
    
    /*
    ** set the colors
    */

    XtVaSetValues (vsbW,
        XmNbackground,          scb_bg,
        XmNtopShadowColor,      scb_topshadow,
        XmNbottomShadowColor,   scb_bottomshadow,
        XmNtroughColor,         scb_select,
        XmNarmColor,            scb_select,
        XmNborderColor,         scb_border,
        NULL);


    if (hsbW != (Widget) NULL)
    {
        XtVaSetValues (hsbW,
            XmNbackground,          scb_bg,
            XmNtopShadowColor,      scb_topshadow,
            XmNbottomShadowColor,   scb_bottomshadow,
            XmNtroughColor,         scb_select,
            XmNarmColor,            scb_select,
            XmNborderColor,         scb_border,
            NULL);
    }
}
