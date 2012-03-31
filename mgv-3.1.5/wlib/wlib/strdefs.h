/*
 * strdefs.h.in
 *	Extra resource strings for wlib.
 *
 * Copyright (C) 1997 Eric A. Howe
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#ifndef	WLIB_STRDEFS_H
#define	WLIB_STRDEFS_H
MU_HID(wlib_strdefs_h, "$Mu: wlib/wlib/strdefs.h 1.36 1998/08/27 03:22:41 $")

/**
 ** Resources.
 **/

/*
 * help stuff
 */
#define	XwlNhelpBrowser		"helpBrowser"
#define	XwlCHelpBrowser		"HelpBrowser"

#define	XwlNhelpChapter		"helpChapter"
#define	XwlCHelpChapter		"HelpChapter"

#define	XwlNhelpPrefix		"helpPrefix"
#define	XwlCHelpPrefix		"HelpPrefix"

#define	XwlNhelpSection		"helpSection"
#define	XwlCHelpSection		"HelpSection"

#define	XwlNhelpType		"helpType"
#define	XwlCHelpType		"HelpType"
#define	XwlRHelpType		"HelpType"

#define	XwlNapplicationVersion	"applicationVersion"
#define	XwlCApplicationVersion	"ApplicationVersion"

#define	XwlNinstallIcon	"wlInstallIcon"
#define	XwlCInstallIcon	"WlInstallIcon"

/*
 * "special" dialog resources
 */
#define	XwlNinitialFocus	"wlInitialFocus"
#define	XwlCInitialFocus	"WlInitialFocus"

#define	XwlNdefaultButton	"wlDefaultButton"
#define	XwlCDefaultButton	"WlDefaultButton"

#define	XwlNcancelButton	"wlCancelButton"
#define	XwlCCancelButton	"WlCancelButton"

#define	XwlNautoDismiss		"wlAutoDismiss"
#define	XwlCAutoDismiss		"WlAutoDismiss"

/*
 * Extra representation types.
 */
#define	XwlRPercentage		"Percentage"

/*
 * Misc.
 */
#define	XwlNuseColorIcon	"wlUseColorIcon"
#define	XwlCUseColorIcon	"WlUseColorIcon"

/*
 * strings
 */
#define	XwlNstringBadVersion	"stringBadVersion"
#define	XwlCStringBadVersion	"StringBadVersion"

#define	XwlNstringCantBrowse	"stringCantBrowse"
#define	XwlCStringCantBrowse	"StringCantBrowse"

#define	XwlNstringCantGetFile	"stringCantGetFile"
#define	XwlCStringCantGetFile	"StringCantGetFile"

#define	XwlNstringEmptyLog	"stringEmptyLog"
#define	XwlCStringEmptyLog	"StringEmptyLog"

#define	XwlNstringNotSupported	"stringNotSupported"
#define	XwlCStringNotSupported	"StringNotSupported"

#define	XwlNstringUnknown	"stringUnknown"
#define	XwlCStringUnknown	"StringUnknown"

/**
 ** Widget types for wlib.
 **/

/*
 * these should cover Motif 1.2
 */
#define	wlArrowB		"ArrowB"
#define	wlArrowBG		"ArrowBG"
#define	wlBulletinB		"BulletinB"
#define	wlCascadeB		"CascadeB"
#define	wlCascadeBG		"CascadeBG"
#define	wlCommand		"Command"
#define	wlDialogS		"DialogS"
#define	wlDrawingA		"DrawingA"
#define	wlDrawnB		"DrawnB"
#define	wlFileSB		"FileSB"
#define	wlForm			"Form"
#define	wlFrame			"Frame"
#define	wlLabel			"Label"
#define	wlLabelG		"LabelG"
#define	wlList			"List"
#define	wlMainW			"MainW"
#define	wlMenuShell		"MenuShell"
#define	wlMessageB		"MessageB"
#define	wlPanedW		"PanedW"
#define	wlPushB			"PushB"
#define	wlPushBG		"PushBG"
#define	wlRowColumn		"RowColumn"
#define	wlScale			"Scale"
#define	wlScrollBar		"ScrollBar"
#define	wlScrolledW		"ScrolledW"
#define	wlSelectionB		"SelectionB"
#define	wlSeparator		"Separator"
#define	wlSeparatorG		"SeparatorG"
#define	wlText			"Text"
#define	wlTextField		"TextField"
#define	wlToggleB		"ToggleB"
#define	wlToggleBG		"ToggleBG"
#define	wlErrorDialog		"ErrorDialog"
#define	wlFileSBDialog		"FileSBDialog"
#define	wlFormDialog		"FormDialog"
#define	wlInformationDialog	"InformationDialog"
#define	wlMessageDialog		"MessageDialog"
#define	wlPromptDialog		"PromptDialog"
#define	wlQuestionDialog	"QuestionDialog"
#define	wlSelectionDialog	"SelectionDialog"
#define	wlTemplateDialog	"TemplateDialog"
#define	wlWarningDialog		"WarningDialog"
#define	wlWorkingDialog		"WorkingDialog"
#define	wlMenuBar		"MenuBar"
#define	wlOptionMenu		"OptionMenu"
#define	wlPopupMenu		"PopupMenu"
#define	wlPulldownMenu		"PulldownMenu"
#define	wlRadioBox		"RadioBox"
#define	wlScrolledList		"ScrolledList"
#define	wlScrolledText		"ScrolledText"

#endif
