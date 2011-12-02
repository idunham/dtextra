/**
 *
 * $Id: SelectDebugLevel.c,v 1.8 2001/06/09 18:38:57 amai Exp $
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

#include <stdio.h>

#include <Xm/ToggleB.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>

#include <Xlt.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static const char rcsid[] = "$Id: SelectDebugLevel.c,v 1.8 2001/06/09 18:38:57 amai Exp $";

/* ******************** */

static void ChangeDebugLevel(Widget W, int NewLevel, XmToggleButtonCallbackStruct *reason)
{
int *DebugLevelPtr;

        XtVaGetValues(XtParent(W),
        		XmNuserData,&DebugLevelPtr,
        		NULL);
        if (reason->set == True)
        {
                *DebugLevelPtr = NewLevel;
        }
        XtUnmanageChild(XtParent(XtParent(W)));
}

/* ******************** */

void XltSelectDebugLevel(Widget W,
                      int *DebugLevelPtr,
                      XmPushButtonCallbackStruct *reason)
{
static Widget DebugDialog = NULL;
int i;
Widget tb;
char name[10];

        if (DebugDialog == NULL)
        {
        Widget tmp;
	Widget rc;

                tmp = W;
                while (! XtIsTopLevelShell(tmp)) tmp = XtParent(tmp);
                DebugDialog = XmCreateMessageDialog(tmp,"DebugDialogShell",NULL,0);
		XtVaSetValues(DebugDialog,
				XmNallowOverlap,False,
				NULL);
                rc = XmCreateRowColumn(DebugDialog,"DebugDialogRc",NULL,0);
		XtVaSetValues(rc,
				XmNuserData,DebugLevelPtr,
				XmNradioBehavior,True,
				XmNorientation,XmVERTICAL,
				NULL);
                {
                int client_data;

		    for (i = 0, client_data = 0; i < 10; i++, client_data++)
		    {
			sprintf(name, "Debug%i", i);
			tb = XmCreateToggleButton(rc, name, NULL, 0);
			XtManageChild(tb);
			XtAddCallback(tb, XmNvalueChangedCallback, (XtCallbackProc)ChangeDebugLevel, (XtPointer)client_data);
		    }
                }
                XtManageChild(rc);
        }
	sprintf(name,"*Debug%i",*DebugLevelPtr);
	tb = XtNameToWidget(DebugDialog,name);
	if (tb != NULL)
	{
		XmToggleButtonSetState(tb,True,False);
	}
        XtManageChild(DebugDialog);
}
