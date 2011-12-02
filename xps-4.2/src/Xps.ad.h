"*border.width:               3  ",
"*background:                 #DA7",
"*foreground:                 Black",
"*normal.width:               600",
"*normal.height:              400",
"*zoomed.width:               1100",
"*zoomed.height:              500",
"*normal*geometry:            750x400+130-65",
"*zoomed*geometry:            940x600+20-10",
"*.mainWindow.process_menu_popup.*.PsInfo.width: 8000",
"*.mainWindow.process_menu_popup.*.PsInfo.height: 1000",
"*.mainWindow.process_menu_popup.*.PsInfo.unitType: Xm100TH_FONT_UNITS",
"*canvas.translations:  #override\\n\
     <Btn1Down>:  showprocess()\\n\
     <Btn2Down>:  showprocess()\\n\
     <Btn3Down>:  showprocess(Kill)\\n\
     <Key>space:  showprocess()\\n\
     <Key>Enter:  showprocess()\\n\
     <Key>Return: showprocess()",
"*mainWindow*input*translations:  #override",
"*mainWindow*color_input*translations:  #override",
"*mainWindow*user_input*translations:  #override",
"*mainWindow*translations:  #override\\n\
     <Key>a: toggleprocs()\\n\
     <Key>b: togglebeep()\\n\
     <Key>-: changespeed(-1)\\n\
    :<Key>f: changespeed(-1)\\n\
    :<Key>F: changespeed(-5)\\n\
    Ctrl<Key>k: togglekernel()\\n\
    :<Key>K: togglekill()\\n\
     <Key>+: changespeed(1)\\n\
    :<Key>s: changespeed(1)\\n\
    :<Key>S: changespeed(5)\\n\
     <Key>r: redisplay()\\n\
     Ctrl<Key>X: quit()\\n\
    :<Key>z: zoom()",
"*RefreshRate*mnemonic:		I",
"*RefreshRate*accelerator:	<Key>I",
"*RefreshRate*acceleratorText:	I",
"*RefreshRate*labelString:	Refresh Interval...",
"*ToggleBeep*mnemonic:		B",
"*Kill*mnemonic:			K",
"*Kill*accelerator:		<Key>K",
"*Kill*acceleratorText:		K",
"*Kill*labelString:		Set Quick Kill Signal...",
"*SetUser*mnemonic:		U",
"*SetUser*accelerator:		<Key>U",
"*SetUser*acceleratorText:	U",
"*SetUser*labelString:		User to watch...",
"*UserColor*mnemonic:		C",
"*UserColor*accelerator:		<Key>C",
"*UserColor*acceleratorText:	C",
"*UserColor*labelString:		Set user Color...",
"*Debugging*mnemonic:		D",
"*Debugging*accelerator:		<Key>D",
"*Debugging*acceleratorText:	D",
"*Debugging*labelString:		Debugging...",
"*PsCmdline*mnemonic:		P",
"*PsCmdline*accelerator:		<Key>P",
"*PsCmdline*acceleratorText:	P",
"*PsCmdline*labelString:		PS Command...",
"*Zoom*acclerator:		<Key>Z",
"*Zoom*accleratorText:		Z",
"*Quit*mnemonic:			Q",
"*Quit*accelerator:		Ctrl<Key>X",
"*Quit*acceleratorText:		Ctrl+X",
"*Quit*labelString:		Quit",
"*Config*mnemonic:		V",
"*Config*accelerator:		<Key>V",
"*Config*acceleratorText:	V",
"*Config*labelString:		View Configuration",
"*NoWarranty*mnemonic:		W",
"*NoWarranty*accelerator:	<Key>W",
"*NoWarranty*acceleratorText:	W",
"*NoWarranty*labelString:	No Warranty",
"*waiting:                    cyan",
"*running:                    green",
"*active:                     white",
"*selected:                   cyan",
"*idle:                       orange",
"*zombie:                     yellow",
"*stopped:                    red",
"*aixterm:                    #668",
"*newproc:                    white",
"*uid1:                       0",
"*usercolor1:                 black",
"*uid2:                       715",
"*usercolor2:                 #22a",
"*normal.font:                -*-lucida-*-r-normal-*-12-*-*-*-p-*-iso8859-1",
"*zoomed.font:                -*-lucida-*-r-normal-*-14-*-*-*-p-*-iso8859-1",
"*.mainWindow.process_menu_popup.*.fontList: \
	-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"*.mainWindow.process_menu_popup.*.PsInfo.fontList: \
	-*-*-Medium-R-Normal--*-120-*-*-m-*-ISO8859-1",
"*.mainWindow.*.form.*.fontList: \
	-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"*.popup_Commands.labelFontList: \
	-*-*-Medium-R-Normal--*-120-*-*-m-*-ISO8859-1",
".Xps.*.popup_Commands.Commands.*.fontList: \
	-*-*-Medium-R-Normal--*-120-*-*-m-*-ISO8859-1",
".Xps.*.popup_Commands.popup_Help.Help.Key/Mouse actions.*.Message.fontList: \
	-*-*-Medium-R-Normal--*-120-*-*-m-*-ISO8859-1",
".Xps.*.popup_Commands.popup_Help.Help.Key/Mouse binding.*.Message.fontList: \
	-*-*-Medium-R-Normal--*-120-*-*-m-*-ISO8859-1",
"*.user_name.labelString:  User name:",
"*.color_name.labelString: Color:",
"*.okLabelString: OK",
"*.cancelLabelString: Cancel",
"*.helpLabelString: Help",
"*.debug.labelString:  Debug value:",
"*traversalOn:	TRUE",
"*keyboardFocusPolicy: explicit",
"*highlightOnEnter: TRUE",
"*highlightThickness: 1",
"*quickkill: False",
"*allprocs:  False",
"*kernel:    False",
"*beep:      False",
"*beepup:		     \"55 50 110 50 220 50 440 100\"",
"*beepdown:		    \"440 50 220 50 110 50 55 100\"",
