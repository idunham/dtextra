/**
 *
 * $Id: Host.c,v 1.32 2005/09/15 14:10:08 rwscott Exp $
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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <sys/socket.h>
#include <sys/time.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <netinet/in.h>
#include <netdb.h>

#include <X11/StringDefs.h>

#include <Xm/MessageB.h>
#include <Xm/Form.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/RepType.h>

#include <HostP.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#undef DEBUG

static const char rcsid[] = "$Id: Host.c,v 1.32 2005/09/15 14:10:08 rwscott Exp $";

typedef struct
{
    String DefaultHelpString;
}
AppResources_t, *AppResourcesPtr;

static AppResources_t AppResources;

static char DefaultHelp[] = "\n"
"The Host Select dialog allows you to select a server\n"
"process to connect to by means of a tcp/ip socket. Host is the\n"
"name or IP address of the node running the server and\n"
"Port is the service port, name or number, that the server\n"
"is bound to. If a successfull connection is made the settings\n"
"are saved to the displays RESORCE_MANAGER property by means\n"
"of xrdb(1).";

/*
   Widget methods, forward declarations
 */

static void class_initialize(void);
static void class_part_initialize(WidgetClass widget_class);
static void initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args);
static void destroy(Widget w);
static Boolean set_values(Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args);
static void get_values_hook(Widget w, ArgList args, Cardinal *num_args);

/*
   Helper functions, forward declarations
 */

static void SendByte(Widget w);
static Boolean Connect(Widget W);
static int SockOpen(char *HostName, char *Service);
static void InputHandler(Widget W, int *Fd, XtInputId *Id);

/*
   Widget default resources
 */

#define Offset(field) XtOffsetOf(XltHostRec, host.field)
static XtResource resources[] =
{
    {
	XltNdelay, XltCDelay, XtRInt,
	sizeof(int), Offset(Delay),
	XtRImmediate, 0
    },
    {
	XltNterminator, XltCTerminator, XtRString,
	sizeof(String), Offset(Terminator),
	XtRImmediate, NULL
    },
    {
	XltNname, XltCName, XtRString,
	sizeof(String), Offset(Name),
	XtRImmediate, NULL
    },
    {
	XltNinputCallback, XtCCallback, XtRCallback,
	sizeof(XtCallbackList), Offset(InputCallback),
	XtRCallback, NULL
    },
    {
	XltNdisconnectCallback, XtCCallback, XtRCallback,
	sizeof(XtCallbackList), Offset(DisconnectCallback),
	XtRCallback, NULL
    },
    {
	XltNconnectCallback, XtCCallback, XtRCallback,
	sizeof(XtCallbackList), Offset(ConnectCallback),
	XtRCallback, NULL
    },
    {
	XltNasciiInputCallback, XtCCallback, XtRCallback,
	sizeof(XtCallbackList), Offset(AsciiInputCallback),
	XtRCallback, NULL
    },
    {
	XltNbinaryInputCallback, XtCCallback, XtRCallback,
	sizeof(XtCallbackList), Offset(BinaryInputCallback),
	XtRCallback, NULL
    },
    {
	XltNport, XltCPort, XtRString,
	sizeof(String), Offset(Port),
	XtRImmediate, NULL
    },
    {
	XltNoutputCallback, XtCCallback, XtRCallback,
	sizeof(XtCallbackList), Offset(OutputCallback),
	XtRCallback, NULL
    },
    {
	XltNhostType, XltCHostType, XltRHostType,
	sizeof(unsigned char), Offset(type),
	XtRImmediate, (XtPointer)XltHOST_CLIENT
    },
};
#undef Offset

/*
   Widget class record
 */
/* *INDENT-OFF* */
XltHostClassRec xrwsHostClassRec = {
	/* Object Class Part */
	{
/* pointer to superclass ClassRec   WidgetClass       */ (WidgetClass) &objectClassRec,
/* widget resource class name       String            */ "XltHost",
/* size in bytes of widget record   Cardinal          */ sizeof(XltHostRec),
/* class initialization proc        XtProc            */ class_initialize,
/* dynamic initialization           XtWidgetClassProc */ class_part_initialize,
/* has class been initialized?      XtEnum            */ False,
/* initialize subclass fields       XtInitProc        */ initialize,
/* notify that initialize called    XtArgsProc        */ NULL,
/* NULL                             XtProc            */ NULL,
/* NULL                             XtPointer         */ NULL,
/* NULL                             Cardinal          */ (Cardinal)NULL,
/* resources for subclass fields    XtResourceList    */ resources,
/* number of entries in resources   Cardinal          */ XtNumber(resources),
/* resource class quarkified        XrmClass          */ NULLQUARK,
/* NULL                             Boolean           */ (Boolean)NULL,
/* NULL                             XtEnum            */ (XtEnum)NULL,
/* NULL				    Boolean           */ (Boolean)NULL,
/* NULL                             Boolean           */ (Boolean)NULL,
/* free data for subclass pointers  XtWidgetProc      */ destroy,
/* NULL                             XtProc            */ NULL,
/* NULL			            XtProc            */ NULL,
/* set subclass resource values     XtSetValuesFunc   */ set_values,
/* notify that set_values called    XtArgsFunc        */ NULL,
/* NULL                             XtProc            */ NULL,
/* notify that get_values called    XtArgsProc        */ get_values_hook,
/* NULL                             XtProc            */ NULL,
/* version of intrinsics used       XtVersionType     */ XtVersion,
/* list of callback offsets         XtPointer         */ NULL,
/* NULL                             String            */ NULL,
/* NULL                             XtProc            */ NULL,
/* NULL                             XtProc            */ NULL,
/* pointer to extension record      XtPointer         */ NULL
	},
	/* Host Class Part */
	{
		NULL
	}
};
/* *INDENT-ON* */





WidgetClass xrwsHostWidgetClass = (WidgetClass)&xrwsHostClassRec;

/*
   Helper routines
 */

static void
Disconnect(Widget w)
{
int i;

#ifdef DEBUG
    printf("Disconnect()\n");
#endif
    for (i = 0; i < Host_NumClients(w); i++)
    {
	close(Host_ClientList(w)[i].fd);
    }
    if (Host_ClientList(w))
    {
    	XtFree((char *)Host_ClientList(w));
    	Host_ClientList(w) = NULL;
    }
    Host_NumClients(w) = 0;
    if (Host_InputId(w))
    {
	XtRemoveInput(Host_InputId(w));
	Host_InputId(w) = 0;
    }
    if (Host_Fd(w) != -1)
    {
    XltHostCallbackStruct tmp;

	tmp.reason = XltCR_DISCONNECT;
	XtCallCallbackList(w, Host_DisconnectCallback(w), &tmp);
	close(Host_Fd(w));
	Host_Fd(w) = -1;
    }
    if (Host_OutputId(w))
    {
	XtRemoveTimeOut(Host_OutputId(w));
	Host_OutputId(w) = 0;
    }
    if (Host_BinaryTimeOutId(w))
    {
	XtRemoveTimeOut(Host_BinaryTimeOutId(w));
	Host_BinaryTimeOutId(w) = 0;
    }
    Host_OutputSize(w) = 0;
    Host_InputSize(w) = 0;
    Host_OutputData(w)[0] = '\0';
    Host_InputData(w)[0] = '\0';
    Host_Mode(w) = MODE_ASCII;
}

/* ******************** */

static void
DelayedConnect(Widget W, Widget FD, XConfigureEvent * event)
{
#ifdef DEBUG
    printf("DelayedConnect()\n");
#endif
    if (event->type == MapNotify)
    {
	XtRemoveEventHandler(W,
			     StructureNotifyMask,
			     False,
			     (XtEventHandler)DelayedConnect,
			     FD);
	Connect(FD);
    }
}

/* ************** */

static void
TimeOut(Widget W)
{
    _XltHostCallbackStruct tmp;

    fprintf(stderr, "%s - Timed out during binary input %i\n", XtName(W), Host_y(W));
    tmp.reason = XltCR_BINARYINPUT;
    tmp.data = XtMalloc(Host_InputSize(W));
    tmp.len = Host_InputSize(W);
    memcpy(tmp.data, Host_InputData(W), Host_InputSize(W));
    Host_InputData(W)[0] = '\0';
    Host_InputSize(W) = 0;
    Host_Mode(W) = MODE_ASCII;
    XtCallCallbackList(W, Host_BinaryInputCallback(W), &tmp);
    XtFree(tmp.data);
}

/* ************** */

static void
BinaryByte(Widget W, char byte)
{
    if (Host_BinaryTimeOutId(W) != (XtIntervalId)NULL)
    {
	XtRemoveTimeOut(Host_BinaryTimeOutId(W));
	Host_BinaryTimeOutId(W) = (XtIntervalId)NULL;
    }
    Host_InputData(W)[Host_InputSize(W)] = byte;
    Host_InputSize(W)++;
    Host_InputData(W)[Host_InputSize(W)] = '\0';
    if (Host_x(W) == 0)
    {
	Host_x(W) = byte - 0x30;
	Host_tmp(W) = 0;
	/*
	   printf("x = %i\n",Host_x(W));
	 */
    }
    else if (Host_tmp(W) < Host_x(W))
    {
	Host_y(W) = Host_y(W) * 10 + (byte - 0x30);
	Host_tmp(W)++;
	Host_InputNeed(W) = Host_y(W) + Host_InputSize(W);
	/*
	   if (Host_tmp(W) == Host_x(W))
	   {
	   printf("y = %i\n",Host_y(W));
	   }
	 */
    }
    else
    {
	Host_y(W)--;
	if (Host_y(W) < 0)
	{
	    _XltHostCallbackStruct tmp;

	    switch (Host_InputData(W)[Host_InputSize(W) - 1])
	    {
	    case 0x0a:
		tmp.reason = XltCR_BINARYINPUT;
		tmp.data = XtMalloc(Host_InputSize(W));
		tmp.len = Host_InputSize(W);
		memcpy(tmp.data, Host_InputData(W), Host_InputSize(W));
		Host_InputData(W)[0] = '\0';
		Host_InputSize(W) = 0;
		Host_Mode(W) = MODE_ASCII;
		XtCallCallbackList(W, Host_BinaryInputCallback(W), &tmp);
		XtFree(tmp.data);
		break;
	    case '#':
		/*
		   printf("Second data block\n");
		 */
		Host_x(W) = 0;
		Host_y(W) = 0;
		Host_InputNeed(W) = Host_y(W) + Host_InputSize(W);
		break;
	    }
	}
    }
    if (Host_Mode(W) == MODE_BINARY)
    {
	Host_BinaryTimeOutId(W) = XtAppAddTimeOut(XtWidgetToApplicationContext(W),
			     10000, (XtTimerCallbackProc)TimeOut, W);
    }
}

/* ************** */

static void
Throttle(Widget W, Boolean state)
{
    Host_Throttle(W) = state;
    if (state)
    {
	if (Host_OutputId(W))
	{
	    XtRemoveTimeOut(Host_OutputId(W));
	    Host_OutputId(W) = 0;
	}
    }
    else
    {
	if (Host_OutputSize(W) > 0 && Host_OutputId(W) == (XtIntervalId)NULL)
	{
	    Host_OutputId(W) = XtAppAddTimeOut(XtWidgetToApplicationContext(W),
					       Host_Delay(W),
					       (XtTimerCallbackProc)SendByte, W);
	}
    }
}

static int
Listen(char *Service)
{
int optval = 1;
int ListenFd;
struct sockaddr_in my_addr_in;
struct servent *sp;

    memset((char *)&my_addr_in, 0, sizeof(struct sockaddr_in));
    my_addr_in.sin_family = AF_INET;
    my_addr_in.sin_addr.s_addr = INADDR_ANY;
    if (atoi(Service) != 0)
    {
    	my_addr_in.sin_port = htons(atoi(Service));
    }
    else
    {
#ifdef HAVE_GETSERVBYNAME
    	sp = getservbyname(Service, "tcp");
    	if (sp == NULL)
#endif
    	{
	    fprintf(stderr, "Service >%s< not found\n", Service);
	    ListenFd = -1;
	    return(ListenFd);
    	}
	my_addr_in.sin_port = sp->s_port;
    }
    ListenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenFd == -1)
    {
	fprintf(stderr, "socket error\n");
	ListenFd = -1;
	return(ListenFd);
    }
    if (setsockopt(ListenFd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(optval)) == -1)
    {
	close(ListenFd);
	fprintf(stderr, "setsockopt error\n");
	ListenFd = -1;
	return(ListenFd);
    }
    if (bind(ListenFd, (void *)&my_addr_in, sizeof(struct sockaddr_in)) == -1)
    {
	close(ListenFd);
	fprintf(stderr, "bind error\n");
	ListenFd = -1;
	return(ListenFd);
    }
    if (listen(ListenFd, 5) == -1)
    {
	close(ListenFd);
	fprintf(stderr, "listen error\n");
	ListenFd = -1;
	return(ListenFd);
    }
    return(ListenFd);
}

static int
SockOpen(char *HostName, char *Service)
{
    struct sockaddr_in my_addr, peer_addr;
    struct servent *sp;
    struct hostent *hp;
    int ServerFd;

    memset((char *)&my_addr, 0, sizeof(struct sockaddr_in));
    memset((char *)&peer_addr, 0, sizeof(struct sockaddr_in));
    peer_addr.sin_family = AF_INET;
    hp = gethostbyname(HostName);
    if (hp == NULL)
    {
	char *buf;

	buf = XtMalloc(strlen(HostName) + 26);
	sprintf(buf, "SockOpen(gethostbyname, %s)", HostName);
	herror(buf);
	XtFree(buf);
	return (-1);
    }
    peer_addr.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    if (atoi(Service) == 0)
    {
	sp = getservbyname(Service, "tcp");
	if (sp == NULL)
	{
	    char *buf;

	    buf = XtMalloc(strlen(Service) + 26);
	    sprintf(buf, "SockOpen(getservbyname, %s)", Service);
	    fprintf(stderr, "%s: service not found\n", buf);
	    free(buf);
	    return (-1);
	}
	peer_addr.sin_port = sp->s_port;
    }
    else
    {
	peer_addr.sin_port = htons(atoi(Service));
    }
    ServerFd = socket(AF_INET, SOCK_STREAM, 0);
    if (ServerFd == -1)
    {
	perror("SockOpen(socket)");
	return (-1);
    }
    if (-1 == connect(ServerFd, (struct sockaddr *)&peer_addr, sizeof(struct sockaddr_in)))
    {
	close(ServerFd);
	perror("SockOpen(connect)");
	return (-1);
    }
    return (ServerFd);
}

static void
SendByte(Widget w)
{
int i;

    while ((XtAppPending(XtWidgetToApplicationContext(w)) & XtIMAlternateInput) == XtIMAlternateInput)
    {
	XtAppProcessEvent(XtWidgetToApplicationContext(w), XtIMAlternateInput);
    }
    Host_OutputId(w) = (XtIntervalId)NULL;
    if (!Host_Throttle(w))
    {
	if (Host_OutputSize(w) > 0)
	{
	    switch (Host_Type(w))
	    {
	    case XltHOST_CLIENT:
		write(Host_Fd(w), &(Host_OutputData(w)[0]), 1);
		break;
	    case XltHOST_SERVER:
		for (i = 0; i < Host_NumClients(w); i++)
		{
		    write(Host_ClientList(w)[i].fd, &(Host_OutputData(w)[0]), 1);
		}
		break;
	    default:
		break;
	    }
	    memmove(&(Host_OutputData(w)[0]), &(Host_OutputData(w)[1]), Host_OutputSize(w));
	    Host_OutputSize(w)--;
	    if (Host_OutputCallback(w))
	    {
		_XltHostCallbackStruct tmp;

		tmp.reason = XltCR_OUTPUT;
		tmp.data = NULL;
		tmp.len = Host_OutputSize(w);
		tmp.input_size = 0;
		tmp.input_need = 0;
		XtCallCallbackList(w, Host_OutputCallback(w), &tmp);
	    }
	}
    }
    Throttle(w, Host_Throttle(w));
}

static void
AsciiByte(Widget W, char byte)
{
#ifdef DEBUG
    printf("AsciiByte(%s)\n", XtName(W));
#endif
    switch (byte)
    {
    case 0x11:
	/*printf("Xon\n");*/
	Throttle(W, False);
	break;
    case 0x13:
	/*printf("Xoff\n");*/
	Throttle(W, True);
	break;
    case 0x0d:
    case 0x0a:
	if (Host_InputSize(W) > 0)
	{
	    _XltHostCallbackStruct tmp;

	    tmp.reason = XltCR_ASCIIINPUT;
	    tmp.data = XtNewString(Host_InputData(W));
	    tmp.len = strlen(tmp.data);
	    Host_InputData(W)[0] = '\0';
	    Host_InputSize(W) = 0;
	    XtCallCallbackList(W, Host_AsciiInputCallback(W), &tmp);
	    XtFree(tmp.data);
	}
	break;
    case '#':
	if (Host_BinaryInputCallback(W))
	{
	    Host_Mode(W) = MODE_BINARY;
	    Host_x(W) = 0;
	    Host_y(W) = 0;
	    Host_InputNeed(W) = Host_y(W) + Host_InputSize(W);
	}
    default:
	Host_InputData(W)[Host_InputSize(W)] = byte;
	Host_InputSize(W)++;
	Host_InputData(W)[Host_InputSize(W)] = '\0';
	break;
    }
}

/* ******************** */

static void
ProcessByte(Widget W, char byte)
{
#ifdef DEBUG
    printf("ProcessByte(%s)\n", XtName(W));
#endif
    if (Host_InputSize(W) + 1 == Host_InputMaxSize(W))
    {
	Host_InputMaxSize(W) += 1;
	Host_InputData(W) = XtRealloc(Host_InputData(W),
				      Host_InputMaxSize(W) + 1);
    }
    switch (Host_Mode(W))
    {
    case MODE_ASCII:
	AsciiByte(W, byte);
	break;
    case MODE_BINARY:
	BinaryByte(W, byte);
	break;
    }
}

/* ******************** */

static void
ConnectionRequest(Widget W, int *Fd, XtInputId *Id)
{
int addrlen;

    Host_NumClients(W)++;
    Host_ClientList(W) = (XltClientInfo *)XtRealloc((char *)Host_ClientList(W), Host_NumClients(W) * sizeof(XltClientInfo));
    addrlen = sizeof(struct sockaddr_in);
    Host_ClientList(W)[Host_NumClients(W) - 1].fd = accept(Host_Fd(W), (void *)&(Host_ClientList(W)[Host_NumClients(W) - 1].addr), &addrlen);
    if (Host_ClientList(W)[Host_NumClients(W) - 1].fd != -1)
    {
	Host_ClientList(W)[Host_NumClients(W) -1].id = XtAppAddInput(XtWidgetToApplicationContext(W),
						Host_ClientList(W)[Host_NumClients(W) - 1].fd,
						(XtPointer)XtInputReadMask,
						(XtInputCallbackProc)InputHandler,
						W);
    }
    else
    {
	Host_NumClients(W)++;
    }
}

/* ******************** */

static void
ReleaseClient(Widget w, int fd)
{
int i, j;

    for (i = 0; i < Host_NumClients(w); i++)
    {
    	if (Host_ClientList(w)[i].fd == fd)
    	{
	    XtRemoveInput(Host_ClientList(w)[i].id);
	    close(Host_ClientList(w)[i].fd);
	    for (j = i + 1; j < Host_NumClients(w); j++)
	    {
	    	Host_ClientList(w)[j - 1] = Host_ClientList(w)[j];
	    }
	    Host_NumClients(w)--;
	    Host_ClientList(w) = (XltClientInfo *)XtRealloc((char *)Host_ClientList(w), Host_NumClients(w) * sizeof(XltClientInfo));
	    break;
    	}
    }
}

/* ******************** */

static void
InputHandler(Widget W, int *Fd, XtInputId *Id)
{
    int i;
    int BytesRead;
    static char *ReadBuffer = NULL;
    static int BufferSize = 0;

#ifdef DEBUG
    printf("InputHandler(%s)\n", XtName(W));
#endif
    if (ReadBuffer == NULL)
    {
	BufferSize = 1;
	ReadBuffer = XtMalloc(BufferSize);
    }
    if (XtAppPending(XtWidgetToApplicationContext(W)) & XtIMAlternateInput)
    {
	fd_set rfds;
	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(*Fd, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	if (select(*Fd + 1, SELECT_TYPE_ARG234(&rfds), NULL, NULL, &tv))
	{
	    BytesRead = read(*Fd, ReadBuffer, BufferSize);
	    if (BytesRead == BufferSize)
	    {
		BufferSize++;
		ReadBuffer = XtRealloc(ReadBuffer, BufferSize);
	    }
	    switch (BytesRead)
	    {
	    case 0:
		fprintf(stderr, "InputHandler(%s) - EOF while reading device\n", XtName(W));
		switch (Host_Type(W))
		{
		case XltHOST_CLIENT:
		    Disconnect(W);
		    break;
		case XltHOST_SERVER:
		    ReleaseClient(W, *Fd);
		    break;
		default:
		    break;
		}
		break;
	    case -1:
		perror("InputHandler");
		Disconnect(W);
		break;
	    default:
		if (Host_InputCallback(W))
		{
		    _XltHostCallbackStruct tmp;

		    tmp.reason = XltCR_INPUT;
		    tmp.len = BytesRead;
		    tmp.data = XtMalloc(tmp.len);
		    memcpy(tmp.data, ReadBuffer, tmp.len);
		    tmp.input_size = Host_InputSize(W);
		    tmp.input_need = Host_InputNeed(W);
		    XtCallCallbackList(W, Host_InputCallback(W), &tmp);
		    if (tmp.len > BufferSize)
		    {
			ReadBuffer = XtRealloc(ReadBuffer, tmp.len);
			BufferSize = tmp.len;
		    }
		    memcpy(ReadBuffer, tmp.data, tmp.len);
		    BytesRead = tmp.len;
		    XtFree(tmp.data);
		}
		for (i = 0; i < BytesRead; i++)
		{
		    ProcessByte(W, ReadBuffer[i]);
		}
		break;
	    }
	    while (XtAppPending(XtWidgetToApplicationContext(W)) & ~XtIMAlternateInput)
	    {
		XtAppProcessEvent(XtWidgetToApplicationContext(W), ~XtIMAlternateInput);
	    }
	}
    }
}

/* ******************** */

static Boolean
Connect(Widget W)
{
    if (Host_Name(W) && strlen(Host_Name(W)) > 0 &&
        Host_Port(W) && strlen(Host_Port(W)) > 0)
    {
	if (!XtIsRealized(XtParent(W)))
	{
	    XtAddEventHandler(XtParent(W),
			      StructureNotifyMask,
			      False,
			      (XtEventHandler)DelayedConnect,
			      W);
	}
	else
	{
	    while (XtAppPending(XtWidgetToApplicationContext(XtParent(W))))
	    {
		XtAppProcessEvent(XtWidgetToApplicationContext(XtParent(W)), XtIMAll);
	    }
	    switch (Host_Type(W))
	    {
	    case XltHOST_CLIENT:
		if (strcmp(Host_Name(W), "-") == 0)
		{
		    Host_Fd(W) = fileno(stdin);
		}
		else
		{
		    Host_Fd(W) = SockOpen(Host_Name(W), Host_Port(W));
		}
		if (Host_Fd(W) >= 0)
		{
		    _XltHostCallbackStruct tmp;
		    String Name;
		    String Class;
		    FILE *xrdb;
		    String xrdb_command;
		    String base = "xrdb -merge -nocpp -display";

		    Host_InputId(W) = XtAppAddInput(XtWidgetToApplicationContext(W),
						    Host_Fd(W),
						    (XtPointer)XtInputReadMask,
						    (XtInputCallbackProc)InputHandler,
						    W);
		    xrdb_command = XtMalloc(strlen(base) + strlen(DisplayString(XtDisplay(XtParent(W)))) + 2);
		    sprintf(xrdb_command, "%s %s", base, DisplayString(XtDisplay(XtParent(W))));
		    xrdb = popen(xrdb_command, "w");
		    if (xrdb != NULL)
		    {
			XtGetApplicationNameAndClass(XtDisplay(XtParent(W)), &Name, &Class);
			fprintf(xrdb, "%s.Host: %s\n%s.Service: %s\n",
				Class,
				Host_Name(W),
				Class,
				Host_Port(W));
			pclose(xrdb);
		    }
		    XtFree(xrdb_command);
		    Host_Throttle(W) = False;
		    tmp.reason = XltCR_CONNECT;
		    XtCallCallbackList(W, Host_ConnectCallback(W), &tmp);
		}
		break;
	    case XltHOST_SERVER:
		Host_Fd(W) = Listen(Host_Port(W));
		if (Host_Fd(W) > 0)
		{
		    Host_InputId(W) = XtAppAddInput(XtWidgetToApplicationContext(W),
						    Host_Fd(W),
						    (XtPointer)XtInputReadMask,
						    (XtInputCallbackProc)ConnectionRequest,
						    W);
		}
		break;
	    default:
		fprintf(stderr, "%s: Unknown host type\n", XtName(W));
		break;
	    }
	}
    }
    else
    {
    }
    return (True);
}

/* ******************** */

static void
OpenHost(Widget W, Widget w)
{
    Widget Host;
    Widget Port;

#ifdef DEBUG
    printf("OpenHost(%s,%s) - %s %s\n", XtName(W), XtName(w), Host_Name(w), Host_Port(w));
#endif
    Disconnect(w);
    Host = XtNameToWidget(W, "*HostText");
    Port = XtNameToWidget(W, "*PortText");
    /* I should be able to free this but can't seem to on HP-UX!!
       XtFree(Host_Name(w));
       XtFree(Host_Port(w));
     */
    Host_Name(w) = XmTextFieldGetString(Host);
    Host_Port(w) = XmTextFieldGetString(Port);
    /*
       printf("Host (%s)>%s< Port (%s)>%s<\n", XtName(Host), Host_Name(w), XtName(Port), Host_Port(w));
     */
    /*
       Connect(w);
     */
    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
		    1000, (XtTimerCallbackProc)Connect, w);
}

/*
   Widget methods
 */

static void
class_initialize(void)
{
XmRepTypeId id;
static char *value_names[] =
{
    "client",
    "server"
};

static unsigned char values[] =
{
    XltHOST_CLIENT,
    XltHOST_SERVER
};

    /*
       printf("class_initialize()\n");
     */
     id = XmRepTypeRegister(XltRHostType, value_names, values, XtNumber(values));
     /* rws 13 Apr 2001
        We need a HostType to String converter
     XtSetTypeConverter(XltRHostType, XmRString, conv, NULL, 0, XtCacheNone, NULL);
     */
     XmRepTypeAddReverse(id);
}

static void
class_part_initialize(WidgetClass widget_class)
{
    /*
       printf("class_part_initialize()\n");
     */
}

static void
initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
    String Name;
    String Class;

    /*
       printf("initialize() - %i args\n", *num_args);
       printf("Fd   %i\n", Host_Fd(new_w));
       printf("Name %s\n", Host_Name(new_w));
       printf("Port %s\n", Host_Port(new_w));
     */
    Host_Mode(new_w) = MODE_ASCII;
    Host_Throttle(new_w) = False;
    Host_InputId(new_w) = 0;
    Host_OutputId(new_w) = 0;
    Host_Fd(new_w) = -1;
    Host_OutputMaxSize(new_w) = 1;
    Host_OutputSize(new_w) = 0;
    Host_OutputData(new_w) = XtMalloc(Host_OutputMaxSize(new_w));
    Host_InputMaxSize(new_w) = 1;
    Host_InputSize(new_w) = 0;
    Host_InputData(new_w) = XtMalloc(Host_InputMaxSize(new_w));
    Host_Dialog(new_w) = NULL;
    Host_InputNeed(new_w) = 0;
    Host_BinaryTimeOutId(new_w) = (XtIntervalId)NULL;
    Host_NumClients(new_w) = 0;
    Host_ClientList(new_w) = NULL;
    Host_SaveDialog(new_w) = NULL;
    if (Host_Terminator(new_w) == NULL)
    {
	Host_Terminator(new_w) = XtNewString("\r\n");
    }
    if (Host_Name(new_w) == NULL)
    {
	Host_Name(new_w) = XtNewString("localhost");
    }
    if (Host_Port(new_w) == NULL)
    {
	XtGetApplicationNameAndClass(XtDisplay(XtParent(new_w)), &Name, &Class);
	Host_Port(new_w) = XtNewString(Name);
    }
    Connect(new_w);
}

static void
destroy(Widget w)
{
    /*
       printf("destroy()\n");
     */
    Disconnect(w);
    XtFree(Host_Name(w));
    XtFree(Host_Port(w));
    XtFree(Host_Terminator(w));
    XtFree(Host_OutputData(w));
    XtFree(Host_InputData(w));
    if (Host_Dialog(w))
    {
	XtDestroyWidget(Host_Dialog(w));
    }
    if (Host_SaveDialog(w))
    {
	XtDestroyWidget(Host_SaveDialog(w));
    }
}

static Boolean
set_values(Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
    Boolean ReConnect = False;

    /*
       printf("set_values()\n");
     */
    if (Host_Terminator(new_w) == NULL)
    {
	Host_Terminator(new_w) = XtNewString("\r\n");
    }
    if (Host_Name(new_w) == NULL)
    {
	Host_Name(new_w) = XtNewString("localhost");
    }
    if (Host_Port(new_w) == NULL)
    {
	String Name;
	String Class;

	XtGetApplicationNameAndClass(XtDisplay(XtParent(new_w)), &Name, &Class);
	Host_Port(new_w) = XtNewString(Name);
    }
    if (strcmp(Host_Name(old), Host_Name(new_w)) != 0)
    {
	ReConnect = True;
    }
    if (strcmp(Host_Port(old), Host_Port(new_w)) != 0)
    {
	ReConnect = True;
    }
    if (Host_Type(old) != Host_Type(new_w))
    {
	ReConnect = True;
    }
    if (Host_Terminator(new_w) != Host_Terminator(old))
    {
    	XtFree(Host_Terminator(old));
    }
    if (Host_Name(new_w) != Host_Name(old))
    {
    	XtFree(Host_Name(old));
    }
    if (Host_Port(new_w) != Host_Port(old))
    {
    	XtFree(Host_Port(old));
    }
    if (ReConnect)
    {
	Disconnect(new_w);
	Connect(new_w);
    }
    return (False);
}

static void
get_values_hook(Widget w, ArgList args, Cardinal *num_args)
{
    /*
       printf("get_values_hook()\n");
     */
}

/*
   Public functions
 */

Widget
XltCreateHost(Widget parent,
	      char *name,
	      Arg *arglist,
	      Cardinal argCount)
{
    return XtCreateWidget(name, xrwsHostWidgetClass, parent, arglist, argCount);
}

void
XltHostSendData(Widget w, char *data, int len)
{
#ifdef DEBUG
    printf("XltHostSendData(%s,%s)\n", XtName(w), data);
#endif
    if (Host_OutputSize(w) + len + 1 > Host_OutputMaxSize(w))
    {
	Host_OutputMaxSize(w) = Host_OutputSize(w) + len + 1;
	Host_OutputData(w) = XtRealloc(Host_OutputData(w), Host_OutputMaxSize(w));
    }
    memcpy(&(Host_OutputData(w)[Host_OutputSize(w)]), data, len);
    Host_OutputSize(w) += len;
    Host_OutputData(w)[Host_OutputSize(w)] = '\0';
    Throttle(w, Host_Throttle(w));
}

void
XltHostSendString(Widget w, String string)
{
#ifdef DEBUG
    printf("XltHostSendString(%s,%s)\n", XtName(w), string);
#endif
    XltHostSendData(w, string, strlen(string));
    XltHostSendData(w, Host_Terminator(w), strlen(Host_Terminator(w)));
}

/* ******************** */

void
XltHostSelect(Widget W, Widget w)
{
    while (!XtIsTopLevelShell(W))
	W = XtParent(W);
    /*
    fprintf(stderr, "%s:%s(%d) - %s %s %s\n", __FILE__, __FUNCTION__, __LINE__,
    	XtName(W), XtName(w),
    	Host_Dialog(w) ? XtName(Host_Dialog(w)) : "NULL");
    */
    if (Host_Dialog(w) == NULL)
    {
	Widget Form;
	Widget Form1;
	Widget Form2;
	Widget HostLabel;
	Widget HostText;
	Widget PortLabel;
	Widget PortText;
	String Name;

	Name = XtMalloc(strlen(XtName(w)) + 7);
	sprintf(Name, "%sSelect", XtName(w));
	Host_Dialog(w) = XmCreateMessageDialog(W, Name, NULL, 0);
	XtFree(Name);
	Form = XmCreateForm(Host_Dialog(w), "Form", NULL, 0);

	Form1 = XmCreateForm(Form, "HostForm", NULL, 0);
	XtVaSetValues(Form1,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_FORM,
		      XmNrightAttachment, XmATTACH_FORM,
		      NULL);
	HostLabel = XmCreateLabel(Form1, "Host", NULL, 0);
	HostText = XmCreateTextField(Form1, "HostText", NULL, 0);
	XtVaSetValues(HostLabel,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_FORM,
		      XmNbottomAttachment, XmATTACH_FORM,
		      NULL);
	XtVaSetValues(HostText,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNbottomAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_WIDGET,
		      XmNleftWidget, HostLabel,
		      XmNrightAttachment, XmATTACH_FORM,
		      NULL);
	XtManageChild(HostLabel);
	XtManageChild(HostText);
	XtManageChild(Form1);

	Form2 = XmCreateForm(Form, "PortForm", NULL, 0);
	XtVaSetValues(Form2,
		      XmNtopAttachment, XmATTACH_WIDGET,
		      XmNtopWidget, Form1,
		      XmNleftAttachment, XmATTACH_FORM,
		      XmNrightAttachment, XmATTACH_FORM,
		      NULL);
	PortLabel = XmCreateLabel(Form2, "Port", NULL, 0);
	PortText = XmCreateTextField(Form2, "PortText", NULL, 0);
	XtVaSetValues(PortLabel,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_FORM,
		      XmNbottomAttachment, XmATTACH_FORM,
		      NULL);
	XtVaSetValues(PortText,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNbottomAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_WIDGET,
		      XmNleftWidget, PortLabel,
		      XmNrightAttachment, XmATTACH_FORM,
		      NULL);
	XtManageChild(PortLabel);
	XtManageChild(PortText);
	XtManageChild(Form2);

	XtManageChild(Form);
	XtAddCallback(Host_Dialog(w), XmNokCallback, (XtCallbackProc)OpenHost, w);
	XtAddCallback(Host_Dialog(w), XmNhelpCallback, (XtCallbackProc)XltHelpOnHostSelect, w);
    }
    XmTextFieldSetString(XtNameToWidget(Host_Dialog(w), "*HostText"), Host_Name(w));
    XmTextFieldSetString(XtNameToWidget(Host_Dialog(w), "*PortText"), Host_Port(w));
    XtManageChild(Host_Dialog(w));
}

/* ******************** */

void
XltHostDiscardOutput(Widget w)
{
    if (Host_OutputId(w))
    {
	XtRemoveTimeOut(Host_OutputId(w));
	Host_OutputId(w) = (XtIntervalId)NULL;
    }
    Host_OutputSize(w) = 0;
}

/* ******************** */

void
XltHelpOnHostSelect(Widget W)
{
    static Widget Dialog = NULL;
    Widget Shell = W;

    while (!XtIsShell(Shell))
	Shell = XtParent(Shell);
    if (Dialog == NULL)
    {
	XmString string;
	XtResource resources[] =
	{
	    {"messageString", "MessageString", XtRString, sizeof(String), XtOffset(AppResourcesPtr, DefaultHelpString), XtRImmediate, DefaultHelp},
	};
	Dialog = XmCreateInformationDialog(Shell, "HelpOnHostSelect", NULL, 0);
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

/* ******************** */

void
XltHostReconnect(Widget w)
{
    Disconnect(w);
    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
		    1000, (XtTimerCallbackProc)Connect, w);
}
