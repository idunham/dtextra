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
#include    "RowP.h"

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#define ROWDIR(rw)  rw->row.orientation
#define ROWSTRETCH(rw)  rw->row.stretch
#define ROWALIGN(rw) rw->row.align_children
#define CHILD_SPACING(w, num_managed) \
	(w->row.spacing * ((num_managed > 0) ? num_managed - 1 : 0))

static void              Initialize();
static void              Resize();
static void              ChangeManaged();
static Boolean           SetValues();
static XtGeometryResult  GeometryManager();
static XtGeometryResult  GetPreferredSize();
static XtGeometryResult  TryLayout();
static void 		 LayoutRow();

static void Class_Initialize();
static Boolean CvtStringToAlign();

static XtResource resources[] = {
    { 
	XmNorientation, XmCOrientation, XmROrientation, sizeof(unsigned char),
	    XtOffsetOf(XgRowRec, row.orientation), 
	XmRImmediate, (XtPointer)XmHORIZONTAL
    },
    { 
	XgNhorizontalStretch, XgCStretch, XmRBoolean, sizeof(Boolean),
	    XtOffsetOf(XgRowRec, row.hstretch), 
	XmRImmediate, (XtPointer)False
    },
    { 
	XgNverticalStretch, XgCStretch, XmRBoolean, sizeof(Boolean),
	    XtOffsetOf(XgRowRec, row.vstretch), 
	XmRImmediate, (XtPointer)False
    },
    { 
	XmNspacing, XmCSpacing, XmRDimension, sizeof(Dimension),
	    XtOffsetOf(XgRowRec, row.spacing), 
	XmRImmediate, (XtPointer)2
    },
    { 
	XgNalignChildren, XgCAlign, XgRAlign, sizeof(XgAlign),
	    XtOffsetOf(XgRowRec, row.align_children), 
	XmRImmediate, (XtPointer)XgALIGN_CENTER 
    }
};


XgRowClassRec xgRowClassRec = {
  {
	/* core_class members      */
	(WidgetClass) &xmManagerClassRec,   /* superclass         */
	"XgRow",                            /* class_name         */
	sizeof(XgRowRec),                   /* widget_size        */
	Class_Initialize,                   /* class_initialize   */
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
	NULL,                               /* destroy            */
	Resize,                             /* resize             */
	NULL,                               /* expose             */
	(XtSetValuesFunc)SetValues,         /* set_values         */
	NULL,                               /* set_values_hook    */
	XtInheritSetValuesAlmost,           /* set_values_almost  */
	NULL,                               /* get_values_hook    */  
	NULL,                               /* accept_focus       */
	XtVersion,                          /* version            */  
	NULL,                               /* callback_private   */
	NULL,                               /* tm_table           */
	GetPreferredSize,                   /* query_geometry     */  
	NULL,                               /* display_accelerator*/
	NULL,                               /* extension          */
  },
  {
	/* composite_class members */
	GeometryManager,                    /* geometry_manager  */
	ChangeManaged,                      /* change_managed    */
	XtInheritInsertChild,               /* insert_child      */  
	XtInheritDeleteChild,               /* delete_child      */  
	NULL				    /* extension         */
  },
  { 
	/* constraint_class fields */
	NULL,                               /* resource list      */ 
	0,                                  /* num resources      */ 
	0,                                  /* constraint size    */    
	NULL,                               /* init proc          */    
	NULL,                               /* destroy proc       */
	NULL,                               /* set values proc    */ 
	NULL                                /* extension          */
  },
  { /* manager_class      */
	XtInheritTranslations,              /* translations       */    
	NULL,                               /* syn_resources      */    
	0,                                  /* num_syn_resources  */    
	NULL,                               /* syn_cont_resources */    
	0,                                  /* num_syn_cont_resources */
	XmInheritParentProcess,             /* parent_process     */    
	NULL                                /* extension          */
  }, 
  {
	/* Row class members */
	0,                                  /* empty        */  
  }
};

WidgetClass xgRowWidgetClass = (WidgetClass) &xgRowClassRec;


static void
Class_Initialize ()
{
	/*
	 * Add a type converter for the Align types
	 */
	XtSetTypeConverter(XmRString, XgRAlign,
	(XtTypeConverter)CvtStringToAlign, NULL, 0, XtCacheAll, NULL);
}



static void
Initialize (request, new)
	XgRowWidget request, new;
{
	/*
	 * Set a reasonable width and height
	 */
	if (request -> core.width <= 0)
		new -> core.width = 10;
  	if (request -> core.height <= 0)
		new -> core.height = 10;
} 


static Boolean SetValues (current, request, new)
	XgRowWidget    current, request, new;
{
Boolean redraw = False; 

#define NOT_EQUAL(field)       (current->field != new->field)

	/* 
	 * Verify validity of requested orientation
	 */
	if ( ROWDIR(new) != XmVERTICAL && ROWDIR(new) != XmHORIZONTAL )
	{
	    	XtWarning( "Orientation not valid, forcing to HORIZONTAL" );
	    	ROWDIR(new) = XmHORIZONTAL;
	}

	redraw = NOT_EQUAL(row.orientation) || 
	NOT_EQUAL(row.hstretch) || NOT_EQUAL(row.vstretch) ||
	NOT_EQUAL(row.align_children );

	if ( redraw )
	    	LayoutRow( new );

	return redraw;
}


static void 
Resize (w)
	XgRowWidget    w;
{
	 LayoutRow(w);
} 



static void
LayoutRow (parent)
	XgRowWidget parent;
{
Widget      child;
int         i;
Dimension   total_child_width = 0,
	    total_child_height = 0;
Dimension   stretchwidth, stretchheight;
Position    xpos = 0,
	    ypos = 0;
Dimension   xpad = 0,
	    ypad = 0;
int         n_managed_children = 0,
	    nx_children = 0,
	    ny_children = 0;

	/*
	 * Determine how many children are managed.
	 */
	for (i = 0; i < parent->composite.num_children; i++)
	{
	    	child = parent->composite.children[i];
	    	if( child->core.managed )
	        	n_managed_children++;
	}


	/* 
	 * Compute new dimensions for h and v stretching
	 */
	if ( ROWDIR(parent) == XmHORIZONTAL )
	     stretchwidth = (parent->core.width - 
		CHILD_SPACING(parent, n_managed_children)) /
		n_managed_children;
	else
	     stretchheight = (parent->core.height -
		CHILD_SPACING(parent, n_managed_children)) / 
		n_managed_children;


	/*
	 * Number of children stacked in each direction 
	 */
	if ( ROWDIR(parent) == XmHORIZONTAL )
	{
		nx_children = n_managed_children;
	    	ny_children = 1;
	}
	else
	{
	    	nx_children = 1;
	    	ny_children = n_managed_children;
	}

	/*
	 * Resize all children if stretched in either direction.
	 */
	if ( parent->row.hstretch  || parent->row.vstretch )
	{
	    Dimension   childwidth, childheight;

	    for (i = 0; i < parent -> composite.num_children; i++)
	    {
	        child = parent -> composite.children[i];
	        if (child->core.managed)
	        {
	       	    /*
	             * Stretching rule:
	             * Make the child fit the extents of its divided space
	             *
	             * This means that horizontal orientations have n x 1 cells
	             * and verticals have 1 x n cells.  We will have a state
	             * table consisting of 2x2=4 areas each according to the 
	             * stretching dimension (2) and widget orientation (2).
	             *
	             */

	            /*
		     * Original child dimensions 
		     */
	            childwidth = child->core.width + 
					child->core.border_width * 2;
	            childheight = child->core.height + 
					child->core.border_width * 2;


	            /* Compute new dimensions for h and v stretching */
	            if ( ROWDIR(parent) == XmHORIZONTAL )
	     	   	stretchheight = parent->core.height - 
				child->core.border_width * 2;
	            else
	                 stretchwidth = parent->core.width - 
				child->core.border_width * 2;


	            /* Only stretch along correct dimensions */
	            XtResizeWidget(child, 
	                parent->row.hstretch ? stretchwidth : childwidth,
	                parent->row.vstretch ? stretchheight : childheight,
	                child->core.border_width);
	          }
	    }
	}

	/*
	 * Compute the total width/height of all managed children 
	 */
	for (i = 0; i < parent -> composite.num_children; i++)
	{
	    child = parent -> composite.children[i];
	    if(child->core.managed)
	    {
	        total_child_width += child->core.width + 
	            child->core.border_width * 2;
	        total_child_height += child->core.height + 
	            child->core.border_width * 2;
	    }
	 }

	if ( ROWDIR(parent) == XmHORIZONTAL )
		total_child_width += CHILD_SPACING(parent, n_managed_children);
	else
		total_child_height += CHILD_SPACING(parent, n_managed_children);

   

	/*
	 *  Divide any remaining space by the number 
	 *  of children.
	 */
	if ( n_managed_children < 1 ) 
		return;

	/* 
	 * Center widgets
	 */
	if ( parent->core.width > total_child_width )
	{
	    xpad = (parent->core.width - total_child_width) / (nx_children + 1);
	    xpos += xpad;
	}

	if ( parent->core.height > total_child_height )
	{
	    ypad = (parent->core.height - total_child_height) / 
	        (ny_children + 1);
	    ypos += ypad;
	}


	/*
	 * Find axis to center children along 
	 */
	if ( ROWDIR(parent) == XmHORIZONTAL )
	{
		switch( ROWALIGN(parent) )
		{
	    	  case XgALIGN_TOP_LEFT:
			ypos = 0;
			break;

	    	  case XgALIGN_BOTTOM_RIGHT:
			ypos = parent->core.height;
			break;

	          case XgALIGN_CENTER:
	    	  default:
			ypos = (parent->core.height) / 2;
			break;
		}
	}
	else
	{
		switch( ROWALIGN(parent) )
		{
	    	  case XgALIGN_TOP_LEFT:
			xpos = 0;
			break;

	    	  case XgALIGN_BOTTOM_RIGHT:
			xpos = parent->core.width;
			break;

	    	  case XgALIGN_CENTER:
	    	  default:
			xpos = (parent->core.width) / 2;
			break;
		}
	}

	/*
	 * Position all children.
	 */

#define POSITION_CHILDS( hstatement, vstatement )		\
	for (i = 0; i < parent -> composite.num_children; i++)	\
	{							\
	   child = parent -> composite.children[i];		\
	   if(child->core.managed)				\
	   {							\
	      if ( ROWDIR(parent) == XmHORIZONTAL )		\
	      {							\
		hstatement;					\
		xpos += xpad + child->core.width +		\
		    child->core.border_width * 2 +		\
			parent->row.spacing;			\
	      }							\
	      else						\
	      {							\
		vstatement;					\
		ypos += ypad + child->core.height +		\
		    child->core.border_width * 2 +		\
			parent->row.spacing;			\
	      }							\
	   }							\
	}

	switch( ROWALIGN(parent) )
	{
	  case XgALIGN_TOP_LEFT:
	  	POSITION_CHILDS( 
			XtMoveWidget (child, xpos, ypos ),
			XtMoveWidget (child, xpos, ypos ))
	    	break;

	  case XgALIGN_BOTTOM_RIGHT:
	    	POSITION_CHILDS(
			XtMoveWidget (child, xpos, ypos - child->core.height -
		    		child->core.border_width*2 ),
			XtMoveWidget (child, xpos - child->core.width - 
		    		child->core.border_width*2, ypos))
	    	break;

	  case XgALIGN_CENTER:
	  default:
	    	POSITION_CHILDS(
			XtMoveWidget (child, xpos, 
				ypos - child->core.height/2 - 
				child->core.border_width ),
			XtMoveWidget (child, xpos - child->core.width/2 - 
		    		child->core.border_width, ypos))
	    	break;
	}
}


static XtGeometryResult 
GetPreferredSize (w, request, preferred)
	XgRowWidget w;
	XtWidgetGeometry *request, *preferred;
{
Widget child;
int i;
int n_managed_children = 0;
int y_add = 0;
int x_add = 0;

	/*
	 * If no changes are being made to width or 
	 * height, just agree. 
	 */
	if (!(request->request_mode & CWWidth) && 
		!(request->request_mode & CWHeight))
	{
		return (XtGeometryYes);
	}

	/*
	 * Calculate our minimum size.
	 */
	preferred->width  = 0;
	preferred->height = 0;       

	for (i = 0; i < w -> composite.num_children; i++)
	{
	    child = w->composite.children[i];
	    if (child->core.managed)
	    {
	  	n_managed_children++;
	  	if ( ROWDIR(w) == XmHORIZONTAL )
	  	{
	    	    preferred->width += child->core.width + 
	      		child->core.border_width * 2;
	    	    if (preferred->height < child->core.height + 
	      		child->core.border_width * 2)
		    {		    
	      	    		preferred->height = child->core.height +
	        			child->core.border_width * 2;
		    }
	  	}
	  	else
	  	{
	    	    preferred->height += child->core.height + 
	      		child->core.border_width * 2;
	    	    if (preferred->width < child->core.width + 
	      		child->core.border_width * 2)
		    {
			preferred->width = child->core.width +
	        		child->core.border_width * 2;
	  	    }
	        }
	    }
	}


	if ( ROWDIR(w) == XmHORIZONTAL )
		preferred->width += CHILD_SPACING(w, n_managed_children);
	else
		preferred->height += CHILD_SPACING(w, n_managed_children);

	preferred->request_mode = CWWidth | CWHeight;

	/* 
	 * If both width and height are requested.
	 */
	if ((request->request_mode & CWWidth) && 
		(request->request_mode & CWHeight))
	{
		/* 
		 * If we are to be the same or bigger, say ok.
		 */
		if (preferred->width <= request->width && 
	        	preferred->height <= request->height)
		{ 
	 		preferred->width  = request->width;
	  		preferred->height = request->height;
	  		return (XtGeometryYes);
		}
		/*
		 * If both dimensions are unacceptable, say no.
		 */
		else if (preferred->width < request->width && 
	   		preferred->height < request->height)
		{
	   		return (XtGeometryNo);
		}
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
	else if (request->request_mode & CWWidth)
	{
		if ( preferred->width <= request->width )
		{
	     		preferred->width = request->width;
	     		return (XtGeometryYes);
	  	} 
	  	else
	        	return (XtGeometryNo);
	}
	else if (request->request_mode & CWHeight)
	{
		if (preferred->height <= request->height)
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
GeometryManager (w, request, reply)
	Widget                  w;
	XtWidgetGeometry      *request;
	XtWidgetGeometry      *reply;
{
XgRowWidget      rw = (XgRowWidget) w -> core.parent;
Mask             mask;
XtGeometryResult result;
Dimension        wdelta, hdelta;

	/*
	 * Say no.  We control the vertical....
	 */
  	if ((request->request_mode & CWX && request->x != w->core.x) ||
	    (request->request_mode & CWY && request->y != w->core.y))
		return (XtGeometryNo);

	/*
	 *  Otherwise, grant all requests if they fit.
	 */
  	if (request->request_mode & (CWWidth | CWHeight | CWBorderWidth))
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
		result = TryLayout(rw, &mask, &wdelta, &hdelta);

		/*
		 * If the children won't fit, restore the widget to its
		 * original size, and return no.
		 */
		if (result == XtGeometryNo)
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
		if (result == XtGeometryAlmost)
		{
	  		reply->request_mode = request->request_mode;
	  		if (!(mask & CWWidth))
			{
	  			reply->width = w->core.width = savewidth;
	   			reply->border_width  = saveborderwidth;
	   			w->core.border_width = saveborderwidth;
	  		}
	  
			if (!(mask & CWHeight))
		    		reply->height = w->core.height = saveheight;
	 
	  		return (XtGeometryAlmost);  
		}

		/*
		 *  If we got here, everything must fit, so reposition
		 *  all children based on the new size, and return "yes".
		 */
		LayoutRow(rw);
		return (XtGeometryYes); 
  	}

	return (XtGeometryYes);
}


static XtGeometryResult 
TryLayout(parent, mask, w_delta, h_delta)
	 XgRowWidget parent;
	 Mask       *mask;
	 Dimension  *w_delta, *h_delta;
{
int         i;
int 	    num_managed = 0;
Dimension   total_width = 0, 
            total_height = 0;

	/*
	 * Get the bounding width and height of all children.
	*/
	for (i = 0; i < parent->composite.num_children; i++)
	{
	    Widget    child;
	    Dimension width, height;

	    child  = parent -> composite.children[i];
	    if(child->core.managed)
	    {
	        num_managed++;
	        height =child->core.height + child->core.border_width * 2;
	        width  =child->core.width + child->core.border_width * 2;
	        if ( ROWDIR(parent) == XmHORIZONTAL )
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


	if ( ROWDIR(parent) == XmHORIZONTAL )
	   	total_width += CHILD_SPACING(parent, num_managed);
   	else
	   	total_height += CHILD_SPACING(parent, num_managed);


	/*
	 *  If everyone doesn't fit, ask if we can grow. Return the 
	 *  result, after setting the mask to indicate which (if 
	 *  any) dimension is ok.
	 */
	if(total_width > parent->core.width || 
		total_height > parent->core.height)
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
ChangeManaged (w)
	 XgRowWidget w; 
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
	    if (w->composite.num_children > 0)
	    {
	  	if ( ROWDIR(w) == XmHORIZONTAL )
	  	{
	    	    delta = width / w->composite.num_children;

	            for ( i = 0;i < w->composite.num_children; i++)
	    	    {
	            	child = w->composite.children[i];
	      		height = MIN(child->core.height,
	        	w->core.height - child->core.border_width);
	      		if (child->core.managed)
	        		XtResizeWidget(child, 
					child->core.width - delta, height, 
					child->core.border_width);
	    	    }
	      	}
	  	else
	  	{
	    	    delta = height / w->composite.num_children;

	    	    for ( i = 0;i < w->composite.num_children; i++ )
	    	    {
	      		child = w->composite.children[i];
	      		width = MIN(child->core.width,
	        	w->core.width - child->core.border_width);
	      		if (child->core.managed)
	        		XtResizeWidget(child, width, 
					child->core.height - delta,
	          			child->core.border_width);
	    	    }
	  	}
	    }
	}

	/*
	 * Move all children to their new positions.
	 */
	LayoutRow(w);
}


/*
 * Compare two strings. The test string must NULL terminated. 
 * Leading and trailing whitespace in the in string is ignored.
 */

static Boolean
CompareStrings (in, test)
	String in, test;
{
	/*
	 * Strip leading whitespace off the in string.
	 */
	while ( isspace(*in) )
		in++;

	for ( ; *test != '\0' && !isspace(*in); test++, in++ )
	{
	char	c = *in;
	char	t = *test;

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
CvtStringToAlign (dpy, args, num_args, from, to)
	Display 	*dpy;
	XrmValuePtr	args;
	Cardinal	*num_args;
	XrmValuePtr	from;
	XrmValuePtr	to;
{
static XgAlign	align_type;

	if ( *num_args != 0 )
	{
	    	XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
	                "cvtStringToAlign", "wrongParameters",
	                "XgRow",
	                "String to Align conversion needs no arguments",
	                 NULL, NULL);
	}

	/*
	 * User didn't provide enough space
	 */
	if ( to->addr != NULL && to->size < sizeof(XgAlign)  )
	{
		to->size = sizeof(XgAlign);
		return False;
	}

	/*
	 * Look for a match with the different types of Alignment
	 */ 
	if ( CompareStrings(from->addr, "top_left") )
		align_type = XgALIGN_TOP_LEFT;
	else if ( CompareStrings(from->addr, "center") )
		align_type = XgALIGN_CENTER;
	else if ( CompareStrings(from->addr, "bottom_right") )
		align_type = XgALIGN_BOTTOM_RIGHT;
	else 
	{
		XtDisplayStringConversionWarning(dpy, from->addr, XgRAlign);
		return False;
	}

	/*
	 * Store our return value
	 */
	if ( to->addr == NULL )
		to->addr = (caddr_t) &align_type;
	else
		*(XgAlign *) to->addr = align_type;

	to->size = sizeof(XgAlign);
	return True;
}

