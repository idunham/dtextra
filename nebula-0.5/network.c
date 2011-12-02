/* --------------------------------------------------------------------- *
 * network.c                                                             *
 * --------------------------------------------------------------------- *
 * This file is part of the nebula irc client                            *
 * http://nebula-irc.sourceforge.net                                     *

 * Copyright (c) 2004 Riccardo Mottola. All rights reserved.             *
 * mail: rmottola@users.sourceforge.net                                  *

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
Copyright (c) 2004-2005, Riccardo Mottola

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer. 
Redistributions in binary form must reproduce the abov copyright notice,
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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  /* for inet_ntoa and similar */
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

#include <X11/Intrinsic.h>

#include "main.h"
#include "network.h"
#include "signals.h"
#include "interpreter.h"

extern systemStatusRecord sysState;
extern XtAppContext  nebulaIrcAppContext;
extern XtInputId     nebulaNetworkEventId;

/* this function opens the connection to the given server
 * it also initializes the system information structure sysState
 * and it logins the user into the server
 */
int connectToServer(char *servName, int port)
{
    int                 sock;
    struct hostent      *hostentPtr;
    struct sockaddr_in  sockAddr;
    char                *tempStr;

    appendText("Resolving ");
    appendText(servName);
    appendText("... ");
    if((hostentPtr = gethostbyname(servName)) == NULL)
    {
    	appendText("Could not resolve ");
	appendText(servName);
	appendText("\n");
	return -1;
    }
    bcopy((char *)hostentPtr->h_addr, (char *)&sockAddr.sin_addr, hostentPtr->h_length);
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_port = htons(port);
    
    tempStr = inet_ntoa(sockAddr.sin_addr);
    appendText(tempStr);
    appendText("\n");
    
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (connect(sock, (struct sockaddr*) &sockAddr, sizeof(sockAddr)) < 0)
    {
	perror("connect failed: ");
	return -1;
    }
    appendText("We are connected.\n");
    
    /* initialize the system structures */
    sysState.isConnected = YES;
    sysState.isAuthenticated = NO;
    sysState.socket = sock;
    sysState.isInChannel = NO;
    sysState.isInQuery = NO;
    strcpy(sysState.serverName, servName);
    sysState.port = port;

    /* we add our socket to the select file descriptors as an Event source for X */
    nebulaNetworkEventId = XtAppAddInput(nebulaIrcAppContext, sysState.socket, (XtPointer)(XtInputReadMask), readLine, NULL);
    
    doNick(sysState.myNick);
    doUser(sysState.myUnixname, sysState.myHostName, sysState.serverName, sysState.myName);
    return 0;
}

void resetStatusToDisconnected()
{
    /* we remove the network from the X event handler again */
    XtRemoveInput(nebulaNetworkEventId);
    sysState.isConnected = NO;
    sysState.isAuthenticated = NO;
    sysState.isInChannel = NO;
    sysState.isInQuery = NO;
    sysState.serverName[0]='\0';
    sysState.currentChannel[0]='\0';
    sysState.currentQuery[0]='\0';
}

/* reads one or more lines from the network
 * possibly splits them up and invokes the interpreter
 */
void readLine()
{
    char inBuff[MAX_COMMAND_LEN+1];
    char inLine[MAX_COMMAND_LEN+1];
    char rest[MAX_COMMAND_LEN+1];
    int  readBytes;
    int  buffLen;
    char *firstNLPos;
    
    readBytes = recv(sysState.socket, inBuff, MAX_COMMAND_LEN, 0);
    if (readBytes < 0)
    {
	if (errno == ECONNRESET) /* connection reset by peer */
	{
            resetStatusToDisconnected();
            appendText("Connectioni reset by peer.\n");
            return;
	} else
	{
	    perror("read < 0 bytes from socket");
#ifdef DEBUG
	    printf("errno is: %d\n", errno);
#endif
        }
    }
    if (readBytes  == 0)
    {
    	perror("Read 0 bytes from socket. Assuming connection closed.");
        resetStatusToDisconnected();
        appendText("Lost connection.\n");
	return;
    }
    inBuff[readBytes] = '\0';
    buffLen = strlen(inBuff);
    firstNLPos = strstr(inBuff, "\r\n");
    if (firstNLPos)
    {
	if (firstNLPos - inBuff == buffLen - 2)
	{
#ifdef DEBUG
	    printf("The NL is only at the end\n");
#endif
    	    inBuff[readBytes - 2] = '\0';
#ifdef DEBUG
    	    printf("|%s|\n", inBuff);
	    assert(strstr(inBuff, "\r\n") == NULL);
            assert(strlen(inBuff) < MAX_COMMAND_LEN);
#endif
    	    interpretMessage(inBuff);
	    return;
	} else
	{
	    while ((firstNLPos - inBuff != buffLen - 2) && (firstNLPos != 0))
	    {
	    	int lineLen;
		int restLen;
		
		lineLen = firstNLPos - inBuff;
	    	strncpy(inLine, inBuff, lineLen);
    	    	inLine[lineLen] = '\0';
#ifdef DEBUG
		printf("line = |%s|\n", inLine);
		assert(strstr(inLine, "\r\n") == NULL);
                assert(strlen(inLine) < MAX_COMMAND_LEN);
#endif
    	    	interpretMessage(inLine);
		restLen = buffLen - 2 - lineLen;
		strncpy(rest, inBuff+lineLen+2, restLen);
		rest[restLen] = '\0';
		strcpy(inBuff, rest);
		buffLen = strlen(inBuff);
		firstNLPos = strstr(inBuff, "\r\n");
	    }
	    if (firstNLPos - inBuff == buffLen - 2)
	    {
    	    	inBuff[strlen(inBuff) - 2] = '\0';
#ifdef DEBUG
    	    	printf("|%s|, %d\n", inBuff, strlen(inBuff));
    	    	assert(strstr(inBuff, "\r\n") == NULL);
                assert(strlen(inBuff) < MAX_COMMAND_LEN);
#endif
    	    	interpretMessage(inBuff);
	    	return;
	    }
	}
    }
    
    /* ok what we read was not a full line */
    /* the buffer was probably full (but it may not be anymore oafter the removal */
    /* of the complete lines */
    if (firstNLPos == NULL)
    {
    	char inBuff2[MAX_COMMAND_LEN+1];
	char ch[1];
	int buff2Curs;
	int gotNewLine;
	int gotCR;
	
	buff2Curs = 0;
	gotNewLine = NO;
	gotCR = NO;
	
	printf("We have a not CR_LF terminated input buffer. Reading more.\n");
	while (buff2Curs < MAX_COMMAND_LEN && !gotNewLine)
	{
	    if (read(sysState.socket, ch, 1))
	    {
	    	inBuff2[buff2Curs++] = *ch;	    
		if (*ch == '\r')
		    gotCR = YES;
		else if (gotCR)
		{
		    if (*ch == '\n')
		    {	    
#ifdef DEBUG
		    	printf("I got a CR-LF\n");
#endif
		    	gotNewLine = YES;
			inBuff2[--buff2Curs] = '\0';
			inBuff2[--buff2Curs] = '\0';
		    } else
		    {
		    	gotCR = NO;
		    }
		}
	    }
	}
	if (gotNewLine)
	{
	    if (strlen(inBuff) + strlen(inBuff2) < MAX_COMMAND_LEN)
	    {
	    	strcat(inBuff, inBuff2);
		printf("Complete command is: ||%s||\n", inBuff);
#ifdef DEBUG
                assert(strlen(inBuff) < MAX_COMMAND_LEN);
#endif
		interpretMessage(inBuff);
	    } else
	    {
	    	printf("Error: our buffer is to small to fit all the size to the CR-LF\n");
	    }
	} else
    	    printf("We have a not CR_LF terminated input buffer, even on the second attempt.\n");
    }
}


void sendLine(char *msg)
{
    int len;
    int sentBytes;
    
    if (sysState.isConnected == NO)
    	return;
    len = strlen(msg);
    if (len <= 0)
    	return;
    if (len >= MAX_CHUNK_LEN - 2)
    {
    	msg[MAX_CHUNK_LEN] = '\0';
    	msg[MAX_CHUNK_LEN-1] = '\n';
	msg[MAX_CHUNK_LEN-2] = '\r';
    } else
    {
    	msg[len++] = '\r';
    	msg[len] = '\n';
    }
    sentBytes = write(sysState.socket, msg, len);
    if (sentBytes < len)
    	printf("Couldn't sent whole message.\n");
}

void disconnectFromServer()
{
    /* close the socket if we are connected*/
    if (sysState.isConnected)
        close(sysState.socket);
}


/* nick is copied into the system status structure
 * a check if the argument poitner is the same address as the sysState struct is done
 * but beware of overlapping strings
 */
void doNick(char *nick)
{
    char outBuff[MAX_COMMAND_LEN+1];
    
    if (!sysState.isAuthenticated)
    {
    	strcpy(outBuff, "NICK ");
    	strcat(outBuff, nick);
    } else
    {
    	sprintf(outBuff, ":%s NICK %s", sysState.myNick, nick);
    }
#ifdef DEBUG
    assert(strlen(nick) < MAX_NICK_LEN);
#endif
    sendLine(outBuff);
    
    /* check if strings are the same */
    if (nick != sysState.myNick)
        strcpy(sysState.myNick, nick);
}

void doUser(char *username, char *hostname, char *servername, char *realname)
/* should check for total length */
{
    char outBuff[MAX_COMMAND_LEN+1];
    
    if (strlen(username) + strlen(hostname) + strlen(servername) + strlen(realname) > MAX_COMMAND_LEN-9)
    {
    	printf("command generated in doUser() is too long\n");
	return;
    }
    strcpy(outBuff, "USER ");
    strcat(outBuff, username);
    strcat(outBuff, " ");
    strcat(outBuff, hostname);
    strcat(outBuff, " ");
    strcat(outBuff, servername);
    strcat(outBuff, " :");
    strcat(outBuff, realname);
    printf("sending: %s\n", outBuff);
    sendLine(outBuff);
}

void doPrivateMsg(char *recipient, char *message)
/* should check for total length */
{
    char outBuff[MAX_COMMAND_LEN+1];
    char localEcho[MAX_COMMAND_LEN+1];
    
    sprintf(outBuff, ":%s PRIVMSG %s :%s", sysState.myNick, recipient, message);
    sendLine(outBuff);
    if (!strcmp(recipient, sysState.currentChannel))
    	sprintf(localEcho, "%s : %s\n", sysState.myNick, message);
    else
    	sprintf(localEcho, "[ %s ] : %s\n", sysState.myNick, message);
    appendText(localEcho);
}

void doPong(char *dest)
{
    char outBuff[MAX_COMMAND_LEN];
    
    strcpy(outBuff, "PONG ");
    strcat(outBuff, dest);
    sendLine(outBuff);
#ifdef DEBUG
    printf("Sent Pong to: %s\n", dest);
#endif
}

void doJoin(char *chan)
{
    char outBuff[MAX_COMMAND_LEN];
    
    strcpy(outBuff, "JOIN ");
    strcat(outBuff, chan);
    sendLine(outBuff);

    sysState.isInChannel = YES;
    strcpy(sysState.currentChannel, chan);
}

void doPart(char *chan)
{
    char outBuff[MAX_COMMAND_LEN];
    
    strcpy(outBuff, "PART ");
    if (strcmp(chan, "*"))
    {
    	strcat(outBuff, chan);
	if (strcmp(chan,  sysState.currentChannel))
	    sysState.isInChannel = NO;
    } else
    {
    	sysState.isInChannel = NO;
    	strcat(outBuff, sysState.currentChannel);
    }
    sendLine(outBuff);
}

void doMode (char *recipient, char *option, char *limit, char *user, char *banmask)
{
    char outBuff[MAX_COMMAND_LEN];
    
    sprintf(outBuff, "MODE %s %s %s %s %s", recipient, option, limit, user, banmask);
    printf ("%s\n", outBuff);
    sendLine(outBuff);
    
}

void doWhois(char *nick)
{
    char outBuff[MAX_COMMAND_LEN];
    
    strcpy(outBuff, "WHOIS ");
    strcat(outBuff, nick);
    sendLine(outBuff);
}

void doCTCPVersion (char *recipient)
{
    char tempStr[2*MAX_COMMAND_LEN+2];
    char localEcho[MAX_COMMAND_LEN+1];
    
    sprintf(tempStr, ":%s PRIVMSG %s :\001VERSION\001\n", sysState.myNick, recipient);
    sendLine(tempStr);
    sprintf(localEcho, "[ requested Version from %s ]\n", recipient);
    appendText(localEcho);
}

void doCTCPAction(char *recipient, char *message)
/* should check for total length */
{
    char outBuff[MAX_COMMAND_LEN+1];
    char localEcho[MAX_COMMAND_LEN+1];
    
    sprintf(outBuff, ":%s PRIVMSG %s :\001ACTION %s\001", sysState.myNick, recipient, message);
    sendLine(outBuff);
    if (!strcmp(recipient, sysState.currentChannel))
    	sprintf(localEcho, "-- %s %s\n", sysState.myNick, message);
    else
    	sprintf(localEcho, "[%s]  %s\n", sysState.myNick, message);
    appendText(localEcho);
}
