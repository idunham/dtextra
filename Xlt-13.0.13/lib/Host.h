/**
 *
 * $Id: Host.h,v 1.6 2005/09/15 14:10:09 rwscott Exp $
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

#ifndef _HOST_H
#define _HOST_H

#include <X11/IntrinsicP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XltNname "name"
#define XltNport "port"
#define XltNinputCallback "inputCallback"
#define XltNdisconnectCallback "disconnectCallback"
#define XltNconnectCallback "connectCallback"
#define XltNasciiInputCallback "asciiInputCallback"
#define XltNbinaryInputCallback "binaryInputCallback"
#define XltNterminator "terminator"
#ifndef XltNdelay
#define XltNdelay "delay"
#endif
#define XltNoutputCallback "outputCallback"
#define XltCName "Name"
#define XltCPort "Port"
#define XltCInputCallback "InputCallback"
#define XltCDisconnectCallback "DisconnectCallback"
#define XltCConnectCallback "ConnectCallback"
#define XltCAsciiInputCallback "AsciiInputCallback"
#define XltCTerminator "Terminator"
#define XltCBinaryInputCallback "BinaryInputCallback"
#define XltCDelay "Delay"
#define XltCOutputCallback "OutputCallback"

#ifndef XltNhostType
#define XltNhostType "hostType"
#endif
#ifndef XltCHostType
#define XltCHostType "HostType"
#endif
#ifndef XltRHostType
#define XltRHostType "HostType"
#endif

#define XltCR_INPUT 0
#define XltCR_ASCIIINPUT 1
#define XltCR_BINARYINPUT 2
#define XltCR_CONNECT 3
#define XltCR_DISCONNECT 4
#define XltCR_OUTPUT 5

enum {
    XltHOST_CLIENT,
    XltHOST_SERVER
};

extern WidgetClass xrwsHostWidgetClass;

typedef struct _XltHostRec *XltHostWidget;
typedef struct _XltHostClassRec *XmHostWidgetClass;
typedef struct {
	int reason;
	char *data;
	int len;
	int input_size;
	int input_need;
} XltHostCallbackStruct, _XltHostCallbackStruct;


#define XltIsHost(w) XtIsSubclass((w), xrwsHostWidgetClass)

extern Widget XltCreateHost(Widget parent,
			     char *name,
			     Arg *arglist,
			     Cardinal argCount);
extern void XltHostSendString(Widget w, String string);
extern void XltHostSendData(Widget w, char *data, int len);
void XltHostSelect(Widget W, Widget w);
void XltHelpOnHostSelect(Widget W);
Widget XltHostCreateTranscript(Widget parent, Widget Host, Arg *args_in, Cardinal n_in);
void XltHostDiscardOutput(Widget w);
#define HAVE_XltHostReconnect
void XltHostReconnect(Widget w);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
