/* 
 * Copyright 1993, 1994, 1995 Liverpool University Computer Science Department
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of L.U.C.S. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. L.U.C.S. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * FILE NAME:	xtar.c
 * CREATED:	Mon Nov  1 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	X windows tar program. Allows you to view a compressed tar
 *              file without unpacking it. Also enables you to extract
 *              individual files and individual directories.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <math.h>
#include <pwd.h>
#include <time.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/SelectioB.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/ScrolledW.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>

#include "Xbr.h"
#include "xtarint.h"
#include "common.h"

#ifndef _INTEGRATE
#include "help.h"
#include "about.h"
#include "resources.h"
#endif

#ifndef UID_NO_CHANGE
#define UID_NO_CHANGE		-1
#endif
#ifndef GID_NO_CHANGE
#define GID_NO_CHANGE		-1
#endif

#define	EXTRACT			0
#define	EXTRACT_AS		1
#define	EXTRACT_FILE		2
#define	EXTRACT_DIRECTORY	3
#define	EXTRACT_ALL		4

#define	REGEX_RESET		0
#define	REGEX_NEW		1
#define	REGEX_NEXT		2

#define	FAILED_Permissions	1
#define	FAILED_TimeStamp	2
#define	FAILED_GroupID		4
#define	FAILED_Ownership	8

#define	IGNORE_ATTRIBUTES	0
#define	SET_ATTRIBUTES		1

#define PERM_IRUSR 0000400
#define PERM_IWUSR 0000200
#define PERM_IXUSR 0000100
#define PERM_IRGRP 0000040
#define PERM_IWGRP 0000020
#define PERM_IXGRP 0000010
#define PERM_IROTH 0000004
#define PERM_IWOTH 0000002
#define PERM_IXOTH 0000001

#ifdef _INTEGRATE
extern Widget TopLevel;
extern XtAppContext app_context;
#else
Widget TopLevel;
XtAppContext app_context;
#endif

Widget TarShell = NULL;	/* Main top level shell                              */
Widget TarWindow = NULL,/* Main window - XmMainWindow                        */
       MenuBar,		/* Menu bar for main window                          */
       MainForm,	/* Work are of window is an XmForm                   */
       TarFileLabel,	/* Name of tar archive                               */
       TarSizeLabel,	/* Size of tar archive                               */
       UnpackSizeLabel,	/* Size of tar archive unpacked                      */
       FileList,	/* List of files in tar archive                      */
       ExtractPB,	/* Push button to extract a single file              */
       ExtractAsPB,	/* Push button to extract a file as something else   */
       ViewPB;		/* Push button to view selected file                 */




/*************************** XTAR ACTIONS ************************************/

static void xtar_nextAction(Widget, XEvent *, String *, Cardinal *);
static void xtar_searchAction(Widget,XEvent *, String*, Cardinal*);

static XtActionsRec xtar_actions[] = {
    {"next", xtar_nextAction},
    {"search", xtar_searchAction}
};

/*************************** PULLDOWN MENUS **********************************/

/*****************************************************************************/
/* Main XTar menu - unpack, search and quit functions.                       */
/*****************************************************************************/
static void xtar_extractCallback(Widget, XtPointer, XtPointer);
static void xtar_filterCallback(Widget, XtPointer, XtPointer);
static void xtar_freeCallback(Widget, XtPointer, XtPointer);
static void xtar_openCallback(Widget, XtPointer, XtPointer);
static void xtar_searchCallback(Widget, XtPointer, XtPointer);
static void xtar_nextCallback(Widget, XtPointer, XtPointer);
static void xtar_listCB(Widget, XtPointer, XtPointer);

#define TAR_Pulldown		0
#define TAR_Cascade		1
#define TAR_Open		2
#define TAR_Unpack		3
#define TAR_UnpackAs		4
#define TAR_Separator1		5
#define TAR_Search		6
#define TAR_Next		7
#define TAR_Separator2		8
#define	TAR_ShowUID		9
#define TAR_Separator3		10
#define TAR_Quit		11

Widget *tar_menu;

static XbrMenuData tar_menu_data[] = {
    { XbrPB,       "Open...", xtar_openCallback,    NULL, 0, NULL, NULL, 0, 0 },
    { XbrPB,       "Unpack all",  xtar_extractCallback, (XtPointer)EXTRACT, 0, NULL, NULL, 0, 0 },
    { XbrPB,       "Unpack  all as...",  xtar_extractCallback, (XtPointer)EXTRACT_AS, 0, NULL, NULL, 0, 0 },
    { XbrSEPARATOR, NULL,     NULL,                 NULL, 0, NULL, NULL, 0, 0 },
    { XbrPB,       "Search...",xtar_searchCallback, NULL, 0, NULL, NULL, 0, 0 },
    { XbrPB,       "Next",    xtar_nextCallback,    NULL, 0, NULL, NULL, 0, 0 },
    { XbrSEPARATOR, NULL,     NULL,                 NULL, 0, NULL, NULL, 0, 0 },
    { XbrTOGGLE,    "Convert UID/GID", xtar_listCB, NULL, XbrNOSET, NULL, NULL, 0, 0 },
    { XbrSEPARATOR, NULL,     NULL,                 NULL, 0, NULL, NULL, 0, 0 },
#ifdef _INTEGRATE
    { XbrPB,       "Close",   NULL,                 NULL, 0, NULL, NULL, 0, 0 },
#else
    { XbrPB,       "Quit",    NULL,                 NULL, 0, NULL, NULL, 0, 0 },
#endif
};

/*****************************************************************************/
/* Options menu - actions for operations on single files.                 */
/*****************************************************************************/
static void xtar_viewCallback(Widget, XtPointer, XtPointer);
static void xtar_saveCallback(Widget, XtPointer, XtPointer);

#define	OPS_Pulldown		0
#define	OPS_Cascade		1
#define	OPS_View		2
#define	OPS_Extract		3
#define	OPS_ExtractAs		4

Widget *ops_menu;

static XbrMenuData ops_menu_data[] = {
    { XbrPB,        "View",          xtar_viewCallback,  NULL,
        0, NULL, NULL, 0, 0 },
    { XbrPB,        "Extract",       xtar_saveCallback,  (XtPointer)EXTRACT,
        0, NULL, NULL, 0, 0 },
    { XbrPB,        "Extract As...", xtar_saveCallback,  (XtPointer)EXTRACT_AS,
        0, NULL, NULL, 0, 0 },
};

/*****************************************************************************/
/* Atttributes menu - which attributes to keep when unpacking files.         */
/*****************************************************************************/
#define	ATTS_Pulldown		0
#define	ATTS_Cascade		1
#define	ATTS_Permissions	2
#define	ATTS_TimeStamp		3
#define	ATTS_Ownership		4
#define	ATTS_GroupID		5

Widget *atts_menu;

static XbrMenuData atts_menu_data[] = {
    { XbrTOGGLE,    "Permissions", NULL, NULL, XbrNOSET, NULL, NULL, 0, 0 },
    { XbrTOGGLE,    "Time Stamp",  NULL, NULL, XbrNOSET, NULL, NULL, 0, 0 },
    { XbrTOGGLE,    "Ownership",   NULL, NULL, XbrNOSET, NULL, NULL, 0, 0 },
    { XbrTOGGLE,    "Group ID",    NULL, NULL, XbrNOSET, NULL, NULL, 0, 0 }
};

/*****************************************************************************/
/* About menu - about screen & help window                                   */
/*****************************************************************************/
#define HELP_Pulldown           0
#define HELP_Cascade            1
#define HELP_About              2
#define HELP_Help 		3

Widget *help_menu;

#ifdef _INTEGRATE
static XbrMenuData *help_menu_data;
#else
static XbrMenuData help_menu_data[] = {
    { XbrPB, "About...",   about_callback, NULL, 0, NULL, NULL, 0, 0 },
    { XbrPB, "On XTar...", help_callback, (XtPointer)&xtar_resources.helpDoc, 0,
      NULL, NULL, 0, 0 },
};
#endif




/***************************** POPUP MENUS ***********************************/

/*****************************************************************************/
/* Popup menu which appears in package list.                                 */
/*****************************************************************************/
static void xtar_popupOptions(Widget, XtPointer, XEvent *, Boolean *);

#define LIST_Popup		0
#define LIST_Title		1
#define LIST_Separtor		2
#define LIST_View		3
#define LIST_Extract		4
#define LIST_ExtractAs		5

Widget *list_menu;

static XbrMenuData list_menu_data[] = {
    { XbrLABEL,     "OPTIONS",       NULL,               NULL,
        0, NULL, NULL, 0, 0 },
    { XbrSEPARATOR, NULL,            NULL,               NULL,
        0, NULL, NULL, 0, 0 },
    { XbrPB,        "View",          xtar_viewCallback,  NULL,
        0, NULL, NULL, 0, 0 },
    { XbrPB,        "Extract",       xtar_saveCallback,  (XtPointer)EXTRACT,
        0, NULL, NULL, 0, 0 },
    { XbrPB,        "Extract As...", xtar_saveCallback,  (XtPointer)EXTRACT_AS,
        0, NULL, NULL, 0, 0 }
};




/******************************* GLOBALS!! ***********************************/

static char tarfile[256];
static char cwd[257];
static TAR_FILE *first_file = NULL;
static int unpacked_size = 0;
static int showPB;



/*************************** FUNCTION PROTOTYPES *****************************/
static int xtar_checksum(char *);		  /* Header checksum         */
static void xtar_create();			  /* Create XTAR widgets     */
static TAR_FILE *xtar_convert(FILE_HEADER *, int);/* TAR_FILE to FILEHEADER  */
static int xtar_extract(char *, char *, int, int);/* Extract file(s)         */
static void xtar_extractItem(int);		  /* Extract file or dir     */
static void xtar_free();			  /* Free linked list        */
static TAR_FILE *xtar_link(TAR_FILE *);		  /* Find soft/hard link     */
static void xtar_list();			  /* List archived files     */
static char *xtar_mode(mode_t, int);		  /* Mode & file permissions */
static int xtar_null(FILE_HEADER *);		  /* Null header??           */
static int xtar_read();				  /* Load archive into list  */
static TAR_FILE *xtar_selection();		  /* Which file selected     */
static int xtar_show(char *, char *);		  /* Display a text file     */
static int xtar_type(int, char *);		  /* Actual file type        */
static void xtar_search();                        /* Search for a filename   */
static void xtar_highlight(int);                  /* Highlight pos in list   */
static int xtar_regex(char *, int);               /* Search, Next etc.       */
static int xtar_attributes(TAR_FILE *, char *);   /* Set file attributes     */
static int xtar_directory(TAR_FILE *);            /* Used for dir attributes */
static char *xtar_user(uid_t);                    /* Convert user ID to name */
static char *xtar_group(gid_t);                   /* Convert group ID to name*/
int xtar_view(char *);				  /* Entry point for xtar    */
#ifdef _BSD_SIGNALS
static void xtar_signal();                        /* Catch child dying & wait*/
#endif
#ifdef _INTEGRATE
void xtar_init(char *);
#endif

/*************************** CALLBACK PROTOTYPES *****************************/
static void xtar_searchOkCallback(Widget, XtPointer, XtPointer);
static void xtar_singleCB(Widget, XtPointer, XtPointer);
static void xtar_editCB(Widget, XtPointer, XtPointer);
static void xtar_dumpCB(Widget, XtPointer, XtPointer);
static void xtar_exitCB(Widget, XtPointer, XtPointer);

/**************************** EXTERNAL FUNCTIONS *****************************/

/* xtar_view:*****************************************************************/
/* xtar_view: This is the entry point for the xtar viewer program.           */
/* xtar_view:*****************************************************************/

int xtar_view(char *archive)
{
    Boolean sensitive;

#ifdef _INTEGRATE
    static int first = 1;
#endif

    /* Get busy */
    XbrCursorBusy(TopLevel);

#ifdef _INTEGRATE
    xtar_create();
    if(archive == NULL && !first) {
        XtPopup(TarShell, XtGrabNone);
        XbrCursorReset(TopLevel);
        return(1);
    }
#endif

    /* Free memory */
    xtar_free();

    /* We require the absolute path name of the tar archive. */
    if(archive && (strlen(archive) != 0) && (archive[0] != '/'))
       sprintf(tarfile, "%s/%s", cwd, archive);
    else
       strcpy(tarfile, archive);

    /* Read in tar archive if present. If successful display details
       of size - both unpacked and current size.
    */
    if(tarfile == NULL || strlen(tarfile) == 0 || !xtar_read()) {
        XbrListClear(FileList);
        XbrLabelSet(TarFileLabel, "Filename: NO TAR FILE OPEN");
        XbrLabelSet(TarSizeLabel, "File Size:");
        XbrLabelSet(UnpackSizeLabel, "Unpacked Size:");
        sensitive = False;
    } else {
        XbrLabelSet(TarFileLabel, "Filename: %s", fname_basename(tarfile));
        XbrLabelSet(TarSizeLabel, "File Size: %s",
          ascii_size(stat_size(tarfile)));
        XbrLabelSet(UnpackSizeLabel, "Unpacked Size: %s",
          ascii_size(unpacked_size));
        xtar_list();
        sensitive = True;
    }

    /* Enable/disable sensitivity of certain widgets. */
    XtSetSensitive(tar_menu[TAR_Unpack], sensitive);
    XtSetSensitive(tar_menu[TAR_UnpackAs], sensitive);
    XtSetSensitive(tar_menu[TAR_Search], sensitive);
    XtSetSensitive(tar_menu[TAR_Next], False);
    XtSetSensitive(ops_menu[OPS_View], False);
    XtSetSensitive(ops_menu[OPS_Extract], False);
    XtSetSensitive(ops_menu[OPS_ExtractAs], False);
#ifndef _DODGY_POPUP
    XtSetSensitive(list_menu[LIST_View], False);
    XtSetSensitive(list_menu[LIST_Extract], False);
    XtSetSensitive(list_menu[LIST_ExtractAs], False);
#endif
    XtSetSensitive(ViewPB, False);
    XtSetSensitive(ExtractPB, False);
    XtSetSensitive(ExtractAsPB, False);

    /* Reset the cursor */
    XbrCursorReset(TopLevel);

#ifdef _INTEGRATE
    XtPopup(TarShell, XtGrabNone);
    first = 0;
#endif

    return(1);
}





/**************************** INTERNAL FUNCTIONS *****************************/

#ifdef _INTEGRATE
void xtar_init(char *mappings)
{
    int argc = 1;
    char **argv;
#else
void main(int argc, char *argv[])
{
    char *mappings;
#endif
    char filename[256];

#ifndef _INTEGRATE
#if XtSpecificationRelease > 4
    /* Create a new top level shell and intialise Intrinsics */
    TarShell = XtAppInitialize(&app_context, "XTar", NULL, 0, &argc, argv,
       fallback_resources, NULL, 0);
#else
    /* Create a new top level shell and intialise Intrinsics */
    TarShell = XtAppInitialize(&app_context, "XTar", NULL, 0, (Cardinal *)&argc,
      argv, fallback_resources, NULL, 0);
#endif
#endif

    /* Parse arguments - usage is <progname> followed by an optional
       tar archive.
    */
    if(argc == 1) {
        strcpy(filename, "");
    } else if(argc == 2) {
        strcpy(filename, argv[1]);
    } else {
        fprintf(stderr, "Error usage %s <tarfile>\n", argv[0]);
        exit(1);
    }

#ifndef _INTEGRATE
    /* Get resources */
    XtGetApplicationResources(TarShell, &xtar_resources, resources,
      XtNumber(resources), NULL, 0);
    mappings = xtar_resources.mappings;
    showPB = xtar_resources.showPB;
#endif

    /* Create mappings. */
    mappings_create(mappings);

    /* Catch child dying and make sure we don't have zombie processes */
#ifdef _BSD_SIGNALS
    signal(SIGCHLD, xtar_signal, -1);
#else
    signal(SIGCHLD, SIG_IGN);
#endif

    /* Get the CWD */
    if(env_cwd(cwd, 256) == (char *)NULL)
        cwd[0] = 0;

#ifndef _INTEGRATE
    /* Build widgets */
    xtar_create();

    TopLevel = TarShell;

    /* Read in tar file and display */
    xtar_view(filename);
#endif

#ifndef _INTEGRATE
    /* Realize top level widget */
    XtRealizeWidget(TopLevel);
    XbrCursorAdd(TarShell, XbrNOGRAB);

    /* Loop */
    XtAppMainLoop(app_context);
#endif
}

/* xtar_create:***************************************************************/
/* xtar_create: Create all the top level widgets associated with xtar.       */
/* xtar_create:***************************************************************/

static void xtar_create()
{
    Cardinal n;
    Arg args[10];
    XmString string;


#ifdef _INTEGRATE
    if(TarWindow != NULL) return;
    TarShell = XtCreatePopupShell("TarShell", topLevelShellWidgetClass,
      TopLevel, NULL, 0);
    XtAddCallback(TarShell, XmNpopupCallback, XbrCursorAddCB,
      (XtPointer)XbrNOGRAB);
    XtAddCallback(TarShell, XmNpopdownCallback, XbrCursorDeleteCB, NULL);

#endif

    /* Install actions for intiating search and selecting next match. */
    XtAppAddActions(app_context, xtar_actions, XtNumber(xtar_actions));

    /* TarWindow. XmMAINWINDOW. */
    n = 0;
    XtSetArg(args[n], XmNshowSeparator, True); n++;
    TarWindow = XmCreateMainWindow(TarShell, "TarWindow", args, n);
    XtManageChild(TarWindow);

    /* MenuBar. XmMENUBAR. */
    MenuBar = XmCreateMenuBar(TarWindow, "MenuBar", NULL, 0);
    XtManageChild(MenuBar);

    /* MainForm. XmFORM. */
    n = 0;
    XtSetArg(args[n], XmNmarginHeight, 5); n++;
    XtSetArg(args[n], XmNmarginWidth, 5); n++;
    MainForm = XmCreateForm(TarWindow, "MainForm", args, n);
    XtManageChild(MainForm);

    /* Set menu bar and work area - set remainder to NULL */
    XmMainWindowSetAreas(TarWindow, MenuBar, NULL, NULL, NULL, MainForm);

    /* ViewPB. XmPUSHBUTTON. */
    n = 0;
    string = XmStringCreateLocalized("View...");
    XtSetArg(args[n], XmNwidth, 150); n++;
    XtSetArg(args[n], XmNheight, 35); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    ViewPB = XmCreatePushButton(MainForm, "ViewPB", args, n);
    XmStringFree(string);
    XtAddCallback(ViewPB, XmNactivateCallback, xtar_viewCallback, NULL);
    if(showPB) XtManageChild(ViewPB);

    /* ExtractPB. XmPUSHBUTTON. */
    n = 0;
    string = XmStringCreateLocalized("Extract");
    XtSetArg(args[n], XmNwidth, 150); n++;
    XtSetArg(args[n], XmNheight, 35); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, ViewPB); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    ExtractPB = XmCreatePushButton(MainForm, "ExtractPB", args, n);
    XmStringFree(string);
    XtAddCallback(ExtractPB, XmNactivateCallback, xtar_saveCallback,
      (XtPointer)EXTRACT);
    if(showPB) XtManageChild(ExtractPB);

    /* ExtractAsPB. XmPUSHBUTTON. */
    n = 0;
    string = XmStringCreateLocalized("Extract As...");
    XtSetArg(args[n], XmNwidth, 150); n++;
    XtSetArg(args[n], XmNheight, 35); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, ExtractPB); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    ExtractAsPB = XmCreatePushButton(MainForm, "ExtractAsPB", args, n);
    XmStringFree(string);
    XtAddCallback(ExtractAsPB, XmNactivateCallback, xtar_saveCallback,
      (XtPointer)EXTRACT_AS);
    if(showPB) XtManageChild(ExtractAsPB);

    /* TarFileLabel. XmLABEL. */
    n = 0;
    string = XmStringCreateLocalized("Filename:");
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    TarFileLabel = XmCreateLabel(MainForm, "TarFileLabel", args, n);
    XmStringFree(string);
    XtManageChild(TarFileLabel);

    /* TarSizeLabel. XmLABEL. */
    n = 0;
    string = XmStringCreateLocalized("File Size:");
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, 49); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, TarFileLabel); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    TarSizeLabel = XmCreateLabel(MainForm, "TarSizeLabel", args, n);
    XmStringFree(string);
    XtManageChild(TarSizeLabel);

    /* UnpackSizeLabel. XmLABEL. */
    n = 0;
    string = XmStringCreateLocalized("Unpacked Size:");
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 50); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, TarFileLabel); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    UnpackSizeLabel = XmCreateLabel(MainForm, "UnpackSizeLabel", args, n);
    XmStringFree(string);
    XtManageChild(UnpackSizeLabel);

    /* FileList. XmLIST. */
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, UnpackSizeLabel); n++;
    if(showPB) {
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNbottomWidget, ViewPB); n++;
        XtSetArg(args[n], XmNbottomOffset, 5); n++;
    } else {
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    }
    FileList = XmCreateScrolledList(MainForm, "FileList", args, n);
#ifndef _DODGY_POPUP
    list_menu = XbrMenu(FileList, "ListPopup", XbrMENU_POPUP, list_menu_data,
        XtNumber(list_menu_data));
    XtAddEventHandler(FileList, ButtonPressMask, False, xtar_popupOptions,
        list_menu[LIST_Popup]);
#endif
    XtAddCallback(FileList, XmNbrowseSelectionCallback, xtar_singleCB, NULL);
    XtAddCallback(FileList, XmNdefaultActionCallback, xtar_viewCallback, NULL);
    XtManageChild(FileList);

    /* Use Xbr library to build pulldown menus */
    tar_menu = XbrMenu(MenuBar, "XTar", XbrMENU_PULLDOWN, tar_menu_data,
      XtNumber(tar_menu_data));
#ifdef _INTEGRATE
    XtAddCallback(tar_menu[TAR_Quit], XmNactivateCallback, XbrCBPopdownShell,
      TarShell);
#else
    XtAddCallback(tar_menu[TAR_Quit], XmNactivateCallback, xtar_exitCB, NULL);
#endif
    ops_menu = XbrMenu(MenuBar, "Options", XbrMENU_PULLDOWN, ops_menu_data,
      XtNumber(ops_menu_data));
    atts_menu = XbrMenu(MenuBar, "Set Attributes", XbrMENU_PULLDOWN,
      atts_menu_data, XtNumber(atts_menu_data));
#ifndef _INTEGRATE
    help_menu = XbrMenu(MenuBar, "Help", XbrMENU_PULLDOWN, help_menu_data,
      XtNumber(help_menu_data));
    XtVaSetValues(MenuBar, XmNmenuHelpWidget,help_menu[HELP_Cascade], NULL);
#endif

    /* Only root has permissions to extract tar files with group and ownership
       intact.
    */
    if(!env_isroot()) {
        XtSetSensitive(atts_menu[ATTS_Ownership], False);
        XtSetSensitive(atts_menu[ATTS_GroupID], False);
    }
}

/* xtar_read:*****************************************************************/
/* xtar_read: This routine passes once over the tar archive and stores the   */
/* xtar_read: headers in a linked list. The offset of each file is also      */
/* xtar_read: stored so that any future reference to that file can be        */
/* xtar_read: accompanied with a seek.                                       */
/* xtar_read:*****************************************************************/

static int xtar_read()
{
    int fp = -1;
    FILE_HEADER file_header;
    TAR_FILE *current_file = NULL;
    int size, seek_size, pos = 0, read_size = 0;
    char errmsg[256];

    /* Current unpacked size set to zero */
    unpacked_size = 0;

    /* Open the file */
    if((fp = pipe_open(tarfile)) == -1) {
        sprintf(errmsg, "Error opening %s!\n", tarfile);
        goto failed;
    }

    /* Read a file header */
    read_size = pipe_read(fp, (char *)&file_header, FILE_HEADER_SIZE);
    if(read_size !=  FILE_HEADER_SIZE) {
        if(read_size == 0)
            sprintf(errmsg, "Error reading tar archive!\nEmpty file");
        else
            sprintf(errmsg, "Error reading tar archive!\n %s\n", str_error(errno));
        goto failed;
    }
    pos += FILE_HEADER_SIZE;

    /* Parse headers until reach NULL record */
    while(!xtar_null(&file_header)) {
        int magic_ok = 0;

        /* Check the magic number - is it a tar file? */
        if(!strncmp(file_header.magic, "ustar", 5))
            magic_ok = 1;

        /* Calculate the checksum - is the archive corrupt? */
        if(!xtar_checksum((char *)&file_header)) {
            if(magic_ok)
                sprintf(errmsg, "Checksum error!\nTar file corrupt");
            else
                sprintf(errmsg, "Not a tar file!");
            goto failed;
        }

        /* We maintain a linked list of files in the archive  */
        if(first_file == NULL) {
            first_file = xtar_convert(&file_header, pos);
            current_file = first_file;
        }
        else {
            current_file->next = xtar_convert(&file_header, pos);
            current_file = current_file->next;
        }

        /* Check memory */
        if(current_file == NULL) {
            sprintf(errmsg, "Error allocating memory!");
            goto failed;
        }

        /* Skip the actual file (if there is one) */
        if(current_file->size > 0) {
            seek_size = (((current_file->size-1)/512)+1)*512;
            if(pipe_seek(fp, seek_size) < 0) {
                sprintf(errmsg, "Error reading tar archive!");
                goto failed;
            }
            pos += seek_size;
        }
        unpacked_size += current_file->size;
 
        /* Read file header for next iteration  */
        read_size = pipe_read(fp, (char *)&file_header, FILE_HEADER_SIZE);
        if(read_size != FILE_HEADER_SIZE) {
              pipe_close(fp);
              if(read_size == 0)
                  sprintf(errmsg, "Error reading tar archive!\nUnexpected EOF");
              else
                  sprintf(errmsg, "Error reading tar archive!\n%s\n", str_error(errno));
              goto failed;
          }
          pos += FILE_HEADER_SIZE;
    }

    /* Close file */
    pipe_close(fp);

    /* OK */
    return(1);

failed:
    /* Summats up */
    if(fp != -1) pipe_close(fp);
    xtar_free();
    XbrDialogError(TarShell, errmsg);
    return(0);
}

/* xtar_convert:**************************************************************/
/* xtar_convert: This takes an actual TAR_HEADER and converts to be of type  */
/* xtar_convert: FILE_HEADER. This is a form which is much more easier to    */
/* xtar_convert: handle than the standard header.                            */
/* xtar_convert:**************************************************************/

static TAR_FILE *xtar_convert(FILE_HEADER *file_header, int offset)
{
    TAR_FILE *tar_file;
    time_t mtime;
    char *mtime_str;

    /* Allocate memory */
    if((tar_file = (TAR_FILE *) malloc(sizeof(TAR_FILE))) == NULL)
        return(NULL);

    /* Copy in simple values */
    tar_file->mode = (mode_t)num_decimal(file_header->mode, MODE_SZ);
    tar_file->file_type = xtar_type(file_header->typeflag, file_header->name);
    tar_file->uid  = num_decimal(file_header->uid, UID_SZ);
    tar_file->gid  = num_decimal(file_header->gid, GID_SZ);
    tar_file->size = num_decimal(file_header->size, SIZE_SZ);
    strcpy(tar_file->name, file_header->name);
    strcpy(tar_file->linkname, file_header->linkname);
    tar_file->file_offset = offset;
    tar_file->next = NULL;

    /* Get the modification time */
    mtime = num_decimal(file_header->mtime, MTIME_SZ);
    tar_file->time = mtime;
    mtime_str = ctime(&mtime);
    mtime_str[16] = 0; mtime_str[24] = 0;  /* Blitz secs & newline */
    sprintf(tar_file->mtime, "%s %s", &mtime_str[4], &mtime_str[20]);

    /* Put file mode into drwxr--r-- form */
    strcpy(tar_file->permissions,xtar_mode(tar_file->mode,tar_file->file_type));

    /* OK */
    return(tar_file);
}

/* xtar_list:*****************************************************************/
/* xtar_list: Once we have read in the archive header and we have a linked   */
/* xtar_list: list of files we can display them one at a time in the list.   */
/* xtar_list: The filter pattern is taken into account so that the user can  */
/* xtar_list: search for a particular file name.                             */
/* xtar_list:*****************************************************************/

static void xtar_list()
{
    Boolean uid = XmToggleButtonGetState(tar_menu[TAR_ShowUID]);
    TAR_FILE *tf;
    char buffer[BUFSIZ];

    /* Clear the list of files to begin with */
    XbrListClear(FileList);

    /*  Display each file in linked list. */ 
    for(tf = first_file; tf != NULL; tf = tf->next) {
        switch(tf->file_type) {
            case TYPE_SOFTLINK:
                if(uid)
                   sprintf(buffer, "%s  %8s/%-8s %10d %s %s -> %s",
                     tf->permissions, xtar_user(tf->uid), xtar_group(tf->gid),
                     tf->size, tf->mtime, tf->name, tf->linkname);
                else
                   sprintf(buffer, "%s  %3d/%-3d %10d %s %s -> %s",
                     tf->permissions, tf->uid, tf->gid, tf->size, tf->mtime,
                     tf->name, tf->linkname);
                break;
            case TYPE_HARDLINK:
                 if(uid)
                    sprintf(buffer, "%s  %8s/%-8s %10d %s %s linked to %s",
                      tf->permissions, xtar_user(tf->uid), xtar_group(tf->gid),
                      tf->size, tf->mtime, tf->name, tf->linkname); 
                 else
                    sprintf(buffer, "%s  %3d/%-3d %10d %s %s linked to %s",
                      tf->permissions, tf->uid, tf->gid, tf->size, tf->mtime,
                      tf->name,        tf->linkname);
                  break;
            default:
                if(uid)
                    sprintf(buffer, "%s  %8s/%-8s %10d %s %s", tf->permissions,
                      xtar_user(tf->uid), xtar_group(tf->gid), tf->size,
                      tf->mtime, tf->name);
                else
                    sprintf(buffer, "%s  %3d/%-3d %10d %s %s", tf->permissions,
                      tf->uid, tf->gid, tf->size, tf->mtime, tf->name);
        }
        XbrListAddItem(FileList, buffer, XbrLIST_SHOWNONE);
    }

    /* Make the first item in the list the first one visible */
    XmListSetPos(FileList, 1);
}

/* xtar_null:*****************************************************************/
/* xtar_null: A normal tar archive has 512bytes full of NULL bytes to mark   */
/* xtar_null: the end of the archive. This function just checks a file       */
/* xtar_null: header to see if it is that block.                             */
/* xtar_null:*****************************************************************/

static int xtar_null(FILE_HEADER *file_header)
{
    char *bytes = (char *) file_header;
    int i = FILE_HEADER_SIZE;

    /* If we come across one byte that isn'y null then return immediately. */
    while(i--) {
        if(bytes[i] != 0)
            return(0);
    }

    /* All nulls! */
    return(1);
}

/* xtar_mode:*****************************************************************/
/* xtar_mode: Given the mode in mode_t format and the file type from the     */
/* xtar_mode: header this routine returns the "drwxr--r--" permission bits.  */
/* xtar_mode:*****************************************************************/

static char *xtar_mode(mode_t mode, int file_type)
{
    static char permissions[11];

    /* Initiliaze mem */
    strcpy(permissions, "?---------");

    switch(file_type) {
        case TYPE_NORMAL    :
        case TYPE_HARDLINK  : permissions[0] = '-'; break;
        case TYPE_SOFTLINK  : permissions[0] = 'l'; break;
        case TYPE_CHARACTER : permissions[0] = 'c'; break;
        case TYPE_BLOCK     : permissions[0] = 'b'; break;
        case TYPE_DIRECTORY : permissions[0] = 'd'; break;
        default             : permissions[0] = '?'; break;
    }

    if(mode & PERM_IRUSR) permissions[1] = 'r';
    if(mode & PERM_IWUSR) permissions[2] = 'w';
    if(mode & PERM_IXUSR) permissions[3] = 'x';
    if(mode & PERM_IRGRP) permissions[4] = 'r';
    if(mode & PERM_IWGRP) permissions[5] = 'w';
    if(mode & PERM_IXGRP) permissions[6] = 'x';
    if(mode & PERM_IROTH) permissions[7] = 'r';
    if(mode & PERM_IWOTH) permissions[8] = 'w';
    if(mode & PERM_IXOTH) permissions[9] = 'x';


    /* Return string */
    return(permissions);
}

/* xtar_type:*****************************************************************/
/* xtar_type: If we get a UNIX file type in the header, then we must decide  */
/* xtar_type: if it is a directory or a normal file by looking at its file   */
/* xtar_type: name.                                                          */
/* xtar_type:*****************************************************************/

static int xtar_type(int file_type, char *name)
{
    /* Test file type - change it, if set to TYPE_UNIX */
    if(file_type == TYPE_UNIX) {
        if(name[strlen(name) - 1] == '/')
            return(TYPE_DIRECTORY);
        else
            return(TYPE_NORMAL);
    }
    else {
        return(file_type);
    }
}

/* xtar_free:*****************************************************************/
/* xtar_free: Free memory allocated for linked list.                         */
/* xtar_free:*****************************************************************/

static void xtar_free()
{
    TAR_FILE *free_file, *tmp_file;

    /* Free items in list on at a time. */
    free_file = first_file;
    while(free_file != NULL) {
        tmp_file = free_file->next;
        free(free_file);
        free_file = tmp_file;
    }

    /* Delete temporary files. */
    tmpfile_free();

    /* Reset our static pointer */
    first_file = NULL;
}

/* xtar_selection:************************************************************/
/* xtar_selection: We find out which item in the list of files is selected   */
/* xtar_selection: and then search the linked list for it. This is based     */
/* xtar_selection: upon its position in both lists and not its name.         */
/* xtar_selection:************************************************************/

static TAR_FILE *xtar_selection()
{
    int *position_list, position, i = 0;
    TAR_FILE *tf = first_file;

    /* Nothing selected */
    if(!XmListGetSelectedPos(FileList, &position_list, &position))
        return(NULL);

    /* Find out which TAR_FILE was selected */
    for(tf = first_file; tf != NULL; tf = tf->next) {
        /* Skip it if it does not match the pattern */
        i++;
        if(i == position_list[0])
            return(tf);
    }

    /* Not found */
    return(NULL);
}

/* xtar_link:*****************************************************************/
/* xtar_link: If we find a link in the tar archive then we must find out     */
/* xtar_link: which file it is linked to.                                    */
/* xtar_link:*****************************************************************/

static TAR_FILE *xtar_link(linkf)
TAR_FILE *linkf;
{
    char *dirname, *ptr, filename[BUFSIZ], linkname[BUFSIZ], *endptr;
    TAR_FILE *tf;

    /* If it is a hard link then life is easier */
    if(linkf->file_type == TYPE_HARDLINK) {
        strcpy(filename, linkf->linkname);
    } else {
        /* Do a UNIX dirname() on the link */
        if((dirname = fname_dirname(linkf->name)) == NULL)
            return(NULL);

        /* Resolve the path name */
        ptr = fname_resolve("%s/%s", dirname, linkf->linkname);
        if(ptr == NULL) return(NULL);
        strcpy(filename, ptr);
    
        /* If this was a link to a directory then add the / to it */
        if(linkf->linkname[strlen(linkf->linkname)-1] == '/')
            if(filename[strlen(filename)-1] != '/')
                strcat(filename, "/");
    }

    /* Search for the name in the tar file */
    for(tf = first_file; tf = tf->next; tf != NULL) {
        if(!strcmp(tf->name, filename))
            return(tf);
    }

    /* Can't find it if we reached here */
    return(NULL);
}

/* xtar_show:*****************************************************************/
/* xtar_show: Open a text widget, extract a file from the tar archive and    */
/* xtar_show: display it. Include filename and close window option.          */
/* xtar_show:*****************************************************************/

static int xtar_show(char *tfname, char *filename)
{
    Widget FileShell, FileForm, FileClosePB, FileSavePB,FileText, FileNameLabel;
    char buffer[256], *data;
    Cardinal n;
    Arg args[10];
    XmString string;
    int size;

    /* FileShell. XtTOPLEVELSHELL. */
    FileShell =  XtCreatePopupShell(tfname, topLevelShellWidgetClass,
      TarShell, NULL, 0);
    XtAddCallback(FileShell, XmNpopupCallback, XbrCursorAddCB, XbrNOGRAB);
    XtAddCallback(FileShell, XmNdestroyCallback, XbrCursorDeleteCB, NULL);

    /* FileForm. XmFORM. */
    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 5); n++;
    XtSetArg(args[n], XmNmarginHeight, 5); n++;
    FileForm = XmCreateForm(FileShell, "FileForm", args, n);
    XtManageChild(FileForm);

    /* FileClosePB. XmPUSHBUTTON. */
    n = 0;
    string = XmStringCreateLocalized("Close");
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNwidth, 150); n++;
    XtSetArg(args[n], XmNheight, 35); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    FileClosePB = XmCreatePushButton(FileForm, "FileClosePB", args, n);
    XmStringFree(string);
    XtAddCallback(FileClosePB,XmNactivateCallback,XbrCBDestroyWidget,FileShell);
    XtManageChild(FileClosePB);

    /* FileSavePB. XmPUSHBUTTON. */
    n = 0;
    string = XmStringCreateLocalized("Save As...");
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, FileClosePB); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNwidth, 150); n++;
    XtSetArg(args[n], XmNheight, 35); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    FileSavePB = XmCreatePushButton(FileForm, "FileSavePB", args, n);
    XmStringFree(string);
    XtManageChild(FileSavePB);

    /* FileNameLabel. XmLABEL. */
    n = 0;
    string = XmStringCreateLocalized("Filename:");
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, FileSavePB); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    FileNameLabel = XmCreateLabel(FileForm, "FileName", args, n);
    XmStringFree(string);
    XtManageChild(FileNameLabel);

    /* FileText. XmSCROLLEDTEXT. */
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget, FileClosePB); n++;
    XtSetArg(args[n], XmNbottomOffset, 5); n++;
    XtSetArg(args[n], XmNeditable, True); n++;
    XtSetArg(args[n], XmNcursorPositionVisible, True); n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    FileText = XmCreateScrolledText(FileForm, "FileText", args, n);
    XtAddCallback(FileSavePB, XmNactivateCallback, xtar_dumpCB, FileText);

    /* Load the file into memory */
    data = file_load(filename, &size);
    if(data == NULL) {
        unlink(filename);
        XtDestroyWidget(FileShell);
        XbrDialogError(TarShell, "Error allocating memory!");
        return(0);
    }
    unlink(filename);

    /* Set the filename label. */
    XbrLabelSet(FileNameLabel, "Filename: %s (%s)", tfname, ascii_size(size));

    /* Load text into widget */
    XmTextSetString(FileText, data);

    XtAddCallback(FileText, XmNvalueChangedCallback, xtar_editCB,FileNameLabel);

    /* Finished with data now */
    XtFree(data);

    /* Display text */
    XmTextShowPosition(FileText,0);
    XtManageChild(FileText);

    /* Show it */
    XtPopup(FileShell, XtGrabNone);

    /* OK. */
    return(1);
}

/* xtar_attributes:***********************************************************/
/* xtar_attributes: Given the file that has been extracted and a pointer to  */
/* xtar_attributes: details about the file set the attributes as requested   */
/* xtar_attributes: in the "Set Attributes" menu.                            */
/* xtar_attributes:***********************************************************/

static int xtar_attributes(TAR_FILE *tf, char *filename)
{
    int retval = 0;
    struct utimbuf *utimbuf;

    /* If the file type is not a directory then try and set the access
       permissions. We exclude directories because if write permission
       is not set we will not be able to extract further files into that
       directory. Therefore, a list of directories extracted is recorded
       and permissions changed later. See the function  "xtar_directory()"
    */
    if(tf->file_type != TYPE_DIRECTORY)
        if(XmToggleButtonGetState(atts_menu[ATTS_Permissions]))
            if(chmod(filename, tf->mode)) retval |= FAILED_Permissions;

    /* Set the time stamp to the same as that in the tar archive. */
    if(XmToggleButtonGetState(atts_menu[ATTS_TimeStamp])) {
        if(file_stamp(filename, tf->time) == 0)
            retval |= FAILED_TimeStamp;
    }

    /* Try and change the group ID to the same as in the tar archive. This
       is probably only useful when unpacking archives as root.
    */
    if(XmToggleButtonGetState(atts_menu[ATTS_GroupID]))
        if(chown(filename, UID_NO_CHANGE, tf->gid)) retval |= FAILED_GroupID;
 
    /* Try and change the ownership to the same as in the tar archive. This
       is probably only useful when unpacking archives as root.
    */
    if(XmToggleButtonGetState(atts_menu[ATTS_Ownership]))
        if(chown(filename, tf->uid, GID_NO_CHANGE)) retval |= FAILED_Ownership;

    /* Return success or failure */
    return(retval);
}

/* xtar_directory:************************************************************/
/* xtar_directory: If the user wants to extract lots of directories then we  */
/* xtar_directory: cannot set the permissions whilst we are extracting them  */
/* xtar_directory: because if one has no write permission then this will     */
/* xtar_directory: hinder extracting files further in the directory. So we   */
/* xtar_directory: keep track of the directory names and change the          */
/* xtar_directory: permissions later.                                        */
/* xtar_directory:************************************************************/

static int xtar_directory(TAR_FILE *tf)
{
    static TAR_FILE *tf_data = NULL;
    TAR_FILE *ptr;
    int retval = 1;

    /* We maintain a static pointer inside the function which points to
       the head of the list of directories extracted so far. When a null
       pointer is passed into the function we process the list and then
       set it to null again.
    */
    if(tf == NULL) {
        for(ptr = tf_data; ptr != NULL; ptr = ptr->nextdir) 
            if(chmod(ptr->name, ptr->mode)) retval = 0;
        tf_data = NULL;
        return(retval);
    }

    /* Add the new directory to the current list. */
    if(tf_data == NULL) {
        tf_data = tf;
    } else {
        for(ptr = tf_data; ptr->nextdir != NULL; ptr = ptr->nextdir);
        ptr->nextdir = tf;
    }
    tf->nextdir = NULL;

    /* OK. */
    return(retval);
}
        
/* xtar_extract:**************************************************************/
/* xtar_extract: Extract files from archive.                                 */
/* xtar_extract:**************************************************************/

static int xtar_extract(char *filename, char *dest, int flag, int doatts)
{
    int fpin = -1;
    FILE *fpout = NULL;
    TAR_FILE *tf;
    int to_read, read_size, seekval = 0, len = filename ? strlen(filename) : 0;
    int count, total, n, attributes = 0, last = -1;
    int dochmod = doatts && XmToggleButtonGetState(atts_menu[ATTS_Permissions]);
    Arg args[10];
    char buffer[512], *name, *ptr, *errmsg = NULL;

    /* Let's get busy */
    XbrCursorBusy(TopLevel);

    /* If we are unpacking more than one file - ie. a directory or the entire
       archive, and a destination file has been seleted then change to the
       appropriate directory.
     */
    if(flag != EXTRACT_FILE) {
        if(dest) {
            if(*dest != '.') {
                if(chdir(dest) == -1) {
                    errmsg = "Failed to change directory!";
                    goto failed;
                }
            }
        } else {
            ptr = fname_dirname(tarfile);
            if(ptr && (strlen(ptr) > 0)) chdir(ptr);  
        }
    }
 
    /* How many items in the list */
    n = 0;
    XtSetArg(args[n], XmNvisibleItemCount, &count); n++;
    XtSetArg(args[n], XmNitemCount, &total); n++;
    XtGetValues(FileList, args, n);

    /* Open tar archive */
    if((fpin = pipe_open(tarfile)) == -1) {
        errmsg = "Error opening archive!";
        goto failed;
    }

    /* Process files in archive one by one */
    n = 0;
    for(tf = first_file; tf != NULL; tf = tf->next) {
        seekval += 512; n++;
        /* Skip the file header block - read it in earlier. */
        if(pipe_seek(fpin, seekval) == -1) {
            errmsg = "Error reading archive!";
            goto failed;
        }
        seekval = 0;

        /* If we are extracting a directory then given it's name we only
           want to include those files which start with the same name.
        */
        if(flag == EXTRACT_DIRECTORY && strncmp(tf->name, filename, len) != 0) {
            if(tf->file_type == TYPE_NORMAL)seekval = num_roundup(tf->size,512);
            continue;
        }

        /* If we are extracting a file then we only want to continue if the
           current file matches the one we are extracting.
        */
        if(flag == EXTRACT_FILE && strcmp(tf->name, filename) != 0) {
            if(tf->file_type == TYPE_NORMAL)
                seekval = num_roundup(tf->size, 512);
            continue;
        }

        /* Highlight the file we are extracting */
        XmListSelectPos(FileList, n, False);
        last = n;

        /* Show the position but don't go off the bottom */
        if((total > count) && (n <= (total-count+1)))
            XmListSetPos(FileList, n);

        /* Some tar files do not have directory entries, which is a pain
           because it means the directory is not created before the file. Which
           means the file cannot be opened. Therefore we see if the directory
           exits first and if not, create it.
        */
        if(flag == EXTRACT_ALL) {
            if(tf->file_type == TYPE_NORMAL||tf->file_type == TYPE_SOFTLINK||
               tf->file_type == TYPE_HARDLINK) {
                ptr = fname_dirname(tf->name);
                if(file_exists(ptr) == 0)
                    mkdirhier(ptr, 0711);
            }
        }
 
        /* What file type do we have? */
        switch(tf->file_type) {
            case TYPE_NORMAL:
                /* Set name of output file */
                if(flag == EXTRACT_FILE && dest) 
                    name = dest;
                else
                    name = tf->name;
                /* Open output file */
                if((fpout = fopen(name, "w")) == NULL) {
                    XbrCursorReset(TopLevel);
                    /* ERROR - do we want to continue? */
                    if(!XbrDialogQuestion(TarShell, "Yes", "No",
                      "Error writing file %s. Continue?", tf->name))
                        goto failed;
                    XbrCursorBusy(TopLevel);
                    seekval += num_roundup(tf->size, 512);
                    break;
                }

                /* Read and write data until all file is written out */
                to_read = tf->size; read_size = 0;
                while(to_read -= read_size) {
                    /* Get size of block to read and write out */
                    if((read_size = (to_read%512)) == 0)
                        read_size = 512;
                    /* Read in a data block */
                    if(pipe_read(fpin, buffer, read_size) != read_size) {
                        errmsg = "Error reading archive!";
                        goto failed;
                    }
                    /* Write out block of data */
                    if(write(fileno(fpout), buffer, read_size) != read_size) {
                        XbrCursorReset(TopLevel);
                        if(!XbrDialogQuestion(TarShell, "Yes", "No", 
                          "Error writing file %s. Continue?", tf->name)) 
                            goto failed;
                        XbrCursorBusy(TopLevel);
                        seekval += to_read;
                        break;
                    }
                }
                fclose(fpout);
                if(doatts) attributes = xtar_attributes(tf, name);
                seekval = (512 - (tf->size % 512));
                if(seekval == 512) seekval = 0;
                break;
            case TYPE_DIRECTORY:
                if(!mkdirhier(tf->name, 0777)) {
                    XbrCursorReset(TopLevel);
                    if(!XbrDialogQuestion(TarShell, "Yes", "No",
                      "Error creating directory %s. Continue?", tf->name)) 
                        goto failed;
                    XbrCursorBusy(TopLevel);
                    break;
                }
                if(doatts) attributes = xtar_attributes(tf, tf->name);
                if(dochmod) xtar_directory(tf);
                break;
            case TYPE_SOFTLINK:
                if(symlink(tf->linkname, tf->name) == -1) {
                    XbrCursorReset(TopLevel);
                    if(!XbrDialogQuestion(TarShell, "Yes", "No",
                      "Error creating soft link %s. Continue?", tf->name))
                        goto failed;
                    XbrCursorBusy(TopLevel);
                    break;
                }   
                if(doatts) attributes = xtar_attributes(tf, tf->linkname);
                break;
            case TYPE_HARDLINK:
                if(link(tf->linkname, tf->name) == -1) {
                    XbrCursorReset(TopLevel);
                    if(!XbrDialogQuestion(TarShell, "Yes", "No",
                      "Error creating link %s. Continue?", tf->name))
                        goto failed;
                    XbrCursorBusy(TopLevel);
                    break;
                }   
                if(doatts) attributes = xtar_attributes(tf, tf->linkname);
                break;
            default:
                XbrCursorReset(TopLevel);
                if(!XbrDialogQuestion(TarShell, "Yes", "No",
                       "Unknown file type for %s. Continue?", tf->name))
                    goto failed;
                XbrCursorBusy(TopLevel);
                break;
        }

        /* Did we manage to set the attributes correctly? */
        if(attributes != 0) {
            XbrCursorReset(TopLevel);
            sprintf(buffer, "Failed to set following attributes: %s%s%s%sContinue?",
              attributes&FAILED_Permissions ? "Permissions " : "",
              attributes&FAILED_TimeStamp ? "Time Stamp " : "",
              attributes&FAILED_Ownership ? "Ownership " : "",
              attributes&FAILED_GroupID ? "Group ID " : "");
            if(!XbrDialogQuestion(TarShell, "Yes", "No", buffer))
                break;
            XbrCursorBusy(TopLevel);
            attributes = 0;
        }
    }

    /* Close the input stream. */
    pipe_close(fpin);

    /* If user wants to change the file permissions then change them for the
       directories.
    */
    if(dochmod)
        if(!xtar_directory(NULL))
            errmsg = "Failed to set directory permissions!";

    /* Return to the original current working directory. */
    chdir(cwd);

    /* Highlight and display the last item extracted in the list. */
    XmListSelectPos(FileList, last, True);

    /* Reset the cursor */
    XbrCursorReset(TopLevel);

    /* Error? */
    if(errmsg) XbrDialogError(TarShell, errmsg);

    /* Okey dokey. */
    return(1);

failed:
    if(fpin != -1) pipe_close(fpin);
    if(fpout) fclose(fpout);
    if(dochmod) xtar_directory(NULL);
    chdir(cwd);
    if(last != -1) XmListSelectPos(FileList, last, True);
    XbrCursorReset(TopLevel);
    if(errmsg) XbrDialogError(TarShell, errmsg);
    return(0);
}

/* xtar_extractItem:**********************************************************/
/* xtar_extractItem: Extract the selected file or directory. If we need to   */
/* xtar_extractItem: save a file under another name, then we display the     */
/* xtar_extractItem: file selection box and get a filename.                  */
/* xtar_extractItem:**********************************************************/

static void xtar_extractItem(int method)
{
    TAR_FILE *tf, *linkf;
    char *filename = NULL;
    int overwrite;

    /* Find the selected item in the list */
    if((tf = xtar_selection()) == NULL) {
        XbrDialogError(TarShell, "Could not get selected file!");
        return;
    }

    /* If the file is a soft-link we have to find out the file it is linked
       to and then extract that.
    */
    if(tf->file_type == TYPE_SOFTLINK || tf->file_type == TYPE_HARDLINK) {
        if((linkf = xtar_link(tf)) == NULL) {
            XbrDialogError(TarShell, "Error tracing link!");
            return;
        }
        tf = linkf;
    }

    /* If it is a directory then call the extraction routine */
    if(tf->file_type == TYPE_DIRECTORY) {
        if(method == EXTRACT_AS) {
            if((filename = XbrDialogFSB(TarShell)) == NULL)
                return;

            if(file_exists(filename)) {
                overwrite = XbrDialogQuestion(TarShell, "Yes", "No",
                  "File \"%s\" already exists! \nOverwrite?", filename);
                if(overwrite == 0)
                    return;
            }

            if(!mkdirhier(filename, 0777)) {
                XbrDialogError(TarShell, "Error stat'ing/creating directory!");
                return;
            }
        } 
        xtar_extract(tf->name, filename, EXTRACT_DIRECTORY, SET_ATTRIBUTES);
        XtFree(filename);
        return;
    }

    /* Make sure the file size is OK */
    if(tf->file_type == TYPE_NORMAL && tf->size <= 0) {
        XbrDialogError(TarShell, "File size is zero!");
        return;
    }

    /* If the user wants to save this as another name then popup the file
       selection box.
    */
    if(method == EXTRACT_AS) {
        if((filename = XbrDialogFSB(TarShell)) == NULL)
            return;
    }
    else {
        filename = strdup(fname_basename(tf->name));
    }

    /* If the file already exists do they really want to overwrite it? */
    if(file_exists(filename)) {
        if(XbrDialogQuestion(TarShell, "Yes", "No", 
            "%s exists\nOverwrite it?", filename))
            unlink(filename);
        else {
            XtFree(filename);
            return;
        }
    }

    xtar_extract(tf->name, filename, EXTRACT_FILE, SET_ATTRIBUTES);

    XtFree(filename);
}

/* xtar_checksum:*************************************************************/
/* xtar_checksum: This checksum is calculated by adding together all the     */
/* xtar_checksum: bytes in the header, with the checksum field full of       */
/* xtar_checksum: blanks.                                                    */
/* xtar_checksum:*************************************************************/

static int xtar_checksum(char *file_header)
{
    char check_header[FILE_HEADER_SIZE];
    int val = 0, i = FILE_HEADER_SIZE;

    /* Make a copy of the header because we are going to mess it up :-) */
    memcpy(check_header, file_header, FILE_HEADER_SIZE);

    /* Mess it up by adding blanks in the checksum field */
    strncpy(&check_header[148], "        ", 8);

    /* Loop round a calculate the checksum */
    while(i--) val += check_header[i];

    /* The TEST! */
    if(val != num_decimal(((FILE_HEADER *)file_header)->chksum, CHKSUM_SZ))
        return(0);
    else
        return(1);
}

/* xtar_user:*****************************************************************/
/* xtar_user: Given a user ID, return the name of that user. Returns a       */
/* xtar_user: pointer to static memory which may be overwritten any time.    */
/* xtar_user:*****************************************************************/

static char *xtar_user(uid_t uid)
{
    static struct passwd *passwd = (struct passwd *)NULL;
    static uid_t lastid = -1;

    /* Did we retrieve this last time? */
    if(lastid != uid) {
        passwd = getpwuid(uid);
        lastid = uid;
    }

    if(passwd)
        return(passwd->pw_name);
    else
        return("?");
}

/* xtar_group:****************************************************************/
/* xtar_group: Given a group ID, return the name of that group. Returns a    */
/* xtar_group: pointer to static memory which may be overwritten any time.   */
/* xtar_group:****************************************************************/

static char *xtar_group(gid_t gid)
{
    static struct group *group = (struct group *)NULL;
    static uid_t lastid = -1;

    /* Did we retrieve this last time? */
    if(lastid != gid) {
        group = getgrgid(gid);
        lastid = gid;
    }

    if(group)
        return(group->gr_name);
    else
        return("?");
}
#ifdef _BSD_SIGNALS
/* xtar_signal:***************************************************************/
/* xtar_signal: Catch child dying and wait for process to quit.              */
/* xtar_signal:***************************************************************/

void xtar_signal()
{
    int statusp;

    wait(&statusp);
}
#endif
    



/************************************** XEVENTS ******************************/

/* xtar_popupOptions:*********************************************************/
/* xtar_popupOptions: Display the popup menu in the list widget.             */
/* xtar_popupOptions:*********************************************************/

static void xtar_popupOptions(Widget w, XtPointer client_data, XEvent *event,
                              Boolean *continue_to_dispatch_return)
{
    TAR_FILE *tf;
    Widget menu = (Widget) client_data;
    XButtonPressedEvent *button_event = (XButtonPressedEvent *) event;

    /* Only act if third mouse button pressed */
    if(button_event->button != 3)
        return;

    /* Display menu in correct position */
    XmMenuPosition(menu, button_event);
    XtManageChild(menu);
}

/* xtar_search:***************************************************************/
/* xtar_search: This replaces the filter mechanism - instead of filtering    */
/* xtar_search: out the filenames we search for the next occurence.          */
/* xtar_search:***************************************************************/

static void xtar_search()
{
    static Widget SearchPrompt = NULL;
    Arg args[10];
    int n;
    XmString string;

    /* If the dialog box has already been created then jusr pop it up. */
    if(SearchPrompt != NULL) {
        XtManageChild(SearchPrompt);
        return;
    }

    /* SearchPrompt. XmPROMPTDIALOG. */
    n = 0;
    string = XmStringCreateLocalized("Enter search term:");
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNselectionLabelString, string); n++;
    SearchPrompt = XmCreatePromptDialog(TarShell, "PasswordPrompt", args, n);
    XmStringFree(string);
    XtUnmanageChild(XmSelectionBoxGetChild(SearchPrompt, XmDIALOG_HELP_BUTTON));
    XtAddCallback(XtParent(SearchPrompt), XmNpopupCallback, XbrCursorAddCB,
      (XtPointer)XbrGRAB);
    XtAddCallback(XtParent(SearchPrompt), XmNpopdownCallback, XbrCursorDeleteCB,
      NULL);
    XtAddCallback(SearchPrompt, XmNokCallback, xtar_searchOkCallback, NULL);

    /* Display it. */
    XtManageChild(SearchPrompt);
}






/********************************* CALLBACKS *********************************/

/* xtar_searchOkCallbac:*****************************************************/
/* xtar_searchOkCallback: User has typed something into the text widget.     */
/* xtar_searchOkCallback: Get the string and look for a file of the same     */
/* xtar_searchOkCallback: name.                                              */
/* xtar_searchOkCallback:*****************************************************/

static void xtar_searchOkCallback(Widget w, XtPointer client_data, XtPointer
                                  call_data)
{
    char *term;
    XmSelectionBoxCallbackStruct *data =
                                    (XmSelectionBoxCallbackStruct *) call_data;

    /* Now we have a seach term we can activate the "next" option which
       searches for the next occurence of the string.
    */
    XtSetSensitive(tar_menu[TAR_Next], True);

    /* Turn the XmString into a proper string. */
    if(!XmStringGetLtoR(data->value, XmFONTLIST_DEFAULT_TAG, &term))
        return;

    /* See if the regular expression string matches a filename. */
    if(!xtar_regex(term, REGEX_NEW)) {
        xtar_regex(term, REGEX_RESET);
        xtar_regex(term, REGEX_NEXT);
    }

    /* Free memory allocated by XmStringGetLtoR routine. */
    XtFree(term);
}

/* xtar_nextCallback:*********************************************************/
/* xtar_nextCallback: Search for the next occurence of a string.             */
/* xtar_nextCallback:*********************************************************/

static void xtar_nextCallback(Widget w, XtPointer client_data, XtPointer
                              call_data)
{
    /* Pass NULL to search function to tell it to use the current search
       term.
    */
    if(!xtar_regex(NULL, REGEX_NEXT)) {
        xtar_regex(NULL, REGEX_RESET);
        xtar_regex(NULL, REGEX_NEXT);
    }
}

/* xtar_regex:****************************************************************/
/* xtar_regex: This bit performs the search and allows use of next etc.      */
/* xtar_regex:****************************************************************/

static int xtar_regex(char *term, int flag)
{
    TAR_FILE *tf;
    static int last = -1, first = 1;
    static regexpr *re = (regexpr *) NULL;
    int i;

    /* We maintain our current position in the file list so that we can
       start again when 'next' is selected. A flag of REGEX_RESET indicates
       that we want to start searching from the beginning. A flag of REGEX_NEW
       indicates that we have a new regular expression. A flag of REGEX_NEXT
       means we want to search using the previous regular expression.
    */
    if(flag == REGEX_RESET) {
        last = -1;
        return(1);
    } else if(flag == REGEX_NEW) {
        if(re)
          free(re);
        if((re = reg_compile(term)) == NULL) {
            XbrDialogError(TopLevel, "Error compiling regular expression!");
            return(-1);
        }
        first = 0;
    } else if(flag == REGEX_NEXT) {
        if(first) {
            XbrDialogError(TarShell, "No previous expression!");
            return(-1);
        }
    }

    /* Scan each file name one at a time. */
    for(tf = first_file, i = 1; tf != NULL; tf = tf->next, i++) {
        if(reg_execute(re, tf->name)) {
            if(i <= last) continue;
            xtar_highlight(i);
            last = i;
            return(1);
        }
    }
    return(0);
}


/* xtar_highlight:************************************************************/
/* xtar_highlight: Highlight the given position and make it visible.         */
/* xtar_highlight:************************************************************/

static void xtar_highlight(int pos)
{
    int count, total, n;
    Arg args[10];

    /* How many items in the list */
    n = 0;
    XtSetArg(args[n], XmNvisibleItemCount, &count); n++;
    XtSetArg(args[n], XmNitemCount, &total); n++;
    XtGetValues(FileList, args, n);

    /* Set the highlight and display the line */ 
    XmListSelectPos(FileList, pos, n);
    if((total > count) && (pos <= (total-count+1)))
        XmListSetPos(FileList, pos);
    else
        XmListSetPos(FileList, total-count+1);
}

/* xtar_searchCallback:*******************************************************/
/* xtar_searchCallback: Call xtar_search() to request a search term.         */
/* xtar_searchCallback:*******************************************************/

static void xtar_searchCallback(Widget w, XtPointer client_data, XtPointer
                                call_data)
{
    xtar_search();
}

/* xtar_extractCallback:******************************************************/
/* xtar_extractCallback: Simple wrap around callback routine for extraction. */
/* xtar_extractCallback:******************************************************/

static void xtar_extractCallback(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
    int overwrite, mode = (int)client_data;
    char *filename = NULL;

    /* If the user wants to extract this under another name then create the
       directory.
    */
    if(mode == EXTRACT_AS) {
        /* Popup selection box and get filename. */
        if((filename = XbrDialogFSB(TarShell)) == NULL)
            return;

        if(file_exists(filename)) {
            overwrite = XbrDialogQuestion(TarShell, "Yes", "No",
              "File \"%s\" already exists! \nOverwrite?", filename);
            if(overwrite == 0) {
                XtFree(filename);
                return;
            }
        }

        /* Try and create directory. */
        if(!mkdirhier(filename, 0777)) {
            XbrDialogError(TarShell, "Error stat'ing/creating directory!");
            XtFree(filename);
            return;
        }
    }

    /* Call extraction routine. */
    xtar_extract(NULL, filename, EXTRACT_ALL, SET_ATTRIBUTES);

    /* Free memory allocated by XbrDialogFSB routine. */
    XtFree(filename);
}

/* xtar_openCallback:*********************************************************/
/* xtar_openCallback: User wants to open a new file - display the file       */
/* xtar_openCallback: selection box and get the selected file. Then free up  */
/* xtar_openCallback: old list and build a new one.                          */
/* xtar_openCallback:*********************************************************/

static void xtar_openCallback(Widget w, XtPointer client_data,
                              XtPointer call_data)
{
    char *filename;

    /* Open file selection box and get a tar archive to open */
    if((filename = XbrDialogFSB(TarShell)) == NULL)
        return;

    /* Call routine to view tar archive. */
    xtar_view(filename);

    /* Free up memory. */
    XtFree(filename);
}

/* xtar_saveCallback:*********************************************************/
/* xtar_saveCallback: Save the selected item.                                */
/* xtar_saveCallback:*********************************************************/

static void xtar_saveCallback(Widget w, XtPointer client_data,
                              XtPointer call_data)
{
    xtar_extractItem((int)client_data);
}

/* xtar_viewCallback:*********************************************************/
/* xtar_viewCallback: Get the selected file and display it in a text widget. */
/* xtar_viewCallback:*********************************************************/

static void xtar_viewCallback(Widget w, XtPointer client_data,
  XtPointer call_data)
{
    TAR_FILE *tf, *linkf;
    char *filename;

    /* Get the selected file */
    if((tf = xtar_selection()) == NULL)
        return;

    /* If it is a link then find out what it is link to */
    if(tf->file_type == TYPE_SOFTLINK || tf->file_type == TYPE_HARDLINK) {
        if((linkf = xtar_link(tf)) == NULL) {
            XbrDialogError(TarShell, "Error tracing link!");
            return;
        }
        tf = linkf;
    }

    /* If we have a weird file size then return */
    if(tf->size <= 0) return;

    /* Show the file */
    XbrCursorBusy(TopLevel);

    /* Extract file to temporary location */
    filename = tmpfile_create();
    if(!xtar_extract(tf->name, filename, EXTRACT_FILE, IGNORE_ATTRIBUTES)) {
        unlink(filename);
        return;
    }
    if(!mappings_execute(tf->name, filename, xtar_show))
        xtar_show(tf->name, filename);
    XbrCursorReset(TopLevel);
}

/* xtar_singleCB:*************************************************************/
/* xtar_singleCB: Action invoked when an item is selected in the list.       */
/* xtar_singleCB:*************************************************************/

static void xtar_singleCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    TAR_FILE *tf, *linkf;
    Boolean view = False, extract = False, extractas = False;

    /* Get the selected file in the tar archive. */    
    if((tf = xtar_selection()) != NULL) {
        /* If it is a link then find the file or directory it is linked too. */
        if(tf->file_type == TYPE_SOFTLINK || tf->file_type == TYPE_HARDLINK) {
            if((linkf = xtar_link(tf)) != NULL)
                tf = linkf;
        }
        /* Set sensitivity of widgets accordingly. */
        switch(tf->file_type) {
           case TYPE_NORMAL:
                view = True;
                extract = True;
                extractas = True;
                break;
            case TYPE_DIRECTORY: 
                view = False;
                extract = True;
                extractas = True;
                break;
            default: 
                view = False;
                extract = False;
                extractas = False;
        }
    }

    /* Set the values. */
#ifndef _DODGY_POPUP
    XtSetSensitive(list_menu[LIST_View], view);
    XtSetSensitive(list_menu[LIST_Extract], extract);
    XtSetSensitive(list_menu[LIST_ExtractAs], extractas);
#endif
    XtSetSensitive(ops_menu[OPS_View], view);
    XtSetSensitive(ops_menu[OPS_Extract], extract);
    XtSetSensitive(ops_menu[OPS_ExtractAs], extractas);
    XtSetSensitive(ViewPB, view);
    XtSetSensitive(ExtractPB, extract);
    XtSetSensitive(ExtractAsPB, extractas);
}

/* xtar_listCB:***************************************************************/
/* xtar_listCB: Re-list the contents of the tar archive.                     */
/* xtar_listCB:***************************************************************/
static void xtar_listCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    xtar_list();
}

/* xtar_editCB:***************************************************************/
/* xtar_editCB: Set a flag to show the user they have edited the file.       */
/* xtar_editCB:***************************************************************/

static void xtar_editCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget label = (Widget) client_data;
    char *string = XbrLabelGet(label);

    XbrLabelSet(label, "%s [EDITED]", string);

    XtRemoveCallback(w, XmNvalueChangedCallback, xtar_editCB, client_data);
}

/* xtar_dumpCB:***************************************************************/
/* xtar_dumpCB: Dump the contents of the text widget to a file.              */
/* xtar_dumpCB:***************************************************************/

static void xtar_dumpCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget text = (Widget) client_data, shell = XtParent(XtParent(text));
    char *filename = XbrDialogFSB(shell);
    FILE *fp;

    if(filename) {
        if(file_exists(filename)) {
            if(XbrDialogQuestion(shell, "Yes", "No",
                  "File \"%s\" already exists! \nOverwrite?", filename) == 0) {
                XtFree(filename);
                return;
            }
        }

        fp = fopen(filename, "w");
        XtFree(filename);
        if(fp == NULL) {
            XbrDialogError(shell, "Error opening file for writing!");
            return;
        }
        XbrCursorBusy(TopLevel);
        fprintf(fp, "%s", XmTextGetString(text));
        fclose(fp);
        XbrCursorReset(TopLevel);
    }
}
            
/* xtar_exitCB:***************************************************************/
/* xtar_exitCB: When program exits, remove all temporary files.              */
/* xtar_exitCB:***************************************************************/

static void xtar_exitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    /* Close windows. */
    XtCloseDisplay(XtDisplay(TarShell));

    /* Call tmpfile() function. */
    tmpfile_free();

    /* Go home. */
    exit(0);
}



/********************************* ACTIONS ***********************************/

/* xtar_nextAction:***********************************************************/
/* xtar_nextAction: Actions registered for key presses.                      */
/* xtar_nextAction:***********************************************************/

static void xtar_nextAction(Widget w,XEvent *event,String *params,Cardinal *num)
{
    /* If the next option is active then search for the next match, otherwise
       sound the bell.
    */
    if(XtIsSensitive(tar_menu[TAR_Next])) {
        if(!xtar_regex(NULL, REGEX_NEXT)) {
            xtar_regex(NULL, REGEX_RESET);
            xtar_regex(NULL, REGEX_NEXT);
        }
    } else {
        XBell(XtDisplay(TopLevel), 50);
    }
}

/* xtar_searchAction:*********************************************************/
/* xtar_searchAction: Action to execute searching mechanism.                 */
/* xtar_searchAction:*********************************************************/

static void xtar_searchAction(Widget w,XEvent *event,String*params,Cardinal*num)
{
    /* If the search option is active then search for the next match, otherwise
       sound the bell.
    */
    if(XtIsSensitive(tar_menu[TAR_Search]))
        xtar_search();
    else
        XBell(XtDisplay(TopLevel), 50);
}
