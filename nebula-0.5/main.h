/* --------------------------------------------------------------------- *
 * main.h                                                                *
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

#include "config.h"
#include <netdb.h>      /* for MAXHOSTNAMELEN */
#include <sys/param.h>  /* for MAXHOSTNAMELEN on other systems */

#define PREF_FILE_NAME ".nebula.rc"

#define CLIENT_NAME "Nebula IRC"
#define CLIENT_VERSION PACKAGE_VERSION

#define SYSINFO_LEN 128
#define MAX_COMMAND_LEN 1024
#define MAX_INPUT_LEN 480
#define MAX_CHUNK_LEN 512 /* RFC 1459 */
#define MAX_SERVERNAME_LEN 128
#define MAX_NICK_LEN 32
#define MAX_NAME_LEN 64
#define MAX_CHANNEL_LEN 64
#define OLD_MOTIF

#include <assert.h>

#define NO 0
#define YES 1

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#warning MAXHOSTNAMELEN not defined in system headers ?
#endif

typedef struct 
{
    char myHostName[MAXHOSTNAMELEN];
    char systemInfo[SYSINFO_LEN];
    char isConnected;
    char isAuthenticated;
    int  socket;
    char serverName[MAX_SERVERNAME_LEN];
    int  port;
    char myNick[MAX_NICK_LEN];
    char myName[MAX_NAME_LEN];
    char myUnixname[MAX_NAME_LEN];
    char isInChannel;
    char currentChannel[MAX_CHANNEL_LEN];
    char isInQuery;
    char currentQuery[MAX_NICK_LEN];
} systemStatusRecord;

void addFdToSelectMask(int fd);
void rmvFdToSelectMask(int fd);
void quitApplication();
void appendText(char *msg);

#ifdef HAVE_XMSTRINGCREATELOCALIZED
#define XM_STRING_CREATE(str)\
    	XmStringCreateLocalized(str)
#else
#define XM_STRING_CREATE(str)\
    	XmStringCreateSimple(str)
#endif

/* remember to check for XmFONTLIST_DEFAULT_TAG when used */

#ifdef HAVE_XMSTRINGGENERATE
#define XM_STRING_GEN_COMP(str)\
    	XmStringGenerate(str, XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);
#else
#define XM_STRING_GEN_COMP(str)\
    	XmStringCreateLtoR(str, XmFONTLIST_DEFAULT_TAG);
#endif
