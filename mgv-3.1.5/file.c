/*
 * file.c
 *	File menu callbacks for mgv.
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
MU_ID("$Mu: mgv/file.c 1.104 1998/09/01 20:49:38 $")

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <Xm/ToggleB.h>
#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/MessageB.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/typesanity.h>
#include <wlib/WlAppShell.h>
#include <mine/mgv.h>
#include <mine/file.h>
#include <mine/sens.h>
#include <mine/strings.h>
#include <mine/uiutil.h>
#include <mine/page_control.h>
#include <mine/app.h>
#include <mine/process.h>
#include <mine/util.h>

/*
 * file->open->ok
 */
static void
file_open_ok(Widget w, XtPointer closure, XtPointer call)
{
	XmFSS	*cbs = (XmFSS *)call;
	MGV	*m   = (MGV *)closure;
	char	*file;
	Widget	top;
	int	status;
	USEUP(w);
	assert(MgvOK(m));

	if(!XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &file))
		return;
	if(XmToggleButtonGetState(wl_find1(w, "*openNew"))) {
		top = XtAppCreateShell(NULL, "MGv", wlAppShellWidgetClass,
							XtDisplay(w), NULL, 0);
		if(top == NULL) {
			XtFree((XtPointer)file);
			return;
		}
		if((m = mgv_create(top, file)) == NULL) {
			XtFree((XtPointer)file);
			XtDestroyWidget(top);
			return;
		}
	}
	else if((status = mgv_open(m, file)) != 0) {
		wl_error(w, "%s: %s", file, strerror(status));
		XtFree((XtPointer)file);
		return;
	}
	XtFree((XtPointer)file);
	if(!XmToggleButtonGetState(wl_find1(w, "*openBrowse")))
		XtUnmanageChild(w);
	if(m->docinfo != NULL)
		XtUnmanageChild(m->docinfo);
	mgv_pgstack_goto(m->pgstack, 0);
}

static WLW open_kids[] = {
	{"openBrowse",	wlToggleBG,	0, NULL},
	{"openNew",	wlToggleBG,	0, NULL},
	{NULL}
};
static WLW open_form[] = {
	{"openForm",	wlForm,		0, open_kids},
	{NULL},
};
static WLW file_open[] = {
	{"fileOpen",	wlFileSBDialog,	0, open_form},
	{NULL}
};
static WLC fo_cb[] = {
	{"*fileOpen",	XmNcancelCallback,	NULL, wl_dismiss},
	{"*fileOpen",	XmNokCallback,		NULL, file_open_ok},
	{NULL}
};

/*
 * file->open
 */
void
mgv_file_open(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	if(m->fs_open == NULL) {
		if((m->fs_open = wl_create(m->main, file_open)) == NULL)
			return;
		wl_callback(XtParent(m->fs_open), &fo_cb[0], (XtPointer)m);
	}
	else {
		XmFileSelectionDoSearch(m->fs_open, NULL);
	}
	wl_dlg_popup(m->fs_open);
}

/*
 * file->reopen
 */
void
mgv_file_reopen(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	int	status, page;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	page = m->page;
	if((status = mgv_open(m, NULL)) != 0) {
		wl_error(m->main, "%s: %s", m->file, strerror(status));
		return;
	}
	if(m->docinfo != NULL)
		XtUnmanageChild(m->docinfo);
	mgv_pgstack_goto(m->pgstack, page);
}

static void
comp_err(XtPointer closure, int *fd, XtInputId *id)
{
	MGV *m = (MGV *)closure;
	assert(MgvOK(m));

	if(m->child_err == NULL)
		m->child_err = XmCreateInformationDialog(m->main, "childStderr",
								NULL, 0);
	if(!mgv_child_msg(m->child_err, *fd)) {
		XtRemoveInput(*id);
		close(*fd);
	}
}

static FILE *
comp(MGV *m, char *compress, char *filename)
{
	PIPEKID pk;
	char    *argv[4];
	char    *cmd;

	/*
	 * Doing it all by hand is too much bother when the shell already
	 * knows what to do.  Praise the unix approach!
	 */
	cmd = XtCalloc(strlen(compress) + 3 + strlen(filename) + 1, 1);
	sprintf(cmd, "%s > %s", compress, filename);
	argv[0] = "sh";
	argv[1] = "-c";
	argv[2] = cmd;
	argv[3] = NULL;

	memset((void *)&pk, '\0', sizeof(pk));
	pk.flags = PK_IN | PK_ERR;
	if(mgv_popen(&pk, *argv, argv) != 0) {
		XtFree(cmd);
		return NULL;
	}
	XtFree(cmd);

	XtAppAddInput(XtWidgetToApplicationContext(m->main),
					pk.err, (XtPointer)XtInputReadMask,
					comp_err, (XtPointer)m);

	return fdopen(pk.in, "w");
}

static void
file_save_write(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	MGV_APP *r;
	FILE	*fp;
	Boolean	unmark, marked;
	int	i;
	Widget	sm, su;
	char	*p;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	assert(m->savefile != NULL);

	wl_find(m->fs_save,
		&sm,	"*saveMarked",
		&su,	"*saveUnmark",
		NULL);

	/*
	 * saveMarked and saveUnmark should always have the same
	 * sensitivity
	 */
	assert(XtIsSensitive(su) == XtIsSensitive(sm));
	marked = unmark = False;
	if(XtIsSensitive(sm)) {
		marked = XmToggleButtonGetState(sm);
		unmark = XmToggleButtonGetState(su);
	}

	fp = NULL;
	r = mgv_appres(m->main);
	if((p = strrchr(m->savefile, '.')) != NULL) {
		if(strcmp(p, ".gz") == 0)
			fp = comp(m, r->gzip, m->savefile);
		else if(strcmp(p, ".Z") == 0)
			fp = comp(m, r->compress, m->savefile);
		else if(strcmp(p, ".bz2") == 0)
			fp = comp(m, r->bzip, m->savefile);
	}
	if(fp == NULL)
		fp = fopen(m->savefile, "w");

	if(fp == NULL) {
		wl_error(w, "%s %s: %s", mgv_s(w, MgvScantOpen),
					m->savefile, strerror(errno));
		XtFree((XtPointer)m->savefile);
		m->savefile = NULL;
		if(m->save_query != NULL)
			XtUnmanageChild(m->save_query);
		return;
	}

	XtFree((XtPointer)m->savefile);
	m->savefile = NULL;

	mgv_write(m, fp, marked);
	fclose(fp);

	if(unmark && m->dsc != NULL) {
		for(i = 0; i < m->dsc->n_pages; ++i)
			mgv_mark(m, i, FALSE, FALSE);
		mgv_list_update(m, -1);
	}

	wl_dlg_popdown(m->fs_save);
}

static WLW save_query[] = {
	{"existOverwrite",	wlQuestionDialog,	0,	NULL},
	{NULL}
};
static WLC save_query_cb[] = {
	{"*existOverwrite", XmNunmapCallback,	NULL, wl_dismiss},
	{"*existOverwrite", XmNcancelCallback,	NULL, wl_dismiss},
	{"*existOverwrite", XmNhelpCallback,	NULL, wl_help},
	{"*existOverwrite", XmNokCallback,	NULL, file_save_write},
	{NULL}
};

/*
 * file->save->ok
 */
static void
file_save_ok(Widget w, XtPointer closure, XtPointer call)
{
	XmFSS	*cbs = (XmFSS *)call;
	MGV	*m   = (MGV *)closure;
	char	*buf = NULL;
	USEUP(w);
	assert(MgvOK(m));

	if(m->save_query != NULL)
		XtUnmanageChild(m->save_query);
	XtFree((XtPointer)m->savefile);
	m->savefile = NULL;
	if(!XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &m->savefile))
		return;
	if(access(m->savefile, F_OK) != 0) {
		file_save_write(w, (XtPointer)m, NULL);
		return;
	}

	if(m->save_query == NULL) {
		m->save_query = wl_create(m->fs_save, save_query);
		wl_callback(XtParent(m->save_query), &save_query_cb[0],
								(XtPointer)m);
	}

	buf = XtMalloc(strlen(m->savefile) + 128);
	sprintf(buf, "%s\n%s", m->savefile, mgv_s(w, MgvSfileExists));
	XtVaSetValues(m->save_query,
		XtVaTypedArg,	XmNmessageString,
				XmRString, buf,
				strlen(buf) + 1,
		NULL);
	XtFree(buf);
	wl_dlg_popup(m->save_query);
}

/*
 * save file selector
 */
static WLW save_kids[] = {
	{"saveMarked",	wlToggleBG,	0, NULL},
	{"saveUnmark",	wlToggleBG,	0, NULL},
	{NULL}
};
static WLW save_form[] = {
	{"form",	wlForm,		0, save_kids},
	{NULL}
};
static WLW file_save[] = {
	{"fileSave",	wlFileSBDialog,	0, save_form},
	{NULL}
};
static WLC fs_cb[] = {
	{"*fileSave",	XmNcancelCallback,	NULL, wl_dismiss},
	{"*fileSave",	XmNokCallback,		NULL, file_save_ok},
	{NULL}
};

/*
 * file->save
 */
void
mgv_file_save(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	if(m->fs_save == NULL) {
		if((m->fs_save = wl_create(m->main, file_save)) == NULL)
			return;
		mgv_sens(m->senscache, m->sens);
		wl_callback(XtParent(m->fs_save), &fs_cb[0], (XtPointer)m);
	}
	else {
		XmFileSelectionDoSearch(m->fs_save, NULL);
	}
	wl_dlg_popup(m->fs_save);
}

/*
 * file->close, WM->delete
 */
void
mgv_file_close(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	XtUnmanageChild(m->main);
	XtDestroyWidget(wl_top(m->main));
}

/*
 * file->info
 */
static WLW info_rc[] = {
	{"titleLabel",	wlLabelG,	0, NULL},
	{"dateLabel",	wlLabelG,	0, NULL},
	{"forLabel",	wlLabelG,	0, NULL},
	{"creatorLabel",wlLabelG,	0, NULL},
	{"mediaLabel",	wlLabelG,	0, NULL},
	{"pagesLabel",	wlLabelG,	0, NULL},
	{"typeLabel",	wlLabelG,	0, NULL},
	{"title",	wlTextField,	0, NULL},
	{"date",	wlTextField,	0, NULL},
	{"for",		wlTextField,	0, NULL},
	{"creator",	wlTextField,	0, NULL},
	{"media",	wlTextField,	0, NULL},
	{"pages",	wlTextField,	0, NULL},
	{"type",	wlTextField,	0, NULL},
	{NULL}
};
static WLW info_kids[] = {
	{"contents",	wlRowColumn,	WL_MANAGE,	info_rc},
	{"dismiss",	wlPushB,	0,		NULL},
	{"help",	wlPushB,	0,		NULL},
	{"sep",		wlSeparatorG,	0,		NULL},
	{NULL}
};
static WLW info_dlg[] = {
	{"infoDialog",	wlFormDialog,	0,	info_kids},
	{NULL}
};
static WLC info_cb[] = {
	{"*dismiss",	XmNactivateCallback,	NULL,	wl_dismiss},
	{"*help",	XmNactivateCallback,	NULL,	wl_help},
	{NULL}
};

static void
info_set(Widget dlg, char *name, char *value)
{
	Widget text = wl_find1(dlg, name);
	XmTextFieldSetString(text, value != NULL ? value : "");
}

void
mgv_file_info(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m = (MGV *)closure;
	char	pages[64], *media, *type;
	USEUP(w); USEUP(call);
	assert(MgvOK(m));

	if(m->docinfo == NULL) {
		m->docinfo = wl_create(m->main, info_dlg);
		wl_callback(XtParent(m->docinfo), &info_cb[0], (XtPointer)m);
	}

	/*
	 * The document information can change whenever the file
	 * is scanned for DSC.  This can happen if we open a new
	 * file or re-open the current file.  Since I'm a little
	 * lazy, I'll just update the dialog every time and avoid
	 * some hassles.
	 */
	sprintf(pages, "%d", m->dsc->n_pages);

	switch(m->dsc->type) {
	case DSC_PS:	type = mgv_s(m->main, MgvSPS);		break;
	case DSC_EPS:	type = mgv_s(m->main, MgvSEPS);		break;
	case DSC_PDF:	type = mgv_s(m->main, MgvSPDF);		break;
	default:	type = mgv_s(m->main, MgvSunknownType);	break;
	}

	if(m->dsc->defmedia != NULL
	&& m->dsc->defmedia->name != NULL)
		media = m->dsc->defmedia->name;
	else
		media = mgv_s(m->main, MgvSunknownMedia);

	info_set(m->docinfo, "*title",   m->dsc->title);
	info_set(m->docinfo, "*date",    m->dsc->date);
	info_set(m->docinfo, "*for",     m->dsc->forwho);
	info_set(m->docinfo, "*creator", m->dsc->creator);
	info_set(m->docinfo, "*media",   media);
	info_set(m->docinfo, "*pages",   pages);
	info_set(m->docinfo, "*type",    type);

	wl_dlg_popup(m->docinfo);
}
