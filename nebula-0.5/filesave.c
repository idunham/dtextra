/* --------------------------------------------------------------------- *
 * filesave.c                                                            *
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

#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/SelectioB.h>
#include <Xm/FileSB.h>

#include "main.h"

extern Widget outputArea;

void saveFileCbk (Widget w, XtPointer clientData, XtPointer callData);

void saveFile(Widget parent)
{
    Widget selDialog;
    Widget remove;
    XmString theXmString;
    Arg      args[1];
    
    theXmString = XM_STRING_CREATE("Save As");
    XtSetArg(args[0], XmNdialogTitle, theXmString);
    selDialog = XmCreateFileSelectionDialog(parent, "select", args, 1);
    XtAddCallback(selDialog, XmNokCallback, saveFileCbk, NULL);
    XtAddCallback(selDialog, XmNcancelCallback, saveFileCbk, NULL);
    remove = XmSelectionBoxGetChild(selDialog, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(remove);
    XtManageChild(selDialog);
}

void saveFileCbk (Widget w, XtPointer clientData, XtPointer callData)
{
    char *fileName;
    XmFileSelectionBoxCallbackStruct *cbStruct;
    
    cbStruct = (XmFileSelectionBoxCallbackStruct *)callData;
    
    if (cbStruct->reason == XmCR_OK)
    {
    	FILE   *outFile;
	char   *text;
	size_t textLen;

	
	/* ##### XmSTRING_DEFAULT_CHARSET is motif 1.1 ##### */
	XmStringGetLtoR(cbStruct->value, XmSTRING_DEFAULT_CHARSET, &fileName);
	outFile = fopen(fileName, "w");
    	XtFree(fileName);
	if (outFile == NULL)
	{
	    printf("error opening file for write.\n");
	    return;
	}
	text = XmTextGetString(outputArea);
	textLen = strlen(text);
	fwrite(text, textLen, sizeof(char), outFile);
	fclose(outFile);
	XtFree(text);
	XtUnmanageChild(w);
    } else if (cbStruct->reason == XmCR_CANCEL)
    {
	XtUnmanageChild(w);
    }
}
