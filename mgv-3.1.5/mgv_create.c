/*
 * mgv_create.c
 *	Primary widget tree creator for mgv.
 *
 * Copyright (C) 1996  Eric A. Howe
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/mgv_create.c 1.168.10.1 1998/09/28 09:46:33 $")

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <Xm/ToggleB.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/icons.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/file.h>
#include <mine/events.h>
#include <mine/help.h>
#include <mine/log.h>
#include <mine/magstep.h>
#include <mine/media.h>
#include <mine/options.h>
#include <mine/orientation.h>
#include <mine/page.h>
#include <mine/mark.h>
#include <mine/print.h>
#include <mine/sens.h>
#include <mine/track.h>
#include <mine/view.h>
#include <mine/zoom.h>
#include <mine/auto.h>
#include <mine/center.h>
#include <mine/toolbar.h>

#if defined(ABS)
#	undef ABS
#endif
#define ABS(x) ((x) < 0 ? -(x) : (x))

/*
 * Do not try to use wlCascadeBG anywhere in here!  Trying to use
 * CascadeButtonGadgets will cause even more brain damage than the
 * Motif menu system gives all by itself.
 */

static WLW menu_file[] = {
	{"open",		wlPushBG,	0, NULL},
	{"reopen",		wlPushBG,	0, NULL},
	{"save",		wlPushBG,	0, NULL},
	{"print",		wlPushBG,	0, NULL},
	{"info",		wlPushBG,	0, NULL},
	{"sep",			wlSeparatorG,	0, NULL},
	{"close",		wlPushBG,	0, NULL},
	{"exit",		wlPushBG,	0, NULL},
	{NULL}
};

static WLW menu_size[] = {
	{"size",		wlToggleBG,	0, NULL},
	{"height",		wlToggleBG,	0, NULL},
	{"width",		wlToggleBG,	0, NULL},
	{"none",		wlToggleBG,	0, NULL},
	{NULL}
};

static WLW menu_page[] = {
	{"next",		wlPushBG,	0, NULL},
	{"prev",		wlPushBG,	0, NULL},
	{"sep1",		wlSeparatorG,	0, NULL},
	{"center",		wlToggleBG,	0, NULL},
	{"redisplay",		wlPushBG,	0, NULL},
	{"sep2",		wlSeparatorG,	0, NULL},
	{"showLabels",		wlToggleBG,	0, NULL},
	{"showPgNums",		wlToggleBG,	0, NULL},
	{"sep3",		wlSeparatorG,	0, NULL},
	{"size",		wlCascadeB,	0, menu_size},
	{NULL}
};

static WLW menu_mark[] = {
	{"mark",		wlPushBG,	0, NULL},
	{"markAll",		wlPushBG,	0, NULL},
	{"markNone",		wlPushBG,	0, NULL},
	{"even",		wlPushBG,	0, NULL},
	{"odd",			wlPushBG,	0, NULL},
	{"invert",		wlPushBG,	0, NULL},
	{NULL}
};

static WLW menu_magstep[] = {
	{"decrease",		wlPushBG,	0, NULL},
	{"-5",			wlToggleBG,	0, NULL},
	{"-4",			wlToggleBG,	0, NULL},
	{"-3",			wlToggleBG,	0, NULL},
	{"-2",			wlToggleBG,	0, NULL},
	{"-1",			wlToggleBG,	0, NULL},
	{"0",			wlToggleBG,	0, NULL},
	{"1",			wlToggleBG,	0, NULL},
	{"2",			wlToggleBG,	0, NULL},
	{"3",			wlToggleBG,	0, NULL},
	{"4",			wlToggleBG,	0, NULL},
	{"5",			wlToggleBG,	0, NULL},
	{"increase",		wlPushBG,	0, NULL},
	{"sep",			wlSeparatorG,	0, NULL},
	{"set",			wlPushBG,	0, NULL},
	{NULL}
};

static WLW menu_orientation[] = {
	{"portrait",		wlToggleBG,	0, NULL},
	{"landscape",		wlToggleBG,	0, NULL},
	{"upsideDown",		wlToggleBG,	0, NULL},
	{"seascape",		wlToggleBG,	0, NULL},
	{NULL}
};

static WLW menu_media[] = {
	{"letter",		wlToggleBG,	0, NULL},
	{"lettersmall",		wlToggleBG,	0, NULL},
	{"tabloid",		wlToggleBG,	0, NULL},
	{"ledger",		wlToggleBG,	0, NULL},
	{"legal",		wlToggleBG,	0, NULL},
	{"statement",		wlToggleBG,	0, NULL},
	{"executive",		wlToggleBG,	0, NULL},
	{"a3",			wlToggleBG,	0, NULL},
	{"a4",			wlToggleBG,	0, NULL},
	{"a4small",		wlToggleBG,	0, NULL},
	{"a5",			wlToggleBG,	0, NULL},
	{"b4",			wlToggleBG,	0, NULL},
	{"b5",			wlToggleBG,	0, NULL},
	{"folio",		wlToggleBG,	0, NULL},
	{"quarto",		wlToggleBG,	0, NULL},
	{"10x14",		wlToggleBG,	0, NULL},
	{NULL}
};

static WLW menu_palette[] = {
	{"monochrome",		wlToggleBG,	0, NULL},
	{"grayscale",		wlToggleBG,	0, NULL},
	{"color",		wlToggleBG,	0, NULL},
	{NULL},
};

static WLW menu_options[] = {
	{"antiAlias",		wlToggleBG,	0, NULL},
	{"magicScroll",		wlToggleBG,	0, NULL},
	{"smoothScroll",	wlToggleBG,	0, NULL},
	{"smartSizing",		wlToggleBG,	0, NULL},
	{"palette",		wlCascadeB,	0, menu_palette},
	{NULL}
};

/* same as the popup menu */
static WLW menu_view[] = {
	{"hideLog",		wlToggleBG,	0, NULL},
	{"hideList",		wlToggleBG,	0, NULL},
	{"hideStatus",		wlToggleBG,	0, NULL},
	{"hideMenu",		wlToggleBG,	0, NULL},
	{"hideToolBar",		wlToggleBG,	0, NULL},
	{"sep",			wlSeparatorG,	0, NULL},
	{"closeZoom",		wlPushBG,	0, NULL},
	{NULL}
};

static WLW menu_help[] = {
	{"contents",		wlPushBG,	0, NULL},
	{"context",		wlPushBG,	0, NULL},
	{"appres",		wlPushBG,	0, NULL},
	{"cmdline",		wlPushBG,	0, NULL},
	{"sep",			wlSeparatorG,	0, NULL},
	{"about",		wlPushBG,	0, NULL},
	{NULL}
};

static WLW menu_bar[] = {
	{"file",		wlCascadeB,	0, menu_file},
	{"page",		wlCascadeB,	0, menu_page},
	{"view",		wlCascadeB,	0, menu_view},
	{"mark",		wlCascadeB,	0, menu_mark},
	{"magstep",		wlCascadeB,	0, menu_magstep},
	{"orientation",		wlCascadeB,	0, menu_orientation},
	{"media",		wlCascadeB,	0, menu_media},
	{"options",		wlCascadeB,	0, menu_options},
	{"help",		wlCascadeB,	0, menu_help},
	{NULL}
};

static WLW view_window[] = {
	{"mgv",			"Ghostview",	0, NULL},
	{NULL}
};

static WLW menu_zoom[] = {
	{"15",			wlPushBG,	0, NULL},
	{"20",			wlPushBG,	0, NULL},
	{"30",			wlPushBG,	0, NULL},
	{"40",			wlPushBG,	0, NULL},
	{"50",			wlPushBG,	0, NULL},
	{NULL},
};

static WLW magstep_form[] = {
	{"value",		wlPushB,	0,		NULL},
	{"text",		wlTextField,	WL_UNMANAGE,	NULL},
	{"label",		wlLabel,	0,		NULL},
	{NULL}
};
static WLW magstep_frame[] = {
	{"form",		wlForm,		WL_MANAGE, magstep_form},
	{NULL}
};

static WLW name_frame[] = {
	{"name",		wlLabelG,	0, NULL},
	{NULL},
};

static WLW date_frame[] = {
	{"date",		wlLabelG,	0, NULL},
	{NULL},
};

static WLW tracker_frame[] = {
	{"tracker",		wlLabelG,	0, NULL},
	{NULL},
};

static WLW status_form[] = {
	{"magstepFrame",	wlFrame,	0, magstep_frame},
	{"nameFrame",		wlFrame,	0, name_frame},
	{"dateFrame",		wlFrame,	0, date_frame},
	{"trackerFrame",	wlFrame,	0, tracker_frame},
	{NULL}
};

static WLW tool_bar[] = {
	{"fileOpen",	wlPushB,	0,	NULL},
	{"filePrint",	wlPushB,	0,	NULL},

	{"mark",	wlPushB,	0,	NULL},
	{"markAll",	wlPushB,	0,	NULL},
	{"markNone",	wlPushB,	0,	NULL},
	{"markEven",	wlPushB,	0,	NULL},
	{"markOdd",	wlPushB,	0,	NULL},

	{"magstepIncr",	wlPushB,	0,	NULL},
	{"magstepDecr",	wlPushB,	0,	NULL},

	{"helpContext",	wlPushB,	0,	NULL},

	{"pagePrev",	wlPushB,	0,	NULL},
	{"pageNext",	wlPushB,	0,	NULL},

	{NULL}
};

static WLW main_window[] = {
	{"menuBar",	wlMenuBar,		WL_MANAGE,	menu_bar},
	{"toolBar",	"WlToolBar",		WL_MANAGE,	tool_bar},
	{"statusLine",	wlForm,			WL_MANAGE,	status_form},
	{"list",	wlScrolledList,		WL_MANAGE,	NULL},
	{"view",	wlScrolledW,		0,		view_window},
	{"zoomMenu",	wlPopupMenu,		0,		menu_zoom},
	{NULL}
};

static WLW main_widget[] = {
	{"mainWindow",		wlForm,		0, main_window},
	{NULL}
};

/*
 * Almost the same as menu_view[].
 */
static WLW menu_popup_kids[] = {
	{"back",		wlPushBG,	0, NULL},
	{"forward",		wlPushBG,	0, NULL},
	{"sep1",		wlSeparatorG,	0, NULL},
	{"hideLog",		wlToggleBG,	0, NULL},
	{"hideList",		wlToggleBG,	0, NULL},
	{"hideStatus",		wlToggleBG,	0, NULL},
	{"hideMenu",		wlToggleBG,	0, NULL},
	{"hideToolBar",		wlToggleBG,	0, NULL},
	{"sep2",		wlSeparatorG,	0, NULL},
	{"closeZoom",		wlPushBG,	0, NULL},
	{NULL}
};

static WLW menu_popup[] = {
	{"popup",		wlPopupMenu,	0, menu_popup_kids},
	{NULL}
};

static WLC cb[] = {
	{"*menuBar*file_pd*open", XmNactivateCallback,
		NULL, mgv_file_open},
	{"*toolBar*fileOpen", XmNactivateCallback,
		NULL, mgv_file_open},
	{"*menuBar*file_pd*reopen", XmNactivateCallback,
		NULL, mgv_file_reopen},
	{"*menuBar*file_pd*save", XmNactivateCallback,
		NULL, mgv_file_save},
	{"*menuBar*file_pd*print", XmNactivateCallback,
		NULL, mgv_print},
	{"*toolBar*filePrint", XmNactivateCallback,
		NULL, mgv_print},
	{"*menuBar*file_pd*info", XmNactivateCallback,
		NULL, mgv_file_info},
	{"*menuBar*file_pd*close", XmNactivateCallback,
		NULL, mgv_file_close},
	{"*menuBar*file_pd*exit", XmNactivateCallback,
		NULL, mgv_app_exit},

	{"*menuBar*page_pd*next", XmNactivateCallback,
		NULL, mgv_page_next},
	{"*toolBar*pageNext", XmNactivateCallback,
		NULL, mgv_page_next},
	{"*menuBar*page_pd*prev", XmNactivateCallback,
		NULL, mgv_page_prev},
	{"*toolBar*pagePrev", XmNactivateCallback,
		NULL, mgv_page_prev},
	{"*menuBar*page_pd*center", XmNvalueChangedCallback,
		NULL, mgv_page_center},
	{"*menuBar*page_pd*redisplay", XmNactivateCallback,
		NULL, mgv_page_redisplay},
	{"*menuBar*page_pd*showLabels", XmNvalueChangedCallback,
		NULL, mgv_page_labels},
	{"*menuBar*page_pd*showPgNums", XmNvalueChangedCallback,
		NULL, mgv_page_labels},

	{"*menuBar*size_pd*size", XmNvalueChangedCallback,
		NULL, mgv_page_size},
	{"*menuBar*size_pd*height", XmNvalueChangedCallback,
		NULL, mgv_page_size},
	{"*menuBar*size_pd*width", XmNvalueChangedCallback,
		NULL, mgv_page_size},
	{"*menuBar*size_pd*none", XmNvalueChangedCallback,
		NULL, mgv_page_size},

	{"*menuBar*mark_pd*mark", XmNactivateCallback,
		NULL, mgv_mark_mark},
	{"*toolBar*mark", XmNactivateCallback,
		NULL, mgv_mark_mark},
	{"*menuBar*mark_pd*markAll", XmNactivateCallback,
		NULL, mgv_mark_markall},
	{"*toolBar*markAll", XmNactivateCallback,
		NULL, mgv_mark_markall},
	{"*menuBar*mark_pd*markNone", XmNactivateCallback,
		NULL, mgv_mark_unmarkall},
	{"*toolBar*markNone", XmNactivateCallback,
		NULL, mgv_mark_unmarkall},
	{"*menuBar*mark_pd*even", XmNactivateCallback,
		NULL, mgv_mark_even},
	{"*toolBar*markEven", XmNactivateCallback,
		NULL, mgv_mark_even},
	{"*menuBar*mark_pd*odd", XmNactivateCallback,
		NULL, mgv_mark_odd},
	{"*toolBar*markOdd", XmNactivateCallback,
		NULL, mgv_mark_odd},
	{"*menuBar*mark_pd*invert", XmNactivateCallback,
		NULL, mgv_mark_invert},

	{"*menuBar*magstep_pd*decrease", XmNactivateCallback,
		NULL, mgv_magstep_decrease},
	{"*toolBar*magstepDecr", XmNactivateCallback,
		NULL, mgv_magstep_decrease},
	{"*menuBar*magstep_pd*-5", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*-4", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*-3", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*-2", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*-1", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*0", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*1", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*2", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*3", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*4", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*5", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*increase", XmNactivateCallback,
		NULL, mgv_magstep_increase},
	{"*toolBar*magstepIncr", XmNactivateCallback,
		NULL, mgv_magstep_increase},
	{"*menuBar*magstep_pd*set", XmNactivateCallback,
		NULL, mgv_magstep_set},

	{"*menuBar*orientation_pd*portrait", XmNvalueChangedCallback,
		NULL, mgv_orientation},
	{"*menuBar*orientation_pd*landscape", XmNvalueChangedCallback,
		NULL, mgv_orientation},
	{"*menuBar*orientation_pd*upsideDown", XmNvalueChangedCallback,
		NULL, mgv_orientation},
	{"*menuBar*orientation_pd*seascape", XmNvalueChangedCallback,
		NULL, mgv_orientation},

	{"*menuBar*media_pd*letter", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*lettersmall", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*tabloid", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*ledger", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*legal", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*statement", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*executive", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a3", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a4", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a4small", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a5", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*b4", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*b5", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*folio", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*quarto", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*10x14", XmNvalueChangedCallback,
		NULL, mgv_media_cb},

	{"*menuBar*options_pd*antiAlias", XmNvalueChangedCallback,
		NULL, mgv_options_antialias},
	{"*menuBar*options_pd*magicScroll", XmNvalueChangedCallback,
		NULL, mgv_options_bits},
	{"*menuBar*options_pd*smoothScroll", XmNvalueChangedCallback,
		NULL, mgv_options_bits},
	{"*menuBar*options_pd*smartSizing", XmNvalueChangedCallback,
		NULL, mgv_options_bits},
	{"*menuBar*palette_pd*monochrome", XmNvalueChangedCallback,
		NULL, mgv_options_palette},
	{"*menuBar*palette_pd*grayscale", XmNvalueChangedCallback,
		NULL, mgv_options_palette},
	{"*menuBar*palette_pd*color", XmNvalueChangedCallback,
		NULL, mgv_options_palette},

	{"*menuBar*help_pd*contents", XmNactivateCallback,
		NULL, wl_help},
	{"*menuBar*help_pd*context", XmNactivateCallback,
		NULL, wl_help_context},
	{"*toolBar*helpContext", XmNactivateCallback,
		NULL, wl_help_context},
	{"*menuBar*help_pd*appres", XmNactivateCallback,
		NULL, wl_help},
	{"*menuBar*help_pd*cmdline", XmNactivateCallback,
		NULL, wl_help},
	{"*menuBar*help_pd*about", XmNactivateCallback,
		NULL, mgv_help_about},

	/*
	 * The view and popup menus attach callbacks to each other
	 * for synchronization; the closure argument should be the
	 * _other_ widget name (with path).
	 */

	{"*menuBar*view_pd*hideLog", XmNvalueChangedCallback,
		NULL, mgv_view_log},
	{"*menuBar*view_pd*hideLog", XmNvalueChangedCallback,
		"*popup*hideLog", wl_toggle_sync},
	{"*menuBar*view_pd*hideList", XmNvalueChangedCallback,
		NULL, mgv_view_page},
	{"*menuBar*view_pd*hideList", XmNvalueChangedCallback,
		"*popup*hideList", wl_toggle_sync},
	{"*menuBar*view_pd*hideStatus", XmNvalueChangedCallback,
		NULL, mgv_view_status},
	{"*menuBar*view_pd*hideStatus", XmNvalueChangedCallback,
		"*popup*hideStatus", wl_toggle_sync},
	{"*menuBar*view_pd*hideMenu", XmNvalueChangedCallback,
		NULL, mgv_view_menu},
	{"*menuBar*view_pd*hideMenu", XmNvalueChangedCallback,
		"*popup*hideMenu", wl_toggle_sync},
	{"*menuBar*view_pd*hideToolBar", XmNvalueChangedCallback,
		NULL, mgv_view_toolbar},
	{"*menuBar*view_pd*hideToolBar", XmNvalueChangedCallback,
		"*popup*hideToolBar", wl_toggle_sync},
	{"*menuBar*view_pd*closeZoom", XmNactivateCallback,
		NULL, mgv_view_zoom},

	{"*popup*back", XmNactivateCallback,
		NULL, mgv_page_backforward},
	{"*popup*forward", XmNactivateCallback,
		NULL, mgv_page_backforward},
	{"*popup*hideLog", XmNvalueChangedCallback,
		NULL, mgv_view_log},
	{"*popup*hideLog", XmNvalueChangedCallback,
		"*menuBar*view_pd*hideLog", wl_toggle_sync},
	{"*popup*hideList", XmNvalueChangedCallback,
		NULL, mgv_view_page},
	{"*popup*hideList", XmNvalueChangedCallback,
		"*menuBar*view_pd*hideList", wl_toggle_sync},
	{"*popup*hideStatus", XmNvalueChangedCallback,
		NULL, mgv_view_status},
	{"*popup*hideStatus", XmNvalueChangedCallback,
		"*menuBar*view_pd*hideStatus", wl_toggle_sync},
	{"*popup*hideMenu", XmNvalueChangedCallback,
		NULL, mgv_view_menu},
	{"*popup*hideMenu", XmNvalueChangedCallback,
		"*menuBar*view_pd*hideMenu", wl_toggle_sync},
	{"*popup*hideToolBar", XmNvalueChangedCallback,
		NULL, mgv_view_toolbar},
	{"*popup*hideToolBar", XmNvalueChangedCallback,
		"*menuBar*view_pd*hideToolBar", wl_toggle_sync},
	{"*popup*closeZoom", XmNactivateCallback,
		NULL, mgv_view_zoom},

	{"*view*mgv", GvNoutputCallback,	NULL, mgv_log_output},
	{"*view*mgv", GvNmessageCallback,	NULL, mgv_log_message},
	{"*view*mgv", GvNnotifyCallback,	NULL, mgv_track},

	{"*list", XmNbrowseSelectionCallback,	NULL, mgv_page_goto},

	{"*zoomMenu*15", XmNactivateCallback,	NULL, mgv_zoom},
	{"*zoomMenu*20", XmNactivateCallback,	NULL, mgv_zoom},
	{"*zoomMenu*30", XmNactivateCallback,	NULL, mgv_zoom},
	{"*zoomMenu*40", XmNactivateCallback,	NULL, mgv_zoom},
	{"*zoomMenu*50", XmNactivateCallback,	NULL, mgv_zoom},
	{"*zoomMenu",    XmNunmapCallback,	NULL, mgv_zoom_unmap},

	{"*mainWindow", XmNhelpCallback,	NULL, wl_help},

	{"*statusLine*magstepFrame*value", XmNactivateCallback,
		NULL, mgv_magstep_edit},
	{"*statusLine*magstepFrame*text", XmNactivateCallback,
		NULL, mgv_magstep_text},
	{"*statusLine*magstepFrame*text", XmNlosingFocusCallback,
		NULL, mgv_magstep_text},

	{NULL}
};

static MGVSD sens[] = {
	{"*menuBar*file_pd*reopen",	MgvSOPEN | MgvSCANREOPEN},
	{"*menuBar*file_pd*print",	MgvSOPEN},
	{"*toolBar*filePrint",		MgvSOPEN},
	{"*menuBar*file_pd*save",	MgvSOPEN},
	{"*menuBar*file_pd*info",	MgvSOPENDOC},
	{"*fileSave*saveUnmark",	MgvSMARKED},
	{"*fileSave*saveMarked",	MgvSMARKED},
	{"*menuBar*page_pd*prev",	MgvSOPENDOC | MgvSNOTFIRST},
	{"*toolBar*pagePrev",		MgvSOPENDOC | MgvSNOTFIRST},
	{"*menuBar*page_pd*next",	MgvSOPENDOC | MgvSNOTLAST,
					MgvSOPEN    | MgvSNOTLAST},
	{"*toolBar*pageNext",		MgvSOPENDOC | MgvSNOTLAST,
					MgvSOPEN    | MgvSNOTLAST},
	{"*menuBar*page_pd*center",	MgvSOPEN},
	{"*menuBar*page_pd*redisplay",	MgvSOPENDOC},
	{"*menuBar*page_pd*showLabels",	MgvSOPENDOC},
	{"*menuBar*page_pd*showPgNums",	MgvSOPENDOC},
	{"*menuBar*mark",		MgvSOPENDOC},
	{"*menuBar*mark_pd*mark",	MgvSOPENDOC | MgvSPAGES},
	{"*toolBar*mark",		MgvSOPENDOC | MgvSPAGES},
	{"*menuBar*mark_pd*markAll",	MgvSOPENDOC | MgvSPAGES},
	{"*toolBar*markAll",		MgvSOPENDOC | MgvSPAGES},
	{"*menuBar*mark_pd*markNone",	MgvSOPENDOC | MgvSPAGES | MgvSMARKED},
	{"*toolBar*markNone",		MgvSOPENDOC | MgvSPAGES | MgvSMARKED},
	{"*menuBar*mark_pd*even",	MgvSOPENDOC | MgvSPAGES},
	{"*toolBar*markEven",		MgvSOPENDOC | MgvSPAGES},
	{"*menuBar*mark_pd*odd",	MgvSOPENDOC | MgvSPAGES},
	{"*toolBar*markOdd",		MgvSOPENDOC | MgvSPAGES},
	{"*menuBar*mark_pd*invert",	MgvSOPENDOC | MgvSPAGES},
	{"*menuBar*magstep",		MgvSOPEN},
	{"*toolBar*magstepIncr",	MgvSOPEN},
	{"*toolBar*magstepDecr",	MgvSOPEN},
	{"*menuBar*orientation",	MgvSOPEN},
	{"*menuBar*media",		MgvSOPEN},
	{"*printDialog*markedOnly",	MgvSMARKED},
	{"*printDialog*unmark",		MgvSMARKED},
	{"*listSW", 			MgvSPAGES},
	{"*menuBar*view_pd*closeZoom",	MgvSGOTZOOMS},
	{"*popup*back",			MgvSCANBACK    | MgvSOPENDOC},
	{"*popup*forward",		MgvSCANFORWARD | MgvSOPENDOC},
	{"*popup*closeZoom",		MgvSGOTZOOMS},
};
#define	N_SENS	(sizeof(sens)/sizeof(sens[0]))

/*
 * initialize various toggles
 */
#define	TOGGLE(top,name,bit) \
	if(bit) XmToggleButtonSetState(wl_find1(top, name), True, True);
#define	TOG_PGLIST	(1 << 0)
#define	TOG_MENU	(1 << 1)
#define	TOG_STAT	(1 << 2)
#define	TOG_MAGIC	(1 << 3)
#define	TOG_LOG		(1 << 4)
#define	TOG_SMOOTH	(1 << 5)
#define	TOG_TOOLBAR	(1 << 6)
static void
frobnicate(Widget mainwin, int mask)
{
	Widget	top  = wl_top(mainwin);

	assert(XtWindow(wl_find1(mainwin, "*menuBar")) != None);
	TOGGLE(top, "*popup*hideLog",		mask & TOG_LOG);
	TOGGLE(top, "*popup*hideList",		mask & TOG_PGLIST);
	TOGGLE(top, "*popup*hideMenu",		mask & TOG_MENU);
	TOGGLE(top, "*popup*hideStatus",	mask & TOG_STAT);
	TOGGLE(top, "*popup*hideToolBar",	mask & TOG_TOOLBAR);
	TOGGLE(top, "*options_pd*magicScroll",	mask & TOG_MAGIC);
	TOGGLE(top, "*options_pd*smoothScroll",	mask & TOG_SMOOTH);
}
#undef HIDE

MGV *
mgv_create(Widget parent, char *file)
{
	Widget	help, orienttog;
	MGV_APP	*r;
	MGV	*m;
	Atom	deletewin;
	int	toggle, status;
	char	*sizer;
	Screen	*screen = DefaultScreenOfDisplay(XtDisplay(parent));
	GvPageOrientation orient = GvPageOrientationPortrait;

	r = mgv_appres(parent);
	if((m = mgv_alloc(NULL)) == NULL)
		return NULL;
	m->type = MgvTFULL;

	/*
	 * store the relevant application resources and other low
	 * level things
	 */
	toggle = 0;
	m->zoom_linewidth   = r->reticule_width;
	m->default_media    = r->default_page_media;
	m->parent           = NULL;
	m->labels           = r->show_labels;
	m->pgnums           = r->show_pgnums;
	m->smoothness       = r->smoothness;
	m->magic_resistance = r->magic_resistance;
	m->scrollfactor     = r->scroll_percentage / 1000.0;
	m->bits            |= r->reset_scroll;
	if(r->antialias_eps)	m->bits |= MgvALIASEPS;
	if(r->smart_sizing)	m->bits |= MgvSMARTSIZE;
	if(r->magic_scroll)	m->bits |= MgvMAGICSCROLL, toggle |= TOG_MAGIC;
	if(r->hide_log)		m->bits |= MgvHIDELOG,     toggle |= TOG_LOG;
	if(r->smooth_scroll)	m->bits |= MgvSMOOTH,      toggle |= TOG_SMOOTH;
	if(r->hide_pglist)	toggle |= TOG_PGLIST;
	if(r->hide_menu)	toggle |= TOG_MENU;
	if(r->hide_status)	toggle |= TOG_STAT;
	if(r->hide_toolbar)	toggle |= TOG_TOOLBAR;

	if(!m->labels && !m->pgnums)
		toggle |= TOG_PGLIST;

	/*
	 * build the widget tree
	 */
	m->main  = wl_create(parent, main_widget);
	m->popup = wl_create(m->main, menu_popup);
	if(mgv_app_add(m) == NULL) {
		mgv_free(m);
		return NULL;
	}
	wl_callback(parent, &cb[0], (XtPointer)m);
	XtVaSetValues(m->main, XmNuserData, (XtPointer)m, NULL);
	wl_find(m->main,
		&m->sw,		"view",
		&m->ghost,	"view*mgv",
		&m->pagelist,	"*list",
		&m->zoom_menu,	"*zoomMenu",
		&m->tracker,	"*trackerFrame*tracker",
		&m->menubar,	"menuBar",
		&m->toolbar,	"toolBar",
		&m->statusline,	"statusLine",
		&help,		"menuBar.help",
		NULL);
	XtVaGetValues(m->sw,      XmNclipWindow,     &m->clip,          NULL);
	XtVaSetValues(m->menubar, XmNmenuHelpWidget, help,              NULL);
	XtAddCallback(parent, XmNdestroyCallback, mgv_app_close, (XtPointer)m);

	mgv_toolbar_init(m->toolbar);

	/*
	 * defaults
	 */
	m->defalias     = GhostviewGetAntiAlias(m->ghost);
	m->orient       = GhostviewGetOrientation(m->ghost);
	m->default_xdpi = GhostviewGetXDPI(m->ghost);
	m->default_ydpi = GhostviewGetYDPI(m->ghost);

	XtSetSensitive(m->zoom_menu, False);

	m->root_width  = WidthOfScreen(screen)  - r->wm_width;
	m->root_height = HeightOfScreen(screen) - r->wm_height;

	assert(MgvOK(m));

	switch(orient) {
	case GvPageOrientationPortrait:
		orienttog = NULL;
		break;
	case GvPageOrientationLandscape:
		orienttog = wl_find1(m->main, "*orientation_pd*landscape");
		break;
	case GvPageOrientationSeascape:
		orienttog = wl_find1(m->main, "*orientation_pd*seascape");
		break;
	case GvPageOrientationUpsideDown:
		orienttog = wl_find1(m->main, "*orientation_pd*upsideDown");
		break;
	default:
		/* not possible */
		assert("Unknown orientation!" != NULL);
		orienttog = NULL;
		break;
	}
	if(orienttog != NULL)
		XmToggleButtonSetState(orienttog, True, True);

	XtVaSetValues(parent, XmNdeleteResponse, XmDO_NOTHING, NULL);
	deletewin = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
	XmAddWMProtocolCallback(parent, deletewin, mgv_file_close,(XtPointer)m);

	/*
	 * open file
	 */
	status = 0;
	if(file != NULL)
		status = mgv_open(m, file);
	else if(!isatty(fileno(stdin)))
		status = mgv_open_fp(m, stdin);
	else
		mgv_close(m, FALSE, FALSE);

	mgv_ui_magnify(m, r->magstep);

	m->senscache = mgv_sens_create(wl_top(m->main), sens, N_SENS);
	mgv_sens(m->senscache, m->sens);

	XmToggleButtonSetState(wl_find1(parent,"*menuBar*page_pd*showLabels"),
							m->labels, False);
	XmToggleButtonSetState(wl_find1(parent,"*menuBar*page_pd*showPgNums"),
							m->pgnums, False);
	wl_icons(parent, &m->icon, &m->mask);
	XtRealizeWidget(parent);
	XSync(XtDisplay(parent), False);
	frobnicate(m->main, toggle);

	/*
	 * And now we can take care of the auto-size requests.
	 * We can't let the toggle button set m->autosizer for us
	 * since the initial mgv_show() probably hasn't been called
	 * yet.
	 */
	switch((r->auto_width == True) + 2*(r->auto_height == True)) {
	case 0:
		sizer        = "*menuBar*size_pd*none";
		m->autosizer = mgv_auto_none;
		break;
	case 1:
		sizer        = "*menuBar*size_pd*width";
		m->autosizer = mgv_auto_width;
		break;
	case 2:
		sizer        = "*menuBar*size_pd*height";
		m->autosizer = mgv_auto_height;
		break;
	case 3:
		sizer        = "*menuBar*size_pd*size";
		m->autosizer = mgv_auto_size;
		break;
	default:
		assert("busted compiler!" != NULL);
		sizer = NULL;
		break;
	}
	XmToggleButtonSetState(wl_find1(m->main, sizer), True, False);
	XmToggleButtonSetState(wl_find1(m->main, "*options_pd*smartSizing"),
				(m->bits & MgvSMARTSIZE) ? True : False, False);

	/*
	 * Similarily for the page|center things.
	 */
	if(r->center) {
		m->center = mgv_center_center;
		XmToggleButtonSetState(wl_find1(m->main, "*page_pd*center"),
				True, False);
	}
	else {
		m->center = mgv_center_noop;
		XmToggleButtonSetState(wl_find1(m->main, "*page_pd*center"),
				False, False);
	}

	XtAddEventHandler(m->main, ButtonPressMask, False, mgv_events,
							(XtPointer)m);

	if(status != 0) {
		wl_warn(m->main, "%s: %s", file, strerror(status));
		mgv_close(m, FALSE, FALSE);
	}

	return m;
}
