/* 
 * Copyright 1993, 1994 Liverpool University Computer Science Department
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of L.U.C.S. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. L.U.C.S. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * FILE NAME:	resources.h
 * CREATED:	Tue Jun 7 1994
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION: Header file for xtar resources.
 *
 */

/*
 * Fri Aug 12 1994
 * Added resource to handle mappings.
 *
 */

#ifndef XTAR_RESOURCE_H
#define XTAR_RESOURCE_H		1

#include <X11/StringDefs.h>

static char *fallback_resources[] = {
"*fontList:	-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1",
"*FileList.fontList:	fixed",
"*XmText.fontList:	fixed",
"*title:		XTar V1.4",
"*iconName:		XTar V1.4",
"*background:		Grey75",
"*foreground:		Black",
"*TarWindow.width:      770",
"*TarWindow.height:     600",
"*FileForm.width:       770",
"*FileForm.height:      640",
"*HelpForm.width:       770",
"*HelpForm.height:      640",
"*XmRowColumn.background:	#9F009B00BF00",
"*XmCascadeButton.background:#9F009B00BF00",
"*XmToggleButton*visibleWhenOff:True",
"*XmToggleButton*indicatorSize:12",
"*XmToggleButton*selectColor:Red",
"*FileList*Translations:     #override \n<Key>n:   next()\n\
                                             <Key>/:   search()",
NULL
};

#define	XTAR_MAPPINGS	".gif xv %s\n.jpg xv %s"

struct XtarResources {
    char *mappings;
    char *helpDoc;
    Boolean showPB;
};

static struct XtarResources xtar_resources;

static XtResource resources[] = {
    { "mappings", "Mappings", XtRString,  sizeof(char *),
      XtOffsetOf(struct XtarResources, mappings),
      XtRString, XTAR_MAPPINGS },
    { "showPB", "ShowPB", XtRBoolean,  sizeof(Boolean),
      XtOffsetOf(struct XtarResources, showPB),
      XtRString, "False" },
    { "helpDoc", "HelpDoc", XtRString,  sizeof(char *),
      XtOffsetOf(struct XtarResources, helpDoc),
      XtRString, "/usr/local/lib/X11/xtar/help.txt" }
};

#endif
