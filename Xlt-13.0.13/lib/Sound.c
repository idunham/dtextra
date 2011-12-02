/**
 *
 * $Id: Sound.c,v 1.8 2001/06/02 09:27:45 amai Exp $
 *
 * Copyright (C) 1996 Free Software Foundation, Inc.
 * Copyright © 1999-2001 by the LessTif developers.
 *
 * This file is part of the GNU LessTif Extension Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **/

#include "../config.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <X11/IntrinsicP.h>
#include <Xm/MessageB.h>
#include <Xm/ToggleB.h>
#include <Xm/FileSB.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/Form.h>

#include "Sound.h"
#include "BubbleButton.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif


static void PlaySound(Widget W, XEvent *event, String *SoundFiles, Cardinal *NumSounds);
static void EnableSound(void);
static void DisableSound(void);


typedef struct
{
    Boolean Sounds;
    String SoundCommand;
    String DefaultHelpString;
}
AppResources_t, *AppResourcesPtr;

static AppResources_t AppResources;
static XmSearchProc default_file_search;

static char DefaultHelp[] = "\n\
The Sound Setup dialog allows you to enable or disable\n\
sounds in this application, as well as select the command\n\
used to play sounds. The command will be invoked with 1\n\
argument, the sound file to play.  The application resource\n\
*SoundCommand can be used to specify a string to be used as\n\
the command and, *Sounds can be used to enable or disable sounds.\n\
";

static XtResource resources[] =
{
    {"SoundCommand", "SoundCommand", XtRString, sizeof(String), XtOffset(AppResourcesPtr, SoundCommand), XtRImmediate, NULL},
    {"Sounds", "Sounds", XtRBoolean, sizeof(Boolean), XtOffset(AppResourcesPtr, Sounds), XtRImmediate, (void *)True},
};

static XtActionsRec GlobalActions[] =
{
    {"SetupSound", (XtActionProc)XltSoundSetup},
    {"PlaySound", (XtActionProc)PlaySound},
    {"EnableSound", (XtActionProc)EnableSound},
    {"DisableSound", (XtActionProc)DisableSound},
};

/* ***** */

Boolean *
XltSoundInitialize(Widget W)
{
    XtGetApplicationResources(W,
			      &AppResources,
			      resources,
			      XtNumber(resources),
			      NULL, 0);
    if (AppResources.SoundCommand == NULL)
    {
	AppResources.SoundCommand = XtNewString("");
    }
    XtAppAddActions(XtWidgetToApplicationContext(W),
		    GlobalActions, XtNumber(GlobalActions));
    return (&AppResources.Sounds);
}

/* ***** */

static void
_SoundSetup(Widget W)
{
    String Name;
    String Class;
    FILE *xrdb;
    char *xrdb_command = "xrdb -merge -nocpp -display ";

    AppResources.Sounds = XmToggleButtonGetState(XtNameToWidget(W, "*Enable"));
    /* HP does not like this
    XtFree(AppResources.SoundCommand);
    */
    AppResources.SoundCommand = XmTextFieldGetString(XtNameToWidget(W, "*Text"));
    xrdb_command = XtNewString(xrdb_command);
    xrdb_command = XtRealloc(xrdb_command, strlen(xrdb_command) + strlen(DisplayString(XtDisplay(W))) + 1);
    strcat(xrdb_command, DisplayString(XtDisplay(W)));
    fflush(stdout);
    xrdb = popen(xrdb_command, "w");
    if (xrdb != NULL)
    {
	XtGetApplicationNameAndClass(XtDisplay(W), &Name, &Class);
	fprintf(xrdb, "%s.Sounds: %s\n%s.SoundCommand: %s\n",
		Class,
		AppResources.Sounds ? "True" : "False",
		Class,
		AppResources.SoundCommand);
	pclose(xrdb);
    }
    else
    {
	fprintf(stderr, "Could not execute >%s<\n", xrdb_command);
    }
    XtFree(xrdb_command);
}

/* ***** */

static void
StuffText(Widget W, Widget Target)
{
    String Text;

    Text = XmTextFieldGetString(XmFileSelectionBoxGetChild(W, XmDIALOG_TEXT));
    XmTextFieldSetString(Target, Text);
    XtFree(Text);
    XtUnmanageChild(W);
}

/* ***** */

static void
file_search(Widget fsb, XmFileSelectionBoxCallbackStruct *cbs)
{
	char           *filename;
	XmString       *default_files;
	int             default_file_count;
	XmString       *writable_files = NULL;
	int             writable_file_count = 0;
	int             i;
	/*
	 * Use default file search proc to do all the wildcard matching and
	 * so on, then remove unwritable files from its list. Even if the
	 * default proc doesn't change the file list, we still need to check
	 * in case permissions have changed.
	 */
	(*default_file_search) (fsb, (XtPointer)cbs);

	/* Get the list of filenames */
	XtVaGetValues(fsb,
		      XmNfileListItems, &default_files,
		      XmNfileListItemCount, &default_file_count,
		      NULL);
	/* Run down the list, copying the names of all writable files */
	writable_files = (XmString *) XtMalloc(default_file_count *
					       sizeof(XmString));
	for (i = 0; i < default_file_count; i++) {
		XmStringGetLtoR(default_files[i], XmFONTLIST_DEFAULT_TAG,
				&filename);
		if (access(filename, X_OK) == 0)
			writable_files[writable_file_count++] =
				XmStringCopy(default_files[i]);
		XtFree(filename);
	}

	XtVaSetValues(fsb,
	    XmNfileListItems, writable_files,
	    XmNfileListItemCount, writable_file_count,
	    XmNlistUpdated, True,
	    NULL);

	for (i = 0; i < writable_file_count; i++)
		XmStringFree(writable_files[i]);
	XtFree((char *)writable_files);
}

/* ***** */

static void
Browse(Widget W, Widget Target)
{
    static Widget Dialog;

    if (Dialog == NULL)
    {
	while (!XtIsTopLevelShell(W)) W = XtParent(W);
	Dialog = XmCreateFileSelectionDialog(W, "Browse", NULL, 0);
	XtAddCallback(Dialog, XmNcancelCallback, (XtCallbackProc)XtUnmanageChild, NULL);
	XtAddCallback(Dialog, XmNokCallback, (XtCallbackProc)StuffText, Target);
	XtVaGetValues(Dialog,
		XmNfileSearchProc, &default_file_search,
		NULL);
	XtVaSetValues(Dialog,
		XmNfileSearchProc, file_search,
		NULL);
	XmFileSelectionDoSearch(Dialog, NULL);
    }
    XtManageChild(Dialog);
}

/* ***** */

void
XltHelpOnSound(Widget W)
{
static Widget Dialog = NULL;
Widget Shell = W;

	while (!XtIsShell(Shell)) Shell = XtParent(Shell);
	if (Dialog == NULL)
	{
	XmString string;
	XtResource resources[] =
	{
	    {"messageString", "MessageString", XtRString, sizeof(String), XtOffset(AppResourcesPtr, DefaultHelpString), XtRImmediate, DefaultHelp},
	};
	    Dialog = XmCreateInformationDialog(Shell, "HelpOnSound", NULL, 0);
	    XtGetSubresources(W,
			      &AppResources,
			      XtName(Dialog),
			      XtClass(Dialog)->core_class.class_name,
			      resources,
			      XtNumber(resources),
			      NULL, 0);
	    string = XmStringCreateLtoR(AppResources.DefaultHelpString, XmFONTLIST_DEFAULT_TAG);
	    XtVaSetValues(Dialog,
		    XmNmessageString, string,
		    NULL);
	    XmStringFree(string);
	    XtUnmanageChild(XmMessageBoxGetChild(Dialog, XmDIALOG_HELP_BUTTON));
	}
	XtManageChild(Dialog);
}

/* ***** */

void
XltSoundSetup(Widget W)
{
    static Widget Dialog = NULL;
    Widget TopLevelShell = W;

    if (Dialog == NULL)
    {
	Widget Label;
	Widget Text;
	Widget Toggle;
	Widget Form;
	Widget Form1;
	Widget Push;

	while (!XtIsTopLevelShell(TopLevelShell))
	    TopLevelShell = XtParent(TopLevelShell);
	Dialog = XmCreateMessageDialog(TopLevelShell, "SoundSetup", NULL, 0);
	Form = XmCreateForm(Dialog, "SoundSetupForm", NULL, 0);
	Form1 = XmCreateForm(Form, "SoundSetupForm1", NULL, 0);
	XtVaSetValues(Form1,
		      XmNleftAttachment, XmATTACH_FORM,
		      XmNrightAttachment, XmATTACH_FORM,
		      NULL);
	Label = XmCreateLabel(Form1, "SoundCommand", NULL, 0);
	Text = XmCreateTextField(Form1, "Text", NULL, 0);
	Push = XltCreateBubbleButton(Form1, "Browse", NULL, 0);
	Toggle = XmCreateToggleButton(Form, "Enable", NULL, 0);
	XtAddCallback(Push, XmNactivateCallback, (XtCallbackProc)Browse, Text);

	XtVaSetValues(Label,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNbottomAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_FORM,
		      NULL);
	XtVaSetValues(Push,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNbottomAttachment, XmATTACH_FORM,
		      XmNrightAttachment, XmATTACH_FORM,
		      NULL);
	XtVaSetValues(Text,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_WIDGET,
		      XmNleftWidget, Label,
		      XmNrightAttachment, XmATTACH_WIDGET,
		      XmNrightWidget, Push,
		      NULL);
	XtVaSetValues(Toggle,
		      XmNtopAttachment, XmATTACH_WIDGET,
		      XmNtopWidget, Form1,
		      XmNleftAttachment, XmATTACH_FORM,
		      NULL);

	XtManageChild(Text);
	XtManageChild(Label);
	XtManageChild(Push);
	XtManageChild(Form1);
	XtManageChild(Toggle);
	XtManageChild(Form);
	XtAddCallback(Dialog, XmNokCallback, (XtCallbackProc)_SoundSetup, NULL);
	XtAddCallback(Dialog, XmNhelpCallback, (XtCallbackProc)XltHelpOnSound, NULL);
    }
    XmToggleButtonSetState(XtNameToWidget(Dialog, "*Enable"), AppResources.Sounds, False);
    XmTextFieldSetString(XtNameToWidget(Dialog, "*Text"), AppResources.SoundCommand);
    XtManageChild(Dialog);
}

/* ***** */

void
XltSoundEnable(Boolean State)
{
    AppResources.Sounds = State;
}

/* ***** */

static void
EnableSound(void)
{
    AppResources.Sounds = True;
}

/* ***** */

static void
DisableSound(void)
{
    AppResources.Sounds = False;
}

/* ***** */

static void
PlaySound(Widget W, XEvent *event, String *SoundFiles, Cardinal *NumSounds)
{
    Cardinal i;

    for (i = 0; i < *NumSounds; i++)
    {
	if (AppResources.Sounds &&
	    (SoundFiles[i] != NULL) &&
	    (AppResources.SoundCommand != NULL))
	{
	    if ((strlen(SoundFiles[i]) > 0) &&
		(strlen(AppResources.SoundCommand) > 0))
	    {
		switch (fork())
		{
		case 0:
		    /*
		       close(0);
		       close(1);
		       close(2);
		     */
		    execlp(AppResources.SoundCommand, AppResources.SoundCommand, SoundFiles[i], (char *)0);
		    fprintf(stderr, "%s(%i):PlaySound() - Error exec'ing sound command >%s<\n",
			    __FILE__, __LINE__,
			    AppResources.SoundCommand);
		    perror("PlaySound()");
		    _exit(0);
		    break;
		case -1:
		    fprintf(stderr, "%s(%i):PlaySound() - Error forking sound command >%s<\n",
			    __FILE__, __LINE__,
			    AppResources.SoundCommand);
		    perror("PlaySound()");
		    break;
		default:
		    signal(SIGCHLD, SIG_IGN);
		    break;
		}
	    }
	}
    }
}
