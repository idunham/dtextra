/*
 * $XConsortium: scrollbar.c,v 1.41 91/05/22 15:20:07 gildea Exp $
 */

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 *
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*
** I believe this file is originally borrowed from mxterm source.
** I did not write it. I just modified it and made it work with various 
** xterms. Lately made it work in Tomas Dickey's xterm with cygnus on 
** MS Windows 2000 with lesstif.
** muquit@muquit.com Jul-25-2002 
*/

#include "ptyx.h"  /* gets Xt headers, too */

#ifdef MOTIF 

#include <stdarg.h> /* for varargs*/

#include <stdio.h>
#include <ctype.h>
#include <X11/Xatom.h>

#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <X11/cursorfont.h>
#include <Xm/ScrollBar.h>

#include "data.h"
#include "error.h"
#include "menu.h"

/* Event handlers */

static void ScrollTextTo();
static void ScrollTextUpDownBy();

#ifndef M_MIN
#define M_MIN(x,y)  (((x) < (y)) ? (x) : (y))
#endif

#define MCFL    __FILE__,__LINE__
static unsigned long m_allocate_named_color(Display *display,Colormap colormap,
                                            char *name);
static void m_set_scrollbar_colors(Widget vsbW, Widget hsbW);




/* resize the text window for a terminal screen, modifying the
 * appropriate WM_SIZE_HINTS and taking advantage of bit gravity.
 */

static void ResizeScreen(XtermWidget xw,int min_width,int min_height )
{
    register TScreen *screen = &xw->screen;
#ifndef nothack

    XSizeHints sizehints;
    long supp;
#endif

    XtGeometryResult geomreqresult;
    Dimension reqWidth, reqHeight, repWidth, repHeight;

    /*
     * I'm going to try to explain, as I understand it, why we
     * have to do XGetWMNormalHints and XSetWMNormalHints here,
     * although I can't guarantee that I've got it right.
     *
     * In a correctly written toolkit program, the Shell widget
     * parses the user supplied geometry argument.  However,
     * because of the way xterm does things, the VT100 widget does
     * the parsing of the geometry option, not the Shell widget.
     * The result of this is that the Shell widget doesn't set the
     * correct window manager hints, and doesn't know that the
     * user has specified a geometry.
     *
     * The XtVaSetValues call below tells the Shell widget to
     * change its hints.  However, since it's confused about the
     * hints to begin with, it doesn't get them all right when it
     * does the SetValues -- it undoes some of what the VT100
     * widget did when it originally set the hints.
     *
     * To fix this, we do the following:
     *
     * 1. Get the sizehints directly from the window, going around
     *    the (confused) shell widget.
     * 2. Call XtVaSetValues to let the shell widget know which
     *    hints have changed.  Note that this may not even be
     *    necessary, since we're going to right ahead after that
     *    and set the hints ourselves, but it's good to put it
     *    here anyway, so that when we finally do fix the code so
     *    that the Shell does the right thing with hints, we
     *    already have the XtVaSetValues in place.
     * 3. We set the sizehints directly, this fixing up whatever
     *    damage was done by the Shell widget during the
     *    XtVaSetValues.
     *
     * Gross, huh?
     *
     * The correct fix is to redo VTRealize, VTInitialize and
     * VTSetValues so that font processing happens early enough to
     * give back responsibility for the size hints to the Shell.
     *
     * Someday, we hope to have time to do this.  Someday, we hope
     * to have time to completely rewrite xterm.
     */

#ifndef nothack
    /*
     * NOTE: If you change the way any of the hints are calculated
     * below, make sure you change the calculation both in the
     * sizehints assignments and in the XtVaSetValues.
     */

    if (! XGetWMNormalHints(screen->display, XtWindow(XtParent(xw)),
                            &sizehints, &supp))
        sizehints.flags = 0;
    sizehints.base_width = min_width;
    sizehints.base_height = min_height;
    sizehints.width_inc = FontWidth(screen);
    sizehints.height_inc = FontHeight(screen);
    sizehints.min_width = sizehints.base_width + sizehints.width_inc;
    sizehints.min_height = sizehints.base_height + sizehints.height_inc;
    sizehints.flags |= (PBaseSize|PMinSize|PResizeInc);
    /* These are obsolete, but old clients may use them */
    sizehints.width = (screen->max_col + 1) * FontWidth(screen)
                      + min_width;
    sizehints.height = (screen->max_row + 1) * FontHeight(screen)
                       + min_height;
#endif

    /*
     * Note: width and height are not set here because they are 
     * obsolete.       
     */
    XtVaSetValues(XtParent(xw),
                  XtNbaseWidth, min_width,
                  XtNbaseHeight, min_height,
                  XtNwidthInc, FontWidth(screen),
                  XtNheightInc, FontHeight(screen),
                  XtNminWidth, min_width + FontWidth(screen),
                  XtNminHeight, min_height + FontHeight(screen),
                  NULL);

    reqWidth = (screen->max_col + 1) * FontWidth(screen) + min_width;
    reqHeight = FontHeight(screen) * (screen->max_row + 1) + min_height;
    geomreqresult = XtMakeResizeRequest ((Widget)xw, reqWidth, reqHeight,
                                         &repWidth, &repHeight);

    if (geomreqresult == XtGeometryAlmost)
    {
        geomreqresult = XtMakeResizeRequest ((Widget)xw, repWidth,
                                             repHeight, NULL, NULL);
    }

#ifndef nothack
    XSetWMNormalHints(screen->display, XtWindow(XtParent(xw)), &sizehints);
#endif
}

void DoResizeScreen (XtermWidget xw)
{
    int border = 2 * xw->screen.border;
     ResizeScreen (xw, border + xw->screen.fullVwin.sb_info.width,border);
}


static Widget CreateScrollBar(XtermWidget xw,int x,int y,int height)
{
    Pixel
    bg; /* scrollbar bg , muquit, 5/18/96 */


    Widget scrollWidget;
    static Arg argList[] =
    {
          {XmNx,               (XtArgVal) 0},
          {XmNy,               (XtArgVal) 0},
          {XmNheight,          (XtArgVal) 0},
          /* motif widget doesn't have reverse video garbage */
          {XmNorientation,     (XtArgVal) XmVERTICAL},
          {XmNborderWidth,     (XtArgVal) 1},
    };

    argList[0].value = (XtArgVal) x;
    argList[1].value = (XtArgVal) y;
    argList[2].value = (XtArgVal) height;

    scrollWidget = XtCreateWidget("scrollbar", xmScrollBarWidgetClass,
                                  (Widget) xw, argList, XtNumber(argList));
    XtAddCallback (scrollWidget, XmNvalueChangedCallback, ScrollTextTo, 0);
    XtAddCallback (scrollWidget, XmNdragCallback, ScrollTextTo, 0);

    /*
    ** now set the scrollbar color. the color name is obtained from 
    ** the X resources file XTerm-ansi if specified or use the one from
    ** fallback_resources
    ** ma_muquit@fccc.edu, Aug-03-1997
    */
    m_set_scrollbar_colors(scrollWidget,(Widget) NULL);

    return (scrollWidget);
}

static void RealizeScrollBar (Widget sbw,TScreen *screen)
{
    Cursor
        arrowCursor;

    XtRealizeWidget (sbw);

    /* define pointer cursor for it */
    arrowCursor = XCreateFontCursor(XtDisplay(sbw), XC_left_ptr);
    XDefineCursor(XtDisplay(sbw), XtWindow(sbw), arrowCursor);
    XFreeCursor(XtDisplay(sbw), arrowCursor);
}


void ScrollBarReverseVideo(Widget scrollWidget)
{
}

void ScrollBarDrawThumb(Widget scrollWidget)
{
    register TScreen *screen = &term->screen;
    register int thumbTop, thumbHeight, totalHeight;
    Arg wargs[4];
    register int n = 0;

    thumbTop    = screen->topline + screen->savedlines;
    thumbHeight = screen->max_row + 1;
    totalHeight = thumbHeight + screen->savedlines;

    XtSetArg(wargs[n], XmNmaximum, totalHeight);
    n++;
    XtSetArg(wargs[n], XmNsliderSize, thumbHeight);
    n++;
    XtSetArg(wargs[n], XmNvalue, thumbTop);
    n++;
    XtSetArg(wargs[n], XmNpageIncrement, thumbHeight);
    n++;
    XtSetValues(scrollWidget, wargs, n);
}

void ResizeScrollBar(TScreen *screen)
{

    /* muquit@muquit.com Jul-23-2002 */
    XtConfigureWidget(screen->scrollWidget,
                      -1,-1,
                      screen->scrollWidget->core.width,
                      screen->fullVwin.height + screen->border * 2,
                      screen->scrollWidget->core.border_width);

    ScrollBarDrawThumb(screen->scrollWidget);
}

void WindowScroll(TScreen *screen,int top)
{
    register int i, lines;
    register int scrolltop, scrollheight, refreshtop;
    register int x = 0;

    if (top < -screen->savedlines)
        top = -screen->savedlines;
    else if (top > 0)
        top = 0;
    if((i = screen->topline - top) == 0)
    {
        ScrollBarDrawThumb(screen->scrollWidget);
        return;
    }

    /*ScrollSelection(screen, i);*/

    if(screen->cursor_state)
        HideCursor();
    lines = i > 0 ? i : -i;
    if(lines > screen->max_row + 1)
        lines = screen->max_row + 1;
    scrollheight = screen->max_row - lines + 1;
    if(i > 0)
        refreshtop = scrolltop = 0;
    else
    {
        scrolltop = lines;
        refreshtop = scrollheight;
    }
    /*x = screen->scrollbar + screen->border;*/
    x=OriginX(screen);

    scrolling_copy_area(screen, scrolltop, scrollheight, -i);
    screen->topline = top;

    ScrollSelection(screen, i);

    XClearArea(
        screen->display,
        /*TextWindow(screen),*/
        VWindow(screen),
        (int) x,
        (int) refreshtop * FontHeight(screen) + screen->border,
        (unsigned) Width(screen),
        (unsigned) lines * FontHeight(screen),
        FALSE);
    ScrnRefresh(screen, refreshtop, 0, lines, screen->max_col + 1, False);

    ScrollBarDrawThumb(screen->scrollWidget);
}


void ScrollBarOn (XtermWidget xw,int init,int  doalloc)
{
    register TScreen *screen = &xw->screen;
    register int border = 2 * screen->border;
    register int i,j,k;

    /*
    if(screen->scrollbar)
        return;
    */
    if(screen->fullVwin.sb_info.width)
        return;



    if (init)
    {   /* then create it only */
        if (screen->scrollWidget)
            return;

        /* make it a dummy size and resize later */
        if ((screen->scrollWidget = CreateScrollBar (xw, -1, - 1, 5))
                == NULL)
        {
            Bell(XkbBI_MinorError,0);
            return;
        }

        return;

    }

    if (!screen->scrollWidget)
    {
        Bell (XkbBI_MinorError,0);
        Bell (XkbBI_MinorError,0);
        return;
    }

    if (doalloc && screen->allbuf)
    {
        /* FIXME: this is not integrated well with Allocate */
        if((screen->allbuf =
                    (ScrnBuf) realloc((char *) screen->visbuf,
                                      (unsigned) MAX_PTRS*(screen->max_row + 2 +
                                                           screen->savelines) *
                                      sizeof(char *)))
                == NULL)
            SysError(ERROR_SBRALLOC);
        screen->visbuf = &screen->allbuf[MAX_PTRS * screen->savelines];
        memmove( (char *)screen->visbuf, (char *)screen->allbuf,
                 MAX_PTRS * (screen->max_row + 2) * sizeof (char *));
        for (i = k = 0; i < screen->savelines; i++)
        {
            k += BUF_HEAD;
            for (j = BUF_HEAD; j < MAX_PTRS; j++)
            {
                if((screen->allbuf[k++] =
                            (Char *)calloc((unsigned) screen->max_col + 1, sizeof(Char))) ==
                        NULL)
                    SysError(ERROR_SBRALLOC2);
            }
        }
    }


    ResizeScrollBar (screen);
    xtermAddInput(screen->scrollWidget);

    XtRealizeWidget (screen->scrollWidget);

    /* Now set the left ptr cursor for the scrollbar muquit@muquit.com */
    XDefineCursor(XtDisplay(screen->scrollWidget),
            XtWindow(screen->scrollWidget),
            screen->arrow);


    screen->fullVwin.sb_info.rv_cached = False;
    screen->fullVwin.sb_info.width = screen->scrollWidget->core.width +
                                     screen->scrollWidget->core.border_width;
    ScrollBarDrawThumb(screen->scrollWidget);
    DoResizeScreen (xw);
    XtMapWidget(screen->scrollWidget);
    update_scrollbar ();
    if (screen->visbuf)
    {
        XClearWindow (screen->display, XtWindow (term));
        Redraw ();
    }

    /* XXX TODO if scrollbar is on right muquit@muquit.com */
}

void ScrollBarOff(TScreen *screen)
{
    /*
    if(!screen->scrollbar)
        return;
    */

    if(!screen->fullVwin.sb_info.width)
        return;

    XtUnmapWidget(screen->scrollWidget);
    screen->fullVwin.sb_info.width=0;
    DoResizeScreen (term);
    update_scrollbar ();
    if (screen->visbuf)
    {
        XClearWindow (screen->display, XtWindow (term));
        Redraw ();
    }
}

/*ARGSUSED*/
static void ScrollTextTo(Widget scrollbarWidget,caddr_t closure,
        XmScrollBarCallbackStruct *call_data)
{
    register TScreen *screen = &term->screen;
    int thumbTop; /* relative to first saved line */
    int newTopLine;

    thumbTop = call_data->value;
    newTopLine = thumbTop - screen->savedlines;
    WindowScroll(screen, newTopLine);
}

/*ARGSUSED*/
static void ScrollTextUpDownBy(Widget scrollbarWidget,Opaque closure,
        int pixels)
{
    register TScreen *screen = &term->screen;
    register int rowOnScreen, newTopLine;

    rowOnScreen = pixels / FontHeight(screen);
    if (rowOnScreen == 0)
    {
        if (pixels < 0)
            rowOnScreen = -1;
        else if (pixels > 0)
            rowOnScreen = 1;
    }
    newTopLine = screen->topline + rowOnScreen;
    WindowScroll(screen, newTopLine);
}


/*
 * assume that b is lower case and allow plural
 */
static int specialcmplowerwiths (char *a,char *b)
{
    register char ca, cb;

    if (!a || !b)
        return 0;

    while (1)
    {
        ca = *a;
        cb = *b;
        if (isascii(ca) && isupper(ca))
        {  /* lowercasify */
#ifdef _tolower
            ca = _tolower (ca);
#else

            ca = tolower (ca);
#endif

        }
        if (ca != cb || ca == '\0')
            break;  /* if not eq else both nul */
        a++, b++;
    }
    if (cb == '\0' && (ca == '\0' || (ca == 's' && a[1] == '\0')))
        return 1;

    return 0;
}

static int params_to_pixels (TScreen *screen,String *params,int n)
{
    register mult = 1;
    register char *s;

    switch (n > 2 ? 2 : n)
    {
        case 2:
        s = params[1];
        if (specialcmplowerwiths (s, "page"))
        {
            mult = (screen->max_row + 1) * FontHeight(screen);
        }
        else if (specialcmplowerwiths (s, "halfpage"))
        {
            mult = ((screen->max_row + 1) * FontHeight(screen)) >> 1;
        }
        else if (specialcmplowerwiths (s, "pixel"))
        {
            mult = 1;
        } /* else assume that it is Line */
        mult *= atoi (params[0]);
        break;
        case 1:
        mult = atoi (params[0]) * FontHeight(screen); /* lines */
        break;
        default:
        mult = screen->scrolllines * FontHeight(screen);
        break;
    }

    return mult;
}


/*ARGSUSED*/
void HandleScrollForward (Widget gw,XEvent *event,String *params,
        Cardinal *nparams)
{
    XtermWidget w = (XtermWidget) gw;
    register TScreen *screen = &w->screen;

    ScrollTextUpDownBy (gw, (Opaque) NULL,
                        params_to_pixels (screen, params, (int) *nparams));
    return;
}


/*ARGSUSED*/
void HandleScrollBack (Widget gw,XEvent *event,String *params,
        Cardinal *nparams)
{
    XtermWidget w = (XtermWidget) gw;
    register TScreen *screen = &w->screen;

    ScrollTextUpDownBy (gw, (Opaque) NULL,
                        -params_to_pixels (screen, params, (int) *nparams));
    return;
}


static int m_find_best_pixel(Display *display, Colormap colormap, XColor *colors,
                      unsigned int number_colors,XColor *color);

/*
 * MallocateColor() - Allocates color and returns Pixel Value
 *
 * RCS:
 *  $Revision: 2 $
 *  $Date: 5/25/03 9:47a $
 *
 * Security:
 *  Unclassified
 *
 * Description:
 *  This function allocates color and returns the pixel index of the
 *  colormap.  If the requested color is not available, it will try
 *  to determine the closest possible color.
 *
 * Input Parameters:
 *  type identifier description
 *
 *  Display  *display
 *  Colormap colormap
 *  int   R, G, B
 *
 * Output Parameters:
 *  type identifier description
 *
 *  text
 *
 * Return Values:
 *  value description
 *  
 *  returns the pixel value
 *
 * Side Effects:
 *  text
 *
 * Limitations and Comments:
 *  text
 *
 * Development History:
 *  when who  why
 * 11/2/93  mm  first_cut
 * 06/1/94  mm  returns the best pixel if colormap is full
 */
unsigned long   MallocateColor(Display *display, Colormap colormap,
                               int R,int G, int B)
{
    XColor
    color;

    int
    status;
    int
    screen;

    int
    rc;

    rc=300;
    screen=XDefaultScreen(display);
    /*
    ** detemine the intensities from the R, G, abd B values
    */

    color.red = R<<8;
    color.green = G<<8;
    color.blue = B<<8;
    color.flags = DoRed | DoGreen | DoBlue;

    status = XAllocColor (display, colormap, &color);

    if (status == 0)
    {
        /*
        ** Colormap is FULL, Now find the closest possible color
        */

        rc=m_find_best_pixel (display, colormap, (XColor *) NULL,
                       (unsigned int) M_MIN(DisplayCells(display,screen),256),
                       &color);
    }

    if (rc == 300)
        return (color.pixel);
    else
    {
        return (BlackPixel(display,screen));
    }
}


/*
 * m_find_best_pixel() - Returns the best pixel 
 *
 * RCS:
 *  $Revision: 2 $
 *  $Date: 5/25/03 9:47a $
 *
 * Security:
 *  Unclassified
 *
 * Description:
 *  This function returns the closet possible color rquested.
 *  It simply determines the closest point in the RGB space of the
 *  requested R, G, B values.
 *
 *  Adapted from Imagemagick 3.0.0
 *
 * Input Parameters:
 *  type identifier description
 *
 *  Display   *display;
 *  Colormap  colormap;
 *  XColor   *colors;
 *  unsigned int number_colors;
 *  XColor   *color;
 *
 * Output Parameters:
 *  type identifier description
 *
 *  XColor *color;
 *
 * Return Values:
 *  value description
 *      300     on success
 *      301     on failure
 *
 * Side Effects:
 *  text
 *
 * Limitations and Comments:
 *  text
 *
 * Development History:
 *  when who  why
 * 6/3/94  mm  ????
 */
static int m_find_best_pixel(Display *display, Colormap colormap, XColor *colors,
                      unsigned int number_colors,XColor *color)
{
    int
    ii,
    m_colors;

    register int
    blue_distance,
    green_distance,
    i,
    red_distance;

    unsigned long
    distance,
    min_distance;

    m_colors = colors == (XColor *) NULL;

    if (m_colors)
    {
        colors = (XColor *) malloc(number_colors*sizeof(XColor));
        if (colors == (XColor *) NULL)
        {
            (void) fprintf(stderr,"%s (%d) Unable to get X Server Color, Memory allocation failed\n",MCFL);
            return (301);
        }

        for (i=0; i < number_colors; i++)
        {
            colors[i].pixel = (unsigned long) i;
        }

        if (number_colors > 256)
        {
            number_colors = 256;
        }

        XQueryColors(display,colormap,colors,number_colors);
    }

    color->pixel=0;
    min_distance=(unsigned long) (~0); /* just a big number */
    ii=0;

    for (i=0; i < number_colors; i++)
    {
        red_distance=(int) (colors[i].red >> 8)-(int) (color->red >> 8);
        green_distance=(int) (colors[i].green >> 8)-(int) (color->green >> 8);
        blue_distance=(int) (colors[i].blue >> 8)-(int) (color->blue >> 8);

        /*
        ** distance is distance, the co-ordinates values do not make any
        ** diffetence, i.e., negative co-ordinate value will give the same
        ** distance, because the points are squired.  We do not care about
        ** taking square root of the distance, because, we just need to find
        ** the point closest to the requested point. distance is not important
        */

        distance=red_distance*red_distance+green_distance*green_distance+
                 blue_distance*blue_distance;

        if (distance < min_distance)
        {
            min_distance=distance;
            color->pixel=colors[i].pixel;
            ii=i;
        }
    }

    /*
    ** this is the closest pixel. So, share it, allocate as Read only
    */

    (void) XAllocColor(display,colormap,&colors[ii]);
    if (m_colors)
    {
        (void) free((char *) colors);
    }

    return (300);
}


/*
 * m_allocate_named_color - allocates color from it's name
 *
 * RCS:
 *  $Revision: 2 $
 *  $Date: 5/25/03 9:47a $
 *
 * Security:
 *  Unclassified
 *
 * Description:
 *  text
 *
 * Input Parameters:
 *  type identifier description
 *
 *  text
 *
 * Output Parameters:
 *  type identifier description
 *
 *  text
 *
 * Return Values:
 *  value description
 *      pixel value  if succeeds
 *      257          if failes
 *
 * Side Effects:
 *  text
 *
 * Limitations and Comments:
 *  text
 *
 * Development History:
 *  when who  why
 * 9/23/93  mm  first cut
 * 6/8/94  mm  returns best pixel if colormap is full
 */

static unsigned long m_allocate_named_color(Display *display,Colormap colormap,
        char *name)
{
    XColor
    def;
    int
    status;

    int
    screen=XDefaultScreen(display);
    if (XParseColor (display,colormap,name,&def ))
    {
        status = XAllocColor (display,colormap,&def);
        if (status == 0)
        {
            /* colormap is full, so find the closest color */
            (void) m_find_best_pixel (display, colormap, (XColor *) NULL,
                 (unsigned int) M_MIN(DisplayCells(display,screen),256),&def);
        }
    }
    else
    {
        (void) fprintf(stderr,"%s (%d) Could not Parse Color: %s\n",MCFL,name);
        return(257);
    }

    return(def.pixel);
}

/*
 *  m_set_scrollbar_colors()    -   set scrollbar colors
 *
 *  RCS:
 *      $Revision: 2 $
 *      $Date: 5/25/03 9:47a $
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

static void m_set_scrollbar_colors(Widget vsbW, Widget hsbW)
{
    Colormap
        colormap;

    Pixel
        scb_bg,
        scb_topshadow,
        scb_bottomshadow,
        scb_select,
        scb_border;

    extern char *g_scb_bgcolor;

    /*
    ** get the colormap
    */


    XtVaGetValues (vsbW,
                   XmNcolormap, &colormap,
                   NULL);

    if (g_scb_bgcolor == NULL)
        g_scb_bgcolor="#97a3bc";
    scb_bg=(Pixel)m_allocate_named_color(XtDisplay(vsbW),colormap,
            g_scb_bgcolor);
    /*
    ** let motif calculate the new colors based on the bg color
    */

    if (vsbW != (Widget ) NULL)
    {
        XtVaSetValues (vsbW,
                       XmNbackground,scb_bg,
                       NULL);
    }

    if (hsbW != (Widget) NULL)
    {
        XtVaSetValues(hsbW,
                      XmNbackground,scb_bg,
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

    if (vsbW != (Widget) NULL)
    {
        XtVaSetValues (vsbW,
                       XmNbackground,          scb_bg,
                       XmNtopShadowColor,      scb_topshadow,
                       XmNbottomShadowColor,   scb_bottomshadow,
                       XmNtroughColor,         scb_select,
                       XmNarmColor,            scb_select,
                       XmNborderColor,         scb_border,
                       NULL);
    }


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

/*
 * Toggle the visibility of the scrollbars.
 */
void
ToggleScrollBar(XtermWidget w)
{
    TScreen *screen = &w->screen;

    if (screen->fullVwin.sb_info.width)
    {
        ScrollBarOff (screen);
    }
    else
    {
        ScrollBarOn (w, FALSE, FALSE);
    }
    update_scrollbar();
}

#endif /* MOTIF */
