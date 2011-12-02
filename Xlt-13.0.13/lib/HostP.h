/**
 *
 * $Id: HostP.h,v 1.6 2005/05/19 16:17:06 rwscott Exp $
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

#ifndef _HOSTP_H
#define _HOSTP_H

#include <X11/IntrinsicP.h>
#include <X11/ObjectP.h>
#include <netinet/in.h>
#include <Host.h>

typedef enum { MODE_ASCII, MODE_BINARY } InputMode;
typedef struct _xltClientInfo_struct {
    int fd;
    struct sockaddr_in addr;
    XtInputId id;
} XltClientInfo;

typedef struct {
	int Fd;
	String Name;
	String Port;
	XtInputId InputId;
	XtIntervalId OutputId;
	XtCallbackList DisconnectCallback;
	XtCallbackList ConnectCallback;
	XtCallbackList InputCallback;
	XtCallbackList AsciiInputCallback;
	XtCallbackList BinaryInputCallback;
	char *OutputData;
	int OutputMaxSize;
	int OutputSize;
	char *InputData;
	int InputMaxSize;
	int InputSize;
	InputMode Mode;
	String Terminator;
	int x;
	int y;
	int tmp;
	Boolean Throttle;
	int Delay;
	Widget host_dialog;
	int InputNeed;
	XtCallbackList OutputCallback;
	XtIntervalId BinaryTimeOutId;
	unsigned char type;
	int num_clients;
	XltClientInfo *client_list;
	Widget save_dialog;
	Widget transcript;
} XltHostPart;

#define Host_ReadBuffer(w) (((XltHostWidget)w)->host.ReadBuffer)
#define Host_BufferSize(w) (((XltHostWidget)w)->host.BufferSize)
#define Host_BinaryTimeOutId(w) (((XltHostWidget)w)->host.BinaryTimeOutId)
#define Host_Fd(w) (((XltHostWidget)w)->host.Fd)
#define Host_Name(w) (((XltHostWidget)w)->host.Name)
#define Host_Port(w) (((XltHostWidget)w)->host.Port)
#define Host_InputId(w) (((XltHostWidget)w)->host.InputId)
#define Host_OutputId(w) (((XltHostWidget)w)->host.OutputId)
#define Host_DisconnectCallback(w) (((XltHostWidget)w)->host.DisconnectCallback)
#define Host_OutputCallback(w) (((XltHostWidget)w)->host.OutputCallback)
#define Host_ConnectCallback(w) (((XltHostWidget)w)->host.ConnectCallback)
#define Host_InputCallback(w) (((XltHostWidget)w)->host.InputCallback)
#define Host_AsciiInputCallback(w) (((XltHostWidget)w)->host.AsciiInputCallback)
#define Host_BinaryInputCallback(w) (((XltHostWidget)w)->host.BinaryInputCallback)
#define Host_OutputData(w) (((XltHostWidget)w)->host.OutputData)
#define Host_OutputMaxSize(w) (((XltHostWidget)w)->host.OutputMaxSize)
#define Host_OutputSize(w) (((XltHostWidget)w)->host.OutputSize)
#define Host_InputData(w) (((XltHostWidget)w)->host.InputData)
#define Host_InputMaxSize(w) (((XltHostWidget)w)->host.InputMaxSize)
#define Host_InputSize(w) (((XltHostWidget)w)->host.InputSize)
#define Host_Mode(w) (((XltHostWidget)w)->host.Mode)
#define Host_Terminator(w) (((XltHostWidget)w)->host.Terminator)
#define Host_x(w) (((XltHostWidget)w)->host.x)
#define Host_y(w) (((XltHostWidget)w)->host.y)
#define Host_tmp(w) (((XltHostWidget)w)->host.tmp)
#define Host_Throttle(w) (((XltHostWidget)w)->host.Throttle)
#define Host_Delay(w) (((XltHostWidget)w)->host.Delay)
#define Host_Dialog(w) (((XltHostWidget)w)->host.host_dialog)
#define Host_InputNeed(w) (((XltHostWidget)w)->host.InputNeed)
#define Host_Type(w) (((XltHostWidget)w)->host.type)
#define Host_NumClients(w) (((XltHostWidget)w)->host.num_clients)
#define Host_ClientList(w) (((XltHostWidget)w)->host.client_list)
#define Host_SaveDialog(w) (((XltHostWidget)w)->host.save_dialog)
#define Host_Transcript(w) (((XltHostWidget)w)->host.transcript)

typedef struct _XltHostRec {
	ObjectPart object;
	XltHostPart host;
} XltHostRec;

typedef struct {
	XtPointer extension;
} XltHostClassPart;

typedef struct _XltHostClassRec {
	ObjectClassPart object_class;
	XltHostClassPart host_class;
} XltHostClassRec;

extern XltHostClassRec xrwsHostClassRec;
#endif
