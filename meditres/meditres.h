/*
 * $Header: /cvsroot-fuse/meditres/meditres/meditres.h,v 1.11 2002/01/16 20:05:43 amai Exp $
 *
 * Copyright © 2001 by Danny Backx.
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
 * author BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the author shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from the author.
 */

#ifndef _MEDITRES_H
#define _MEDITRES_H

#include "editresP.h"


/* actions.c: */
extern Boolean  do_get_values;
extern void SetApplicationActions(XtAppContext app_con);


/* appres.c: */
extern char *fallback[];


/* comm.c: */
extern void InternAtoms(Display *dpy);
extern void SetCommand(Widget w, ResCommand command, char *msg);


/* editres.c: */
extern Atom wm_delete_window;


/* finddlg.c: */
extern void FindEntry(Widget, XEvent *, String *, Cardinal *);
extern void PopupFindDialog(Widget parent, XEvent *event);
extern void SearchPopup(Widget, XEvent *, String *, Cardinal *);


/* geometry.c: */
extern void DisplayChild(Event *);
extern char *HandleFlashWidget(Event *);
extern void _FindWidget(Widget);
extern void _FlashActiveWidgets(TreeInfo *);


/* handler.c: */
extern void ActivateResourceWidgets(Widget w, XtPointer node_ptr, XtPointer junk);
extern void FindWidget(Widget w, XtPointer client_data, XtPointer call_data);
extern void FlashActiveWidgets(Widget w, XtPointer junk, XtPointer garbage);
extern void InitFindDlg(Widget w, XtPointer call_data, XtPointer client_data);
extern void InitSetValues(Widget w, XtPointer call_data, XtPointer client_data);
extern void PopdownResBox(Widget w, XtPointer shell_ptr, XtPointer junk);
extern void ResourceListCallback(Widget list, XtPointer node_ptr, XtPointer junk);
extern void SetResourceString(Widget w, XtPointer node_ptr, XtPointer call_data);
extern void Quit(Widget w, XtPointer client, XtPointer call);
extern void brApplyResource(char *, char *, char *);
extern void brConnectToClient(XtPointer , XtIntervalId *);
extern void printExpose(Widget, XtPointer, XEvent *, Boolean *);


/* motif.c: */
extern Widget MessageArea;
extern void BuildWidgetTree(Widget parent);
extern void LayoutTree (Widget tree);
extern void PrepareToLayoutTree (Widget tree);
extern void RebuildMenusAndLabel (String toolkit);


/* print.c: */
extern void printTree(Widget, XtPointer, XtPointer);


/* setvalues.c: */
char *PrintSetValuesError(Event *event);
char *brGetResourceValueForSetValues(char *, unsigned short *);


/* svpopup.c: */
extern void PopupSetValues(Widget parent, XEvent *event);
extern void PopupSetValues(Widget, XEvent *);
extern void ModifySVEntry(Widget, XEvent *, String *, Cardinal *);


/* utils.c: */
extern void AddString(char **, char *);
extern Boolean CheckDatabase(XrmDatabase db, XrmQuarkList names, XrmQuarkList classes);
extern void CreateResourceBox(WNode *node, char **errors);
extern void ExecuteOverAllNodes(WNode *top_node, void (*func)(), XtPointer data);
extern WNode* FindNode(WNode *top_node, unsigned long *ids, Cardinal number);
extern char* GetFailureMessage(ProtocolStream *stream);
extern void GetNamesAndClasses(WNode *node, char  ***names, char  ***classes);
extern char* HandleGetResources(Event *event);
extern int HandleXErrors(Display *display, XErrorEvent *error);
extern void InsertWidgetFromNode(ProtocolStream *stream, WNode *node);
extern void NextLine(char *);
void PopupCentered(XEvent *event, Widget w, XtGrabKind mode);
extern char* ProtocolFailure(ProtocolStream *stream);
extern void startWait(XtAppContext app_con, int time);
extern void  _DumpTreeToFile(Widget w, XtPointer tree_ptr, char *filename);
extern void _PopupFileDialog(Widget w, String str, String default_value,
                 void (*func)(), XtPointer data);


/* wtree.c: */
extern void BuildVisualTree(Widget, Event *);
extern WNode **CopyActiveNodes (TreeInfo *tree_info);
extern TreeInfo *CreateTree(Event *event);
extern void PerformTreeToFileDump (WNode *, int, FILE *);
extern XrmQuarkList Quarkify(char **, char *);
extern void RemoveNodeFromActiveList (WNode *node);
extern void SetAndCenterTreeNode (WNode *node);
extern void _TreeActivateNode (WNode *node, SelectTypes type);
extern void _TreeRelabel (TreeInfo *tree_info, LabelTypes type);
extern void _TreeRelabelNode (WNode *node, LabelTypes type, Boolean recurse);
extern void _TreeSelect (TreeInfo *tree_info, SelectTypes type);
extern void _TreeSelectNode(WNode *node, SelectTypes type, Boolean recurse);
extern void _newTopParent(Boolean recursive);
extern void _treeFold(WNode *node, FoldTypes type);


/* unsorted ... */
extern void CreateResourceBoxWidgets(WNode *, char **, char **);
extern void ApplyResource(Widget, XtPointer, XtPointer);
extern WNode * FindNodeById(WNode *top, unsigned long id);

extern void WidgetTreeRemoveChildren(Widget, Widget);
extern void AddTreeNode(Widget, WNode *);
extern void SetNumWidLabel(int);
extern void SetWorkLabel(int);
extern void SetMessage(Widget, char *, char *);
extern Boolean IsActiveNode(WNode *);
extern void SetShownWidLabel(int);
extern void OpenAdditionalResourceBoxes(void);
extern Boolean notShown(WNode *);
extern void brSetCommand(Widget, ResCommand, char *);
extern void SetSelWidLabel(int);
extern void SetWaitLabel(int);
extern void SetZoomLabel(int);
extern void SetRcvLabel(Boolean);
extern void SendTree(Widget, XtPointer, XtPointer);
extern void DumpTreeToFile(Widget, XtPointer, XtPointer);
extern void DumpTreeAction(Widget, XEvent *, String *, Cardinal *);
extern void GetResourceList(Widget, XtPointer, XtPointer);
extern void _newTopSub(WNode *);
extern void displayNewTree(void);
extern void newTop(Widget, XtPointer, XtPointer);
extern void showAll(Widget, XtPointer, XtPointer);

#endif /* _MEDITRES_H */
