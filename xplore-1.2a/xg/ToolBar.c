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


#include    <Xm/XmP.h>
#include    <Xm/PushB.h>
#include    <Xm/Label.h>

#include    "ToolBarP.h"

#include "XgUtils.h"


/*
 * Include the bitmap files for the default drag cursor
 */
#include     "btn15x15.xbm"
#include     "btn15x15_m.xbm"
#include     "btn31x31.xbm"
#include     "btn31x31_m.xbm"


#ifndef RES_CONVERT
#   define RES_CONVERT(r, v)   XtVaTypedArg, r, XmRString, v, strlen(v) + 1
#endif


#ifndef MAX
#  define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#  define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#define SPACER_WIDTH	    8

#define TOOLBARDIR(tb)      tb->toolbar.orientation
#define TOOLBARSTRETCH(tb)  tb->toolbar.stretch

#define NUM_TOOLBARS	   (xgtoolBarClassRec.toolbar_class.num_toolbars)
#define TOOLBAR_LIST	   (xgtoolBarClassRec.toolbar_class.toolbar_list)
#define DROP_WIDGET	   (xgtoolBarClassRec.toolbar_class.drop_widget)
#define DROP_TIMEOUT	   (xgtoolBarClassRec.toolbar_class.drop_timeout)
#define DRAG_CURSOR	   (xgtoolBarClassRec.toolbar_class.drag_cursor)
#define DROP_TIMER_ID	   (xgtoolBarClassRec.toolbar_class.drop_timer_id)

#define NUM_REGISTERED	   (xgtoolBarClassRec.toolbar_class.num_registered)
#define REGISTERED_WIDGETS (xgtoolBarClassRec.toolbar_class.registered_widgets)

#define CLONES(w)	   (((XgToolBarWidget)w)->toolbar.clones)
#define NUM_CLONES(w)	   (((XgToolBarWidget)w)->toolbar.num_clones)
#define HINT_TIMER(w)	   (((XgToolBarWidget)w)->toolbar.hint_timer_id)
#define HINT_WINDOW(w)	   (((XgToolBarWidget)w)->toolbar.hint_window)
#define HINT_DELAY(w)      (((XgToolBarWidget)w)->toolbar.hint_delay)
#define HINT_BACKGROUND(w) (((XgToolBarWidget)w)->toolbar.hint_bg)
#define HINT_FOREGROUND(w) (((XgToolBarWidget)w)->toolbar.hint_fg)
#define HINT_FONTLIST(w)   (((XgToolBarWidget)w)->toolbar.font_list)
#define HINTS_ENABLED(w)   (((XgToolBarWidget)w)->toolbar.hints_enabled)



static void		 ClassInitialize();
static void              Initialize();
static void		 Destroy();
static void 		 InsertChild();
static void              ChangeManaged();
static Boolean           SetValues();
static XtGeometryResult  GeometryManager();
static XtGeometryResult  PreferredSize();
static XtGeometryResult  Trylayout();
static void		 DoLayout();
static XtGeometryResult	 TryLayout();
static RegisteredWidget_t *GetRegisteredWidget();
static int               GetToolbarIndex();
static void		 AddWidgetToToolBar();

static void DropTimeout();
static void DropWidget();
static void ToolHintHandler();
static void ToolHintTimeout();
static void AbortToolHint();


static RegisteredWidget_t spacer_widget = 
	{NULL, "SPACER", NULL, XgToolBarSpacerCloner, NULL};


/*
 * Setup translations for tools (widgets) to be added to a ToolBar
 */
static char toolTranslationsTable[] =
    "Shift<Btn1Down>:	  StartDrop()";
     
static void StartDrop();

static XtActionsRec toolActionsTable[] = {
    { "StartDrop", StartDrop},
    { NULL, NULL }};

static XtTranslations toolTranslations;



/*
 *
 */
static XtResource resources[] = {
    { 
	XmNorientation, XmCOrientation, XmROrientation, sizeof(unsigned char),
        XtOffsetOf(XgToolBarRec, toolbar.orientation), 
	XmRImmediate, (XtPointer)XmHORIZONTAL
    },
    { 
	XgNhorizontalStretch, XgCStretch, XmRBoolean, sizeof(Boolean),
        XtOffsetOf(XgToolBarRec, toolbar.hstretch), 
	XmRImmediate, (XtPointer)False
    },
    { 
	XgNverticalStretch, XgCStretch, XmRBoolean, sizeof(Boolean),
        XtOffsetOf(XgToolBarRec, toolbar.vstretch), 
	XmRImmediate, (XtPointer)False
    },
    { 
	XmNspacing, XmCSpacing, XmRDimension, sizeof(Dimension),
        XtOffsetOf(XgToolBarRec, toolbar.spacing), 
	XmRImmediate, (XtPointer)2
    },
    { 
	XmNmarginHeight, XmCMarginHeight, XmRDimension, sizeof(Dimension),
        XtOffsetOf(XgToolBarRec, toolbar.margin_height), 
	XmRImmediate, (XtPointer)2
    },
    { 
	XmNmarginWidth, XmCMarginWidth, XmRDimension, sizeof(Dimension),
        XtOffsetOf(XgToolBarRec, toolbar.margin_width), 
	XmRImmediate, (XtPointer)2
    },
    {
	XgNcloneList, XgCCloneList, XmRStringTable, sizeof(String *),
	XtOffsetOf(XgToolBarRec, toolbar.clone_list),
	XmRImmediate, (XtPointer)NULL
    },
    {
        XgNhintDelay, XgCHintDelay, XmRInt, sizeof(int),
	XtOffsetOf(XgToolBarRec, toolbar.hint_delay),
	XmRImmediate, (XtPointer)100
    },
    {
    	XgNhintBackground, XgCHintBackground, XmRPixel, sizeof(Pixel),
	XtOffsetOf(XgToolBarRec, toolbar.hint_bg),
	XmRString, (XtPointer)"white"
    },
    {
    	XgNhintForeground, XgCHintForeground, XmRPixel, sizeof(Pixel),
	XtOffsetOf(XgToolBarRec, toolbar.hint_fg),
	XmRString, (XtPointer)"black"
    },
    {
        XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
        XtOffsetOf(XgToolBarRec, toolbar.font_list),
        XtRString, (XtPointer)"fixed"
    },
    {
	XgNhintsEnabled, XgCHintsEnabled, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XgToolBarRec, toolbar.hints_enabled),
	XmRImmediate, (XtPointer)True
    },
    {
	XmNeditable, XmCEditable, XmRBoolean, sizeof(Boolean),
        XtOffsetOf(XgToolBarRec, toolbar.editable), 
	XmRImmediate, (XtPointer)True
    }
};




XgToolBarClassRec xgtoolBarClassRec = {
  {
    /*
     * core_class members
     */
    (WidgetClass) &xmManagerClassRec,   /* superclass         */
    "ToolBar",                          /* class_name         */
    sizeof(XgToolBarRec),               /* widget_size        */
    ClassInitialize,                    /* class_initialize   */
    NULL,                               /* class_part_init    */    
    FALSE,                              /* class_inited       */    
    Initialize,                         /* initialize         */
    NULL,                               /* initialize_hook    */    
    XtInheritRealize,                   /* realize            */
    NULL,                               /* actions            */
    0,                                  /* num_actions        */    
    resources,                          /* resources          */
    XtNumber(resources),                /* num_resources      */
    NULLQUARK,                          /* xrm_class          */
    TRUE,                               /* compress_motion    */  
    TRUE,                               /* compress_exposure  */  
    TRUE,                               /* compress_enterleave*/  
    FALSE,                              /* visible_interest   */
    (XtWidgetProc)Destroy,              /* destroy            */
    (XtWidgetProc)DoLayout,             /* resize             */
    NULL,                               /* expose             */
    (XtSetValuesFunc)SetValues,         /* set_values         */
    NULL,                               /* set_values_hook    */
    XtInheritSetValuesAlmost,           /* set_values_almost  */
    NULL,                               /* get_values_hook    */  
    NULL,                               /* accept_focus       */
    XtVersion,                          /* version            */  
    NULL,                               /* callback_private   */
    NULL,                               /* tm_table           */
    PreferredSize,                      /* query_geometry     */  
    NULL,                               /* display_accelerator*/
    NULL,                               /* extension          */
  },
  {
    /*
     * composite_class members
     */
    GeometryManager,                    /* geometry_manager  */
    ChangeManaged,                      /* change_managed    */
    InsertChild,	               	/* insert_child      */  
    XtInheritDeleteChild,               /* delete_child      */  
    NULL                                /* next_extension    */  
  },
  { 
    /*
     * constraint_class fields
     */
    NULL,                               /* resource list      */ 
    0,                                  /* num resources      */ 
    0,                                  /* constraint size    */    
    NULL,                               /* init proc          */    
    NULL,                               /* destroy proc       */
    NULL,                               /* set values proc    */ 
    NULL                                /* next_extension     */
  },
  {
    /*
     * manager_class
     */
    XtInheritTranslations,              /* translations       */    
    NULL,                               /* syn_resources      */    
    0,                                  /* num_syn_resources  */    
    NULL,                               /* syn_cont_resources */    
    0,                                  /* num_syn_cont_resources */
    XmInheritParentProcess,             /* parent_process     */    
    NULL                                /* next_extension     */
  }, 
  {
    /*
     * Toolbar class members
     */
    NULL,                               /* drop_widget        */  
    NULL,			        /* toolbar_list       */
    0,					/* num_toolbars       */
    10,					/* drop_timeout       */
    None,			        /* drag_cursor        */
    None,				/* drop_timer_id      */
    0,					/* num_registered     */
    NULL,				/* registered_classes */
  }
};

WidgetClass xgToolBarWidgetClass = (WidgetClass) &xgtoolBarClassRec;




static void
ClassInitialize()
{
        /*
         * Parse the translation table used by tools to add themselves to 
	 * a ToolBar
         */
	toolTranslations = XtParseTranslationTable(toolTranslationsTable);

	NUM_TOOLBARS       = 0;
	TOOLBAR_LIST       = NULL;
	DROP_WIDGET        = NULL;
	DROP_TIMEOUT       = None;
	DRAG_CURSOR        = None;
	DROP_TIMER_ID      = None;
	NUM_REGISTERED     = 0;
	REGISTERED_WIDGETS = NULL;
}


static void 
Initialize(request, new)
XgToolBarWidget request;
XgToolBarWidget new;
{
Arg 	  dropArgs[6];
Cardinal  ac;

	/*
	 * No hint_window created yet
	 */
	new->toolbar.hint_window = NULL;
	new->toolbar.hint_timer_id = None;

	if ( request->core.width <= 0 )
    		new->core.width = 5;

  	if ( request->core.height <= 0 )
    		new->core.height = 5;

	/*
	 * Add this widget to the list od toolbars
	 */
	TOOLBAR_LIST = (XgToolBarWidget *)XtRealloc((char *)TOOLBAR_LIST,
		(NUM_TOOLBARS + 1) * sizeof(XgToolBarWidget));

	if ( TOOLBAR_LIST != NULL )
	{
		TOOLBAR_LIST[NUM_TOOLBARS] = new;
		NUM_TOOLBARS++;
	}

	/*
	 * Stuff to keep track of the widgets that we cloned for the 
	 * widgets on the toolbar
	 */
	NUM_CLONES(new) = 0;
	CLONES(new) = NULL;
} 


static void
Destroy(toolbar)
XgToolBarWidget toolbar;
{
int i, found;


	/*
	 *
	 */
        if ( DROP_WIDGET == NULL )
	{

	        XtRemoveEventHandler((Widget)TOOLBAR_LIST[0], 
			ButtonReleaseMask, False, DropWidget, NULL);

		XtUngrabPointer((Widget)TOOLBAR_LIST[0], CurrentTime);
	}


	/*
	 * Remove the Drop Timeout if present
	 */
        if ( DROP_TIMER_ID != None )
        {
                XtRemoveTimeOut(DROP_TIMER_ID);
                DROP_TIMER_ID = None;
        }


	/*
	 * Delete the tool hint timeout if one was created
	 */
	if ( toolbar->toolbar.hint_timer_id != None )
	{
		XtRemoveTimeOut(toolbar->toolbar.hint_timer_id);
		toolbar->toolbar.hint_timer_id = None;
	}


	/*
	 * Destroy the hint window
	 */
	if ( toolbar->toolbar.hint_window != NULL )
	{
		XtDestroyWidget(XtParent(toolbar->toolbar.hint_window));
		toolbar->toolbar.hint_window = NULL;
	}

	/*
	 * Try and find this ToolBar in the toolbar list
	 */
	for ( i = 0, found = -1; i < NUM_TOOLBARS && found == -1; i++ )
		if ( TOOLBAR_LIST[i] == toolbar )
			found = i;

	if ( found == -1 )
		return;


	/*
	 * Remove this toolbar from the list of toolbars
	 */
	NUM_TOOLBARS--;
	for ( i = found; i < NUM_TOOLBARS; i++ )
		TOOLBAR_LIST[i]	= TOOLBAR_LIST[i + 1];

	/*
	 * Free or reallocate the toolbar list
	 */
	if ( NUM_TOOLBARS == 0 )
	{
		XtFree((char *)TOOLBAR_LIST);
		TOOLBAR_LIST = NULL;
	}
	else
		TOOLBAR_LIST = (XgToolBarWidget *)XtRealloc(
			(char *)TOOLBAR_LIST, 
			(NUM_TOOLBARS) * sizeof(XgToolBarWidget));	


        /*
         * Free up the list of clones widgets
         */
	if ( CLONES(toolbar) != NULL )
	{
	    for ( i = 0; i < NUM_CLONES(toolbar); i++ )
	    	if ( CLONES(toolbar)[i].tool_hint != NULL )
			XtFree(CLONES(toolbar)[i].tool_hint);

            XtFree((char *)CLONES(toolbar));
	}
}




static Boolean 
SetValues(current, request, new)
XgToolBarWidget current;
XgToolBarWidget request;
XgToolBarWidget new;
{
Boolean redraw = False; 

#define NE(field)       (current->field != new->field)

    /* 
     * Verify validity of requested state 
     */
    if ( TOOLBARDIR(new) != XmVERTICAL &&
         TOOLBARDIR(new) != XmHORIZONTAL )
    {
        XtWarning( "Orientation not valid, forcing to HORIZONTAL" );
        TOOLBARDIR(new) = XmHORIZONTAL;
    }

    redraw = NE(toolbar.orientation) || 
	NE(toolbar.hstretch) || NE(toolbar.vstretch);


    if ( redraw )
        DoLayout(new);

    return redraw;
}





static void
DoLayout(parent)
XgToolBarWidget parent;
{
Widget      child;
int         i;
Position    xpos,
            ypos;
Dimension   xpad,
            ypad;
int         n_managed_children;

    /*
    * Determine how many children are managed.
    */
    for ( n_managed_children = i = 0; i < parent->composite.num_children; i++ )
    {
        child = parent->composite.children[i];
        if ( child->core.managed )
            n_managed_children++;
    }

    if ( n_managed_children < 1 )
	return;


    /*
     * Resize all children if stretched in either direction.
     */
    if ( parent->toolbar.hstretch  || parent->toolbar.vstretch )
    {
    Dimension   childwidth,
                childheight,
                stretchwidth,
                stretchheight;

        for ( i = 0; i < parent->composite.num_children; i++ )
        {
            child = parent->composite.children[i];
            if ( child->core.managed )
            {
                /*
                 * Stretching rule:
                 *   Make the child fit the extents of its divided space
                 *
                 * This means that horizontal orientations have n x 1 cells
                 * and verticals have 1 x n cells.  We will have a state
                 * table consisting of 2x2=4 areas each according to the 
                 * stretching dimension (2) and widget orientation (2).
                 *
                 */

                /*
		 * Get Original child dimensions 
		 */
                childwidth = child->core.width + child->core.border_width * 2;
                childheight = child->core.height + child->core.border_width * 2;

                /* Compute new dimensions for h and v stretching */
                if ( TOOLBARDIR(parent) == XmHORIZONTAL )
                {
                    stretchwidth = parent->core.width / n_managed_children;
                    stretchheight = parent->core.height - 
                        child->core.border_width * 2;
                }
                else
                {
                    stretchwidth = parent->core.width - 
                        child->core.border_width * 2;
                    stretchheight = parent->core.height / n_managed_children;
                }

                /* Only stretch along correct dimensions */
                XtResizeWidget(child, 
                    parent->toolbar.hstretch ? stretchwidth : childwidth,
                    parent->toolbar.vstretch ? stretchheight : childheight,
                    child->core.border_width);
              }
        }
    }


    /*	
     * Position all children.
     */
    ypos = 0;
    xpos = 0;
    ypad = parent->toolbar.spacing;
    xpad = parent->toolbar.spacing;
    for ( i = 0; i < parent->composite.num_children; i++ )
    {	
        child = parent->composite.children[i];
        if ( child->core.managed )
        {
	    if ( TOOLBARDIR(parent) == XmHORIZONTAL )
	    {
		XtMoveWidget(child, xpos, ypos);
		xpos += xpad + child->core.width +
		    child->core.border_width * 2;
	    }	
	    else
	    {
		XtMoveWidget(child, xpos, ypos);
		ypos += ypad + child->core.height +
		    child->core.border_width * 2;
	    }
        }
    }
}



static XtGeometryResult
PreferredSize(w, request, preferred)
XgToolBarWidget w;
XtWidgetGeometry *request, *preferred;
{
Widget 	child;
int	num_managed = 0;
int 	i;

   /*
    * If no changes are being made to width or 
    * height, just agree. 
    */
   if(!(request->request_mode & CWWidth) &&
       !(request->request_mode & CWHeight))
    	    return (XtGeometryYes);


   /*
    * Calculate our minimum size.
    */
    preferred->width = 0;
    preferred->height = 0;       
    for ( num_managed = i = 0; i < w->composite.num_children; i++ )
    {
        child = w->composite.children[i];
        if ( child->core.managed )
        {
	    num_managed++;

            if ( TOOLBARDIR(w) == XmHORIZONTAL )
            {
                preferred->width += child->core.width + 
                    child->core.border_width * 2;
                if ( preferred->height < child->core.height + 
                    child->core.border_width * 2 )
		          preferred->height = child->core.height +
            			child->core.border_width * 2;
            }
            else
            {
                  preferred->height += child->core.height + 
                       child->core.border_width * 2;
                  if ( preferred->width < child->core.width + 
                       child->core.border_width * 2 )
          		    preferred->width = child->core.width +
            			child->core.border_width * 2;
            }
        }
    }

    if ( TOOLBARDIR(w) == XmHORIZONTAL )
	preferred->width += w->toolbar.spacing * (num_managed - 1);
    else
	preferred->height += w->toolbar.spacing * (num_managed - 1);

    preferred->request_mode = CWWidth | CWHeight;

    /* 
     * If both width and height are requested.
     */
    if((request->request_mode & CWWidth) && 
        (request->request_mode & CWHeight))
    {
    	/* 
    	 * If we are to be the same or bigger, say ok.
    	 */
    	if ( preferred->width <= request->width && 
            preferred->height <= request->height )
	{
      	    preferred->width  = request->width;
            preferred->height = request->height;
            return (XtGeometryYes);
        }
        /*
         * If both dimensions are unacceptable, say no.
         */
        else if ( preferred->width < request->width && 
            preferred->height < request->height )
                return (XtGeometryNo);
        /*
         * Otherwise one must be right, so say almost.
         */
        else
             return (XtGeometryAlmost);
    }
    /*
     * If only the width is requested, either it's 
     * OK or it isn't. Same for height.
     */
    else if ( request->request_mode & CWWidth )
    {
      if(preferred->width <= request->width)
      {
          preferred->width = request->width;
          return (XtGeometryYes);
      } 
      else
            return (XtGeometryNo);
    }
    else if ( request->request_mode & CWHeight )
    {
        if ( preferred->height <= request->height)
	{
             preferred->height = request->height;
             return (XtGeometryYes);
        }
        else
            return (XtGeometryNo);
    }

    return (XtGeometryYes);
}




static XtGeometryResult
GeometryManager(w, request, reply)
Widget                w;
XtWidgetGeometry      *request;
XtWidgetGeometry      *reply;
{
XgToolBarWidget  tb = (XgToolBarWidget) w->core.parent;
Mask             mask;
XtGeometryResult result;
Dimension        wdelta, hdelta;

    /*
     * Say no.  We control the vertical and horizontal....
     */
    if ( (request->request_mode & CWX && request->x != w->core.x) || 
	(request->request_mode & CWY && request->y != w->core.y) )
    	    return (XtGeometryNo);

    /*
     *  Otherwise, grant all requests if they fit.
     */
    if ( request->request_mode & (CWWidth | CWHeight | CWBorderWidth) )
    {
        /*
         * Save the original widget size, and set the 
         * corresponding widget fields to the requested sizes.
         */
        Dimension savewidth       = w->core.width;
        Dimension saveheight      = w->core.height;
        Dimension saveborderwidth = w->core.border_width;

        if (request->request_mode & CWWidth)
             w->core.width  = request->width;

        if (request->request_mode & CWHeight)
             w->core.height = request->height;
    
	if (request->request_mode & CWBorderWidth)
            w->core.border_width = request->border_width;
        /*
         * See if we can still handle all the children 
         * if the request is granted.
         */
        result = TryLayout(tb, &mask, &wdelta, &hdelta);

        /*
         * If the children won't fit, restore the widget to its
         * original size, and return no.
         */
        if ( result == XtGeometryNo )
	{
             w->core.width  = savewidth;
             w->core.height = saveheight;
             w->core.border_width = saveborderwidth;
             return (XtGeometryNo);
        }
    
	/*
         * If only one dimension fits, restore the one that
         * doesn't fit and return "almost".
         */
        if ( result == XtGeometryAlmost ) 
	{    
             reply->request_mode = request->request_mode;
             if ( !(mask & CWWidth) )
	     {
                 reply->width = w->core.width = savewidth;
                 reply->border_width  = saveborderwidth;
                 w->core.border_width = saveborderwidth;
             }
      
            if ( !(mask & CWHeight) )
                 reply->height = w->core.height = saveheight;
     
            return (XtGeometryAlmost);  
        }
        /*
         *  If we got here, everything must fit, so reposition
         *  all children based on the new size, and return "yes".
         */
        DoLayout(tb);
    
        return (XtGeometryYes); 
    }
  
    return (XtGeometryYes);
}




static XtGeometryResult
TryLayout(parent, mask, w_delta, h_delta)
XgToolBarWidget parent;
Mask       *mask;
Dimension  *w_delta, *h_delta;
{
int         i;
int	    num_managed = 0;
Dimension   total_width = 0, 
            total_height = 0;

    /*
     * Get the bounding width and height of all children.
     */
    for ( i = 0; i < parent -> composite.num_children; i++ )
    {
        Widget    child;
        Dimension width, height;

        child  = parent -> composite.children[i];
        if(child->core.managed)
        {
	    num_managed++;
            height =child->core.height + child->core.border_width * 2;
            width  =child->core.width + child->core.border_width * 2;
            if ( TOOLBARDIR(parent) == XmHORIZONTAL )
            {
                total_width += width;
                total_height = MAX(total_height, height);
            }
            else
            {
                total_width = MAX(total_width, width);
                total_height += height;
            }
        }
    }

    if ( num_managed > 1 )
    {
        if ( TOOLBARDIR(parent) == XmHORIZONTAL )
      	    total_width += (num_managed - 1) * parent->toolbar.spacing;
        else
            total_height += (num_managed - 1) * parent->toolbar.spacing;
    }


    /*
     *  If everyone doesn't fit, ask if we can grow. Return the 
     *  result, after setting the mask to indicate which (if 
     *  any) dimension is ok.
     */

    if(total_width > parent->core.width || total_height > parent->core.height)
    {
        XtGeometryResult result;
        Dimension replyWidth, replyHeight;
        Dimension width  =  MAX(total_width, parent->core.width);
        Dimension height = MAX(total_height, parent->core.height);

        result = XtMakeResizeRequest ((Widget)parent, width, height,
                                 &replyWidth, &replyHeight);

        *mask = (Mask)NULL;
        if(total_width == replyWidth)
            *mask  = CWWidth;
        if(total_height == replyHeight)     
            *mask |= CWHeight;

        if(result == XtGeometryAlmost)
	{
            XtMakeResizeRequest ((Widget)parent, replyWidth, replyHeight, 
                NULL, NULL);
	}

        *w_delta = total_width - parent->core.width;
        *h_delta = total_height - parent->core.height;
        return (result);
    }

    /*
     * If everybody fits, just return yes.
     */
    *mask = CWWidth | CWHeight;
    return (XtGeometryYes);
}




static void
ChangeManaged(w)
XgToolBarWidget w; 
{
XtGeometryResult result;
Dimension        width, height, delta;
int              i;
Mask             mask;
Widget           child;

    /*
     * See if all children fit.
     */
    result = TryLayout(w, &mask, &width, &height);
    
    /*
     * If they don't, resize all children to be smaller.
     */
    if(result != XtGeometryYes) 
    {
        if(w->composite.num_children > 0)
        {
            if ( TOOLBARDIR(w) == XmHORIZONTAL )
            {
        	delta = width / w->composite.num_children;

        	for ( i=0; i < w->composite.num_children; i++ )
        	{
          	    child = w->composite.children[i];
          	    height = MIN(child->core.height,
            	    w->core.height - child->core.border_width);
          	    if ( child->core.managed )
            		XtResizeWidget(child, child->core.width - delta,
              			height, child->core.border_width);
        	}
            }
      	    else
      	    {
        	delta = height / w->composite.num_children;

        	for ( i = 0; i < w->composite.num_children; i++ )
        	{
          	    child = w->composite.children[i];
          	    width = MIN(child->core.width,
            	    w->core.width - child->core.border_width);
          	    if ( child->core.managed )
            		XtResizeWidget(child, width, child->core.height - 
				delta, child->core.border_width);
                }
            }
        }
    }
    
    /*
     * Move all children to their new positions.
     */
    DoLayout(w);
}




void
XgToolBarSetClones(toolbar, names, count)
Widget toolbar;
String *names;
int count;
{
int	i;
Widget	shell, sibling, clone;
String  my_names = NULL;

	if ( !XgIsToolBar(toolbar) )
		return;

	if ( count == 0 )
	{
		if ( names == NULL )
			return;

		my_names = XtNewString(names[0]);			

		/*
		 * Parse the string into strings
		 */
	

	}

	/*
	 * Loop thru each name, find its widget pointer then add it
	 * to the ToolBar
	 */	
	for ( i = 0; i < count; i++ )
 	{
	RegisteredWidget_t *registered_class;

		if ( strcmp("SPACER", names[i]) == 0 )
		{
			registered_class = &spacer_widget;
			sibling = NULL;
		}
		else
		{
	       		sibling = XgToolBarNameToWidget(names[i]);

		        if ( sibling == NULL )
			    continue;

	       		/*
		         * We have a sibling to clone so do it
		         */
	       		registered_class = GetRegisteredWidget(sibling);
		       	if ( registered_class == NULL )
		    		continue;
		}

	       	/*
	       	 * Clone the widget
	       	 */
	       	clone = registered_class->clone_proc(toolbar, sibling,
			registered_class->client_data);

		if ( clone != NULL )
		{
	        	AddWidgetToToolBar(clone, registered_class);
	        	XtManageChild(clone);
		}
	}
}



/*
 *
 */

void
XgToolBarGetClones(toolbar, names, count)
Widget toolbar;
String **names;
int *count;
{
int i;
String	*name_list;

	*count = 0;
	*names = NULL;

	if ( NUM_CLONES(toolbar) == 0 )
		return;

	*names = name_list = 
		(String *)XtMalloc(NUM_CLONES(toolbar) * sizeof(String));
	if ( name_list == NULL )
		return;

	for ( i = 0; i < NUM_CLONES(toolbar); i++ )
	{
	RegisteredWidget_t *registered_as;

	    registered_as = 
	        GetRegisteredWidget(CLONES(toolbar)[i].cloned_from);

	    if ( registered_as == NULL )
		continue;

	    name_list[*count] = registered_as->name;
	    (*count)++;
	}
}



void
XgToolBarSetDragCursor(w, cursor)
Widget w;
Cursor cursor;
{
	DRAG_CURSOR = cursor;
}



static Cursor
CreateDefaultDragCursor(w)
Widget w;
{
Pixmap 		cursor_pixmap, cursor_mask;
int 		maxwidth, maxheight;
unsigned int 	hot_x, hot_y;
Status 		sizeOk;
XColor 		foreRGB, backRGB;
Display 	*display = XtDisplay(w);
Colormap 	cmap;
Cursor		cursor;


	cmap = DefaultColormap(display, DefaultScreen(display));

	/*
	 *  First find out the maximum size of cursor can this server
	 *  takes.
	 */
	sizeOk = XQueryBestCursor(display, DefaultRootWindow(display),
		    btn31x31_width, btn31x31_height, 
		    (unsigned int *)&maxwidth, (unsigned int *)&maxheight);


	/*
	 *  If the maxwidth is smaller then sand2_width (large cursor width)
	 *  then we must use the small cursor.
	 */
	if ( !sizeOk || maxwidth < btn31x31_width || 
		maxheight < btn31x31_height )
	{
		cursor_pixmap = XCreateBitmapFromData(display,
			RootWindowOfScreen(XtScreen(w)), 
			(char *)btn15x15_bits,
			btn15x15_width, btn15x15_height);

		cursor_mask = XCreateBitmapFromData(display,
			RootWindowOfScreen(XtScreen(w)), 
			(char *)btn15x15_m_bits,
			btn15x15_m_width, btn15x15_m_height);

		hot_x = btn15x15_width / 2;
		hot_y = btn15x15_height / 2;
	}

	/*
	 *  Or else use the big one
	 */
	else
	{
		cursor_pixmap = XCreateBitmapFromData(display,
			RootWindowOfScreen(XtScreen(w)), 
			(char *)btn31x31_bits,
			btn31x31_width, btn31x31_height);

		cursor_mask = XCreateBitmapFromData(display,
			RootWindowOfScreen(XtScreen(w)), 
			(char *)btn31x31_m_bits,
			btn31x31_m_width, btn31x31_m_height);

		hot_x = btn31x31_width / 2;
		hot_y = btn31x31_height / 2;
	}


	/*
	 *  Get the XColor for the passed pixel values
	 */
	foreRGB.pixel = BlackPixelOfScreen(XtScreen(w));
	backRGB.pixel = WhitePixelOfScreen(XtScreen(w));
	XQueryColor(display, cmap, &foreRGB);
	XQueryColor(display, cmap, &backRGB);

	cursor = XCreatePixmapCursor(display, cursor_pixmap, cursor_mask,
		&foreRGB, &backRGB, hot_x, hot_y);


	/*
	 *  Free the Pixmaps (Once the cursor is created we don't need them)
	 */
	XFreePixmap(display, cursor_mask);
	XFreePixmap(display, cursor_pixmap);

	return cursor;
}


static void
RemoveWidgetFromToolBar(widget)
Widget widget;
{
int     i, found, num_children;
Widget  toolbar;

	if ( widget == NULL )
		return;

	toolbar = XtParent(widget);
	if ( !XgIsToolBar(toolbar) )
		return;

	XgToolBarUnregisterWidget(widget);


	/*
	 * Try and find this Widget in the clone list
	 */
	found = GetToolbarIndex(widget);
	if ( found == -1 )
		return;


	/*
	 * If this widget had a tool hint free it
	 */
	if ( CLONES(toolbar)[found].tool_hint != NULL )
	{
		XtRemoveEventHandler(widget,
			EnterWindowMask | LeaveWindowMask,
			False, ToolHintHandler,
			CLONES(toolbar)[found].tool_hint);

		XtFree(CLONES(toolbar)[found].tool_hint);
		CLONES(toolbar)[found].tool_hint = NULL;
	}


	/*
	 * Remove this widget
	 */
	NUM_CLONES(toolbar)--;
	for ( i = found; i <  NUM_CLONES(toolbar); i++ )
		CLONES(toolbar)[i] = CLONES(toolbar)[i + 1];

	/*
	 * Free or reallocate the toolbar clone list
	 */
	if ( NUM_CLONES(toolbar) == 0 )
	{
		XtFree((char *)CLONES(toolbar));
		NUM_CLONES(toolbar) = 0;
		CLONES(toolbar) = NULL;
	}
	else
		CLONES(toolbar) = (XgToolBarCloneDataStruct *)
			XtRealloc((char *)CLONES(toolbar), 
			NUM_CLONES(toolbar) * sizeof(XgToolBarCloneDataStruct));	
}


static void
InsertChild(w)
Widget w;
{
Widget toolbar;

    	(*((CompositeWidgetClass)(xgToolBarWidgetClass->
		core_class.superclass))->composite_class.insert_child)(w);

	/*
	 * Now go ahead and add this clone to the ToolBars list
	 * of cloned widget names
	 */
	toolbar = XtParent(w);
        CLONES(toolbar) = (XgToolBarCloneDataStruct *)XtRealloc(
                (char *) CLONES(toolbar),
                (NUM_CLONES(toolbar) + 1) * sizeof(XgToolBarCloneDataStruct));

        if ( CLONES(toolbar) != NULL )
        {
	    int num_clones = NUM_CLONES(toolbar);

            CLONES(toolbar)[num_clones].cloned_from = NULL;
            CLONES(toolbar)[num_clones].self = w;
	    CLONES(toolbar)[num_clones].tool_hint = NULL;
            NUM_CLONES(toolbar)++;
        }
}


#ifndef _NO_PROTO
Boolean XgToolBarGetCloneData(Widget toolbar,
        XgToolBarCloneDataStruct ** data, int *num_clones)
#else
Boolean XgToolBarGetCloneData(toolbar, data, num_clones)
Widget toolbar;
XgToolBarCloneDataStruct ** data;
int *num_clones;
#endif
{

	if ( !XgIsToolBar(toolbar) )
		return False;

	*num_clones = NUM_CLONES(toolbar);
	*data = CLONES(toolbar);
	
	return True;	
}


static void
AddWidgetToToolBar(clone, registered_widget)
Widget clone;
RegisteredWidget_t *registered_widget;
{
Widget  toolbar, parent;
Widget	cloned_from = NULL;
int	found;


	/*
	 * If the sibling is a toolbar then just copy its cloned_from widget
	 */
	cloned_from = registered_widget->widget;


	/*
	 * Make sure this widget is already on the toolbar
	 */
	found = GetToolbarIndex(clone);
	if ( found < 0 )
		return;

	toolbar = XtParent(clone);
        CLONES(toolbar)[found].cloned_from = cloned_from;

        if ( registered_widget->tool_hint != NULL )
	    	CLONES(toolbar)[found].tool_hint = 
			XtNewString(registered_widget->tool_hint);
	else
	   	CLONES(toolbar)[found].tool_hint = NULL;

    	/*
	 * Add the event handler for the tool hint if we have one
	 */
	if ( CLONES(toolbar)[found].tool_hint != NULL )
		XtAddEventHandler(clone,
			EnterWindowMask | LeaveWindowMask,
			False, ToolHintHandler, (XtPointer)
			CLONES(toolbar)[found].tool_hint);

        /*
         * Add the Tool's actions
         */
        XtAppAddActions(XtWidgetToApplicationContext(clone),
                    toolActionsTable,
		    XtNumber(toolActionsTable));

        /*
         * Now overwrite the widgets translations with our translations
         */
        XtOverrideTranslations(clone, toolTranslations);
}


static void
ToolHintHandler(w, client_data, event)
Widget w;
XtPointer client_data;
XEvent *event;
{
Widget toolbar = XtParent(w);
XtAppContext app;
Boolean bInstallButtonHandler = True;

	if ( !XgIsToolBar(toolbar) )
		return;
	
	/*
	 * Delete the tool hint timeout if one was created
	 */
	if ( HINT_TIMER(toolbar) != None )
	{
		XtRemoveTimeOut(HINT_TIMER(toolbar));
		HINT_TIMER(toolbar) = None;

		bInstallButtonHandler = False;
	}


	/*
	 * On enter setup the timer to popup the tool hint
	 */
	if ( event->type == EnterNotify )
	{
	static ToolBarHintData_t data;

		if ( !HINTS_ENABLED(toolbar) )
			return;

		data.tool_hint = (String)client_data;
		data.tool = w;

		app = XtWidgetToApplicationContext(w);

		HINT_TIMER(toolbar) = XtAppAddTimeOut(app, 
			HINT_DELAY(toolbar), ToolHintTimeout, 
			(XtPointer)&data);

		if ( bInstallButtonHandler )
	        	XtAddEventHandler(w, ButtonPressMask, 
				False, AbortToolHint, NULL);
	}
	else if ( event->type == LeaveNotify )
	{
		if ( HINTS_ENABLED(toolbar) )
			AbortToolHint(w, client_data, event);
	}
}




static void
ToolHintTimeout(client_data, id)
XtPointer client_data;
XtIntervalId *id;
{
ToolBarHintData_t *data = (ToolBarHintData_t *)client_data;
Widget toolbar, popup;
Position root_x, root_y, x, y;
Dimension height, label_height, label_width, disp_width, disp_height;
Display *display;

	/*
	 * Clear the timer id for the hint
	 */
	toolbar = XtParent(data->tool);
	HINT_TIMER(toolbar) = None;


	/*
	 * Create the tool hint window for this toolbar
	 */
	if ( HINT_WINDOW(toolbar) == NULL )
	{
		popup = XtVaAppCreateShell(NULL, NULL,
			applicationShellWidgetClass,
			XtDisplay(toolbar),
			XmNoverrideRedirect, True,
			XmNforeground, HINT_FOREGROUND(toolbar),
			XmNbackground, HINT_BACKGROUND(toolbar),
			XmNborderWidth, 1, 
			XmNborderColor, HINT_FOREGROUND(toolbar), 
			NULL);

		HINT_WINDOW(toolbar) = XtVaCreateWidget("toolHint",
			xmLabelWidgetClass, popup,
			XmNrecomputeSize, True,
			XmNtraversalOn, False,
			XmNforeground, HINT_FOREGROUND(toolbar),
			XmNbackground, HINT_BACKGROUND(toolbar),
			XmNborderWidth, 1, 
			XmNborderColor, HINT_FOREGROUND(toolbar), 
			XmNfontList, HINT_FONTLIST(toolbar),
			XmNalignment, XmALIGNMENT_BEGINNING,
			NULL);
	}
	else
		popup = XtParent(HINT_WINDOW(toolbar));


	XtVaSetValues(HINT_WINDOW(toolbar),
		RES_CONVERT(XmNlabelString, data->tool_hint), NULL);
	XtVaGetValues(HINT_WINDOW(toolbar),
		XmNwidth, &label_width, XmNheight, &label_height, NULL);

	XtVaGetValues(data->tool, XmNheight, &height, NULL);
	XtTranslateCoords(data->tool, 0, 0, &root_x, &root_y);

	display = XtDisplay(toolbar);
	disp_width  = DisplayWidth(display, DefaultScreen(display));
	disp_height = DisplayHeight(display, DefaultScreen(display));

	x = root_x + 1;
	if ( x + label_width > disp_width )
		x = disp_width - label_width;

	y = root_y + height + 1;
	if ( y + label_height > disp_height )
		y = disp_height - label_height;
		
	XtVaSetValues (popup,
		XmNx, x,
		XmNy, y,
		XmNwidth, label_width,
		XmNheight, label_height,
		NULL);

	/*
	 * Popup the tool hint
	 */
	XtManageChild(HINT_WINDOW(toolbar));
	XtPopup(popup, XtGrabNone);
}


static void
AbortToolHint(w, client_data, event)
Widget w;
XtPointer client_data;
XEvent *event;
{
Widget toolbar = XtParent(w);

	/*
	 * Abort the tool hint delay if the timer was installed
	 */
	if ( HINT_TIMER(toolbar) != None )
	{
		XtRemoveTimeOut(HINT_TIMER(toolbar));	
		HINT_TIMER(toolbar) = None;

        	XtRemoveEventHandler(w, ButtonPressMask, 
			False, (XtEventHandler)AbortToolHint, NULL);
	}

	/*
	 * And popdown the tool hint if it was poped up
	 */
	if ( HINT_WINDOW(toolbar) != NULL )
	{
		if ( XtIsManaged(HINT_WINDOW(toolbar)) )
		{
			XtPopdown(XtParent(HINT_WINDOW(toolbar)));
			XtUnmanageChild(HINT_WINDOW(toolbar));
		}
	}
}


static void
DropTimeout(client_data, id)
XtPointer client_data;
XtIntervalId *id;
{
	if ( DROP_WIDGET == NULL )
		return;

	DROP_WIDGET = NULL;

	if ( DROP_TIMER_ID != None )
	{
		XtRemoveTimeOut(DROP_TIMER_ID);
		DROP_TIMER_ID = None;
	}

        XtRemoveEventHandler((Widget)TOOLBAR_LIST[0], ButtonReleaseMask,
                False, DropWidget, NULL);    

	XtUngrabPointer((Widget)TOOLBAR_LIST[0], CurrentTime);
}



static void
DropWidget(w, client_data, event)
Widget w;
XtPointer client_data;
XEvent *event;
{
Widget	    drop_widget = DROP_WIDGET;
Widget      toolbar, clone, cloned_from;
Pixmap	    pixmapLabel;
XmString    stringLabel;
int	    i, found;
Window	    win = ((XButtonEvent *)event)->window;
Position    x = ((XButtonEvent *)event)->x_root;
Position    y = ((XButtonEvent *)event)->y_root;
RegisteredWidget_t *registered_class;


	/*
	 * Make sure we have a widget to add to the selected toolbar
	 */
	if ( DROP_WIDGET == NULL )
		return;


	/*
	 * Remove the drop handler and the pointer grab
	 */
	XtRemoveEventHandler((Widget)TOOLBAR_LIST[0], ButtonReleaseMask,
                False, (XtEventHandler)DropWidget, NULL);

	XtUngrabPointer((Widget)TOOLBAR_LIST[0], CurrentTime);
	XFlush(XtDisplay(w));


	/*
	 * Remove the drop timeout
	 */
	if ( DROP_TIMER_ID != None )
	{
		XtRemoveTimeOut(DROP_TIMER_ID);
		DROP_TIMER_ID = None;
	}

	DROP_WIDGET = NULL;


	/*
	 * We've got this far, we better have a valid widget to drop
	 * onto a toolbar, make sure the widget was dropped on a
	 * ToolBar widget. We do this by checking the X/Y point of
	 * the widget drop againts the location and size of each ToolBar
	 */
	toolbar = NULL;
	for ( i = 0; i < NUM_TOOLBARS && toolbar == NULL; i++ )
	{
	Position	root_x, root_y;
	Dimension	width, height;

		XtVaGetValues((Widget)TOOLBAR_LIST[i],
			XmNwidth, &width, XmNheight, &height, NULL);

		XtTranslateCoords((Widget)TOOLBAR_LIST[i], 0, 0,
			&root_x, &root_y);

		if ( x >= root_x && x < root_x + width &&
		    y >= root_y && y < root_y + height )
			toolbar = (Widget)TOOLBAR_LIST[i];
	}


	/*
	 * Wasn't dropped in a ToolBar nothing to do here
	 */
	if ( toolbar == NULL )
	{
		/*
 		 * If the widget was on a toolbar and the user draged it
		 * off, delete the widget
		 */
		if ( XgIsToolBar(XtParent(drop_widget)) )
		{
			RemoveWidgetFromToolBar(drop_widget);
			XtDestroyWidget(drop_widget);
		}

		return;
	}

	/*
	 * We have a ToolBar to put the dropped widget on. Get the
	 * cloning procedure for this widget, first look for one
	 * specific to the widget, if none found look for one for
	 * the widget's class
	 */
	found = GetToolbarIndex(drop_widget);
	if ( found >= 0 )
	{
		Widget from_toolbar = XtParent(drop_widget);
		cloned_from = CLONES(from_toolbar)[found].cloned_from;
	}
	else
		cloned_from = drop_widget;

	registered_class = GetRegisteredWidget(cloned_from);

	if ( registered_class == NULL )
		return;

	/*
	 * Clone the widget
	 */
	clone = registered_class->clone_proc(toolbar, 
		registered_class->widget,
		registered_class->client_data);

	if ( clone != NULL )
	{
		AddWidgetToToolBar(clone, registered_class);
		XtManageChild(clone);
	}


	/*
 	 * If the widget was on a toolbar then the user must have moved it,
	 * so delete the sibling
	 */
	if ( XgIsToolBar(XtParent(drop_widget)) )
	{
		RemoveWidgetFromToolBar(drop_widget);
		XtDestroyWidget(drop_widget);
	}
}



void
XgToolBarUnregisterWidget(widget)
Widget widget;
{
int 	i, found;

	if ( widget == NULL )
		return;

	/*
	 * Try and find this Widget in the registered list
	 */
	for ( i = 0, found = -1; i < NUM_REGISTERED && found == -1; i++ )
		if ( REGISTERED_WIDGETS[i].widget == widget )
			found = i;
	if ( found == -1 )
		return;

	if ( REGISTERED_WIDGETS[found].name != NULL )
		XtFree( REGISTERED_WIDGETS[found].name);

	if ( REGISTERED_WIDGETS[found].tool_hint != NULL )
		XtFree( REGISTERED_WIDGETS[found].tool_hint);


	/*
	 * Remove this widget
	 */
	NUM_REGISTERED--;
	for ( i = found; i < NUM_REGISTERED; i++ )
		REGISTERED_WIDGETS[i] = REGISTERED_WIDGETS[i + 1];

	/*
	 * Free or reallocate the toolbar list
	 */
	if ( NUM_REGISTERED == 0 )
	{
		XtFree((char *)REGISTERED_WIDGETS);
		REGISTERED_WIDGETS = NULL;
	}
	else
		REGISTERED_WIDGETS = (RegisteredWidget_t *)
			XtRealloc((char *)REGISTERED_WIDGETS, 
			(NUM_REGISTERED) * sizeof(RegisteredWidget_t));	
}


#ifndef _NO_PROTO
void XgToolBarSetToolHint(Widget widget, String hint)
#else
void XgToolBarSetToolHint(widget, hint)
Widget widget;
String hint;
#endif
{
int i, found;
Widget toolbar;
String old_hint;

	if ( widget == NULL )
		return;

	/*
	 * Try and find this Widget in the registered list
	 */
	for ( i = 0, found = -1; i < NUM_REGISTERED && found == -1; i++ )
		if ( REGISTERED_WIDGETS[i].widget == widget )
			found = i;

	if ( found >= 0 )
	{
	    if ( REGISTERED_WIDGETS[found].tool_hint != NULL )
	    {
		    XtFree(REGISTERED_WIDGETS[found].tool_hint);
		    REGISTERED_WIDGETS[found].tool_hint = NULL;
	    }

	    if ( hint != NULL )
		REGISTERED_WIDGETS[found].tool_hint = XtNewString(hint);
	}
	else
	{
	    if ( hint == NULL ) 
		return;
	}

	/*
	 * Try and find this Widget in the clone list
	 */
	found = GetToolbarIndex(widget);
	if ( found == -1 )
		return;

	toolbar = XtParent(widget);
	
	/*
	 * If this widget had a tool hint free it
	 */
	if ( CLONES(toolbar)[found].tool_hint != NULL )
	{
		XtRemoveEventHandler(widget,
			EnterWindowMask | LeaveWindowMask,
			False, (XtEventHandler)ToolHintHandler,
			CLONES(toolbar)[found].tool_hint);

		XtFree(CLONES(toolbar)[found].tool_hint);
	}


	/*
	 * Setup an event handler to display the tip
	 */
	CLONES(toolbar)[found].tool_hint = XtNewString(hint);
	XtAddEventHandler(widget,
		EnterWindowMask | LeaveWindowMask,
		False, (XtEventHandler)ToolHintHandler,
		CLONES(toolbar)[found].tool_hint);
}
		


void
#ifndef _NO_PROTO
XgToolBarRegisterWidget(Widget widget, 
	Widget (*clone_proc)(Widget, Widget, void *), XtPointer client_data)
#else
XgToolBarRegisterWidget(widget, clone_proc, client_data)
Widget widget;
Widget (*clone_proc)();
XtPointer client_data;
#endif
{
	/*
	 * Make sure the ToolBar class has been initialized
	 */
	XtInitializeWidgetClass(xgToolBarWidgetClass);


	/*
	 * First see if this class has already been added, if so
	 * just return without doing anything
	 */
	if ( XgToolBarWidgetIsRegistered(widget) )
		return;


        /*
         * Add the Tool's actions
         */
        XtAppAddActions(XtWidgetToApplicationContext(widget),
                toolActionsTable,
		XtNumber(toolActionsTable));

        /*
         * Now overwrite the widgets translations with our translations
         */
	XtOverrideTranslations(widget, toolTranslations);


        /*
         * Add this class to the list of registered classes
         */
        REGISTERED_WIDGETS = (RegisteredWidget_t *)XtRealloc(
		(char *)REGISTERED_WIDGETS,
                (NUM_REGISTERED + 1) * sizeof(RegisteredWidget_t));

        if ( REGISTERED_WIDGETS != NULL )
        {
		REGISTERED_WIDGETS[NUM_REGISTERED].clone_proc = clone_proc;
		REGISTERED_WIDGETS[NUM_REGISTERED].widget = widget;
		REGISTERED_WIDGETS[NUM_REGISTERED].name = 
			XgWidgetToName(widget);
		REGISTERED_WIDGETS[NUM_REGISTERED].client_data = client_data;
		REGISTERED_WIDGETS[NUM_REGISTERED].tool_hint = NULL;
                NUM_REGISTERED++;
        }
}

static int
GetToolbarIndex(widget)
Widget widget;
{
int i, found;
Widget toolbar = XtParent(widget);

	if ( !XgIsToolBar(toolbar) )
		return -1;

	/*
	 * Try and find this Widget in the clone list
	 */
	for ( i = 0, found = -1; i < NUM_CLONES(toolbar) && found == -1; i++ )
	    if (  CLONES(toolbar)[i].self == widget )
		found = i;

	return found;
}

	
static RegisteredWidget_t *
GetRegisteredWidget(widget)
Widget widget;
{
int	i;

	if ( widget == NULL )
	{
		return &spacer_widget;
	}

	/*
	 * Look thru the list of registered widgets for a match
	 */
	for ( i = 0; i < NUM_REGISTERED; i++ )
		if ( widget == REGISTERED_WIDGETS[i].widget )
			return &REGISTERED_WIDGETS[i];

	return NULL;
}




Boolean
XgToolBarWidgetIsRegistered(widget)
Widget widget;
{
	return ( (GetRegisteredWidget(widget) == NULL) ? False : True );
}



Widget
XgToolBarSpacerCloner(parent, sibling, client_data)
Widget parent;
Widget sibling;
XtPointer client_data;
{
	Pixel bg;
	XtVaGetValues(parent, XmNbackground, &bg, 0);
	return XtVaCreateWidget("SPACER", widgetClass, parent,
		XmNwidth, SPACER_WIDTH,
		XmNbackground, bg,
		XmNborderWidth, 0,
		NULL);
}



Widget
XgToolBarDefaultCloner(parent, sibling, client_data)
Widget parent;
Widget sibling;
XtPointer client_data;
{
XmFontList	font_list = NULL;
unsigned char	alignment = XmALIGNMENT_CENTER;
unsigned char	label_type = XmSTRING;
XmString	label_string = NULL;
Pixmap		label_pixmap = XmUNSPECIFIED_PIXMAP;
Pixmap          arm_pixmap = XmUNSPECIFIED_PIXMAP;
Pixmap          insensitive_pixmap = XmUNSPECIFIED_PIXMAP;
Pixmap		tool_pixmap = None;
Dimension	width;
Dimension 	height;
Dimension	shadow_thickness;
XtCallbackList	activate_list = NULL;
XtCallbackList  arm_list = NULL;
XtCallbackList  help_list = NULL;
Boolean		fill_on_arm;
Pixel		arm_color;
Pixel		fg;
Pixel		bg;
Pixel		top_color;
Pixel		bottom_color;
Dimension	margin_width;
Dimension	margin_height;
Widget		sub_menu;
Widget		clone;


	/*
	 * Get some of the resources from the widget, we are
	 * assuming that the widget is a PushButton (or Cascade),
	 * that's why the following resources were selected
	 */
	XtVaGetValues(sibling, 
		XmNfontList, &font_list,
		XmNalignment, &alignment,
		XmNlabelType, &label_type,
		XmNlabelString, &label_string,
		XmNlabelPixmap, &label_pixmap,
		XmNarmPixmap, &arm_pixmap,
		XmNlabelInsensitivePixmap, &insensitive_pixmap,
		XgNtoolPixmap, &tool_pixmap,
		XmNshadowThickness, &shadow_thickness,
		XmNactivateCallback, &activate_list,
		XmNarmCallback, &arm_list,
		XmNhelpCallback, &help_list,
		XmNfillOnArm, &fill_on_arm,
		XmNarmColor, &arm_color,
		XmNforeground, &fg,
		XmNbackground, &bg,
		XmNtopShadowColor, &top_color,
		XmNbottomShadowColor, &bottom_color,
		XmNmarginWidth, &margin_width,
		XmNmarginHeight, &margin_height,
		XmNsubMenuId, &sub_menu,
		NULL);

	/*
	 * If a Tool pixmap resource was specified then use it
	 */
	if ( tool_pixmap != None && tool_pixmap != XmUNSPECIFIED_PIXMAP )
	{
		label_type = XmPIXMAP;
		label_pixmap = tool_pixmap;
	}

	/*
	 * Clone the sibling widget (let parent adopt it)
	 */
	clone =  XtVaCreateWidget(XtName(sibling), 
		    XtClass(sibling), parent,
		    XmNfontList, font_list,
		    XmNalignment, alignment,
		    XmNlabelType, label_type,
		    XmNlabelString, label_string,
		    XmNlabelPixmap, label_pixmap,
		    XmNarmPixmap, arm_pixmap,
		    XmNlabelInsensitivePixmap, insensitive_pixmap,
		    XmNshadowThickness, shadow_thickness,
		    XmNactivateCallback, activate_list,
		    XmNarmCallback, arm_list,
		    XmNhelpCallback, help_list,
		    XmNfillOnArm, fill_on_arm,
		    XmNarmColor, arm_color,
		    XmNforeground, fg,
		    XmNbackground, bg,
		    XmNtopShadowColor, top_color,
		    XmNbottomShadowColor, bottom_color,
		    XmNmarginWidth, margin_width,
		    XmNmarginHeight, margin_height,
		    XmNsubMenuId, sub_menu,
		    NULL);
}



int
_XgToolBarStartDrop(w, event)
Widget w;
XEvent *event;
{
XtAppContext	app = XtWidgetToApplicationContext(w);
Pixmap 		drag_cursor;
Widget		shell;

	/*
	 * Make sure we have some ToolBar widgets, if not get outa here
	 */
	if ( NUM_TOOLBARS  < 1 )
		return;


	/*
	 * Keep track of this widget so we can duplicate it later
	 */
	DROP_WIDGET = w;


	/*
	 * First thing to do is to set up a timeout on the widget drop
	 */
	if ( DROP_TIMER_ID != None )
	{
		XtRemoveTimeOut(DROP_TIMER_ID);
		DROP_TIMER_ID = None;
	}

	if ( DROP_TIMEOUT > 0 )
		DROP_TIMER_ID = XtAppAddTimeOut(app, DROP_TIMEOUT * 1000,
			DropTimeout, NULL);

	/*
	 * Get the cursor to use for the drag of the widget
	 */
	drag_cursor = None;
	XtVaGetValues(w, XgNdragCursor, &drag_cursor, NULL);

	if ( drag_cursor == None )
	{
		if ( DRAG_CURSOR == None )
			DRAG_CURSOR = CreateDefaultDragCursor(w);

		drag_cursor = DRAG_CURSOR;
	}

        XtAddEventHandler((Widget)TOOLBAR_LIST[0], ButtonReleaseMask, 
		False, (XtEventHandler)DropWidget, NULL);

	XtGrabPointer((Widget)TOOLBAR_LIST[0], False, 
		ButtonReleaseMask,
		GrabModeAsync, GrabModeAsync, 
		None, drag_cursor, CurrentTime);

}





static void
StartDrop(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
Widget	shell;
Boolean bOkToEdit;
int	i;

	/*
	 * Make sure we have a toolbar enabled for editting
	 */
	bOkToEdit = False;
	for ( i = 0; i < NUM_TOOLBARS && bOkToEdit == False; i++ )
	{
		XtVaGetValues((Widget)TOOLBAR_LIST[i],
			XmNeditable, &bOkToEdit, NULL);
	}

	if ( !bOkToEdit )
		return;



	/*
	 * Make sure this widget or it's class has been registered with the
	 * ToolBar class
	 */
	if ( !XgToolBarWidgetIsRegistered(w) && !XgIsToolBar(XtParent(w)) )
                return;

	/*
	 * We need to popdown the menu if the button is on a menu, 
	 * so the first thing we do is find the widget's shell
	 * and if it is a XmMenuShell we call the shell's 
	 * MenuShellPopdownDone action procedure to unpost the
	 * menu
	 */
	shell = w;
	while ( !XtIsShell(shell) )
	{
		shell = XtParent(shell);
		if ( shell == NULL )
			break;
	}

	/*
	 * See if the parent is an XmMenuShell, if so pop it down
	 */
	if ( shell != NULL )
		if ( XmIsMenuShell(shell) )
		{
			event->type = ButtonRelease;
			((XButtonEvent *)event)->x = 0;
			((XButtonEvent *)event)->y = 0;
			((XButtonEvent *)event)->state = Button1Mask;
			XtCallActionProc(w, "MenuShellPopdownOne", 
				event, NULL, 0);
		}


	/*
	 * Now we simply call _XgToolBarStartDrop() to monitor for
	 * a button drop on a tool bar
	 */
	_XgToolBarStartDrop((Widget)w, event);
}


Widget
XgToolBarNameToWidget(name)
String name;
{
int i;

	if ( name == NULL )
		return NULL;


	for ( i = 0; i < NUM_REGISTERED; i++ )
	{
	char *registered_name = (char *)REGISTERED_WIDGETS[i].name;
	char *tmp_name = (char *)name;


	    if ( registered_name == NULL )
		continue;

	    while ( *registered_name != '\0' && *tmp_name != '\0' )
	    {
		if ( *tmp_name != *registered_name )
		{
		    if ( (*tmp_name != '.' && *tmp_name != '*') ||
			(*registered_name != '.' && *registered_name != '*') )
			    break;
		}

		tmp_name++;
		registered_name++;

		if ( *registered_name == '\0' && *tmp_name == '\0' )
		    return REGISTERED_WIDGETS[i].widget;
	    }
	}

	return NULL;
}


