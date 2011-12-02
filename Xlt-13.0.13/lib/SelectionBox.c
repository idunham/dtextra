/**
 *
 * $Id: SelectionBox.c,v 1.6 2001/06/02 09:27:45 amai Exp $
 *
 * Copyright (C) 1996 Free Software Foundation, Inc.
 * Copyright © 1999-2001 by the LessTif developers.
 *
 * This file is part of the GNU LessTif Extension Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **/
 
#include "../config.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <Xm/DialogS.h>
#include <Xm/TextF.h>
#include <Xm/List.h>
#include <Xm/SelectioB.h>
#include <Xm/SelectioBP.h>

#include <SelectionBoxP.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#undef DEBUG

static const char rcsid[] = "$Id: SelectionBox.c,v 1.6 2001/06/02 09:27:45 amai Exp $";

/*
   Widget methods, forward declarations
 */

static void initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args);

/*
   Helper functions, forward declarations
 */

/*
   Widget default resources
 */

#define Offset(field) XtOffsetOf(XltSelectionBoxRec, xlt_selection_box.field)
static XtResource resources[] =
{
    {
	XltNmagicScroll, XltCMagicScroll, XtRBoolean,
	sizeof(Boolean), Offset(magic_scroll),
	XtRImmediate, (XtPointer)True
    },
};

#undef Offset

/*
   Widget class record
 */
/* *INDENT-OFF* */
XltSelectionBoxClassRec xltSelectionBoxClassRec = {
    /* Core Class Part */
    {
    /* superclass            */ (WidgetClass) & xmSelectionBoxClassRec,
    /* class_name            */ "XltSelectionBox",
    /* widget_size           */ sizeof(XltSelectionBoxRec),
    /* class_initialize      */ NULL /*class_initialize*/,
    /* class_part_initialize */ NULL /*class_part_initialize*/,
    /* class_inited          */ False,
    /* initialize            */ initialize,
    /* initialize_hook       */ NULL,
    /* realize               */ XtInheritRealize /*realize*/,
    /* actions               */ NULL /*actions*/,
    /* num_actions           */ 0 /*XtNumber(actions)*/,
    /* resources             */ resources,
    /* num_resources         */ XtNumber(resources),
    /* xrm_class             */ NULLQUARK,
    /* compress_motion       */ True,
    /* compress_exposure     */ XtExposeCompressMultiple,
    /* compress_enterleave   */ True,
    /* visible_interest      */ True,
    /* destroy               */ NULL /*destroy*/,
    /* resize                */ XtInheritResize,
    /* expose                */ XtInheritExpose,
    /* set_values            */ NULL /*set_values*/,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL /*get_values_hook*/,
    /* accept_focus          */ NULL,
    /* version               */ XtVersion,
    /* callback offsets      */ NULL,
    /* tm_table              */ XtInheritTranslations,
    /* query_geometry        */ XtInheritQueryGeometry,
    /* display_accelerator   */ XtInheritDisplayAccelerator,
    /* extension             */ NULL
    },
    /* Composite Class Part */
    {
    /* geometry manager */	XtInheritGeometryManager,
    /* change_managed   */	XtInheritChangeManaged,
    /* insert_child     */	XtInheritInsertChild,
    /* delete_child     */	XtInheritDeleteChild,
    /* extension        */	NULL,
    },
    /* Constraint Class Part */
    {
    /* subresources      */ NULL,
    /* subresource_count */ 0,
    /* constraint_size   */ 0,
    /* initialize        */ NULL /*constraint_initialize*/,
    /* destroy           */ NULL,
    /* set_values        */ NULL /*constraint_set_values*/,
    /* extension         */ NULL,
    },
    /* Manager Class Part */
    {
    /* translations           */ XmInheritTranslations,
    /* syn_resources          */ NULL /*syn_resources*/,
    /* num_syn_resources      */ 0 /*XtNumber(syn_resources)*/,
    /* syn_constraint_res     */ NULL,
    /* num_syn_constraint_res */ 0,
    /* parent_process         */ XmInheritParentProcess,
    /* extension              */ NULL
    },
    /* XmBulletinBoard part */
    {
    /* always_install_accelerators  */ True,
    /* geo_matrix_create            */ XmInheritGeoMatrixCreate,
    /* focus_moved_proc             */ XmInheritFocusMovedProc,
    /* extension */ NULL
    },
    /* XmSelectionBox part */
    {
    /* list_callback */ XmInheritCallbackProc,
    /* extension */ NULL
    },
    /* XltSelectionBox Class Part */
    {
    /* extension */ NULL
    }
};
/* *INDENT-ON* */





WidgetClass xltSelectionBoxWidgetClass = (WidgetClass)&xltSelectionBoxClassRec;

/*
   Helper routines
 */

static void
ScrollToMatch(Widget List, int i)
{
int itemCount;
int visibleItemCount;
int topItemPosition;

    XtVaGetValues(List,
	XmNtopItemPosition, &topItemPosition,
	XmNvisibleItemCount, &visibleItemCount,
	XmNitemCount, &itemCount,
	NULL);
    if (itemCount > visibleItemCount &&
	(i + 1 < topItemPosition || i + 1 >= topItemPosition + visibleItemCount))
    {
	if (i < itemCount - visibleItemCount)
	{
	    XmListSetPos(List, i + 1);
	}
	else
	{
	    XmListSetPos(List, itemCount - visibleItemCount + 1);
	}
    }
			XmListSelectPos(List, i + 1, False);
}

static void
ScrollList(Widget Text, XtPointer client_data, XmAnyCallbackStruct *cbs)
{
    if (SelectionBox_MagicScroll(XtParent(Text)) && cbs->reason == XmCR_VALUE_CHANGED &&
        cbs->event && cbs->event->type == KeyPress)
    {
    Widget List = XmSelectionBoxGetChild(XtParent(Text), XmDIALOG_LIST);
    String NewText = XmTextFieldGetString(Text);
    String Item;
    XmStringTable items;
    int itemCount;
    int i;

	if (List && strlen(NewText) > 0)
	{
	    XtVaGetValues(List,
		    XmNitemCount, &itemCount,
		    XmNitems, &items,
		    NULL);
	    for (i = 0; i < itemCount; i++)
	    {
		if (XmStringGetLtoR(items[i], XmFONTLIST_DEFAULT_TAG, &Item))
		{
		    if (strstr(Item, NewText) == Item)
		    {
			ScrollToMatch(List, i);
			break;
		    }
		    XtFree(Item);
		}
	    }
	    if (i >= itemCount)
	    {
	    char *tmp;
	    String NewTextU = XtNewString(NewText);

		tmp = NewTextU;
		while (tmp[0])
		{
		    tmp[0] = toupper(tmp[0]);
		    tmp++;
		}
		for (i = 0; i < itemCount; i++)
		{
		    if (XmStringGetLtoR(items[i], XmFONTLIST_DEFAULT_TAG, &Item))
		    {
			tmp = Item;
			while (tmp[0])
			{
			    tmp[0] = toupper(tmp[0]);
			    tmp++;
			}
			if (strstr(Item, NewTextU) == Item)
			{
			    ScrollToMatch(List, i);
			    break;
			}
			XtFree(Item);
		    }
		}
		XtFree(NewTextU);
	    }
	    if (i >= itemCount)
	    {
		for (i = 0; i < itemCount; i++)
		{
		    if (XmStringGetLtoR(items[i], XmFONTLIST_DEFAULT_TAG, &Item))
		    {
			if (strstr(Item, NewText) != NULL)
			{
			    ScrollToMatch(List, i);
			    break;
			}
			XtFree(Item);
		    }
		}
	    }
	    if (i >= itemCount)
	    {
	    char *tmp;
	    String NewTextU = XtNewString(NewText);

		tmp = NewTextU;
		while (tmp[0])
		{
		    tmp[0] = toupper(tmp[0]);
		    tmp++;
		}
		for (i = 0; i < itemCount; i++)
		{
		    if (XmStringGetLtoR(items[i], XmFONTLIST_DEFAULT_TAG, &Item))
		    {
			tmp = Item;
			while (tmp[0])
			{
			    tmp[0] = toupper(tmp[0]);
			    tmp++;
			}
			if (strstr(Item, NewTextU) != NULL)
			{
			    ScrollToMatch(List, i);
			    break;
			}
			XtFree(Item);
		    }
		}
		XtFree(NewTextU);
	    }
	}

	XtFree(NewText);
    }
}


/* ******************** */

/*
   Widget methods
 */

static void
initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
    XtAddCallback(XmSelectionBoxGetChild(new_w, XmDIALOG_TEXT), 
	    XmNvalueChangedCallback, (XtCallbackProc)ScrollList, NULL);
}

/*
   Synthetic resource hooks
 */

/*
   Public functions
 */

Widget
XltCreateSelectionBox(Widget parent,
		  char *name,
		  Arg *arglist,
		  Cardinal argCount)
{
    return XtCreateWidget(name, xltSelectionBoxWidgetClass, parent, arglist, argCount);
}

Widget
XltVaCreateSelectionBox(Widget parent, char *name,...)
{
    Widget w;
    va_list args;

    va_start(args, name);
    w = XtVaCreateWidget(name, xltSelectionBoxWidgetClass, parent, args);
    va_end(args);
    return (w);
}

static String
MakeDialogName(String name)
{
    String s;

    s = XtMalloc((name ? strlen(name) : 0) + strlen(XmDIALOG_SUFFIX) + 1);
    if (name)
    {
	strcpy(s, name);
    }
    else
    {
	s[0] = '\0';
    }

    strcat(s, XmDIALOG_SUFFIX);

    return s;
}

Widget
XltCreateSelectionDialog(Widget parent, char *name,
			Arg *argList, Cardinal argcount)
{
    Widget d, r;
    char *s;
    Arg *al = (Arg *)XtCalloc(argcount + 1, sizeof(Arg));
    int ac;
    XtArgVal i;

    s = MakeDialogName(name);

    ac = 0;
    XtSetArg(al[ac], XmNallowShellResize, True); ac++;
    for (i = 0; i < (XtArgVal)argcount; i++)
    {
	XtSetArg(al[ac], argList[i].name, argList[i].value); ac++;
    }

    d = XmCreateDialogShell(parent, s, al, ac);
    XtFree(s);

    r = XtCreateWidget(name, xltSelectionBoxWidgetClass, d, al, ac);
    XtFree((XtPointer)al);
    return r;
}

