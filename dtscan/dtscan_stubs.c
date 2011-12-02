/* Copyright (c) 2011, mghis 
 * All rights reserved.
 *
 * This software is provided under the BSD License,
 * ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES.
 */

/*
 *LIBS: -lXm -lXt -lX11
 */

#include <stdlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>

#include "dtscan.h"


extern void XDmanage_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDunmanage_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDpopup_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDpopdown_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDmap_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDunmap_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDenable_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDdisable_link( Widget w, XtPointer client_data, XtPointer call_data);

#define LINEART 0
#define GRAY    1
#define COLOR   2

#define NONUM   0
#define ONE     1
#define TWO     2

int mode = LINEART;
int num  = NONUM;
int nn = 1;  		/* No. of page. */
char basename[100];


void quitCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmPushButtonCallbackStruct *call_data = (XmPushButtonCallbackStruct *) xt_call_data ;

	exit(0);
}


void scanCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmPushButtonCallbackStruct *call_data = (XmPushButtonCallbackStruct *) xt_call_data ;

	char *scanner;
	char *file = (char *)malloc(100);
	char *ffile; /* file from field */
	char *path;
	char *x;
	char *y;
	char *s_mode = (char *)malloc(100); /* the mode of scanner in string */

	char cmd[200];     /* the command that system(3) will execute */	

	scanner = (char *)XmTextGetString(scannerW);
	ffile = (char *)XmTextGetString(fileW);
	path = (char *)XmTextGetString(pathW);
	x = (char *)XmTextGetString(xW);
	y = (char *)XmTextGetString(yW);
	
	if (strlen(scanner) < 2 || strlen(file) < 2 || strlen(path) < 2) {
		XtVaSetValues(statusW,
				XmNlabelString, XmStringCreateLocalized("Missing field."),
				NULL);
		return;
	}

	/* parsing of mode */

	if (mode == LINEART)
		strcpy(s_mode, "lineart");
	if (mode == COLOR)
		strcpy(s_mode, "color");
	if (mode == GRAY)
		strcpy(s_mode, "gray");

	/* parsing of numbering */
	if (num == NONUM)
		strcpy(file, ffile);
	else if (num == ONE) {
		if (atoi(ffile) == 0) {
			strcpy(basename, ffile);
			strcpy(file, "1_");
			nn = 1;
			strcat(file, basename);
			XmTextSetString(fileW, file);
		} else {
			nn = atoi(ffile);
			nn++;
			char ns[50];
			snprintf(ns, 50, "%d_", nn);
			strcpy(file, ns);
			strcat(file, basename);
			XmTextSetString(fileW, file);
		}
	} else if (num == TWO) {
		if (atoi(ffile) == 0) {
			strcpy(basename, ffile);
			strcpy(file, "2_");
			nn = 2;
			strcat(file, basename);
			XmTextSetString(fileW, file);
		} else {
			nn = atoi(ffile);
			nn++; nn++;
			char ns[50];
			snprintf(ns, 50, "%d_", nn);
			strcpy(file, ns);
			strcat(file, basename);
			XmTextSetString(fileW, file);
		}
	}

	if (atoi(x) <= 0 && atoi(y) <= 0)
		snprintf(cmd, sizeof cmd,
				"scanimage -d %s --format=tiff --mode=%s > %s%s.tif\n",
				scanner, s_mode, path, file);
	else
		snprintf(cmd, sizeof cmd,
				"scanimage -d %s --format=tiff --mode=%s -x %d -y %d > %s%s.tif\n",
				scanner, s_mode, atoi(x), atoi(y), path, file);

	free(s_mode);


#ifdef DEBUG
	printf("CMD: %s\n", cmd);
#endif

	XtVaSetValues(statusW,
			XmNlabelString, XmStringCreateLocalized("Scanning..."),
			NULL);
	XmUpdateDisplay(shell);

	system(cmd);

	XtVaSetValues(statusW,
			XmNlabelString, XmStringCreateLocalized("Ready"),
			NULL);

	free(file);
}

void lineartCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmPushButtonCallbackStruct *call_data = (XmPushButtonCallbackStruct *) xt_call_data ;

	mode = LINEART;
}

void grayCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmPushButtonCallbackStruct *call_data = (XmPushButtonCallbackStruct *) xt_call_data ;

	mode = GRAY;
}

void colorCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmPushButtonCallbackStruct *call_data = (XmPushButtonCallbackStruct *) xt_call_data ;

	mode = COLOR;
}

void searchCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmPushButtonCallbackStruct *call_data = (XmPushButtonCallbackStruct *) xt_call_data ;

	XmListDeleteAllItems(listW);

	FILE *fp;
	fp = popen("scanimage -f \"%d\"", "r");

	char *c = (char *)malloc(100);
	while(fgets(c, 90, fp)) {
		XmListAddItem(listW, XmStringCreateLocalized(c), 0);
		XmListSelectPos(listW, 0, True);
	}

	pclose(fp);	
	free(c);
}

void nonumCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmPushButtonCallbackStruct *call_data = (XmPushButtonCallbackStruct *) xt_call_data ;

	num = NONUM;
}

void oneCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmToggleButtonCallbackStruct *call_data = (XmToggleButtonCallbackStruct *) xt_call_data ;

	num = ONE;
}

void twoCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmToggleButtonCallbackStruct *call_data = (XmToggleButtonCallbackStruct *) xt_call_data ;

	num = TWO;
}

void set_toggleCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmRowColumnCallbackStruct *call_data = (XmRowColumnCallbackStruct *) xt_call_data ;

	if (num == ONE) {
		XmToggleButtonSetState(oneW, True, True);
		XmToggleButtonSetState(twoW, False, True);
	} else if (num == TWO) {
		XmToggleButtonSetState(twoW, True, True);
		XmToggleButtonSetState(oneW, False, True);
	} else if (num == NONUM) {
		XmToggleButtonSetState(oneW, False, True);
		XmToggleButtonSetState(twoW, False, True);
	}
}

