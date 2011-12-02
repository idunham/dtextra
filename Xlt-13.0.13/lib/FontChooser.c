/**
 *
 * $Header: /cvsroot/xlt/Xlt/lib/FontChooser.c,v 1.19 2001/06/02 09:27:45 amai Exp $
 *
 * Copyright © 1999, 2001 by the LessTif developers.
 *
 * This file is part of the LessTif Extension Library (Xlt).
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

static const char rcsid[] = "$Header: /cvsroot/xlt/Xlt/lib/FontChooser.c,v 1.19 2001/06/02 09:27:45 amai Exp $";

#include <../config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Xm/DialogS.h>
#include <Xm/PushBG.h>
#include <Xm/XmP.h>
#include <Xm/MessageBP.h>
#include <Xm/RowColumn.h>
#include <Xm/LabelG.h>

#include <FontChooser.h>
#include <FontChooserP.h>


#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

/* Forward Declarations */

static void class_part_initialize(WidgetClass w_class);
static void initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args);
static Boolean set_values(Widget current, Widget request, Widget new_w, ArgList args, Cardinal *num_args);
static void Destroy(Widget w);
static void Realize(Widget aw, XtValueMask * value_mask, XSetWindowAttributes * attributes);

/*
 * Resources for the FontChooser class
 */
#define Offset(field) XtOffsetOf(XltFontChooserRec, font_chooser.field)
static XtResource resources[] =
{
    {
	XltNshowDemo, XltCShowDemo, XmRBoolean,
	sizeof(Boolean), Offset(show_demo),
	XmRImmediate, (XtPointer)True
    },
    {
	XltNcountString, XltCCountString, XmRString,
	sizeof(String), Offset(count_string),
	XmRString, (XtPointer)"%d matching fonts"
    },
    {
	XltNdemoString, XltCDemoString, XmRString,
	sizeof(String), Offset(demo_string),
	XmRString, (XtPointer)"The quick brown fox jumps over the lazy dog."
    },
    {
	XltNfieldNames, XltCFieldNames, XmRString,
	sizeof(String), Offset(field_names),
	XmRString, (XtPointer)"Foundry,Family,Weight,Slant,Width,Style,Pixels,Points,Hres,Vres,Spacing,Avgwidth,Charset,Encoding"
    },
    {
	XltNfontName, XltCFontName, XmRString,
	sizeof(String), Offset(font_name),
	XmRString, (XtPointer)NULL
    },
    {
	XltNpattern, XltCPattern, XmRString,
	sizeof(String), Offset(pattern),
	XmRString, (XtPointer)"-*-*-*-*-*-*-*-*-*-*-*-*-*-*"
    },
    {
	XltNsortCriteria, XltCSortCriteria, XmRString,
	sizeof(String), Offset(sort_criteria),
	XmRString, (XtPointer)"aaaaaannnnanaa"
    },
};

#undef	Offset

XltFontChooserClassRec xltFontChooserClassRec =
{
    /* Core class part */
    {
	/* superclass            */ (WidgetClass)&xmMessageBoxClassRec,
	/* class_name            */ "XltFontChooser",
	/* widget_size           */ sizeof(XltFontChooserRec),
	/* class_initialize      */ NULL,
	/* class_part_initialize */ class_part_initialize,
	/* class_inited          */ False,
	/* initialize            */ initialize,
	/* initialize_hook       */ NULL,
	/* realize               */ Realize,
	/* actions               */ NULL,
	/* num_actions           */ 0,
	/* resources             */ resources,
	/* num_resources         */ XtNumber(resources),
	/* xrm_class             */ NULLQUARK,
	/* compress_motion       */ False,
	/* compress_exposure     */ XtExposeCompressMaximal,
	/* compress_enterleave   */ False,
	/* visible_interest      */ False,
	/* destroy               */ Destroy,
	/* resize                */ XtInheritResize,
	/* expose                */ XtInheritExpose,
	/* set_values            */ set_values,
	/* set_values_hook       */ NULL,
	/* set_values_almost     */ XtInheritSetValuesAlmost,
	/* get_values_hook       */ NULL,
	/* accept_focus          */ NULL,
	/* version               */ XtVersion,
	/* callback offsets      */ NULL,
	/* tm_table              */ XtInheritTranslations,
	/* query_geometry        */ XtInheritQueryGeometry,
	/* display_accelerator   */ NULL,
	/* extension             */ NULL
    },
    /* Composite class part */
    {
	/* geometry manager */	XtInheritGeometryManager,
	/* change_managed   */	XtInheritChangeManaged,
	/* insert_child     */	XtInheritInsertChild,
	/* delete_child     */	XtInheritDeleteChild,
	/* extension        */	NULL,
    },
    /* Constraint class part */
    {
	/* subresources      */		NULL,
	/* subresource_count */		0,
	/* constraint_size   */		0,
	/* initialize        */		NULL,
	/* destroy           */		NULL,
	/* set_values        */		NULL,
	/* extension         */		NULL,
    },
    /* XmManager class part */
    {
	/* translations                 */	XtInheritTranslations,
	/* syn_resources                */	NULL,
	/* num_syn_resources            */	0,
	/* syn_constraint_resources     */	NULL,
	/* num_syn_constraint_resources */	0,
	/* parent_process               */	XmInheritParentProcess,
	/* extension                    */	NULL
    },
    /* XmBulletinBoard class part */
    {
	/* always_install_accelerators  */	True,
	/* geo_matrix_create            */	XmInheritGeoMatrixCreate,
	/* focus_moved_proc             */	XmInheritFocusMovedProc,
	/* extension                    */	NULL
    },
    /* XmMessageBox class part */
    {
	/* extension                    */ NULL
    },
    /* XltFontChooser class part */
    {
	/* ??? */			   NULL
    }
};


WidgetClass xltFontChooserWidgetClass = (WidgetClass)&xltFontChooserClassRec;

static void
class_part_initialize(WidgetClass widget_class)
{
}

struct xlfd_fields {
	int	num;
	char	**names;
};

static void
Remember(struct xlfd_fields **fields, int i, char *name)
{
	int	j;

	if (fields[i] == 0) {
		fields[i] = (struct xlfd_fields *)XtMalloc(sizeof(struct xlfd_fields));
		fields[i]->num = 0;
		fields[i]->names = 0;
	}

	for (j = 0; j < fields[i]->num; j++) {
		if (strcmp(fields[i]->names[j], name) == 0)
			return;
	}
	fields[i]->num++;
	fields[i]->names = (char **)XtRealloc((XtPointer)fields[i]->names,
			sizeof(char *) * fields[i]->num);
	fields[i]->names[fields[i]->num-1] = XtNewString(name);
}

static void
ShowCount(Widget fc, int num)
{
	Widget		countlabel;
	char		s[80];
	XmString	xms;

	countlabel = FC_CountWidget(fc);

	if (! FC_ShowDemo(fc))
		return;

	if (countlabel) {
		sprintf(s, FC_CountString(fc), num);
		xms = XmStringCreateSimple(s);
		XtVaSetValues(countlabel, XmNlabelString, xms, NULL);
		XmStringFree(xms);
	}
}

static void
DisableAllMenus(Widget fc, int column)
{
	int	i;
      Cardinal j;
	Widget	m, w;

	for (i=0; i<14; i++)
	    if (column != i) {
		m = FC_Menus(fc)[i];
		for (j=0; j<MGR_NumChildren(m); j++) {
			w = MGR_Children(m)[j];
			if (strcmp(XtName(w), "*") != 0
					&& ! XmIsTearOffButton(w))
				XtSetSensitive(MGR_Children(m)[j], False);
		}
	}
		
}

static void
EnableMenu(Widget fc, char *s)
{
	int	nogood, j, ii;
	char	*p, *q;
	char	field[14][64];	/* ??? FIX ME */
	Widget	b;

	nogood = 0;
	p = s;
	for (j=0, s; j<14; j++) {
		/* Scan for a dash and a string starting at p, store it in field */
		if (*p != '-') 
		{
		    fprintf(stderr, "%s: Can't scan font name '%s'\n", XtName(fc), s);
		    nogood = 1;
		    break;
		}
		p++;
		ii=0;
		field[j][ii++] = '*';
		for (q=p; *q && *q != '-' && ii < 64; q++, ii++)
			field[j][ii] = *q;
		field[j][ii] = '\0';

		b = XtNameToWidget(FC_Menus(fc)[j], field[j]);
		if (b)
			XtSetSensitive(b, True);

		if (*q)
			p = q;
		else
			break;
	}
}

static void
FCFilter(Widget w, XtPointer client, XtPointer call)
{
	int				i = (int)client, j, nfonts;
	Widget				fc, b, demo;
	char				*f = XtMalloc(256), **list;
	XmRowColumnCallbackStruct	*cbs = (XmRowColumnCallbackStruct *)call;

/* Figure out who we are and who called us */
#if 0
	for (fc = w; fc && ! XltIsFontChooser(fc); fc = XtParent(fc)) ;
	if (fc == NULL) {
		/* Probably in a tearoff */
		abort();
	}
#endif

	b = cbs->widget;		/* This widget invoked us */
	XtVaGetValues(b, XmNuserData, &fc, NULL);

	demo = FC_DemoWidget(fc);

/* Store the setting */
	if (FC_Setting(fc)[i] != NULL)
		XtFree(FC_Setting(fc)[i]);

	FC_Setting(fc)[i] = XtNewString(XtName(b));

	f[0] = '\0';
	for (j=0; j<14; j++) {
		strcat(f, "-");
		strcat(f, FC_Setting(fc)[j]);
	}

	if (FC_FontName(fc))
		XtFree(FC_FontName(fc));
	FC_FontName(fc) = f;

/* How many fonts ? */
	DisableAllMenus(fc, i);
	list = XListFonts(XtDisplay(fc), f, 4096, &nfonts);
	for (i=0; i<nfonts; i++)
		EnableMenu(fc, list[i]);
	XFreeFontNames(list);

/* Change the display */
	if (FC_ShowDemo(fc)) {
	    if (demo) {
		XmFontList      fl;
		XmFontListEntry fle;
		XFontSet        fs;
		char            m1[2][80], **missing = (char **)&m1[0][0];
		char            m2[80], *def = &m2[0];
		int             nmissing = 0;

		fs = XCreateFontSet(XtDisplay(fc), f, &missing, &nmissing, &def);

		if (fs != NULL) {
                	fle = XmFontListEntryCreate(XmFONTLIST_DEFAULT_TAG,
			         XmFONT_IS_FONTSET, (XtPointer)fs);
                	fl = XmFontListAppendEntry(NULL, fle);

			XtVaSetValues(demo, XmNfontList, fl, NULL);

			XmFontListEntryFree(&fle);
			XmFontListFree(fl);
#if 0
			/* Freeing the font set produces nice little core dumps */
			XFreeFontSet(XtDisplay(fc), fs);	/* FIX ME leak ?? */
#endif
        	}
	    }

	    ShowCount(fc, nfonts);
	}
}

static void
Realize(Widget aw, XtValueMask * value_mask, XSetWindowAttributes * attributes)
{
	XltFontChooserWidget	fc = (XltFontChooserWidget) aw;
	char			*f = XtMalloc(256), **list;
	int			i, j, nfonts;

#define	superclass	(&xmMessageBoxClassRec)
  (*superclass->core_class.realize) (aw, value_mask, attributes);
#undef	superclass

	if (! FC_ShowDemo(fc))
		return;

/* Show the right font initially */
	f[0] = '\0';
	for (j=0; j<14; j++) {
		strcat(f, "-");
		strcat(f, FC_Setting(fc)[j]);
	}

	if (FC_FontName(fc))
		XtFree(FC_FontName(fc));
	FC_FontName(fc) = f;

/* How many fonts ? */
	list = XListFonts(XtDisplay(fc), f, 4096, &nfonts);
	for (i=0; i<nfonts; i++)
		EnableMenu(aw, list[i]);
	XFreeFontNames(list);

	if (FC_DemoWidget(fc)) {
		XmFontList      fl;
		XmFontListEntry fle;
		XFontSet        fs;
		char            m1[2][80], **missing = (char **)&m1[0][0];
		char            m2[80], *def = &m2[0];
		int             nmissing = 0;

		fs = XCreateFontSet(XtDisplay(fc), f, &missing, &nmissing, &def);

		if (fs != NULL) {
                	fle = XmFontListEntryCreate(XmFONTLIST_DEFAULT_TAG,
			         XmFONT_IS_FONTSET, (XtPointer)fs);
                	fl = XmFontListAppendEntry(NULL, fle);

			XtVaSetValues(FC_DemoWidget(fc), XmNfontList, fl, NULL);

			XmFontListEntryFree(&fle);
			XmFontListFree(fl);
#if 0
			/* Freeing the font set produces nice little core dumps */
			XFreeFontSet(XtDisplay(fc), fs);	/* FIX ME leak ?? */
#endif
        	}

	}

	ShowCount(aw, nfonts);
}

static int qsort_helper_num(const void *p, const void *q)
{
	int	i, j, r;

	i = atoi(*(char **)p);
	j = atoi(*(char **)q);

	if (i == j)
		r = 0;
	else if (i < j)
		r = -1;
	else
		r = 1;

	return r;
}

static int qsort_helper_alpha(const void *p, const void *q)
{
	return strcmp(*(char **)p, *(char **)q);
}

static void
GetFullFontList(Widget w)
{
	Widget	b;
	int	i, j, nfonts, ii, nogood, ac;
	char	**list, *p, *q;
	char	field[14][64];	/* ??? FIX ME */
	struct xlfd_fields	**fields;
	Arg	al[2];

	/*
	 * Use userdata field to pass the FC widget id.
	 *
	 * We need this because we might get called from a tearoff,
	 * and in a tearoff we cannot walk up the widget tree to find
	 * the FontChooser.
	 */
	ac = 0;
	XtSetArg(al[ac], XmNuserData, (XtPointer)w); ac++;

	for (i=0; i<14; i++) {
		XtAddCallback(FC_Menus(w)[i], XmNentryCallback, FCFilter, (XtPointer)i);

		b = XmCreatePushButtonGadget(FC_Menus(w)[i], "*", al, ac);
		XtManageChild(b);

		XtVaSetValues(FC_Menus(w)[i], XmNmenuHistory, b, NULL);

		FC_Setting(w)[i] = XtNewString("*");
	}

	list = XListFonts(XtDisplay(w), FC_Pattern(w), 4096, &nfonts);
	fields = (struct xlfd_fields **)XtCalloc(14, sizeof(struct xlfd_fields *));

	for (i=0; i<nfonts; i++) {
		/* sscanf doesn't cope well with empty fields, so do this ourselves. */
		nogood = 0;
		for (j=0, p=list[i]; j<14; j++) {
			/* Scan for a dash and a string starting at p, store it in field */
			if (*p != '-') {
				fprintf(stderr, "%s: Can't scan font name '%s'\n",
					XtName(w), list[i]);
				nogood = 1;
				break;
			}
			p++;
			for (q=p, ii=0; *q && *q != '-' && ii < 64; q++, ii++)
				field[j][ii] = *q;
			field[j][ii] = '\0';
			if (*q)
				p = q;
			else
				break;
		}

		for (j=0; j<14; j++) {
			Remember(fields, j, field[j]);
		}
	}

	/* Create buttons */
	for (i=0; i<14; i++) {
		/*
		 * Sort according to criteria from resource.
		 */
		if (FC_SortCriteria(w) && strlen(FC_SortCriteria(w)) > 13) {
		    size_t	s, n;

		    s = sizeof(char *);
		    n = fields[i]->num;

		    if (FC_SortCriteria(w)[i] == 'n' ||  FC_SortCriteria(w)[i] == 'N') {
			qsort(fields[i]->names, n, s, qsort_helper_num);
		    } else if (FC_SortCriteria(w)[i] == 'a' ||  FC_SortCriteria(w)[i] == 'A') {
			qsort(fields[i]->names, n, s, qsort_helper_alpha);
		    } else {
			;	/* don't sort, silently */
		    }
		}

		for (j=0; j<fields[i]->num; j++) {
			b = XmCreatePushButtonGadget(FC_Menus(w)[i], fields[i]->names[j], al, ac);
			XtManageChild(b);
			XtFree(fields[i]->names[j]);
		}
		XtFree((XtPointer)fields[i]->names);
		XtFree((XtPointer)fields[i]);
	}

	XtFree((XtPointer)fields);
	XFreeFontNames(list);

	ShowCount(w, nfonts);
}

static void
Destroy(Widget w)
{
int i;

    if (FC_Menus(w))
    {
	XtFree((XtPointer)FC_Menus(w));
    }
    for (i=0; i<14; i++)
    {
	if (FC_Setting(w)[i])
	{
	    XtFree(FC_Setting(w)[i]);
	}
    }
    XtFree((char *)FC_Setting(w));
}

static void
FitMenuOnScreen(Widget menu, XtPointer client, XtPointer call)
{
    if (XtHeight(menu) > HeightOfScreen(XtScreen(menu)))
    {
    int height;
    short numColumns;

    	XtVaGetValues(menu,
    		XmNnumColumns, &numColumns,
    		NULL);
	height = XtHeight(menu) * numColumns;
	numColumns = (height / HeightOfScreen(XtScreen(menu))) + 1;
    	XtVaSetValues(menu,
    		XmNnumColumns, numColumns,
    		XmNpacking, XmPACK_COLUMN,
    		NULL);
    }
}

static void
initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
	Widget		option = NULL, menu = NULL, demo, form, top = NULL, count_label;
	Arg		al[10];
	int		ac, i;
	XmString	xms;
	char		*p, *q, *fn, *n;

	MB_DialogType(new_w) = XmDIALOG_MESSAGE;

	ac = 0;
	form = XmCreateForm(new_w, "form", al, ac);

	p = fn = strdup(FC_FieldNames(new_w));
	n = XtMalloc(strlen(fn) + 10);

	FC_Menus(new_w) = (Widget *)XtCalloc(14, sizeof(Widget));
	FC_Setting(new_w) = (String *)XtCalloc(14, sizeof(String));

	for (i=0; i<14; i++)
		FC_Setting(new_w)[i] = NULL;

	for (i=0; i<14; i++) {	/* XLFD has this many fields */
		q = strchr(p, ',');
		if (q)
			*q = '\0';

		strcpy(n, p);
		strcat(n, "_pulldown");

		ac = 0;
		FC_Menus(new_w)[i] = menu = XmCreatePulldownMenu(form, n, al, ac);
		XtAddCallback(menu, XmNmapCallback, FitMenuOnScreen, NULL);

		strcpy(n, p);
		xms = XmStringCreateSimple(n);
		strcat(n, "_option");

		ac = 0;
		XtSetArg(al[ac], XmNsubMenuId, menu); ac++;
		if (i == 0 || i == 4 || i == 10) {
			XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
		} else {
			XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
			XtSetArg(al[ac], XmNleftWidget, option); ac++;
		}
		if (i >= 4) {
			XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
			XtSetArg(al[ac], XmNtopWidget, top); ac++;
		}
		XtSetArg(al[ac], XmNleftOffset, 10); ac++;
		XtSetArg(al[ac], XmNlabelString, xms); ac++;
		option = XmCreateOptionMenu(form, n, al, ac);
		XtManageChild(option);
		XmStringFree(xms);

		if (i == 3 || i == 9)
			top = option;

		if (q)
			p = q+1;
	}
	free(fn);
	XtFree(n);

	ac = 0;
	xms = XmStringCreateSimple(FC_CountString(new_w));
	XtSetArg(al[ac], XmNlabelString, xms); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 10); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftOffset, 10); ac++;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, option); ac++;
	XtSetArg(al[ac], XmNtopOffset, 10); ac++;
	FC_CountWidget(new_w) = count_label = XmCreateLabelGadget(form, "count_label", al, ac);
	XmStringFree(xms);

	ac = 0;
	xms = XmStringCreateSimple(FC_DemoString(new_w));
	XtSetArg(al[ac], XmNlabelString, xms); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 10); ac++;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, option); ac++;
	XtSetArg(al[ac], XmNtopOffset, 10); ac++;
	FC_DemoWidget(new_w) = demo = XmCreateLabelGadget(form, "demo_label", al, ac);
	XmStringFree(xms);

	if (FC_ShowDemo(new_w)) {
		XtManageChild(count_label);
		XtManageChild(demo);
	}

	GetFullFontList(new_w);
	XtManageChild(form);
}

static Boolean
set_values(Widget old, Widget request, Widget new_w,
	   ArgList args, Cardinal *num_args)
{
    Boolean refresh = False;

    return refresh;
}

/************************ EXTERNAL INTERFACE ****************************/

Widget
XltCreateFontChooser(Widget parent, char *name, Arg *arglist, Cardinal argCount)
{
    return XtCreateWidget(name, xltFontChooserWidgetClass, parent, arglist, argCount);
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
XltCreateFontChooserDialog(Widget parent, char *name, Arg *arglist, Cardinal argcount)
{
    Widget shell, r;
    char *shell_name;
    int ac;
    XtArgVal i;
    Arg *al = (Arg *)XtCalloc(argcount + 1, sizeof(Arg));

    ac = 0;
    XtSetArg(al[ac], XmNallowShellResize, True);
    ac++;
    for (i = 0; i < argcount; i++)
    {
	XtSetArg(al[ac], arglist[i].name, arglist[i].value);
	ac++;
    }

    shell_name = MakeDialogName(name);
    shell = XmCreateDialogShell(parent, shell_name, al, ac);
    XtFree(shell_name);

    r = XltCreateFontChooser(shell, name, al, ac);
    XtFree((XtPointer)al);

    return r;
}
