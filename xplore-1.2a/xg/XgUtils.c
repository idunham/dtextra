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
 *	Filename: XgUtils.c
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *
 *
 *
 *	Changes
 *
 *	By	 Date	  Description
 *	-------  -------- ----------------------------------------------
 *	gmessner 11/14/95 Original file
 *
 *
 * ---FHDR--- */

#include <stdio.h>


/*
 * Include the Motif header file
 */
#include <Xm/XmP.h>
#include <Xm/List.h>
#include <Xm/ScrolledW.h>
#include <Xm/DialogS.h>
#include <Xm/Label.h>
#include <Xm/AtomMgr.h>


#include "XgUtils.h"

#if NeedFunctionPrototypes
static XtCallbackProc CenterShellCB(Widget pShell, XtPointer pClientData);
#else
static XtCallbackProc CenterShellCB();
#endif



/* +++PHDR+++
 *
 * Function:	XgConvertXmStringToString()
 *
 * Scope:	static
 *
 * Description:
 *	This function will convert an XmString to a char *.
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:	char * to converted XmString.
 *
 *
 * ---PHDR--- */

char *
XgConvertXmStringToString(xmstr)
XmString xmstr;
{
XmStringContext		context;
XmStringCharSet		charSet;
XmStringDirection	direction;
Boolean 		separator;
char			*string, *text;

	XmStringInitContext(&context, xmstr);

	XmStringGetNextSegment(context, &string, &charSet, 
		&direction, &separator);

	if ( string != NULL )
	{
		text = XtMalloc(strlen(string) + 1);
		strcpy(text, string);
	}
	else
		text = NULL;
	
	XmStringFreeContext(context);

	return ((char *)text);
}



void XgUpdateLabel(pLabel)
Widget pLabel;
{
Dimension w, h;
XExposeEvent event;

	event.window  = XtWindow(pLabel);
	if ( event.window == None )
		return;

	XtVaGetValues(pLabel, XmNwidth, &w, XmNheight, &h, NULL);
	event.type = Expose;
	event.display = XtDisplay(pLabel);
	event.x = event.y = 0;
	event.width  = w;
	event.height = h;
	(XtClass(pLabel))->core_class.expose(pLabel, (XEvent *)&event, 0);

	XmUpdateDisplay(pLabel);
}


void XgSetListStrings(pList, pStrings, iCount)
Widget pList;
char ** pStrings;
int iCount;
{
XmString *pXmList;
int i;

	if ( iCount < 1 || pStrings == NULL )
	{
		XtVaSetValues(pList, XmNitems, NULL, XmNitemCount, 0, NULL);
		return;
	}

	/*
	 * Convert the string list into an XmString list
	 */
	pXmList = (XmString *)XtMalloc(sizeof(XmString) * iCount);
	for ( i = 0; i < iCount; i++ )
		pXmList[i] = XmStringCreateLtoR(pStrings[i],
			XmSTRING_DEFAULT_CHARSET);

	/*
	 * Set the string list into the list
	 */
	XtVaSetValues(pList, XmNitems, pXmList, XmNitemCount, iCount, NULL);

	/*
	 * Free the created list up
	 */
	for ( i = 0; i < iCount; i++ )
		XmStringFree(pXmList[i]);

	XtFree((XtPointer)pXmList);
}


void XgSetCloseCallback(pShell, pCB, pClientData)
Widget pShell;
XtCallbackProc pCB;
XtPointer pClientData;
{
Atom DeleteWindowAtom;

	while ( !XtIsShell(pShell) )
		pShell = XtParent(pShell);
	
	/*
	 * Set it back to default if no callback
	 */
	if ( pCB == 0 )
	{
		XtVaSetValues(pShell, XmNdeleteResponse, XmDESTROY, NULL);
		return;
	}

	/*
	 * Get the WM_DELETE_WINDOW Atom
	 */
	DeleteWindowAtom = XInternAtom(XtDisplay(pShell), 
		"WM_DELETE_WINDOW", FALSE);

	if ( DeleteWindowAtom == None )
		return;

	XtVaSetValues(pShell, XmNdeleteResponse, XmDO_NOTHING, NULL);
	XmAddWMProtocolCallback(pShell, DeleteWindowAtom, pCB, pClientData);	
}


int XgRemoveDecorationAndFunction(pShell, iDecorations, iFunctions)
Widget pShell;
int iDecorations;
int iFunctions;
{
int iDecors, iFuncs;
Cardinal ac;
Arg al[2];

	/*
	 * First get the shell
	 */
	while ( !XtIsShell(pShell) )
		pShell = XtParent(pShell);

	XtVaGetValues(pShell, XmNmwmDecorations, &iDecors,
		XmNmwmFunctions, &iFuncs, NULL);

	ac = 0;
	if ( iDecorations != 0 )
	{
		if ( iDecors == MWM_DECOR_ALL || iDecors == -1 )
		{
			iDecors = MWM_DECOR_ALL | iDecorations;
			XtSetArg(al[ac], XmNmwmDecorations, iDecors); ac++;
		}
		else if ( iDecors & iDecorations )
		{
			iDecors -= iDecorations;
			XtSetArg(al[ac], XmNmwmDecorations, iDecors); ac++;
		}
	}

	if ( iFunctions != 0 )
	{
		if ( iFuncs == MWM_FUNC_ALL || iFuncs == -1 )
		{
			iFuncs = MWM_FUNC_ALL | MWM_FUNC_MAXIMIZE;
			XtSetArg(al[ac], XmNmwmFunctions, iFuncs); ac++;
		}
		else if ( iFuncs & iFunctions )
		{
			iFuncs -= iFunctions;
			XtSetArg(al[ac], XmNmwmFunctions, iFuncs); ac++;
		}
	}

	if ( ac == 0 )
		return FALSE;

	XtSetValues(pShell, al, ac);
	return TRUE;
}


void XgSetOlwmDecor(widget, decor)
Widget widget;
int decor;
{
Widget		shell = widget;
Display		*display;
Window		window;
Atom		decors[4];
int		count = 0;
static Atom	olDecor = None, olClose = None, olHeader = None, 
			olPin = None, olResize = None;

	if ( decor == -1 )
		return;
	
	/*
	 *  Get the shell widget of the widget
	 */
	while ( !XtIsShell(shell) )
		shell = XtParent(shell);


	if ( window = XtWindow(shell), window == None )
		return;
	
	display = XtDisplay(shell);
	

	/*
	 *  First get the required atoms if they are not created then assume
	 *  that OpenLook is not running and get outa here
	 */
	if ( olDecor == None )
		olDecor = XInternAtom(display, "_OL_DECOR_ADD", False);

	if ( olDecor == None )
		return;

	if ( olClose == None )
		olClose = XInternAtom(display, "_OL_DECOR_CLOSE", False);

	if ( olHeader == None )
		olHeader = XInternAtom(display, "_OL_DECOR_HEADER", False);

	if ( olPin == None )
		olPin = XInternAtom(display, "_OL_DECOR_PIN", False);

	if ( olResize == None )
		olResize = XInternAtom(display, "_OL_DECOR_RESIZE", False);


	if ( (decor & MWM_DECOR_RESIZEH) && olResize )
		decors[count++] = olResize;

	if ( (decor & MWM_DECOR_TITLE) && olHeader )
		decors[count++] = olHeader;

	if ( (decor & MWM_DECOR_MINIMIZE) && olClose )
		decors[count++] = olClose;

	if ( count == 0 )
		return;

	/*
	 *  Add the decorations to the widget properties
	 */
	XChangeProperty(display, window, olDecor, XA_ATOM, 32, 
		PropModeAppend, (unsigned char *)decors, count);
}


void XgCenterShellOverParent(pShell, pParent)
Widget pShell;
Widget pParent;
{
	pShell = XgGetShellWidget(pShell);
	if ( pParent != NULL )
		pParent = XgGetShellWidget(pParent);
	XtAddCallback(pShell, XmNpopupCallback,
		(XtCallbackProc)CenterShellCB, (XtPointer)pParent);
}


static XtCallbackProc CenterShellCB(pShell, pClientData)
Widget pShell;
XtPointer pClientData;
{
	Widget pParent, pChild;
	Display *pDisplay;
	Dimension display_width, display_height;
	Dimension parent_width, parent_height;
	Position  parent_x, parent_y, x, y;

	/*
	 * Get the info out of the client data
	 */
	pDisplay = XtDisplay(pShell);
	pParent = (Widget)pClientData;
	
	pChild = XgGetShellChildWidget(pShell);
    	if ( pChild == NULL )
        	pChild = pShell;

	/*
	 * Get the info required to center this shell over it's parent
	 */
	display_width = DisplayWidth(pDisplay,DefaultScreen(pDisplay));
	display_height = DisplayHeight(pDisplay, DefaultScreen(pDisplay));
	if ( pParent != NULL )
	{
		XtVaGetValues(pParent, 
			XmNwidth, &parent_width,
			XmNheight, &parent_height, 
			XmNx, &parent_x,
			XmNy, &parent_y, NULL);
	}
	else
	{
		parent_width = display_width;
		parent_height = display_height;
		parent_x = parent_y = 0;
	}

	/*
	 * Now compute the X and Y to center the shell over another widget
	 */
	x = (parent_x + parent_width  / 2) - pChild->core.width  / 2;
	y = (parent_y + parent_height / 2) - pChild->core.height / 2;

	x = ((x < 1) ? 0 : (x + (Position)pChild->core.width > 
	    (Position)display_width) ? display_width - pChild->core.width : x);
	y = ((y < 1) ? 0 : 
	    (y + (Position)pChild->core.height > (Position)display_height) ?
	    display_height - pChild->core.height : y);


	/*
	 * Position the shell
	 */
	XtVaSetValues(pShell, XmNx, x, XmNy, y, NULL);

	XtRemoveCallback(pShell, XmNpopupCallback,
		(XtCallbackProc)CenterShellCB, pClientData);
}


Widget XgGetShellWidget(w)
Widget w;
{
Widget pShell = w;

	while( pShell && !XtIsSubclass(pShell, shellWidgetClass) )
        	pShell = XtParent(pShell);

    	return pShell;
}


Widget XgGetShellChildWidget(w)
Widget w;
{
Cardinal num_childern;
WidgetList pChildren;

    	if ( w == NULL )
        	return NULL;

    	if ( !XtIsSubclass(w, shellWidgetClass) )
        	return NULL;

    	XtVaGetValues(w, 
    		XmNnumChildren, &num_childern, 
    		XmNchildren, &pChildren, NULL);
    	if ( num_childern == 0 )
        	return NULL;
    	else
        	return pChildren[0];
}



String
XgWidgetToName(widget)
Widget widget;
{
Widget parent, shell, w;
String	widget_name, tmp_name, parent_name, shell_name;


	if ( widget == NULL )
		return NULL;

	/*
	 * Set things up to get rid of popup_ shells in the name
	 */
	shell = XgGetShellWidget(widget);
	shell_name = XtName(shell);
	if ( strncmp("popup_", shell_name, 6) != 0 )
		shell_name = NULL;


	/*
	 * build a fully qualified name for the master widget
	 * (so we can clone at a later running of the application
	 */
	widget_name = XtNewString(XtName(widget));
	parent = XtParent(widget);
        while ( parent != NULL )
        {
	    tmp_name = widget_name;
	    parent_name = XtName(parent);
	    widget_name = (String)XtMalloc(strlen(tmp_name) +
		strlen(parent_name) + 2);
	    strcpy(widget_name, parent_name);
	    strcat(widget_name, ".");
	    strcat(widget_name, tmp_name);
	    XtFree((char *)tmp_name);

	    /*
	     * We can stop here if the parent is just below the shell and
	     * the shell has the same name as the parent + "popup_"
	     */
	    if ( shell_name != NULL )
		if ( shell == XtParent(parent) )
			break;

            if ( XtIsShell(parent) )
                break;

	    w = XtParent(parent);
	    if ( w == NULL )
	    	break;

    	    if ( XtIsShell(w) )
	   	if ( strcmp(XtName(parent), XtName(w)) == 0 )
		    break;
	    
	    parent = w;
        }

	return widget_name;
}



Boolean
XgScrollToPosition(w, pos, item_height)
Widget w;
int pos;
int item_height;
{
Widget		clip_win, vsb, work_win, parent;
Dimension 	clip_h;
Position 	work_y;
int 		value, max_value, slider_size, view_start, view_end;

	if ( pos < 0 )
		return False;

	parent = w;
	while ( !XtIsSubclass((Widget)parent, xmScrolledWindowWidgetClass) )
	{
		if ( XtIsShell(parent) )
			return False;

		parent = XtParent(parent);
	}


	XtVaGetValues(parent, XmNclipWindow, &clip_win, 
		XmNworkWindow, &work_win,
		XmNverticalScrollBar, &vsb, NULL);

	if ( clip_win == NULL || vsb == NULL || work_win == NULL )
		return False;

	XtVaGetValues(clip_win, XmNheight, &clip_h, NULL);
	XtVaGetValues(work_win, XmNy, &work_y, NULL);
	XtVaGetValues(vsb, XmNsliderSize, &slider_size,
		XmNmaximum, &max_value, NULL);

	view_start = (work_y < 0) ? work_y * -1 : work_y;
	view_end = view_start + clip_h;

	if ( pos >= view_start && pos + item_height <= view_end )
		return False;


	if ( pos < view_start )
		value = pos;
	else 
		value = pos + item_height - clip_h;

	if ( value > max_value )
		value = max_value;

	XmScrollBarSetValues(vsb, value, slider_size, 0, 0, True);

	return True;
}


/* +++PHDR+++
 *
 * Function:	XgGetColorTable()
 *
 * Scope:	extern
 *
 * Description:	Gets the XColors in the passed in colormap.
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * display      Display *
 * cmap         Colormap
 * max_colors   int *
 *
 *
 * Returns:	An allocated array of XColor
 *
 *
 * Notes:
 *
 * ---PHDR--- */


XColor *
XgGetColorTable(display, cmap, max_colors)
	Display * display;
	Colormap cmap;
	int * max_colors;
{
	XColor * color_table;
	int i;
	
	/*
	 * Do we need to get the size of the color table
	 */
	if ( *max_colors == -1 )
		*max_colors = DisplayCells(display, DefaultScreen(display));

	/*
	 * Allocate the color table
	 */
	color_table = (XColor *)malloc(sizeof(XColor) * *max_colors);
	if (color_table == (XColor *)NULL)
		return ((XColor *)NULL);

	/*
	 * Defeault the pixels then query the colors
	 */
	for ( i = 0; i < *max_colors; i++ )
		color_table[i].pixel = i;
	XQueryColors(display, cmap, color_table, *max_colors);

	return (color_table);
}

	
/* +++PHDR+++
 *
 * Function:	XgGetClosestColor()
 *
 * Scope:	extern
 *
 * Description: Returns the closest color in the passed in array of XColor.
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * display      Display *               X display pointer
 * xcolor       XColor *                Color to find
 * xcolors      XColor *                Array of XColor to look at
 * max_colors   int                     Number of colors in xcolors
 *
 *
 * Returns:	The pixel of the closest color, -1 on failure.
 *
 *
 * Notes:	xcolors is created with a call to XgGetColorTable(), or
 *		pass NULL for xcolors and the call will be made for you.
 *
 * ---PHDR--- */

Pixel
XgGetClosestColor(display, xcolor, xcolors, max_colors)
	Display * display;
	XColor *xcolor;
	XColor *xcolors;
	int max_colors;
{
	long closest_distance, distance, tmp;
	int use_pixel = -1;
	int i;
	BOOL my_table = FALSE;

	/*
	 * Do we need to get a color table?
	 */
	if ( xcolors == (XColor *)NULL )
	{
		Colormap cmap = DefaultColormap(display, 
			DefaultScreen(display));
		max_colors = DisplayCells(display, DefaultScreen(display));

		xcolors = XgGetColorTable(display, cmap, &max_colors);
		if ( xcolors == (XColor *)NULL )
			return ((Pixel)-1);

		my_table = TRUE;
	}
			

	/*
	 * Get the farthest color distance possible
	 */
	tmp = 32766 * 30 / 100;
	closest_distance = tmp * tmp;
	tmp = 32766 * 61 / 100;
	closest_distance += tmp * tmp;
	tmp = 32766 * 11 / 100;
	closest_distance += tmp * tmp;


	/*
	 * Now loop thru the passed in colors for the closest one
	 */
	for ( i = 0; i < max_colors; i++ )
	{
	    	/*
	    	 * Use Euclidean distance in RGB space, weighted by Y (of YIQ)
	   	 * as the objective function;  this accounts for differences
	    	 * in the color sensitivity of the eye.
	    	 */
		tmp = (((int)xcolor->red) - (int)xcolors[i].red) * 30 / 100;
		distance = tmp * tmp;
		tmp = (((int)xcolor->green) - (int)xcolors[i].green) * 61 / 100;
		distance += tmp * tmp;
		tmp = (((int)xcolor->blue) - (int)xcolors[i].blue) * 11 / 100;
		distance += tmp * tmp;
		if ( distance < closest_distance )		
		{
			closest_distance = distance;
			use_pixel = i;
			if ( distance == 0 )
				break;
		}
	}

	/*
	 * If the color table is owned by this function then free it.
	 */
	if ( my_table )
		free(xcolors);

	return ((Pixel)use_pixel);
}
