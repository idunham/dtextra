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
 *	Filename: ExtPushB.c
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.4 07 Nov 1996
 *
 *	Description:
 *		This is the source file for an Extended Motif Push Button
 *		widget.
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	GMM	07/06/94	Original file
 *
 *
 * ---FHDR--- */


#include	<stdio.h>

/*
 * These includes are needed to describe the widget
 */
#include 	<Xm/XmP.h>
#include 	"ExtPushBP.h"


#include "xgpm.h"



/*
 * Declaration of methods
 */
static void Class_Initialize();
static void Destroy();
static void Initialize();
static Boolean SetValues();


/*
 * Internal use functions and callbacks
 */
static void SetButtonPixmap();
static void SetButtonBitmap();
static void SetButtonLabel();
static XFontStruct *GetFontStruct();




/* 
 * 4 x 4 stipple for desensitized widget 
 */
#define stipple_width  4
#define stipple_height 4
static char stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05};
#define STIPPLE_BITMAP \
	xgextPushButtonClassRec.extpushbutton_class.stipple_bitmap



static XtResource resources[] = {

    { 
        XgNextLabelType, XgCExtLabelType, XmRInt, sizeof(int),
	XtOffsetOf(XgExtPushButtonRec, extpushbutton.label_type), 
	XmRImmediate, (XtPointer)XgSTRING
    },
    { 
    	XgNextPixmap, XgCExtPixmap, XmRString, sizeof(String),
	XtOffsetOf(XgExtPushButtonRec, extpushbutton.label_pixmap), 
	XmRImmediate, (XtPointer)NULL
    },
    { 
    	XgNextString, XgCExtString, XmRString, sizeof(String),
	XtOffsetOf(XgExtPushButtonRec, extpushbutton.label_string),
	XmRImmediate, (XtPointer)NULL
    },
    { 
    	XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension),
	XtOffsetOf(XgExtPushButtonRec, extpushbutton.spacing),
	XmRImmediate, (XtPointer)2
    },
    { 
    	XgNshift, XgCShift, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XgExtPushButtonRec, extpushbutton.shift), 
	XmRImmediate, (XtPointer)True
    },
    {
        XgNlabelLocation, XgCLocation, XmRInt, sizeof(int),
        XtOffsetOf(XgExtPushButtonRec, extpushbutton.label_location),
        XtRImmediate, (XtPointer)XgRIGHT
    }
};


XgExtPushButtonClassRec xgextPushButtonClassRec = {
    {
    /* core_class fields */
	/* superclass		*/ (WidgetClass) &xmPushButtonClassRec,
	/* class_name		*/ "XgExtPushButton",
	/* widget_size		*/ sizeof(XgExtPushButtonRec),
	/* class_initialize	*/ Class_Initialize,
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
	/* resize		*/ XtInheritResize,
	/* expose		*/ XtInheritExpose,
	/* set_values		*/ SetValues,
	/* set_values_hook	*/ NULL,
	/* set_values_almost	*/ XtInheritSetValuesAlmost,
	/* get_values_hook	*/ NULL,
	/* accept_focus		*/ XtInheritAcceptFocus,
	/* version		*/ XtVersion,
	/* callback_private	*/ NULL,
	/* tm_table		*/ NULL,
	/* query_geometry	*/ NULL,
	/* display_accelerator	*/ XtInheritDisplayAccelerator,
	/* extension		*/ NULL
    },

    /* primitive_class fields 	*/
    {
        ((XtWidgetProc) _XtInherit),      /* border highlight        */
        ((XtWidgetProc) _XtInherit),      /* border_unhighlight      */
       	XtInheritTranslations,            /* translations            */
        NULL,                             /* arm and activate        */
        NULL,                             /* synthetic resources     */
        0,                                /* number of syn res       */
	NULL,				  /* extension 		     */
    },

    /* label_class fields 	*/
    {
	XmInheritSetOverrideCallback,	/* setOverrideCallback	   	*/
	((XmMenuProc) _XtInherit),	/* menuProcs			*/
	XtInheritTranslations,		/* translations			*/
	NULL,				/* extension               	*/
    },

    /* pushbutton_class     */
    { 
	0,				/* foo */
    },

    /* extpushbutton_class fields */
    {
	None				/* stipple_bitmap */
    }
};


WidgetClass xgExtPushButtonWidgetClass = (WidgetClass) &xgextPushButtonClassRec;


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
CvtStringToExtLabelType(dpy, args, num_args, from, to)
Display 	*dpy;
XrmValuePtr	args;
Cardinal	*num_args;
XrmValuePtr	from;
XrmValuePtr	to;
{
static XgExtLabelType	label_type;

	if ( *num_args != 0 )
	{
        	XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
                    "cvtStringToExtLabelType", "wrongParameters",
                    "XgExtPushButton",
                    "String to ExtLabelType conversion needs no arguments",
                     NULL, NULL);
	}

	/*
	 * User didn't provide enough space
	 */
	if ( to->addr != NULL && to->size < sizeof(XgExtLabelType)  )
	{
		to->size = sizeof(XgExtLabelType);
		return False;
	}

	/*
	 * Look for a match with the different types of Extended Labels
	 */ 
	if ( CompareStrings(from->addr, "string") )
		label_type = XgSTRING;
	else if ( CompareStrings(from->addr, "pixmap_data") )
		label_type = XgPIXMAP_DATA;
	else if ( CompareStrings(from->addr, "pixmap_file") )
		label_type = XgPIXMAP_FILE;
	else if ( CompareStrings(from->addr, "pixmap_pixmap") )
		label_type = XgPIXMAP_PIXMAP;
	else if ( CompareStrings(from->addr, "bitmap_file") )
		label_type = XgBITMAP_FILE;
	else 
	{
		XtDisplayStringConversionWarning(dpy, from->addr, 
			XgRExtLabelType);
		return False;
	}

	/*
	 * Store our return value
	 */
	if ( to->addr == NULL )
		to->addr = (caddr_t) &label_type;
	else
		*(XgExtLabelType *) to->addr = label_type;

	to->size = sizeof(XgExtLabelType);

	return True;
}



static void
Class_Initialize()
{
	/*
	 * Add a type converter for the Extended Label types
	 */
	XtSetTypeConverter(XmRString, XgRExtLabelType,
		(XtTypeConverter)CvtStringToExtLabelType, NULL, 0, 
		XtCacheAll, NULL);
}


/* +++PHDR+++
 *
 * Function:	Initialize()
 *
 * Scope:	static
 *
 * Description:
 *	This function is the Initialize() method for the XgExtPushButton widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * request	XgExtPushButtonWidget	Widget filled in from resources
 * new		XgExtPushButtonWidget	Copy of request widget that has
 *					  been potentialy altered by 
 *					  XgExtPushButton's superclasses
 *
 *
 * Returns:	void
 *
 *
 * ---PHDR--- */

static void Initialize(request, new, args, num_args)
XgExtPushButtonWidget	request, new;
ArgList			args;
Cardinal		*num_args;
{
	/* 
	 * Create a bitmap for stippling (Drawable resources are cheap) 
	 */
	if ( STIPPLE_BITMAP == None )
	{
	Display	*dpy = XtDisplay((Widget)request);
	Window	rootW = DefaultRootWindow(dpy);

		STIPPLE_BITMAP = XCreateBitmapFromData(dpy, rootW, 
			stipple_bits, stipple_width, stipple_height );
	}


	new->extpushbutton.doing_setvalues = FALSE;

	if ( new->extpushbutton.label_string == NULL )
	{
	char *ptr;
	
		if ( new->extpushbutton.label_type == XgSTRING )
			ptr = XtName((Widget)new);
		else
			ptr = NULL;

                new->extpushbutton.label_string = XtNewString((String)
			(ptr == NULL) ? "" : ptr);
	}
	else
		new->extpushbutton.label_string = 
			XtNewString(new->extpushbutton.label_string);
		

	/*
	 * First see what type of extended label this is
	 */
	if ( new->extpushbutton.label_type == XgSTRING )
		SetButtonLabel(new);
	else 
		SetButtonPixmap(new);
}


static void
Destroy(w)
XgExtPushButtonWidget	w;
{
	/*
	 * Make sure this is a ExtPushButton widget
	 */
	if ( !XgIsExtPushButton((Widget)w) )
		return;

	if ( w->extpushbutton.label_string != NULL )
		XtFree(w->extpushbutton.label_string);

	if (  w->extpushbutton.label_type != XgPIXMAP_PIXMAP )
	{
		if ( w->extpushbutton.label_type != XgPIXMAP_PIXMAP )
		{
		Pixmap	pix = w->extpushbutton.pix;
		Pixmap	arm_pix = w->extpushbutton.arm_pix;
		Pixmap	insen_pix = w->extpushbutton.insen_pix;
		Display *dpy= XtDisplay((Widget)w);

			if ( pix != None && pix != XmUNSPECIFIED_PIXMAP )
				XFreePixmap(dpy, pix);

			if ( arm_pix != None && 
				arm_pix != XmUNSPECIFIED_PIXMAP )
					XFreePixmap(dpy, arm_pix);

			if ( insen_pix != None && 
				insen_pix != XmUNSPECIFIED_PIXMAP )
					XFreePixmap(dpy, insen_pix);
		}
	}
}


/* +++PHDR+++
 *
 * Function:	SetValues()
 *
 * Scope:	static	
 *
 * Description:
 *	SetValues() method for the XgExtPushButton widget. 
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

static Boolean SetValues(current, request, new)
Widget	current, request, new;
{
XgExtPushButtonWidget	cur = (XgExtPushButtonWidget)current,
		        req = (XgExtPushButtonWidget)request,
		        neww = (XgExtPushButtonWidget)new;
Boolean			redraw = False;
Display			*dpy = XtDisplay(current);

#define NOT_EQUAL(field)       (cur->field != neww->field)

        if ( NOT_EQUAL(label._label) &&
		cur->extpushbutton.doing_setvalues == FALSE )
	{
        	XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
                    "SetValues", "notSettable",
                    "XgExtPushButton",
                    "Use XgNextLabel to change the label string",
                     NULL, NULL);
	}

	if ( NOT_EQUAL(label.label_type) &&
		cur->extpushbutton.doing_setvalues == FALSE )
	{
        	XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
                    "SetValues", "notSettable",
                    "XgExtPushButton",
                    "Use XgNextLabelType to change the label type",
                     NULL, NULL);

		redraw = True;
	}


	if ( NOT_EQUAL(label.pixmap) &&
		cur->extpushbutton.doing_setvalues == FALSE )
	{
        	XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
                    "SetValues", "notSettable",
                    "XgExtPushButton",
                    "Use XgNextPixmap to change the label pixmap",
                     NULL, NULL);
	}

	if ( NOT_EQUAL(label.pixmap_insen) &&
		cur->extpushbutton.doing_setvalues == FALSE )
	{
        	XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
                    "SetValues", "notSettable",
                    "XgExtPushButton",
                    "Cannot change the insensitive pixmap",
                     NULL, NULL);
	}


	if ( NOT_EQUAL(pushbutton.arm_pixmap) &&
		cur->extpushbutton.doing_setvalues == FALSE )
	{
        	XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
                    "SetValues", "notSettable",
                    "XgExtPushButton",
                    "Cannot change the arm pixmap",
                     NULL, NULL);
	}


	/* 
	 * Check for changed ExtLabelString
	 */
	if ( NOT_EQUAL(extpushbutton.label_string) )
	{
	char	*ptr = neww->extpushbutton.label_string;

		if ( cur->extpushbutton.label_string != NULL )
			XtFree(cur->extpushbutton.label_string);

		if ( ptr != NULL )
			neww->extpushbutton.label_string = XtNewString(ptr);

		redraw = True;
	}

	/* 
	 * Check for changed ExtPixmap
	 */
	if ( NOT_EQUAL(extpushbutton.label_pixmap) )
		redraw = True;

	/*
	 * Check for Changed extLabelType
	 */
	if ( NOT_EQUAL(extpushbutton.label_type) )
		redraw = True;


	/* 
	 * Check for changed Shift
	 */
	if ( NOT_EQUAL(extpushbutton.shift) )
		redraw = True;

	/*
	 * Check for changed shadow thickness
	 */
	if ( NOT_EQUAL(primitive.shadow_thickness) )
		redraw = True;

	/*
	 * Check for changed foreground
	 */
	if ( NOT_EQUAL(primitive.foreground) )
		redraw = True;

	/*
	 * Check for changed background
	 */
	if ( NOT_EQUAL(core.background_pixel) )
		redraw = True;


	/* 
	 * Check for changed ExtPixmap
	 */
	if ( NOT_EQUAL(pushbutton.fill_on_arm) )
		redraw = True;


	/*
	 * Check for a new label spacing
	 */
	if ( NOT_EQUAL(extpushbutton.spacing) )
		redraw = True;


	if ( NOT_EQUAL(extpushbutton.label_location) )
		redraw = True;


	
	if ( redraw == True )
	{
		if ( neww->extpushbutton.label_type == XgSTRING )
			SetButtonLabel(neww);
		else 
			SetButtonPixmap(neww);

		/*
		 * Clear the old pixmaps
		 */
		if ( cur->extpushbutton.label_type != XgPIXMAP_PIXMAP )
		{
		Pixmap	pix = cur->extpushbutton.pix;
		Pixmap	arm_pix = cur->extpushbutton.arm_pix;
		Pixmap	insen_pix = cur->extpushbutton.insen_pix;

			if ( pix != None && pix != XmUNSPECIFIED_PIXMAP )
				XFreePixmap(dpy, pix);

			if ( arm_pix != None && 
				arm_pix != XmUNSPECIFIED_PIXMAP )
					XFreePixmap(dpy, arm_pix);

			if ( insen_pix != None && 
				insen_pix != XmUNSPECIFIED_PIXMAP )
					XFreePixmap(dpy, insen_pix);
		}


		redraw = False;
	}

	return redraw;
}


static Pixmap
AddStringToPixmap(w, pixmap, fg, bg)
XgExtPushButtonWidget w;
Pixmap pixmap;
Pixel fg, bg;
{
Pixmap		pix;
int             len, direction, ascent, descent, x, y;
Display		*display = XtDisplay((Widget)w);
unsigned int	width, height, depth, border_width, 
		    text_width, text_height, pix_height, pix_width;
Window		root;
char            *string = w->extpushbutton.label_string;
XFontStruct     *font;
XCharStruct     overall;
GC		gc;
XGCValues       values;
XtGCMask        mask;


	/*
	 * If there is a XgNextString specified then image the string in
	 * the pixmap, otherwise get outa here
	 */
	if ( string == NULL )
		return pixmap;
	else if ( *string == '\0' )
		return pixmap;
	else if ( w->extpushbutton.label_location == XgNONE )
	    	return pixmap;
	else if ( pixmap == BadPixmap || pixmap == XmUNSPECIFIED_PIXMAP ||
		pixmap == None )
			return pixmap;


	/*
	 * Get the geometry of the pixmap
	 */
	if ( XGetGeometry(display, pixmap, &root, &x, &y, &width, &height,
		&border_width, &depth) == 0 )
	{
		return(pixmap);
	}


        /*
         * Compute the width and height of the pixmap
         */
	font = GetFontStruct(w->label.font);
        len = strlen(string);
        XTextExtents(font, string, len, 
    	    &direction, &ascent, &descent, &overall);
        text_width = overall.width;
        text_height = overall.ascent + overall.descent;

	if ( w->extpushbutton.label_location == XgTOP ||
		w->extpushbutton.label_location == XgBOTTOM )
	{
	    pix_height = height + text_height + w->extpushbutton.spacing;
	    pix_width  = (width > text_width) ? width : text_width;
	}
	else
	{
	    pix_height = (height > text_height) ? height : text_height;
	    pix_width  = width + w->extpushbutton.spacing + text_width;
	}

	/*
	 * Create a pixmap to hold the button pixmap and it label
	 */
	pix = XCreatePixmap(display, DefaultRootWindow(display), 
		pix_width, pix_height, depth);
	if ( pix == None )
		return pixmap;


	/*
	 * Get a GC to image the text
	 */
	mask = GCForeground | GCFont | GCFillStyle;
	values.foreground = bg;
	values.font = font->fid;
	values.fill_style = FillSolid;
	gc = XtGetGC((Widget)w, mask, &values);

	/*
	 * Clear the entire pixmap
	 */
	XFillRectangle(display, pix, gc, 0, 0, pix_width, pix_height);


	/*
	 * Now copy the button pixmap into the combined pixmap,
	 * first get the location for the pixmap
	 */
	if ( w->extpushbutton.label_location == XgRIGHT )
	{
		x = 0;
		y = (pix_height - height) / 2;
	}
	else if ( w->extpushbutton.label_location == XgBOTTOM )
	{
		x = (pix_width - width) / 2;
		y = 0;
	}
	else if ( w->extpushbutton.label_location == XgLEFT )
	{
		x = text_width + w->extpushbutton.spacing;
		y = (pix_height - height) / 2;
	}
	else 
	{
		x = (pix_width - width) / 2;
		y = text_height + w->extpushbutton.spacing;
	}

	XCopyArea(display, pixmap, pix, gc, 0, 0, width, height, x, y);


	/*
	 * Free the button pixmap we don't need it anymore
	 */
	XFreePixmap(display, pixmap);


	/*
	 * Now image the text into the pixmap, first get the location for 
	 * the label
	 */
	if ( w->extpushbutton.label_location == XgRIGHT )
	{
		x = width + w->extpushbutton.spacing;
		y = (pix_height - text_height) / 2 + overall.ascent;
	}
	else if ( w->extpushbutton.label_location == XgBOTTOM )
	{
		x = (pix_width - text_width) / 2;
		y = height + w->extpushbutton.spacing + overall.ascent;
	}
	else if ( w->extpushbutton.label_location == XgLEFT )
	{
		x = 0;
		y = (pix_height - text_height) / 2 + overall.ascent;
	}
	else 
	{
		x = (pix_width - text_width) / 2;
		y = overall.ascent;
	}

	XSetForeground(display, gc, fg);
	XDrawString(display, pix, gc, x, y, string, len);

	XtReleaseGC((Widget)w, gc);
	return(pix);
}


static void
FreePixmapData(data)
char **data;
{
char	**old_data = data;

	while ( *data != NULL )
	{
		XtFree(*data);
		data++;
	}

	XtFree((char *)old_data);
}


static char **
ReadPixmapFile(filename)
char *filename;
{
int	i, c, open_quote, nlines;
char	buf[1600], **data;
FILE	*fp;

	if ( fp = fopen(filename, "r"), fp == NULL )
		return NULL;

	open_quote = FALSE;
	nlines = 0;
	data = NULL;

	while ( c = fgetc(fp), c != EOF )
	{
		if ( c == '\"' )
		{
			if ( open_quote == FALSE )
			{
				i = 0;
				open_quote = TRUE;
			}
			else
			{
				open_quote = FALSE;
				buf[i++] = '\0';

				data = (char **)XtRealloc((char *)data,
					(nlines + 2) * sizeof(char *));

				if ( data == NULL )
					break;

				data[nlines] = XtNewString(buf);
				data[nlines + 1] = NULL;

				if ( data[nlines] == NULL )
				{
					for ( i = 0; i < nlines; i++ )
						XtFree(data[i]);

					XtFree((char *)data);
					data = NULL;

					break;
				}

				nlines++;
			}

			continue;
		}

		if ( open_quote == FALSE )
			continue;

		buf[i++] = c;
	}

	fclose(fp);
	return data;
}


static void
SetButtonPixmap(epb)
XgExtPushButtonWidget	epb;
{
Pixmap		pix = None, arm_pix = None, stipple_pix = None;
Display		*dpy = XtDisplay((Widget)epb);
char		**data = (char **)epb->extpushbutton.label_pixmap;
char		*filename = (char *)epb->extpushbutton.label_pixmap;
char		*buffer;
char            *string = epb->extpushbutton.label_string;
int		len, status, x, y, shift;
unsigned int    width, height, border_width, depth;
Window		root;
GC		gc;


	epb->extpushbutton.pix = epb->extpushbutton.insen_pix = 
		epb->extpushbutton.arm_pix = None;

	if ( epb->extpushbutton.label_type == XgBITMAP_FILE )
	{
		SetButtonBitmap(epb);
		return;
	}

	if ( epb->extpushbutton.label_type == XgPIXMAP_FILE )
		data = ReadPixmapFile(filename);

	if ( epb->extpushbutton.label_type != XgPIXMAP_PIXMAP  &&
		data == NULL )
	{
		epb->extpushbutton.doing_setvalues = TRUE;
		XtVaSetValues((Widget)epb, XmNlabelType, XmPIXMAP,
			XmNlabelPixmap, XmUNSPECIFIED_PIXMAP,
			XmNarmPixmap, XmUNSPECIFIED_PIXMAP, NULL);
		epb->extpushbutton.doing_setvalues = FALSE;

		return;
	}


	if (  epb->extpushbutton.label_type != XgPIXMAP_PIXMAP )
		pix = XgpmWidgetPixmap((Widget)epb, data, 
			epb->primitive.foreground, epb->core.background_pixel, 
			epb->core.background_pixel);
	else
		pix = (Pixmap)data;	

	pix = AddStringToPixmap(epb, pix, epb->primitive.foreground,
	    	epb->core.background_pixel);

	epb->extpushbutton.pix = pix;
	epb->extpushbutton.doing_setvalues = TRUE;
	XtVaSetValues((Widget)epb, XmNlabelType, XmPIXMAP,
		XmNlabelPixmap, (pix == None ? XmUNSPECIFIED_PIXMAP : pix),
		NULL);
	epb->extpushbutton.doing_setvalues = FALSE;

	if ( pix == None || pix == XmUNSPECIFIED_PIXMAP || pix == BadPixmap )
	{
		if ( epb->extpushbutton.label_type == XgPIXMAP_FILE )
			FreePixmapData(data);
		return;
	}

	/*
	 * Get the geometry of the pixmap
	 */
	if ( XGetGeometry(dpy, pix, &root, &x, &y, &width, &height,
		&border_width, &depth) == 0 )
	{
		XtVaSetValues((Widget)epb,
			XmNlabelInsensitivePixmap, XmUNSPECIFIED_PIXMAP,
			XmNarmPixmap, XmUNSPECIFIED_PIXMAP, NULL);

		if ( epb->extpushbutton.label_type == XgPIXMAP_FILE )
			FreePixmapData(data);

		return;
	}

	/*
	 * Create the insensitive pixmap
	 */
	stipple_pix = XCreatePixmap(dpy, pix, width, height, depth);
	epb->extpushbutton.insen_pix = stipple_pix;
	if ( stipple_pix == None )
	{
		epb->extpushbutton.doing_setvalues = TRUE;
		XtVaSetValues((Widget)epb,
			XmNlabelInsensitivePixmap, XmUNSPECIFIED_PIXMAP,
			XmNarmPixmap, XmUNSPECIFIED_PIXMAP, NULL);
		epb->extpushbutton.doing_setvalues = FALSE;

		if ( epb->extpushbutton.label_type == XgPIXMAP_FILE )
			FreePixmapData(data);

		return;
	}

	/*
	 * Copy and gray out the pixmap
	 */
	gc = XtGetGC((Widget) epb, 0, NULL);
	XSetStipple(dpy, gc, STIPPLE_BITMAP);
	XSetFillStyle(dpy, gc, FillStippled);
	XCopyArea(dpy, pix, stipple_pix, gc, 0, 0, width, height, 0, 0); 
	XSetForeground(dpy, gc, epb->core.background_pixel);
	XFillRectangle(dpy, stipple_pix, gc, 0, 0, width, height);

	/*
	 * Set the insensitive pixmap into the widget
	 */
	epb->extpushbutton.doing_setvalues = TRUE;
	XtVaSetValues((Widget)epb, 
		XmNlabelInsensitivePixmap, stipple_pix, NULL);
	epb->extpushbutton.doing_setvalues = FALSE;


	/*
	 * Don't do the Arm pixmap if fill on arm is not set
	 */
	if ( epb->pushbutton.fill_on_arm == False )
	{
		XtReleaseGC((Widget)epb, gc);
		return;
	}



	/*
	 * Now create the arm pixmap
	 */
	if ( epb->extpushbutton.label_type != XgPIXMAP_PIXMAP )
		pix = XgpmWidgetPixmap((Widget)epb, data, 
			epb->primitive.foreground, epb->pushbutton.arm_color, 
			epb->pushbutton.arm_color);
	else
		pix = (Pixmap)data;	


	if ( epb->extpushbutton.label_type == XgPIXMAP_FILE )
		FreePixmapData(data);


	pix = AddStringToPixmap(epb, pix, epb->primitive.foreground,
		epb->pushbutton.arm_color);


	if ( pix == None || pix == XmUNSPECIFIED_PIXMAP )
	{
		epb->extpushbutton.doing_setvalues = TRUE;
		XtVaSetValues((Widget)epb,
			XmNarmPixmap, XmUNSPECIFIED_PIXMAP, NULL);
		epb->extpushbutton.doing_setvalues = FALSE;

		XtReleaseGC((Widget)epb, gc);

		return;
	}


	if ( epb->extpushbutton.label_string == NULL || 
		*epb->extpushbutton.label_string == '\0' )
	{
		shift = 0;
	}
	else
	{
		if ( epb->extpushbutton.label_location == XgTOP ||
                	epb->extpushbutton.label_location == XgBOTTOM )
		        	shift = epb->primitive.shadow_thickness / 2;
		else
			shift = epb->primitive.shadow_thickness / 2;

		if ( shift < 1 )
			shift = 1;
	}

	arm_pix = XCreatePixmap(dpy, pix, 
		width + shift, height + shift, depth);
	epb->extpushbutton.arm_pix = arm_pix;	
	if ( arm_pix == None || arm_pix == XmUNSPECIFIED_PIXMAP )
	{
		epb->extpushbutton.doing_setvalues = TRUE;
		XtVaSetValues((Widget)epb,
			XmNarmPixmap, XmUNSPECIFIED_PIXMAP, NULL);
		epb->extpushbutton.doing_setvalues = FALSE;

		if ( epb->extpushbutton.label_type != XgPIXMAP_PIXMAP )
			XFreePixmap(dpy, pix);

		XtReleaseGC((Widget)epb, gc);

		return;
	}

	XSetForeground(dpy, gc, epb->pushbutton.arm_color);
	XSetFillStyle(dpy, gc, FillSolid);
	XFillRectangle(dpy, arm_pix, gc, 0, 0, width + shift, height + shift);

	XCopyArea(dpy, pix, arm_pix, gc, 0, 0, width, height, shift, shift); 

	if ( epb->extpushbutton.label_type != XgPIXMAP_PIXMAP )
		XFreePixmap(dpy, pix);

	XtReleaseGC((Widget)epb, gc);

	epb->extpushbutton.doing_setvalues = TRUE;
	XtVaSetValues((Widget)epb,
		XmNarmPixmap, arm_pix, NULL);
	epb->extpushbutton.doing_setvalues = FALSE;
}


static void
SetButtonBitmap(epb)
XgExtPushButtonWidget	epb;
{
Display		*dpy = XtDisplay((Widget)epb);

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


static void
SetButtonLabel(epb)
XgExtPushButtonWidget   epb;
{
Pixmap		pix = None;
Pixmap		arm_pix = None;
Pixmap		stipple_pix = None;
Window		root;
Display		*dpy = XtDisplay((Widget)epb);
XFontStruct	*font = GetFontStruct(epb->label.font);
GC		gc;
XGCValues	values;
XtGCMask	mask;
int		depth = epb->core.depth;
char		*string = epb->extpushbutton.label_string;
int		len, x, y;
int		offset;
int		direction, ascent, descent;
unsigned int    width, height, border_width;
XCharStruct	overall;


	epb->extpushbutton.pix = epb->extpushbutton.insen_pix = 
		epb->extpushbutton.arm_pix = None;


	if ( string == NULL || string[0] == '\0' )
		string = " ";

	/*
	 * Compute the width and height of the pixmap
	 */
	len = strlen(string);
	XTextExtents(font, string, len, 
		&direction, &ascent, &descent, &overall);
	width = overall.width;
	height = overall.ascent + overall.descent;


	/*
	 *
	 */
	pix = XCreatePixmap(dpy, DefaultRootWindow(dpy), 
		width, height, depth);
	epb->extpushbutton.pix = pix;

	if ( pix == None )
		return;

	/*
	 *
	 */
	mask = GCForeground | GCBackground | GCFont;
	values.foreground = epb->primitive.foreground;
	values.background = epb->core.background_pixel;
	values.font = font->fid;
	gc = XtGetGC((Widget)epb, mask, &values);

	/*
	 * Draw the text in the pixmap
	 */
	XDrawImageString(dpy, pix, gc, 0, overall.ascent, string, len);
	XtReleaseGC((Widget)epb, gc);

	epb->extpushbutton.doing_setvalues = TRUE;
	XtVaSetValues((Widget)epb, XmNlabelType, XmPIXMAP,
		XmNlabelPixmap, pix, NULL);
	epb->extpushbutton.doing_setvalues = FALSE;


	if ( pix == None || pix == XmUNSPECIFIED_PIXMAP || pix == BadPixmap )
		return;


	/*
	 * Get the geometry of the pixmap
	 */
	if ( XGetGeometry(dpy, pix, &root, &x, &y, &width, &height,
		&border_width, (unsigned int *)&depth) == 0 )
	{
		epb->extpushbutton.doing_setvalues = TRUE;
		XtVaSetValues((Widget)epb,
			XmNlabelInsensitivePixmap, XmUNSPECIFIED_PIXMAP,
			XmNarmPixmap, XmUNSPECIFIED_PIXMAP, NULL);
		epb->extpushbutton.doing_setvalues = FALSE;

		return;
	}

	/*
	 * Create the insensitive pixmap
	 */
	stipple_pix = XCreatePixmap(dpy, pix, width, height, depth);
	epb->extpushbutton.insen_pix = stipple_pix;
	if ( stipple_pix == None )
	{
		epb->extpushbutton.doing_setvalues = TRUE;
		XtVaSetValues((Widget)epb,
			XmNlabelInsensitivePixmap, XmUNSPECIFIED_PIXMAP,
			XmNarmPixmap, XmUNSPECIFIED_PIXMAP, NULL);
		epb->extpushbutton.doing_setvalues = FALSE;

		return;
	}

	/*
	 * Copy and gray out the pixmap
	 */
	gc = XtGetGC((Widget) epb, 0, NULL);
	XSetStipple(dpy, gc, STIPPLE_BITMAP);
	XSetFillStyle(dpy, gc, FillStippled);
	XCopyArea(dpy, pix, stipple_pix, gc, 0, 0, width, height, 0, 0); 
	XSetForeground(dpy, gc, epb->core.background_pixel);
	XFillRectangle(dpy, stipple_pix, gc, 0, 0, width, height);
	XtReleaseGC((Widget)epb, gc);

	/*
	 * Set the insensitive pixmap into the widget
	 */
	epb->extpushbutton.doing_setvalues = TRUE;
	XtVaSetValues((Widget)epb, 
		XmNlabelInsensitivePixmap, stipple_pix, NULL);
	epb->extpushbutton.doing_setvalues = FALSE;


	/*
	 * Don't do the Arm pixmap if fill on arm is not set
	 */
	if ( epb->pushbutton.fill_on_arm == False )
		return;


	/*
	 * Now create the arm pixmap
	 */
	pix = XCreatePixmap(dpy, DefaultRootWindow(dpy), 
		width, height, depth);

	if ( pix == None || pix == XmUNSPECIFIED_PIXMAP )
	{
		epb->extpushbutton.doing_setvalues = TRUE;
		XtVaSetValues((Widget)epb,
			XmNarmPixmap, XmUNSPECIFIED_PIXMAP, NULL);
		epb->extpushbutton.doing_setvalues = FALSE;

		return;
	}

	/*
	 * Draw the text in the pixmap
	 */
	mask = GCForeground | GCBackground | GCFont;
	values.foreground = epb->primitive.foreground;
	values.background = epb->pushbutton.arm_color;
	values.font = font->fid;
	gc = XtGetGC((Widget)epb, mask, &values);

	XDrawImageString(dpy, pix, gc, 0, overall.ascent, string, len);

	if ( epb->extpushbutton.shift == True )
	{
		x = y = epb->primitive.shadow_thickness / 2;
		if ( x < 1 )
			x = y = 1;
	}
	else
		x = y = 0;

	arm_pix = XCreatePixmap(dpy, pix, width + x, height + y, depth);
	epb->extpushbutton.arm_pix = arm_pix;	
	if ( arm_pix == None || arm_pix == XmUNSPECIFIED_PIXMAP )
	{
		epb->extpushbutton.doing_setvalues = TRUE;
		XtVaSetValues((Widget)epb,
			XmNarmPixmap, XmUNSPECIFIED_PIXMAP, NULL);
		epb->extpushbutton.doing_setvalues = FALSE;

		XtReleaseGC((Widget)epb, gc);
		XFreePixmap(dpy, pix);

		return;
	}


	XSetForeground(dpy, gc, epb->pushbutton.arm_color);
	XSetFillStyle(dpy, gc, FillSolid);
	XFillRectangle(dpy, arm_pix, gc, 0, 0, width + x, height + y);

	XCopyArea(dpy, pix, arm_pix, gc, 0, 0, width, height, x, y); 

	if ( epb->extpushbutton.label_type != XgPIXMAP_PIXMAP )
		XFreePixmap(dpy, pix);

	XtReleaseGC((Widget)epb, gc);

	epb->extpushbutton.doing_setvalues = TRUE;
	XtVaSetValues((Widget)epb,
		XmNarmPixmap, arm_pix, NULL);
	epb->extpushbutton.doing_setvalues = FALSE;
}

