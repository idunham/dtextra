/*
 * $Header: /cvsroot-fuse/meditres/meditres/appres.c,v 1.5 2001/12/23 16:51:47 dannybackx Exp $
 */
char *fallback[] = {
	"meditres*flashColor:			Yellow",
#if 0
	"*Background:			RoyalBlue4",
	"*Foreground:			Azure",
	"*BorderColor:			LightCyan1",

	"*Tree.Background: 		Grey",
	"*Tree.Foreground: 		White",
#endif
	"meditres.geometry:			520x700",
	"*flash.BorderWidth:			0",
/*
 * Menu Button Names
 */
	"meditres.mw.menubar*fileCascade.labelString:		Tree",
	"meditres.mw.menubar*fileCascade.mnemonic:			T",
	"meditres.mw.menubar*resourceCascade.labelString:		Resource",
	"meditres.mw.menubar*resourceCascade.mnemonic:		R",
	"meditres.mw.menubar*findCascade.labelString:		Find",
	"meditres.mw.menubar*selectCascade.labelString:		Select",
	"meditres.mw.menubar*viewCascade.labelString:		View",
	"meditres.mw.menubar*optionsCascade.labelString:		Options",
	"meditres.mw.menubar*foldCascade.labelString:		Folding",
	"meditres.mw.menubar*helpCascade.labelString:		Help",
/*
 * Resource Box
 */
	"meditres.mw.menubar*ok.labelString:			OK",
	"meditres.mw.menubar*get.labelString:			Get",
	"meditres.mw.menubar*apply.labelString:			Apply",
	"meditres.mw.menubar*cancel.labelString:			Dismiss",
/*
 *
 * For the Top Area
 *
 * statusBox.Box.orientation:		horizontal
 * statusBox.Box.borderWidth:		0
 */
	"*statusBox*numWidLabel.label:		#",
	"*statusBox*shownWidLabel.label:		shown",
	"*statusBox*selWidLabel.label:		sel",
	"*statusBox*waitLabel.label:		wait",
	"*statusBox*zoomLabel.label:		zoom",
	"*statusBox*work.label:			??progress??",
/*
 * For the Panner inside the Form
 */
	"*treeform.Panner.width:			200",
	"*treeform.Panner.height:		100",
	"*treeform.Panner.foreground:		red",
	"*treeform.Panner.background:		green",
	"",
	"*porthole.tree.width:			500",
	"*porthole.tree.height:			480",
	"*porthole.tree.autoReconfigure:		FALSE",
	"*hPane.min:				20",
	"*hPane.height:				100",
/*
 * Universal Menu Entry Names
 */
	"meditres.mw.menubar*getTree.labelString:			Get Tree",
	"meditres.mw.menubar*getTree.mnemonic:			G",
	"meditres.mw.menubar*getTree.accelerator:			Ctrl<Key>G",
	"meditres.mw.menubar*getTree.acceleratorText:		^G",
	"meditres.mw.menubar*dumpTreeToFile.labelString:		Dump to a File ...",
	"meditres.mw.menubar*printTree.labelString:		Print ...",
	"meditres.mw.menubar*printTree.accelerator:		Ctrl<Key>P",
	"meditres.mw.menubar*printTree.acceleratorText:		^P",
	"meditres.mw.menubar*quitButton.labelString:		Quit",
	"meditres.mw.menubar*quitButton.mnemonic:			Q",
	"meditres.mw.menubar*quitButton.accelerator:		Ctrl<Key>Q",
	"meditres.mw.menubar*quitButton.acceleratorText:		^Q",

	"meditres.mw.menubar*unselectAll.labelString:		Unselect All",
	"meditres.mw.menubar*invertAll.labelString:		Invert All",
	"meditres.mw.menubar*selectAll.labelString:		Select All",
	"meditres.mw.menubar*selectChildren.labelString:		Children",
	"meditres.mw.menubar*selectParent.labelString:		Parent",
	"meditres.mw.menubar*selectDescendants.labelString:	All Descendants",
	"meditres.mw.menubar*selectAncestors.labelString:		All Ancestors",

	"meditres.mw.menubar*viewView.labelString:		Folding Status",

	"meditres.mw.menubar*zoom.labelString:			Zoom ...",
	"meditres.mw.menubar*orientation.labelString:		Tree Orientation ...",
	"meditres.mw.menubar*scrolling.labelString:		Scrolling ...",
	"meditres.mw.menubar*prefs.labelString:			Preferences ...",
	"meditres.mw.menubar*sysinfo.labelString:			System Info ...",

	"meditres.mw.menubar*findInTreeButton.labelString:		Search ...",

	"meditres.mw.menubar*foldSelected.label:		Fold",
	"meditres.mw.menubar*foldChildSelected.label:	Fold Subtree",
	"meditres.mw.menubar*unfold1Selected.label:	Unfold Children only",
	"meditres.mw.menubar*unfoldSelected.label:		Unfold",
	"meditres.mw.menubar*unfoldAll.label:		Unfold All",
	"meditres.mw.menubar*newTopSelected.label:		New Top",
	"meditres.mw.menubar*newTopParent.label:		Restore Parent",
	"meditres.mw.menubar*restoreRoot.label:		Restore Root",
	"meditres.mw.menubar*showAll.label:		Show All",
/*
 * Xt Menu Entry Names
 */
	"meditres.mw.menubar*refreshTree.labelString:		Refresh Current",

	"meditres.mw.menubar*resourceBoxButton.labelString:	Resource Box ...",
	"meditres.mw.menubar*resourceBoxButton.accelerator:	Ctrl<Key>R",
	"meditres.mw.menubar*resourceBoxButton.acceleratorText:	^R",
	"meditres.mw.menubar*resourceBoxButton.mnemonic:		B",
	"meditres.mw.menubar*setResourceButton.labelString:	Set Resource ...",

	"meditres.mw.menubar*selectWidgetButton.labelString:	Select Widget in Client",
	"meditres.mw.menubar*flashActiveButton.labelString:	Flash Active Widgets",

	"meditres.mw.menubar*viewWidgetNames.labelString:	Widget Names",
	"meditres.mw.menubar*viewClassNames.labelString:	Class Names",
	"meditres.mw.menubar*viewWidgetIDs.labelString:	Widget IDs",
	"meditres.mw.menubar*viewWidgetWindows.labelString:	Widget Windows",
/*
 * Universal Toolkit Messages
 */
	"*label0: Action `%s' must have the Editres Protocol.",
	"*label1: Action %s's argument must be either `%s' or `%s'.",
	"*label2: Action `%s' must have exactly one argument.",
	"*label3: Unknown parameter to action `%s' must be one of:\n ",
	"*label4: It appears that this client does not understand\n the Editres Protocol.",
	"*label5: Can't grab the mouse.\n ",
	"*label6: Click the mouse pointer on any toolkit client.",
	"*label8: Message sent to client%s.",
	"*label9: Incorrectly formatted message from client.",
	"*label10: Unable to own the Resource Editor Command Selection",
	"*label11: Unknown Error code %d",
	"*label13: Welcome to the X Resource Editor version 1.1",
	"*label15: That window does not appear to be\n in the currently displayed client.",
	"*label16: Editres Internal Error: Unable to find node.\n ",
	"*label18: There are no active nodes.",
	"*label20: ApplyResource: found no matches for `%s%s:%s'.",
	"*label21: Error: SVActiveEntry Action must have exactly one argument.",
	"*label22: Error: SVActiveEntry Action's first Argument must be either 'Resource' or 'Value'.",
	"*label24: Unable to open the file `%s' for writing.",
	"*label25: Error while trying to save Context\n Aborting file dialog popup.",
	"*label26: Error while trying to find Context\n Aborting...",
	"*label28: Loop in tree: node %s's parent (%s) has not been created yet\n ",
	"*label30: There are no active nodes.",
	"*label31: Internal Error: Unknown select type.",
	"*label32: Internal Error: Unknown label type.",
	"*label33: Internal Error: Unknown activate type.",
	"*label34: Only one Resource Box can be active at a time.",
	"*label35: Unable to unpack protocol request.",
	"*label36: This version of editres uses protocol version %s,\n but the client speaks version %s.",
	"*label37: -unable to grab keyboard",
	"*label38: Unable to unpack protocol request.",
	"*label39: SetValues was Successful.",
	"*label40: Error while trying to save Context\nAborting file dialog popup.",
	"*label41: Error while trying to find Context\n Aborting...",
	"*label42: Warning: Widget %s contains illegal character '%c'",
	"*label43: Error: Search Action must have zero or one argument.",
	"*label44: Error: Search Actions's first Argument must be 'All', 'NameOrClass', 'Name', 'Class', 'AnyID', 'WinID' or 'WidID'",
	"*label45: Editres internal error: Illegal find type.",
	"*label46: Editres internal error: Parameter NULL.",
	"*label47: Could not load font '%s'.",
/*
 * Xt Messages
 */
	"*xt*label7: Unable to own the Resource Selection",
	"*xt*label12: Widget Tree for client %s(%s).",
	"*xt*label14: Click on any widget in the client.\n Editres will select that widget in the tree display.",
	"*xt*label17: No widget Tree is available.",
	"*xt*label19: This function requires exactly one (1) widget to be selected.",
	"*xt*label23: There are no currently active widgets.",
	"*xt*label27: There is no widget tree to display.",
	"*xt*label29: XSaveContext failed on widget %s.",

/*
 * For the list of messages
 */
	"*hpane*List.defaultColumns:	1",
	"*hpane*List.forceColumns:	TRUE",
/*
 * For the Filename popup dialog.
 */
	"*fileDialog*value:		",
	"*fileDialog*allowShellResize:		True",
/*
 * Universal Resources for the Resource Box.
 */
	"*resourceLabel.font:			-*-new century schoolbook-bold-r-*-*-12-*-*-*-*-*-*-*",
	"*allowShellResize:				True",
	"*svResourceLabel.allowResize:			True",
	"*List.verticalList:				True",
	"*namesAndClasses*BorderWidth:		0",
	"*star.labelString:				*",
	"*dot.labelString:					.",
	"*namesLabel.font:			-*-new century schoolbook-bold-r-*-*-10-*-*-*-*-*-*-*",
	"*constraintLabel.font:			-*-new century schoolbook-bold-r-*-*-10-*-*-*-*-*-*-*",
	"*svValueLabel.BorderWidth:			0",
	"*valueForm*preferredPaneSize:		100",
	"*valueText*editType:			edit",
	"*valueText.Scroll:			WhenNeeded",
	"*valueText.width:			300",
	"*setFile.labelString:				Set Save File",
	"*save.labelString:				Save",
	"*apply.labelString:				Apply",
	"*saveAndApply.labelString:			Save & Apply",
	"*commandBox.show.labelString:		        Show",
/*
 * Xt Resources for the Resource Box.
 */
	"*xt*single.labelString:                              Any Widget",
	"*xt*any.labelString:                                 Any Widget Chain",
	"*xt*namesLabel.labelString:                          Normal Resources: mb2 gets a value",
	"*xt*constraintLabel.labelString:                     Constraint Resources",
	"*xt*valueLabel.labelString:                          Enter Resource Value:",
	"*xt*commandBox.cancel.labelString:                   Close",

	"*commandBox.skipAdjust:				True",
/*
 * For the SetValues popup
 */
	"*svPopup*left:			ChainLeft",
	"*svPopup*right:			ChainLeft",

	"*SimpleMenu*svPopup.title:			Set Resource Values",
	"*SimpleMenu*svPopup*svLabel.labelString:		Use <Tab> to Change Fields,\n"
		"<Ctrl>q <Tab> to insert a <Tab>.",
	"*SimpleMenu*svPopup*svResourceLabel.labelString:	Resource Name:",
	"*SimpleMenu*svPopup*svValueLabel.labelString:		Value:",
	"*SimpleMenu*svPopup*svSetValues.labelString:		Apply",
	"*SimpleMenu*svPopup*svCancel.labelString:		Close",
/*
 * For the Find dialog
 */
	"*findPopup.title:			Find in Local Tree",
	"*findPopup*findLabel.labelString:		Search for:\n"
	"(unrealized: win 0,\n"
	"nonwindow object: win 2)",

	"*findPopup*findTypeAll.labelString:		All (Name, Class or ID)",
	"*findPopup*findTypeNameOrClass.labelString:	Names or Classes",
	"*findPopup*findTypeName.labelString:		Names",
	"*findPopup*findTypeClass.labelString:		Classes",
	"*findPopup*findTypeAnyID.labelString:		Any ID",
	"*findPopup*findTypeWinID.labelString:		Window ID",
	"*findPopup*findTypeWidID.labelString:		Widget ID",

	"*findPopup*searchNext.labelString:		Search",
	"*findPopup*findCancel.labelString:		Close",

/*
 * For the Print dialog
 *
 * the label can´t be set from resource files (at start time)
 * the action 'focus-out' does nothing
 */
	"*printTreeSh.pdialog.Label.label:	Filename:",

	"*saveResourceFile:	test2",
	(char *)0
};
