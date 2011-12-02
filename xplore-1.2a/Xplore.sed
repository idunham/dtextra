
! Xplore application defaults file
! 03-09-96, revised 03-31-97, 10-25-97, 01-05-01 AG

! application-specific resources: *****************************************

! Note that many of the following resources can also be set interactively
! and be saved to a resources file which will override the settings in this
! file (see the xplore(1) man page).

! base directory of the tree pane (default: /)
?.rootdir:		/

! directory initially shown in the file pane (default: `.' which specifies
! the directory in which xplore is invoked); this *must* be a subdirectory
! of rootdir
?.curdir:		.

! shelf directory (default: ~/shelf)
?.shelfdir:		~/shelf

! shelf to be shown initially (this is commented out to denote the default
! which is the shelf marked as "Default" in the xplorerc file; can also be
! set using the -s command line option)
!?.curshelf:		Desk

! path to search for pixmaps
?.iconpath:		@XPLORELIBDIR@/icons

! personal configuration directory (-c command line option)
?.configdir:		~/.xplore

! show splash screen at startup (default: False)
?.splash:		False

! redirect output to /dev/console (-C; default: False)
?.console:		False

! disable X11R6 session management functionality (-nosm; default: False)
?.nosm:			False

! automagically save defaults when exiting (default: False)
?.autosave:		False

! automagically save session in response to sm/wm request (default: True)
?.autosession:		True

! automagically pop up log window on captured output (default: True)
?.autopopup:		True

! max number of lines in the log (zero means no limit; default: 600)
?.savelines:		600

! options to pass to the C preprocessor when parsing the xplorerc file (-o
! command line option)
?.cppoptions:		-I@XPLORELIBDIR@

! list of Bourne-compatible shells (only needed when shell test at startup
! fails)
!?.bourneShells:	AUTO
!?.bourneShells:	/bin/sh /bin/bash

! shell used to invoke commands -- make sure this is a Bourne-compatible shell,
! the distributed system.xplorerc file depends on it
! NOTE: when not set, the SHELL environment variable is consulted
?.shell:		/bin/sh

! list of shell metacharacters which cause parameters in echoed commands to be
! quoted; the following should do for most shells
! NOTE: this is only for cosmetic purposes; it does not change the way in
! which parameters are actually passed to the shell
?.metachars:		\ \	\n'"`$|&;()<>

! show "real" name of current directory (as returned by getwd) in title bar
! (default: True)
?.getwd:		True

! multiclick time in milliseconds (default: 300)
?.multiclick:		300

! update cycle in milliseconds for the directory view (default: 10000; set to
! zero to completely disable automatic updates)
?.update:		10000

! enable automatic updates (default: True)
?.updates:		True

! default view type for directory displays; recognized values are LargeIcons
! (default), SmallIcons and Detail
?.view:			LargeIcons

! specify the panes to show in the main window (default: all but the log);
! these all belong to class Panes
?.tree:			True
?.file:			True
?.shelf:		True
?.log:			False

! enable various items in the directory display (class Items)
?.updir:		True
?.hidden:		False
?.files:		True
?.dirs:			True

! list subdirectories first in directory display (default)
?.dirsfirst:		True

! sort type for directory displays; recognized values are ByName (default),
! BySize and ByDate
?.sort:			ByName

! sort files in reverse order (default: disabled)
?.reverse:		False

! rescan all files in the directory to check whether directory contents
! has changed, and enable magic headers (both enabled by default)
?.rescan:		True
?.magic:		True

! determine whether mount/umount errors should be handled (default: True)
?.check:		True

! determine whether pathnames in selections are specified as relative or
! absolute pathnames (default: False)
?.absolute:		False

! determine whether push actions on executables in the shelf should be
! invoked in the current directory (True, default) or in the shelf
?.pushdir:		True

! determine whether drop actions should be invoked in the target directory
! (True) or in the directory containing the dropped files (default: False)
?.dirtarget:		False

! echo commands (default: False)
?.echo:			False

! create backups of files to be overwritten (default: True)
?.backups:		True

! confirmation for various operations (class Confirm; all enabled by default)
?.drop:			True
?.move:			True
?.copy:			True
?.link:			True
?.delete:		True
?.deldir:		True
?.delfile:		True
?.overwrt:		True
?.quit:			True

! application widget resources **********************************************

! some nice settings for the splash screen
?.splash_popup*shadowThickness: 1
?.splash_popup.borderWidth: 0

! keyboard focus policy (default: XmEXPLICIT); set this resource to XmPOINTER
! to disable keyboard navigation
?.keyboardFocusPolicy: XmEXPLICIT

! drag and drop protocols
*dragInitiatorProtocolStyle: DRAG_PREFER_DYNAMIC
*dragReceiverProtocolStyle: DRAG_PREFER_DYNAMIC

! force standard Motif button assignments (drag on Btn2, everything else
! currently DOES NOT WORK with xplore)
?.enableBtn1Transfer: XmOFF

! dialog decorations (26 = no resize handles; see VendorShell(3) for a
! description of possible values)
*XmDialogShell.mwmDecorations: 26

! disable keyboard traversal of scrollbars, sashes and the status line
*XmScrollBar.traversalOn: False
*XmSash.traversalOn: False
*status_line.traversalOn: False

! mouse wheel translations (assumes Z axis mapping to buttons 4 and 5)
*VertScrollBar.baseTranslations: #augment\n\
<Btn4Down>: IncrementUpOrLeft(Up)\n\
<Btn5Down>: IncrementDownOrRight(Down)\n
*HorScrollBar.baseTranslations: #augment\n\
<Btn4Down>: IncrementUpOrLeft(Left)\n\
<Btn5Down>: IncrementDownOrRight(Right)\n
*XmScrolledWindow*Text.baseTranslations: #augment\n\
<Btn4Down>: scroll-one-line-down()\n\
<Btn5Down>: scroll-one-line-up()\n
*XmScrolledWindow*log_scroll.baseTranslations: #augment\n\
<Btn4Down>: scroll-one-line-down()\n\
<Btn5Down>: scroll-one-line-up()\n

! enable tear off in menus (lets you use the menus like toolbars)
*menubar*XmRowColumn.tearOffModel: XmTEAR_OFF_ENABLED

! reduce margin height of the menubar (Motif default is 3)
*menubar.marginHeight: 1

! layout of the panes
*work_area.orientation: XmVERTICAL
*shelf_scrollSW.positionIndex: 0
*file_view.positionIndex: 1
*log_scrollSW.positionIndex: 2
*file_view.orientation: XmHORIZONTAL
*tree_scrollSW.positionIndex: 0
*file_scrollSW.positionIndex: 1

*XmPanedWindow.marginWidth: 0
*XmPanedWindow.marginHeight: 0

*XmContainer.marginWidth: 3
*XmContainer.marginHeight: 3

*XmText.marginWidth: 3
*XmText.marginHeight: 3

! tree container selection policy -- XmSINGLE_SELECT (default) or
! XmBROWSE_SELECT (see the XmContainer manpage for details about this)
*tree_scroll.selectionPolicy: XmSINGLE_SELECT

! file and shelf container selection policies -- either XmEXTENDED_SELECT
! (default) or XmMULTIPLE_SELECT
*file_scroll_large_icons.selectionPolicy: XmEXTENDED_SELECT
*file_scroll_small_icons.selectionPolicy: XmEXTENDED_SELECT
*file_scroll_detail.selectionPolicy: XmEXTENDED_SELECT
*shelf_scroll.selectionPolicy: XmEXTENDED_SELECT

! selection technique for file and shelf container -- valid values are:
! XmMARQUEE, XmMARQUEE_EXTEND_START, XmMARQUEE_EXTEND_BOTH, XmTOUCH_ONLY and
! XmTOUCH_OVER (default)
*file_scroll_large_icons.selectionTechnique: XmTOUCH_OVER
*file_scroll_small_icons.selectionTechnique: XmTOUCH_OVER
*file_scroll_detail.selectionTechnique: XmTOUCH_OVER
*shelf_scroll.selectionTechnique: XmTOUCH_OVER

! appearance of the shelf tabs (location: 0 = top, 1 = bottom, 2 = left,
! 3 = right; shape: 0 = chamferred, 1 = slanted)
*shelf_tabs.tabLocation: 0
*shelf_tabs.tabShape: 1

! take away extra horizontal space around icon gadgets (Motif 2.1???)
*XmIconGadget.marginHeight: 0

! file view frame
*frame.shadowType: XmSHADOW_OUT
*frame.shadowThickness: 1

! default and minimum sizes of the panes
*shelf_scrollSW.width: 620
*shelf_scrollSW.height: 64
*file_view.height: 386
*tree_scrollSW.width: 250
*tree_scrollSW.height: 380
*file_scrollSW.width: 350
*file_scrollSW.height: 380
*log_scrollSW.width: 620
*log_scrollSW.height: 64

*shelf_scrollSW*paneMinimum: 64
*file_view*paneMinimum: 100
*file_scrollSW*paneMinimum: 100
*tree_scrollSW*paneMinimum: 100
*log_scrollSW*paneMinimum: 64

! status line
*message_frame.shadowType: XmSHADOW_IN
*message_frame.shadowThickness: 1
*indicators_frame.shadowType: XmSHADOW_IN
*indicators_frame.shadowThickness: 1
*indicators.height: 16
*indicators.marginWidth: 2

! tree container
*tree_scroll.outlineIndentation: 20
*tree_scroll.OutlineButton.shadowThickness: 0

! file container
!*file_scroll_large_icons.XmIconGadget.borderWidth: 5
*file_scroll_large_icons.largeCellWidth: 64
!*file_scroll_small_icons.XmIconGadget.borderWidth: 5
*file_scroll_small_icons.smallCellWidth: 32
*file_scroll_detail.detailTabList: +2cm,+2cm,+2cm,+3cm,+6cm
! uncomment this if you want to have column headings in the detail view
!*file_scroll_detail.detailColumnHeadingCount: 6
!*file_scroll_detail.detailColumnHeading: Name,Size,Owner,Group,Perms,Date

! shelf container
!*shelf_scroll.file_icon.borderWidth: 5
*shelf_scroll.largeCellWidth: 64

! log
*log_scroll.editMode: XmMULTI_LINE_EDIT
*log_scrollSW.HorScrollBar.increment: 10
! no horizontal scrollbar, use word wrap instead
*log_scroll.scrollHorizontal: False
*log_scroll.wordWrap: True

! scroll bar spacing
*XmScrolledWindow*spacing: 0

! if you have troubles using the autoscroll drag feature, try increasing the
! margin widths of the panes (default: 0)
!*scrolledWindowMarginWidth: 2
!*scrolledWindowMarginHeight: 2

! labels for dummy entries
*unreadable.labelString: <DIRECTORY IS UNREADABLE>
*subdir.labelString: <SUBDIR>
*empty.labelString: 

! make the dummy entries insensitive s.t. they cannot be selected or activated
*unreadable.sensitive: False
*subdir.sensitive: False
*empty.sensitive: False

! dialog resources
*Line.Label.alignment: XmALIGNMENT_END
*Line.Input.width: 300
*Line.Input.leftOffset: 0.5cm
*Line.Browse.leftOffset: 0.5cm
*Line*Text.editMode: XmMULTI_LINE_EDIT
*Line*Text.width: 300
*Line*Text.rows: 4
*Line*TextSW.leftOffset: 0.5cm
*Line*TextSW.HorScrollBar.increment: 10
*Line.Value.width: 300
*Line.Value.leftOffset: 0.5cm
*Line.Value.alignment: XmALIGNMENT_BEGINNING
*Line.Button.leftOffset: 0.5cm
*Line.ComboBox.width: 300
*Line.ComboBox.leftOffset: 0.5cm
*Line.ComboBox*List.visibleItemCount: 10
!*XmFileSelectionBox*XmList.listSizePolicy: XmCONSTANT

! colors

! (see the schemes subdirectory in the xplore library directory
! for some nice color schemes)

!*foreground: black
*background: #c5c2c5
!*highlightColor: black
*highlightThickness: 1

! dialog input fields
*XmList.background: white
*XmText.background: white
*XmTextField.background: white

! containers
!*XmContainer.background: white
!*XmContainer*XmIconGadget.background: white

! shelf tab colors
!*shelf_tabs.tabColor: #c5c2c5
!*shelf_tabs.selectedTabColor: white

! icon gadget text colors

*file_icon.foreground: black
*file_link_icon.foreground: maroon
*file_link_bad_icon.foreground: dark slate grey
*dir_icon.foreground: black
*dir_up_icon.foreground: black
*dir_link_icon.foreground: maroon
*exec_icon.foreground: black
*exec_link_icon.foreground: maroon

! you can also specify different colors for the individual panes like this:
!*shelf_scroll.XmIconGadget.foreground: black

! highlight premounted devices in the status line
*status_line*indicators.highlightColor: maroon

! fonts

! menubar
*menubar*fontList:			-adobe-helvetica-medium-r-*-*-12-*
*menubar*XmRowColumn*fontList:		-adobe-helvetica-medium-r-*-*-12-*
! these are used when a menu is torn off
?.TransientShell.XmRowColumn*fontList:	-adobe-helvetica-medium-r-*-*-12-*

! shelf tabs
*shelf_tabs.fontList:			-adobe-helvetica-bold-r-*-*-12-*

! popups
*XmContainer*file_popup*fontList:	-adobe-helvetica-medium-r-*-*-12-*
*log_scroll*log_popup*fontList:		-adobe-helvetica-medium-r-*-*-12-*

! dialogs
!*XmMessageBox*Message*fontList:	-adobe-helvetica-medium-r-*-*-12-*
*XmMessageBox*Label*fontList:		-adobe-helvetica-bold-r-*-*-12-*
*XmMessageBox*Line*Label*fontList:	-adobe-helvetica-medium-r-*-*-12-*
*XmMessageBox*Line*Input*fontList:	-adobe-helvetica-medium-r-*-*-12-*
*XmMessageBox*Line*Text*fontList:	-adobe-helvetica-medium-r-*-*-12-*
*XmMessageBox*Line*Value*fontList:	-adobe-helvetica-medium-r-*-*-12-*
*XmMessageBox*Line*Button*fontList:	-adobe-helvetica-medium-r-*-*-12-*
*XmMessageBox*fontList:			-adobe-helvetica-medium-r-*-*-12-*
*XmPushButtonGadget*fontList:		-adobe-helvetica-medium-r-*-*-12-*

*param_dlg*Label*fontList:		-adobe-helvetica-medium-r-*-*-12-*
*param_dlg*Line*Label*fontList:		-adobe-helvetica-medium-r-*-*-12-*
*param_dlg*Line*Input*fontList:		-adobe-helvetica-medium-r-*-*-12-*
*param_dlg*Line*Value*fontList:		-adobe-helvetica-medium-r-*-*-12-*

*file_selection_dlg*Label*fontList:	-adobe-helvetica-medium-r-*-*-12-*
*file_selection_dlg*XmLabelGadget*fontList: \
					-adobe-helvetica-medium-r-*-*-12-*
*file_selection_dlg*XmTextField*fontList: \
					-adobe-helvetica-medium-r-*-*-12-*
*file_selection_dlg*XmList*fontList:	-adobe-helvetica-medium-r-*-*-12-*

! panes
*log_scroll*fontList:			fixed
*XmText*fontList:			-adobe-helvetica-medium-r-*-*-12-*
*XmContainer*fontList:			-adobe-helvetica-medium-r-*-*-10-*
*XmIconHeader*fontList:			-adobe-helvetica-medium-r-*-*-10-*

! status line
*status_line*XmLabelGadget*fontList:	-adobe-helvetica-medium-r-*-*-10-*
*status_line*XmIconGadget*fontList:	-adobe-helvetica-medium-r-*-*-10-*

! dialog titles

*confirm_drop_dlg_popup.title:		Confirm
*confirm_move_dlg_popup.title:		Confirm
*confirm_copy_dlg_popup.title:		Confirm
*confirm_link_dlg_popup.title:		Confirm
*confirm_delete_dlg_popup.title:	Confirm
*confirm_overwrite_dir_dlg_popup.title:	Confirm
*confirm_overwrite_file_dlg_popup.title:Confirm
*confirm_rename_dir_dlg_popup.title:	Confirm
*confirm_rename_file_dlg_popup.title:	Confirm
*confirm_delete_dir_dlg_popup.title:	Confirm
*confirm_delete_file_dlg_popup.title:	Confirm
*confirm_quit_dlg_popup.title:		Confirm
*confirm_nomatch_type_dlg_popup.title:	Confirm
*confirm_delete_type_dlg_popup.title:	Confirm
*scheme_dlg_popup.title:		Color scheme
*mount_dlg_popup.title:			Mount
*umount_dlg_popup.title:		Unmount
*mkdir_dlg_popup.title:			Make directory
*chdir_dlg_popup.title:			Change directory
*rename_dlg_popup.title:		Rename
*move_dlg_popup.title:			Move
*copy_dlg_popup.title:			Copy
*link_dlg_popup.title:			Link
*props_dlg_popup.title:			Properties
*bind_dlg_popup.title:			File type
*select_dlg_popup.title:		Select
*filter_dlg_popup.title:		Filter
*param_dlg_popup.title:			Command
*about_dlg_popup.title:			About xplore
*setup_dlg_popup.title:			Xplore setup
*error_dlg_popup.title:			Error
*warn_dlg_popup.title:			Warning

! titles and accelerators for the menus

! file pulldown
*FilePulldown.tearOffTitle: File
*FilePulldown*Open.accelerator: Ctrl<Key>O
*FilePulldown*Open.acceleratorText: Ctrl+O
*FilePulldown*Rename.accelerator: Ctrl<Key>N
*FilePulldown*Rename.acceleratorText: Ctrl+N
*FilePulldown*Move.accelerator: Ctrl<Key>M
*FilePulldown*Move.acceleratorText: Ctrl+M
*FilePulldown*Copy.accelerator: Ctrl<Key>C
*FilePulldown*Copy.acceleratorText: Ctrl+C
*FilePulldown*Link.accelerator: Ctrl<Key>L
*FilePulldown*Link.acceleratorText: Ctrl+L
*FilePulldown*Delete.accelerator: Ctrl<Key>D
*FilePulldown*Delete.acceleratorText: Ctrl+D
*FilePulldown*Properties.accelerator: Ctrl<Key>P
*FilePulldown*Properties.acceleratorText: Ctrl+P
*FilePulldown*File_type.accelerator: Ctrl<Key>T
*FilePulldown*File_type.acceleratorText: Ctrl+T
*FilePulldown*Select.accelerator: Ctrl<Key>S
*FilePulldown*Select.acceleratorText: Ctrl+S
*FilePulldown*Select_all.accelerator: Ctrl<Key>A
*FilePulldown*Select_all.acceleratorText: Ctrl+A
*FilePulldown*Invert_all.accelerator: Ctrl<Key>V
*FilePulldown*Invert_all.acceleratorText: Ctrl+V
*FilePulldown*Filter.accelerator: Ctrl<Key>F
*FilePulldown*Filter.acceleratorText: Ctrl+F
*FilePulldown*Quit.accelerator: Ctrl<Key>Q
*FilePulldown*Quit.acceleratorText: Ctrl+Q

! directory pulldown
*DirectoryPulldown.tearOffTitle: Directory
*DirectoryPulldown*Make_directory.accelerator: Ctrl<Key>K
*DirectoryPulldown*Make_directory.acceleratorText: Ctrl+K
*DirectoryPulldown*Change_directory.accelerator: <Key>F5
*DirectoryPulldown*Change_directory.acceleratorText: F5
*DirectoryPulldown*Parent.accelerator: Ctrl<Key>F5
*DirectoryPulldown*Parent.acceleratorText: Ctrl+F5
*DirectoryPulldown*Home.accelerator: Shift<Key>F5
*DirectoryPulldown*Home.acceleratorText: Shift+F5

! shelf pulldown
*ShelfPulldown.tearOffTitle: Shelf
*ShelfPulldown*Open.accelerator: Shift Ctrl<Key>O
*ShelfPulldown*Open.acceleratorText: Shift+Ctrl+O
*ShelfPulldown*Rename.accelerator: Shift Ctrl<Key>N
*ShelfPulldown*Rename.acceleratorText: Shift+Ctrl+N
*ShelfPulldown*Move.accelerator: Shift Ctrl<Key>M
*ShelfPulldown*Move.acceleratorText: Shift+Ctrl+M
*ShelfPulldown*Copy.accelerator: Shift Ctrl<Key>C
*ShelfPulldown*Copy.acceleratorText: Shift+Ctrl+C
*ShelfPulldown*Link.accelerator: Shift Ctrl<Key>L
*ShelfPulldown*Link.acceleratorText: Shift+Ctrl+L
*ShelfPulldown*Delete.accelerator: Shift Ctrl<Key>D
*ShelfPulldown*Delete.acceleratorText: Shift+Ctrl+D
*ShelfPulldown*Properties.accelerator: Shift Ctrl<Key>P
*ShelfPulldown*Properties.acceleratorText: Shift+Ctrl+P
*ShelfPulldown*File_type.accelerator: Shift Ctrl<Key>T
*ShelfPulldown*File_type.acceleratorText: Shift+Ctrl+T
*ShelfPulldown*Select.accelerator: Shift Ctrl<Key>S
*ShelfPulldown*Select.acceleratorText: Shift+Ctrl+S
*ShelfPulldown*Select_all.accelerator: Shift Ctrl<Key>A
*ShelfPulldown*Select_all.acceleratorText: Shift+Ctrl+A
*ShelfPulldown*Invert_all.accelerator: Shift Ctrl<Key>V
*ShelfPulldown*Invert_all.acceleratorText: Shift+Ctrl+V

! options pulldown
*OptionsPulldown.tearOffTitle: Options
*OptionsPulldown*Update.accelerator: Ctrl<Key>U
*OptionsPulldown*Update.acceleratorText: Ctrl+U
*OptionsPulldown*Reread.accelerator: Ctrl<Key>R
*OptionsPulldown*Reread.acceleratorText: Ctrl+R
*OptionsPulldown*Unmount.accelerator: Ctrl<Key>G
*OptionsPulldown*Unmount.acceleratorText: Ctrl+G
*OptionsPulldown*Unmount_all.accelerator: Shift Ctrl<Key>G
*OptionsPulldown*Unmount_all.acceleratorText: Shift+Ctrl+G
*OptionsPulldown*Save_defaults.accelerator: Alt<Key>S
*OptionsPulldown*Save_defaults.acceleratorText: Alt+S
*OptionsPulldown*Save_session.accelerator: Shift Alt<Key>S
*OptionsPulldown*Save_session.acceleratorText: Shift+Alt+S
*OptionsPulldown*Clear_session.accelerator: Shift Alt<Key>C
*OptionsPulldown*Clear_session.acceleratorText: Shift+Alt+C
*OptionsPulldown*Reload_configuration_file.accelerator: Alt<Key>R
*OptionsPulldown*Reload_configuration_file.acceleratorText: Alt+R

! view pulldown
*ViewPulldown.tearOffTitle: View
*ViewPulldown*Large_icons.accelerator: Ctrl<Key>F1
*ViewPulldown*Large_icons.acceleratorText: Ctrl+F1
*ViewPulldown*Small_icons.accelerator: Ctrl<Key>F2
*ViewPulldown*Small_icons.acceleratorText: Ctrl+F2
*ViewPulldown*Detail.accelerator: Ctrl<Key>F3
*ViewPulldown*Detail.acceleratorText: Ctrl+F3
*ViewPulldown*Shelf_pane.accelerator: Shift<Key>F1
*ViewPulldown*Shelf_pane.acceleratorText: Shift+F1
*ViewPulldown*Tree_pane.accelerator: Shift<Key>F2
*ViewPulldown*Tree_pane.acceleratorText: Shift+F2
*ViewPulldown*File_pane.accelerator: Shift<Key>F3
*ViewPulldown*File_pane.acceleratorText: Shift+F3
*ViewPulldown*Log_pane.accelerator: Shift<Key>F4
*ViewPulldown*Log_pane.acceleratorText: Shift+F4
*ViewPulldown*Show_hidden_files.accelerator: Ctrl<Key>H
*ViewPulldown*Show_hidden_files.acceleratorText: Ctrl+H

! preferences pulldown
*PreferencesPulldown.tearOffTitle: Preferences

! help pulldown
*HelpPulldown.tearOffTitle: Help
*HelpPulldown*Xplore_manual.accelerator: <Key>F1
*HelpPulldown*Xplore_manual.acceleratorText: F1

! command pulldown
*CommandPulldown.tearOffTitle: Command

! Some convenient accelerators for the Command menu ... Of course these
! depend on the contents of the command menu (Command section in the
! xplorerc file).

*CommandPulldown.Bookmark.accelerator: Alt<Key>B
*CommandPulldown.Bookmark.acceleratorText: Alt+B
*CommandPulldown.Program_shortcut.accelerator: Alt<Key>P
*CommandPulldown.Program_shortcut.acceleratorText: Alt+P
*CommandPulldown.URL_shortcut.accelerator: Alt<Key>U
*CommandPulldown.URL_shortcut.acceleratorText: Alt+U
*CommandPulldown.Editor.accelerator: Alt<Key>E
*CommandPulldown.Editor.acceleratorText: Alt+E
*CommandPulldown.Calculator.accelerator: Alt<Key>C
*CommandPulldown.Calculator.acceleratorText: Alt+C
*CommandPulldown.Manual.accelerator: Alt<Key>M
*CommandPulldown.Manual.acceleratorText: Alt+M
*CommandPulldown.Terminal.accelerator: Alt<Key>T
*CommandPulldown.Terminal.acceleratorText: Alt+T
*CommandPulldown.Xplore.accelerator: Alt<Key>F10
*CommandPulldown.Xplore.acceleratorText: Alt+F10
*CommandPulldown.Find.accelerator: Alt<Key>F
*CommandPulldown.Find.acceleratorText: Alt+F
*CommandPulldown.Grep.accelerator: Alt<Key>G
*CommandPulldown.Grep.acceleratorText: Alt+G
*CommandPulldown.Diff.accelerator: Alt<Key>D
*CommandPulldown.Diff.acceleratorText: Alt+D
*CommandPulldown.Shell_command.accelerator: <Key>F8
*CommandPulldown.Shell_command.acceleratorText: F8
*CommandPulldown.Make.accelerator: <Key>F9
*CommandPulldown.Make.acceleratorText: F9

! dummy button for empty command menu
*CommandPulldown.Empty.sensitive: False

! you can also have buttons with icons like this:
!*CommandPulldown.Xterm.labelType: XmPIXMAP
!*CommandPulldown.Xterm.labelPixmap: @XPLORELIBDIR@/icons/term.xpm
