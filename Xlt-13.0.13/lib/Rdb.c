/**
 *
 * $Id: Rdb.c,v 1.4 2005/01/26 23:04:37 rwscott Exp $
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

#include <X11/Intrinsic.h>
#include <Xm/Gadget.h>

static XrmDatabase
GetDb(Display *display)
{
Status status;
Window window = DefaultRootWindow(display);
XTextProperty text_prop;
Atom prop_atom = XInternAtom(display, "RESOURCE_MANAGER", False);
char **list;
int count;
XrmDatabase db;

    status = XGetTextProperty(display, window, &text_prop, prop_atom);
    status = XTextPropertyToStringList(&text_prop, &list, &count);
    XtFree(text_prop.value);
    db = XrmGetStringDatabase(list[0]);
    XFreeStringList(list);
    return(db);
}

static void
strappend(char **dest, char *src)
{
char *tmp;
int i;
int j;

    tmp = XtMalloc(2 * strlen(src) + 1);
    for (i = 0, j = 0; i < strlen(src); i++, j++)
    {
    	if (i > 0 && src[i] == '\n')
    	{
	    tmp[j] = '\\';
	    j++;
	    tmp[j] = 'n';
    	}
    	else
    	{
	    tmp[j] = src[i];
    	}
    }
    tmp[j] = 0;

    dest[0] = XtRealloc(dest[0], strlen(dest[0]) + strlen(tmp) + 1);
    strcat(dest[0], tmp);
    XtFree(tmp);
}

static Bool
DumpEntry(XrmDatabase *db, XrmBindingList bindings, XrmQuarkList quarks, XrmRepresentation *type, XrmValuePtr value, XPointer data)
{
XrmQuark XrmQString = XrmPermStringToQuark("String");
char **list = (char **)data;

    if (*type == XrmQString)
    {
    Bool firstNameSeen;

    	for (firstNameSeen = False; *quarks; bindings++, quarks++)
    	{
	    if (*bindings == XrmBindLoosely)
	    {
		strappend(&list[0], "*");
	    }
	    else if (firstNameSeen)
	    {
		strappend(&list[0], ".");
	    }
	    firstNameSeen = True;
	    strappend(&list[0], XrmQuarkToString(*quarks));
    	}
    	strappend(&list[0], ":\t");
    	strappend(&list[0], value->addr);
    	strappend(&list[0], "\n");
    }
    else
    {
	fprintf(stderr, "%s:%s(%d) - not a string resource\n", 
	    __FILE__, __FUNCTION__, __LINE__);
    }
    return(False);
}

static void
PutDb(Display *display, XrmDatabase db)
{
Status status;
Window window = DefaultRootWindow(display);
Atom prop_atom = XInternAtom(display, "RESOURCE_MANAGER", False);
XTextProperty text_prop;
int count = 1;
char *list[1];
XrmQuark empty = NULLQUARK;

    list[0] = XtNewString("");

    XrmEnumerateDatabase(db, &empty, &empty, XrmEnumAllLevels, DumpEntry, (XPointer)list);
    status = XStringListToTextProperty(list, count, &text_prop);
    XSetTextProperty(display, window, &text_prop, prop_atom);
    XtFree(text_prop.value);

    XtFree(list[0]);
}

static void
SaveDb(Display *display, XrmDatabase new_db)
{
XrmDatabase old_db;

    XGrabServer(display);
    old_db = GetDb(display);
    XrmMergeDatabases(new_db, &old_db);
    /* new_db is destroyed in the merge */
    PutDb(display, old_db);
    XUngrabServer(display);
    XrmDestroyDatabase(old_db);
}

void
XltRdbPutString(Widget w, String name, String value)
{
String app_name, class;
XrmDatabase new_db = NULL;
char *specifier;
Display *display;
XrmDatabase disp_db;

    display = XmIsGadget(w) ? XtDisplay(XtParent(w)) : XtDisplay(w);
    XtGetApplicationNameAndClass(display, &app_name, &class);
    specifier = XtMalloc(strlen(app_name) + strlen(name) + 2);
    sprintf(specifier, "%s.%s", app_name, name);
    XrmPutStringResource(&new_db, specifier, value);
    disp_db = XrmGetDatabase(display);
    if (disp_db)
    {
	XrmPutStringResource(&disp_db, specifier, value);
    }
    XtFree(specifier);
    SaveDb(display, new_db);
}

#if 0
int
main(int argc, char *argv[])
{
Widget shell;
XtAppContext app;

    shell = XtVaAppInitialize(&app, "PrefsTest",
    	NULL, 0,
    	&argc, argv,
    	NULL,
    	NULL);
    RdbPutString(shell, "test", "test\nvalue 13");
    XtDestroyWidget(shell);
    return(0);
}
#endif
