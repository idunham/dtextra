/* 
Copyright 1996 COMPUTER GENERATION, INC.,

The software is provided "as is", without warranty of any kind, express
or implied, including but not limited to the warranties of
merchantability, fitness for a particular purpose and noninfringement.
In no event shall Computer Generation, inc. nor the author be liable for
any claim, damages or other liability, whether in an action of contract,
tort or otherwise, arising from, out of or in connection with the
software or the use or other dealings in the software.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation.

Author:
Gary Aviv 
Computer Generation, Inc.,
gary@compgen.com
*/
/*
;+
LiteClueTest	--	Test program for LiteClue widget

Author:		Gary Aviv (gary@compgen.com)

Functions:	Provide a test fixture for LiteClue widget
 
;-
*/

/*	Revision Information:

$Log: LiteClueTest.c,v $
Revision 1.2  2005/05/18 22:16:00  rwscott
There goes the time I was going to use to add some stuff. Instead
I spent it getting distcheck to work .....

Revision 1.1  2005/04/18 15:34:52  dannybackx
Add the old version of LiteClue to keep track of the original verson.
Michel will be updating this soon.


$log
Initial version.
$log

*/


#define 	VERSION "$Revision: 1.2 $"
#ident		"@(#)$Id: LiteClueTest.c,v 1.2 2005/05/18 22:16:00 rwscott Exp $ $Revision: 1.2 $"

#define	PGMNAME		"LiteClueTest"

/*----------- Include Files -------------*/

#include <stdio.h>
#include <stdlib.h>

#ifdef NeedFunctionPrototypes
#undef NeedFunctionPrototypes
#endif
#define NeedFunctionPrototypes 1

#ifdef NeedVarargsPrototypes
#undef NeedVarargsPrototypes
#endif
#define NeedVarargsPrototypes 1

#include <X11/Intrinsic.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <LiteClue.h>


static Widget toplevel;
static 	XtAppContext AppContext;

int
main(int 	argc, char 	*argv[])
{
	Widget liteClue, menub, megatb;


  	toplevel = XtVaAppInitialize(&AppContext,
			"LiteClueTest",
			NULL, 0, /*appl cmd line options*/
			&argc, argv, 
			NULL, /* fallback resources */
			NULL);	/* top level resources, VA args */

	liteClue = XtVaCreatePopupShell( 
		"popup_shell", 
		xcgLiteClueWidgetClass, toplevel,
		NULL);

	menub = XtVaCreateManagedWidget("menub", boxWidgetClass, 
			toplevel, XtNorientation, XtEhorizontal, NULL);

	megatb = XtVaCreateManagedWidget("Button1", commandWidgetClass, 
		menub, XtNlabel, "Button1", NULL);
	XcgLiteClueAddWidget(liteClue, megatb,  "LiteClue1", 0, 0);

	megatb = XtVaCreateManagedWidget("Button2", commandWidgetClass, 
		menub, XtNlabel, "Button1", NULL);
	XcgLiteClueAddWidget(liteClue, megatb,  "LiteClue2", 0, 0);

	XtRealizeWidget(toplevel);

	XtAppMainLoop(AppContext);
	return(0);
}

