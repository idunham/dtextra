
/***************************************************************************/
/* callbacks.h: callback routines                                          */
/***************************************************************************/

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <Tabs.h>
#include "xplore.h"

/* file ops */

void OpenCB(Widget, XtPointer, XtPointer); 
void CommandActionCB(Widget, int, XtPointer);
void MkdirCB(Widget, XtPointer, XtPointer); 
void ChdirCB(Widget, XtPointer, XtPointer); 
void ParentCB(Widget, XtPointer, XtPointer); 
void HomeCB(Widget, XtPointer, XtPointer); 
void RenameCB(Widget, XtPointer, XtPointer); 
void MoveCB(Widget, XtPointer, XtPointer); 
void CopyCB(Widget, XtPointer, XtPointer); 
void LinkCB(Widget, XtPointer, XtPointer); 
void DeleteCB(Widget, XtPointer, XtPointer); 
void PropertiesCB(Widget, XtPointer, XtPointer); 
void BindingsCB(Widget, XtPointer, XtPointer); 
void SelectCB(Widget, XtPointer, XtPointer); 
void SelectAllCB(Widget, XtPointer, XtPointer); 
void InvertAllCB(Widget, XtPointer, XtPointer); 
void FilterCB(Widget, XtPointer, XtPointer); 
void QuitCB(Widget, XtPointer, XtPointer); 
void MenuActionCB(Widget, int, XtPointer);

/* shelf ops */

void ShelfOpenCB(Widget, XtPointer, XtPointer); 
void ShelfRenameCB(Widget, XtPointer, XtPointer); 
void ShelfMoveCB(Widget, XtPointer, XtPointer); 
void ShelfCopyCB(Widget, XtPointer, XtPointer); 
void ShelfLinkCB(Widget, XtPointer, XtPointer); 
void ShelfDeleteCB(Widget, XtPointer, XtPointer); 
void ShelfPropertiesCB(Widget, XtPointer, XtPointer); 
void ShelfBindingsCB(Widget, XtPointer, XtPointer); 
void ShelfSelectCB(Widget, XtPointer, XtPointer); 
void ShelfSelectAllCB(Widget, XtPointer, XtPointer); 
void ShelfInvertAllCB(Widget, XtPointer, XtPointer); 
void ShelfMenuActionCB(Widget, int, XtPointer);

/* view ops */

void LargeIconsCB(Widget, XtPointer, XtPointer); 
void SmallIconsCB(Widget, XtPointer, XtPointer);
void DetailCB(Widget, XtPointer, XtPointer); 
void TreePaneCB(Widget, XtPointer, XtPointer); 
void FilePaneCB(Widget, XtPointer, XtPointer); 
void ShelfPaneCB(Widget, XtPointer, XtPointer); 
void LogPaneCB(Widget, XtPointer, XtPointer); 
void SortByNameCB(Widget, XtPointer, XtPointer); 
void SortBySizeCB(Widget, XtPointer, XtPointer); 
void SortByDateCB(Widget, XtPointer, XtPointer); 
void ReverseCB(Widget, XtPointer, XtPointer); 
void DirsFirstCB(Widget, XtPointer, XtPointer); 
void ShowHiddenFilesCB(Widget, XtPointer, XtPointer); 
void ShowParentDirCB(Widget, XtPointer, XtPointer); 
void ShowFilesCB(Widget, XtPointer, XtPointer); 
void ShowDirsCB(Widget, XtPointer, XtPointer); 
void RescanFilesCB(Widget, XtPointer, XtPointer); 
void MagicHeadersCB(Widget, XtPointer, XtPointer); 
void UpdateCB(Widget, XtPointer, XtPointer); 
void RereadCB(Widget, XtPointer, XtPointer); 
void UnmountCB(Widget, XtPointer, XtPointer); 
void UnmountAllCB(Widget, XtPointer, XtPointer); 
void ClearLogCB(Widget, XtPointer, XtPointer); 

/* option ops */

void AbsolutePathsCB(Widget, XtPointer, XtPointer); 
void TargetDirCB(Widget, XtPointer, XtPointer); 
void PushDirCB(Widget, XtPointer, XtPointer); 
void EchoCommandsCB(Widget, XtPointer, XtPointer); 
void CheckMountsCB(Widget, XtPointer, XtPointer); 
void BackupsCB(Widget, XtPointer, XtPointer); 
void ShowSplashCB(Widget w, XtPointer, XtPointer);
void AutosaveDefaultsCB(Widget w, XtPointer, XtPointer);
void AutosaveSessionCB(Widget w, XtPointer, XtPointer);
void AutopopupCB(Widget w, XtPointer, XtPointer);
void AutoUpdatesCB(Widget w, XtPointer, XtPointer);
void ConfirmDropCB(Widget, XtPointer, XtPointer); 
void ConfirmMoveCB(Widget, XtPointer, XtPointer); 
void ConfirmCopyCB(Widget, XtPointer, XtPointer); 
void ConfirmLinkCB(Widget, XtPointer, XtPointer); 
void ConfirmDeleteCB(Widget, XtPointer, XtPointer); 
void ConfirmDelDirCB(Widget, XtPointer, XtPointer); 
void ConfirmDelFileCB(Widget, XtPointer, XtPointer); 
void ConfirmOverwriteCB(Widget, XtPointer, XtPointer); 
void ConfirmQuitCB(Widget, XtPointer, XtPointer); 
void SchemeCB(Widget, XtPointer, XtPointer); 
void SaveSessionCB(Widget, XtPointer, XtPointer); 
void ClearSessionCB(Widget, XtPointer, XtPointer); 
void SaveDefaultsCB(Widget, XtPointer, XtPointer); 
void ReloadConfigCB(Widget, XtPointer, XtPointer); 

/* help ops */

void HelpCB(Widget, XtPointer, XtPointer);
void AboutCB(Widget, XtPointer, XtPointer); 

/* log ops */

void LogPopupCB(Widget, XtPointer, XtPointer);
void LocateCB(Widget, XtPointer, XtPointer);

/* shelf tabs callback */

void ShelfCB(Widget w, XtPointer app_data, XgTabsCallbackStruct *tcs);

/* file/shelf/tree pane container callbacks */

void FileActionCB(Widget w, XtPointer app_data,
		  XmContainerSelectCallbackStruct *cscs);
void FileSelectionCB(Widget w, XtPointer app_data,
		     XmContainerSelectCallbackStruct *cscs);
void ShelfActionCB(Widget w, XtPointer app_data,
		   XmContainerSelectCallbackStruct *cscs);
void ShelfSelectionCB(Widget w, XtPointer app_data,
		      XmContainerSelectCallbackStruct *cscs);
void TreeOutlineCB(Widget w, XtPointer app_data,
		   XmContainerOutlineCallbackStruct *cocs);
void TreeSelectionCB(Widget w, XtPointer app_data,
		     XmContainerSelectCallbackStruct *cscs);

/* drag and drop callbacks of file/shelf/tree pane containers */

void FileConvertCB(Widget w, XtPointer app_data,
		   XmConvertCallbackStruct *ccs);
void ShelfConvertCB(Widget w, XtPointer app_data,
		    XmConvertCallbackStruct *ccs);
void TreeConvertCB(Widget w, XtPointer app_data,
		   XmConvertCallbackStruct *ccs);
void FileDestinationCB(Widget w, XtPointer app_data,
		       XmDestinationCallbackStruct *dcs);
void ShelfDestinationCB(Widget w, XtPointer app_data,
			XmDestinationCallbackStruct *dcs);
void TreeDestinationCB(Widget w, XtPointer app_data,
		       XmDestinationCallbackStruct *dcs);
void DragProcCB(Widget w, XtPointer app_data,
		XmDragProcCallbackStruct *dpcs);
void DropFinishCB(Widget w, XtPointer app_data,
		  XmDropFinishCallbackStruct *dfcs);

/* input callback */

void InputCB(XtPointer data, int *fd, XtInputId *id);

/* timer callback for automatic updates */

extern Boolean lock; /* set to inhibit updates */
void TimerCB(XtPointer data, XtIntervalId *id);

/* message handlers */

void UpdateH(Widget w, XtPointer client_data, XEvent *event);
void DeleteWindowCB(Widget, XtPointer, XtPointer); 
void SaveYourselfCB(Widget, XtPointer, XtPointer); 

#endif /* CALLBACKS_H */
