/*-----------------------------------------------------------------------------
 * $Header: /cvsroot/xlt/Xlt/lib/ListTree.h,v 1.13 2001/09/26 00:22:59 amai Exp $
 *
 * ListTree	A list widget that displays a file manager style tree
 *
 * Copyright © 1996 Robert W. McMullen
 * Copyright © 1999, 2000, 2001 by the LessTif developers.
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
 *
 * Author: Rob McMullen <rwmcm@mail.ae.utexas.edu>
 *         http://www.ae.utexas.edu/~rwmcm
 */

#ifndef _ListTree_H
#define _ListTree_H

#include <X11/Core.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>

#define _ListTree_WIDGET_VERSION	3.1

#ifdef	LISTTREE_COMPAT
#define XtNmargin		"margin"
#define XtNindent		"indent"
#define XtNspacing		"spacing"
#define XtNhorizontalSpacing	"horizontalSpacing"
#define XtNverticalSpacing	"verticalSpacing"
#define XtNlineWidth		"lineWidth"
#define XtNhighlightPath	"highlightPath"
#define XtNclickPixmapToOpen	"clickPixmapToOpen"
#define XtNdoIncrementalHighlightCallback	"incCallback"
#define XtNbranchPixmap		"branchPixmap"
#define XtNbranchOpenPixmap	"branchOpenPixmap"
#define XtNleafPixmap		"leafPixmap"
#define XtNleafOpenPixmap	"leafOpenPixmap"
#define XtNactivateCallback	"activateCallback"
#define XtNhighlightCallback	"highlightCallback"
#define XtNmenuCallback		"menuCallback"
#define XtNdestroyItemCallback	"destroyItemCallback"

#define XtBRANCH	1
#define XtLEAF		2
#define XtMENU		3
#define XtDESTROY	4

#endif

/*
 * These are XLT prefixed names
 */
#define XltNmargin				"margin"
#define XltNindent				"indent"
#define XltNspacing				"spacing"
#define XltNhorizontalSpacing			"horizontalSpacing"
#define XltNverticalSpacing			"verticalSpacing"
#define XltNlineWidth				"lineWidth"
#define XltNhighlightPath			"highlightPath"
#define XltNclickPixmapToOpen			"clickPixmapToOpen"
#define XltNdoIncrementalHighlightCallback	"incCallback"
#define XltNbranchPixmap			"branchPixmap"
#define XltNbranchOpenPixmap			"branchOpenPixmap"
#define XltNleafPixmap				"leafPixmap"
#define XltNleafOpenPixmap			"leafOpenPixmap"
#define XltNactivateCallback			"activateCallback"
#define XltNhighlightCallback			"highlightCallback"
#define XltNmenuCallback			"menuCallback"
#define XltNdestroyItemCallback			"destroyItemCallback"
#define	XltNdefaultMode				"defaultMode"
#define XltCDefaultMode				"DefaultMode"

#define XltBRANCH	1
#define XltLEAF		2
#define XltMENU		3
#define XltDESTROY	4


#ifdef __cplusplus
extern "C" {
#endif

extern WidgetClass listtreeWidgetClass;
extern WidgetClass xltListtreeWidgetClass;

typedef struct _ListTreeClassRec *ListTreeWidgetClass;
typedef struct _ListTreeRec      *ListTreeWidget;
typedef struct _XltListTreeClassRec *XltListTreeWidgetClass;
typedef struct _XltListTreeRec      *XltListTreeWidget;

typedef enum _XltListTreeItemType {
    ItemDetermineType = 0,
    ItemBranchType = XltBRANCH,
    ItemLeafType = XltLEAF
} XltListTreeItemType;

typedef struct _XltListTreeItem XltListTreeItem;

struct _XltListTreeItem {
  Boolean	open;
  Boolean	highlighted;
  char		*text;
  int		length;
  int		x,y,ytext;
  int		count;
  Dimension	height;
  XltListTreeItemType type;
  XltListTreeItem 	*parent,*firstchild,*prevsibling,*nextsibling;
  Pixmap	openPixmap,closedPixmap;
  XtPointer	user_data;
};

typedef struct _XltListTreeReturnStruct {
  int		reason;
  XltListTreeItem	*item;
  XltListTreeItem	**path;
  int		count;
  Boolean	open;
} XltListTreeReturnStruct;


typedef struct _XltListTreeMultiReturnStruct {
  XltListTreeItem	**items;
  int		count;
} XltListTreeMultiReturnStruct;

typedef struct _XltListTreeActivateStruct {
  int		reason;
  XltListTreeItem	*item;
  Boolean	open;
  XltListTreeItem	**path;
  int		count;
} XltListTreeActivateStruct;

typedef struct _XltListTreeItemReturnStruct {
  int		reason;
  XltListTreeItem	*item;
  XEvent	*event;
} XltListTreeItemReturnStruct;

#ifdef	LISTTREE_COMPAT
typedef XltListTreeItemReturnStruct ListTreeItemReturnStruct;
typedef XltListTreeActivateStruct ListTreeActivateStruct;
typedef XltListTreeReturnStruct ListTreeReturnStruct;
typedef XltListTreeItemType ListTreeItemType;
typedef XltListTreeItem ListTreeItem;
typedef XltListTreeMultiReturnStruct ListTreeMultiReturnStruct;
#endif

/*
** Public function declarations
*/

/* ListTree.c */
void XltListTreeRefresh (Widget w);
void XltListTreeRefreshOff (Widget w);
void XltListTreeRefreshOn (Widget w);
XltListTreeItem *XltListTreeAdd (Widget w, XltListTreeItem *parent, char *string);
XltListTreeItem *XltListTreeAddType (Widget w, XltListTreeItem *parent, char *string, XltListTreeItemType type);
XltListTreeItem *XltListTreeAddBranch (Widget w, XltListTreeItem *parent, char *string);
XltListTreeItem *XltListTreeAddLeaf (Widget w, XltListTreeItem *parent, char *string);
void XltListTreeSetItemPixmaps (Widget w, XltListTreeItem *item, Pixmap openPixmap, Pixmap closedPixmap);
void XltListTreeRenameItem (Widget w, XltListTreeItem *item, char *string);
int XltListTreeDelete (Widget w, XltListTreeItem *item);
int XltListTreeDeleteChildren (Widget w, XltListTreeItem *item);
int XltListTreeReparent (Widget w, XltListTreeItem *item, XltListTreeItem *newparent);
int XltListTreeReparentChildren (Widget w, XltListTreeItem *item, XltListTreeItem *newparent);
int XltListTreeOrderSiblings (Widget w, XltListTreeItem *item);
int XltListTreeOrderChildren (Widget w, XltListTreeItem *item);
int XltListTreeUserOrderChildren (Widget w, XltListTreeItem *item, int (*func)(const void *, const void *));
int XltListTreeUserOrderSiblings (Widget w, XltListTreeItem *item, int (*func)(const void *, const void *));
XltListTreeItem *XltListTreeFindSiblingName (Widget w, XltListTreeItem *item, char *name);
XltListTreeItem *XltListTreeFindChildName (Widget w, XltListTreeItem *item, char *name);
void XltListTreeGetPathname(XltListTreeReturnStruct * ret, char *dir);
void XltListTreeGetPathnameFromItem(XltListTreeItem * item, char *dir);
void XltListTreeHighlightItem (Widget w, XltListTreeItem *item);
void XltListTreeClearHighlighted(Widget w);
void XltListTreeHighlightAll (Widget w);
XltListTreeItem *XltListTreeFirstItem (Widget w);
Position XltListTreeGetItemPosition(XltListTreeWidget w, XltListTreeItem * item);
void XltListTreeGetHighlighted(Widget w,XltListTreeMultiReturnStruct *ret);
void XltListTreeSetHighlighted(Widget w,XltListTreeItem **items,
			    int count,Boolean clear);
void XltListTreeOpen(Widget w, XltListTreeItem *item, Boolean s);

Widget XmCreateScrolledListTree (Widget parent, char *name, Arg *args, Cardinal count);
Widget XltCreateScrolledListTree (Widget parent, char *name, Arg *args, Cardinal count);


#ifdef	LISTTREE_COMPAT
void ListTreeRefresh (Widget w);
void ListTreeRefreshOff (Widget w);
void ListTreeRefreshOn (Widget w);
ListTreeItem *ListTreeAdd (Widget w, ListTreeItem *parent, char *string);
ListTreeItem *ListTreeAddType (Widget w, ListTreeItem *parent, char *string, ListTreeItemType type);
ListTreeItem *ListTreeAddBranch (Widget w, ListTreeItem *parent, char *string);
ListTreeItem *ListTreeAddLeaf (Widget w, ListTreeItem *parent, char *string);
void ListTreeSetItemPixmaps (Widget w, ListTreeItem *item, Pixmap openPixmap, Pixmap closedPixmap);
void ListTreeRenameItem (Widget w, ListTreeItem *item, char *string);
int ListTreeDelete (Widget w, ListTreeItem *item);
int ListTreeDeleteChildren (Widget w, ListTreeItem *item);
int ListTreeReparent (Widget w, ListTreeItem *item, ListTreeItem *newparent);
int ListTreeReparentChildren (Widget w, ListTreeItem *item, ListTreeItem *newparent);
int ListTreeOrderSiblings (Widget w, ListTreeItem *item);
int ListTreeOrderChildren (Widget w, ListTreeItem *item);
int ListTreeUserOrderChildren (Widget w, ListTreeItem *item, int (*func)(const void *, const void *));
int ListTreeUserOrderSiblings (Widget w, ListTreeItem *item, int (*func)(const void *, const void *));
ListTreeItem *ListTreeFindSiblingName (Widget w, ListTreeItem *item, char *name);
ListTreeItem *ListTreeFindChildName (Widget w, ListTreeItem *item, char *name);
void ListTreeGetPathname(ListTreeReturnStruct * ret, char *dir);
void ListTreeGetPathnameFromItem(ListTreeItem * item, char *dir);
void ListTreeHighlightItem (Widget w, ListTreeItem *item);
void ListTreeClearHighlighted(Widget w);
void ListTreeHighlightAll (Widget w);
ListTreeItem *ListTreeFirstItem (Widget w);
Position ListTreeGetItemPosition(ListTreeWidget w, ListTreeItem * item);
void ListTreeGetHighlighted(Widget w,ListTreeMultiReturnStruct *ret);
void ListTreeSetHighlighted(Widget w,ListTreeItem **items,
			    int count,Boolean clear);

Widget XmCreateScrolledListTree (Widget parent, char *name, Arg *args, Cardinal count);
Widget CreateScrolledListTree (Widget parent, char *name, Arg *args, Cardinal count);
#endif

#ifdef __cplusplus
};
#endif

#endif /* _ListTree_H */
