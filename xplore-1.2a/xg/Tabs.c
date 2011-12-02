/*@
 * Copyright(c) 1995-1997 Gregory M. Messner
 * All rights reserved
 *
 * Permission to use, copy, modify and distribute this material for
 * non-commercial personal and educational use without fee is hereby
 * granted, provided that the above copyright notice and this permission 
 * notice appear in all copies, and that the name of Gregory M. Messner
 * not be used in advertising or publicity pertaining to this material
 * without the specific, prior written permission of Gregory M. Messner 
 * or an authorized representative.
 *
 * GREGORY M. MESSNER MAKES NO REPRESENTATIONS AND EXTENDS NO WARRANTIES, 
 * EXPRESS OR IMPLIED, WITH RESPECT TO THE SOFTWARE, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * ANY PARTICULAR PURPOSE, AND THE WARRANTY AGAINST INFRINGEMENT OF PATENTS
 * OR OTHER INTELLECTUAL PROPERTY RIGHTS. THE SOFTWARE IS PROVIDED "AS IS",
 * AND IN NO EVENT SHALL GREGORY M. MESSNER BE LIABLE FOR ANY DAMAGES,
 * INCLUDING ANY LOST PROFITS OR OTHER INCIDENTAL OR CONSEQUENTIAL DAMAGES
 * RELATING TO THE SOFTWARE.
 *
 */

/* +++FHDR+++
 *
 *	Filename: Tabs.c
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.5 07 Nov 1996
 *
 *  Description:
 *  This is the source file for the Tabs widget.
 *
 *
 *
 *  Changes
 *
 *  By     Date           Description
 *  ------ -------------- ----------------------------------------------
 *  GMM    06/14/95       Original file
 *  GMM    07/26/95       Now keep pixmaps of vertical tab labels,
 *                        this speeds things up substantially
 *
 *
 * ---FHDR--- */


/*
 * These includes are needed to describe the widget
 */
#include <Xm/XmP.h>
#include "TabsP.h"

/*
 * We include this for rotated text
 */
#include "x_rotate.h"



#if NeedFunctionPrototypes
#  undef PROTO
#  define PROTO(f, p) f p
#else
#  undef PROTO
#  define PROTO(f, p) f()
#endif


/*
 * Ratio of height to slant for slanted shape 
 */
#define TAB_SKEW    3

/*
 * Length of corner for chamferred shape
 */
#define SLANT       4        /* Length of corner for chamferred shape */


#define SHADOW_OFFSET(s)        ((s < 1) ? 1 : s)

/*
 * Various macros used within
 */
#define RIGHT_TABS(tabw)   (tabw->tabs.numtabs - tabw->tabs.curtab - 1)
#define TAB_SHAPE(tabw)    (tabw->tabs.tab_shape)
#define TAB_LOCATION(tabw) (tabw->tabs.tab_location)
#define IS_HORIZ_TAB(tabw) (TAB_LOCATION(tabw) == XgTOP || \
                                TAB_LOCATION(tabw) == XgBOTTOM)
#define HEIGHT(w)          w->core.height
#define WIDTH(w)           w->core.width
#define TOP_SHADOW_GC(tabw)         (tabw->tabs.topgc)
#define BOTTOM_SHADOW_GC(tabw)      (tabw->tabs.bottomgc)
#define HIGHLIGHT_THICKNESS(tabw)   (tabw->primitive.highlight_thickness)




/*
 * Method prototypes
 */
PROTO(static void ClassInitialize, ( void ) );
PROTO(static void Initialize, ( XgTabsWidget,XgTabsWidget,ArgList,Cardinal *));
PROTO(static void Realize, ( Widget tabw, XtValueMask *mask, 
        XSetWindowAttributes *attributes ) );
PROTO(static XtGeometryResult QueryGeometry, (
        XgTabsWidget, XtWidgetGeometry*, XtWidgetGeometry * ) );
PROTO(static Boolean SetValues,(XgTabsWidget, XgTabsWidget, XgTabsWidget));
PROTO(static void GetValuesHook, ( 
	XgTabsWidget w, ArgList args, Cardinal *num_args ) );
PROTO(static void Resize, ( XgTabsWidget));
PROTO(static void Destroy, ( XgTabsWidget ));
PROTO(static void Redraw, ( XgTabsWidget, XEvent *, Region ));


/*
 * Prototypes for the tab drawing functions
 */
PROTO(static void DrawLeftTab, ( XgTabsWidget, Region, int ));
PROTO(static void DrawRightTab, ( XgTabsWidget, Region, int ));
PROTO(static void DrawCurrentTab, ( XgTabsWidget, Region ));
PROTO(static void Highlight, ( XgTabsWidget ));
PROTO(static void UnHighlight, ( XgTabsWidget ));
PROTO(static void HorizCurrentChamferred, (XgTabsWidget, Region, Boolean));
PROTO(static void HorizRightChamferred, (XgTabsWidget, Region, int, Boolean));
PROTO(static void HorizLeftChamferred, (XgTabsWidget, Region, int, Boolean));
PROTO(static void HorizCurrentSlant, (XgTabsWidget, Region, Boolean));
PROTO(static void HorizRightSlant, (XgTabsWidget, Region, int, Boolean));
PROTO(static void HorizLeftSlant, (XgTabsWidget, Region, int, Boolean));
PROTO(static void VertCurrentChamferred, (XgTabsWidget, Region, Boolean));
PROTO(static void VertBottomChamferred, (XgTabsWidget, Region, int, Boolean));
PROTO(static void VertTopChamferred, (XgTabsWidget, Region, int, Boolean));
PROTO(static void VertCurrentSlant, (XgTabsWidget, Region, Boolean));
PROTO(static void VertBottomSlant, (XgTabsWidget, Region, int, Boolean));
PROTO(static void VertTopSlant, (XgTabsWidget, Region, int, Boolean));
PROTO(static void DrawHorizText, ( XgTabsWidget, int, int, int, int,
    unsigned long, Boolean ) );
PROTO(static void DrawVertText, ( XgTabsWidget, int, int, int, int,
    unsigned long, unsigned long, Region, Boolean ) );


/*
 * Prototypes for utility functions
 */
PROTO(static void CreateTopGc, ( XgTabsWidget));
PROTO(static void CreateBottomGc, ( XgTabsWidget));
PROTO(static void CreateTextGc, ( XgTabsWidget));
PROTO(static void CreateFillGc, ( XgTabsWidget));
PROTO(static void CopyBg, ( XgTabsWidget,int ,XrmValue *));
PROTO(static int  GetMainTabWidth, ( XgTabsWidget, int ) );
PROTO(static void GetPreferredSize, ( XgTabsWidget, int *, int * ) );
PROTO(static void SetMainTabWidth, ( XgTabsWidget ) );
PROTO(static void SetTabs, ( XgTabsWidget, int ) );
PROTO(static XFontStruct *GetFontStruct, ( XmFontList ) );
PROTO(static void CreatePixmapArray, ( XgTabsWidget ) );
PROTO(static void FreePixmapArray, ( XgTabsWidget ) );



/*
 * Prototypes for the resource convertors
 */
PROTO(static Boolean cvtStringToStringArray, (
    Display *display,
    XrmValuePtr args,
    Cardinal *num_args,
    XrmValuePtr from,
    XrmValuePtr to,
    XtPointer *converter_data
));
PROTO(static Boolean cvtStringToTabShape, (
    Display *display,
    XrmValuePtr args,
    Cardinal *num_args,
    XrmValuePtr from,
    XrmValuePtr to,
    XtPointer *converter_data
));
PROTO(static void freeStringArray, ( String * ) );


/*
 * Define things for the widgets actions
 */
PROTO(static void Activate, ( XgTabsWidget, XEvent*, String*, Cardinal* ));
PROTO(static void TraverseTabs, ( XgTabsWidget, XEvent*, String*, Cardinal* ));
PROTO(static void ProcessTraversal, (XgTabsWidget,XEvent*,String*,Cardinal*));

static char defaultTranslations[] =
    "<FocusIn>:             ProcessTraversal(in)  \n\
     <FocusOut>:            ProcessTraversal(out) \n\
     <Btn1Down>:            Activate(mouse)       \n\
     <Key>space:            Activate(space)       \n\
     <Key>osfUp:            TraverseTabs(prev)    \n\
     <Key>osfDown:          TraverseTabs(next)    \n\
     <Key>osfRight:         TraverseTabs(next)    \n\
     <Key>osfLeft:          TraverseTabs(prev)    \n\
     ~Shift<Key>Tab:        ProcessTraversal(next)\n\
     Shift<Key>Tab:         ProcessTraversal(prev) ";

static XtActionsRec actionsList[] = {
    { "Activate",         (XtActionProc)Activate         },
    { "TraverseTabs",     (XtActionProc)TraverseTabs     },
    { "ProcessTraversal", (XtActionProc)ProcessTraversal }};


/*
 * Define the resource table
 */
static XtResource resources[] = {
    {
        XgNtabCount, XgCTabCount, XmRInt, sizeof(int),
        XtOffsetOf(XgTabsRec,tabs.numtabs),
        XtRImmediate, (XtPointer)1 
    },
    {
        XgNcurrentTab, XgCCurrentTab, XmRInt, sizeof(int),
        XtOffsetOf(XgTabsRec,tabs.curtab),
        XtRImmediate, (XtPointer)0 
    },
    {
        XgNfocusTab, XgCFocusTab, XmRInt, sizeof(int),
        XtOffsetOf(XgTabsRec,tabs.focus_tab),
        XtRImmediate, (XtPointer)0 
    },
    {
        XgNtabLabels, XgCTabLabels, XmRStringArray,
        sizeof(((XgTabsRec*)NULL)->tabs.tablabels),
        XtOffsetOf(XgTabsRec,tabs.tablabels),
        XtRImmediate, (XtPointer)NULL 
    },
    {
        XgNtabShape, XgCShape, XmRInt, sizeof(int),
        XtOffsetOf(XgTabsRec,tabs.tab_shape),
        XtRImmediate, (XtPointer)XgSLANTED
    },
    {
        XgNtabLocation, XgCLocation, XmRInt, sizeof(int),
        XtOffsetOf(XgTabsRec,tabs.tab_location),
        XtRImmediate, (XtPointer)XgTOP
    },
    {
        XgNtabWidgets, XgCTabWidgets, XmRWidgetList, sizeof(Widget *),
        XtOffsetOf(XgTabsRec,tabs.tabwidgets),
        XtRImmediate, (XtPointer)NULL 
    },
    {
        XgNautoUnmanage, XmCAutoUnmanage, XmRBoolean, sizeof(Boolean),
        XtOffsetOf(XgTabsRec,tabs.auto_unmanage),
        XtRImmediate, (XtPointer)False
    },
    {
        XgNtabColor, XmCBackground, XmRPixel, sizeof(Pixel),
        XtOffsetOf(XgTabsRec,tabs.tabcolor),
        XtRCallProc, (XtPointer)CopyBg 
    },
    {
        XgNselectedTabColor, XmCBackground, XmRPixel, sizeof(Pixel),
        XtOffsetOf(XgTabsRec,tabs.selected_color),
        XtRCallProc, (XtPointer)CopyBg 
    },
    {
        XgNfontList, XmCFontList, XmRFontList, 
        sizeof(((XgTabsRec*)NULL)->tabs.font_list),
        XtOffsetOf(XgTabsRec,tabs.font_list),
        XtRString, (XtPointer)XtDefaultFont 
    },
    {
        XgNactivateCallback, XmCCallback, XmRCallback,
        sizeof(((XgTabsRec*)NULL)->tabs.activateCallback),
        XtOffsetOf(XgTabsRec,tabs.activateCallback),
        XtRImmediate, (XtPointer)NULL 
    },
    {
        XgNlabelMargin, XmCMargin, XmRInt, sizeof(int),
        XtOffsetOf(XgTabsRec,tabs.textmargin),
        XtRImmediate, (XtPointer)3 
    },
    {
        XgNhighlightThickness, XmCHighlightThickness, 
        XmRDimension, sizeof(Dimension), 
        XtOffsetOf(XgTabsRec, primitive.highlight_thickness),
        XtRImmediate, (XtPointer)1
    },
    {
        XgNnavigationType, XmCNavigationType, 
        XmRNavigationType, sizeof(XmNavigationType), 
        XtOffsetOf(XgTabsRec, primitive.navigation_type),
        XtRImmediate, (XtPointer)XmTAB_GROUP
    }
};


/*
 * Define the class record for the widget
 */
XgTabsClassRec tabsClassRec = {
    { 
            /* 
         * core_class part 
         */
        (WidgetClass) &xmPrimitiveClassRec,    /* superclass           */
        "XgTabs",                              /* class_name           */
        sizeof(XgTabsRec),                     /* widget_size          */
        ClassInitialize,                       /* ClassInitialize      */
        NULL,                                  /* class_part_initialize*/
        FALSE,                                 /* class_inited         */
        (XtInitProc)Initialize,                /* initialize           */
        NULL,                                  /* initialize_hook      */
        Realize,                               /* realize              */
        actionsList,                           /* actions              */
        XtNumber(actionsList),                 /* num_actions          */
        resources,                             /* resources            */
        XtNumber(resources),                   /* num_resources        */
        NULLQUARK,                             /* xrm_class            */
        TRUE,                                  /* compress_motion      */
        XtExposeCompressMultiple,              /* compress_exposure    */
        False,                                 /* compress_enterleave  */
        False,                                 /* visible_interest     */
        (XtWidgetProc)Destroy,                 /* destroy              */
        (XtWidgetProc)Resize,                  /* resize               */
        (XtExposeProc)Redraw,                  /* expose               */
        (XtSetValuesFunc)SetValues,            /* set_values           */
        NULL,                                  /* set_values_hook      */
        XtInheritSetValuesAlmost,              /* set_values_almost    */
        (XtArgsProc)GetValuesHook,             /* get_values_hook      */
        NULL,                                  /* accept_focus         */
        XtVersion,                             /* version              */
        NULL,                                  /* callback_private     */
        defaultTranslations,                   /* tm_table             */
        (XtGeometryHandler)QueryGeometry,      /* query_geometry       */
        XtInheritDisplayAccelerator,           /* display_accelerator  */
        NULL                                   /* extension            */
    },
    { 
        /* 
         * primitive_class part 
         */
        NULL,                 /* border_highlight */
        NULL,                 /* border_unhighlight */
        NULL,                 /* translations */
        NULL,                 /* arm_and_activate */
        0,                    /* syn_resources */
        0,                    /* num_syn_resources */
        NULL,                 /* primitive_extension */
    },
    { 
        /*
         * tabs_class part
         */
        0
    },
};

WidgetClass xgTabsWidgetClass = (WidgetClass) &tabsClassRec;



static void
ClassInitialize()
{
    /*
     * Add a type converter for the TabShape type
     */
    XtSetTypeConverter(XmRString, XgRTabShapeType,
        (XtTypeConverter)cvtStringToTabShape, NULL, 0, 
        XtCacheAll, NULL);

    /*
     * And one for string arrays (tab labels)
     */
    XtSetTypeConverter(XmRString, XmRStringArray, cvtStringToStringArray,
       NULL, 0, XtCacheNone, (XtDestructor)freeStringArray);
}



static void 
Initialize(request, tabw, args, num_args)
XgTabsWidget request;
XgTabsWidget tabw;
ArgList      args;
Cardinal     *num_args;
{
String *tabstrings;
int i, h, w;

    tabw->tabs.i_have_focus = False;

    /*
     * Make sure the focus and current tab are within range
     */
    if (  tabw->tabs.focus_tab >= tabw->tabs.numtabs )
        tabw->tabs.focus_tab = 0;

    if (  tabw->tabs.curtab >= tabw->tabs.numtabs )
        tabw->tabs.curtab = 0;



    /*
     * Create the GCs for drawing the tabs
     */
    tabw->tabs.topgc = tabw->tabs.bottomgc = 
        tabw->tabs.textgc = tabw->tabs.fillgc = None;
    CreateTopGc(tabw);
    CreateBottomGc(tabw);
    CreateTextGc(tabw);
    CreateFillGc(tabw);


    /*
     * Get a local copy of the tab label strings
     */
    if ( tabw->tabs.tablabels != NULL ) 
    {
    Boolean last_one = False;

        tabstrings = (String*) XtMalloc((tabw->tabs.numtabs + 1) * 
                sizeof(*tabstrings));

        for ( i = 0; i < tabw->tabs.numtabs; i++ )
        {
            if ( !last_one )
                if ( tabw->tabs.tablabels[i] == NULL )
                    last_one = True;

            if ( last_one )
                tabstrings[i] = NULL;
            else
                tabstrings[i] = XtNewString(tabw->tabs.tablabels[i]);
        }

        tabstrings[tabw->tabs.numtabs] = NULL;
        tabw->tabs.labels = tabstrings;
    }
    else
        tabw->tabs.labels = NULL;


    /*
     * If the tabs are vertical allocate an array to hold the label pixmaps
     */
    tabw->tabs.label_pixmaps = NULL;
    CreatePixmapArray(tabw);


    /*
     * Set dimensions to something acceptable
     */
    GetPreferredSize(tabw, &w, &h);
    if ( WIDTH(tabw) < w )
        WIDTH(tabw) = w;
    if ( HEIGHT(tabw) < h )
        HEIGHT(tabw) = h;

    SetMainTabWidth(tabw);

}


static void
Realize(tabw,mask,attributes)
Widget tabw;
XtValueMask * mask;
XSetWindowAttributes *attributes;
{
    xmPrimitiveClassRec.core_class.realize(tabw, mask, attributes);
    XClearWindow(XtDisplay(tabw), XtWindow(tabw));
}



static XtGeometryResult
QueryGeometry(tabw, proposed, desired)
XgTabsWidget tabw;
XtWidgetGeometry *proposed, *desired;
{
#define Set(bit) (proposed->request_mode & bit)
int         width;
int        height;

    /*
     * Get the size we'd like to be
     */
    GetPreferredSize(tabw, &width, &height);
    desired->request_mode = CWWidth | CWHeight;
    desired->width = width;
    desired->height = height;

    desired->request_mode = CWWidth | CWHeight;

    if ( Set(CWWidth) && proposed->width == desired->width &&
        Set(CWHeight) && proposed->height == desired->height )
            return XtGeometryYes;

    if ( desired->width == WIDTH(tabw) &&
        desired->height == HEIGHT(tabw) )
            return XtGeometryNo;

    return XtGeometryAlmost;

#undef Set
}



static void
Destroy(tabw)
XgTabsWidget tabw;
{
int i;

    /*
     * Free the local copy of the labels
     */
    if ( tabw->tabs.labels != NULL ) 
    {
        for ( i = 0; tabw->tabs.labels[i] != NULL; i++ )
           XtFree(tabw->tabs.labels[i]);

        XtFree((XtPointer)tabw->tabs.labels);
    }

    /*
     * Free the array of label pixmaps
     */
    FreePixmapArray(tabw);


    /*
     * Destroy the GC created for this widget
     */
    if ( tabw->tabs.topgc != NULL ) 
            XtReleaseGC((Widget)tabw, tabw->tabs.topgc);

    if ( tabw->tabs.bottomgc != NULL ) 
            XtReleaseGC((Widget)tabw, tabw->tabs.bottomgc);

    if (tabw->tabs.textgc != NULL) 
            XFreeGC(XtDisplay((Widget)tabw), tabw->tabs.textgc);

    if (tabw->tabs.fillgc != NULL) 
            XFreeGC(XtDisplay((Widget)tabw), tabw->tabs.fillgc);
}



static void
Activate(tabw, event, params, num_params)
XgTabsWidget tabw;
XEvent *event;
String *params;
Cardinal *num_params;
{
Dimension width, slant, height;
int old_tab, i, current_tab = -1, righttabs = RIGHT_TABS(tabw);
Dimension text_height;
XgTabsCallbackStruct cbs;


    cbs.doit = True;
    cbs.old_tab = old_tab = tabw->tabs.curtab;
    
    /*
     * Was a tab selected with the Space bar
     */
    if ( params[0][0] == 's' )
    {
         cbs.tab = current_tab = tabw->tabs.focus_tab;
         cbs.tab_label = tabw->tabs.labels[cbs.tab];

         if ( tabw->tabs.curtab == tabw->tabs.focus_tab )
             cbs.doit = False;

         XtCallCallbackList((Widget)tabw, tabw->tabs.activateCallback,
             (XtPointer)&cbs);

        if ( cbs.doit == True )
            SetTabs(tabw, current_tab);

        return;
    }


    /*
     * Make sure this widget has the keyboard focus
     */
    if ( tabw->tabs.i_have_focus == False )
        XmProcessTraversal((Widget)tabw, XmTRAVERSE_CURRENT); 

    /*
     * See which tab was selected, we have 2 methods here, one for horizontal
     * and one for vertical tabs
     */
    if ( IS_HORIZ_TAB(tabw) )
    {
    Dimension ww, w1;

        if ( TAB_SHAPE(tabw) == XgSLANTED )
                slant = (HEIGHT(tabw) - event->xbutton.y) / TAB_SKEW;
        else
            slant = 0;

        if ( tabw->tabs.numtabs > 1 )
        {
        int max_width;
        
            max_width = tabw->tabs.main_tab_width - 
                (HEIGHT(tabw) * 2 / TAB_SKEW *
                ((TAB_SHAPE(tabw) == XgSLANTED) ? 1 : 0));
            width = (WIDTH(tabw) - tabw->tabs.main_tab_width) / 
               (tabw->tabs.numtabs - 1);
            if ( width > max_width )
                width = max_width;
        }
        else
            width = 0;


        w1 = tabw->tabs.curtab * width;

        if ( event->xbutton.x < w1 + slant) 
        {
            /*
             * Click above the main tab 
             */
            if (tabw->tabs.curtab != 0) 
            {
                ww = width + slant;
                for ( i = tabw->tabs.curtab ; event->xbutton.x >= ww; 
                        i--, ww += width);
                cbs.tab = current_tab = tabw->tabs.curtab - i;
                cbs.tab_label = tabw->tabs.labels[cbs.tab];
            }
        } 
        else if ( event->xbutton.x < w1 + tabw->tabs.main_tab_width - slant ) 
        {
            /*
             * Click on main tab 
             */
            cbs.tab = current_tab = tabw->tabs.curtab;
            cbs.tab_label = tabw->tabs.labels[cbs.tab];
            cbs.doit = False;
        } 
        else if ( event->xbutton.x < (tabw->tabs.curtab) * width +
                tabw->tabs.main_tab_width + width * righttabs )
        {
            /*
             * Click below the main tab
             */
            if ( righttabs != 0 )
            {
                ww = w1 + tabw->tabs.main_tab_width + width;
                for ( i = tabw->tabs.curtab + 1; event->xbutton.x > ww; 
                    i++, ww += width );

                cbs.tab = current_tab = i;
                cbs.tab_label = tabw->tabs.labels[cbs.tab];
            }
        }
    }
    else
    {
    Dimension hh, h1;

        if ( TAB_SHAPE(tabw) == XgSLANTED )
                slant = (WIDTH(tabw) - event->xbutton.x) / TAB_SKEW;
        else
            slant = 0;

        if ( tabw->tabs.numtabs > 1 )
        {
        int max_height;
        
            max_height = tabw->tabs.main_tab_width - 
                (WIDTH(tabw) * 2 / TAB_SKEW *
                ((TAB_SHAPE(tabw) == XgSLANTED) ? 1 : 0));
            height= (HEIGHT(tabw) - tabw->tabs.main_tab_width) / 
               (tabw->tabs.numtabs - 1);
            if ( height > max_height )
                height = max_height;
        }
        else
            height = 0;

    
        h1 = tabw->tabs.curtab * height;

        if ( event->xbutton.y < h1 + slant) 
        {
            /*
             * Click left of main tab 
             */
            if (tabw->tabs.curtab != 0) 
            {
                hh = height + slant;
                for ( i = tabw->tabs.curtab ; event->xbutton.y >= hh; 
                        i--, hh += height );
                cbs.tab = current_tab = tabw->tabs.curtab - i;
                cbs.tab_label = tabw->tabs.labels[cbs.tab];
            }
        } 
        else if ( event->xbutton.y < h1 + tabw->tabs.main_tab_width - slant ) 
        {
            /*
             * Click on main tab 
             */
            cbs.tab = current_tab = tabw->tabs.curtab;
            cbs.tab_label = tabw->tabs.labels[cbs.tab];
            cbs.doit = False;
        } 
        else if ( event->xbutton.y < (tabw->tabs.curtab) * height +
                tabw->tabs.main_tab_width + height * righttabs )
        {
            /*
             * Click right of main tab
             */
            if ( righttabs != 0 )
            {
                hh = h1 + tabw->tabs.main_tab_width + height;
                for ( i = tabw->tabs.curtab + 1; event->xbutton.y > hh; 
                    i++, hh += height );

                cbs.tab = current_tab = i;
                cbs.tab_label = tabw->tabs.labels[cbs.tab];
            }
        }
    }

    /*
     * Set the focus tab, draw the tabs with the selected one on top and
     * then call the callbacks
     */
    if ( current_tab != -1 )
    {
        if ( tabw->tabs.focus_tab != current_tab )
        {
            UnHighlight(tabw);

            tabw->tabs.focus_tab = current_tab;

	    if ( cbs.doit == False )
	    	Highlight(tabw);
        }

	if ( cbs.doit == True )
        	SetTabs(tabw, current_tab);

	XtCallCallbackList((Widget)tabw, tabw->tabs.activateCallback,
		(XtPointer)&cbs);

        /*
         * The client did not want this new tab, so redraw the old one
         */
        if ( cbs.doit == False && (old_tab != current_tab) )
            SetTabs(tabw, old_tab);
    }
}


static void
SetTabs(tabw, current_tab)
XgTabsWidget  tabw;
int           current_tab;
{
int        i;
int        old_curtab = tabw->tabs.curtab;


    if ( current_tab == tabw->tabs.curtab )
        return;

    tabw->tabs.curtab = current_tab;

    /* 
     * Only draw the tabs that have been exposed and the one
     * on each side of the new top tab
     */
    if ( old_curtab < current_tab )
        for ( i = old_curtab; i < current_tab; i++)
            DrawLeftTab(tabw, NULL, i);
    else
       for ( i = old_curtab; i > current_tab; i-- )
            DrawRightTab(tabw, NULL, i);

    /*
     * Draw the new current tab
     */
    DrawCurrentTab(tabw, NULL);


    /*
     * Manage and raise the associated widget if there is one
     */
    if ( tabw->tabs.tabwidgets != NULL )
    {
    Widget w = tabw->tabs.tabwidgets[current_tab];

        if ( w != NULL )
        {
            Window win = XtWindow(w);

            if ( win != None )
            {
                /*
                 * Manage the widget if it's not
                 */
                if ( !XtIsManaged(w) )
                    XtManageChild(w);

                /*
                 * Raise its window
                 */
                XRaiseWindow(XtDisplay(w), win);
            }
        }


        /*
         * If auto unmanage is set then unmanage the last current widget
         */
        if ( tabw->tabs.auto_unmanage == True && old_curtab >= 0 && 
            old_curtab < tabw->tabs.numtabs )
        {
            w = tabw->tabs.tabwidgets[old_curtab];
            
            if ( w != NULL )
                XtUnmanageChild(w);
        }
    }
}


static void
GetValuesHook(w, args, num_args)
XgTabsWidget	w;
ArgList 	args;
Cardinal 	*num_args;
{
int 	i;
Arg	xtarg;

	/*
	 *
	 */
	for (i = 0; i < *num_args; i++)
	{
		if ( strcmp(args[i].name, XgNtabLabels) == 0 )
		{
			XtSetArg(args[i], XgNtabLabels, w->tabs.labels);
		}
	}
}


static Boolean  
SetValues(old, request, new)
XgTabsWidget  old;
XgTabsWidget  request;
XgTabsWidget  new;
{
Boolean redraw = False;

#define NOT_EQUAL(field)       (old->field != new->field)

    if ( NOT_EQUAL(core.background_pixel) ||
            NOT_EQUAL(core.background_pixmap) ||
        NOT_EQUAL(primitive.shadow_thickness) )
    {
        CreateTopGc(new);
        CreateBottomGc(new);
        redraw = True;
    }


    if ( NOT_EQUAL(primitive.highlight_color) )
    {
        redraw = True;
    }


    if ( NOT_EQUAL(primitive.foreground) || NOT_EQUAL(tabs.font_list) ||
        NOT_EQUAL(primitive.highlight_thickness) )
    {
        CreateTextGc(new);
        redraw = True;
    }

    if ( NOT_EQUAL(tabs.textmargin) )
        redraw = True;

    if ( NOT_EQUAL(tabs.tabcolor) )
    {
        CreateFillGc(new);
        redraw = True;
    }

    /*
     * if the labels have changed then get our local copy
     */
    if ( NOT_EQUAL(tabs.tablabels) )
    {
    int       i;
    String    *strings;

        if ( old->tabs.labels != NULL ) 
        {
            for ( i = 0; old->tabs.labels[i] != NULL; i++ )
                    XtFree(old->tabs.labels[i]);

            XtFree((XtPointer)old->tabs.labels);
        }


        if ( new->tabs.tablabels != NULL ) 
        {
        Boolean last_one = False;

            strings = (String*) XtMalloc((new->tabs.numtabs + 1) * 
                    sizeof(String));

            for ( i = 0; i < new->tabs.numtabs; i++ ) 
            {
                if ( !last_one )
                    if  ( new->tabs.tablabels[i] == NULL )
                        last_one = True;

                if ( last_one )
                    strings[i] = NULL;
                else
                    strings[i] = XtNewString(new->tabs.tablabels[i]);
            }

            strings[new->tabs.numtabs] = NULL;
            new->tabs.labels = strings;
	    new->tabs.tablabels = NULL;
        }
        else
             new->tabs.labels = NULL;
                
        redraw = True;
    }
    
    if ( NOT_EQUAL(tabs.tab_shape) ||  NOT_EQUAL(tabs.tab_location) )
            redraw = True;


    if ( NOT_EQUAL(tabs.numtabs) || NOT_EQUAL(tabs.curtab) ||
            NOT_EQUAL(tabs.focus_tab) )
            redraw = True;

    /*
     * Make sure the focus and current tab are within range
     */
    if (  new->tabs.focus_tab >= new->tabs.numtabs )
    {
        XtWarning( "Focus Tab not valid, forcing to 0");
        new->tabs.focus_tab = 0;
    }

    if (  new->tabs.curtab >= new->tabs.numtabs )
    {
        XtWarning( "Current Tab not valid, forcing to 0");
        new->tabs.curtab = 0;
    }


    /*
     * If we need to redraw recompute the current tab width, and get the
     * array of label pixmaps
     */
    if ( redraw )
    {
        SetMainTabWidth(new);

        FreePixmapArray(old);
        new->tabs.label_pixmaps = NULL;
        CreatePixmapArray(new);
    }


    return redraw;

#undef NOT_EQUAL
}



static void 
Resize(tabw)
XgTabsWidget tabw;
{
    SetMainTabWidth(tabw);
}




static void 
Redraw(tabw, event, region)
XgTabsWidget tabw;
XEvent * event;
Region  region;
{
int i;

    if ( !XtIsRealized((Widget)tabw) ) 
        return;

    for ( i = 0; i < tabw->tabs.curtab; i++ )
        DrawLeftTab(tabw, region, i);

    for ( i = tabw->tabs.numtabs - 1; i > tabw->tabs.curtab; i-- )
        DrawRightTab(tabw, region, i);

    DrawCurrentTab(tabw, region);
}





static void 
Highlight(tabw)
XgTabsWidget tabw;
{
int     focus_tab = tabw->tabs.focus_tab;


    /*
     * Draw the focus tab based on shape and location, we
     * use the same routine that we use to draw the tabs and
     * it's label except we set the highlight flag (the last i
     * parameter), this auses only the highlight to be drawn
     */
    if ( TAB_SHAPE(tabw) == XgSLANTED )
    {
        if ( IS_HORIZ_TAB(tabw) )
        {
            if ( focus_tab < tabw->tabs.curtab )
                HorizLeftSlant(tabw, NULL, focus_tab, True);
            else if ( focus_tab > tabw->tabs.curtab )
                HorizRightSlant(tabw, NULL, focus_tab, True);
            else
                HorizCurrentSlant(tabw, NULL, True);
        }
        else
        {
            if ( focus_tab < tabw->tabs.curtab )
                VertTopSlant(tabw, NULL, focus_tab, True);
            else if ( focus_tab > tabw->tabs.curtab )
                VertBottomSlant(tabw, NULL, focus_tab, True);
            else
                VertCurrentSlant(tabw, NULL, True);
        }
    }
    else
    {
        if ( IS_HORIZ_TAB(tabw) )
        {
            if ( focus_tab < tabw->tabs.curtab )
                HorizLeftChamferred(tabw, NULL, focus_tab, True);
            else if ( focus_tab > tabw->tabs.curtab )
                HorizRightChamferred(tabw, NULL, focus_tab, True);
            else
                HorizCurrentChamferred(tabw, NULL, True);
        }
        else
        {
            if ( focus_tab < tabw->tabs.curtab )
                VertTopChamferred(tabw, NULL, focus_tab, True);
            else if ( focus_tab > tabw->tabs.curtab )
                VertBottomChamferred(tabw, NULL, focus_tab, True);
            else
                VertCurrentChamferred(tabw, NULL, True);
        }
    }
}


static void 
UnHighlight(tabw)
XgTabsWidget tabw;
{
int     old_focus_tab;


    /*
     * Save the index of the focus tab and then set it to -1. This
     * will cause the highlight to be cleared.
     */
    old_focus_tab = tabw->tabs.focus_tab;
    tabw->tabs.focus_tab = -1;

    /*
     * Clear the highlight on the focus tab. We do this based on 
     * shape and location, we use the same routine that we use to 
     * draw the tabs and it's label except we set the highlight flag 
     * (the last parameter), this auses only the highlight to be drawn
     */
    if ( TAB_SHAPE(tabw) == XgSLANTED )
    {
         if ( IS_HORIZ_TAB(tabw) )
        {
            if ( old_focus_tab < tabw->tabs.curtab )
                HorizLeftSlant(tabw, NULL, old_focus_tab, True);
            else if ( old_focus_tab > tabw->tabs.curtab )
                HorizRightSlant(tabw, NULL, old_focus_tab, True);
            else
                HorizCurrentSlant(tabw, NULL, True);
        }
        else
        {
            if ( old_focus_tab < tabw->tabs.curtab )
                VertTopSlant(tabw, NULL, old_focus_tab, True);
            else if ( old_focus_tab > tabw->tabs.curtab )
                VertBottomSlant(tabw, NULL, old_focus_tab, True);
            else
                VertCurrentSlant(tabw, NULL, True);
        }
    }
    else
    {
        if ( IS_HORIZ_TAB(tabw) )
        {
            if ( old_focus_tab < tabw->tabs.curtab )
                HorizLeftChamferred(tabw, NULL, old_focus_tab, True);
            else if ( old_focus_tab > tabw->tabs.curtab )
                HorizRightChamferred(tabw, NULL, old_focus_tab, True);
            else
                HorizCurrentChamferred(tabw, NULL, True);
        }
        else
        {
            if ( old_focus_tab < tabw->tabs.curtab )
                VertTopChamferred(tabw, NULL, old_focus_tab, True);
            else if ( old_focus_tab > tabw->tabs.curtab )
                VertBottomChamferred(tabw, NULL, old_focus_tab, True);
            else
                VertCurrentChamferred(tabw, NULL, True);
        }
    }

    /*
     * restore the focus tab index
     */
    tabw->tabs.focus_tab = old_focus_tab;
}


static void
GetPreferredSize(tabw, width, height)
XgTabsWidget tabw;
int *width;
int *height;
{
int pref_height;
int pref_width;

    pref_height = (tabw->primitive.shadow_thickness +
        tabw->tabs.textmargin + HIGHLIGHT_THICKNESS(tabw)) * 2 +
        tabw->tabs.font->ascent + tabw->tabs.font->descent;

    if ( width != NULL )
    {
    int main_tab_width = GetMainTabWidth(tabw, pref_height);

        if ( TAB_SHAPE(tabw) == XgSLANTED )
            pref_width = main_tab_width + (tabw->tabs.numtabs - 1) * 
                (main_tab_width - pref_height * 2 / TAB_SKEW);
        else
            pref_width = main_tab_width + (tabw->tabs.numtabs - 1) *
                main_tab_width;
    }

    if ( height != NULL )
    {
        if ( tabw->tabs.tab_location == XgTOP || 
            tabw->tabs.tab_location == XgBOTTOM )
                *height = pref_height;
        else
                 *height = pref_width;
    }

    if ( width != NULL )
    {
        if ( tabw->tabs.tab_location == XgTOP || 
            tabw->tabs.tab_location == XgBOTTOM )
                *width = pref_width;
        else
                *width = pref_height;        
    }
}


static void
SetMainTabWidth(tabw)
XgTabsWidget tabw;
{
int    main_tab_width = GetMainTabWidth(tabw, 0);
int    other_tabs;
int    height = (IS_HORIZ_TAB(tabw) ? HEIGHT(tabw) : WIDTH(tabw));

    if ( tabw->tabs.numtabs > 1 )
    {
        if ( TAB_SHAPE(tabw) == XgSLANTED )
            other_tabs = height / TAB_SKEW + 
                (tabw->tabs.numtabs - 1) * 
                tabw->primitive.shadow_thickness; 
        else
            other_tabs = (tabw->tabs.numtabs - 1) *
                tabw->primitive.shadow_thickness;
    }
    else
        other_tabs = 0;

    if ( IS_HORIZ_TAB(tabw) )
    {
        if ( main_tab_width > WIDTH(tabw) - other_tabs )
            main_tab_width = WIDTH(tabw) - other_tabs;
    }
    else if ( main_tab_width > HEIGHT(tabw) - other_tabs )
        main_tab_width = HEIGHT(tabw) - other_tabs;

    tabw->tabs.main_tab_width = (main_tab_width > 0) ? main_tab_width : 0;
}



static int
GetMainTabWidth(tabw, passed_height)
XgTabsWidget tabw;
int passed_height;
{
int    new_width = 0;
int    height;

    if ( passed_height > 0 )
        height = passed_height;
    else
        height = (IS_HORIZ_TAB(tabw) ? HEIGHT(tabw) : WIDTH(tabw));

    /*
     * Get the width of the longest tab label
     */
    if ( tabw->tabs.labels != NULL ) 
    {
    int i, tw;

        for ( i = 0; i < tabw->tabs.numtabs; i++ )
        {
        char *string = tabw->tabs.labels[i];

            if ( string == NULL )
                tw = 0;
            else
                    tw = XTextWidth(tabw->tabs.font, string, strlen(string));

            if ( tw > new_width )
                    new_width = tw;
        }
    }

    /*
     * Add the margin, shadow thickness, highlight thickness and tab skew
     * the the max text length to compute the width of the biggest tab
     */
    new_width += (tabw->tabs.textmargin + tabw->primitive.shadow_thickness + 
            HIGHLIGHT_THICKNESS(tabw)) * 2 +
            (height * 2 / TAB_SKEW) * 
            ((TAB_SHAPE(tabw) == XgSLANTED) ? 1 : 0);

    return new_width;
}


static void
DrawLeftTab(tab, region, index)
XgTabsWidget tab;
Region  region;
int  index;
{
    /*
     * Draw the left (or top) tab based on shape and location
     */
    if ( TAB_SHAPE(tab) == XgSLANTED )
    {
        if ( IS_HORIZ_TAB(tab) )
            HorizLeftSlant(tab, region, index, False);
        else
            VertTopSlant(tab, region, index, False);
    }
    else
    {
        if ( IS_HORIZ_TAB(tab) )
            HorizLeftChamferred(tab, region, index, False);
        else
            VertTopChamferred(tab, region, index, False);
    }
}


static void
DrawRightTab(tab, region, index)
XgTabsWidget tab;
Region  region;
int  index;
{
    /*
     * Draw the right (or bottom) tab based on shape and location
     */
    if ( TAB_SHAPE(tab) == XgSLANTED )
    {
        if ( IS_HORIZ_TAB(tab) )
            HorizRightSlant(tab, region, index, False);
        else
            VertBottomSlant(tab, region, index, False);
    }
    else
    {
        if ( IS_HORIZ_TAB(tab) )
            HorizRightChamferred(tab, region, index, False);
        else
            VertBottomChamferred(tab, region, index, False);
    }
}


static void
DrawCurrentTab(tab, region)
XgTabsWidget tab;
Region  region;
{
    /*
     * Draw the current tab based on shape and location
     */
    if ( TAB_SHAPE(tab) == XgSLANTED )
    {
        if ( IS_HORIZ_TAB(tab) )
            HorizCurrentSlant(tab, region, False);
        else
            VertCurrentSlant(tab, region, False);
    }
    else
    {
        if ( IS_HORIZ_TAB(tab) )
            HorizCurrentChamferred(tab, region, False);
        else
            VertCurrentChamferred(tab, region, False);
    }
}



static void 
#if NeedFunctionPrototypes
HorizLeftSlant(XgTabsWidget tabw, Region region, int index, Boolean highlight)
#else
HorizLeftSlant(tabw, region, index, highlight)
XgTabsWidget tabw;
Region  region;
int  index;
Boolean highlight;
#endif
{
Region clip;
XPoint tab_shape[5];
Dimension w, slant, shadow, max_width;
Position x;
Display *dpy = XtDisplay((Widget)tabw);
Window win = XtWindow((Widget)tabw);

    shadow = tabw->primitive.shadow_thickness / 2;

    w = (WIDTH(tabw) - tabw->tabs.main_tab_width) / (tabw->tabs.numtabs - 1);

    slant = HEIGHT(tabw) / TAB_SKEW;
    max_width = tabw->tabs.main_tab_width - HEIGHT(tabw) * 2 / TAB_SKEW;

    if ( w >= max_width )
        w = max_width;
           
    x = index * w;

    /*
     * X values are constant for horizontal tabs
     */
    tab_shape[0].x = x + w - 1;
    tab_shape[1].x = x + shadow;                
    tab_shape[2].x = x + shadow + slant;        
    tab_shape[3].x = x + w + shadow + slant;

    /*
     * Base the Y points on location
     */
    if ( TAB_LOCATION(tabw) == XgTOP )
    {
        tab_shape[0].y = tab_shape[1].y = HEIGHT(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].y = tab_shape[3].y = shadow;
    }
    else
    {
        tab_shape[0].y = tab_shape[1].y = shadow;
        tab_shape[2].y = tab_shape[3].y = HEIGHT(tabw) - SHADOW_OFFSET(shadow);
    }

    /*
     * Clip graphics to the tab shape
     */
    clip = XPolygonRegion(tab_shape, 4, EvenOddRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);

    if ( highlight == False )
        XFillPolygon(dpy, win, tabw->tabs.fillgc, 
            tab_shape, 4, Convex, CoordModeOrigin);

    if ( tabw->tabs.labels != NULL && tabw->tabs.labels[index] != NULL ) 
    {
    int len, textw, textx;

        len = strlen(tabw->tabs.labels[index]);
        textw = XTextWidth(tabw->tabs.font, tabw->tabs.labels[index], len);
        if ( w == max_width )
            textx = (x + x + w - textw) / 2 + slant;
        else
            textx = x + shadow + slant + tabw->tabs.textmargin + 
                    HIGHLIGHT_THICKNESS(tabw);

        DrawHorizText(tabw, textx, textw, index, len,
            tabw->primitive.foreground, highlight);
    }

    /*
     * Only draw the tab shape if not highlighting
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgTOP )
        {
                XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
    	            tab_shape + 1, 3, CoordModeOrigin);
        }
        else
        {
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                tab_shape + 1, 2, CoordModeOrigin);
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                tab_shape + 2, 2, CoordModeOrigin);
        }
    }

    /*
     * Destroy the clipping region
     */
    XDestroyRegion(clip);
}



static void 
#if NeedFunctionPrototypes
HorizRightSlant(XgTabsWidget tabw, Region region, int index, Boolean highlight)
#else
HorizRightSlant(tabw, region, index, highlight)
XgTabsWidget tabw;
Region  region;
int  index;
Boolean highlight;
#endif
{
Region clip;
XPoint tab_shape[4];
Dimension w, slant, shadow, max_width;
Position x;
Display *dpy = XtDisplay((Widget)tabw);
Window win = XtWindow((Widget)tabw);
    
    shadow = tabw->primitive.shadow_thickness / 2;

    slant = HEIGHT(tabw) / TAB_SKEW;
    max_width = tabw->tabs.main_tab_width - HEIGHT(tabw) * 2 / TAB_SKEW;

    w = (WIDTH(tabw) - tabw->tabs.main_tab_width) / 
           (tabw->tabs.numtabs - 1);
    if ( w > max_width )
        w = max_width;
    x = index * w + tabw->tabs.main_tab_width;

    tab_shape[0].x = x - w;                        
    tab_shape[1].x = x - SHADOW_OFFSET(shadow);
    tab_shape[2].x = x - SHADOW_OFFSET(shadow) - slant;
    tab_shape[3].x = x - w - slant;

    if ( TAB_LOCATION(tabw) == XgTOP )
    {
        tab_shape[0].y = tab_shape[1].y = HEIGHT(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].y = tab_shape[3].y = shadow;
    }
    else
    {
        tab_shape[0].y = tab_shape[1].y = shadow;
        tab_shape[2].y = tab_shape[3].y = HEIGHT(tabw) - SHADOW_OFFSET(shadow);
    }

    clip = XPolygonRegion(tab_shape, 4, EvenOddRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);

    /*
     * Fill the tab background
     */
    if ( highlight == False )
        XFillPolygon(dpy, win, 
                tabw->tabs.fillgc, tab_shape, 4, Convex, CoordModeOrigin);
    
    /*
     * Draw the tab text and possibly the highlight
     */
    if ( tabw->tabs.labels && tabw->tabs.labels[index] != NULL )
    {
    int        len, textw, textx;

        len = strlen(tabw->tabs.labels[index]);
        textw = XTextWidth(tabw->tabs.font, tabw->tabs.labels[index], len);
        if ( w == max_width )
	    textx = (x + x - w - textw) / 2 - slant;
        else
            textx = x - textw - slant - tabw->tabs.textmargin -
                        HIGHLIGHT_THICKNESS(tabw);

        DrawHorizText(tabw, textx, textw, index, len,
            tabw->primitive.foreground, highlight);
    }


    /*
     * Only draw the tab shape if not highlighting
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgTOP )
        {
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw), 
                    tab_shape + 1, 2, CoordModeOrigin);
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw), 
                    tab_shape + 2, 2, CoordModeOrigin);
        }
        else
        {
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw), 
                    tab_shape + 1, 3, CoordModeOrigin);
        }
    }

    /*
     * Destroy the clipping region
     */
    XDestroyRegion(clip);

}


static void 
#if NeedFunctionPrototypes
HorizCurrentSlant(XgTabsWidget tabw, Region region, Boolean highlight)
#else
HorizCurrentSlant(tabw, region, highlight)
XgTabsWidget tabw;
Region  region;
Boolean highlight;
#endif
{
Dimension w, max_width, slant, shadow;
Position  x1, x2;
Display   *dpy = XtDisplay(tabw);
Region    clip;
XPoint    tab_shape[10];
Window    win = XtWindow((Widget)tabw);
int       index = tabw->tabs.curtab;


    max_width = tabw->tabs.main_tab_width - HEIGHT(tabw) * 2 / TAB_SKEW;
    slant = HEIGHT(tabw) / TAB_SKEW;

    if ( tabw->tabs.numtabs > 1 )
        w = (WIDTH(tabw) - tabw->tabs.main_tab_width) /
           (tabw->tabs.numtabs - 1);
    else
        w = 0;

    w = ( w > max_width ) ? (max_width < 1 ? 1 : max_width) : w;

    x1 = tabw->tabs.curtab * w;
    x2 = x1 + tabw->tabs.main_tab_width;
    shadow = tabw->primitive.shadow_thickness / 2;

    tab_shape[0].x = 0;                        
    tab_shape[1].x = tab_shape[2].x = tab_shape[3].x = x1 + shadow;
    tab_shape[4].x = x1 + shadow + slant;        
    tab_shape[5].x = x2 - SHADOW_OFFSET(shadow) - slant;        
    tab_shape[6].x = tab_shape[7].x = tab_shape[8].x = 
            x2 - SHADOW_OFFSET(shadow);
    tab_shape[9].x = WIDTH(tabw);                        

    if ( TAB_LOCATION(tabw) == XgTOP )
    {
        tab_shape[0].y = tab_shape[1].y = tab_shape[3].y = tab_shape[6].y =
            tab_shape[8].y = tab_shape[9].y =
                HEIGHT(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].y = tab_shape[7].y = HEIGHT(tabw);
        tab_shape[4].y = tab_shape[5].y = shadow;
    }
    else
    {
        tab_shape[0].y = tab_shape[1].y = tab_shape[3].y = 
            tab_shape[6].y = tab_shape[8].y = tab_shape[9].y = shadow;
        tab_shape[2].y = tab_shape[7].y = 0;
        tab_shape[4].y = tab_shape[5].y = HEIGHT(tabw) - SHADOW_OFFSET(shadow);
    }

    clip = XPolygonRegion(tab_shape, XtNumber(tab_shape), EvenOddRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);


    /*
     * Fill the tab background
     */
    if ( highlight == False )
    {
        if ( tabw->tabs.tabcolor != tabw->tabs.selected_color )
            XSetForeground(dpy, tabw->tabs.fillgc, tabw->tabs.selected_color);

        XFillPolygon(dpy, win, 
            tabw->tabs.fillgc, tab_shape + 2, 6, Convex, CoordModeOrigin);

        if ( tabw->tabs.tabcolor != tabw->tabs.selected_color )
            XSetForeground(dpy, tabw->tabs.fillgc, tabw->tabs.tabcolor);
    }



    /*
     * Draw the tab text and possibly the highlight
     */
    if ( tabw->tabs.labels && tabw->tabs.labels[index] != NULL )
    {
    int  len, x, textw;

        len = strlen(tabw->tabs.labels[index]);
        textw = XTextWidth(tabw->tabs.font, tabw->tabs.labels[index], len);
        x = (x1 + x2 - textw) / 2;
        DrawHorizText(tabw, x, textw, index, len,
            tabw->primitive.foreground, highlight);
    }
    

    /*
     * Only draw the tab shape if not highlighting
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgTOP )
        {
            XDrawLines(dpy, XtWindow(tabw), TOP_SHADOW_GC(tabw), 
                tab_shape, 6, CoordModeOrigin);
            XDrawLines(dpy, XtWindow(tabw), BOTTOM_SHADOW_GC(tabw),
                tab_shape + 5, 4, CoordModeOrigin);
            XDrawLines(dpy, XtWindow(tabw), TOP_SHADOW_GC(tabw),
                tab_shape + 8, 2, CoordModeOrigin);
        }
        else
        {
            XDrawLines(dpy, XtWindow(tabw), BOTTOM_SHADOW_GC(tabw),
                tab_shape, 3, CoordModeOrigin);
            XDrawLines(dpy, XtWindow(tabw), TOP_SHADOW_GC(tabw),
                tab_shape + 3, 2, CoordModeOrigin);
            XDrawLines(dpy, XtWindow(tabw), BOTTOM_SHADOW_GC(tabw),
                tab_shape + 4, 6, CoordModeOrigin);
        }
    }


    /*
     * Destroy the clipping region
     */
    XDestroyRegion(clip);
}


static void 
#if NeedFunctionPrototypes
HorizLeftChamferred(XgTabsWidget tabw, Region region, 
	int index, Boolean highlight)
#else
HorizLeftChamferred(tabw, region, index, highlight)
XgTabsWidget tabw;
Region  region;
int  index;
Boolean highlight;
#endif
{
Region    clip;
XPoint    tab_shape[7];
Dimension w, shadow, max_width;
Position  x;
Display   *dpy = XtDisplay((Widget)tabw);
Window    win = XtWindow((Widget)tabw);

    shadow = tabw->primitive.shadow_thickness / 2;
    w = (WIDTH(tabw) - tabw->tabs.main_tab_width) / (tabw->tabs.numtabs - 1);

    max_width = tabw->tabs.main_tab_width;
    if ( w >= max_width )
        w = max_width;
    
    x = index * w;

    tab_shape[0].x = x + w - SHADOW_OFFSET(shadow);                        
    tab_shape[1].x = tab_shape[2].x = x + shadow;        
    tab_shape[3].x = x + shadow + SLANT;        
    
    if ( w != max_width )
    {
        tab_shape[4].x = x + w + SLANT;
        tab_shape[5].x = tab_shape[6].x = tab_shape[0].x;
    }
    else
    {
        tab_shape[4].x = x + w - SLANT - SHADOW_OFFSET(shadow);
        tab_shape[5].x = x + w - SHADOW_OFFSET(shadow);
        tab_shape[6].x = tab_shape[0].x;
    }


    /*
     * Get Y positions based on location
     */
    if ( TAB_LOCATION(tabw) == XgTOP )
    {
        tab_shape[0].y = tab_shape[1].y = HEIGHT(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].y = tab_shape[5].y = shadow + SLANT;
        tab_shape[3].y = tab_shape[4].y = shadow;
            tab_shape[6].y = tab_shape[0].y;
    }
    else
    {
        tab_shape[0].y = tab_shape[1].y = shadow;
        tab_shape[2].y = tab_shape[5].y = 
            HEIGHT(tabw) - SHADOW_OFFSET(shadow) - SLANT;
        tab_shape[3].y = tab_shape[4].y = HEIGHT(tabw) - SHADOW_OFFSET(shadow);
            tab_shape[6].y = tab_shape[0].y;
    }

    /*
     * Set up the clip region for the tab text
     */
    clip = XPolygonRegion(tab_shape, XtNumber(tab_shape), WindingRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);

    /*
     * Fill the tab background
     */
    if ( highlight == False )
        XFillPolygon(dpy, win, tabw->tabs.fillgc, 
            tab_shape, XtNumber(tab_shape), Convex, CoordModeOrigin);

    /*
     * Draw the tab text
     */
    if ( tabw->tabs.labels != NULL && tabw->tabs.labels[index] != NULL ) 
    {
    int len, textw, textx;

        len = strlen(tabw->tabs.labels[index]);
        textw = XTextWidth(tabw->tabs.font, tabw->tabs.labels[index], len);
        if ( w == max_width )
            textx = (x + x + w - textw) / 2 + 1;
        else
            textx = x + tabw->primitive.shadow_thickness + 
                HIGHLIGHT_THICKNESS(tabw) + tabw->tabs.textmargin;

        DrawHorizText(tabw, textx, textw, index, len,
            tabw->primitive.foreground, highlight);
    }

    /*
     * Only draw the tab shape if not highlighting
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgTOP )
        {
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                tab_shape + 1, 4, CoordModeOrigin);            

            if ( w == max_width )
                XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                    tab_shape + 4, 3, CoordModeOrigin);
        }
        else
        {
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                tab_shape + 1, 3, CoordModeOrigin);            

            if ( w == max_width )
                XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                    tab_shape + 3, 4, CoordModeOrigin);
            else
                XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                    tab_shape + 3, 2, CoordModeOrigin);
        }
    }

    /*
     * Destroy the clipping region
     */
    XDestroyRegion(clip);
}



static void 
#if NeedFunctionPrototypes
HorizRightChamferred(XgTabsWidget tabw, Region region, 
	int index, Boolean highlight)
#else
HorizRightChamferred(tabw, region, index, highlight)
XgTabsWidget tabw;
Region  region;
int  index;
Boolean highlight;
#endif
{
Region    clip;
XPoint    tab_shape[7];
Dimension w, shadow, max_width;
Position  x;
Display   *dpy = XtDisplay((Widget)tabw);
Window    win = XtWindow((Widget)tabw);


    /*
     * Compute the size and location constants for the tab
     */
    shadow = tabw->primitive.shadow_thickness / 2;

    max_width = tabw->tabs.main_tab_width;
    w = (WIDTH(tabw) - tabw->tabs.main_tab_width) / (tabw->tabs.numtabs - 1);
    if ( w > max_width )
        w = max_width;

    x = index * w + tabw->tabs.main_tab_width;

    if ( w != max_width )
    {
        tab_shape[0].x = tab_shape[5].x = 
              tab_shape[6].x = x - SHADOW_OFFSET(shadow);
        tab_shape[1].x = tab_shape[2].x = x - w;
        tab_shape[3].x = x - w - SLANT;
        tab_shape[4].x = x - SLANT - SHADOW_OFFSET(shadow);

    }
    else
    {
        tab_shape[0].x = tab_shape[5].x = 
             tab_shape[6].x = x - SHADOW_OFFSET(shadow);
        tab_shape[1].x = tab_shape[2].x = x - w + shadow;
        tab_shape[3].x = x - w + shadow + SLANT;
        tab_shape[4].x = x - SLANT - SHADOW_OFFSET(shadow);
    }


    /*
     * Get Y positions based on location
     */
    if ( TAB_LOCATION(tabw) == XgTOP )
    {
        tab_shape[0].y = tab_shape[1].y = 
            tab_shape[6].y = HEIGHT(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].y = tab_shape[5].y = shadow + SLANT;
        tab_shape[3].y = tab_shape[4].y = shadow;
    }
    else
    {
        tab_shape[0].y = tab_shape[1].y = tab_shape[6].y = shadow;
        tab_shape[2].y = tab_shape[5].y = 
                HEIGHT(tabw) - SHADOW_OFFSET(shadow) - SLANT;
        tab_shape[3].y = tab_shape[4].y = 
                HEIGHT(tabw) - SHADOW_OFFSET(shadow);
    }


    /*
     * Set up the clip region for the tab text
     */
    clip = XPolygonRegion(tab_shape, XtNumber(tab_shape), EvenOddRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);

    /*
     * Fill the tab background
     */
    if ( highlight == False )
        XFillPolygon(dpy, win, tabw->tabs.fillgc, 
            tab_shape, XtNumber(tab_shape), Convex, CoordModeOrigin);

    /*
     * Draw the tab text
     */
    if ( tabw->tabs.labels && tabw->tabs.labels[index] != NULL )
    {
    int         len, textw, textx;
    
        len = strlen(tabw->tabs.labels[index]);
        textw = XTextWidth(tabw->tabs.font, tabw->tabs.labels[index], len);
        if ( w == max_width )
            textx = x - (w + textw) / 2 + 1;
        else
            textx = x - tabw->tabs.textmargin - HIGHLIGHT_THICKNESS(tabw) - 
                tabw->primitive.shadow_thickness - textw;

        DrawHorizText(tabw, textx, textw, index, len,
            tabw->primitive.foreground, highlight);
    }

    /*
     * Only draw the tab shape if not highlighting
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgTOP )
        {
            if ( w == max_width )
                XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                    tab_shape + 1, 4, CoordModeOrigin);
            else
                XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                        tab_shape + 3, 2, CoordModeOrigin);

            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                tab_shape + 4, 3, CoordModeOrigin);            

        }
        else
        {
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                tab_shape + 3, 4, CoordModeOrigin);            

            if ( w == max_width )
                XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                    tab_shape + 1, 3, CoordModeOrigin);
        }
    }


    /*
     * Destroy the clipping region
     */
    XDestroyRegion(clip);
}



static void 
#if NeedFunctionPrototypes
HorizCurrentChamferred(XgTabsWidget tabw, Region region, Boolean highlight)
#else
HorizCurrentChamferred(tabw, region, highlight)
XgTabsWidget tabw;
Region  region;
Boolean highlight;
#endif
{
Dimension w, shadow, max_width;
Position  x1, x2;
Display   *dpy = XtDisplay(tabw);
Region    clip;
XPoint    tab_shape[10];
Window    win = XtWindow(tabw);
int       index = tabw->tabs.curtab;


    /*
     * Compute the size and location constants for the tab
     */
    max_width = tabw->tabs.main_tab_width;

    if ( tabw->tabs.numtabs > 1 )
        w = (WIDTH(tabw) - tabw->tabs.main_tab_width) /
           (tabw->tabs.numtabs - 1);
    else
        w = 0;

    w = ( w > max_width ) ? (max_width < 1 ? 1 : max_width) : w;

    x1 = tabw->tabs.curtab * w;
    x2 = x1 + tabw->tabs.main_tab_width;
    shadow = tabw->primitive.shadow_thickness / 2;


    /*
     * Horizontal tabs have same X location, whether we are 
     * on the top or bottom
     */
    tab_shape[0].x = 0;                        
    tab_shape[1].x = tab_shape[2].x = tab_shape[3].x = x1 + shadow;        
    tab_shape[4].x = x1 + shadow + SLANT;
    tab_shape[5].x = x2 - SHADOW_OFFSET(shadow) - SLANT;                
    tab_shape[6].x = tab_shape[7].x = tab_shape[8].x = 
            x2 - SHADOW_OFFSET(shadow);
    tab_shape[9].x = WIDTH(tabw);                        


    /*
     * Get Y positions based on location
     */
    if ( TAB_LOCATION(tabw) == XgTOP )
    {
        tab_shape[0].y = tab_shape[1].y = tab_shape[8].y = tab_shape[9].y =
            HEIGHT(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].y = tab_shape[7].y = HEIGHT(tabw);
        tab_shape[3].y = tab_shape[6].y = shadow + SLANT;
        tab_shape[4].y = tab_shape[5].y = shadow;
    }
    else
    {
        tab_shape[0].y = tab_shape[1].y = 
            tab_shape[8].y = tab_shape[9].y = shadow;
        tab_shape[2].y = tab_shape[7].y = 0;
        tab_shape[3].y = tab_shape[6].y = 
            HEIGHT(tabw) - SHADOW_OFFSET(shadow) - SLANT;
        tab_shape[4].y = tab_shape[5].y = HEIGHT(tabw) - SHADOW_OFFSET(shadow);
    }

    /*
     * Set up the clip region for the tab text
     */
    clip = XPolygonRegion(tab_shape, XtNumber(tab_shape), EvenOddRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);


    /*
     * Fill the tab background
     */
    if ( highlight == False )
    {
        if ( tabw->tabs.tabcolor != tabw->tabs.selected_color )
            XSetForeground(dpy, tabw->tabs.fillgc, tabw->tabs.selected_color);

        XFillPolygon(dpy, win, tabw->tabs.fillgc, 
            tab_shape + 2, 6, Convex, CoordModeOrigin);

        if ( tabw->tabs.tabcolor != tabw->tabs.selected_color )
            XSetForeground(dpy, tabw->tabs.fillgc, tabw->tabs.tabcolor);
    }


    /*
     * Draw the tab text
     */
    if ( tabw->tabs.labels && tabw->tabs.labels[index] != NULL )
    {
    int        len, textw, textx;

        len = strlen(tabw->tabs.labels[index]);
        textw = XTextWidth(tabw->tabs.font, tabw->tabs.labels[index], len);
        textx = (x1 + x2 - textw) / 2 + 1;

        DrawHorizText(tabw, textx, textw, index, len,
            tabw->primitive.foreground, highlight);
    }
    

    /*
     * Only draw the tab shape if not highlighting
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgTOP )
        {
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw), 
                tab_shape, 6, CoordModeOrigin);
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw), 
                tab_shape + 5, 4, CoordModeOrigin);
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw), 
                tab_shape + 8, 2, CoordModeOrigin);
        }
        else
        {
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw), 
                tab_shape, 2, CoordModeOrigin);
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw), 
                tab_shape + 2, 3, CoordModeOrigin);
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw), 
                tab_shape + 4, 6, CoordModeOrigin);
        }
    }

    /*
     * Destroy the clipping region
     */
    XDestroyRegion(clip);
}



static void 
#if NeedFunctionPrototypes
VertTopSlant(XgTabsWidget tabw, Region region, int index, Boolean highlight)
#else
VertTopSlant(tabw, region, index, highlight)
XgTabsWidget tabw;
Region  region;
int  index;
Boolean highlight;
#endif
{
Region    clip;
XPoint    tab_shape[4];
Dimension h, slant, shadow, max_height;
Position  y;
Display   *dpy = XtDisplay((Widget)tabw);
Window    win = XtWindow((Widget)tabw);

    shadow = tabw->primitive.shadow_thickness / 2;
    h = (HEIGHT(tabw) - tabw->tabs.main_tab_width) / 
           (tabw->tabs.numtabs - 1);

    slant = WIDTH(tabw) / TAB_SKEW;
    max_height = tabw->tabs.main_tab_width - WIDTH(tabw) * 2 / TAB_SKEW;

    if ( h >= max_height )
        h = max_height;
           
    y = index * h;

    /*
     * Y values are constant for vertical tabs
     */
    tab_shape[0].y = y + h;                        
    tab_shape[1].y = y + shadow;                
    tab_shape[2].y = y + shadow + slant;        
    tab_shape[3].y = y + h + slant - 1;        

    /*
     * Base the X values on the tab location
     */
    if ( TAB_LOCATION(tabw) == XgLEFT )
    {
        tab_shape[0].x = tab_shape[1].x = WIDTH(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].x = tab_shape[3].x = SHADOW_OFFSET(shadow);
    }
    else
    {
        tab_shape[0].x = tab_shape[1].x = SHADOW_OFFSET(shadow);
        tab_shape[2].x = tab_shape[3].x = WIDTH(tabw) - SHADOW_OFFSET(shadow);
    }

    /*
     * Clip graphics to the shape of the tab
     */
    clip = XPolygonRegion(tab_shape, 4, EvenOddRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);

    /*
     * Fill the tab shape if we are not highlighting
     */
    if ( highlight == False )
        XFillPolygon(dpy, win, tabw->tabs.fillgc, tab_shape, 
            4, Convex, CoordModeOrigin);

    /*
     * Now draw the label (and possibly the highlight) on the tab
     */
    if ( tabw->tabs.labels != NULL && tabw->tabs.labels[index] != NULL ) 
    {
    int     len, textw, texty;

        len = strlen(tabw->tabs.labels[index]);
        textw = XTextWidth(tabw->tabs.font, tabw->tabs.labels[index], len);

        if ( h == max_height )
            texty = (y + y + h - textw) / 2 + slant;
        else
            texty = y + shadow + slant + tabw->tabs.textmargin +
                            HIGHLIGHT_THICKNESS(tabw);

        DrawVertText(tabw, texty, textw, index, len, tabw->tabs.tabcolor, 
            tabw->primitive.foreground, clip, highlight);
    }

    /*
     * Only do the tab shape if we are not highlighting the text
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgLEFT )
        {
                XDrawLines(dpy, win, TOP_SHADOW_GC(tabw), 
                tab_shape + 1, 3, CoordModeOrigin);
        }
        else
        {
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                tab_shape + 1, 2, CoordModeOrigin);
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                tab_shape + 2, 2, CoordModeOrigin);
        }
    }

    /*
     * Destroy the clip region
     */
    XDestroyRegion(clip);
}



static void 
#if NeedFunctionPrototypes
VertBottomSlant(XgTabsWidget tabw, Region region, int index, Boolean highlight)
#else
VertBottomSlant(tabw, region, index, highlight)
XgTabsWidget tabw;
Region  region;
int  index;
Boolean highlight;
#endif
{
Region    clip;
XPoint    tab_shape[4];
Dimension h, slant, shadow, max_height;
Position  y;
Display   *dpy = XtDisplay((Widget)tabw);
Window    win   = XtWindow((Widget)tabw);
    
    shadow = tabw->primitive.shadow_thickness / 2;

    max_height = tabw->tabs.main_tab_width - WIDTH(tabw) * 2 / TAB_SKEW;
    slant = WIDTH(tabw) / TAB_SKEW;

    h = (HEIGHT(tabw) - tabw->tabs.main_tab_width) / (tabw->tabs.numtabs - 1);
    if ( h > max_height )
        h = max_height;
    y = index * h + tabw->tabs.main_tab_width;

    tab_shape[0].y = y - h;                        
    tab_shape[1].y = y - SHADOW_OFFSET(shadow);
    tab_shape[2].y = y - SHADOW_OFFSET(shadow) - slant;
    tab_shape[3].y = y - h - slant;

    if ( TAB_LOCATION(tabw) == XgLEFT )
    {
        tab_shape[0].x = tab_shape[1].x = WIDTH(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].x = tab_shape[3].x = SHADOW_OFFSET(shadow);
    }
    else
    {
        tab_shape[0].x = tab_shape[1].x = SHADOW_OFFSET(shadow);
        tab_shape[2].x = tab_shape[3].x = WIDTH(tabw) - SHADOW_OFFSET(shadow);
    }

    clip = XPolygonRegion(tab_shape, 4, EvenOddRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);

    if ( highlight == False )
        XFillPolygon(dpy, win, tabw->tabs.fillgc, 
                tab_shape, 4, Convex, CoordModeOrigin);

    /*
     * Now draw the label (and maybe highlight) on the tab
     */
    if ( tabw->tabs.labels && tabw->tabs.labels[index] != NULL )
    {
    int len, textw, texty;

        len = strlen(tabw->tabs.labels[index]);
        textw = XTextWidth(tabw->tabs.font, tabw->tabs.labels[index], len);

        if ( h == max_height )
            texty = y - (h + textw) / 2 - slant - 
	    	((shadow == 0) ? 1 : 0);
        else
            texty = y - shadow - textw - slant - tabw->tabs.textmargin -
                HIGHLIGHT_THICKNESS(tabw);

        DrawVertText(tabw, texty, textw, index, len, tabw->tabs.tabcolor, 
            tabw->primitive.foreground, clip, highlight);
    }

    /*
     * Only do the tab shape if we are not highlighting the text
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgLEFT )
        {
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw), 
                tab_shape + 1, 2, CoordModeOrigin);
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw), 
                tab_shape + 2, 2, CoordModeOrigin);
        }
        else
        {
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw), 
                tab_shape + 1, 3, CoordModeOrigin);
        }
    }

    /*
     * Destroy the clip region
     */
    XDestroyRegion(clip);

}



static void 
#if NeedFunctionPrototypes
VertCurrentSlant(XgTabsWidget tabw, Region region, Boolean highlight)
#else
VertCurrentSlant(tabw, region, highlight)
XgTabsWidget tabw;
Region  region;
Boolean highlight;
#endif
{
Dimension slant, shadow, max_height, h;
Position  y1, y2;
Display   *dpy = XtDisplay(tabw);
Region    clip;
XPoint    tab_shape[10];
Window    win = XtWindow((Widget)tabw);


    max_height = tabw->tabs.main_tab_width - WIDTH(tabw) * 2 / TAB_SKEW;
    slant = WIDTH(tabw) / TAB_SKEW;

    if ( tabw->tabs.numtabs > 1 )
        h = (HEIGHT(tabw) - tabw->tabs.main_tab_width) /
           (tabw->tabs.numtabs - 1);
    else
        h = 0;

    h = (h > max_height) ? (max_height < 1 ? 1 : max_height) : h;

    y1 = tabw->tabs.curtab * h;
    y2 = y1 + tabw->tabs.main_tab_width;
    shadow = tabw->primitive.shadow_thickness / 2;


    tab_shape[0].y = 0;                        
    tab_shape[1].y = tab_shape[2].y = tab_shape[3].y = y1 + shadow;                
    tab_shape[4].y = y1 + shadow + slant;        
    tab_shape[5].y = y2 - SHADOW_OFFSET(shadow) - slant;        
    tab_shape[6].y = tab_shape[7].y = tab_shape[8].y = 
                            y2 - SHADOW_OFFSET(shadow);                
    tab_shape[9].y = HEIGHT(tabw);                        

    if ( TAB_LOCATION(tabw) == XgLEFT )
    {
        tab_shape[0].x = tab_shape[1].x = tab_shape[8].x = tab_shape[9].x = 
            WIDTH(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].x = tab_shape[7].x = WIDTH(tabw);
        tab_shape[3].x = tab_shape[6].x = WIDTH(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[4].x = tab_shape[5].x = shadow;
    }
    else
    {
        tab_shape[0].x = tab_shape[1].x = tab_shape[3].x = 
            tab_shape[6].x = tab_shape[8].x = tab_shape[9].x = shadow;
        tab_shape[2].x = tab_shape[7].x = 0;
        tab_shape[4].x = tab_shape[5].x = WIDTH(tabw) - SHADOW_OFFSET(shadow);
    }

    clip = XPolygonRegion(tab_shape, XtNumber(tab_shape), EvenOddRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);


    /*
     * Fill the tab background
     */
    if ( highlight == False )
    {
        if ( tabw->tabs.tabcolor != tabw->tabs.selected_color )
            XSetForeground(dpy, tabw->tabs.fillgc, tabw->tabs.selected_color);

        XFillPolygon(dpy, win, tabw->tabs.fillgc, 
            tab_shape + 2, 6, Convex, CoordModeOrigin);

        if ( tabw->tabs.tabcolor != tabw->tabs.selected_color )
            XSetForeground(dpy, tabw->tabs.fillgc, tabw->tabs.tabcolor);
    }


    /*
     * Now draw the label on the tab
     */
    if ( tabw->tabs.labels && 
        tabw->tabs.labels[tabw->tabs.curtab] != NULL )
    {
    int len, textw, texty;

        len = strlen(tabw->tabs.labels[tabw->tabs.curtab]);
        textw = XTextWidth(tabw->tabs.font, 
                tabw->tabs.labels[tabw->tabs.curtab], len);
        texty = (y1 + y2 - textw) / 2;

        DrawVertText(tabw, texty, textw, tabw->tabs.curtab, len,
            tabw->tabs.selected_color, tabw->primitive.foreground, clip, 
            highlight);
    }


    /*
     * Only do the tab shape if we are not highlighting the text
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgLEFT )
        {
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw), 
                tab_shape, 6, CoordModeOrigin);
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                tab_shape + 5, 4, CoordModeOrigin);
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                tab_shape + 8, 2, CoordModeOrigin);
        }
        else
        {
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                tab_shape, 2, CoordModeOrigin);
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                tab_shape + 2, 3, CoordModeOrigin);
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                tab_shape + 4, 6, CoordModeOrigin);
        }
    }

    /*
     * Destroy the clip region
     */
    XDestroyRegion(clip);
}



static void 
#if NeedFunctionPrototypes
VertTopChamferred(XgTabsWidget tabw, Region region, 
	int index, Boolean highlight)
#else
VertTopChamferred(tabw, region, index, highlight)
XgTabsWidget tabw;
Region  region;
int  index;
Boolean highlight;
#endif
{
Region    clip;
XPoint    tab_shape[7];
Dimension h, shadow, max_height;
Position  y;
Display   *dpy = XtDisplay((Widget)tabw);
Window    win   = XtWindow((Widget)tabw);

    shadow = tabw->primitive.shadow_thickness / 2;


    h = (HEIGHT(tabw) - tabw->tabs.main_tab_width) / (tabw->tabs.numtabs - 1);

    max_height = tabw->tabs.main_tab_width;
    if ( h >= max_height )
        h = max_height;
    
    y = index * h;

    tab_shape[1].y = tab_shape[2].y = y + shadow;        
    tab_shape[3].y = y + shadow + SLANT;        
    
    if ( h != max_height )
    {
        tab_shape[0].y = tab_shape[5].y = tab_shape[6].y = y + h;
        tab_shape[4].y = y + h + SLANT;
    }
    else
    {
        tab_shape[0].y = tab_shape[5].y = tab_shape[6].y =
            y + h - SHADOW_OFFSET(shadow);
        tab_shape[4].y = y + h - SLANT - SHADOW_OFFSET(shadow);
    }


    /*
     * Get X positions based on location
     */
    if ( TAB_LOCATION(tabw) == XgLEFT )
    {
        tab_shape[0].x = tab_shape[1].x = WIDTH(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].x = shadow + SLANT;
        tab_shape[3].x = tab_shape[4].x = shadow;
        tab_shape[5].x = shadow + SLANT;
            tab_shape[6].x = tab_shape[0].x;
    }
    else
    {
        tab_shape[0].x = tab_shape[1].x = shadow;
        tab_shape[2].x = WIDTH(tabw) - SHADOW_OFFSET(shadow) - SLANT;
        tab_shape[3].x = tab_shape[4].x = WIDTH(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[5].x = WIDTH(tabw) - SHADOW_OFFSET(shadow) - SLANT;
            tab_shape[6].x = tab_shape[0].x;
    }

    /*
     * Set up the clip region for the tab text
     */
    clip = XPolygonRegion(tab_shape, XtNumber(tab_shape), EvenOddRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);

    /*
     * Fill the tab background
     */
    if ( highlight == False )
        XFillPolygon(dpy, win, tabw->tabs.fillgc, 
                tab_shape, XtNumber(tab_shape), Convex, CoordModeOrigin);

    /*
     * Draw the tab text
     */
    if ( tabw->tabs.labels != NULL && tabw->tabs.labels[index] != NULL ) 
    {
    int len, textw, texty;

        len = strlen(tabw->tabs.labels[index]);
        textw = XTextWidth(tabw->tabs.font, tabw->tabs.labels[index], len);
        if ( h == max_height )
            texty = (y + y + h - textw) / 2;
        else
            texty = y + tabw->primitive.shadow_thickness + 
                        HIGHLIGHT_THICKNESS(tabw) + tabw->tabs.textmargin;

        DrawVertText(tabw, texty, textw, index, len, tabw->tabs.tabcolor, 
            tabw->primitive.foreground, clip, highlight);
    }

    /*
     * Now actually draw the tab
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgLEFT )
        {
            if ( h == max_height )
            {
                XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                    tab_shape + 1, 5, CoordModeOrigin);            

                XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                    tab_shape + 5, 2, CoordModeOrigin);
            }
            else
                XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                    tab_shape + 1, 4, CoordModeOrigin);
        }
        else
        {
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                tab_shape + 1, 2, CoordModeOrigin);            

            if ( h == max_height )
                XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                    tab_shape + 2, 5, CoordModeOrigin);
            else
                XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                    tab_shape + 2, 3, CoordModeOrigin);
        }
    }


    /*
     * Destroy the clipping region
     */
    XDestroyRegion(clip);
}



static void 
#if NeedFunctionPrototypes
VertBottomChamferred(XgTabsWidget tabw, Region region, 
	int index, Boolean highlight)
#else
VertBottomChamferred(tabw, region, index, highlight)
XgTabsWidget tabw;
Region  region;
int  index;
Boolean highlight;
#endif
{
Region    clip;
XPoint    tab_shape[7];
Dimension h, shadow, max_height;
Position  y;
Display   *dpy = XtDisplay((Widget)tabw);
Window    win   = XtWindow((Widget)tabw);


    /*
     * Compute the size and location constants for the tab
     */
    shadow = tabw->primitive.shadow_thickness / 2;

    max_height = tabw->tabs.main_tab_width;

    h = (HEIGHT(tabw) - tabw->tabs.main_tab_width) / 
           (tabw->tabs.numtabs - 1);
    if ( h > max_height )
        h = max_height;
    y = index * h + tabw->tabs.main_tab_width;

    tab_shape[0].y = tab_shape[5].y = tab_shape[6].y = y - h + shadow;
    tab_shape[1].y = tab_shape[2].y = y - SHADOW_OFFSET(shadow);
    tab_shape[3].y = y - SHADOW_OFFSET(shadow) - SLANT;

    if ( h != max_height )
    {
        tab_shape[0].y = tab_shape[5].y = tab_shape[6].y = y - h;
        tab_shape[4].y = y - h - SLANT - SHADOW_OFFSET(shadow);
    }
    else
    {
        tab_shape[0].y = tab_shape[5].y = tab_shape[6].y = y - h + shadow;
        tab_shape[4].y = y - h + SLANT + shadow;
    }


    /*
     * Get X positions based on location
     */
    if ( TAB_LOCATION(tabw) == XgLEFT )
    {
        tab_shape[0].x = tab_shape[1].x = WIDTH(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].x = shadow + SLANT;
        tab_shape[3].x = tab_shape[4].x = shadow;
        tab_shape[5].x = shadow + SLANT;
        tab_shape[6].x = tab_shape[0].x;
    }
    else
    {
        tab_shape[0].x = tab_shape[1].x = shadow;
        tab_shape[2].x = WIDTH(tabw) - SHADOW_OFFSET(shadow) - SLANT;
        tab_shape[3].x = tab_shape[4].x = WIDTH(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[5].x = WIDTH(tabw) - SHADOW_OFFSET(shadow) - SLANT;
            tab_shape[6].x = tab_shape[0].x;
    }


    /*
     * Set up the clip region for the tab text
     */
    clip = XPolygonRegion(tab_shape, XtNumber(tab_shape), EvenOddRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);

    /*
     * Fill the tab background
     */
    if ( highlight == False )
        XFillPolygon(dpy, win, tabw->tabs.fillgc, 
            tab_shape, XtNumber(tab_shape), Convex, CoordModeOrigin);

    /*
     * Draw the tab text
     */
    if ( tabw->tabs.labels && tabw->tabs.labels[index] != NULL )
    {
    int textw, len, texty;

        len = strlen(tabw->tabs.labels[index]);
        textw = XTextWidth(tabw->tabs.font, tabw->tabs.labels[index], len);
        if ( h == max_height )
            texty = y - (h + textw) / 2;
        else
            texty = y - tabw->primitive.shadow_thickness - textw -
                    HIGHLIGHT_THICKNESS(tabw) - tabw->tabs.textmargin;

        DrawVertText(tabw, texty, textw, index, len, tabw->tabs.tabcolor, 
            tabw->primitive.foreground, clip, highlight);
    }

    /*
     * Now actually draw the tab
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgLEFT )
        {
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                tab_shape + 1, 2, CoordModeOrigin);            

            if ( h == max_height )
                XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                    tab_shape + 2, 5, CoordModeOrigin);
            else
                XDrawLines(dpy, win, TOP_SHADOW_GC(tabw), 
                    tab_shape + 2, 3, CoordModeOrigin);
        }
        else
        {
            if ( h == max_height )
            {
                XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                    tab_shape + 1, 5, CoordModeOrigin);            

                XDrawLines(dpy, win, TOP_SHADOW_GC(tabw),
                    tab_shape + 5, 2, CoordModeOrigin);
            }
            else
                XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw),
                    tab_shape + 1, 4, CoordModeOrigin);
        }
    }


    /*
     * Destroy the clipping region
     */
    XDestroyRegion(clip);
}



static void 
#if NeedFunctionPrototypes
VertCurrentChamferred(XgTabsWidget tabw, Region region, Boolean highlight)
#else
VertCurrentChamferred(tabw, region, highlight)
XgTabsWidget tabw;
Region  region;
Boolean highlight;
#endif
{
Dimension h, shadow, max_height;
Position  y1, y2;
Display   *dpy = XtDisplay(tabw);
Region    clip;
XPoint    tab_shape[10];
Window    win   = XtWindow((Widget)tabw);


    /*
     * Compute the size and location constants for the tab
     */
    max_height = tabw->tabs.main_tab_width;

    if ( tabw->tabs.numtabs > 1 )
        h = (HEIGHT(tabw) - tabw->tabs.main_tab_width) /
           (tabw->tabs.numtabs - 1);
    else
        h = 0;

    h = (h > max_height) ? (max_height < 1 ? 1 : max_height) : h;

    y1 = tabw->tabs.curtab * h;
    y2 = y1 + tabw->tabs.main_tab_width;
    shadow = tabw->primitive.shadow_thickness / 2;


    /*
     * Vertical tabs have same Y location, whether we are 
     * on the right or left
     */
    tab_shape[0].y = 0;                        
    tab_shape[1].y = tab_shape[2].y = tab_shape[3].y = y1 + shadow;        
    tab_shape[4].y = y1 + shadow + SLANT;
    tab_shape[5].y = y2 - SHADOW_OFFSET(shadow) - SLANT;                
    tab_shape[6].y = tab_shape[7].y = tab_shape[8].y = 
        y2 - SHADOW_OFFSET(shadow);
    tab_shape[9].y = HEIGHT(tabw);                        

    /*
     * Get X positions based on location
     */
    if ( TAB_LOCATION(tabw) == XgLEFT )
    {
        tab_shape[0].x = tab_shape[1].x = tab_shape[8].x = tab_shape[9].x = 
            WIDTH(tabw) - SHADOW_OFFSET(shadow);
        tab_shape[2].x = tab_shape[7].x = WIDTH(tabw);
        tab_shape[3].x = tab_shape[6].x = shadow + SLANT;
        tab_shape[4].x = tab_shape[5].x = shadow;
    }
    else
    {
        tab_shape[0].x = tab_shape[1].x = tab_shape[8].x = 
            tab_shape[9].x = shadow;
        tab_shape[2].x = tab_shape[7].x = 0;
        tab_shape[3].x = tab_shape[6].x = 
            WIDTH(tabw) - SHADOW_OFFSET(shadow) - SLANT;
        tab_shape[4].x = tab_shape[5].x = WIDTH(tabw) - SHADOW_OFFSET(shadow);
    }

    /*
     * Set up the clip region for the tab text
     */
    clip = XPolygonRegion(tab_shape, XtNumber(tab_shape), EvenOddRule);
    XSetRegion(dpy, tabw->tabs.textgc, clip);

    /*
     * Fill the tab background
     */
    if ( highlight == False )
    {
        if ( tabw->tabs.tabcolor != tabw->tabs.selected_color )
            XSetForeground(dpy, tabw->tabs.fillgc, tabw->tabs.selected_color);

        XFillPolygon(dpy, win, tabw->tabs.fillgc, 
            tab_shape + 2, 6, Convex, CoordModeOrigin);

        if ( tabw->tabs.tabcolor != tabw->tabs.selected_color )
            XSetForeground(dpy, tabw->tabs.fillgc, tabw->tabs.tabcolor);
    }


    /*
     * Draw the tab text
     */
    if ( tabw->tabs.labels && 
        tabw->tabs.labels[tabw->tabs.curtab] != NULL )
    {
    int len, textw, texty, index = tabw->tabs.curtab;

        len = strlen(tabw->tabs.labels[index]);
        textw = XTextWidth(tabw->tabs.font, tabw->tabs.labels[index], len);
        texty = (y1 + y2 - textw) / 2;
        DrawVertText(tabw, texty, textw, index, len, 
            tabw->tabs.selected_color, tabw->primitive.foreground, 
            clip, highlight);
    }


    /*
     * Now actually draw the tab
     */
    if ( highlight == False )
    {
        if ( TAB_LOCATION(tabw) == XgLEFT )
        {
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw), 
                tab_shape, 6, CoordModeOrigin);
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw), 
                tab_shape + 5, 3, CoordModeOrigin);
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw), 
                tab_shape + 8, 2, CoordModeOrigin);
        }
        else
        {
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw), 
                tab_shape, 2, CoordModeOrigin);
            XDrawLines(dpy, win, TOP_SHADOW_GC(tabw), 
                tab_shape + 2, 2, CoordModeOrigin);
            XDrawLines(dpy, win, BOTTOM_SHADOW_GC(tabw), 
                tab_shape + 3, 7, CoordModeOrigin);
        }
    }


    /*
     * Destroy the clipping region
     */
    XDestroyRegion(clip);
}


static void 
CreateTopGc(tabw)
XgTabsWidget tabw;
{
XtGCMask mask = GCForeground | GCLineWidth;
XGCValues values;

    if ( tabw->tabs.topgc != None ) 
        XtReleaseGC((Widget)tabw, tabw->tabs.topgc);

    values.foreground = tabw->primitive.top_shadow_color;
    values.line_width = tabw->primitive.shadow_thickness;
    tabw->tabs.topgc = XtGetGC((Widget)tabw, mask, &values);
}


static void
CreateBottomGc(tabw)
XgTabsWidget tabw;
{
XtGCMask mask = GCForeground | GCLineWidth;
XGCValues values;

    if ( tabw->tabs.bottomgc != None ) 
            XtReleaseGC((Widget)tabw, tabw->tabs.bottomgc);

    values.foreground = tabw->primitive.bottom_shadow_color;
    values.line_width = tabw->primitive.shadow_thickness;
    tabw->tabs.bottomgc = XtGetGC((Widget)tabw, mask, &values);
}


static void
CreateTextGc(tabw)
XgTabsWidget tabw;
{
XtGCMask mask = GCForeground | GCBackground | GCFont | GCLineWidth | 
                        GCFunction | GCPlaneMask | GCGraphicsExposures;
XGCValues values;

    /*
     * Get the XFontStruct from the XmFontList
     */
    tabw->tabs.font = GetFontStruct(tabw->tabs.font_list);

    values.foreground = tabw->primitive.foreground;
    values.background = tabw->tabs.tabcolor;
    values.font = tabw->tabs.font->fid;
    values.plane_mask = AllPlanes;
    values.function = GXcopy;
    values.line_width = tabw->primitive.highlight_thickness;
    values.graphics_exposures = False;

    if ( tabw->tabs.textgc == None )
        tabw->tabs.textgc = XCreateGC(XtDisplay((Widget)tabw), 
                RootWindowOfScreen(XtScreen((Widget)tabw)), mask, &values);
    else
        XChangeGC(XtDisplay((Widget)tabw), tabw->tabs.textgc,
            mask, &values);
}



static void
CreateFillGc(tabw)
XgTabsWidget tabw;
{
XtGCMask mask = GCForeground;
XGCValues values;

    values.foreground = tabw->tabs.tabcolor;
    
    if ( tabw->tabs.fillgc == None )
        tabw->tabs.fillgc = XCreateGC(XtDisplay((Widget)tabw), 
                RootWindowOfScreen(XtScreen((Widget)tabw)), mask, &values);
    else
        XSetForeground(XtDisplay((Widget)tabw), tabw->tabs.fillgc,
            tabw->tabs.tabcolor);
}



static void 
CopyBg(tabw,offset,value)
XgTabsWidget tabw;
int  offset;
XrmValue * value;
{
    /*
     * Get the background from the core part
     */
    value->addr = (XtPointer) &tabw->core.background_pixel;
}





/*
 * Compare two strings. The test string must NULL terminated. 
 * Leading and trailing whitespace in the in string is ignored.
 */

static Boolean
CompareStrings(in, test)
String in, test;
{
    /*
     * Strip leading whitespace off the in string.
     */
    while ( isspace(*in) )
        in++;

    for ( ; *test != '\0' && !isspace(*in); test++, in++ )
    {
    char        c = *in;
    char        t = *test;

        if ( isupper(c) )
            c = tolower(c);

        if ( isupper(t) )
            t = tolower(t);

        if ( c != t )
            return False;
    }

    if ( *test == '\0' && (*in == '\0' || isspace(*in)) )
        return True;
    else
        return False;
}


static Boolean
cvtStringToTabShape(dpy, args, num_args, from, to, converter_data)
Display         *dpy;
XrmValuePtr        args;
Cardinal        *num_args;
XrmValuePtr        from;
XrmValuePtr        to;
XtPointer         *converter_data;
{
static XgTabShapeType tab_shape;

    if ( *num_args != 0 )
    {
        XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
            "cvtStringToTabShape", "wrongParameters",
            "XgTabs",
            "String to TabShape conversion needs no arguments",
             NULL, NULL);
    }

    /*
     * User didn't provide enough space
     */
    if ( to->addr != NULL && to->size < sizeof(XgTabShapeType)  )
    {
        to->size = sizeof(XgTabShapeType);
        return False;
    }

    /*
     * Look for a match with the different shapes of Tabs
     */ 
    if ( CompareStrings(from->addr, "slanted") )
        tab_shape = XgSLANTED;
    else if ( CompareStrings(from->addr, "chamferred") )
        tab_shape = XgCHAMFERRED;
    else 
    {
        XtDisplayStringConversionWarning(dpy, from->addr, 
            XgRTabShapeType);
        return False;
    }

    /*
     * Store our return value
     */
    if ( to->addr == NULL )
        to->addr = (caddr_t) &tab_shape;
    else
        *(XgTabShapeType*) to->addr = tab_shape;

    to->size = sizeof(XgTabShapeType);

    return True;
}




#if NeedFunctionPrototypes
static Boolean 
cvtStringToStringArray(
    Display *display,
    XrmValuePtr args,
    Cardinal *num_args,
    XrmValuePtr from,
    XrmValuePtr to,
    XtPointer *converter_data
)
#else
static Boolean 
cvtStringToStringArray(display, args, num_args, from, to, converter_data)
    Display *display;
    XrmValuePtr args;
    Cardinal *num_args;
    XrmValuePtr from;
    XrmValuePtr to;
    XtPointer *converter_data;
#endif
{
int                i;
char                *tmp_string, *string;
static String        *items;        
static int        item_count = 0;

    if (*num_args != 0)
        XtAppErrorMsg(XtDisplayToApplicationContext(display),
              "cvtStringToStringArray", "wrongParameters",
              "XtToolkitError",
              "String to StringArray conversion needs no arguments",
              (String*) NULL, (Cardinal*) NULL);


    items = (String *) XtMalloc(sizeof(String));        
    items[0] = NULL;
    item_count = 0;

    string = (char *)from->addr;

    if ( string != NULL )
    {
    int        len;
    char *saved_string;

        string = saved_string = XtNewString(string);
        len = strlen(string);
        

        for ( tmp_string = string, i = 0; i <= len; i++ )
        {
            if ( string[i] == ',' || string[i] == '\0' )
            {
                if ( *tmp_string == '\0' )
                        break;

                string[i] = '\0';

                items[item_count] = XtNewString(tmp_string);

                tmp_string = string + i + 1;

                if ( *tmp_string == '\n' )
                        tmp_string++;

                item_count++;
                items = (String *)XtRealloc((char *)items, 
                    sizeof(String) * (item_count + 1));
                items[item_count] = NULL;
            }
        }

        XtFree(saved_string);
    }

    if (to->addr != NULL) 
    {
            if (to->size < sizeof(String *)) 
            {
            to->size = sizeof(String *);
            return False;
            }

        *(String**)(to->addr) = (items);
    }
    else 
    {
            static String *static_val;
            static_val = (items);
            to->addr = (XtPointer)&static_val;
    }

    to->size = sizeof(String *);
    return True;
}


static void
freeStringArray(string)
String *string;
{
int i;

    if ( string == NULL )
        return;

    for ( i = 0; string[i] != NULL; i++ )
        XtFree(string[i]);

    XtFree((String) string);
}




Widget
XgCreateTabsWidget(parent, name, al, ac)
Widget parent;
char *name;
ArgList al;
Cardinal ac;
{
    return XtCreateWidget(name, xgTabsWidgetClass, parent, al, ac);
}



static void 
TraverseTabs(tabw, event, params, num_params)
XgTabsWidget tabw;
XEvent *event;
String *params;
Cardinal *num_params;
{
    /*
     * Unhighlight the current tab
     */
    UnHighlight(tabw);

    /*
     * See which direction to move the focus tab
     */
    switch ( *(params[0]) )
    {
      case 'p': /* prev */ 
        tabw->tabs.focus_tab--;
        break;

      case 'n': /* next */
        tabw->tabs.focus_tab++;
        break;
    }

    /*
     * Wrap around
     */
    if ( tabw->tabs.focus_tab < 0 )
        tabw->tabs.focus_tab = tabw->tabs.numtabs - 1;
    else if ( tabw->tabs.focus_tab >= tabw->tabs.numtabs )
        tabw->tabs.focus_tab = 0;

    /*
     * Highlight the new current tab
     */
    Highlight(tabw);

}


static void 
ProcessTraversal(tabw, event, params, num_params)
XgTabsWidget tabw;
XEvent *event;
String *params;
Cardinal *num_params;
{
    /*
     * See which direction to process traversal
     */
    switch ( *(params[0]) )
    {
      case 'n': /* next */
        XmProcessTraversal((Widget)tabw, XmTRAVERSE_NEXT_TAB_GROUP);
        break;

      case 'p': /* prev */
        XmProcessTraversal((Widget)tabw, XmTRAVERSE_PREV_TAB_GROUP);
        break;

      case 'i': /* in   */
        tabw->tabs.i_have_focus = True;
        Highlight(tabw);
        break;

      case 'o': /* out  */
        tabw->tabs.i_have_focus = False;
        UnHighlight(tabw);
        break;

      default:
        break;
    }
}


static void
#if NeedFunctionPrototypes
DrawHorizText(XgTabsWidget tabw, int x, int width, int index, int len,
    unsigned long foreground, Boolean highlight)
#else
DrawHorizText(tabw, x, width, index, len, foreground, highlight)
XgTabsWidget        tabw;
int                x;
int                width;
int                index;
int                len;
unsigned long        foreground;
Boolean                highlight;
#endif
{
Display *dpy = XtDisplay((Widget)tabw);
Window  win = XtWindow((Widget)tabw);
Boolean reset_foreground = False;
int        y = tabw->primitive.shadow_thickness + tabw->tabs.textmargin +
                HIGHLIGHT_THICKNESS(tabw) + tabw->tabs.font->ascent;;

    /*
     * Change the foregound color if needed
     */
    if ( tabw->primitive.foreground != foreground )
    {
        reset_foreground = True;
        XSetForeground(dpy, tabw->tabs.textgc, foreground);
    }

        
    /*
     * Only draw the text if we are not highlighting the tab
     */
    if ( highlight == False )
         XDrawString(dpy, win, tabw->tabs.textgc,
             x, y, tabw->tabs.labels[index], len);

    /*
     * Do we need to highlight this tab?
     */
    if ( HIGHLIGHT_THICKNESS(tabw) > 0 && (highlight == True || 
        (index == tabw->tabs.focus_tab && 
        tabw->tabs.i_have_focus == True)) )
    {
    int            highlight_thickness;
    int            half;
    unsigned long  line_color;

        highlight_thickness = HIGHLIGHT_THICKNESS(tabw);
        half = highlight_thickness / 2;

        if ( index == tabw->tabs.focus_tab )
            line_color = tabw->primitive.highlight_color;
        else
        {
            if ( index == tabw->tabs.curtab )
                line_color = tabw->tabs.selected_color;
            else
                line_color = tabw->tabs.tabcolor;
        }

        if ( line_color != tabw->primitive.foreground )
        {
            reset_foreground = True;
            XSetForeground(dpy, tabw->tabs.textgc, line_color);
        }

        XDrawRectangle(dpy, win, tabw->tabs.textgc,
            x - highlight_thickness + half - 1,
            tabw->primitive.shadow_thickness + SHADOW_OFFSET(half) +
                tabw->tabs.textmargin,
            width + highlight_thickness + 2, 
            tabw->tabs.font->ascent + tabw->tabs.font->descent +
            highlight_thickness + 1);
    }


    /*
     * If needed reset the foreground 
     */
    if ( reset_foreground )
        XSetForeground(dpy, tabw->tabs.textgc, tabw->primitive.foreground);
}



static void
#if NeedFunctionPrototypes
DrawVertText(XgTabsWidget tabw, int y, int width, int index, int len,
    unsigned long background, unsigned long foreground, 
    Region clip, Boolean highlight)
#else
DrawVertText(tabw, y, width, index, len, background, foreground, 
        clip, highlight)
XgTabsWidget   tabw;
int            y;
int            width;
int            index;
int            len;
unsigned long  background;
unsigned long  foreground;
Region         clip;
Boolean        highlight;
#endif
{
Display *dpy = XtDisplay((Widget)tabw);
Window  win = XtWindow((Widget)tabw);
Boolean reset_foreground = False, reset_background = False;
int        x = tabw->primitive.shadow_thickness + tabw->tabs.textmargin +
                HIGHLIGHT_THICKNESS(tabw);


    /*
     * Change the backround color if needed
     */
    if ( tabw->tabs.tabcolor != background )
    {
        reset_background = True;
        XSetBackground(dpy, tabw->tabs.textgc, background);
    }

    /*
     * Only draw the text if we are not highlighting the tab
     */
    if ( highlight == False )
    {
    unsigned int w = tabw->tabs.font->ascent + tabw->tabs.font->descent;
    unsigned int h = width;

        if ( index != tabw->tabs.curtab )
        {
            /*
             * We keep a copy of the label pixmap around for each unselected
             * tab to speed things up so if it has not been created do it now 
             */
            if ( tabw->tabs.label_pixmaps[index] == None )
            {
                tabw->tabs.label_pixmaps[index] =
                    XCreatePixmap(dpy, DefaultRootWindow(dpy),
                            w, h, tabw->core.depth); 

                /*
                 * Change the foregound color if needed
                 */
                if ( tabw->primitive.foreground != foreground )
                {
                    reset_foreground = True;
                    XSetForeground(dpy, tabw->tabs.textgc, foreground);
                }

                XSetClipMask(dpy, tabw->tabs.textgc, None);

                XgDrawRotatedText(dpy, 
                    tabw->tabs.label_pixmaps[index], tabw->tabs.textgc, 0, 0,
                    ((TAB_LOCATION(tabw) == XgRIGHT) ? XgALIGN_LEFT : 
                        XgALIGN_RIGHT),
                    ((TAB_LOCATION(tabw) == XgRIGHT) ? 90 : 270), 
                    tabw->tabs.labels[index], len);

                XSetRegion(dpy, tabw->tabs.textgc, clip);
            }

            /*
             * Just copy the pixmap that contains the label (this is fast)
             */
            if ( tabw->tabs.label_pixmaps[index] != None )
                XCopyArea(dpy, tabw->tabs.label_pixmaps[index], win,
                    tabw->tabs.textgc, 0, 0, w, h, x, y);

        }
        else
        {
            /*
             * Change the foregound color if needed
             */
            if ( tabw->primitive.foreground != foreground )
            {
                reset_foreground = True;
                XSetForeground(dpy, tabw->tabs.textgc, foreground);
            }

            /*
             * To avoid having to keep a pixmap with both selected
             * and unselected labels we draw the selected label each time
             */
                   XgDrawRotatedText(dpy, win, tabw->tabs.textgc, x, y,
                (TAB_LOCATION(tabw) == XgRIGHT) ? XgALIGN_LEFT : 
                    XgALIGN_RIGHT,
                (TAB_LOCATION(tabw) == XgRIGHT) ? 90 : 270, 
                tabw->tabs.labels[index], len);
        }
    }
    

    /*
     * Do we need to highlight this tab?
     */
    if ( HIGHLIGHT_THICKNESS(tabw) > 0 && (highlight == True || 
        (index == tabw->tabs.focus_tab && 
        tabw->tabs.i_have_focus == True)) )
    {
    int                   highlight_thickness;
    int            half;
    unsigned long  line_color;

        highlight_thickness = HIGHLIGHT_THICKNESS(tabw);
        half = highlight_thickness / 2;

        if ( index == tabw->tabs.focus_tab )
            line_color = tabw->primitive.highlight_color;
        else
        {
            if ( index == tabw->tabs.curtab )
                line_color = tabw->tabs.selected_color;
            else
                line_color = tabw->tabs.tabcolor;
        }

        if ( line_color != tabw->primitive.foreground )
        {
            reset_foreground = True;
            XSetForeground(dpy, tabw->tabs.textgc, line_color);
        }


        /*
         * The rotated text is shifted down 1 pixel if the text
         * rotation is 270 (the Tabs is on the left side)
         */
        if ( tabw->tabs.tab_location == XgLEFT )
            y--;
        
        /*
         * Draw the highlight rectangle
         */
        XDrawRectangle(dpy, win, tabw->tabs.textgc,
            x - highlight_thickness + half,
            y - highlight_thickness + half - 1,
            tabw->tabs.font->ascent + tabw->tabs.font->descent +
                    highlight_thickness,
            width + highlight_thickness + 1);
    }


    /*
     * If needed reset the foreground 
     */
    if ( reset_foreground )
        XSetForeground(dpy, tabw->tabs.textgc, tabw->primitive.foreground);

    /*
     * If needed reset the background 
     */
    if ( reset_background )
        XSetBackground(dpy, tabw->tabs.textgc, tabw->tabs.tabcolor);

}


/* +++PHDR+++
 *
 * Function:        GetFontStruct()
 *
 * Scope:        static
 *
 * Description:
 *        This function takes the passed XmFontList and returns a pointer
 *        to an XFontStruct.
 *
 * Argument     Type                     Description
 * ------------ -----------------------  -------------------------------------
 * font_list    XmFontList               XmFontList to get the XFontStruct from
 *
 *
 * Returns:        XFontStruct *
 *
 * ---PHDR--- */

static XFontStruct *
GetFontStruct(font_list)
XmFontList        font_list;
{
#if (XmVERSION == 1 && XmREVISION > 0) || XmVERSION > 1
XmFontContext        context;
XmStringCharSet      charset;
static XFontStruct   *font;

    XmFontListInitFontContext(&context, font_list);
    XmFontListGetNextFont(context, &charset, &font);

    /*
     * Free the XmStringCharSet which was allocated 
     * by XmFontListGetNextFont()
     */
    if ( charset != NULL )
        XtFree(charset);

    XmFontListFreeFontContext(context);        

    return font;
#else

    return font_list->font;
#endif
}


static void
FreePixmapArray(tabw)
XgTabsWidget tabw;
{
int     i;
Display *dpy = XtDisplay((Widget)tabw);

    if ( tabw->tabs.label_pixmaps == NULL )
        return;

    for ( i = 0; i < tabw->tabs.numtabs; i++ )
        if ( tabw->tabs.label_pixmaps[i] != None )
            XFreePixmap(dpy, tabw->tabs.label_pixmaps[i]);

    XtFree((char *)tabw->tabs.label_pixmaps);
    tabw->tabs.label_pixmaps = NULL;
}


static void
CreatePixmapArray(tabw)
XgTabsWidget tabw;
{
int        i;

    FreePixmapArray(tabw);

    /*
     * Only allocate if the tabs are vertical
     */
    if ( IS_HORIZ_TAB(tabw) )
        return;

    tabw->tabs.label_pixmaps = (Pixmap *)XtMalloc(sizeof(Pixmap) *
        tabw->tabs.numtabs);

    for ( i = 0; i < tabw->tabs.numtabs; i++ )
        tabw->tabs.label_pixmaps[i] = None;
}

