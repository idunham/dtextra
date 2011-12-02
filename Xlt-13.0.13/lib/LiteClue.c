/* 
LiteClue.c - LiteClue widget
	See LiteClue documentation

Copyright 1996 COMPUTER GENERATION, INC.,

The software is provided "as is", without warranty of any kind, express
or implied, including but not limited to the warranties of
merchantability, fitness for a particular purpose and noninfringement.
In no event shall Computer Generation, inc. nor the author be liable for
any claim, damages or other liability, whether in an action of contract,
tort or otherwise, arising from, out of or in connection with the
software or the use or other dealings in the software.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation.

Author:
Gary Aviv 
Computer Generation, Inc.,
gary@compgen.com

*/
/* Revision History:
$Log: LiteClue.c,v $
Revision 1.2  2005/05/18 22:15:59  rwscott
There goes the time I was going to use to add some stuff. Instead
I spent it getting distcheck to work .....

Revision 1.1  2005/04/18 15:34:52  dannybackx
Add the old version of LiteClue to keep track of the original verson.
Michel will be updating this soon.


$log
$log
*/
#include <unistd.h>
#include <signal.h>
/* #include <Xm/XmP.h> */
#include <X11/IntrinsicP.h> 
#include <X11/StringDefs.h>

#include "LiteClueP.h"

#include <stdio.h>

#define CheckWidgetClass(routine) \
	if (XtClass(w) != xcgLiteClueWidgetClass) \
		wrong_widget(routine)

/* extern _XmSelectColorDefault();	*//* cgi */
static Boolean setValues( Widget _current, Widget _request, Widget _new);
static void Initialize(Widget treq, Widget tnew, ArgList args, Cardinal *num_args);

/* keep information about each widget we are keeping track of */
struct liteClue_context_str
{
	ListThread next;	/* next in list */
	Widget watched_w;	/* the widget we are watching */
	XcgLiteClueWidget cw;	/* pointer back to the liteClue widget */
	Position  abs_x, abs_y;
	Boolean sensitive;	/* if False, liteClue is suppressed */
	char * text;		/* text to display */
	short text_size;	/* its size */
};

/*
Widget resources: eg to set LiteClue box background:
 *XcgLiteClue.background: yellow
       
*/
#define offset(field) XtOffsetOf(LiteClueRec, field)
static XtResource resources[] =
{
	{XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
		offset(liteClue.foreground), XtRString, "black"},
	{XtNfontSet, XtCFontSet, XtRFontSet, sizeof(XFontSet),
		offset(liteClue.fontset), XtRString, "-adobe-new century schoolbook-bold-r-normal-*-12-*"},
};

#undef offset

#if 0
static XtActionsRec actions[] = 
}; /* actions */
#endif


LiteClueClassRec xcgLiteClueClassRec =
{
    {
	(WidgetClass)&overrideShellClassRec,	/* superclass */
	"XcgLiteClue",				/* class_name */
	(Cardinal)sizeof(LiteClueRec),		/* widget size */
	NULL,	/* classInit,			*//* class_init */
	(XtWidgetClassProc)NULL,	/* classPartInit,	*//* class_part_init */
	(XtEnum)FALSE,				/* class_inited */
	(XtInitProc)Initialize,			/* initialize */
	(XtArgsProc)NULL,			/* init_hook */
	XtInheritRealize,			/* realize */
	(XtActionList)0,			/* actions */
	(Cardinal)0,			/* num_actions */
	(XtResourceList)resources,		/* resources */
	(Cardinal)XtNumber(resources),		/* num_resources */
	NULLQUARK,				/* xrm_class */
	TRUE,					/* compress_motion */
	(XtEnum)FALSE,				/* compress_exposur */
	TRUE,					/* compress enterleave */
	FALSE,					/* visibility_interest */
	(XtWidgetProc)NULL,	/* destroy,			*//* destroy */
	XtInheritResize,
	XtInheritExpose,	/* expose, */
	(XtSetValuesFunc)setValues,		/* set_values */
	(XtArgsFunc)NULL,			/* set_values_hook */
	XtInheritSetValuesAlmost,		/* set_values_almost */
	(XtArgsProc)NULL,			/* get_values_hook */
	XtInheritAcceptFocus,		/* accept_focus */
	XtVersion,				/* version */
	(XtPointer)NULL,			/* callback_private */
	XtInheritTranslations,
	XtInheritQueryGeometry,			/* query_geometry */
	XtInheritDisplayAccelerator,		/* display_accelerator */
	(XtPointer)0,			/* extension */
    },
    { /*** composite-Class ***/
	XtInheritGeometryManager,	/* geometry_manager   	*/    	
	XtInheritChangeManaged,	/* change_managed		*/	
	XtInheritInsertChild,	/* insert_child		*/	
	XtInheritDeleteChild,	/* delete_child		*/	
	NULL	/* extension		*/	
    }, 
	{ /* Shell */
	(XtPointer) NULL,       /* pointer to extension record      */
	},
	/* Override Shell */
	{
	0,
	},
	/* LiteClue */
	{
	0,
	},
};

WidgetClass xcgLiteClueWidgetClass = (WidgetClass) & xcgLiteClueClassRec;

/* doubly linked list processing */

/*
	 initialize header - both pointers point to it
*/
static void xcgListInit(ListThread *newbuf)
{
	newbuf->back = newbuf;
	newbuf->forw = newbuf;
}

/*
	 insert newbuf after posbuf
*/
#if 0
static void xcgListInsert(ListThread *newbuf, ListThread *posbuf)
{
	ListThread *nextbuf;

	nextbuf = posbuf->forw;

	posbuf->forw = newbuf;
	newbuf->back = posbuf;
	newbuf->forw = nextbuf;
	nextbuf->back = newbuf;
}
#endif

/*
	 insert newbuf before posbuf
*/
static void xcgListInsertBefore(ListThread *newlist, ListThread *poslist)
{
	ListThread *prevbuf;

	prevbuf = poslist->back;

	poslist->back = newlist;
	newlist->forw = poslist;
	newlist->back = prevbuf;
	prevbuf->forw = newlist;
}


/*
	remove rembuf from queue
*/
static ListThread * xcgListRemove(ListThread *rembuf)
{
	ListThread *prevbuf, *nextbuf;

	prevbuf = rembuf->back;
	nextbuf = rembuf->forw;

	prevbuf->forw = nextbuf;
	nextbuf->back = prevbuf;		

	rembuf->back = (ListThread *) NULL;	/* for safety to cause trap if ..*/
	rembuf->forw = (ListThread *) NULL;	/* .. mistakenly refed */
	return rembuf;
}

#if 0
/*
	replace element of a queue by another 
	pointers of old are copied to new while all pointers to old
	are re-directed at new. In effect, old is removed and new
	is inserted in its place.
*/
static void xcgListReplace(ListThread *old, ListThread *new)
{
	ListThread *prevbuf, *nextbuf;

	new->back = prevbuf = old->back;
	new->forw = nextbuf = old->forw;

	prevbuf->forw = new;
	nextbuf->back = new;		

	old->back = (ListThread *) NULL;	/* for safety to cause trap if ..*/
	old->forw = (ListThread *) NULL;	/* .. mistakenly refed */
	
}
#endif


/*
The font_information is derived 
*/

static void
compute_font_info(XcgLiteClueWidget cw)
{
	XRectangle ink;
	XRectangle logical;

	if (!cw->liteClue.fontset)
		return;
	XmbTextExtents(cw->liteClue.fontset, "1", 1,&ink, &logical);

	cw->liteClue.font_baseline = -logical.y;	/* y offset from top to baseline, 
			don't know why this is returned as negative */
	cw->liteClue.font_width = logical.width;	/* the width and height of the object */
	cw->liteClue.font_height = logical.height;
}

/*
 Creates the various graphic contexts we will need 
*/
static void create_GC(XcgLiteClueWidget cw )
{
	XtGCMask valuemask;
	XGCValues myXGCV;


	valuemask = GCForeground | GCBackground | GCFillStyle ;
	myXGCV.foreground = cw->liteClue.foreground;
	myXGCV.background = cw->core.background_pixel;
	myXGCV.fill_style = FillSolid; 

	if (cw->liteClue.text_GC )
		XtReleaseGC((Widget) cw, cw->liteClue.text_GC );
	cw->liteClue.text_GC = XtGetGC((Widget)cw, valuemask, &myXGCV);
}


/* a routine to halt execution and force  
a core dump for debugging analysis	
when a public routine is called with the wrong class of widget
*/
static void wrong_widget(char * routine)
{
	int mypid = getpid(); 
	fprintf(stderr, "Wrong class of widget passed to %s\n", routine);
	fflush(stderr); 
	kill(mypid, SIGABRT); 
}

/*
Find the target in the widget list. Return context pointer if found,
NULL if not
*/
static struct liteClue_context_str * find_watched_widget(XcgLiteClueWidget cw,
	Widget target)
{
	struct liteClue_context_str * obj;

	for (obj = (struct liteClue_context_str *) cw->liteClue.widget_list.forw; 
		obj != (struct liteClue_context_str *) & cw->liteClue.widget_list; 
		obj = (struct liteClue_context_str *)obj->next.forw )
	{
		if (target == obj->watched_w)
			return obj;
	}
	return NULL;
}

/*
	allocate and initialize a widget context
*/
struct liteClue_context_str * alloc_liteClue_context(void)
{
	struct liteClue_context_str * out;
	out = (struct liteClue_context_str *) XtMalloc(sizeof(struct liteClue_context_str));
	memset(out, 0, sizeof(struct liteClue_context_str));
	xcgListInit(&out->next);	
	return out ;
}

/*
	allocate, initialize and link a liteClue context to the list
*/
static struct liteClue_context_str * alloc_link_liteClue_context(XcgLiteClueWidget cw )
{
	struct liteClue_context_str * out = alloc_liteClue_context();

	/* link as new last */
	xcgListInsertBefore(&out->next, &cw->liteClue.widget_list);
	out->cw = cw;	/* initialize this emeber - its always the same */
	return out;
}

/*
	free a widget context
*/
void free_widget_context(XcgLiteClueWidget cw, struct liteClue_context_str * obj)
{
	xcgListRemove((ListThread *)obj);
	/* free up all things object points to */
	if (obj->text )
		XtFree(obj->text);
	XtFree((char *) obj);
}


/* -------------------- Widget Methods ---------------------- */
/* Initialize method */
static void Initialize(Widget treq, Widget tnew, ArgList args, 
Cardinal *num_args)
{
	XcgLiteClueWidget cw = (XcgLiteClueWidget) tnew;

	cw->liteClue.text_GC = NULL;
	xcgListInit(&cw->liteClue.widget_list);	/* initialize empty list */
	compute_font_info(cw);
	create_GC(cw );
}

static Boolean setValues( Widget _current, Widget _request, Widget _new)
{
	return FALSE;
}

/* ----------------- Event handlers ------------------------*/

static void Enter_event(Widget w, XtPointer client_data, XEvent * event, Boolean * continue_to_dispatch )
{
#define BorderPix 2
	struct liteClue_context_str * obj = (struct liteClue_context_str *) client_data;
	XcgLiteClueWidget cw = obj->cw;
	Position  abs_x, abs_y;
	XRectangle ink;
	XRectangle logical;
	Position   w_height;	

	if (obj->sensitive == False)
		return;

	XtVaGetValues(w, XtNheight, &w_height, NULL );
	/* position just below the widget */
	XtTranslateCoords(w, 0, w_height, &abs_x, &abs_y);

	XmbTextExtents(cw->liteClue.fontset, obj->text , obj->text_size ,&ink, &logical);

	XtResizeWidget((Widget) cw, 2*BorderPix +logical.width, 
			2*BorderPix + cw->liteClue.font_height, cw->core.border_width );
	XtMoveWidget((Widget) cw, abs_x, abs_y );
	XtPopup((Widget) cw, XtGrabNone);
	XmbDrawImageString(XtDisplay((Widget) cw), XtWindow((Widget) cw), 
		cw->liteClue.fontset,
		cw->liteClue.text_GC , BorderPix, 
		BorderPix + cw->liteClue.font_baseline, obj->text , obj->text_size);
}

static void Leave_event(Widget w, XtPointer client_data, XEvent * event, Boolean * continue_to_dispatch )
{
	struct liteClue_context_str * obj = (struct liteClue_context_str *) client_data;
	XcgLiteClueWidget cw = obj->cw;

	if (obj->sensitive == False)
		return;
	XtPopdown((Widget) cw);
}

/* ---------------- Widget API ---------------------------- */

/*
;+
XcgLiteClueAddWidget -- Add a widget to be watched. LiteClue will be given for this widget

Func:	A widget will be added to the LiteClue watched list. Clues are given for
	sensitive watched widgets when the pointer enters its window. If the
	widget is already watched, the passed text replaces its current clue
	text. If text is null, the widget is still added, if it is not already
	in the list, but no clue will appear. Text may be specified with
	XcgLiteClueAddWidget in a subsequent call. When text is null and the
	widget is already in the list, its text is not changed. When a widget
	will is added to the watched list, it automatically becomes sensitive.
	Otherwise, its sensitivity is not changed. A watched widget which is not
	sensitive retains its context but clues are suppressed.
	None of this affects the behaviour of the watched widget itself.
	LiteClue monitors enter and leave events of the watched widget's
	window passively.

Input:	w - LiteClue widget
	watch - the widget to give liteClues for
	text - pointer to liteClue text. (May be NULL)
	size - size of text. May be zero
		in which case a strlen will be done.
	option - option mask, future use, zero for now.
Output: 

Return:	

;-
*/
void XcgLiteClueAddWidget(Widget w, Widget watch,  char * text, int size, int option )
{
#	define ROUTINE "XcgLiteClueAddWidget"
	XcgLiteClueWidget cw = (XcgLiteClueWidget) w;
	struct liteClue_context_str * obj;
	Boolean exists = False;

	CheckWidgetClass(ROUTINE);	/* make sure we are called with a LiteClue widget */

	obj = find_watched_widget(cw, watch);
	if (obj)
	{
		exists = True;
		if (text)
		{
			if(obj->text)
				XtFree(obj->text);
			obj->text = NULL;
		}
	}
	else
	{
		obj = alloc_link_liteClue_context(cw );
		obj->watched_w = watch;
	}
	if (text && !(obj->text))
	{
		if (!size)
			size = strlen(text);
		obj->text = XtMalloc(size+1);
		memcpy(obj->text, text, size);
		obj->text[size] = 0;
		obj->text_size = size;
	}
	if (!exists)	/* was created */
	{
		XtAddEventHandler(watch, EnterWindowMask, False, 
			Enter_event, (XtPointer) obj);
		XtAddEventHandler(watch, LeaveWindowMask, False, 
			Leave_event, (XtPointer) obj);
		obj->sensitive = True;
	}

#	undef ROUTINE
}


/*
;+
XcgLiteClueDeleteWidget -- Delete a widget that is watched. 

Func:	A widget is deleted from the watched list and its resources are
	freed. LiteClue is no longer given for the widget.
	If the widget is not watched, nothing is done.

Input:	w - LiteClue widget
	watch - the widget to delete
Output: 

Return:	

;-
*/
void XcgLiteClueDeleteWidget(Widget w, Widget watch)
{
#	define ROUTINE "XcgLiteClueDeleteWidget"
	XcgLiteClueWidget cw = (XcgLiteClueWidget) w;
	struct liteClue_context_str * obj;

	CheckWidgetClass(ROUTINE);	/* make sure we are called with a LiteClue widget */
	obj = find_watched_widget(cw, watch);
	if (obj)
	{
		XtRemoveEventHandler(watch, EnterWindowMask, False, 
			Enter_event, (XtPointer) obj);
		XtRemoveEventHandler(watch, LeaveWindowMask, False, 
			Leave_event, (XtPointer) obj);
		free_widget_context(cw, obj);
	}

#	undef ROUTINE
}


/*
;+
XcgLiteClueSetSensitive -- Enable/disable sensitivity for watched widget. 

Func:	When a watched widget is sensitive, a clue is poped up when the pointer
	enters its window. When a watched widget is insensitive, the widget is
	retained in the watched list but no clue is poped. The sensitivity of a
	watched widget relative to clues is set or reset by this function. The
	Xt sensitivity of the watched widget is not altered by this function.

Input:	w - LiteClue widget
	watch - the widget to make sensitive or insensitive or NULL
		to change all watched widgets
	sensitive - True or False
Output: 

Return:	

;-
*/
void XcgLiteClueSetSensitive(Widget w, Widget watch, Boolean sensitive)
{
#	define ROUTINE "XcgLiteClueSetSensitive"
	XcgLiteClueWidget cw = (XcgLiteClueWidget) w;
	struct liteClue_context_str * obj;

	CheckWidgetClass(ROUTINE);	/* make sure we are called with a LiteClue widget */
	if (watch)
	{
		obj = find_watched_widget(cw, watch);
		if (obj)
		{
			obj->sensitive = sensitive;
			return;
		}
		else
			return;
	}

	/* do them all */
	for (obj = (struct liteClue_context_str *) cw->liteClue.widget_list.forw; 
		obj != (struct liteClue_context_str *) & cw->liteClue.widget_list; 
		obj = (struct liteClue_context_str *)obj->next.forw )
	{
		obj->sensitive = sensitive;
	}

#	undef ROUTINE
}

/*
;+
XcgLiteClueGetSensitive -- Get sensitivity mode for watched widget. 

Func:	When a watched widget is sensitive, a clue is poped up when the pointer
	enters its window. When a watched widget is insensitive, the widget is
	retained in the watched list but no clue is poped. The sensitivity state
	of a watched widget relative to clues is returned by this function. The
	Xt sensitivity of a widget is a totally independent concept.

Input:	w - LiteClue widget
	watch - the widget for which to get sensitivity state. If NULL
		first watched widget is used. If there are no watched widgets,
		False is returned.
Output: 

Return:	sensitive - True or False

;-
*/
Boolean XcgLiteClueGetSensitive(Widget w, Widget watch)
{
#	define ROUTINE "XcgLiteClueGetSensitive"
	XcgLiteClueWidget cw = (XcgLiteClueWidget) w;
	struct liteClue_context_str * obj;

	CheckWidgetClass(ROUTINE);	/* make sure we are called with a LiteClue widget */
	if (watch)
	{
		obj = find_watched_widget(cw, watch);
		if (obj)
			return obj->sensitive;
		else
			return False;
	}
	/* do the first one */
	obj = (struct liteClue_context_str *) cw->liteClue.widget_list.forw; 
	if (obj != (struct liteClue_context_str *) & cw->liteClue.widget_list)
		return obj->sensitive;
	else
		return False;

#	undef ROUTINE
}

