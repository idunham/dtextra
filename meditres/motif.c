/*
 * $Header: /cvsroot-fuse/meditres/meditres/motif.c,v 1.26 2002/01/15 19:23:23 dannybackx Exp $
 *
 * Based on $XConsortium: widgets.c,v 1.22 94/04/17 20:38:58 dave Exp $
 *
 * Copyright (c) 1989  X Consortium
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the X Consortium shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from the X Consortium.
 */

/*
 * Modifications by Volker Gering, 1997
 *
 * Port to Motif, convert into MEDITRES, © 2001, 2002 by Danny Backx.
 */

/*
 * Code for creating all widgets used by EditRes.
 */

#include "config.h"
#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>	/* Get standard string definations. */

#include <X11/cursorfont.h>     /* Standard cursor font */

#include <Xm/Xm.h>
#include <Xm/CascadeBG.h>
#include <Xm/Container.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/MessageB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>

#include "meditres.h"

/*
 * functions.
 */

static void CreateResourceNameForm(Widget parent, WNode *node);
static void SetToggleGroupLeaders(WNode *node);
static void CreateLists (Widget parent, WNode *node, char **names, char **cons_names);
static void CreateSelectMenu(Widget parent, String toolkit);
static void CreateViewMenu (Widget parent, String toolkit);
static void CreateOptionsMenu (Widget parent, String toolkit);
static void CreateFoldMenu (Widget parent, String toolkit);
static void FreeClientData (Widget w, XtPointer ptr, XtPointer junk);
static void FreeResBox (Widget w, XtPointer ptr, XtPointer junk);
static void CreateValueWidget (Widget parent, WNode  *node);
static void PopupOnNode (WNode *node, Widget shell);
static Widget CreateTopArea (Widget parent);
static void MakeBoxLookNice(Widget dot, Widget star, Widget any, Widget single,
           Widget name, Widget class, int endbox);


static void	PopupTreeMenu(Widget, XtPointer, XtPointer);
static void	TreeHighlight(Widget w, XtPointer client, XtPointer call);

/*
 * global Variables
 */
static int   menuCount = 0;

#define      NUM_ENTRIES 51      /* number of menu widgets */

/* Function Name:
 *   RebuildMenusAndLabel
 *
 * Description:
 *   Determins if the user has selected an application
 *   which uses a different toolkit.  Xt is the default.
 *   If this is so, destroys and recreates the menus and
 *   information (label at the top/list at the bottom)
 *   of the application.
 *
 * Arguments:
 *   toolkit - name of the toolkit: "xt" (X Toolkit)  or "iv" (InterViews)
 *
 * Returns:
 *   none.
 *
 * Calls:
 *    CreateTreeMenu - (self)
 *    CreateResourceMenu - (self)
 *    CreateFindMenu - (self)
 *    CreateSelectMenu - (self)
 *    CreateViewMenu - (self)
 *    CreateOptionsMenu - (self)
 *    CreateFoldMenu - (self)
 */

static Widget box = NULL;
Widget MessageArea = NULL;

#define Offset(index) sizeof(String) * index

#define res_entry(index, name, class) \
  {name, class, XtRString, sizeof(String), \
     Offset(index), XtRString, (XtPointer)NULL}

static XtResource resources[] =
{
  res_entry (0, "label0", "Label0"),
  res_entry (1, "label1", "Label1"),
  res_entry (2, "label2", "Label2"),
  res_entry (3, "label3", "Label3"),
  res_entry (4, "label4", "Label4"),
  res_entry (5, "label5", "Label5"),
  res_entry (6, "label6", "Label6"),
  res_entry (7, "label7", "Label7"),
  res_entry (8, "label8", "Label8"),
  res_entry (9, "label9", "Label9"),
  res_entry (11, "label11", "Label11"),
  res_entry (12, "label12", "Label12"),
  res_entry (13, "label13", "Label13"),
  res_entry (14, "label14", "Label14"),
  res_entry (15, "label15", "Label15"),
  res_entry (16, "label16", "Label16"),
  res_entry (17, "label17", "Label17"),
  res_entry (18, "label18", "Label18"),
  res_entry (19, "label19", "Label19"),
  res_entry (20, "label20", "Label20"),
  res_entry (21, "label21", "Label21"),
  res_entry (22, "label22", "Label22"),
  res_entry (23, "label23", "Label23"),
  res_entry (24, "label24", "Label24"),
  res_entry (25, "label25", "Label25"),
  res_entry (26, "label26", "Label26"),
  res_entry (27, "label27", "Label27"),
  res_entry (28, "label28", "Label28"),
  res_entry (29, "label29", "Label29"),
  res_entry (30, "label30", "Label30"),
  res_entry (31, "label31", "Label31"),
  res_entry (32, "label32", "Label32"),
  res_entry (33, "label33", "Label33"),
  res_entry (34, "label34", "Label34"),
  res_entry (35, "label35", "Label35"),
  res_entry (36, "label36", "Label36"),
  res_entry (37, "label37", "Label37"),
  res_entry (38, "label38", "Label38"),
  res_entry (39, "label39", "Label39"),
  res_entry (40, "label40", "Label40"),
  res_entry (41, "label41", "Label41"),
  res_entry (42, "label42", "Label42"),
  res_entry (43, "label43", "Label43"),
  res_entry (44, "label44", "Label44"),
  res_entry (45, "label45", "Label45"),
  res_entry (46, "label46", "Label46"),
  res_entry (47, "label47", "Label47")
};

#undef res_entry

#undef Offset

void
RebuildMenusAndLabel (String toolkit)
{
}

static void VersionDialog(Widget w, XtPointer client, XtPointer call)
{
	static Widget	d = NULL;

	if (d == NULL) {
		XmString	xms;

		d = XmCreateMessageDialog(w, "versionDialog", NULL, 0);
		xms = XmStringCreateSimple(PACKAGE " Version " VERSION
			"\n"
			"Written by Danny Backx (danny.backx@skynet.be)");
		XtVaSetValues(d, XmNmessageString, xms, NULL);
		XmStringFree(xms);		
	}
	XtManageChild(d);
}

/* Function Name:
 *   BuildWidgetTree
 *
 * Description:
 *   Creates all widgets for Editres.
 *
 * Arguments:
 *   parent - the shell to put them into.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   CreateTopArea - (self)
 */
void
BuildWidgetTree(Widget parent)
{
  Arg       al[10];
  int       ac;
  Widget    mw, sw;
  Widget    mb, fm, fcb, w, hm, hcb;
  Widget    treeform;

  /* the whole client area at normal startup. It keeps menu bar, status area, */
  /* the tree and the messages */
  ac = 0;
  XtSetArg(al[ac], XmNresizePolicy, XmRESIZE_NONE); ac++;
  mw = XmCreateForm(parent, "mw", al, ac);
  XtManageChild(mw);

  ac = 0;
  XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  mb = XmCreateMenuBar(mw, "menubar", al, ac);
  XtManageChild(mb);

  /* File Menu */
  ac = 0;
  fm = XmCreatePulldownMenu(mb, "fileMenu", al, ac);

  ac = 0;
  XtSetArg(al[ac], XmNsubMenuId, fm); ac++;
  fcb = XmCreateCascadeButtonGadget(mb, "fileCascade", al, ac);
  XtManageChild(fcb);

  ac = 0;
  w = XmCreatePushButtonGadget(fm, "getTree", al, ac);
  XtManageChild(w);
  XtAddCallback(w, XmNactivateCallback, SendTree, (XtPointer)True);

  ac = 0;
  w = XmCreatePushButtonGadget(fm, "refreshTree", al, ac);
  XtManageChild(w);
  XtAddCallback(w, XmNactivateCallback, SendTree, (XtPointer)False);

  ac = 0;
  w = XmCreatePushButtonGadget(fm, "dumpTreeToFile", al, ac);
  XtManageChild(w);
  XtAddCallback(w, XmNactivateCallback, DumpTreeToFile, NULL);

  ac = 0;
  w = XmCreatePushButtonGadget(fm, "printTree", al, ac);
  XtManageChild(w);
  XtAddCallback(w, XmNactivateCallback, printTree, NULL);

  ac = 0;
  w = XmCreatePushButtonGadget(fm, "quitButton", al, ac);
  XtManageChild(w);
  XtAddCallback(w, XmNactivateCallback, Quit, NULL);

  /* Resource menu */
  ac = 0;
  fm = XmCreatePulldownMenu(mb, "resourceMenu", al, ac);
  ac = 0;
  XtSetArg(al[ac], XmNsubMenuId, fm); ac++;
  fcb = XmCreateCascadeButtonGadget(mb, "resourceCascade", al, ac);
  XtManageChild(fcb);

  ac = 0;
  XtSetArg(al[ac], XmNsensitive, False); ac++;	/* FIX ME */
  w = XmCreatePushButtonGadget(fm, "setResourceButton", al, ac);
  XtManageChild(w);
  XtAddCallback(w, XmNactivateCallback, InitSetValues, NULL);

  ac = 0;
  w = XmCreatePushButtonGadget(fm, "resourceBoxButton", al, ac);
  XtManageChild(w);
  XtAddCallback(w, XmNactivateCallback, GetResourceList, NULL);

  ac = 0;
  XtSetArg(al[ac], XmNsensitive, False); ac++;	/* FIX ME */
  w = XmCreatePushButtonGadget(fm, "resourceFileButton", al, ac);
  XtManageChild(w);
/*  XtAddCallback(w, XmNactivateCallback, resourceFile, NULL); */

  /* Find Menu */
  ac = 0;
  fm = XmCreatePulldownMenu(mb, "findMenu", al, ac);
  ac = 0;
  XtSetArg(al[ac], XmNsubMenuId, fm); ac++;
  fcb = XmCreateCascadeButtonGadget(mb, "findCascade", al, ac);
  XtManageChild(fcb);

  ac = 0;
  XtSetArg(al[ac], XmNsensitive, False); ac++;	/* FIX ME */
  w = XmCreatePushButtonGadget(fm, "flashActiveButton", al, ac);
  XtManageChild(w);
  XtAddCallback(w, XmNactivateCallback, FlashActiveWidgets, NULL);

  ac = 0;
  XtSetArg(al[ac], XmNsensitive, False); ac++;	/* FIX ME */
  w = XmCreatePushButtonGadget(fm, "selectWidgetButton", al, ac);
  XtManageChild(w);
  XtAddCallback(w, XmNactivateCallback, FindWidget, NULL);

  ac = 0;
  XtSetArg(al[ac], XmNsensitive, False); ac++;	/* FIX ME */
  w = XmCreatePushButtonGadget(fm, "findInTreeButton", al, ac);
  XtManageChild(w);
  XtAddCallback(w, XmNactivateCallback, InitFindDlg, NULL);

  /* The active area */
  ac = 0;
  XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
//  XtSetArg(al[ac], XmNscrollBarDisplayPolicy, XmSTATIC); ac++;
  XtSetArg(al[ac], XmNscrollingPolicy, XmAUTOMATIC); ac++;
  XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(al[ac], XmNtopWidget, mb); ac++;
  XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_SELF); ac++;
  XtSetArg(al[ac], XmNheight, 400); ac++;
  XtSetArg(al[ac], XmNwidth, 400); ac++;
  sw = XmCreateScrolledWindow(mw, "sw", al, ac);
  XtManageChild(sw);
  ac = 0;
  XtSetArg(al[ac], XmNlayoutType, XmOUTLINE); ac++;
  global_tree_parent =
  treeform = XmCreateContainer(sw, "treeform", al, ac);
  XtManageChild(treeform);
  XtAddCallback(treeform, XmNselectionCallback, TreeHighlight, NULL);

  /* Help Menu */
  ac = 0;
  hm = XmCreatePulldownMenu(mb, "helpMenu", al, ac);
  ac = 0;
  XtSetArg(al[ac], XmNsubMenuId, hm); ac++;
  hcb = XmCreateCascadeButtonGadget(mb, "helpCascade", al, ac);
  XtManageChild(hcb);
  XtVaSetValues(mb, XmNmenuHelpWidget, hcb, NULL);

  ac = 0;
  w = XmCreatePushButtonGadget(hm, "version", al, ac);
  XtManageChild(w);
  XtAddCallback(w, XmNactivateCallback, VersionDialog, NULL);

  /* This is a message area */
  ac = 0;
  XtSetArg(al[ac], XmNcolumns, 80); ac++;
  XtSetArg(al[ac], XmNeditable, False); ac++;
  XtSetArg(al[ac], XmNeditMode, XmMULTI_LINE_EDIT); ac++;
  XtSetArg(al[ac], XmNtopWidget, sw); ac++;
  XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  global_screen_data.info_label =
  MessageArea = XmCreateScrolledText(mw, "hPane",  al, ac);
  XtManageChild(MessageArea);

#if 0
  /* get the "xt label" application resources for info_label */
  XtGetApplicationResources (global_screen_data.info_label,
			     res_labels,
			     resources, XtNumber (resources),
			     NULL, 0);
#endif

}   /* BuildWidgetTree() */

void WidgetTreeRemoveChildren(Widget cont, Widget ch)
{
	int	i, n;
	WidgetList	l = NULL;

	n = XmContainerGetItemChildren(cont, ch, &l);
	if (n == 0)
		return;
	for (i=0; i<n; i++) {
		WidgetTreeRemoveChildren(cont, l[i]);
		XtDestroyWidget(l[i]);
	}
}

/* Function Name:
 *   CreateTopArea
 *
 * Description:
 *   Creates the top part of the display (menu bar and status area)
 *
 * Arguments:
 *   parent - widget to put this menu bar into (panned widget).
 *
 * Returns:
 *   none. 
 *
 * Calls:
 *    CreateTreeMenu - (self)
 *    CreateResourceMenu - (self)
 *    CreateFindMenu - (self)
 *    CreateSelectMenu - (self)
 *    CreateViewMenu - (self)
 *    CreateOptionsMenu - (self)
 *    CreateFoldMenu - (self)
 *    SetNumWidLabel - (self)
 *    SetSelWidLabel - (self)
 *    SetWaitLabel - (self)
 *    SetZoomLabel - (self)
 *    SetWorkLabel - (self)
 *    SetRcvLabel - (self)
 */
static Widget
CreateTopArea (Widget parent)
{
#if 0
//  CreateTreeMenu (box, "xt");
//  CreateResourceMenu (box, "xt");
//  CreateOptionsMenu (box, "xt");
//  CreateFindMenu (box, "xt");
//  CreateSelectMenu (box, "xt");
//  CreateViewMenu (box, "xt");
//  CreateFoldMenu (box, "xt");
#endif
	return NULL;
}   /* CreateTopArea() */

/* Function Name:
 *   SetNumWidLabel
 *
 * Description:
 *   Sets the new value for "number of widgets in client" in the status area
 *
 * Arguments:
 *   num - number of widgets.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 */
void
SetNumWidLabel(int num)
{
#if 0
	Arg  args[2];
	int  num_args = 0;
	char buf[BUFSIZ];

	sprintf(buf, "%d", num);
	num_args = 0;
	XtSetArg(args[num_args], XtNlabel, buf);
	num_args++;
	if (global_numWid)
		XtSetValues(global_numWid, args, num_args);
#endif

	fprintf(stderr, "SetNumWidLabel(%d)\n", num);
}   /* SetNumWidLabel() */

/* Function Name:
 *   SetShownWidLabel
 *
 * Description:
 *   Sets the new value for "number of toggles shown" in the status area
 *
 * Arguments:
 *   num - number of widgets.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 */
void
SetShownWidLabel(int num)
{
  Arg  args[2];
  int  num_args = 0;
  char buf[BUFSIZ];

  sprintf(buf, "%d", num);
  num_args = 0;
  XtSetArg(args[num_args], XtNlabel, buf);
  num_args++;
  if (global_shownWid)
    XtSetValues(global_shownWid, args, num_args);
}   /* SetShownWidLabel() */


/* Function Name:
 *   SetSelWidLabel
 *
 * Description:
 *   Sets the new value for "number of selected/active widgets" in the status area
 *
 * Arguments:
 *   num - number of widgets.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 */
void
SetSelWidLabel(int num)
{
#if 0
	Arg  args[2];
	int  num_args = 0;
	char buf[BUFSIZ];

	sprintf(buf, "%d", num);
	num_args = 0;
	XtSetArg(args[num_args], XtNlabel, buf);
	num_args++;
	if (global_selWid)
		XtSetValues(global_selWid, args, num_args);
#endif

	fprintf(stderr, "SetSelWidLabel(%d)\n", num);
}   /* SetSelWidLabel() */

/* Function Name:
 *   SetWaitLabel
 *
 * Description:
 *   Sets the new value for "countdown (waiting for answer)" in the status area
 *
 * Arguments:
 *   num - actual countdown value.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 */
void
SetWaitLabel (num)
     int num;
{
 
}

/* Function Name:
 *   SetZoomLabel
 *
 * Description:
 *   Sets the new value for "actual zoom" in the status area
 *
 * Arguments:
 *   num - zoom value.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 */
void
SetZoomLabel(int num)
{
	Arg  args[2];
	int  num_args = 0;
	char buf[BUFSIZ];

	sprintf(buf, "%d", num);
	num_args = 0;
	XtSetArg(args[num_args], XtNlabel, buf);
	num_args++;
	XtSetValues(global_zoom, args, num_args);
}   /* SetZoomLabel() */


/* Function Name:
 *   SetWorkLabel
 *
 * Description:
 *   Sets the new value for "working in progress" in the status area.
 *   It is an bar showing the percentage of work done.
 *
 * Arguments:
 *   num - percentage of work done.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 */
void
SetWorkLabel(int num)
{
	Arg  args[2];
	int  num_args = 0;
	char buf[BUFSIZ];

	sprintf(buf, "%d %%", num);
	num_args = 0;
	XtSetArg(args[num_args], XtNlabel, buf);
	num_args++;
	XtSetValues(global_work, args, num_args);
}   /* SetWorkLabel() */


/* Function Name:
 *   SetRcvLabel
 *
 * Description:
 *   Sets the new value for "receiving data" in the status area
 *
 * Arguments:
 *   active - receiving data.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 */
void
SetRcvLabel (active)
     Boolean active;
{
  Arg  args[2];
  int  num_args = 0;


  num_args = 0;
  if (active)
    {
      XtSetArg(args[num_args],
	       XtNlabel, "X");
    }
  else
    {
      XtSetArg(args[num_args],
	       XtNlabel, " ");
    }
  num_args++;
  XtSetValues(global_rcv,
	      args, num_args);

}   /* SetRcvLabel() */


static Widget menuEntries[NUM_ENTRIES];


/* Function Name:
 *   CreateSelectMenu
 *
 * Description:
 *   Creates the select menu.
 *
 * Arguments:
 *   parent - widget to put this menu into.
 *   toolkit - name given to the SimpleMenu widget ("xt" or "iv").
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 *
 * Callbacks installed:
 *   TreeSelect - handler.c
 */
static void
CreateSelectMenu(Widget parent, String toolkit)
{
#if 0
  Arg           args[1];
  static Widget semenu = NULL;
  static Widget sebutton = NULL;


  if (semenu)
    {
      XtDestroyWidget (semenu);
    }   /* if (pulldown menu already exists) */
  else
    {
      sebutton = XtCreateManagedWidget ("selectMenu",
					menuButtonWidgetClass,
					parent,
					NULL, ZERO);
    }   /* else (first call: create menu button) */

  /* set the menu name to the toolkit name */
  XtSetArg (args[0],
	    XtNmenuName, toolkit);
  XtSetValues (sebutton,
	       args, ONE);

  semenu = XtCreatePopupShell (toolkit,
			       simpleMenuWidgetClass,
			       sebutton,
			       NULL, ZERO);

  menuEntries[menuCount] = XtCreateManagedWidget ("unselectAll",
						  smeBSBObjectClass,
						  semenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeSelect, (XtPointer)SelectNone);

  menuEntries[menuCount] = XtCreateManagedWidget ("invertAll",
						  smeBSBObjectClass,
						  semenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeSelect, (XtPointer)SelectInvert);

  menuEntries[menuCount] = XtCreateManagedWidget ("selectAll",
						  smeBSBObjectClass,
						  semenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeSelect, (XtPointer)SelectAll);

  menuEntries[menuCount++] = XtCreateManagedWidget ("line",
						    smeLineObjectClass,
						    semenu,
						    NULL, ZERO);

  menuEntries[menuCount] = XtCreateManagedWidget ("selectChildren",
						  smeBSBObjectClass,
						  semenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeSelect, (XtPointer)SelectChildren);

  menuEntries[menuCount] = XtCreateManagedWidget ("selectParent",
						  smeBSBObjectClass,
						  semenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeSelect, (XtPointer)SelectParent);

  menuEntries[menuCount] = XtCreateManagedWidget ("selectDescendants",
						  smeBSBObjectClass,
						  semenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeSelect, (XtPointer)SelectDescendants);

  menuEntries[menuCount] = XtCreateManagedWidget ("selectAncestors",
						  smeBSBObjectClass,
						  semenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeSelect, (XtPointer)SelectAncestors);
#endif
}   /* CreateSelectMenu() */

/* Function Name:
 *   CreateViewMenu
 *
 * Description:
 *   Creates the view menu.
 *
 * Arguments:
 *   parent - widget to put this menu into.
 *   toolkit - name given to the SimpleMenu widget ("xt" or "iv").
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 *
 * Callbacks installed:
 *   TreeRelabel - handler.c
 */
static void
CreateViewMenu (Widget parent, String toolkit)
{
#if 0
  Arg           args[1];
  static Widget shmenu = NULL;
  static Widget shbutton = NULL;


  if (shmenu)
    {
      XtDestroyWidget (shmenu);
    }   /* if (pulldown menu already exists) */
  else
    {
      shbutton = XtCreateManagedWidget ("viewMenu",
				       menuButtonWidgetClass,
				       parent,
				       NULL, ZERO);
    }   /* else (first call: create menu button) */

  /* set the menu name to the toolkit name */
  XtSetArg (args[0],
	    XtNmenuName, toolkit);
  XtSetValues (shbutton,
	       args, ONE);

  shmenu = XtCreatePopupShell (toolkit,
			      simpleMenuWidgetClass,
			      shbutton,
			      NULL, ZERO);

  menuEntries[menuCount] = XtCreateManagedWidget ("viewWidgetNames",
						  smeBSBObjectClass,
						  shmenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeRelabel, (XtPointer)NameLabel);

  menuEntries[menuCount] = XtCreateManagedWidget ("viewClassNames",
						  smeBSBObjectClass,
						  shmenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeRelabel, (XtPointer)ClassLabel);

  menuEntries[menuCount] = XtCreateManagedWidget ("viewWidgetIDs",
						  smeBSBObjectClass,
						  shmenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeRelabel, (XtPointer)IDLabel);

  menuEntries[menuCount] = XtCreateManagedWidget ("viewWidgetWindows",
						  smeBSBObjectClass,
						  shmenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeRelabel, (XtPointer)WindowLabel);

#ifdef DEBUG
  menuEntries[menuCount] = XtCreateManagedWidget ("viewView",
						  smeBSBObjectClass,
						  shmenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
		 XtNcallback, TreeRelabel, (XtPointer)ViewLabel);
#endif   /* DEBUG */
#endif
}   /* CreateViewMenu() */


/* Function Name:
 *   CreateOptionsMenu
 *
 * Description:
 *   Creates the options menu.
 *
 * Arguments:
 *   parent - widget to put this menu into.
 *   toolkit - name given to the SimpleMenu widget ("xt" or "iv").
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 *
 * Callbacks installed:
 */
static void
CreateOptionsMenu (Widget parent, String toolkit)
{
#if 0
  Arg           args[1];
  static Widget omenu = NULL;
  static Widget obutton = NULL;


  if (omenu)
    {
      XtDestroyWidget (omenu);
    }   /* if (pulldown menu already exists) */
  else
    {
      obutton = XtCreateManagedWidget ("optionsMenu",
				       menuButtonWidgetClass,
				       parent,
				       NULL, ZERO);
    }   /* else (first call: create menu button) */

  /* set the menu name to the toolkit name */
  XtSetArg (args[0],
	    XtNmenuName, toolkit);
  XtSetValues (obutton,
	       args, ONE);

  omenu = XtCreatePopupShell (toolkit,
			      simpleMenuWidgetClass,
			      obutton,
			      NULL, ZERO);

  menuEntries[menuCount] = XtCreateManagedWidget ("zoom",
						  smeBSBObjectClass,
						  omenu,
						  NULL, ZERO);
/*   XtAddCallback (menuEntries[menuCount++], */
/* 		 XtNcallback, zoom, (XtPointer)NULL); */

  menuEntries[menuCount] = XtCreateManagedWidget ("orientation",
						  smeBSBObjectClass,
						  omenu,
						  NULL, ZERO);
/*   XtAddCallback (menuEntries[menuCount++], */
/* 		 XtNcallback, orientation, (XtPointer)NULL); */

  menuEntries[menuCount] = XtCreateManagedWidget ("scrolling",
						  smeBSBObjectClass,
						  omenu,
						  NULL, ZERO);
/*   XtAddCallback (menuEntries[menuCount++], */
/* 		 XtNcallback, scrolling, (XtPointer)NULL); */

  menuEntries[menuCount] = XtCreateManagedWidget ("prefs",
						  smeBSBObjectClass,
						  omenu,
						  NULL, ZERO);
/*   XtAddCallback (menuEntries[menuCount++], */
/* 		 XtNcallback, prefs, (XtPointer)NULL); */

  menuEntries[menuCount] = XtCreateManagedWidget ("sysinfo",
						  smeBSBObjectClass,
						  omenu,
						  NULL, ZERO);
/*   XtAddCallback (menuEntries[menuCount++], */
/* 		 XtNcallback, sysinfo, (XtPointer)NULL); */
#endif
}   /* CreateOptionsMenu() */

/* Function Name:
 *   CreateFoldMenu
 *
 * Description:
 *   Creates the folding menu.
 *
 * Arguments:
 *   parent - widget to put this menu into.
 *   toolkit - name given to the SimpleMenu widget ("xt" or "iv").
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 *
 * Callbacks installed:
 *   treeFold - handler.c
 *   newTop - handler.c
 *   showAll - handler.c
 */
static void
CreateFoldMenu (Widget parent, String toolkit)
{
#if 0
  Arg           args[1];
  static Widget fomenu = NULL;
  static Widget fobutton = NULL;


  if (fomenu)
    {
      XtDestroyWidget (fomenu);
    }   /* if (pulldown menu already exists) */
  else
    {
      fobutton = XtCreateManagedWidget ("foldMenu",
					menuButtonWidgetClass,
					parent,
					NULL, ZERO);
    }   /* else (first call: create menu button) */

  /* set the menu name to the toolkit name */
  XtSetArg (args[0],
	    XtNmenuName, toolkit);
  XtSetValues (fobutton,
	       args, ONE);

  fomenu = XtCreatePopupShell (toolkit,
			       simpleMenuWidgetClass,
			       fobutton,
			       NULL, ZERO);

  menuEntries[menuCount] = XtCreateManagedWidget ("foldSelected",
						  smeBSBObjectClass,
						  fomenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
 		 XtNcallback, treeFold, (XtPointer)FoldSelected);

  menuEntries[menuCount] = XtCreateManagedWidget ("foldChildSelected",
						  smeBSBObjectClass,
						  fomenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
 		 XtNcallback, treeFold, (XtPointer)FoldChildSelected);

  menuEntries[menuCount++] = XtCreateManagedWidget ("line",
						    smeLineObjectClass,
						    fomenu,
						    NULL, ZERO);

  menuEntries[menuCount] = XtCreateManagedWidget ("unfold1Selected",
						  smeBSBObjectClass,
						  fomenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
 		 XtNcallback, treeFold, (XtPointer)Unfold1Selected);

  menuEntries[menuCount] = XtCreateManagedWidget ("unfoldSelected",
						  smeBSBObjectClass,
						  fomenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
 		 XtNcallback, treeFold, (XtPointer)UnfoldSelected);

  menuEntries[menuCount] = XtCreateManagedWidget ("unfoldAll",
						  smeBSBObjectClass,
						  fomenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
 		 XtNcallback, treeFold, (XtPointer)UnfoldAll);

  menuEntries[menuCount++] = XtCreateManagedWidget ("line",
						    smeLineObjectClass,
						    fomenu,
						    NULL, ZERO);

  menuEntries[menuCount] = XtCreateManagedWidget ("newTopSelected",
						  smeBSBObjectClass,
						  fomenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
 		 XtNcallback, newTop, (XtPointer)NewSelected);

  menuEntries[menuCount] = XtCreateManagedWidget ("newTopParent",
						  smeBSBObjectClass,
						  fomenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
 		 XtNcallback, newTop, (XtPointer)NewParent);

  menuEntries[menuCount] = XtCreateManagedWidget ("restoreRoot",
						  smeBSBObjectClass,
						  fomenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
 		 XtNcallback, newTop, (XtPointer)NewRoot);

  menuEntries[menuCount++] = XtCreateManagedWidget ("line",
						    smeLineObjectClass,
						    fomenu,
						    NULL, ZERO);

  menuEntries[menuCount] = XtCreateManagedWidget ("showAll",
						  smeBSBObjectClass,
						  fomenu,
						  NULL, ZERO);
  XtAddCallback (menuEntries[menuCount++],
 		 XtNcallback, showAll, (XtPointer)NULL);
#endif
}   /* CreateFoldMenu() */


static Pixmap old_pixmap;

/* Function Name:
 *   PrepareToLayoutTree
 *
 * Description:
 *   prepares the Tree widget to be layed out.
 *
 * Arguments:
 *   tree - the Tree widget.
 *
 * Returns:
 *   none
 *
 * Calls:
 *  Xt-stuff
 */
void
PrepareToLayoutTree (Widget tree)
{
#if 0
  Arg args[1];


  XtSetArg (args[0], XtNbackgroundPixmap, &old_pixmap);
  XtGetValues (XtParent (tree), args, ONE);

  XtSetArg (args[0], XtNbackgroundPixmap, None);
  XtSetValues (XtParent (tree), args, ONE);

  XtUnmapWidget (tree);
#endif
}   /* PrepareToLayoutTree() */

/* Function Name:
 *   LayoutTree
 *
 * Description:
 *   Laysout the tree widget.
 *
 * Arguments:
 *   tree - the widget tree.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xaw- and Xt-stuff
 */
void
LayoutTree (Widget tree)
{
#if 0
  Arg args[1];


//  XawTreeForceLayout (tree);
  XtMapWidget (tree);

  XtSetArg (args[0], XtNbackgroundPixmap, old_pixmap);
  XtSetValues (XtParent (tree), args, ONE);
#endif
}   /* LayoutTree() */

static void CancelDialog(Widget w, XtPointer client, XtPointer call)
{
	Widget	d = (Widget)client;
	
	XtUnmanageChild(d);
}

/************************************************************

 * Functions for creating the Resource Box.
 *
 ************************************************************/

/* Function Name:
 *   CreateResourceBoxWidgets
 *
 * Description:
 *   Creates the widgets that make up the resource box.
 *
 * Arguments:
 *   node - the widget node.
 *   names - the list of names that make up the normal resources.
 *   cons_names - the list of names that make up the constraint resources. 
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   CreateResourceNameForm - (self)
 *   CreateLists - (self)
 *   CreateValueWidget - (self)
 *   SetToggleGroupLeaders - (self)
 *   PopupOnNode - (self)
 *
 * Callbacks installed:
 *   SetFile - handler.c
 *   SaveResource - handler.c
 *   ApplyResource - handler.c
 *   PopdownResBox - handler.c
 */
void
CreateResourceBoxWidgets(WNode *node, char **names, char **cons_names)
{
	Widget		item;
	Widget		sw, ok, apply, get, cancel, tw, listw, form;
	ResourceBoxInfo *res_box;
	XmString	*list, title, xms1, xms2;
	int		i, j, n, cn, ac;
	Arg		al[10];

	/* if this node is not shown, we can return */
	if (node->widget == NULL) {
		fprintf(stderr, "CreateResourceBoxWidgets(NULL)\n");
		return;
	}

	item = node->widget;
	xms2 = XmStringCreateSimple(XtName(item));
	res_box = (ResourceBoxInfo*)XtMalloc(sizeof (ResourceBoxInfo));
	node->resources->res_box = res_box;

	xms1 = XmStringCreateSimple("Resource box for "/* FIX ME */);
	title = XmStringConcat(xms1, xms2);
	XmStringFree(xms1);
	XmStringFree(xms2);

	ac = 0;
	XtSetArg(al[ac], XmNautoUnmanage, False); ac++;
	XtSetArg(al[ac], XmNdialogTitle, title); ac++;
	sw = XmCreateTemplateDialog(global_tree_parent, "resource box", al, ac);
	XtManageChild(sw);
	res_box->shell = XtParent(sw);
	ok = XmCreatePushButtonGadget(sw, "ok", NULL, 0);
	XtManageChild(ok);
	apply = XmCreatePushButtonGadget(sw, "apply", NULL, 0);
	XtManageChild(apply);
	get = XmCreatePushButtonGadget(sw, "get", NULL, 0);
	XtManageChild(get);
	cancel = XmCreatePushButtonGadget(sw, "cancel", NULL, 0);
	XtManageChild(cancel);
	XmStringFree(title);

	ac = 0;
	form = XmCreateForm(sw, "form", al, ac);
	XtManageChild(form);

	ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNvisibleItemCount, 10); ac++;
	listw = XmCreateScrolledList(form, "resourcelist", al, ac);
	XtManageChild(listw);

	XtAddCallback(get, XmNactivateCallback, GetResource, (XtPointer)node);
	XtAddCallback(ok, XmNactivateCallback, ApplyResource, (XtPointer)node);	/* FIX ME */
	XtAddCallback(apply, XmNactivateCallback, ApplyResource, (XtPointer)node);
	XtAddCallback(cancel, XmNactivateCallback, CancelDialog, (XtPointer)sw);

	ac = 0;
	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, XtParent(listw)); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	tw = XmCreateTextField(form, "value", al, ac);
	XtManageChild(tw);
	global_tree_info->active_nodes[0]->resources->res_box->value_wid = tw;

	n = cn = 0;
	if (names) {
		for (n = 0; names[n]; n++) ;
	}
	if (cons_names) {
		for (cn = 0; cons_names[cn]; cn++) ;
	}

	list = (XmString *)XtCalloc(n + cn + 1, sizeof(XmString));
	for (i=0; names[i]; i++)
		list[i] = XmStringCreateSimple(names[i]);
	if (cons_names)
		for (j=0, i=n; cons_names[j]; i++, j++)
			list[i] = XmStringCreateSimple(cons_names[j]);

	XtVaSetValues(listw,
		XmNitemCount,	n + cn,
		XmNitems,	list,
		NULL);

	XtAddCallback(res_box->shell, XtNdestroyCallback, FreeResBox, (XtPointer)node);
	XtAddCallback(sw, XmNcancelCallback, PopdownResBox, (XtPointer)res_box->shell);

	XtManageChild(sw);
}   /* CreateResourceBoxWidgets() */

/* Function Name:
 *   CreateResourceNameForm
 *
 * Description:
 *   Creates the Form widget with children that represent the
 *   full resource name for this object.
 *
 * Arguments:
 *   parent - parent of the form.
 *   node - the node corresponding to this object.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   GetNamesAndClasses - utils.c
 *   MakeBoxLookNice - (self)
 *
 * Callbacks installed:
 *   ActivateWidgetsAndSetResourceString - handler.c
 *   AnyChosen - handler.c
 *   FreeClientData - (self)
 */
static void
CreateResourceNameForm(Widget parent, WNode *node)
{
#if 0
  ResourceBoxInfo *res_box = node->resources->res_box;
  AnyInfo         *new_info;
  AnyInfo         *old_info;
  char            **names;
  char            **classes;
  Widget          form;
  NameInfo        *name_info = NULL;
  Cardinal        num_args;
  Arg             args[10];
  int             i;
  Widget          dot;
  Widget          star;
  Widget          name;
  Widget          class;
  Widget          single;
  Widget          any;


  GetNamesAndClasses (node,
		      &names,
		      &classes);

  form = XtCreateManagedWidget ("namesAndClasses",
				formWidgetClass,
				parent,
				NULL, ZERO);

  name = class = any = NULL;
  i = 0;
  old_info = NULL;

  while (TRUE)
    {
      num_args = 0;
      XtSetArg(args[num_args],
	       XtNtranslations, XtParseTranslationTable(
                  "<EnterWindow>:         highlight(Always) \n\
                   <LeaveWindow>:         unhighlight() \n\
                   <Btn1Down>, <Btn1Up>:  set() notify() "));
      num_args++;
      XtSetArg (args[num_args],
		XtNfromHoriz, name);
      num_args++;
      XtSetArg (args[num_args],
		XtNradioData, ".");
      num_args++;
      dot = XtCreateManagedWidget ("dot",
				   toggleWidgetClass,
				   form,
				   args, num_args);
      XtAddCallback (dot,
		     XtNcallback,
		     ActivateWidgetsAndSetResourceString,
		     (XtPointer)node);

      num_args = 1;   /* keep the XtNtranslations */
      XtSetArg (args[num_args],
		XtNfromHoriz, class);
      num_args++;
      XtSetArg (args[num_args],
		XtNfromVert, dot);
      num_args++;
      XtSetArg (args[num_args],
		XtNradioGroup, dot);
      num_args++;
      XtSetArg (args[num_args],
		XtNradioData, "*");
      num_args++;
      star = XtCreateManagedWidget ("star",
				    toggleWidgetClass,
				    form,
				    args, num_args);
      XtAddCallback (star,
		     XtNcallback,
		     ActivateWidgetsAndSetResourceString,
		     (XtPointer)node);

      if (name_info != NULL)
	{
	  name_info->next = (NameInfo*)XtMalloc (sizeof (NameInfo));
	  name_info = name_info->next;
	}
      else
	{
	  res_box->name_info =
	    name_info = (NameInfo*)XtMalloc (sizeof (NameInfo));
	}   /* else (name_info == NULL) */

      name_info->sep_leader = dot;
      name_info->name_leader = NULL;

      if (names[i] != NULL)
	{
	  new_info = (AnyInfo*)XtMalloc (sizeof (AnyInfo));
	  new_info->node = node;
	  new_info->left_dot = dot;
	  new_info->left_star = star;
	  new_info->left_count = 0;
	  if (old_info != NULL)
	    {
	      old_info->right_count = &(new_info->left_count);
	    }
	}   /* if (names[i] != NULL) */

      else if (old_info != NULL)
	{
	  old_info->right_count = NULL;
	}   /* elsif (old_info != NULL) */

      /* else */
      /* {} */

      if (old_info != NULL)
	{
	  old_info->right_dot = dot;
	  old_info->right_star = star;

	  /* if the "AnyWidgetChain"-Toggle changes, AnyChosen() is called */
	  /* to set the sensitivity of the neighbour-dot and -star */
	  XtAddCallback (any,
			 XtNcallback, AnyChosen, (XtPointer)old_info);
	  XtAddCallback (any,
			 XtNdestroyCallback, FreeClientData, (XtPointer)old_info);
	}   /* if (old_info != NULL) */

      if (names[i] == NULL)	/* no more name and class boxes. */
	{
	  break;
	}

      old_info = new_info;

      num_args = 1;   /* keep the XtNtranslations */
      XtSetArg (args[num_args],
		XtNfromHoriz, dot);
      num_args++;
      XtSetArg (args[num_args],
		XtNlabel, names[i]);
      num_args++;
      XtSetArg (args[num_args],
		XtNradioData, names[i]);
      num_args++;
      name = XtCreateManagedWidget ("name",
				    toggleWidgetClass,
				    form,
				    args, num_args);
      XtAddCallback (name,
		     XtNcallback,
		     ActivateWidgetsAndSetResourceString,
		     (XtPointer)node);

      num_args = 1;   /* keep the XtNtranslations */
      XtSetArg (args[num_args],
		XtNfromHoriz, star);
      num_args++;
      XtSetArg (args[num_args],
		XtNfromVert, name);
      num_args++;
      XtSetArg (args[num_args],
		XtNlabel, classes[i]);
      num_args++;
      XtSetArg (args[num_args],
		XtNradioGroup, name);
      num_args++;
      XtSetArg (args[num_args],
		XtNradioData, classes[i]);
      num_args++;
      class = XtCreateManagedWidget ("class",
				     toggleWidgetClass,
				     form,
				     args, num_args);
      XtAddCallback (class,
		     XtNcallback,
		     ActivateWidgetsAndSetResourceString,
		     (XtPointer)node);

      num_args = 1;   /* keep the XtNtranslations */
      XtSetArg (args[num_args],
		XtNfromHoriz, star);
      num_args++;
      XtSetArg (args[num_args],
		XtNfromVert, class);
      num_args++;
      XtSetArg (args[num_args],
		XtNradioData, "?");
      num_args++;
      XtSetArg (args[num_args],
		XtNradioGroup, name);
      num_args++;
      single = XtCreateManagedWidget ("single",
				      toggleWidgetClass,
				      form,
				      args, num_args);
      XtAddCallback (single,
		     XtNcallback,
		     ActivateWidgetsAndSetResourceString,
		     (XtPointer)node);

      num_args = 1;   /* keep the XtNtranslations */
      XtSetArg (args[num_args],
		XtNfromHoriz, any);
      num_args++;
      XtSetArg (args[num_args],
		XtNfromVert, single);
      num_args++;
      XtSetArg (args[num_args],
		XtNradioGroup, name);
      num_args++;
      XtSetArg (args[num_args],
		XtNradioData, ANY_RADIO_DATA);
      num_args++;
      any = XtCreateManagedWidget ("any",
				   toggleWidgetClass,
				   form,
				   args, num_args);

      name_info->name_leader = name;

      MakeBoxLookNice (dot,
		       star,
		       any,
		       single,
		       name,
		       class,
		       (i == 0  ?  -1 : (names[i+1] ? 0 : 1) ));

      i++;
    }   /* while (TRUE) */

  name_info->next = NULL;
  XtFree ((char*) names);	/* Free what you allocate... */
  XtFree ((char*) classes);
#endif
}   /* CreateResourceNameForm() */

/* Function Name:
 *   SetToggleGroupLeaders
 *
 * Description:
 *   Sets the leaders of each toggle group.
 *
 * Arguments:
 *   node - The widget node containing this res box.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   SetResourceString - handler.c
 */
static void
SetToggleGroupLeaders(WNode *node)
{
#if 0
  NameInfo        *name;
  ResourceBoxInfo *res_box = node->resources->res_box;
  static Arg      args[] =
  {
    {XtNstate, (XtArgVal) TRUE}
  };


  name = res_box->name_info;
  while (name != NULL)
    {
      XtSetValues (name->sep_leader,
		   args, XtNumber (args));

      if (name->name_leader != NULL)
	{
	  XtSetValues (name->name_leader,
		       args, XtNumber (args));
	}
      name = name->next;
    }   /* while () */

  SetResourceString (NULL, (XtPointer)node, NULL);
#endif

}   /* SetToggleGroupLeaders() */

/* Function Name:
 *   MakeBoxLookNice
 *
 * Description:
 *   Resizes the box that contains the resource names to look a bit nicer.
 *
 * Arguments:
 *   dot, star - the widgets containing the separator types.
 *   any, single, name, class - the widgets that contain the
 *                              name and class of this object.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 */
static void
MakeBoxLookNice(Widget dot, Widget star, Widget any, Widget single,
	Widget name, Widget class, int endbox)
{

#define MAX_HDIST 3

  Arg        args[10];
  Cardinal   num_args;
  Dimension  any_width;
  Dimension  name_class_width;
  Dimension  dot_star_width;
  Dimension  width_1;
  Dimension  width_2;
  int        h_dist[MAX_HDIST];
  int        i;


  /*
   * Make sure that the dot and star widgets are the same size.
   */

  num_args = 0;
#if 0
  XtSetArg (args[num_args],
	    XtNhorizDistance, &(h_dist[0]));
  num_args++;
#endif
  XtSetArg (args[num_args],
	    XtNwidth, &width_1);
  num_args++;
  XtGetValues (dot,
	       args, num_args);

  num_args = 0;
#if 0
  XtSetArg (args[num_args],
	    XtNhorizDistance, &(h_dist[1]));
  num_args++;
#endif
  XtSetArg (args[num_args],
	    XtNwidth, &width_2);
  num_args++;
  XtGetValues (star,
	       args, num_args);

  num_args = 0;
#if 0
  XtSetArg (args[num_args],
	    XtNhorizDistance, &(h_dist[2]));
  num_args++;
#endif
  XtSetArg (args[num_args],
	    XtNwidth, &any_width);
  num_args++;
  XtGetValues (any,
	       args, num_args);

  dot_star_width = (width_1 > width_2) ? width_1 : width_2;
  for (i = 1; i < MAX_HDIST; i++)
    {
      if (h_dist[i] > h_dist[0])
	{
	  h_dist[0] = h_dist[i];
	}
    }   /* for() */

  num_args = 0;
#if 0
  XtSetArg (args[num_args],
	    XtNhorizDistance, h_dist[0]);
  num_args++;
#endif
  XtSetValues (any,
	       args, num_args);

  /*
   * Add a new arg, and continue...
   */
  XtSetArg (args[num_args],
	    XtNwidth, dot_star_width);
  num_args++;
  XtSetValues (star,
	       args, num_args);
  XtSetValues (dot,
	       args, num_args);


  /*
   * Now make sure that the Any Widget is as wide as the longest
   * of the name and class widgets, plus space for the dot and star widgets.
   * Don't forget the Form widget's internal space.
   */

  num_args = 0;
  XtSetArg (args[num_args],
	    XtNwidth, &width_1);
  num_args++;
#if 0
  XtSetArg (args[num_args],
	    XtNhorizDistance, &(h_dist[0]));
  num_args++;
#endif
  XtGetValues (name,
	       args, num_args);

  num_args = 0;
  XtSetArg (args[num_args],
	    XtNwidth, &width_2);
  num_args++;
#if 0
  XtSetArg (args[num_args],
	    XtNhorizDistance, &(h_dist[1]));
  num_args++;
#endif
  XtGetValues (class,
	       args, num_args);

  if (width_2 > width_1)
    {
      width_1 = width_2;
    }

  if (h_dist[1] > h_dist[0])
    {
      h_dist[0] = h_dist[1];
    }

  num_args = 0;
  XtSetArg (args[num_args],
	    XtNwidth, &width_2);
  num_args++;
#if 0
  XtSetArg (args[num_args],
	    XtNhorizDistance, &(h_dist[1]));
  num_args++;
#endif
  XtGetValues (single,
	       args, num_args);

  name_class_width = (width_1 > width_2) ? width_1 : width_2;

  if (h_dist[1] > h_dist[0])
    {
      h_dist[0] = h_dist[1];
    }

  if (any_width > name_class_width)
    {
      name_class_width = any_width;
    }

  any_width = dot_star_width + h_dist[0] + name_class_width;
  if (endbox < 0)
    {
      any_width += dot_star_width / 2;
    }
  else if (endbox > 0)
    {
      any_width += (dot_star_width - dot_star_width / 2);
    }
  /* else */
    /* {} */

  num_args = 0;
  XtSetArg (args[num_args],
	    XtNwidth, any_width);
  num_args++;
  XtSetValues (any,
	       args, num_args);

  num_args = 0;
  XtSetArg (args[num_args],
	    XtNwidth, name_class_width);
  num_args++;
#if 0
  XtSetArg (args[num_args],
	    XtNhorizDistance, h_dist[0]);
  num_args++;
#endif
  XtSetValues (name,
	       args, num_args);
  XtSetValues (class,
	       args, num_args);
  XtSetValues (single,
	       args, num_args);
}   /* MakeBoxLookNice() */

/* Function Name:
 *   CreateLists
 *
 * Description:
 *   Creates the list widgets for the normal and constraint resources
 *
 * Arguments:
 *   parent - parent of the lists.
 *   node - The widget node containing this res box.
 *   names, cons_names - lists for norm and constraint resource boxes.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   Xt-stuff
 *
 * Callbacks installed:
 *   ResourceListCallback - handler.c
 *   FreeClientData - (self)
 */

static char *noneList[] =
{"None"};

static void
CreateLists (Widget parent, WNode *node, char **names, char **cons_names)
{
#if 0
  Cardinal        num_args;
  ResourceBoxInfo *res_box = node->resources->res_box;
  Arg             args[3];


  (void)XtCreateManagedWidget ("namesLabel",
			       labelWidgetClass,
			       parent,
			       NULL, ZERO);

  num_args = 0;
  /*
   * if the first list item is the widget name we want an empty
   * list.
   */
  if (!names)
    {
      XtSetArg (args[num_args],
		XtNlist, noneList);
      num_args++;
      XtSetArg (args[num_args],
		XtNnumberStrings, 1);
      num_args++;
      XtSetArg (args[num_args],
		XtNsensitive, False);
      num_args++;
    }

  else
    {
      XtSetArg (args[num_args],
		XtNlist, names);
      num_args++;
    }

  res_box->norm_list = XtCreateManagedWidget ("namesList",
					      listWidgetClass,
					      parent,
					      args, num_args);
  XtAddCallback (res_box->norm_list,
		 XtNcallback, ResourceListCallback, (XtPointer)node);
  XtAddCallback (res_box->norm_list,
		 XtNdestroyCallback, FreeClientData, (XtPointer)names);

  if (cons_names != NULL)
    {
      (void) XtCreateManagedWidget ("constraintLabel",
				    labelWidgetClass,
				    parent,
				    NULL, ZERO);

      num_args = 0;
      XtSetArg (args[num_args],
		XtNlist, cons_names);
      num_args++;
      res_box->cons_list = XtCreateManagedWidget ("constraintList",
						  listWidgetClass,
						  parent,
						  args, num_args);
      XtAddCallback (res_box->cons_list,
		     XtNcallback, ResourceListCallback, (XtPointer)node);
      XtAddCallback (res_box->cons_list,
		     XtNdestroyCallback, FreeClientData, (XtPointer)cons_names);
    }   /* if (cons_names != NULL) */

  else
    {
      res_box->cons_list = NULL;
    }
#endif
}   /* CreateLists() */

/* Function Name:
 *   CreateValueWidget
 *
 * Description:
 *   Creates the value widget for entering the resources value.
 *
 * Arguments:
 *   parent - parent of this widget.
 *   res_box - the resource box info.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xt-stuff
 *
 * Callbacks installed:
 *   (SetResourceString - handler.c)
 */
static void
CreateValueWidget (Widget parent, WNode  *node)
{
#if 0
  Widget          form;
  Widget          label;
  Cardinal        num_args;
  Arg             args[10];
  ResourceBoxInfo *res_box = node->resources->res_box;


  form = XtCreateManagedWidget ("valueForm",
				formWidgetClass,
				parent,
				NULL, ZERO);

  num_args = 0;
  XtSetArg (args[num_args],
	    XtNleft, XawChainLeft);
  num_args++;
  XtSetArg (args[num_args],
	    XtNright, XawChainLeft);
  num_args++;
  XtSetArg (args[num_args],
	    XtNtop, XawChainTop);
  num_args++;
  XtSetArg (args[num_args],
	    XtNbottom, XawChainBottom);
  num_args++;
  label = XtCreateManagedWidget ("valueLabel",
				 labelWidgetClass,
				 form,
				 args, num_args);

  num_args = 0;
  XtSetArg (args[num_args],
	    XtNfromHoriz, label);
  num_args++;
  XtSetArg (args[num_args],
	    XtNleft, XawChainLeft);
  num_args++;
  XtSetArg (args[num_args],
	    XtNright, XawChainRight);
  num_args++;
  XtSetArg (args[num_args],
	    XtNtop, XawChainTop);
  num_args++;
  XtSetArg (args[num_args],
	    XtNbottom, XawChainBottom);
  num_args++;
  res_box->value_wid = XtCreateManagedWidget ("valueText",
					      asciiTextWidgetClass,
					      form,
					      args, num_args);
#ifdef notdef
  XtAddCallback (XawTextGetSource (res_box->value_wid),
		 XtNcallback, SetResourceString, (XtPointer)node);
#endif
#endif
}   /* CreateValueWidget() */


/* Function Name:
 *   PopupOnNode
 *
 * Description:
 *   Pops a shell widget up centered on the node specified.
 *
 * Arguments:
 *   node - the node.
 *   shell - the shell to popup.
 *
 * Returns:
 *   none.
 *
 * Calls:
 *   Xlib- and Xt-stuff
 */

static void
PopupOnNode (WNode *node, Widget shell)
{
  Arg       args[3];
  Cardinal  num_args;
  Position  x;
  Position  y;
  Dimension width;
  Dimension height;
  Dimension bw;


  /* if this node is not shown, we can return */
  if (node->widget == NULL) {
      return;
  }

  num_args = 0;
  XtSetArg (args[num_args],
	    XtNwidth, &width);
  num_args++;
  XtSetArg (args[num_args],
	    XtNheight, &height);
  num_args++;
  XtSetArg (args[num_args],
	    XtNborderWidth, &bw);
  num_args++;
  XtGetValues (node->widget, args, num_args);
  XtTranslateCoords (node->widget,
		     (Position) (width / 2 + bw),
		     (Position) (height / 2 + bw),
		     &x,
		     &y);

  XtOverrideTranslations
   (shell, XtParseTranslationTable ("<Message>WM_PROTOCOLS: quit()"));
  XtRealizeWidget (shell);
  wm_delete_window = XInternAtom (XtDisplay (shell), "WM_DELETE_WINDOW", False);
  (void)XSetWMProtocols (XtDisplay (shell), XtWindow (shell), &wm_delete_window, 1);
  XtGetValues (shell, args, num_args);	/* use same arg_list. */

  x -= (Position)(width / 2 + bw);
  y -= (Position)(height / 2 + bw);

  /* Set x position between 0 and the maximum to see full window */
  if (x < 0) {
      x = 0;
  } else { /* if (lower limit for x is 0) */
      Position max_loc = WidthOfScreen(XtScreen(shell)) - (Position)(width + 2*bw);
      if (x > max_loc) {
	  x = max_loc;
	}   /* if (upper limit for x ist max_loc) */
      /* else */
      /* {} */

  }   /* else (x is >= 0) */

  /* Set y position between 0 and the maximum to see full window */
  if (y < 0)
    {
      y = 0;
    }   /* if (lower limit for y is 0) */

  else
    {
      Position max_loc
	= HeightOfScreen(XtScreen(shell)) - (Position)(height + 2*bw);
      if (y > max_loc)
	{
	  y = max_loc;
	}   /* if (upper limit for y is max_loc) */
      /* else */
      /* {} */

    }   /* else ( y is >= 0) */

  num_args = 0;
  XtSetArg (args[num_args], XtNx, x);
  num_args++;
  XtSetArg (args[num_args], XtNy, y);
  num_args++;
  XtSetValues (shell, args, num_args);

  XtPopup (shell, XtGrabNone);
}   /* PopupOnNode() */

/* Function Name:
 *   FreeClientData
 *
 * Description:
 *   Frees the client data passed to this function.
 *
 * Arguments:
 *   w - UNUSED.
 *   list_ptr - pointer to the list to check.
 *   junk - UNUSED.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   XtFree - Xtoolkit
 */

/* ARGSUSED */
static void
FreeClientData (Widget w, XtPointer ptr, XtPointer junk)
{
  XtFree (ptr);
}   /* FreeClientData() */

/* Function Name:
 *   FreeResBox.
 *
 * Description:
 *   Frees resource box allocated memory.
 *
 * Arguments:
 *   w - UNUSED.
 *   ptr - pointer to the node that has this resources box.
 *   junk - UNUSED.
 *
 * Returns:
 *   none
 *
 * Calls:
 *   XtFree - Xtoolkit
 */

/* ARGSUSED */
static void
FreeResBox (Widget w, XtPointer ptr, XtPointer junk)
{
  WNode    *node = (WNode*)ptr;
  NameInfo *old_name;
  NameInfo *name = node->resources->res_box->name_info;


  global_resource_box_up = FALSE;

  XtFree ((XtPointer)node->resources->res_box);
  node->resources->res_box = NULL;

  while (name != NULL)
    {
      old_name = name;
      name = name->next;
      XtFree ((XtPointer)old_name);
    }   /* while() */

}   /* FreeResBox() */

/*
 * PopupTreeMenu
 */
static void
PopupTreeMenu(Widget w, XtPointer client, XtPointer call)
{
	XmContainerSelectCallbackStruct	*cbp = (XmContainerSelectCallbackStruct *)call;
	int				i;

	fprintf(stderr, "PopupTreeMenu : %d selected items\n",
			cbp->selected_item_count);
	for (i=0; i<cbp->selected_item_count; i++) {
		fprintf(stderr, "\t%s\n", XtName(cbp->selected_items[i]));
	}

/*
	GetResourceList(w, client, call);
 */
}

static void
TreeHighlight(Widget w, XtPointer client, XtPointer call)
{
	XmContainerSelectCallbackStruct	*cbp = (XmContainerSelectCallbackStruct *)call;
	XtPointer			id;
	int				i;

	if (global_tree_info->active_nodes == NULL)
		global_tree_info->active_nodes = (WNode **)XtCalloc(5, sizeof(WNode *));

	if (cbp->selected_item_count) {
		for (i=0; i<cbp->selected_item_count; i++) {
			XtVaGetValues(cbp->selected_items[i],
				XmNuserData, &id,
				NULL);
			global_tree_info->active_nodes[i] = 
			        FindNodeById(global_tree_info->top_node,
				             (unsigned long)id);
		}
		global_tree_info->num_active = cbp->selected_item_count;
	} else {
		global_tree_info->active_nodes[0] = NULL;
		global_tree_info->num_active = 0;
	}

}
