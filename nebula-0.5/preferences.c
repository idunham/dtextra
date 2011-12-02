/* --------------------------------------------------------------------- *
 * preferences.c                                                         *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>

#include "main.h"
#include "preferences.h"

#ifndef XmFONTLIST_DEFAULT_TAG
#define XmFONTLIST_DEFAULT_TAG ""
#endif

#define FILE_BUFF_SIZE 1024

extern systemStatusRecord sysState;
extern XtAppContext  nebulaIrcAppContext;

Widget startPref;
Widget startPrefDiag;

void okButtStartupCbk();
void helpButtStartupCbk();
void okButtPrefCbk();
void helpButtPrefCbk();

Widget       nickTextForm;
Widget       nameTextForm;
Widget       serverTextForm;
Widget       portTextForm;

void getStartupPrefs(Widget mainWin)
{
    Widget       prefForm;
    Widget       portLabel;
    Widget       child;
    XmString     labelString;
    Arg          args[4];
    int          argcount;
    FILE         *confFile;
    char         serverPortNum[6];
    char    	 *confFilePath;
    
    
    argcount = 0;
    labelString = XM_STRING_CREATE("Startup Prefs");
    XtSetArg(args[argcount], XmNdialogTitle, labelString); argcount++;
    XtSetArg(args[argcount], XmNautoUnmanage, True); argcount++;
    startPrefDiag = XmCreatePromptDialog(mainWin, "startup_prefs", args, argcount);
    XmStringFree(labelString);
    
    /* Now get rid of the things we don't want */
    child = XmSelectionBoxGetChild(startPrefDiag, XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(child);
    child = XmSelectionBoxGetChild(startPrefDiag, XmDIALOG_TEXT);
    XtUnmanageChild(child);

    prefForm = XtVaCreateWidget("form", xmFormWidgetClass, startPrefDiag,
				XmNfractionBase, 18, NULL);
    
    labelString = XM_STRING_CREATE("Name:");
    XtVaCreateManagedWidget("name_label",
    	    xmLabelWidgetClass, prefForm,
	    XmNlabelString, labelString,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 0,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
	    NULL);
    XmStringFree(labelString);
    
    labelString = XM_STRING_CREATE("Nick:");
    XtVaCreateManagedWidget("nick_label",
    	    xmLabelWidgetClass, prefForm,
	    XmNlabelString, labelString,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 6,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
	    NULL);
    XmStringFree(labelString);

    labelString = XM_STRING_CREATE("Server:");
    XtVaCreateManagedWidget("server_label",
    	    xmLabelWidgetClass, prefForm,
	    XmNlabelString, labelString,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 12,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
	    NULL);
    XmStringFree(labelString);
    

    nameTextForm = XtVaCreateManagedWidget("name_form",
    	    xmTextFieldWidgetClass, prefForm,
    	    XmNmaxLength, MAX_NAME_LEN,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 0,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 4,
	    XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 18,
	    NULL);

    nickTextForm = XtVaCreateManagedWidget("nick_form",
    	    xmTextFieldWidgetClass, prefForm,
    	    XmNmaxLength, MAX_NICK_LEN,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 6,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 4,
	    XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 18,
	    NULL);

    portTextForm = XtVaCreateManagedWidget("port_form",
    	    xmTextFieldWidgetClass, prefForm,
    	    XmNmaxLength, 5,
	    XmNcolumns, 5,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 12,
	    XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 18,
	    NULL); 

    labelString = XM_STRING_CREATE("Port: ");
    portLabel = XtVaCreateManagedWidget("port_label",
    	    xmLabelWidgetClass, prefForm,
	    XmNlabelString, labelString,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 12,
	    XmNrightAttachment, XmATTACH_WIDGET, XmNrightWidget, portTextForm,
	    NULL);
    XmStringFree(labelString);

    serverTextForm = XtVaCreateManagedWidget("server_form",
    	    xmTextFieldWidgetClass, prefForm,
    	    XmNmaxLength, MAX_SERVERNAME_LEN,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 12,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 4,
	    XmNrightAttachment, XmATTACH_WIDGET, XmNrightWidget, portLabel,
	    NULL);


    XtAddCallback(startPrefDiag, XmNokCallback, okButtStartupCbk, NULL);
    XtAddCallback(startPrefDiag, XmNhelpCallback, helpButtStartupCbk, NULL);

    XtManageChild(prefForm);
   
    XtManageChild(startPrefDiag);

    /* read configuration file */
    if (confFilePath = getPreferencesFilePath())
    {
    	confFile = fopen (confFilePath, "r");
    	if (confFile)
    	{
	    char buff[FILE_BUFF_SIZE];
	    char *pos;
	    
	    fgets(buff, FILE_BUFF_SIZE, confFile);
	    if (buff[strlen(buff)-1] == '\n')
	    	buff[strlen(buff)-1] = '\0';
	    if(!strncmp(buff, "DefaultNick", strlen("DefaultNick")))
		if (pos = strchr(buff, ' '))
		    strcpy(sysState.myNick, pos+1);
	    fgets(buff, FILE_BUFF_SIZE, confFile);
	    if (buff[strlen(buff)-1] == '\n')
	    	buff[strlen(buff)-1] = '\0';
	    if(!strncmp(buff, "DefaultName", strlen("DefaultName")))
		if (pos = strchr(buff, ' '))
		    strcpy(sysState.myName, pos+1);
	    fgets(buff, FILE_BUFF_SIZE, confFile);
	    if (buff[strlen(buff)-1] == '\n')
	    	buff[strlen(buff)-1] = '\0';
	    if(!strncmp(buff, "DefaultServer", strlen("DefaultServer")))
		if (pos = strchr(buff, ' '))
		    strcpy(sysState.serverName, pos+1);
	    fgets(buff, FILE_BUFF_SIZE, confFile);
	    if (buff[strlen(buff)-1] == '\n')
	    	buff[strlen(buff)-1] = '\0';
	    if(!strncmp(buff, "DefaultPortNum", strlen("DefaultPortNum")))
		if (pos = strchr(buff, ' '))
		    strcpy(serverPortNum, pos+1);
	    fclose(confFile);
    	} else
    	{
    	    printf("config file not found.\n");
    	    strcpy(sysState.myNick, "nebula_user");
	    strcpy(sysState.myName, "Nebula User");
	    strcpy(sysState.serverName, "irc.freenode.org");
	    strcpy(serverPortNum, "6666");
    	}
    	free(confFilePath);
    }
    XmTextSetString(nickTextForm, sysState.myNick);
    XmTextSetString(nameTextForm, sysState.myName);
    XmTextSetString(serverTextForm, sysState.serverName);
    XmTextSetString(portTextForm, serverPortNum);
}

void okButtStartupCbk()
{
    char *nick;
    char *name;
    char *server;
    char *port;
    char theServer[MAX_SERVERNAME_LEN];
    
    nick = XmTextGetString(nickTextForm);
    name = XmTextGetString(nameTextForm);
    server = XmTextGetString(serverTextForm);
    port = XmTextGetString(portTextForm);
    if (strlen(nick) > 0 && strlen(nick) <= MAX_NICK_LEN)
    	strcpy(sysState.myNick, nick);
    else
    	strcpy(sysState.myNick, "nebulauser");
    if (strlen(name) > 0 && strlen(name) <= MAX_NAME_LEN)
    	strcpy(sysState.myName, name);
    else
    	strcpy(sysState.myName, "nebula user");
    if (strlen(server) > 0 &&strlen(server) <= MAX_SERVERNAME_LEN)
    	strcpy(theServer, server);
    else
    	strcpy(theServer, "irc.freenode.org");
    printf("port: %s, %d\n", port, atoi(port));
    if (strlen(port) > 0 && strlen(port) <= 5)
    {
	sysState.port = atoi(port);
    	if (sysState.port <= 0)
	    sysState.port = 6666;
    } else
    	sysState.port = 6666;

    connectToServer(theServer, sysState.port);
}

void helpButtStartupCbk()
{
    XmString theXmString, theXmString2;
    Arg      args[8];
    int      argcount;
    Widget   remove;
    Widget   startupPrefsHelpDlgBox;
    
    argcount = 0;
    theXmString = XM_STRING_CREATE("Help");
    XtSetArg(args[argcount], XmNdialogTitle, theXmString); argcount++;
    theXmString2 = XM_STRING_GEN_COMP("Set nickname and name you want to use.\n\nEnter the server you want to connect to.");
    XtSetArg(args[argcount], XmNmessageString, theXmString2); argcount++;
    XtSetArg(args[argcount], XmNautoUnmanage, True); argcount++;
    startupPrefsHelpDlgBox = XmCreateInformationDialog(startPrefDiag, "help", args, argcount);
    XmStringFree(theXmString);
    XmStringFree(theXmString2);
    
    /* Now get rid of the things we don't want */
    remove = XmMessageBoxGetChild(startupPrefsHelpDlgBox, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(remove);
    remove = XmMessageBoxGetChild(startupPrefsHelpDlgBox, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(remove);
    XtManageChild(startupPrefsHelpDlgBox);
}


/* ---- set  preferences ----- */

void setPreferences(Widget mainWin)
{
    Widget       prefForm;
    Widget       portLabel;
    Widget       child;
    XmString     labelString;
    Arg          args[4];
    int          argcount;
    FILE         *confFile;
    char         serverPortNum[6];
    char    	 *confFilePath;
    char         serverName[MAX_SERVERNAME_LEN];
    char         myNick[MAX_NICK_LEN];
    char         myName[MAX_NAME_LEN];    
    
    argcount = 0;
    labelString = XM_STRING_CREATE("Startup Prefs");
    XtSetArg(args[argcount], XmNdialogTitle, labelString); argcount++;
    XtSetArg(args[argcount], XmNautoUnmanage, True); argcount++;
    startPrefDiag = XmCreatePromptDialog(mainWin, "startup_prefs", args, argcount);
    XmStringFree(labelString);
    
    /* Now get rid of the things we don't want */
    child = XmSelectionBoxGetChild(startPrefDiag, XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(child);
    child = XmSelectionBoxGetChild(startPrefDiag, XmDIALOG_TEXT);
    XtUnmanageChild(child);

    prefForm = XtVaCreateWidget("form", xmFormWidgetClass, startPrefDiag,
				XmNfractionBase, 18, NULL);
    
    labelString = XM_STRING_CREATE("Name:");
    XtVaCreateManagedWidget("name_label",
    	    xmLabelWidgetClass, prefForm,
	    XmNlabelString, labelString,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 0,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
	    NULL);
    XmStringFree(labelString);
    
    labelString = XM_STRING_CREATE("Nick:");
    XtVaCreateManagedWidget("nick_label",
    	    xmLabelWidgetClass, prefForm,
	    XmNlabelString, labelString,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 6,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
	    NULL);
    XmStringFree(labelString);

    labelString = XM_STRING_CREATE("Server:");
    XtVaCreateManagedWidget("server_label",
    	    xmLabelWidgetClass, prefForm,
	    XmNlabelString, labelString,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 12,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
	    NULL);
    XmStringFree(labelString);
    

    nameTextForm = XtVaCreateManagedWidget("name_form",
    	    xmTextFieldWidgetClass, prefForm,
    	    XmNmaxLength, MAX_NAME_LEN,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 0,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 4,
	    XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 18,
	    NULL);

    nickTextForm = XtVaCreateManagedWidget("nick_form",
    	    xmTextFieldWidgetClass, prefForm,
    	    XmNmaxLength, MAX_NICK_LEN,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 6,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 4,
	    XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 18,
	    NULL);

    portTextForm = XtVaCreateManagedWidget("port_form",
    	    xmTextFieldWidgetClass, prefForm,
    	    XmNmaxLength, 5,
	    XmNcolumns, 5,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 12,
	    XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 18,
	    NULL); 

    labelString = XM_STRING_CREATE("Port: ");
    portLabel = XtVaCreateManagedWidget("port_label",
    	    xmLabelWidgetClass, prefForm,
	    XmNlabelString, labelString,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 12,
	    XmNrightAttachment, XmATTACH_WIDGET, XmNrightWidget, portTextForm,
	    NULL);
    XmStringFree(labelString);

    serverTextForm = XtVaCreateManagedWidget("server_form",
    	    xmTextFieldWidgetClass, prefForm,
    	    XmNmaxLength, MAX_SERVERNAME_LEN,
	    XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 12,
	    XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 4,
	    XmNrightAttachment, XmATTACH_WIDGET, XmNrightWidget, portLabel,
	    NULL);


    XtAddCallback(startPrefDiag, XmNokCallback, okButtPrefCbk, NULL);
    XtAddCallback(startPrefDiag, XmNhelpCallback, helpButtPrefCbk, NULL);

    XtManageChild(prefForm);
   
    XtManageChild(startPrefDiag);

    /* read configuration file */
    if (confFilePath = getPreferencesFilePath())
    {
    	confFile = fopen (confFilePath, "r");
    	if (confFile)
    	{
	    char buff[FILE_BUFF_SIZE];
	    char *pos;
	    
	    fgets(buff, FILE_BUFF_SIZE, confFile);
	    if (buff[strlen(buff)-1] == '\n')
	    	buff[strlen(buff)-1] = '\0';
	    if(!strncmp(buff, "DefaultNick", strlen("DefaultNick")))
		if (pos = strchr(buff, ' '))
		    strcpy(myNick, pos+1);
	    fgets(buff, FILE_BUFF_SIZE, confFile);
	    if (buff[strlen(buff)-1] == '\n')
	    	buff[strlen(buff)-1] = '\0';
	    if(!strncmp(buff, "DefaultName", strlen("DefaultName")))
		if (pos = strchr(buff, ' '))
		    strcpy(myName, pos+1);
	    fgets(buff, FILE_BUFF_SIZE, confFile);
	    if (buff[strlen(buff)-1] == '\n')
	    	buff[strlen(buff)-1] = '\0';
	    if(!strncmp(buff, "DefaultServer", strlen("DefaultServer")))
		if (pos = strchr(buff, ' '))
		    strcpy(serverName, pos+1);
	    fgets(buff, FILE_BUFF_SIZE, confFile);
	    if (buff[strlen(buff)-1] == '\n')
	    	buff[strlen(buff)-1] = '\0';
	    if(!strncmp(buff, "DefaultPortNum", strlen("DefaultPortNum")))
		if (pos = strchr(buff, ' '))
		    strcpy(serverPortNum, pos+1);
	    fclose(confFile);
    	} else
    	{
    	    printf("config file not found.\n");
    	    strcpy(myNick, "nebula_user");
	    strcpy(myName, "Nebula User");
	    strcpy(serverName, "irc.freenode.org");
	    strcpy(serverPortNum, "6666");
    	}
    	free(confFilePath);
    }
    XmTextSetString(nickTextForm, myNick);
    XmTextSetString(nameTextForm, myName);
    XmTextSetString(serverTextForm, serverName);
    XmTextSetString(portTextForm, serverPortNum);
}

void okButtPrefCbk()
{
    char *nick;
    char *name;
    char *server;
    char *port;
    char theServer[MAX_SERVERNAME_LEN];
    char myNick[MAX_NICK_LEN];
    char myName[MAX_NAME_LEN];
    char serverPort[6];
    FILE *confFile;
    char *confFilePath;
    
    nick = XmTextGetString(nickTextForm);
    name = XmTextGetString(nameTextForm);
    server = XmTextGetString(serverTextForm);
    port = XmTextGetString(portTextForm);
    if (strlen(nick) > 0 && strlen(nick) <= MAX_NICK_LEN)
    	strcpy(myNick, nick);
    else
    	strcpy(myNick, "nebulauser");
    if (strlen(name) > 0 && strlen(name) <= MAX_NAME_LEN)
    	strcpy(myName, name);
    else
    	strcpy(myName, "nebula user");
    if (strlen(server) > 0 &&strlen(server) <= MAX_SERVERNAME_LEN)
    	strcpy(theServer, server);
    else
    	strcpy(theServer, "irc.freenode.org");
    if (strlen(port) > 0 && strlen(port) <= 5)
    	strcpy(serverPort, port);
    else
    	strcpy(serverPort, "6666");
    
    /* write out configuration file */
    if(confFilePath = getPreferencesFilePath())
    {
    	confFile = fopen (confFilePath, "w");
    	if (confFile)
    	{
	    fprintf(confFile, "DefaultNick: %s\n", myNick);
	    fprintf(confFile, "DefaultName: %s\n", myName);
	    fprintf(confFile, "DefaultServer: %s\n", theServer);
	    fprintf(confFile, "DefaultPortNum: %s\n", port);
	    fclose(confFile);
	}
    	free(confFilePath);
    }
}

void helpButtPrefCbk()
{
    XmString theXmString, theXmString2;
    Arg      args[8];
    int      argcount;
    Widget   remove;
    Widget   startupPrefsHelpDlgBox;
    
    argcount = 0;
    theXmString = XM_STRING_CREATE("Help");
    XtSetArg(args[argcount], XmNdialogTitle, theXmString); argcount++;
    theXmString2 = XM_STRING_GEN_COMP("This panel sets the startup preferences.\n\
Ok will save them to the user\'s defaults.\n\n\
Set nickname and name you want to use.\nEnter the server you want to connect to.");
    XtSetArg(args[argcount], XmNmessageString, theXmString2); argcount++;
    XtSetArg(args[argcount], XmNautoUnmanage, True); argcount++;
    startupPrefsHelpDlgBox = XmCreateInformationDialog(startPrefDiag, "help", args, argcount);
    XmStringFree(theXmString);
    XmStringFree(theXmString2);
    
    /* Now get rid of the things we don't want */
    remove = XmMessageBoxGetChild(startupPrefsHelpDlgBox, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(remove);
    remove = XmMessageBoxGetChild(startupPrefsHelpDlgBox, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(remove);
    XtManageChild(startupPrefsHelpDlgBox);
}

/*
 * Return the full path to the user preferences file
 * the caller is responsible of freeing the string if the call was successful
 * failure will return a NULL pointer
 */
char* getPreferencesFilePath()
{
    char *homeDirPath;
    char *filePath;

    filePath = NULL;
    homeDirPath = getenv("HOME");
    if (homeDirPath)
    {
    	filePath = malloc ((strlen(homeDirPath) + strlen(PREF_FILE_NAME) + 1) * sizeof(char)); /* 1 is for the slash */
	if (filePath == NULL)
    	{
	    perror("Could not allocate string buffer for config file Path");
	} else
    	{
    	    strcpy(filePath, homeDirPath);
    	    strcat(filePath, "/");
	    strcat(filePath, PREF_FILE_NAME);
    	}
    } else
    {
    	perror("Could not determine home path of the user.");
    }
    return filePath;
}
