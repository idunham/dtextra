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
 *	Filename: ComboBox.c
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *		This is the source file for a Motif Combo Box widget.
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	GMM	08/27/93	Original file
 *	GMM	09/02/93	General cleanup
 *	GMM	01/25/94	Added XgNexpandListWidth and XgNautoFillIn
 *				resources
 *
 *
 * ---FHDR--- */



#include	<ctype.h>

/*
 * We define a cursor for the list
 */
#include	<X11/cursorfont.h>

/*
 * These includes are needed to describe the widget
 */
#include 	<X11/StringDefs.h>
#include	<X11/IntrinsicP.h>
#include 	<Xm/XmP.h>

/*
 * These includ file are for the elements of the widget
 */
#include	<Xm/Text.h>
#include	<Xm/ArrowB.h>
#include	<Xm/List.h>
#include	<Xm/DialogS.h>
#include	<Xm/ScrolledW.h>

#include 	"ComboBoxP.h"
#include 	"XgUtils.h"


/*
 * Declaration of methods
 */
static void Class_Initialize();
static void Destroy();
static void Initialize();
static void InsertChild();
static Boolean SetValues();
static void Resize();
static XtGeometryResult QueryGeometry();
static XtGeometryResult GeometryManager();


/*
 * Internal use functions and callbacks
 */
static Widget createComboBoxList();
static void verifyInputCB();
static void comboArrowCB();
static void ActivateCallback();
static Boolean CheckFocus();
static void FocusChangeHandler();
static void FillCallbackStruct();
static void hideList();
static void updateListAndText();
static void CopyBg();
static void CopyFg();
static void FocusChangeCB();
static void visibleCB();
static void comboListCB();

#define AddFocusChangeHandler(w)	\
	XtAddEventHandler(w, FocusChangeMask, False, \
	(XtEventHandler)FocusChangeHandler, NULL)




static char textTranslations[] =
    "Alt<Key>osfDown:	    ComboBox-Manager(show-hide-list)	\n\
     Meta<Key>osfDown:	    ComboBox-Manager(show-hide-list)	\n\
     Alt<Key>osfUp:	    ComboBox-Manager(hide-list)		\n\
     Meta<Key>osfUp:	    ComboBox-Manager(hide-list)		\n\
     <Key>osfUp:	    ComboBox-Manager(up)		\n\
     <Key>osfDown:	    ComboBox-Manager(down)		\n\
     <Key>osfPageUp:	    ComboBox-Manager(page-up)		\n\
     <Key>osfPageDown:	    ComboBox-Manager(page-down)		\n\
     <Key>osfCancel:	    ComboBox-Manager(cancel)		\n\
     <Key>Return:	    ComboBox-Manager(activate) activate()";

static char noEditTextTranslations[] = 
    "<Key>osfBeginLine:	    ComboBox-Manager(top)  \n\
     <Key>osfEndLine:	    ComboBox-Manager(bottom)";


#ifdef NODRAGNDROP
static char noDDListTranslations[] =
    "<Btn2Down>:	    ComboBox-Manager(no-operation)";
#endif
static char listTranslations[] =
    "<Key>osfPageUp:	    ComboBox-Manager(page-up)		\n\
     <Key>osfPageDown:	    ComboBox-Manager(page-down)		";


static void CBoxManager();

static XtActionsRec actions[] = {
    { "ComboBox-Manager", CBoxManager },
    { NULL, NULL }
};

static XtTranslations newTextTranslations, newNoEditTextTranslations, 
                      newListTranslations, newNoDDListTranslations;


#define TextChild(w)	((CompositeWidget)(w->combobox.text))
#define ArrowChild(w)	((CompositeWidget)(w->combobox.arrow))


static XtResource resources[] = {
    { XmNcolumns, XmCColumns, XmRShort, sizeof(short),
        XtOffsetOf(XgComboBoxRec, combobox.textColumns),
		XmRImmediate, (XtPointer)10 },

    { XgNtextForeground, XmCForeground, XmRPixel, sizeof(Pixel),
        XtOffsetOf(XgComboBoxRec, combobox.textFg),
		XtRCallProc, (XtPointer)CopyFg },

    { XgNlistForeground, XmCForeground, XmRPixel, sizeof(Pixel),
        XtOffsetOf(XgComboBoxRec, combobox.listFg),
		XtRCallProc, (XtPointer)CopyFg },

    { XgNtextBackground, XmCBackground, XmRPixel, sizeof(Pixel),
        XtOffsetOf(XgComboBoxRec, combobox.textBg),
		XtRCallProc, (XtPointer)CopyBg },

    { XgNlistBackground, XmCBackground, XmRPixel, sizeof(Pixel),
        XtOffsetOf(XgComboBoxRec, combobox.listBg),
		XtRCallProc, (XtPointer)CopyBg },

    { XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension,sizeof(Dimension),
	XtOffsetOf(XgComboBoxRec, combobox.margin_height), 
		XmRImmediate, (XtPointer)5 },
		
    { XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension, 
       sizeof(Dimension), XtOffsetOf(XgComboBoxRec, combobox.margin_width),
		XmRImmediate, (XtPointer)5 },

    { XmNmaxLength, XmCMaxLength, XmRInt, sizeof(int),
        XtOffsetOf(XgComboBoxRec, combobox.textMaxLength),
		XmRImmediate, (XtPointer)128 },

    { XmNeditable, XmCEditable, XmRBoolean, sizeof(Boolean),
        XtOffsetOf(XgComboBoxRec, combobox.editable), 
		XmRImmediate, (XtPointer)True},

    { XmNverifyBell, XmCVerifyBell, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XgComboBoxRec, combobox.audible), 
		XmRImmediate, (XtPointer)False },

    { XmNvisibleItemCount, XmCVisibleItemCount, XmRInt, sizeof(int),
        XtOffsetOf(XgComboBoxRec, combobox.visibleItems), 
		XmRImmediate, (XtPointer) 1 },

    { XmNitemCount, XmCItemCount, XmRInt, sizeof(int),
        XtOffsetOf(XgComboBoxRec, combobox.listCount), 
		XmRImmediate, (XtPointer) 0 },

    { XmNitems, XmCItems, XmRXmStringTable, sizeof(XmStringTable),
        XtOffsetOf(XgComboBoxRec, combobox.listTable), 
		XmRImmediate, (XtPointer)NULL },

    { XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
	XtOffsetOf(XgComboBoxRec, combobox.font_list), 
		XmRImmediate, (XtPointer) NULL},

    { XgNautoFillIn, XgNautoFillIn, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(XgComboBoxRec, combobox.autoFillIn),
		XmRImmediate, (XtPointer)False},

    { XgNexpandListWidth, XgNexpandListWidth, XmRBoolean, sizeof(Boolean),
      	XtOffsetOf(XgComboBoxRec, combobox.expandListWidth), 
		XmRImmediate, (XtPointer)True},

    { XmNfocusCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
	XtOffsetOf( XgComboBoxRec, combobox.focus_list ), 
		XmRImmediate, (XtPointer) NULL },

    { XmNlosingFocusCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
	XtOffsetOf( XgComboBoxRec, combobox.losing_focus_list ), 
		XmRImmediate, (XtPointer) NULL },

    { XmNvalueChangedCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
	XtOffsetOf( XgComboBoxRec, combobox.value_changed ), 
		XmRImmediate, (XtPointer) NULL },

    { XmNactivateCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
	XtOffsetOf( XgComboBoxRec, combobox.activate ), 
		XmRImmediate, (XtPointer) NULL }

};


XgComboBoxClassRec XgcomboBoxClassRec = {
    {
    /* core_class fields */
	/* superclass		*/ (WidgetClass) &xmManagerClassRec,
	/* class_name		*/ "XgComboBox",
	/* widget_size		*/ sizeof(XgComboBoxRec),
	/* class_initialize	*/ Class_Initialize,
	/* class_part_initialize*/ NULL,
	/* class_inited		*/ False,
	/* initialize		*/ Initialize,
	/* initialize_hook	*/ NULL,
	/* realize		*/ XtInheritRealize,
	/* actions		*/ NULL,
	/* num_actions		*/ 0,
	/* resources		*/ resources,
	/* num_resources	*/ XtNumber(resources),
	/* xrm_class		*/ NULLQUARK,
	/* compress_motion	*/ True,
	/* compress_exposure	*/ XtExposeCompressMaximal,
	/* compress_enterleave	*/ True,
	/* visible_interest	*/ True,
	/* destroy		*/ Destroy,
	/* resize		*/ Resize,
	/* expose		*/ XtInheritExpose,
	/* set_values		*/ SetValues,
	/* set_values_hook	*/ NULL,
	/* set_values_almost	*/ XtInheritSetValuesAlmost,
	/* get_values_hook	*/ NULL,
	/* accept_focus		*/ XtInheritAcceptFocus,
	/* version		*/ XtVersion,
	/* callback_private	*/ NULL,
	/* tm_table		*/ XtInheritTranslations,
	/* query_geometry	*/ QueryGeometry,
	/* display_accelerator	*/ NULL,
	/* extension		*/ NULL
    },

    /* composite_class fields 	*/
    {
	/* geometry_manager	*/ GeometryManager,
	/* changed_managed	*/ XtInheritChangeManaged,
	/* insert_child		*/ XtInheritInsertChild,
	/* delete_child		*/ XtInheritDeleteChild,
	/* extension		*/ NULL
    },

    /* constraint_class fields 	*/
    {
	NULL,				/* resource list	*/		
	0,				/* num resources 	*/
	0,				/* constraint size	*/
	NULL,				/* init proc		*/
	NULL,				/* destroy proc       	*/
	NULL,				/* set values proc    	*/
	NULL	
    },

    /* manager_class     */
    { 
	XtInheritTranslations, 		/* translations			*/
	NULL, 				/* syn_resources		*/
	0, 				/* num_syn_resources  		*/
	NULL, 				/* syn_cont_resources 		*/
	0, 				/* num_syn_cont_resources	*/
	XmInheritParentProcess, 	/* parent_process		*/
	NULL
    },

    /* combo box fields */
    {
	0,
    }
};


WidgetClass xgComboBoxWidgetClass = (WidgetClass) &XgcomboBoxClassRec;

static Boolean
CvtStringToStringTable(display, args, num_args, from, to)
Display 	*display;
XrmValuePtr	args;
Cardinal	*num_args;
XrmValuePtr	from;
XrmValuePtr	to;
{
int			i;
char			*tmp_string, *string;
static XmStringTable	items = NULL;
static int		item_count = 0;

	if ( *num_args != 0 )
		XtWarningMsg("XtToolkitError", "wrongParameters",
			"CvtStringToStringTable",
			"conversion needs no arguments", (String *) NULL,
			(Cardinal *) NULL);

	if ( items != NULL )
	{
		for ( i = 0; i < item_count; i++ )
			XmStringFree(items[i]);

		XtFree((char *)items);
		items = NULL;
	}


	/*
	 * User didn't provide enough space
	 */
	if ( to->addr != NULL && to->size < sizeof(XmStringTable) )
	{
		to->size = sizeof(XmStringTable);
		return False;
	}


	items = (XmStringTable) malloc(sizeof(XmString));	
	items[0] = NULL;
	item_count = 0;

	string = (char *)from->addr;

	if ( string != NULL )
	{
	int	len = strlen(string);


		for ( tmp_string = string, i = 0; i <= len; i++ )
		{
			if ( string[i] == ',' || string[i] == '\0' )
			{
				string[i] = '\0';

				items[item_count] = XmStringCreateLtoR(
					tmp_string, XmSTRING_DEFAULT_CHARSET);
				tmp_string = string + i + 1;

				item_count++;
				items = (XmStringTable)realloc(items, 
				    sizeof(XmString) * (item_count + 1));
				items[item_count] = NULL;

				if ( *tmp_string == '\0' )
					break;
			}
		}
	}

	to->size = sizeof(XmStringTable);

	if ( item_count == 0 )
	{
		XtFree((char *)items);
		items = NULL;

		return False;
	}

	if ( to->addr == NULL )
		to->addr = (caddr_t) &items;
	else
		*(XmStringTable *) to->addr = items;

	return True;
}

static void 
CopyBg(cbw, offset, value)
XgComboBoxWidget cbw;
int  offset;
XrmValue * value;
{
    value->addr = (XtPointer) &cbw->core.background_pixel;
}


static void 
CopyFg(cbw, offset, value)
XgComboBoxWidget cbw;
int  offset;
XrmValue * value;
{
    value->addr = (XtPointer) &cbw->manager.foreground;
}



static void
Class_Initialize()
{
	XtSetTypeConverter(XmRString, XmRXmStringTable, 
		(XtTypeConverter)CvtStringToStringTable, NULL, 
		0, XtCacheNone, NULL);

    	newTextTranslations = 
	    	XtParseTranslationTable(textTranslations);
    	newNoEditTextTranslations = 
		XtParseTranslationTable(noEditTextTranslations);
#ifdef NODRAGNDROP
    	newNoDDListTranslations = 
		XtParseTranslationTable(noDDListTranslations);
#endif
    	newListTranslations = 
		XtParseTranslationTable(listTranslations);
}



/* +++PHDR+++
 *
 * Function:	Initialize()
 *
 * Scope:	static
 *
 * Description:
 *	This function is the Initialize() method for the XgComboBox widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * request	XgComboBoxWidget	Widget filled in from resources
 * new		XgComboBoxWidget	Copy of request widget that has
 *					  been potentialy altered by 
 *					  XgComboBox's superclasses
 *
 *
 * Returns:	void
 *
 *
 * ---PHDR--- */

static void
Initialize(request, new, args, num_args)
XgComboBoxWidget	request, new;
ArgList			args;
Cardinal		*num_args;
{
Dimension 	textWidth, textHeight, width, height;
Position 	arrowX, arrowY;
Boolean		editable, cursorPositionVisible;


	new->combobox.initializing = True;

	new->combobox.saved_text = NULL;


	XtAppAddActions(XtWidgetToApplicationContext((Widget) new),
       		actions, XtNumber(actions));


	new->combobox.activate = new->combobox.value_changed = NULL;
	new->combobox.focus_list = new->combobox.losing_focus_list = NULL;
	new->combobox.workproc_id = None;
	new->combobox.focus_widget = NULL;
	new->combobox.i_have_focus = False;


	/*
	 * Create the Text part of the Combo Box
	 */
	if ( new->combobox.editable == False )
		editable = cursorPositionVisible = False; 
	else 
		editable = cursorPositionVisible = True;

	new->combobox.text = XtVaCreateManagedWidget(NULL,
		xmTextWidgetClass, (Widget) new,
		XmNforeground, new->combobox.textFg,
		XmNbackground, new->combobox.textBg,
		XmNtraversalOn, True,
		XmNfontList, new->combobox.font_list,
		XmNeditable, editable,
		XmNverifyBell, new->combobox.audible,
		XmNcursorPositionVisible, cursorPositionVisible,
		XmNrows, 1,
		XmNcolumns, new->combobox.textColumns,
		XmNmarginHeight, new->combobox.margin_height,
		XmNmarginWidth, new->combobox.margin_width,
		XmNmaxLength, new->combobox.textMaxLength,
		XmNwordWrap, False,
		XmNresizeWidth, False,
		XmNresizeHeight, False,
		XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNnavigationType, XmNONE,
		NULL);


    	XtOverrideTranslations(new->combobox.text, newTextTranslations);
    	if ( !new->combobox.editable ) 
		XtOverrideTranslations(new->combobox.text, 
			newNoEditTextTranslations);
#ifdef NODRAGNDROP
    	XtOverrideTranslations(new->combobox.text, newNoDDListTranslations);
#endif


	/*
	 * Get the fontList if ULL was passed
	 */
	if ( new->combobox.font_list == NULL )
		XtVaGetValues(new->combobox.text,
			XmNfontList, &new->combobox.font_list, NULL);


	/*
 	 *  Get the size and location of the Text Widget, we use this
	 *  to locate the arrow button
	 */
	XtVaGetValues(new->combobox.text,
		XmNwidth, &textWidth,
		XmNheight, &textHeight,
		XmNx, &arrowX,
		XmNy, &arrowY,
		NULL);
		

	/*
	 * Now create and manage the arrow button
	 */
	new->combobox.arrow = XtVaCreateManagedWidget(NULL,
		xmArrowButtonWidgetClass, (Widget) new,
		XmNtraversalOn, True,
		XmNarrowDirection, XmARROW_DOWN,
		XmNwidth, (Dimension)((double)textHeight * .75),
		XmNheight, (Dimension)textHeight,
		XmNx, arrowX + textWidth,
		XmNy, arrowY,
		XmNforeground, new->manager.foreground,
		XmNbackground, new->core.background_pixel,
		XmNtraversalOn, False,
		NULL);


	/*
	 * Add an event handler for FocusChange
	 */
	AddFocusChangeHandler(new->combobox.arrow);


	/*
	*  Add the width and spacing of the ArrowButton to the textWidth
	*/
	textWidth += (Dimension)((double)textHeight * .75);


	/*
	 * Save these values as the minimal combo box size
	 */
	new->combobox.minWidth = textWidth;
	new->combobox.minHeight = textHeight;


	/*
	 *  Set the Combo Box to it's computed size, or resize it to the 
	 * passed size
	 */
	if ( textWidth > new->core.width )
		new->core.width = textWidth;

	if ( textHeight > new->core.height )
		new->core.height = textHeight;

	Resize(new);


	createComboBoxList(new);


	/*
	 *
	 */
	new->combobox.dont_reenter = FALSE;
	new->combobox.initializing = False;

	
	XtAddCallback(new->combobox.text,
		XmNlosingFocusCallback, FocusChangeCB, NULL);

	/*
	 *  Add an Event Handler to Unmanage the list if the window of
 	 *  it's parent widget is unmapped
	 */
	XtAddEventHandler((Widget)new->combobox.text, 
		VisibilityChangeMask, False, visibleCB, new);


	/*
	 *  Add a callback to verify the text typed into the text widget
	 */
	XtAddCallback(new->combobox.text,
		XmNvalueChangedCallback, verifyInputCB, NULL);

	/*
	 * Add a callback for activate
	 */
	XtAddCallback(new->combobox.text,
		XmNactivateCallback, ActivateCallback, NULL);


	/*
	 *  Add a callback to popdown the Combo List Box
	 */
	XtAddCallback(new->combobox.arrow,
		XmNactivateCallback, comboArrowCB, NULL);
}


static void
FillCallbackStruct(cbox, reason, event, cbs)
XgComboBoxWidget	cbox;
int			reason;
XEvent			*event;
XgComboBoxCallbackStruct        *cbs;
{
int				pos_count, *pos_list;
XmString			xstr;


	cbs->reason = reason;
	cbs->event = event;
	cbs->value = XmTextGetString(cbox->combobox.text);
	cbs->list_pos = -1;

	if ( cbs->value != NULL )
	    xstr = XmStringCreateLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET);
	else
	    xstr = NULL;

	if ( xstr != NULL )
	{
		if ( XmListGetMatchPos(cbox->combobox.list, xstr, 
			&pos_list, &pos_count) == True )
		{
			if ( pos_list != NULL )
			{
				cbs->list_pos = pos_list[0];
				XtFree((char *)pos_list);
			}
		}

		XmStringFree(xstr);
	}
}


static Boolean
CheckFocus(cb)
XgComboBoxWidget        cb;
{
XgComboBoxCallbackStruct        cbs;

        cb->combobox.workproc_id = None;

        /*
         * See if we've lost or gained focus
         */
        if ( cb->combobox.i_have_focus == True )
        {
		if ( cb->combobox.focus_widget != cb->combobox.text )
		{
			cb->combobox.focus_widget = cb->combobox.text;
			XmProcessTraversal(cb->combobox.text,
				XmTRAVERSE_CURRENT);

			return;
		}

		cb->combobox.i_have_focus = False;

		/*
		 * Make sure the list is poped up
		 */
		hideList(cb);


                if ( cb->combobox.losing_focus_list == NULL )
                        return True;

		FillCallbackStruct(cb, XmCR_LOSING_FOCUS, NULL, &cbs);
                XtCallCallbacks((Widget)cb, XmNlosingFocusCallback, &cbs);
		if ( cbs.value != NULL )
			XtFree(cbs.value);
        }
        else
        {
		cb->combobox.i_have_focus = True;

		if ( cb->combobox.focus_widget != cb->combobox.text )
		{
			cb->combobox.focus_widget = cb->combobox.text;
			XmProcessTraversal(cb->combobox.text, 
				XmTRAVERSE_CURRENT);
		}

                if ( cb->combobox.focus_list == NULL )
                        return True;

		FillCallbackStruct(cb, XmCR_FOCUS, NULL, &cbs);
                XtCallCallbacks((Widget)cb, XmNfocusCallback, &cbs);
		if ( cbs.value != NULL )
			XtFree(cbs.value);
        }


        /*
         * Tell XtMainLoop to remove the work procedure
         */
        return True;
}


static void
FocusChangeCB(w, client_data, call_data)
Widget          w;
XtPointer	client_data;
XtPointer	call_data;
{
XgComboBoxWidget        cb = (XgComboBoxWidget)XtParent(w);

        if ( cb->combobox.workproc_id == None &&
		cb->combobox.focus_widget == cb->combobox.text )
	{
		cb->combobox.workproc_id = XtAppAddWorkProc(
			XtWidgetToApplicationContext(w),
			(XtWorkProc)CheckFocus, (XtPointer)cb);
	}
}


static void
FocusChangeHandler(w, not_used, event, continue_dispatch)
Widget                  w;
XFocusChangeEvent       *event;
XtPointer               not_used;
Boolean                 *continue_dispatch;
{
XgComboBoxWidget        cb;

	cb = (XgComboBoxWidget)XtParent(w);
	while ( !XgIsComboBox((Widget)cb) )
		cb = (XgComboBoxWidget)XtParent((Widget)cb);

	cb->combobox.focus_widget = w;

        if ( event->type == FocusIn && cb->combobox.workproc_id == None )
		{
			cb->combobox.workproc_id = XtAppAddWorkProc(
				XtWidgetToApplicationContext(w),
				(XtWorkProc)CheckFocus, (XtPointer)cb);
		}

	*continue_dispatch = True;
}


static XtGeometryResult
QueryGeometry(w, proposed, desired)
XgComboBoxWidget w;
XtWidgetGeometry *proposed, *desired;
{
#define Set(bit) (proposed->request_mode & bit)

	desired->width = TextChild(w)->core.width + ArrowChild(w)->core.width;
	desired->height = TextChild(w)->core.height;
	desired->request_mode = CWWidth | CWHeight;

	if ( Set(CWWidth) && proposed->width == desired->width &&
		Set(CWHeight) && proposed->height == desired->height )
			return XtGeometryYes;

	if ( desired->width == w->core.width &&
		desired->height == w->core.height )
			return XtGeometryNo;

	return XtGeometryAlmost;

#undef Set
}



static void
InsertChild(w)
Widget w;
{
XgComboBoxWidget	cb = (XgComboBoxWidget)XtParent(w);

	if ( cb->combobox.initializing != True ) 
	{
	        XtAppWarningMsg(XtWidgetToApplicationContext(w),
                        "insertChild", "badChild", "XbaeCaption",
                        "XgComboBox: Cannot add children.",
                        (String *)NULL, (Cardinal *)NULL);
        	return;
    	}

    	(*((CompositeWidgetClass)
           (xgComboBoxWidgetClass->core_class.superclass))->composite_class.
     	    insert_child) (w);
}


static void
Destroy(w)
XgComboBoxWidget	w;
{
	/*
	 * Make sure this is a ComboBox widget
	 */
	if ( !XgIsComboBox((Widget)w) )
		return;

	/*
	 * Destroy the dialog shell for the popup list
	 */
	if ( w->combobox.popup != NULL )
		XtDestroyWidget(XtParent(w->combobox.popup));

	/*
	 * Remove the work procedure for the FocusHandler
	 */
	if ( w->combobox.workproc_id != None )
		XtRemoveWorkProc(w->combobox.workproc_id);

	/*
	 * Remove the callbacks, if any installed
	 */
	if ( w->combobox.focus_list != NULL )
		XtRemoveCallbacks( (Widget)w, XmNfocusCallback,
			w->combobox.focus_list);

	if ( w->combobox.losing_focus_list != NULL )
		XtRemoveCallbacks( (Widget)w, XmNlosingFocusCallback,
			w->combobox.losing_focus_list);

	if ( w->combobox.value_changed != NULL )
		XtRemoveAllCallbacks((Widget)w, XmNvalueChangedCallback);

	if ( w->combobox.activate != NULL )
		XtRemoveAllCallbacks((Widget)w, XmNactivateCallback);
}


static void
GetValuesHook(w, args, num_args)
XgComboBoxWidget	w;
ArgList 		args;
Cardinal 		*num_args;
{
int 	i;
Arg	xtarg;

	/*
	 * We don't save a copy of the items or itemsCount.
	 * If the user wants these, we get them from the list widget.
	 */
	for (i = 0; i < *num_args; i++)
		if ( strcmp(args[i].name, XmNitems) == 0 )
		{
			if ( w->combobox.list != NULL )
				XtGetValues(w->combobox.list, &args[i], 1);
		}
		else if ( strcmp(args[i].name, XmNitemCount) == 0 )
		{
			if ( w->combobox.list != NULL )
				XtGetValues(w->combobox.list, &args[i], 1);
		}
		else if ( strcmp(args[i].name, XmNvisibleItemCount) == 0 )
		{
			if ( w->combobox.list != NULL )
				XtGetValues(w->combobox.list, &args[i], 1);
		}
		else if ( strcmp(args[i].name, XmNcolumns) == 0 )
			XtGetValues(w->combobox.text, &args[i], 1);
		else if ( strcmp(args[i].name, XmNmaxLength) == 0 )
			XtGetValues(w->combobox.text, &args[i], 1);
		else if ( strcmp(args[i].name, XmNeditable) == 0 )
			XtGetValues(w->combobox.text, &args[i], 1);
		else if ( strcmp(args[i].name, XmNverifyBell) == 0 )
			XtGetValues(w->combobox.text, &args[i], 1);
		else if ( strcmp(args[i].name, XgNtextBackground) == 0 )
		{
			XtSetArg(xtarg, XmNbackground, args[i].value);
			XtGetValues(w->combobox.text, &xtarg, 1);
		}
		else if ( strcmp(args[i].name, XgNlistBackground) == 0 )
		{
		    if ( w->combobox.list != NULL )
		    {
			XtSetArg(xtarg, XmNbackground, args[i].value);
		    	XtGetValues(w->combobox.list, &xtarg, 1);
		    }
		}
}


static XtGeometryResult
GeometryManager(w, request, reply)
Widget		w;
XtWidgetGeometry	*request;
XtWidgetGeometry	*reply;
{
XgComboBoxWidget	cb = (XgComboBoxWidget)XtParent(w);

	if ( w == cb->combobox.arrow )
	{
		if ( !(request->request_mode & (CWX | CWHeight)) )
			return XtGeometryNo;
			
	}
	else if ( w == cb->combobox.text )
	{
		if ( !(request->request_mode & (CWHeight | CWWidth)) )
			return XtGeometryNo;

	}
	else if ( w == cb->combobox.list )
		return XtGeometryYes;
	else
		return XtGeometryNo;

	return XtGeometryYes;
}


static void
Resize(w)
XgComboBoxWidget w;
{
Dimension	newWidth, width, height, border;
Position	y;

	/*
	 * What we need to do here is get the width of the arrow button
	 * and subtract that from the passed width to compute the new
	 * width of the text portion of the combo box. We then use
	 * XtVaSetValues to set the new width of the text and the X location
	 * of the arrow button.
	 */ 
	XtVaGetValues((Widget)w->combobox.arrow, 
		XmNy, &y, XmNwidth, &width, NULL);
	newWidth = w->core.width - width;
	XtMoveWidget((Widget)w->combobox.arrow, newWidth, y);

	XtVaGetValues((Widget)w->combobox.text, 
		XmNheight, &height, XmNborderWidth, &border, NULL);
	XtResizeWidget((Widget)w->combobox.text, newWidth, height, border);	

}


/* +++PHDR+++
 *
 * Function:	createComboBoxList()
 *
 * Scope:	static
 *
 * Description:
 *	This function creates the List Widget for use with a Combo Box.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * comboBox	XgComboBoxWidget	XgComboBoxWidget (parent)
 *
 *
 * Returns:	Widget to popdown list
 *
 * ---PHDR--- */

static Widget
createComboBoxList(comboBox)
XgComboBoxWidget	comboBox;
{
Widget		list_shell, shell;
Dimension	width, height, hThickness, shadow;
int		ac;
Position	x, y;
Arg		al[24];

	/*
	 * First lets get some required info from the comboed Text Widget
	 */
	XtVaGetValues(comboBox->combobox.text,
		XmNx, &x,
		XmNy, &y,
		XmNheight, &height,
		XmNwidth, &width,
		XmNhighlightThickness, &hThickness,
		XmNshadowThickness, &shadow,
		NULL);


	/*
	 * Create a Dialog Shell to hold the ScrolledWindow for the List
	 */
	list_shell = XtVaCreatePopupShell(NULL,
		xmDialogShellWidgetClass, (Widget)comboBox,
		XmNforeground, comboBox->manager.foreground,
		XmNbackground, comboBox->core.background_pixel,
		XmNallowShellResize, True,
		XmNoverrideRedirect, True,
		XmNallowOverlap, False,
		XmNtransient, False,
		NULL);

	/*
	 * Now create the ScrolledWindow to hold the List
	 */
	comboBox->combobox.popup = XtVaCreateWidget(NULL,
		xmScrolledWindowWidgetClass, list_shell,
		XmNforeground, comboBox->manager.foreground,
		XmNbackground, comboBox->core.background_pixel,
		XmNspacing, 2,
		XmNshadowThickness, 0,
		XmNscrollBarDisplayPolicy, XmSTATIC,
		NULL);


	/*
	 *  Now create the (Scrolled) List Widget
	 */
	ac = 0;
	XtSetArg(al[ac], XmNfontList, comboBox->combobox.font_list); ac++;
	XtSetArg(al[ac], XmNselectionPolicy, XmBROWSE_SELECT); ac++;
	XtSetArg(al[ac], XmNautomaticSelection, False); ac++;
	XtSetArg(al[ac], XmNvisibleItemCount, 
		comboBox->combobox.visibleItems); ac++;
	XtSetArg(al[ac], XmNforeground, comboBox->combobox.listFg); ac++;
	XtSetArg(al[ac], XmNbackground, comboBox->combobox.listBg); ac++;


	/*
	 * If we have a XmStringTable for the list add it the the ArgList
	 */
	if ( comboBox->combobox.listCount > 0 &&
		comboBox->combobox.listTable != NULL )
	{
		XtSetArg(al[ac], XmNitemCount, comboBox->combobox.listCount);
		ac++;
		XtSetArg(al[ac], XmNitems, comboBox->combobox.listTable); ac++;
	}


	/*
	 * If expandListWidth is set do it
	 */
	if ( comboBox->combobox.expandListWidth == True )
	{
		XtSetArg(al[ac], XmNlistSizePolicy, XmCONSTANT); ac++;
		XtSetArg(al[ac], XmNwidth, width - hThickness * 2); ac++;
	}

	comboBox->combobox.list = XtCreateManagedWidget(NULL, 
		xmListWidgetClass, comboBox->combobox.popup, al, ac);


#ifdef NODRAGNDROP
    	XtOverrideTranslations(comboBox->combobox.list, 
		newNoDDListTranslations);
#endif


	/*
	 *  Add a callback to write the value of the selected item in the List
	 *  into the text Widget
	 */
	XtAddCallback(comboBox->combobox.list, XmNbrowseSelectionCallback, 
		comboListCB, comboBox);
	XtAddCallback(comboBox->combobox.list, XmNdefaultActionCallback, 
		comboListCB, comboBox);

	/*
	 * Add an event handler for FocusChange
	 */
	AddFocusChangeHandler(comboBox->combobox.list);

	shell = (Widget) comboBox;
	while ( !XtIsShell(shell) )
		shell = XtParent(shell);

	XtAddEventHandler(shell, StructureNotifyMask, False, 
		visibleCB, comboBox);

	return (comboBox->combobox.list);
}


/* +++PHDR+++
 *
 * Function:	SetValues()
 *
 * Scope:	static	
 *
 * Description:
 *	SetValues() method for the XgComboBox widget. 
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
XgComboBoxWidget	cur = (XgComboBoxWidget)current,
		        req = (XgComboBoxWidget)request,
		        neww = (XgComboBoxWidget)new;
Boolean			redraw = False;
#define NOT_EQUAL(field)       (cur->field != neww->field)


	/* 
	 * Check for changed columns
	 */
	if ( cur->combobox.textColumns != neww->combobox.textColumns )
	{
		/*
		 * Make sure the requested columns is valid
		 */
		if ( neww->combobox.textColumns <= 0 ||
			neww->combobox.textColumns > 2048 )
		{
       			XtWarning("Columns must be > 0 and < 2048");
			neww->combobox.textColumns = 
				cur->combobox.textColumns;
		}
		/*
		 * Requested columns is valid, so set it
		 */
		else if ( neww->combobox.text != NULL )
		{
			XtVaSetValues(neww->combobox.text,
				XmNcolumns, neww->combobox.textColumns,
				NULL);

		}
		
		req->combobox.textColumns = neww->combobox.textColumns;

		redraw = True;
	}


	/* 
	 * Check for changed margin height
	 */
	if ( cur->combobox.margin_height != neww->combobox.margin_height && 
		neww->combobox.text != NULL )
	{
		XtVaSetValues(neww->combobox.text,
			XmNmarginHeight, neww->combobox.margin_height,
			NULL);

		redraw = True;
	}


	/* 
	 * Check for changed margin width
	 */
	if ( cur->combobox.margin_width != neww->combobox.margin_width && 
		neww->combobox.text != NULL )
	{
		XtVaSetValues(neww->combobox.text,
			XmNmarginWidth, neww->combobox.margin_width,
			NULL);

		req->combobox.margin_width = neww->combobox.margin_width;
		redraw = True;
	}


	/* 
	 * Check for changed max length of text
	 */
	if ( cur->combobox.textMaxLength != neww->combobox.textMaxLength )
	{
		/*
		 * Make sure the requested max length is valid
		 */
		if ( neww->combobox.textMaxLength <= 0 ||
			neww->combobox.textMaxLength > 2048 )
		{
       			XtWarning("Max Length must be > 0 and < 2048");
			neww->combobox.textMaxLength = 
				cur->combobox.textMaxLength;
		}
		/*
		 * Requested max length is valid, so set it
		 */
		else if ( neww->combobox.text != NULL )
		{
			XtVaSetValues(neww->combobox.text,
				XmNmaxLength, neww->combobox.textMaxLength,
				NULL);

		}
	}

	/* 
	 * Check for changed list item count
	 */
	if ( cur->combobox.listCount != neww->combobox.listCount )
	{
		/*
		 * Make sure the requested max length is valid
		 */
		if ( neww->combobox.listCount < 0 )
		{
       			XtWarning("List item count must be >= 0");
			neww->combobox.listCount = req->combobox.listCount = 
				cur->combobox.listCount;
		}
		else
			req->combobox.listCount = cur->combobox.listCount =
				neww->combobox.listCount;
	}

	/* 
	 * Check for changed listForeground
	 */
	if ( NOT_EQUAL(combobox.listFg) && neww->combobox.list != NULL )
	{
		XtVaSetValues(neww->combobox.list,
			XmNforeground, neww->combobox.listFg,
			NULL);
	}

	/* 
	 * Check for changed listBackground
	 */
	if ( NOT_EQUAL(combobox.listBg) && neww->combobox.list != NULL )
	{
		XtVaSetValues(neww->combobox.list,
			XmNbackground, neww->combobox.listBg,
			NULL);
	}


	/* 
	 * Check for changed textBackground
	 */
	if ( NOT_EQUAL(combobox.textBg) && neww->combobox.text != NULL )
	{
		XtVaSetValues(neww->combobox.text,
			XmNbackground, neww->combobox.textBg,
			NULL);
	}

	/* 
	 * Check for changed textBackground
	 */
	if ( NOT_EQUAL(combobox.textFg) && neww->combobox.text != NULL )
	{
		XtVaSetValues(neww->combobox.text,
			XmNforeground, neww->combobox.textFg,
			NULL);
	}


	/* 
	 * Check for changed editable
	 */
	if ( cur->combobox.editable != neww->combobox.editable && 
		neww->combobox.text != NULL )
	{
		XtVaSetValues(neww->combobox.text,
			XmNeditable, neww->combobox.editable,
			NULL);
	}


	/* 
	 * Check for changed expandListWidth 
	 */
	if ( cur->combobox.expandListWidth != neww->combobox.expandListWidth ) 
	{
		if ( neww->combobox.list != NULL )
		{
			if ( neww->combobox.expandListWidth == True )
				XtVaSetValues(neww->combobox.list,
					XmNlistSizePolicy, XmCONSTANT,
					XmNvisualPolicy, XmCONSTANT, NULL);
			else
				XtVaSetValues(neww->combobox.list,
					XmNlistSizePolicy, XmVARIABLE,
					XmNvisualPolicy, XmVARIABLE, NULL);
		}

		req->combobox.expandListWidth = neww->combobox.expandListWidth;
	}


	/* 
	 * Check for changed verifyBell 
	 */
	if ( cur->combobox.audible != neww->combobox.audible && 
		neww->combobox.text != NULL )
	{
		XtVaSetValues(neww->combobox.text,
			XmNverifyBell, neww->combobox.audible,
			NULL);

		req->combobox.audible = neww->combobox.audible;
	}



	/* 
	 * Check for changed list table
	 */
	if ( cur->combobox.listTable != neww->combobox.listTable &&
		neww->combobox.list != NULL )
	{

		if ( neww->combobox.list != NULL )
		{
			XtVaSetValues(neww->combobox.list,
				XmNitems, neww->combobox.listTable,
				XmNitemCount, req->combobox.listCount,
				NULL);

			XtVaGetValues(neww->combobox.list,
				XmNitems, &neww->combobox.listTable,
				NULL);
		}

		req->combobox.listCount = neww->combobox.listCount;
	}



	/* 
	 * Check for changed visible items for list
	 */
	if ( cur->combobox.visibleItems != neww->combobox.visibleItems )
	{
		/*
		 * Make sure the requested visible items is valid
		 */
		if ( neww->combobox.visibleItems <= 0 ||
			neww->combobox.visibleItems > 2048 )
		{
       			XtWarning("Visible items must be > 0 and < 2048");
			neww->combobox.visibleItems = 
				cur->combobox.visibleItems;
		}
		/*
		 * Requested visible items is valid, so set it
		 */
		else if ( neww->combobox.list != NULL )
		{
			XtVaSetValues(neww->combobox.list,
			    XmNvisibleItemCount, neww->combobox.visibleItems,
				NULL);

		}

		req->combobox.visibleItems = neww->combobox.visibleItems;
	}


	/* 
	 * Check for changed font_list
	 */
	if ( cur->combobox.font_list != neww->combobox.font_list )
	{
		XtVaSetValues(neww->combobox.text,
			XmNfontList, neww->combobox.font_list,
			NULL);

		if ( neww->combobox.list != NULL )
			XtVaSetValues(neww->combobox.list,
				XmNfontList, neww->combobox.font_list,
				NULL);

		req->combobox.font_list = neww->combobox.font_list;
		redraw = True;
	}

	/*
	 * Did the foreground color change?
	 */
	if ( neww->manager.foreground != cur->manager.foreground )
	{
		XtVaSetValues(neww->combobox.text,
			XmNforeground, neww->manager.foreground,
			NULL);

		XtVaSetValues(neww->combobox.arrow,
			XmNforeground, neww->manager.foreground,
			NULL);


		if ( neww->combobox.list != NULL )
			XtVaSetValues(neww->combobox.list,
				XmNforeground, neww->manager.foreground,
				NULL);

		req->manager.foreground = neww->manager.foreground;

	}



	/*
	 * Did the background color change?
	 */
	if ( neww->core.background_pixel != cur->core.background_pixel )
	{
		XtVaSetValues(neww->combobox.text,
			XmNbackground, neww->core.background_pixel,
			NULL);

		XtVaSetValues(neww->combobox.arrow,
			XmNbackground, neww->core.background_pixel,
			NULL);


		if ( neww->combobox.list != NULL )
			XtVaSetValues(neww->combobox.list,
				XmNbackground, neww->core.background_pixel,
				NULL);

		req->core.background_pixel = neww->core.background_pixel;

	}


	if ( redraw == True )
	{
	Dimension	width, height, bwidth;

		XtVaGetValues(neww->combobox.text, XmNwidth, &width,
			XmNheight, &height, NULL);

		XtVaSetValues(neww->combobox.arrow, 
			XmNwidth, (Dimension)(height * .75),
			XmNx, width, 
			XmNheight, height, NULL);

		XtVaGetValues(neww->combobox.arrow, XmNwidth, &bwidth, NULL);

		if ( width + bwidth > neww->core.width )
			neww->core.width = req->core.width = width + bwidth;
	}


	return redraw;
}


static void
ActivateCallback(w, client_data, cb)
Widget			w;
XtPointer		client_data;
XmAnyCallbackStruct	*cb;
{
XgComboBoxWidget		cbox = (XgComboBoxWidget)XtParent(w);
XgComboBoxCallbackStruct	cbs;


	if ( cbox->combobox.activate == NULL )
		return;

	FillCallbackStruct(cbox, cb->reason, cb->event, &cbs);
	XtCallCallbacks((Widget)cbox, XmNactivateCallback, &cbs);
	if ( cbs.value != NULL )
		XtFree(cbs.value);
}


 
/* +++PHDR+++
 *
 * Function:	verifyInputCB()
 *
 * Scope:	extern
 *
 * Description:
 *	This function verifies that the passed text is in the list widget.
 *
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
verifyInputCB(widget, client, cb)
Widget			widget;
caddr_t			client;
XmAnyCallbackStruct	*cb;
{
char			*text, *list_string;
int			i, s, found, len, listCount;
XmStringTable		items;
XgComboBoxWidget       comboBox = (XgComboBoxWidget)XtParent(widget);
Boolean			bellOn;
XgComboBoxCallbackStruct	cbs;


	/*
	 * Make sure we are not renetering this function from the same widget
	 */
	if ( comboBox->combobox.dont_reenter == TRUE )
		return;


	/*
	 * Convert the string in the Text widget to a char *
	 */
	if ( text = XmTextGetString(comboBox->combobox.text), text == NULL )
		return;

	/*
	 * Get the length of the string
	 */
	len = strlen(text);
	for ( i = len - 1; text[i] == ' ' && i > 0; i-- )
		text[i] = '\0';
	len = strlen(text);

	if ( len == 0 )
	{
		if ( comboBox->combobox.value_changed != NULL  )
		{
			cbs.reason = XmCR_VALUE_CHANGED;
			cbs.event = cb->event;
			cbs.value = text;
			cbs.list_pos = -1;
			cbs.old_value = NULL;
			XtCallCallbacks((Widget)comboBox, 
				XmNvalueChangedCallback, &cbs);
		}


		XtFree(text);
		return;
	}

	
	/*
	 * Get the item count and items XmStrings from the list widget
	 */
	XtVaGetValues(comboBox->combobox.list,
		XmNitems, &items, XmNitemCount, &listCount, NULL);

	/*
	 *
	 */
	for ( found = s = 0; s < listCount; s++ )
	{
		list_string = XgConvertXmStringToString(items[s]);

		/*
		 * Make sure that we got the string from the list
		 */
		if ( list_string == NULL )
			continue;


		/*
		 * We have a match
		 */
		if ( strncmp(text, list_string, len) == 0 )
		{
		Widget	list = comboBox->combobox.list;
		int	topPos = s - comboBox->combobox.visibleItems / 2;

			if ( found != 0 )
			{
				if ( comboBox->combobox.value_changed != NULL )
				{
					cbs.reason = XmCR_VALUE_CHANGED;
					cbs.event = cb->event;
					cbs.value = text;
					cbs.list_pos = -1;
					cbs.old_value = NULL;
					XtCallCallbacks((Widget)comboBox, 
						XmNvalueChangedCallback, &cbs);
				}

				XtFree(list_string);
				XtFree(text);
				return;
			}
			else
			{
			int	topItem;

				XtVaGetValues(list,
					XmNtopItemPosition, &topItem, NULL);

			
				if ( topItem + comboBox->combobox.visibleItems
					<= s + 1 || topItem >= s + 1 )
				{
				    while ( topPos < 1 )
					topPos++;

				    while ( topPos > s + 1 )
					topPos--;
						
				    XmListSetPos(list, topPos);
				}

				/*
				 * Select the matched item
				 */
				XmListSelectPos(list, s + 1, False);
			}

			found = s + 1;
		}

		XtFree(list_string);
	}


	if ( comboBox->combobox.value_changed != NULL && found == 0 )
	{
		cbs.reason = XmCR_VALUE_CHANGED;
		cbs.event = cb->event;
		cbs.value = text;
		cbs.list_pos = -1;
		cbs.old_value = NULL;
		XtCallCallbacks((Widget)comboBox, 
			XmNvalueChangedCallback, &cbs);

		XtFree(text);
	}


	/*
	 * We found a single match of the list so, set the Text widget
	 * to the matching value from the list
	 */
	if ( found != 0 )
	{
		list_string = XgConvertXmStringToString(items[found - 1]);

		if ( comboBox->combobox.autoFillIn == False )
		{
			len = strlen(list_string);
			for ( i = len - 1; list_string[i] == ' ' && i > 0; i-- )
				list_string[i] = '\0';

			if ( strcmp(text, list_string) != 0 )
				found = -1;
		}
		else
		{
			comboBox->combobox.dont_reenter = TRUE;

			XmTextSetString(comboBox->combobox.text, list_string);
		        XtVaSetValues(comboBox->combobox.text, 
				XmNcursorPosition, 0, NULL);

			comboBox->combobox.dont_reenter = FALSE;
		}

		if ( comboBox->combobox.value_changed != NULL  )
		{
			cbs.reason = XmCR_VALUE_CHANGED;
			cbs.event = cb->event;
			cbs.value = text;
			cbs.list_pos = found;
			cbs.old_value = NULL;
			XtCallCallbacks((Widget)comboBox, 
				XmNvalueChangedCallback, &cbs);
		}

		XtFree(text);
		XtFree(list_string);
		return;
	}


	if ( comboBox->combobox.audible == True )
		XBell(XtDisplay(widget), 50);
}

/* +++PHDR+++
 *
 * Function:	comboArrowCB()
 *
 * Scope:	static
 *
 * Description:
 *	This function is called to popdown/popup the List on a Combo Box Widget.
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
comboArrowCB(arrow, client, cbd)
Widget 			arrow;
XtPointer		client;
XmAnyCallbackStruct 	*cbd;
{
int			ac, visible, itemCount;
Position		x, y, px, py;
Dimension		height, width, hThickness;
Arg			al[8];
static Cursor		cursor;
XgComboBoxWidget       w = (XgComboBoxWidget)XtParent(arrow);


	/*
	 * Make sure the text widget has focus
	 */
	if ( w->combobox.i_have_focus == False )
	{
		w->combobox.focus_widget = w->combobox.text;
		w->combobox.i_have_focus = True;
		XmProcessTraversal(w->combobox.text, XmTRAVERSE_CURRENT);
	}

	/*
	* Toggle the Managed (Displayed) State of the Combo List Widget
	*/
	if ( w->combobox.list != NULL )
	{
		if ( XtIsManaged(w->combobox.popup) )
		{
			hideList(w);
			return;
		}
	}
	else
		w->combobox.list = createComboBoxList(w);

	if ( w->combobox.list == NULL )
		return;

	
	/*
	 * There has to be some items in the list to pop it up
	 */
	XtVaGetValues(w->combobox.list, XmNitemCount, &itemCount, NULL);
	if ( itemCount < 1 )
		return;

	
	/*
	 * Save the text in the combobox so we can reset it on cancel
	 */
	if ( w->combobox.saved_text != NULL )
		XtFree(w->combobox.saved_text);

	w->combobox.saved_text = XmTextGetString(w->combobox.text);



	/*-----------------------------------------------------------
 	*  First lets get info from the comboed Text Widget to 
 	*  place the List
	*/
	XtVaGetValues(w->combobox.text,
		XmNx, &x,
		XmNy, &y,
		XmNwidth, &width,
		XmNheight, &height,
		XmNhighlightThickness, &hThickness, NULL);

	/*-----------------------------------------------------------
	*  Assign a cursor for the Popdown Combo Box List
	*/
	if ( cursor == 0 )
		cursor = XCreateFontCursor(
			XtDisplay(w->combobox.list), XC_arrow);

	XDefineCursor(XtDisplay(w->combobox.list),
		XtWindow(XtParent(w->combobox.popup)), cursor);

	XtTranslateCoords(XtParent(w->combobox.text),
		x + hThickness, y + height, &px, &py);

	if ( w->combobox.expandListWidth == True )
	{
		XtVaSetValues(w->combobox.list, 
			XmNwidth, width - hThickness * 2, NULL);

		XtMoveWidget(XtParent(w->combobox.popup), px, py);
	}
	else
	{
	Dimension list_width;
	
		XtVaGetValues(w->combobox.list, XmNwidth, &list_width, NULL);
		XtMoveWidget(XtParent(w->combobox.popup), 
			px + (width - hThickness * 2) - list_width, py);
	}

	XtManageChild(w->combobox.popup);
	XtPopup(XtParent(w->combobox.popup), XtGrabNone);

	/*
	 * Make the Arrow point up
	 */
	XtVaSetValues(w->combobox.arrow,
		XmNarrowDirection, XmARROW_UP,
		NULL);
}


 
/* +++PHDR+++
 *
 * Function:	comboListCB()
 *
 * Scope:	static
 *
 * Description:
 *	This function is called to to put the selected item in a List in
 *	a List in it's associated text Widget in the Combo Box.
 * 
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
comboListCB(list, w, cbd)
Widget 			list;
XgComboBoxWidget	w;
XmListCallbackStruct 	*cbd;
{
char 				*string = NULL, *old_string = NULL;
XgComboBoxCallbackStruct	cbs;
XgComboBoxWidget		comboBox;
int  				dont_reenter;

	comboBox = (XgComboBoxWidget)XtParent(XtParent(XtParent(list)));
	dont_reenter = comboBox->combobox.dont_reenter;

	if ( comboBox->combobox.saved_text != NULL )
	{
		XtFree(comboBox->combobox.saved_text);
		comboBox->combobox.saved_text = NULL;
	}

	old_string  = XmTextGetString(comboBox->combobox.text);
	string = XgConvertXmStringToString(cbd->item);

	if ( string != NULL )
	{
		comboBox->combobox.dont_reenter = TRUE;
		XmTextSetString(comboBox->combobox.text, string);
		comboBox->combobox.dont_reenter = dont_reenter;
	}

	if ( comboBox->combobox.value_changed != NULL && 
		dont_reenter == FALSE )
	{
		cbs.reason = XmCR_VALUE_CHANGED;
		cbs.event = cbd->event;
		cbs.value = string;
		cbs.list_pos = cbd->item_position;
		cbs.old_value = old_string;
		XtCallCallbacks((Widget)comboBox, 
			XmNvalueChangedCallback, (XtPointer)&cbs);
	}


	if ( old_string != NULL )
		XtFree(old_string);

	if ( string != NULL )
		XFree(string);

	/*
	 * Is the list visible, if not get outa here
	 */
	if ( XtIsManaged(comboBox->combobox.popup) == False )
		return;

	/*
	*  Popdown the List Widget
	*/
	hideList(comboBox);
}




static void
updateListAndText(cbw, index, item, notify)
XgComboBoxWidget cbw;
int index;
XmString item;
Boolean notify;
{
char *string;

	/*
	 * Update the selected pos of the list
	 */
	XmListSelectPos(cbw->combobox.list, index, notify);

	/*
	 * Now try and put the string in the Text widget
	 */
	string = XgConvertXmStringToString(item);

	if ( string != NULL )
	{
		if ( !notify )
			cbw->combobox.dont_reenter = TRUE;

		XmTextSetString(cbw->combobox.text, string);
		
		if ( !notify )
			cbw->combobox.dont_reenter = FALSE;
	}
}

 
/* +++PHDR+++
 *
 * Function:	visibleCB()
 *
 * Scope:	static
 *
 * Description:
 *	This function is called when the Visibility state of Text Widget
 *	for a Combo List Widget has changed in order to unmanage a popped
 *	up Combo Box List widget.
 *	
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
visibleCB(parent, w, event)
Widget			parent;
XgComboBoxWidget	w;
XEvent 			*event;
{
XVisibilityEvent	*vevent = (XVisibilityEvent *) event;


	if ( w->combobox.popup == NULL )
		return;

	/*-------------------------------------------------------------------
	*  See if the Shell for the Text Form has been moved, if so
	*  popdown the Combo Box List Widget
	*/
	if ( event->type == ConfigureNotify ||  event->type == UnmapNotify )
		hideList(w);

	if ( event->type != VisibilityNotify )
		return;

	/*
	*  Hide the Combo Box List if it's Text Widget isn't fully visible
	*/
	if ( vevent->state != VisibilityUnobscured )
		hideList(w);
}




static void
hideList(w)
XgComboBoxWidget w;
{
	if ( w->combobox.popup == NULL )
		return;

	/*
	 * get outa here if the popup is already popped down
	 */
	if ( !XtIsManaged(w->combobox.popup) )
		return;

	/*
	* Popdown the List Widget
	*/
	XtPopdown(XtParent(w->combobox.popup));
	XtUnmanageChild(w->combobox.popup);

	if ( w->combobox.saved_text != NULL )
	{
	Boolean dont_reenter = w->combobox.dont_reenter;		

		w->combobox.dont_reenter = TRUE;
		XmTextSetString(w->combobox.text, w->combobox.saved_text);
		w->combobox.dont_reenter = dont_reenter;

		XtFree(w->combobox.saved_text);
		w->combobox.saved_text = NULL;
	}


	/*
	 * Make the Arrow point down
	 */
	XtVaSetValues(w->combobox.arrow,
		XmNarrowDirection, XmARROW_DOWN,
		NULL);
}


/* +++PHDR+++
 *
 * Function:	XgComboBoxGetChild()
 *
 * Scope:	extern
 *
 * Description:
 *	This function returns a child widget of the passed XgComboBox widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgComboBox
 * child	unsigned char		Constant for child to return
 *
 * child constants:
 *	XgCOMBOBOX_TEXT
 *	XgCOMBOBOX_LIST
 *	XgCOMBOBOX_ARROW
 *
 *
 * Returns:	Widget id of child widget, NULL if error
 *
 *
 * ---PHDR--- */


Widget 
XgComboBoxGetChild(w, child)
Widget w; 
int child;
{
	/*
	 * Make sure this is a ComboBox widget
	 */
	if ( !XgIsComboBox(w) )
		return NULL;

	switch ( (int) child )
	{
	  case XgCOMBOBOX_TEXT:
		return(((XgComboBoxWidget)w)->combobox.text);

	  case XgCOMBOBOX_LIST:
		return(((XgComboBoxWidget)w)->combobox.list);

	  case XgCOMBOBOX_ARROW:
		return(((XgComboBoxWidget)w)->combobox.arrow);
	}


	return NULL;
}


/* +++PHDR+++
 *
 * Function:	XgComboBoxGetString()
 *
 * Scope:	extern
 *
 * Description:
 *	This function returns a the string in the text portion of the 
 *	ComboBox widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgComboBox
 *
 *
 * Returns: 	char * to string in text widget, user must free this	
 *
 *
 * ---PHDR--- */

char *
XgComboBoxGetString(w)
Widget w; 
{
	/*
	 * Make sure this is a ComboBox widget
	 */
	if ( !XgIsComboBox(w) )
		return NULL;

	return((char *) XmTextGetString(((XgComboBoxWidget)w)->combobox.text));
}



/* +++PHDR+++
 *
 * Function:	XgComboBoxSetString()
 *
 * Scope:	extern
 *
 * Description:
 *	This function sets the string in the text portion of the 
 *	ComboBox widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgComboBox
 *
 *
 * Returns: 	void
 *
 *
 * ---PHDR--- */

#ifndef _NO_PROTO
void XgComboBoxSetString(Widget w, char *value, Boolean notify)
#else
void
XgComboBoxSetString(w, value, notify)
Widget 	w; 
char	*value;
Boolean notify;
#endif
{
XgComboBoxWidget        comboBox = (XgComboBoxWidget)w;

	/*
	 * Make sure this is a ComboBox widget
	 */
	if ( XgIsComboBox(w) )
	{
		if ( value == NULL )
			value = "";

		if ( !notify )
			comboBox->combobox.dont_reenter = TRUE;

		XmTextSetString(comboBox->combobox.text, value);

		if ( !notify )
			comboBox->combobox.dont_reenter = FALSE;
	}

}



/* +++PHDR+++
 *
 * Function:	XgComboBoxSelectPos()
 *
 * Scope:	extern
 *
 * Description:
 *	This function sets the ComboBox widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgComboBox
 *
 *
 * Returns: 	void
 *
 *
 * ---PHDR--- */

#ifdef _NO_PROTO
int
XgComboBoxGetSelectedPos(w, pos_value)
Widget w;
char ** pos_value;
#else
int XgComboBoxGetSelectedPos(Widget w, char ** pos_value)
#endif
{
XgComboBoxWidget        comboBox = (XgComboBoxWidget)w;
int              	*selection_list;
int              	selection_count;
int			selected_pos = 0;
XmStringTable	 	items;
Widget			list;

	if ( pos_value != NULL )
		*pos_value = NULL;

	if ( !XgIsComboBox(w) )
		return 0;

	list = comboBox->combobox.list;
	if ( XmListGetSelectedPos(list, &selection_list, &selection_count) ) 
	{
		if ( selection_count > 0 )
		   	selected_pos = selection_list[0]; 
	    	XtFree((char *)selection_list);
        
		if ( pos_value != NULL && selected_pos > 0 )
		{
			XtVaGetValues(list, XmNitems, &items, NULL);
			*pos_value = XgConvertXmStringToString(
				items[selected_pos - 1]);
		}
	} 

	return selected_pos;
}


/* +++PHDR+++
 *
 * Function:	XgComboBoxSelectPos()
 *
 * Scope:	extern
 *
 * Description:
 *	This function sets the ComboBox widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgComboBox
 *
 *
 * Returns: 	void
 *
 *
 * ---PHDR--- */

#ifdef _NO_PROTO
void 
XgComboBoxSelectPos(w, selected_pos, notify)
Widget w;
int selected_pos;
Boolean notify;
#else
void 
XgComboBoxSelectPos(Widget w, int selected_pos, Boolean notify)
#endif
{
XgComboBoxWidget        comboBox = (XgComboBoxWidget)w;

	if ( !XgIsComboBox(w) )
		return;

	comboBox->combobox.dont_reenter = TRUE;
	XmListSelectPos(comboBox->combobox.list, selected_pos, notify);
	comboBox->combobox.dont_reenter = FALSE;

	if ( notify == False )
	{
	int item_count;
	XmStringTable items;

		XtVaGetValues(comboBox->combobox.list,
			XmNitems, &items, XmNitemCount, &item_count, NULL);

		if ( item_count > 0 )
		{
		int item_index;
		char *string;

			if ( selected_pos == 0 )
				item_index = item_count - 1;
			else
				item_index = selected_pos - 1;


			string = XgConvertXmStringToString(items[item_index]);
			if ( string != NULL )
			{
				comboBox->combobox.dont_reenter = TRUE;
				XmTextSetString(comboBox->combobox.text, 
					string);
				comboBox->combobox.dont_reenter = FALSE;
				XtFree(string);	
			}
		}
	}
}



/* +++PHDR+++
 *
 * Function:	XgComboBoxSetItems()
 *
 * Scope:	extern
 *
 * Description:
 *	This function sets the string in the text portion of the 
 *	ComboBox widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgComboBox
 *
 *
 * Returns: 	void
 *
 *
 * ---PHDR--- */

#ifdef _NO_PROTO
void
XgComboBoxSetItems(w, list, list_count, selected_pos, notify)
Widget 		w; 
XmString	*list;
int		list_count;
int		selected_pos;
Boolean		notify;
#else
void
XgComboBoxSetItems(Widget w, XmString *list,
        int list_count, int selected_pos, Boolean notify)
#endif
{
XgComboBoxWidget	comboBox = (XgComboBoxWidget)w;
	/*
	 * Make sure this is a ComboBox widget
	 */
	if ( XgIsComboBox(w) == FALSE )
		return;


	/*
	 * Put the passed list table into the popup list
	 */
	XtVaSetValues(w, XmNitems, list, XmNitemCount, list_count, NULL);

	/*
	 * Put the string in the ComboBox text widget
	 */
	if ( selected_pos >= 0 )
	{
		if ( notify == False )
			comboBox->combobox.dont_reenter = TRUE;

		XgComboBoxSelectPos(w, selected_pos, notify); 

		if ( notify == False )
			comboBox->combobox.dont_reenter = FALSE;
	}
		
}



/* +++PHDR+++
 *
 * Function:	XgComboBoxSetList()
 *
 * Scope:	extern
 *
 * Description:
 *	This function sets the string in the text portion of the 
 *	ComboBox widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgComboBox
 *
 *
 * Returns: 	void
 *
 *
 * ---PHDR--- */

#ifdef _NO_PROTO
void
XgComboBoxSetList(w, list, list_count, selected_pos, notify)
Widget	w; 
String	*list;
int	list_count;
int	selected_pos;
Boolean	notify;
#else
void
XgComboBoxSetList(Widget w,  char **list, 
	int list_count, int selected_pos, Boolean notify)
#endif
{
XgComboBoxWidget	comboBox = (XgComboBoxWidget)w;
XmString *xmList;
int i;

	/*
	 * Make sure this is a ComboBox widget
	 */
	if ( XgIsComboBox(w) == FALSE )
		return;


	if ( list_count == 0 )
	{
		if ( comboBox->combobox.list != NULL )
		{
			XtVaSetValues(comboBox->combobox.list,
				XmNitems, 0, XmNitemCount, 0, NULL);

			XtVaGetValues(comboBox->combobox.list,
				XmNitems, &comboBox->combobox.listTable,
				XmNitemCount, &comboBox->combobox.listCount,
				NULL);
		}
		else
			XtVaSetValues(w, XmNitems, 0, XmNitemCount, 0, NULL);

		return;
	}


	/*
	 * First convert the string list into an XmString list
	 */
	xmList = (XmString *)XtMalloc(sizeof(XmString) * list_count);
	for ( i = 0; i < list_count; i++ )
		xmList[i] = XmStringCreateLtoR(list[i],
			XmSTRING_DEFAULT_CHARSET);

	XtVaSetValues(w, XmNitems, xmList, XmNitemCount, list_count, NULL);

	/*
	 * Free the created list up
	 */
	for ( i = 0; i < list_count; i++ )
		XmStringFree(xmList[i]);
	XtFree((char *)xmList);

	/*
	 * Put the string in the ComboBox text widget
	 */
	if ( selected_pos >= 0 )
	{
		if ( notify == False )
			comboBox->combobox.dont_reenter = TRUE;

		XgComboBoxSelectPos(w, selected_pos, notify); 

		if ( notify == False )
			comboBox->combobox.dont_reenter = FALSE;
	}
}



/* +++PHDR+++
 *
 * Function:	XgComboBoxAddItem()
 *
 * Scope:	extern
 *
 * Description:
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgComboBox
 * str          char *
 * pos          int
 *
 *
 * Returns: 	void
 *
 *
 * ---PHDR--- */

void
#ifndef _NO_PROTO
XgComboBoxAddItem(Widget w, String str, int pos, Boolean select_it)
#else
XgComboBoxAddItem(w, str, pos, select_it)
Widget	w; 
String	str;
int	pos;
Boolean	select_it;
#endif
{
XgComboBoxWidget comboBox = (XgComboBoxWidget)w;
XmString	 xmstr;

	/*
	 * Make sure this is a ComboBox widget
	 */
	if ( XgIsComboBox(w) == FALSE )
		return;

	/*
	 * First convert the string list into an XmString list
	 */
	if ( str == NULL )
		str = "";
	xmstr = XmStringCreateLtoR(str, XmSTRING_DEFAULT_CHARSET);

	/*
	 * Add it to the list widget
	 */
	if ( select_it )
		XmListAddItem(comboBox->combobox.list, xmstr, pos);
	else
	{
		comboBox->combobox.dont_reenter = TRUE;
		XmListAddItemUnselected(comboBox->combobox.list, xmstr, pos);
		comboBox->combobox.dont_reenter = FALSE;
	}

	XmStringFree(xmstr);
}


/* +++PHDR+++
 *
 * Function:	XgComboBoxChangeItem()
 *
 * Scope:	extern
 *
 * Description:
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgComboBox
 * str          char *
 * pos          int
 *
 *
 * Returns: 	void
 *
 *
 * ---PHDR--- */

void
XgComboBoxChangeItem(w, str, pos)
Widget	w; 
String	str;
int	pos;
{
XgComboBoxWidget comboBox = (XgComboBoxWidget)w;
XmString	 xmstr[1];
int		 item_count;

	/*
	 * Make sure this is a ComboBox widget
	 */
	if ( XgIsComboBox(w) == FALSE )
		return;

	if ( pos < 1 )
		return;

	XtVaGetValues(comboBox->combobox.list, XmNitemCount, &item_count, NULL);
	if ( pos > item_count )
		return;


	/*
	 * First convert the string list into an XmString list
	 */
	if ( str == NULL )
		str = "";
	xmstr[0] = XmStringCreateLtoR(str, XmSTRING_DEFAULT_CHARSET);

	/*
	 *  Replace the item in the list widget
	 */
	XmListReplaceItemsPos(comboBox->combobox.list, xmstr, 1, pos);
	XmStringFree(xmstr[0]);
}


/* +++PHDR+++
 *
 * Function:	XgComboBoxDeletePos()
 *
 * Scope:	extern
 *
 * Description:
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgComboBox
 * pos          int
 *
 *
 * Returns: 	void
 *
 *
 * ---PHDR--- */

void
XgComboBoxDeletePos(w, pos)
Widget	w; 
int	pos;
{
XgComboBoxWidget comboBox = (XgComboBoxWidget)w;

	/*
	 * Make sure this is a ComboBox widget
	 */
	if ( XgIsComboBox(w) == FALSE )
		return;

	XmListDeletePos(comboBox->combobox.list, pos);
}


static void CBoxManager(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
XgComboBoxWidget cbw;
Widget           list;
int              *selectionList;
int              selectionCount;
int		 topPos;
XmStringTable	 items;
int              selectionIndex;
int              listCount; 
int		 visibleItems;
char             opt;
    
    /*
     * Get the ComboBox widget
     */
    if ( XtClass(w) == xmListWidgetClass )
        cbw = (XgComboBoxWidget) XtParent(XtParent(w));
    else
        cbw = (XgComboBoxWidget) XtParent(w);
    list = cbw->combobox.list;
    

    /*
     * See which action to take
     */
    switch ( *(params[0]) ) 
    {

      /* 
       * show-hide-list
       */
      case 's':
	comboArrowCB(cbw->combobox.arrow, NULL, NULL);
	break;

      case 'h': /* hide-list */
	hideList(cbw);
	break;

      case 'u': /* up                */
      case 'd': /* down              */
      case 't': /* top               */
      case 'b': /* bottom            */
      case 'p': /* page-up/page-down */

      	if ( !XtIsManaged(cbw->combobox.popup) )
		break;

	opt = *(params[0]);

        XtVaGetValues(list,
	 	XmNitems, &items, 
		XmNitemCount, &listCount,
	        XmNtopItemPosition, &topPos,
                XmNvisibleItemCount, &visibleItems, NULL);

	if ( XmListGetSelectedPos(list, &selectionList, &selectionCount) ) 
	{
	    selectionIndex = *selectionList;
	    XtFree((char *)selectionList);
	    switch ( opt ) 
	    {
		case 'u': selectionIndex--;		  break;
		case 'd': selectionIndex++;		  break;
		case 't': selectionIndex = 1;		  break;
		case 'b': selectionIndex = listCount;	  break;

		case 'p': 

		    if ( *(params[0]+5) == 'u' )
		          selectionIndex -= visibleItems;
		    else
		          selectionIndex += visibleItems;

		    selectionIndex = (selectionIndex > listCount) ?
			listCount : (selectionIndex < 1) ? 1 : selectionIndex;	

		    break;
	    }
	} 
	else 
	{
	    if ( opt == 'b' ) 
		selectionIndex = listCount;
	    else
	        selectionIndex = 1;
	}

	if ( selectionIndex < 1 || selectionIndex > listCount )
		break;

	updateListAndText(cbw, selectionIndex,
                items[selectionIndex - 1], False);

	if ( selectionIndex < topPos )
		XmListSetPos(list, selectionIndex);
	else if ( selectionIndex > topPos + visibleItems - 1 )
		XmListSetBottomPos(list, selectionIndex);

	break;



      /*
       * Return = activate
       */
      case 'a':
	if ( !XtIsManaged(cbw->combobox.popup) ) 
		break;

	XtVaGetValues(list, XmNitemCount, &listCount, 
	 	XmNitems, &items, NULL);
	if ( listCount == 0 ) 
		break;

	if ( XmListGetSelectedPos(list, &selectionList, &selectionCount) ) 
	{
	    selectionIndex = *selectionList;
	    XtFree((char *)selectionList);
	} 
	else 
	    selectionIndex = 1;
	
	hideList(cbw);

	updateListAndText(cbw, selectionIndex, 
		items[selectionIndex - 1], True);

	break;


      /*
       * Cancel
       */
      case 'c':
	if ( XtIsManaged(cbw->combobox.popup) )
	    hideList(cbw);
	else
	    XtCallActionProc(cbw->combobox.text, 
		"process-cancel", event, NULL, 0);
	break;


      /*
       * No operation
       */
      case 'n':
        break;
    }
} 


