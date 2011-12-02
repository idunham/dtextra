/* --------------------------------------------------------------------- *
 * interpreter.c                                                         *
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
#include <string.h>

#include "main.h"
#include "interpreter.h"
#include "network.h"
#include "userlist.h"

extern systemStatusRecord sysState;

extern Widget userList;  /* user list widget */

void interpretCommand(char *cmd)
{
    char token1[MAX_INPUT_LEN+1];
    char token2[MAX_INPUT_LEN+1];
    char token3[MAX_INPUT_LEN+1];

    if (strlen(cmd) <= 0)
    	return;
    if (cmd[0] == '/')
    {
    	int cmdCursor;
    	int i, k;
    	int cmdLen;
	int token2Start;

    	cmdLen = strlen(cmd);

#ifdef DEBUG
    	assert(cmdLen <= MAX_INPUT_LEN);
#endif
    	cmdCursor = 1;
    	i = 0;
    	while (cmdCursor < cmdLen && cmd[cmdCursor] != ' ')
    	    token1[i++] = cmd[cmdCursor++];
    	token1[i] = '\0';
#ifdef DEBUG
    	printf("Token 1: |%s|\n", token1);
#endif
    	k = 0;
	cmdCursor++;
	token2Start = cmdCursor;
    	while (cmdCursor < cmdLen && cmd[cmdCursor] != ' ')
    	    token2[k++] = cmd[cmdCursor++];
    	token2[k] = '\0';
#ifdef DEBUG
    	printf("Token 2: |%s|\n", token2);
#endif  
        if (!strcmp(token1, "deop"))
	{
	    if (strlen(token2) > 0)
	    	doMode(sysState.currentChannel, "-o", token2, NULL, NULL);
	} else if (!strcmp(token1, "msg"))
	{
	    if (strlen(token2) > 0)
	    {
	    	int l;
	    	l = 0;
	    	cmdCursor++;
    	    	while (cmdCursor < cmdLen)
    	    	    token3[l++] = cmd[cmdCursor++];
    	    	token3[l] = '\0';
		if (strlen(token3) > 0)
		    doPrivateMsg(token2, token3);
		else
		    appendText("You fool, no message specified\n");
	    } else
		appendText("You fool, no victim!\n");
	} else if (!strcmp(token1, "nick"))
	{
	    if (strlen(token2) > 0)
	    {
    	    	renameUserInList(sysState.myNick, token2, userList);
    	    	doNick(token2);
	    }
	} else if (!strcmp(token1, "join"))
	{
	    if (strlen(token2) > 0)
	    {
	    	doJoin(token2);
	    }
	} else if (!strcmp(token1, "ns"))
	{
	    if (strlen(token2) > 0)
	        doPrivateMsg("nickserv", cmd+token2Start);
	    else
	        appendText("You fool, no message specified\n");
	} else if (!strcmp(token1, "mode"))
	{
	    char token4[MAX_INPUT_LEN+1];
	    char token5[MAX_INPUT_LEN+1];
	    char token6[MAX_INPUT_LEN+1];
	    
	    token4[0] = '\0';
	    token5[0] = '\0';
	    token6[0] = '\0';
	    if (strlen(token2) > 0)
	    {
	    	int l;
	    	l = 0;
	    	cmdCursor++;
    	    	while (cmdCursor < cmdLen && cmd[cmdCursor] != ' ')
    	    	    token3[l++] = cmd[cmdCursor++];
    	    	token3[l] = '\0';
#ifdef DEBUG
		printf("token3: %s\n", token3);
#endif
		if (cmdCursor < cmdLen)
		{
		    cmdCursor++;
		    l = 0;
		    while (cmdCursor < cmdLen && cmd[cmdCursor] != ' ')
    	    	    	token4[l++] = cmd[cmdCursor++];
		    token4[l] = '\0';
#ifdef DEBUG
		    printf("token4: %s\n", token3);
#endif
		    if (cmdCursor < cmdLen)
		    {
		    	cmdCursor++;
			l = 0;
		        while (cmdCursor < cmdLen && cmd[cmdCursor] != ' ')
    	    	    	    token5[l++] = cmd[cmdCursor++];
		    	token5[l] = '\0';
#ifdef DEBUG
			printf("token5: %s\n", token5);
#endif
			if (cmdCursor < cmdLen)
		        {
		    	    cmdCursor++;
			    l = 0;
			    while (cmdCursor < cmdLen)
    	    	    	        token6[l++] = cmd[cmdCursor++];
		    	    token6[l] = '\0';
#ifdef DEBUG
			    printf("token6: %s\n", token6);
#endif
			}
		    }
		}
		doMode(token2, token3, token4, token5, token6);
	    } else
		appendText("You fool, no channel or user!\n");
	} else if (!strcmp(token1, "op"))
	{
	    if (strlen(token2) > 0)
	    	doMode(sysState.currentChannel, "+o", token2, NULL, NULL);
    	} else if (!strcmp(token1, "part"))
	{
	    if (strlen(token2) > 0)
	    {
		removeAllUsersFromList(userList);
	    	doPart(token2);
	    } else
	    	appendText("PART: must specify a channel\n");
	} else if (!strcmp(token1, "query"))
	{
	    if (sysState.isConnected)
	    {
	    	if (strlen(token2) > 0)
	    	{
	    	    sysState.isInQuery = YES;
		    strcpy(sysState.currentQuery, token2);
    	    	} else
		    sysState.isInQuery = NO;
	    }
	} else if (!strcmp(token1, "user"))
	{
	    doUser(sysState.myUnixname, sysState.myHostName, sysState.serverName, sysState.myName);
	} else if (!strcmp(token1, "whois"))
	{
	    if (strlen(token2) > 0)
	    	doWhois(token2);
	} else if (!strcmp(token1, "me"))
	{
	    doCTCPAction(sysState.currentChannel, cmd + token2Start);
    	} else if (!strcmp(token1, "ctcp"))
    	{ /* CTCP commands */
	    int l;
	    l = 0;
	    cmdCursor++;
    	    while (cmdCursor < cmdLen)
    	        token3[l++] = cmd[cmdCursor++];
    	    token3[l] = '\0';
	    if (strlen(token3) > 0)
	    {
		printf("Ctcp command: %s, recipient: %s\n", token3, token2);
		if (!strcmp(token3, "version"))
		{
		    doCTCPVersion(token2);
		}
	    }
	    else
		printf("You fool, no command specified\n");
    	} else if (!strcmp(token1, "quit"))
	{
	    quitApplication();
	} else if (!strcmp(token1, "server"))
    	{
	    if (strlen(token2) > 0)
	    	connectToServer(token2, 6666);
	} else
    	{ /* unrecongized command */
    	    appendText("Unknown command: ");
            appendText(token1);
            appendText("\n");
    	}
    } else if (sysState.isConnected)
    {
    	if (sysState.isInQuery)
	    doPrivateMsg(sysState.currentQuery, cmd);
	else if (sysState.isInChannel)
    	    doPrivateMsg(sysState.currentChannel, cmd);
	else
	    appendText("No command given and not explicit state.\n");
    } else
      appendText("Not connected\n");
}

void interpretMessage(char *inBuff)
{
    if (inBuff[0] == ':')
    {
    	char token1[MAX_COMMAND_LEN+1];
	char token2[MAX_COMMAND_LEN+1];
	char token3[MAX_COMMAND_LEN+1];
	char token4[MAX_COMMAND_LEN+1];
	char token5[MAX_COMMAND_LEN+1];
    	int k;
	int buffCursor;
	int buffLen;
	    
    	buffLen = strlen(inBuff);
#ifdef DEBUG
    	assert(buffLen <= MAX_COMMAND_LEN);
#endif
	k = 0;
    	buffCursor = 1;
    	while (buffCursor < buffLen && inBuff[buffCursor] != ' ')
    	    token1[k++] = inBuff[buffCursor++];
    	token1[k] = '\0';
    	if (strlen(token1) > 0)
    	{
	    int l;
	    
#ifdef DEBUG
	    printf ("sender is: |%s|\n", token1);
#endif
	    l = 0;
	    buffCursor++;
	    while (buffCursor < buffLen && inBuff[buffCursor] != ' ')
    	    	token2[l++] = inBuff[buffCursor++];
    	    token2[l] = '\0';
    	    if (strlen(token2) > 0)
    	    {
	    	int m;
                int token3start;
		
#ifdef DEBUG
	    	printf ("command is: |%s|\n", token2);
#endif
		m = 0;
	    	buffCursor++;
                token3start = buffCursor;
	    	while (buffCursor < buffLen && inBuff[buffCursor] != ' ')
    	    	    token3[m++] = inBuff[buffCursor++];
    	    	token3[m] = '\0';
    	    	if (strlen(token3) > 0)
    	    	{
		    int n;
		    int token4start;
		    
#ifdef DEBUG
		    printf ("recipient is: |%s|\n", token3);
#endif
		    n = 0;
	    	    buffCursor++;
		    if (inBuff[buffCursor] == ':')
		    	buffCursor++;
		    token4start = buffCursor;
	    	    while (buffCursor < buffLen)
    	    	    	token4[n++] = inBuff[buffCursor++];
    	    	    token4[n] = '\0';
#ifdef DEBUG
		    printf("full rest token4: |%s|\n", inBuff+token4start);		    
		    printf("Token4 is: |%s|, %d\n", token4, strlen(token4));
#endif
		    if (!strcmp(token2, "PRIVMSG"))
		    {
			if (token4[0] == '\001')
		    	{
		    	    int tok4Len;
			
			    tok4Len = strlen(token4);
		    	    if (token4[tok4Len-1] == '\001')
			    {
#ifdef DEBUG
			    	printf ("should chop last 001\n");
#endif
			    	token4[tok4Len-1] = '\0';
				tok4Len--;
			    }
		    	    n = 0;
			    while (n < tok4Len && token4[n] != ' ')
			    	n++;
			    if (token4[n] == ' ')
			    {
			    	strcpy(token5, token4 + n + 1);
			    	token4[n] = '\0';
#ifdef DEBUG
			    	printf("Token5 is: |%s|\n", token5);
#endif
			    }
		    	    if (!strcmp(token4, "\001ACTION"))
		    	    {   
			    	handleAction(token1, token5);
			    } else if (!strcmp(inBuff+token4start, "\001VERSION\001"))
			    {
			    	handleCTCPVersion(token1);
			    }
			} else
			{
			    if (sysState.isInChannel && (token3[0] == '#'|| token3[0] == '&'))
		    	    	handleChannelMessage(token1, token4);
		    	    else
		    	        handlePrivateMessage(token1, token4);
			}
    	    	    } else if (!strcmp(token2, "NICK"))
    	    	    {
			char oldNick[MAX_NICK_LEN];
			char *nickEnd;
    	    	    	
			nickEnd = strchr(token1, '!');
			if (nickEnd)
			{
    			    strncpy(oldNick, token1, nickEnd-token1);
			    oldNick[nickEnd-token1] = '\0';
			    /* we crop out the leading : form token3 */
#ifdef DEBUG
			    printf("change %s to %s\n", oldNick, token3+1);
#endif
			    renameUserInList(oldNick, token3+1, userList);
			}
		    } else if (!strcmp(token2, "NOTICE"))
		    {
		    	handleNotice(token1, token4); 
		    } else if (!strcmp(token2, "JOIN"))
		    {
		    	handleJoinNotify(token1, token3);
		    } else if (!strcmp(token2, "MODE"))
		    {
		    	handleMode(token1, token3, token4); 
		    } else if (!strcmp(token2, "QUIT"))
		    {
		    	handleQuitNotify(token1, inBuff+token3start+1);
		    } else if (!strcmp(token2, "PART"))
		    {
		    	handlePartNotify(token1, inBuff+token3start+1);
		    } else if (!strcmp(token2, "001"))
		    {
			handleCommandResponse(RPL_WELCOME, token4);
		    } else if (!strcmp(token2, "311"))
		    {
		    	handleCommandResponse(RPL_WHOISUSER, token4);
		    } else if (!strcmp(token2, "312"))
		    {
		    	handleCommandResponse(RPL_WHOISSERVER, token4);
		    } else if (!strcmp(token2, "313"))
		    {
		    	handleCommandResponse(RPL_WHOISOPERATOR, token4);
		    } else if (!strcmp(token2, "317"))
		    {
		    	handleCommandResponse(RPL_WHOISIDLE, token4);
		    } else if (!strcmp(token2, "318"))
		    {
		    	handleCommandResponse(RPL_ENDOFWHOIS, token4);
		    } else if (!strcmp(token2, "319"))
		    {
		    	handleCommandResponse(RPL_WHOISCHANNELS, token4);
		    } else if (!strcmp(token2, "353"))
		    {
		    	handleCommandResponse(RPL_NAMREPLY, token4);
    	    	    } else if (!strcmp(token2, "372"))
		    {
		    	handleCommandResponse(RPL_MOTD, token4);
    	    	    } else if (!strcmp(token2, "375"))
		    {
		    	handleCommandResponse(RPL_MOTDSTART, token4);
		    } else if (!strcmp(token2, "376"))
		    {
		    	handleCommandResponse(RPL_ENDOFMOTD, token4);
		    } else if (!strcmp(token2, "401"))
		    {
		    	handleCommandResponse(ERR_NOSUCHNICK, token4);
		    } else if (!strcmp(token2, "403"))
		    {
		    	handleCommandResponse(ERR_NOSUCHCHANNEL, token4);
		    } else if (!strcmp(token2, "404"))
		    {
		    	handleCommandResponse(ERR_CANNOTSENDTOCHAN, token4);
		    } else if (!strcmp(token2, "433"))
		    {
		    	handleCommandResponse(ERR_NICKNAMEINUSE, token4);
		    } else if (!strcmp(token2, "436"))
		    {
		    	handleCommandResponse(ERR_NICKCOLLISION, token4);
		    } else
		    	printf("got some : command message: %s\n", token2);
		} /* token 3 */
	    } /* token 2*/
	} /* token 1 */
    /* commands not starting with : */
    } else if (!strncmp(inBuff, "NOTICE", 6))
    {
    	char token[MAX_COMMAND_LEN+1];
	int i;
	int len;
	int delta;
	
	delta = strlen("NOTICE") + 1;
	i = 7;
	len = strlen(inBuff);
#ifdef DEBUG
    	assert(len <= MAX_COMMAND_LEN);
#endif
	while (i < len)
	{
	    token[i-delta] = inBuff[i];
	    i++;
	}
	token[i-delta] = '\0';
#ifdef DEBUG
	printf("Should handle notice with: %s\n", token);
#endif
    	handleNotice(0, token);
    } else if (!strncmp(inBuff, "PING", strlen("PING")))
    {
    	char token[MAX_COMMAND_LEN+1];
	int i;
	int len;
	int delta;
	
	delta = strlen("PING") + 1;
	len = strlen(inBuff);
	if (inBuff[delta] == ':')
	    delta++;
	i = delta;
	while (i < len)
	{
	    token[i-delta] = inBuff[i];
	    i++;
	}
	token[i-delta] = '\0';
    	handlePing(token);
    } else
    {
    	printf("I got unknown: |%s|\n", inBuff);
    }

}


void handlePing (char *sender)
{
#ifdef DEBUG
    char msg[MAX_COMMAND_LEN+1];
    

    strcpy(msg, "Got ping from: ");
    strcat(msg, sender);
    strcat(msg, "\n");
    appendText(msg); 
#endif
    doPong(sender);
}

void handlePrivateMessage (char *sender, char *msg)
{
    char tempStr[2*MAX_COMMAND_LEN+2];
    char nick[MAX_COMMAND_LEN+1];
    int i;
    int senderLen;
    
    senderLen = strlen(sender);
    i = 0;
    while (i < senderLen && sender[i] != '!')
    	i++;
    if (sender[i] == '!')
    	strncpy(nick, sender, i);
    nick[i] = '\0';
    sprintf(tempStr, "[%s] %s\n", nick, msg);
    appendText(tempStr);
}

void handleChannelMessage (char *sender, char *msg)
{
    char tempStr[2*MAX_COMMAND_LEN+2];
    char senderNick[MAX_COMMAND_LEN+1];
    int i;
    int nickLen;
    
    i = 0;
    nickLen = strlen(sender);
    while (i < nickLen && sender[i] != '!')
    {
    	senderNick[i] = sender[i];
	i++;
    }
    
    senderNick[i] = '\0';
    sprintf(tempStr, "%s: %s\n", senderNick, msg);
    appendText(tempStr);
}

void handleNotice (char *sender, char *msg)
{
    char tempStr[2*MAX_COMMAND_LEN+2];
    char nick[MAX_COMMAND_LEN+1];
    int i;
    int senderLen;
    
    if (sender)
    {
        senderLen = strlen(sender);
    	i = 0;
    	while (i < senderLen && sender[i] != '!')
    	    i++;
    	if (sender[i] == '!')
    	    strncpy(nick, sender, i);
    	nick[i] = '\0';
    	sprintf(tempStr, "# notice (%s) # : %s\n", nick, msg);
    } else /* sender is NULL */
    {
    	sprintf(tempStr, "# notice # : %s\n", msg);
    }
    appendText(tempStr);
}


void handleAction (char *sender, char *msg)
{
    char tempStr[2*MAX_COMMAND_LEN+2];
    char senderNick[MAX_COMMAND_LEN+1];
    int i;
    int nickLen;
    
    i = 0;
    nickLen = strlen(sender);
    while (i < nickLen && sender[i] != '!')
    {
    	senderNick[i] = sender[i];
	i++;
    }
    senderNick[i] = '\0';
    sprintf(tempStr, "--- %s %s\n", senderNick, msg);
    appendText(tempStr);
}

void handleCommandResponse(int code, char *msg)
{
    char tempStr[MAX_COMMAND_LEN+1];
    switch (code)
    {
    	case RPL_WELCOME:
	    if (!strncmp(msg, "Welcome to the Internet Relay Network", 37))
	    {
	    	char *realNick;
		char realNick2[MAX_NICK_LEN];
		int nickLen;
		int i;
		
		realNick = msg + 38;
		nickLen = strlen(realNick);;
		i = 0;
		while (i < nickLen && realNick[i] != '!')
		{
    	    	    realNick2[i] = realNick[i];
		    i++;
		}
    	    	realNick2[i] = '\0';
		if (strlen(realNick2) && strstr(sysState.myNick, realNick2))
		{
		    printf("your real nick is: %s\n", realNick2);
		    strcpy(sysState.myNick, realNick2);
		}
	    }
	    sprintf(tempStr, "> %s\n", msg);
	    appendText(tempStr);
	    break;
	case RPL_WHOISUSER:
	case RPL_WHOISSERVER:
	case RPL_WHOISOPERATOR:
	case RPL_WHOISIDLE:
	case RPL_ENDOFWHOIS:
	case RPL_WHOISCHANNELS:
	    sprintf(tempStr, "> %s\n", msg);
	    appendText(tempStr);
	    break;
	case RPL_NAMREPLY:
	    handleNamesReply(msg);
	    break;
    	case RPL_MOTDSTART:
	case RPL_MOTD:
	case RPL_ENDOFMOTD:
	    sprintf(tempStr, "> %s\n", msg);
	    appendText(tempStr);
	    break;
	case ERR_NOSUCHNICK:
	    sprintf(tempStr, "> %s\n", msg);
	    appendText(tempStr);
	    break;
	case ERR_NOSUCHCHANNEL:
	    sprintf(tempStr, "> %s\n", msg);
	    appendText(tempStr);
	    break;
	case ERR_CANNOTSENDTOCHAN:
	    sprintf(tempStr, "> %s\n", msg);
	    appendText(tempStr);
	    break;
	case ERR_NICKNAMEINUSE:
	    sprintf(tempStr, "> %s\n", msg);
	    appendText(tempStr);
	    break;
	case ERR_NICKCOLLISION:
	    sprintf(tempStr, "> %s\n", msg);
	    appendText(tempStr);
	    break;
	default:
	    printf("Unknown command response: %d\n", code);
    }
}

void handleJoinNotify(char *sender, char *channel)
{
    char tempStr[2*MAX_COMMAND_LEN+2];
    char senderNick[MAX_COMMAND_LEN+1];
    int i;
    int nickLen;
    
    i = 0;
    nickLen = strlen(sender);
    while (i < nickLen && sender[i] != '!')
    {
    	senderNick[i] = sender[i];
	i++;
    }
    senderNick[i] = '\0';
    sprintf(tempStr, "> %s joined %s\n", senderNick, channel);
    appendText(tempStr);
    if (senderNick[0] == '@' || senderNick[0] == '+')
    	addUserToList(senderNick+1, userList);
    else
    	addUserToList(senderNick, userList);
}

void handleQuitNotify(char *sender, char *desc)
{
    char tempStr[2*MAX_COMMAND_LEN+2];
    char senderNick[MAX_COMMAND_LEN+1];
    int i;
    int nickLen;
    
    i = 0;
    nickLen = strlen(sender);
    while (i < nickLen && sender[i] != '!')
    {
    	senderNick[i] = sender[i];
	i++;
    }
    senderNick[i] = '\0';
    sprintf(tempStr, "> %s quit (%s)\n", senderNick, desc);
    appendText(tempStr);
    removeUserFromList(senderNick, userList);
}

void handlePartNotify(char *sender, char *desc)
{
    char tempStr[2*MAX_COMMAND_LEN+2];
    char senderNick[MAX_COMMAND_LEN+1];
    int i;
    int nickLen;
    
    i = 0;
    nickLen = strlen(sender);
    while (i < nickLen && sender[i] != '!')
    {
    	senderNick[i] = sender[i];
	i++;
    }
    senderNick[i] = '\0';
    sprintf(tempStr, "> %s parted (%s)\n", senderNick, desc);
    appendText(tempStr);
    removeUserFromList(senderNick, userList);
}

void handleNamesReply(char *message)
{
    char nick[MAX_NICK_LEN+1];
    char *beginNick, *endNick, *cursor;
    int  strLen;

    strLen = strlen(message);
    cursor = strchr(message, ':');
    if (cursor == NULL)
	return;
    cursor++;
    while (cursor-message < strLen)
    {
        beginNick = cursor;
	endNick = strchr(cursor, ' ');
	if (endNick == NULL)
	    endNick = message + strLen;
	if (endNick - beginNick <= MAX_NICK_LEN)
	{
	    strncpy(nick, beginNick, endNick - beginNick);
	    nick[endNick - beginNick] = '\0';
	} else
	{
	    strncpy(nick, beginNick, MAX_NICK_LEN);
	    nick[MAX_NICK_LEN] = '\0';
	    printf("Warning, %s was longer than %d characters.\n", nick, MAX_NICK_LEN);
	}
#ifdef DEBUG
	printf("Found user |%s|\n", nick);
#endif
    	if (nick[0] == '@' || nick[0] == '+')
    	    addUserToList(nick+1, userList);
    	else
	    addUserToList(nick, userList);
	cursor = endNick + 1;
    }
}

void handleMode (char *sender, char *dest, char *msg)
{
    char tempStr[2*MAX_COMMAND_LEN+2];
    char nick[MAX_COMMAND_LEN+1];
    char token1[MAX_COMMAND_LEN+1];
    char token2[MAX_COMMAND_LEN+1];
    int msgLen;
    int buffCursor;
    int i;
    int nickLen;
    
    nick[0] = '\0';
    token1[0] = '\0';
    token2[0] = '\0';
    
    /* extract nick part from sender */
    i = 0;
    nickLen = strlen(sender);
    while (i < nickLen && sender[i] != '!')
    {
    	nick[i] = sender[i];
	i++;
    }
    nick[i] = '\0';
    
    buffCursor = 0;
    msgLen = strlen(msg);
    i = 0;
    while (buffCursor < msgLen && msg[buffCursor] != ' ')
    	token1[i++] = msg[buffCursor++];
	
    token1[i] = '\0';
    if (strlen(token1) > 0)
    {
    	int j;
	
	j = 0;
	buffCursor++;
    	while (buffCursor < msgLen)
	    token2[j++] = msg[buffCursor++];
    	token2[j] = '\0';
    }
#ifdef DEBUG
    printf("%s made %s to %s\n", nick, msg, dest);
#endif
    if (!strcmp(token1, "+o"))
    {
    	sprintf(tempStr, "- %s: %s gives Op to %s\n", dest, nick, token2);
    	appendText(tempStr);
    } else if (!strcmp(token1, "-o"))
    {
    	sprintf(tempStr, "- %s: %s removes Op from %s\n", dest, nick, token2);
    	appendText(tempStr);
    }
}

void handleCTCPVersion (char *sender)
{
    char tempStr[2*MAX_COMMAND_LEN+2];
    char nick[MAX_COMMAND_LEN+1];
    char localEcho[MAX_COMMAND_LEN+1];
    int i;
    int senderLen;
    
    senderLen = strlen(sender);
    i = 0;
    while (i < senderLen && sender[i] != '!')
    	i++;
    if (sender[i] == '!' || i == senderLen)
    	strncpy(nick, sender, i);
    nick[i] = '\0';
    sprintf(tempStr, ":%s NOTICE %s :\001VERSION %s:%s:%s\001\n", sysState.myNick, nick, CLIENT_NAME, CLIENT_VERSION, sysState.systemInfo);
    sendLine(tempStr);
    sprintf(localEcho, "[ sent Version to %s ]\n", nick);
    appendText(localEcho);
}
