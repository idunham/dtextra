/* --------------------------------------------------------------------- *
 * help.c                                                                *
 * --------------------------------------------------------------------- *
 * This file is part of the nebula irc client                            *
 * http://nebula-irc.sourceforge.net                                     *

 * Copyright (c) 2004-2005 Riccardo Mottola. All rights reserved.        *
 * mail: rmottola@users.sourceforge.net                                  *

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
Copyright (c) 2004-2005, Riccardo Mottola

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer. 
Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution. 
Neither the name of "Riccardo Mottola" nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS;´OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/Text.h>
#include <Xm/Form.h>

#include "main.h"
#include "help.h"

#ifndef XmFONTLIST_DEFAULT_TAG
#define XmFONTLIST_DEFAULT_TAG ""
#endif

extern systemStatusRecord sysState;
extern Widget aboutDlgBox;
extern Widget commandsHelpDlgBox;

void createAboutBox(Widget parent)
{
    XmString theXmString;
    XmString theXmString2;
    char     theString[256];
    Arg      args[2];
    Widget   remove;
    
    sprintf(theString,
    	"Nebula\n\
	an IRC Client.\n\n\
	version: %s\n\
	%s\n\n\
	Copyright 2004-2005 Riccardo Mottola\n\
	http://nebula-irc.sf.net\n\
	Nebula IRC is distributed under BSD.",\
	CLIENT_VERSION, sysState.systemInfo);
    theXmString = XM_STRING_GEN_COMP(theString);

    theXmString2 = XM_STRING_CREATE("About");
    XtSetArg(args[0], XmNdialogTitle, theXmString2);
    XtSetArg(args[1], XmNmessageString, theXmString);
    aboutDlgBox = XmCreateInformationDialog(parent, "about", args, 2);
    XmStringFree(theXmString);
    XmStringFree(theXmString2);
    
    remove = XmMessageBoxGetChild(aboutDlgBox, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(remove);
    remove = XmMessageBoxGetChild(aboutDlgBox, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(remove);
}


void displayAboutBox()
{
    XtManageChild(aboutDlgBox);
}

void createCommandsHelpBox(Widget parent)
{
    XmString theXmString;
    Arg      args[8];
    int      argcount;
    Widget   remove;
    Widget   textArea;
    Widget   form;

    argcount = 0;
    theXmString = XM_STRING_CREATE("Help");
    XtSetArg(args[argcount], XmNdialogTitle, theXmString); argcount++;
    XtSetArg(args[argcount], XmNautoUnmanage, True); argcount++;
    commandsHelpDlgBox = XmCreatePromptDialog(parent, "help", args, argcount);
    XmStringFree(theXmString);

    /* Now get rid of the things we don't want */
    remove = XmSelectionBoxGetChild(commandsHelpDlgBox, XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(remove);
    remove = XmSelectionBoxGetChild(commandsHelpDlgBox, XmDIALOG_TEXT);
    XtUnmanageChild(remove);
    remove = XmSelectionBoxGetChild(commandsHelpDlgBox, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(remove);
    remove = XmSelectionBoxGetChild(commandsHelpDlgBox, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(remove);

    form =  XtVaCreateWidget("form", xmFormWidgetClass, commandsHelpDlgBox, NULL);
    argcount = 0;
    XtSetArg(args[argcount], XmNeditable, False); argcount++;
    XtSetArg(args[argcount], XmNeditMode, XmMULTI_LINE_EDIT); argcount++;
    XtSetArg(args[argcount], XmNrows, 10); argcount++;
    XtSetArg(args[argcount], XmNcolumns, 50); argcount++;
    XtSetArg(args[argcount], XmNtopAttachment, XmATTACH_FORM); argcount++;
    XtSetArg(args[argcount], XmNleftAttachment, XmATTACH_FORM); argcount++;
    XtSetArg(args[argcount], XmNbottomAttachment, XmATTACH_FORM); argcount++;
    XtSetArg(args[argcount], XmNrightAttachment, XmATTACH_FORM); argcount++;
    textArea = XmCreateScrolledText(form, "output", args, argcount);

    if (textArea == NULL)
        perror("textArea: ");
    XtManageChild(textArea);

    XtManageChild(form);

    /* now add the help text */
    {
    	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/deop NICK: removes op (shortcut for mode)");
    	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/mode CHANNEL {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [USER] [BAN MASK] : change channel mode\n");
	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/mode USER {[+|-]|i|w|s|o} : change user mode\n");
	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/msg NICKNAME: sends private message (remember: channels are like users with # or & at the beginning)\n");
	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/nick NICKNAME: change nickname\n");
	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/ns command: shortcut for /msg nickserv command\n");
    	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/op NICK: gives op (shortcut for mode)\n");
	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/query NICKNAME: start private conversation (empty nick ends)\n");
	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/quit: quit nebula\n");
	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/server SERVERNAME: connect to server\n");
    	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/whois NICKNAME: get user information from server\n");
	
	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "\nCTCP commands\n");	
	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/ctcp NICKNAME version: requests client versioni\n");
	XmTextInsert(textArea,  XmTextGetLastPosition(textArea), "/me ACTION: sends an action (typically a message starting with the nickname)");
	XmTextSetInsertionPosition(textArea, 0);
    }
}


void displayCommandsHelpBox()
{
    XtManageChild(commandsHelpDlgBox);
}
