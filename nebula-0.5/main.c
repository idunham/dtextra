/* --------------------------------------------------------------------- *
 * main.c                                                                *
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

#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <errno.h>

#include "main.h"

#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/CascadeB.h>
#include <Xm/PanedW.h>
#include <Xm/List.h>

#include "interpreter.h"
#include "help.h"
#include "preferences.h"
#include "network.h"
#include "filesave.h"

#ifndef XmFONTLIST_DEFAULT_TAG
#define XmFONTLIST_DEFAULT_TAG ""
#endif

void fileMenuCall(Widget w, XtPointer clientData, XtPointer callData);
void userMenuCall(Widget w, XtPointer clientData, XtPointer callData);
void helpMenuCall(Widget w, XtPointer clientData, XtPointer callData);
void userListSelectCbk(Widget w, XtPointer data, XtPointer callData);
void commandInputCall();

Widget mainWindow;

Widget commandInput;
Widget outputArea;
Widget userList;

Widget aboutDlgBox;
Widget commandsHelpDlgBox;

XtAppContext  nebulaIrcAppContext;
XtInputId     nebulaNetworkEventId;

char *selectedUser;


/* holds global information about the system */
systemStatusRecord sysState;

int main (int argc, char *argv[])
{
    Widget topWidget;
    Widget mainForm;
    Widget userForm;

    /* Menu Stuff */
    Widget menuBar, helpMenu;

    static String fallback_resources[] = { 
    	"*sgiMode: True",
    	"*useSchemes: all",
    	NULL }; 

    topWidget = XtVaAppInitialize(&nebulaIrcAppContext, "Nebula IRC",
	    NULL, 0, /* no command line options */
	    &argc, argv,
	    fallback_resources, /* fallback resource list */
	    NULL);

    mainWindow = XtVaCreateManagedWidget("main_window",
    	xmMainWindowWidgetClass, topWidget,
    	XmNcommandWindowLocation, XmCOMMAND_BELOW_WORKSPACE,
	XmNwidth, 600,
	XmNheight, 500,
    	NULL);

    /* create menu bar */
    {
	XmString fileMenuStr, userMenuStr, helpMenuStr;
	
    	fileMenuStr = XM_STRING_CREATE("File");
	userMenuStr = XM_STRING_CREATE("User");
    	helpMenuStr = XM_STRING_CREATE("Help");

    	menuBar = XmVaCreateSimpleMenuBar(mainWindow, "menubar",
    	    XmVaCASCADEBUTTON, fileMenuStr, 'F',
	    XmVaCASCADEBUTTON, userMenuStr, 'U',
    	    NULL);

    	XmStringFree(fileMenuStr);
	XmStringFree(userMenuStr);
    	XmStringFree(helpMenuStr);
    }
    
    /* File Menu */
    {
    	XmString prefStr;
    	XmString saveConvStr;
	XmString quitStr;
    	
    	prefStr = XM_STRING_CREATE("Preferences...");
	saveConvStr = XM_STRING_CREATE("Save Conversation...");
    	quitStr = XM_STRING_CREATE("Quit");
    	XmVaCreateSimplePulldownMenu(menuBar, "file_menu", 0,
    	    fileMenuCall,
	    XmVaPUSHBUTTON, prefStr, 'P', NULL, NULL,
	    XmVaPUSHBUTTON, saveConvStr, 'S', NULL, NULL,
    	    XmVaPUSHBUTTON, quitStr, 'Q', NULL, NULL,
    	    NULL);
    	XmStringFree(prefStr);
	XmStringFree(saveConvStr);
	XmStringFree(quitStr);
    }
    /* User Menu */
    {
    	XmString whoisStr;
    	XmString versionStr;
	XmString opStr;
	XmString deopStr;
    	
    	whoisStr = XM_STRING_CREATE("Whois");
	versionStr = XM_STRING_CREATE("Client version");
    	opStr = XM_STRING_CREATE("Op");
	deopStr = XM_STRING_CREATE("Deop");
    	XmVaCreateSimplePulldownMenu(menuBar, "user_menu", 1,
    	    userMenuCall,
	    XmVaPUSHBUTTON, whoisStr, 'W', NULL, NULL,
	    XmVaPUSHBUTTON, versionStr, '\0', NULL, NULL,
    	    XmVaPUSHBUTTON, opStr, 'O', NULL, NULL,
	    XmVaPUSHBUTTON, deopStr, 'D', NULL, NULL,
    	    NULL);
    	XmStringFree(whoisStr);
	XmStringFree(versionStr);
	XmStringFree(opStr);
	XmStringFree(deopStr);
    }
    /* Help Menu */
    {
	XmString helpCommandStr;
	XmString aboutStr;
	
	helpCommandStr = XM_STRING_CREATE("Commands...");
	aboutStr = XM_STRING_CREATE("About...");
   	helpMenu = XtVaCreateManagedWidget("Help",
    	    xmCascadeButtonWidgetClass, menuBar,
    	    XmNmnemonic, 'H',
    	    NULL);
   	XmVaCreateSimplePulldownMenu(menuBar, "Help", 2,
    	    helpMenuCall,
    	    XmVaPUSHBUTTON, aboutStr, 'A', NULL, NULL,
	    XmVaPUSHBUTTON, helpCommandStr, 'C', NULL, NULL,
    	    NULL);
	XmStringFree(helpCommandStr);
	XmStringFree(aboutStr);

	/* now set the help button */
    	XtVaSetValues(menuBar, XmNmenuHelpWidget, helpMenu, NULL);
    }
    
    /* now manage the menu bar */ 
    XtManageChild(menuBar);
    
    /* create a form */
    mainForm = XtVaCreateWidget("form", xmFormWidgetClass, mainWindow, XmNfractionBase, 3, NULL);

    /* vertical split user form */
    userForm = XtVaCreateWidget("userform", xmFormWidgetClass, mainForm,
    	    XmNfractionBase, 5,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    NULL);
    
    /* output area */
    {
   	Arg args[12];

    	XtSetArg(args[0], XmNeditable, False);
	XtSetArg(args[1], XmNeditMode, XmMULTI_LINE_EDIT);
	XtSetArg(args[2], XmNscrollHorizontal, False);
	XtSetArg(args[3], XmNwordWrap, True);
	XtSetArg(args[4], XmNtopAttachment, XmATTACH_POSITION);
	XtSetArg(args[5], XmNtopPosition, 0);
	XtSetArg(args[6], XmNbottomAttachment, XmATTACH_POSITION);
	XtSetArg(args[7], XmNbottomPosition, 5);
	XtSetArg(args[8], XmNleftAttachment, XmATTACH_POSITION);
	XtSetArg(args[9], XmNleftPosition, 0);
	XtSetArg(args[10], XmNrightAttachment, XmATTACH_POSITION);
	XtSetArg(args[11], XmNrightPosition, 4);
    	outputArea = XmCreateScrolledText(userForm, "output", args, 12);
	
	if (outputArea == NULL)
	    perror("outputArea: ");
	XtManageChild(outputArea);
    }
    
    /* user list at left */
    {
    	Arg args[9];
	
    	XtSetArg(args[0], XmNtopAttachment, XmATTACH_POSITION);
	XtSetArg(args[1], XmNtopPosition, 0);
	XtSetArg(args[2], XmNbottomAttachment, XmATTACH_POSITION);
	XtSetArg(args[3], XmNbottomPosition, 5);
	XtSetArg(args[4], XmNleftAttachment, XmATTACH_POSITION);
	XtSetArg(args[5], XmNleftPosition, 4);
	XtSetArg(args[6], XmNrightAttachment, XmATTACH_POSITION);
	XtSetArg(args[7], XmNrightPosition, 5);
	XtSetArg(args[8], XmNselectionPolicy, XmSINGLE_SELECT);
    	userList = XmCreateScrolledList(userForm, "list", args, 9);
	XtAddCallback(userList, XmNsingleSelectionCallback, userListSelectCbk, NULL);
	XtManageChild(userList);
    }
    XtManageChild(userForm);
    XtManageChild(mainForm);
    
    /* Command line area */
    commandInput = XtVaCreateWidget("command",
    	xmTextFieldWidgetClass, mainWindow,
    	XmNmaxLength, MAX_INPUT_LEN,
    	NULL);
    XtAddCallback(commandInput, XmNactivateCallback, commandInputCall, NULL);
    XtManageChild(commandInput);
    
    XtVaSetValues(mainWindow,
	    XmNmenuBar, menuBar,
	    XmNworkWindow, mainForm,
	    XmNcommandWindow, commandInput,
	    NULL);
    
    XtManageChild(mainWindow);
    XtRealizeWidget(topWidget);

    /* initialize System Status */
    sysState.isConnected = NO;
    
    /* get some preferences */
    getStartupPrefs(mainWindow);
    
    /* set focus to input area */
    /* For Motif 1.0 we should use _XmGrabTheFocus(target) according to FAQ */
    XmProcessTraversal(commandInput, XmTRAVERSE_CURRENT);
    
    /* initialize hostname and system info */
    /* should switch back to ARCHOS if uname not available */
    {
    	struct utsname sysInfo;
	char           tempName[2*SYSINFO_LEN];
	
    	gethostname(sysState.myHostName, MAXHOSTNAMELEN);
    	uname(&sysInfo);
	sprintf(tempName, "%s %s running on %s", sysInfo.sysname, sysInfo.release, sysInfo.machine);
	tempName[SYSINFO_LEN] = '\0';
	strcpy(sysState.systemInfo, tempName);
    }
    
    /* get the user login name from unix */
    {
        char *tempName;

        tempName = getenv("USER");
        if (tempName == NULL)
            tempName = getenv("LOGNAME");
        if (strlen(tempName) > MAX_NAME_LEN || tempName == NULL)
        {
            strcpy(sysState.myUnixname, "joesmith");
            printf("Failed to get a proper Unix login name. Using: %s\n", sysState.myUnixname); 
        }
        strcpy(sysState.myUnixname, tempName);
    }
 
    /* create unmanaged dialogs */
    createAboutBox(mainWindow);
    createCommandsHelpBox(mainWindow);  
    
    while(1)
    {
    	XEvent theEvent;
	XtAppNextEvent(nebulaIrcAppContext, &theEvent);
	XtDispatchEvent(&theEvent);
    }
}


/* saves the text in the conversation window */
void fileMenuCall(Widget w, XtPointer clientData, XtPointer callData)
{
    int itemNum;

    itemNum = (int) clientData;
    switch (itemNum)
    {
    case 0:
	setPreferences(mainWindow);
	break;
    case 1:
	saveFile(mainWindow);
	break;
    case 2:
        quitApplication();
	break;
    default:
    	break;
    }
}

void helpMenuCall(Widget w, XtPointer clientData, XtPointer callData)
{
    int itemNum;
    
    itemNum = (int) clientData;
    switch (itemNum)
    {
    case 0:
	displayAboutBox();
	break;
    case 1:
	displayCommandsHelpBox();
	break;
    default:
    	break;
    }
}

void userMenuCall(Widget w, XtPointer clientData, XtPointer callData)
{
    int itemNum;
    
#ifdef DEBUG    
    printf("user: %s\n", selectedUser);
#endif
    if (selectedUser == NULL)
    	return;
    
    itemNum = (int) clientData;
    switch (itemNum)
    {
    case 0:
    	printf("Whois!\n");
	doWhois(selectedUser);
	break;
    case 1:
    	printf("version!\n");
	doCTCPVersion(selectedUser);
	break;
    case 2:
    	printf("op!\n");
	doMode(sysState.currentChannel, "+o", selectedUser, NULL, NULL);
	break;
    case 3:
    	printf("deop!\n");
	doMode(sysState.currentChannel, "-o", selectedUser, NULL, NULL);
	break;
    default:
    	break;
    }
}

void userListSelectCbk(Widget w, XtPointer data, XtPointer callData)
{
    XmListCallbackStruct *listCbs;
    
    listCbs = (XmListCallbackStruct *)callData;
    XmStringGetLtoR(listCbs->item, XmFONTLIST_DEFAULT_TAG, &selectedUser);
}

void commandInputCall()
{
    char *command;
    
    command = XmTextGetString(commandInput);
    interpretCommand(command);
    XmTextSetString(commandInput,"");
    XtFree(command);
}

void quitApplication()
{
    disconnectFromServer();
    exit(0);
}


/* appends text at the bottom of the main text widget */
void appendText(char *msg)
{
    XmTextPosition lastPos;

    lastPos = XmTextGetLastPosition(outputArea);
    XmTextInsert(outputArea, lastPos, msg);
    lastPos = XmTextGetLastPosition(outputArea);
    XmTextSetInsertionPosition(outputArea, lastPos);
}
