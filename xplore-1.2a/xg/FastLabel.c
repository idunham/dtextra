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
 *	Filename: FastLabel.c
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.2 07 Nov 1996
 *
 *	Description:
 *		This is the source file for a fast Motif Label. The
 *		widgets supports the rotation of the label by 90 degree
 *		increments;
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	GMM	08/27/93	Original file
 *	GMM	09/02/93	General cleanup
 *	GMM	07/10/95	Added rotation functionality (re-wrote to
 *				use XgDrawRotatedText())
 *
 *
 * ---FHDR--- */



/*
 * These includes are needed to describe the widget
 */
#include	<Xm/XmP.h>
#include	"FastLabelP.h"
#include	"x_rotate.h"



/*
 * Declaration of methods
 */
static void Initialize();
static Boolean SetValues();
static void GetValuesHook();
static XtGeometryResult QueryGeometry();
static void Destroy();
static void Resize();
static void ComputeSize();
static void RedrawPixmap();
static void resizePixmap();
static void Redisplay();
static void Realize();
static XFontStruct *GetFontStruct();



static XtResource resources[] = {
    { 
        XmNmarginWidth, XmCMarginWidth, XmRDimension, sizeof(Dimension),
	XtOffsetOf(XgFastLabelRec, fastlabel.margin_width),
	XmRImmediate, (XtPointer) 2
    },
    { 
    	XmNmarginHeight, XmCMarginHeight, XmRDimension, sizeof(Dimension),
	XtOffsetOf(XgFastLabelRec, fastlabel.margin_height),
	XmRImmediate, (XtPointer) 2
    },
    { 
    	XmNstring, XmCString, XmRString, sizeof(String),
	XtOffsetOf(XgFastLabelRec, fastlabel.label_string), 
	XmRImmediate, (XtPointer)NULL
    },
    { 
        XmNfontList, XmCFontList, XmRFontList,
        sizeof(((XgFastLabelRec*)NULL)->fastlabel.font_list),
	XtOffsetOf(XgFastLabelRec, fastlabel.font_list),
	XtRString, (XtPointer)XtDefaultFont
    },
    { 
    	XmNalignment, XmCAlignment, XmRAlignment, sizeof(unsigned char),
	XtOffsetOf(XgFastLabelRec, fastlabel.alignment),
	XmRImmediate, (XtPointer)XmALIGNMENT_CENTER
    },
    {
    	XgNrotation, XgCRotation, XmRInt, sizeof(int),
	XtOffsetOf(XgFastLabelRec, fastlabel.rotation),
	XmRImmediate, (XtPointer)0
    },
    {
    	XmNrecomputeSize, XmCRecomputeSize, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XgFastLabelRec, fastlabel.recompute_size),
	XmRImmediate, (XtPointer)True
    },
};

XgFastLabelClassRec xgFastLabelClassRec = {
    {
    /* core_class fields */
	/* superclass		*/ (WidgetClass) &xmPrimitiveClassRec,
	/* class_name		*/ "XgFastLabel",
	/* widget_size		*/ sizeof(XgFastLabelRec),
	/* class_initialize	*/ NULL,
	/* class_part_initialize*/ NULL,
	/* class_inited		*/ FALSE,
	/* initialize		*/ Initialize,
	/* initialize_hook	*/ NULL,
	/* realize		*/ XtInheritRealize,
	/* actions		*/ NULL,
	/* num_actions		*/ 0,
	/* resources		*/ resources,
	/* num_resources	*/ XtNumber(resources),
	/* xrm_class		*/ NULLQUARK,
	/* compress_motion	*/ TRUE,
	/* compress_exposure	*/ TRUE,
	/* compress_enterleave	*/ TRUE,
	/* visible_interest	*/ TRUE,
	/* destroy		*/ Destroy,
	/* resize		*/ Resize,
	/* expose		*/ Redisplay,
	/* set_values		*/ SetValues,
	/* set_values_hook	*/ NULL,
	/* set_values_almost	*/ XtInheritSetValuesAlmost,
	/* get_values_hook	*/ GetValuesHook,
	/* accept_focus		*/ NULL,
	/* version		*/ XtVersion,
	/* callback_private	*/ NULL,
	/* tm_table		*/ NULL,
	/* query_geometry	*/ QueryGeometry,
	/* display_accelerator	*/ NULL,
	/* extension		*/ NULL
    },

    /* Primitive Class Part */
    {
        ((XtWidgetProc) _XtInherit),           /* border highlight        */
        ((XtWidgetProc) _XtInherit),           /* border_unhighlight      */
        NULL,				       /* translations            */
        NULL,                                  /* arm and activate        */
        NULL,                                  /* synthetic resources     */
        0,                                     /* number of syn res       */
        NULL                                   /* extension               */
    },


    /* FastLabel fields */
    {
	0
    }
};


WidgetClass xgFastLabelWidgetClass = (WidgetClass) &xgFastLabelClassRec;


/* +++PHDR+++
 *
 * Function:	Initialize()
 *
 * Scope:	static
 *
 * Description:
 *	This function is the Initialize() method for the XgFastLabel widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * request	XgFastLabelWidget	Widget filled in from resources
 * new		XgFastLabelWidget	Copy of request widget that has
 *					  been potentialy altered by 
 *					  XgFastLabel's superclasses
 *
 *
 * Returns:	void
 *
 *
 * ---PHDR--- */

static void Initialize(request, new)
XgFastLabelWidget	request, new;
{
XGCValues	values;
XtGCMask	mask;
int		width, height;

	/*
	 * 
	 */
	if ( request->fastlabel.label_string != NULL )
	{
		new->fastlabel.my_string = new->fastlabel.label_string =
			XtNewString(request->fastlabel.label_string);
	}
	else
		new->fastlabel.my_string = NULL;

	/*
	 * Get the XFontStruct from the XmFontList
	 */
        new->fastlabel.font = GetFontStruct(new->fastlabel.font_list);


	/*
 	 * Set some realistic values for the size of the widget
	 */
	ComputeSize(new, &width, &height);
	if ( request->core.width == 0 )
		new->core.width = width;

	if ( request->core.height == 0 )
		new->core.height = height;

	/*
	 * Make a GC for the text
	 */
	mask = GCForeground | GCBackground | GCFont;
	values.foreground = new->primitive.foreground;
	values.background = new->core.background_pixel;
	values.font = new->fastlabel.font->fid;
	new->fastlabel.text_gc = XtGetGC((Widget)new, mask, &values);

	/*
	 * Recompute the size of the widget
	 */
	new->fastlabel.pix = None;
	Resize(new);
}


/* +++PHDR+++
 *
 * Function:	ComputeSize()
 *
 * Scope:	static
 *
 * Description:
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:
 *
 *
 * ---PHDR--- */

static void
ComputeSize(w, width, height)
XgFastLabelWidget	w;
int			*width;
int			*height;
{
int		text_width, text_height, extra_width, extra_height;
XFontStruct     *font = w->fastlabel.font;


	text_height = font->max_bounds.ascent + font->max_bounds.descent;

	extra_width = (w->fastlabel.margin_width + w->core.border_width) * 2;
	extra_height = (w->fastlabel.margin_height + w->core.border_width) * 2;

	if ( w->fastlabel.my_string != NULL )
	{
		text_width = XTextWidth(font, w->fastlabel.my_string,
			strlen(w->fastlabel.my_string));
	}
	else
		text_width = text_height;


	if ( w->fastlabel.rotation == 270 || w->fastlabel.rotation == 90 )
	{
	  	w->fastlabel.desired_pix_width = text_height;
		w->fastlabel.desired_pix_height = text_width;

	        *width = text_height + extra_width;
		*height = text_width + extra_height;

	}
	else
	{
		w->fastlabel.desired_pix_width = text_width;
		w->fastlabel.desired_pix_height = text_height;

		*width = text_width + extra_width;
		*height = text_height + extra_height;
	}
}


/* +++PHDR+++
 *
 * Function:	ResizePixmap()
 *
 * Scope:	static
 *
 * Description:
 *	This function 
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:	void
 *
 * ---PHDR--- */

static void
ResizePixmap(w)
XgFastLabelWidget	w;
{

	/*
	 * If the pixmap already exists and is the size needed then
	 * draw the string on it and get outa here
	 */
	 if ( w->fastlabel.pix == None ||
	 	w->fastlabel.pix_width != w->fastlabel.desired_pix_width ||
		w->fastlabel.pix_height != w->fastlabel.desired_pix_height )
	{
	Display *display = XtDisplay(w);

		/*
		 * Free the old pixmap
		 */
		if ( w->fastlabel.pix != None )
		{
			XFreePixmap(display, w->fastlabel.pix);
			w->fastlabel.pix = None;
		}

		w->fastlabel.pix_width = w->fastlabel.desired_pix_width;
		w->fastlabel.pix_height = w->fastlabel.desired_pix_height;

		w->fastlabel.pix = XCreatePixmap(
			display, DefaultRootWindow(display),
			w->fastlabel.pix_width, w->fastlabel.pix_height, 
			w->core.depth);
	}


	RedrawPixmap(w);
}


/* +++PHDR+++
 *
 * Function:	Resize()
 *
 * Scope:	static
 *
 * Description:
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:
 *
 *
 * ---PHDR--- */

static void
Resize(w)
XgFastLabelWidget	w;
{
int	height, width;


	/*
	 * Compute the deisired size
	 */
	ComputeSize(w, &width, &height);


	/*
	 * recompute_size is True then resize the widget to contain the
	 * string (at a minimum).
	 */
	if ( w->fastlabel.recompute_size == True )
	{
		/*
		 * Only change the size if it is currently smaller
		 */
		if ( w->core.width < width )
			w->core.width = width;

		if ( w->core.height < height )
			w->core.height = height;

		ResizePixmap(w);


		/*
		 * If the FastLabel is realized the resize the window
		 */
		if ( XtIsRealized((Widget)w) )
			XtResizeWindow((Widget)w);
	}
	else
		ResizePixmap(w);
}


/* +++PHDR+++
 *
 * Function:	QueryGeometry()
 *
 * Scope:	static
 *
 * Description:
 *	This function is called to query the preferred geometry of this
 *	instance.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:	XtGeometryResult
 *
 * ---PHDR--- */

static XtGeometryResult
QueryGeometry(w, proposed, desired)
XgFastLabelWidget w;
XtWidgetGeometry *proposed, *desired;
{
int	width, height;
#define IsBitSet(bit) (proposed->request_mode & bit)

	ComputeSize(w, &width, &height);

	desired->width = width;
	desired->height = height;
	desired->request_mode = CWWidth | CWHeight;

	if ( IsBitSet(CWWidth) && proposed->width == desired->width &&
		IsBitSet(CWHeight) && proposed->height == desired->height )
			return(XtGeometryYes);

	if ( desired->width == w->core.width && 
		desired->height == w->core.height )
			return(XtGeometryNo);

	return(XtGeometryAlmost);

#undef IsBitSet
}


/* +++PHDR+++
 *
 * Function:	GetValuesHook()
 *
 * Scope:	static
 *
 * Description:
 *	This function 
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:	void
 *
 * ---PHDR--- */

static void
GetValuesHook(w, args, num_args)
XgFastLabelWidget	w;
ArgList 		args;
Cardinal 		*num_args;
{
int	i;

    /*
     * We don't save a copy of the label_string or font_list.
     * If the user wants these, we get them from the label widget.
     */
    for ( i = 0; i < *num_args; i++ )
	if ( strcmp(args[i].name, XmNstring) == 0 )
		*((char **)args[i].value) = w->fastlabel.my_string;	
}


/* +++PHDR+++
 *
 * Function:	SetValues()
 *
 * Scope:	static	
 *
 * Description:
 *	SetValues() method for the XgFastLabel widget. 
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * currnet	Widget
 * request	Widget
 * new		Widget
 *
 *
 * Returns:	Boolean which indicates wether the widget needs to be redrawn.
 *
 *
 * ---PHDR--- */

static Boolean SetValues(cur, req, new)
XgFastLabelWidget cur, req, new;
{
Boolean			redraw = False, do_resize = False;

#define NOT_EQUAL(field)       (cur->field != new->field)

	/* 
	 * Check for changed label string
	 */
	if ( NOT_EQUAL(fastlabel.label_string) )
	{
	char	*ptr = new->fastlabel.label_string;
	int	len = -1, newlen = -1;

		if ( new->fastlabel.my_string != NULL )
			len = strlen(new->fastlabel.my_string);

		if ( ptr != NULL )
			newlen = strlen(ptr);
		

		if ( newlen > len  )
		{
			if ( new->fastlabel.my_string != NULL )
			{
				XtFree(new->fastlabel.my_string);
				new->fastlabel.my_string = NULL;
			}

			new->fastlabel.my_string = XtMalloc(newlen + 1);
		}

		if ( ptr != NULL && new->fastlabel.my_string != NULL )
			strcpy(new->fastlabel.my_string, ptr);
		else
			new->fastlabel.my_string = NULL;

		new->fastlabel.label_string = new->fastlabel.my_string;

		redraw = True;
		do_resize = True;
	}


	/*
	 * Check for changed alignment
	 */
	if ( NOT_EQUAL(fastlabel.alignment) )
		redraw = True;

	/*
	 * Check for changed Font List
	 */
	if ( NOT_EQUAL(fastlabel.font_list) ||
		NOT_EQUAL(primitive.foreground) ||
		NOT_EQUAL(core.background_pixel) )
	{
	XGCValues	values;
	XtGCMask	mask;

		/*
		 * Get the XFontStruct from the XmFontList
		 */
	        new->fastlabel.font = GetFontStruct(new->fastlabel.font_list);

		mask = GCForeground | GCBackground | GCFont;
		values.foreground = new->primitive.foreground;
		values.background = new->core.background_pixel;
		values.font = new->fastlabel.font->fid;
		XtReleaseGC((Widget)new, new->fastlabel.text_gc);
		new->fastlabel.text_gc = XtGetGC((Widget)new, mask, &values);

		redraw = True;
		do_resize = True;
	}

	/*
	 * Check for changed recompute size
	 */
	if ( NOT_EQUAL(fastlabel.recompute_size) ||
		NOT_EQUAL(fastlabel.rotation) )
			redraw = do_resize = True;

	if ( do_resize == True )
		Resize(new);

	if ( redraw == True && XtIsRealized((Widget)new) )
		Redisplay(new, NULL);

	return redraw;
}


/* +++PHDR+++
 *
 * Function:	Destroy()
 *
 * Scope:	static
 *
 * Description:
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:	void
 *
 *
 * ---PHDR--- */

static void 
Destroy(w)
XgFastLabelWidget	w;
{
	if ( w->fastlabel.my_string != NULL )
		XtFree(w->fastlabel.my_string);

	if ( w->fastlabel.text_gc != None )
		XtReleaseGC((Widget) w, w->fastlabel.text_gc);
}


/* +++PHDR+++
 *
 * Function:	Redisplay()
 *
 * Scope:	static
 *
 * Description:
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:
 *
 *
 * ---PHDR--- */

static void
Redisplay(w, event)
XgFastLabelWidget	w;
XExposeEvent		*event;
{
int		x, y, width, height;
Display		*display = XtDisplay((Widget)w);
Window		win = XtWindow((Widget)w);
XFontStruct	*font = w->fastlabel.font;
XGCValues	values;
XtGCMask	mask;


	/*
	 * No widget window then get outa here
	 */
	if ( win == None )
		return;


	/*
	 * Clear the widgets background 
	 */
	XClearWindow(display, win);


	/*
	 * If no label string then we're done here
	 */
	if ( w->fastlabel.my_string == NULL )
		return;


	/*
	 * Do all that's required is to copy the label pixmap 
	 * into the widget window
	 */
	width = w->fastlabel.pix_width;
	height = w->fastlabel.pix_height;

	if ( w->fastlabel.rotation == 90 )
	{
	    x = (w->core.width - width) / 2;

	    switch ( w->fastlabel.alignment )
	    {
	      case XmALIGNMENT_BEGINNING:
		    y = w->fastlabel.margin_width;
	    	    break;

	      case XmALIGNMENT_END:
		    y = w->core.height - height - w->core.border_width -
			w->fastlabel.margin_width;
		    break;

	      case XmALIGNMENT_CENTER:
	      default:
		    y = (w->core.height - height) / 2;
		    break;
	    }

	}
	else if ( w->fastlabel.rotation == 270 )
	{
	    x = (w->core.width - width) / 2;

	    switch ( w->fastlabel.alignment )
	    {
	      case XmALIGNMENT_END:
		    y = w->fastlabel.margin_width;
	    	    break;

	      case XmALIGNMENT_BEGINNING:
		    y = w->core.height - height - w->core.border_width -
			w->fastlabel.margin_width;
		    break;

	      case XmALIGNMENT_CENTER:
	      default:
		    y = (w->core.height - height) / 2;
		    break;
	    }

	}
	else
	{
	    y = (w->core.height - height) / 2;

	    switch ( w->fastlabel.alignment )
	    {
	      case XmALIGNMENT_BEGINNING:
		    x = w->fastlabel.margin_width;
	    	    break;

	      case XmALIGNMENT_END:
		    x = w->core.width - width - w->core.border_width -
			w->fastlabel.margin_width;
		    break;

	      case XmALIGNMENT_CENTER:
	      default:
		    x = (w->core.width - width) / 2;
		    break;
	    }
	}



	XCopyArea(display, w->fastlabel.pix, win, w->fastlabel.text_gc,
	    0, 0, w->fastlabel.pix_width,  w->fastlabel.pix_height, x, y);

	/*
	 * Flush the display to reduce flicker
	 */
	XmUpdateDisplay((Widget)w);
}


/* +++PHDR+++
 *
 * Function:	RedrawPixmap()
 *
 * Scope:	static
 *
 * Description:
 *	This function 
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:	void
 *
 * ---PHDR--- */

static void
RedrawPixmap(w)
XgFastLabelWidget	w;
{
Display		*display = XtDisplay((Widget)w);
XFontStruct	*font = w->fastlabel.font;

	/*
	 * If no label then we're done here
	 */
	if ( w->fastlabel.my_string == NULL )
		return;

	/*
	 * Draw the text in the pixmap, we'll copy it to the window later
	 */
	if ( w->fastlabel.rotation == 90 || w->fastlabel.rotation == 180 )
	{
		XgDrawRotatedText(display, 
			w->fastlabel.pix, w->fastlabel.text_gc,
			0, 0, XgALIGN_LEFT, w->fastlabel.rotation,
			w->fastlabel.my_string, -1);

	}
	else if ( w->fastlabel.rotation == 270 )
	{
		XgDrawRotatedText(display, 
			w->fastlabel.pix, w->fastlabel.text_gc,
			0, 0, XgALIGN_RIGHT, w->fastlabel.rotation,
			w->fastlabel.my_string, -1);

	}
	else
	{
		XgDrawRotatedText(display, 
			w->fastlabel.pix, w->fastlabel.text_gc,
			0, font->ascent, XgALIGN_LEFT, w->fastlabel.rotation,
			w->fastlabel.my_string, -1);
	}

}



Widget
XgCreateFastLabel(parent, name, al, ac)
Widget parent;
char *name;
ArgList al;
Cardinal ac;
{
	return XtCreateWidget(name, xgFastLabelWidgetClass, parent, al, ac);
}



/* +++PHDR+++
 *
 * Function:	GetFontStruct()
 *
 * Scope:	static
 *
 * Description:
 *	This function takes the passed XmFontList and returns a pointer
 *	to an XFontStruct.
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * font_list	XmFontList		XmFontList to get the XFontStruct from
 *
 *
 * Returns:	XFontStruct *
 *
 * ---PHDR--- */

static XFontStruct *
GetFontStruct(font_list)
XmFontList	font_list;
{
#if (XmVERSION == 1 && XmREVISION > 0) || XmVERSION > 1
XmFontContext		context;
XmStringCharSet		charset;
static XFontStruct	*font;

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


