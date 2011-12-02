/*
    $Id: test1.c,v 1.6 2001/06/09 16:34:18 amai Exp $

    stroke.c is a test program for the Stroke library.
    Copyright (C) 1997  Richard Scott

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <Xm/RowColumn.h>
#include <Xm/MessageB.h>
#include <Xm/CascadeB.h>
#include <Xm/ToggleB.h>
#include <Xm/FileSB.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <Xm/ScrolledW.h>
#include <Xm/BulletinB.h>
#include <Xm/SelectioB.h>
#include <Xm/DrawingA.h>
#include <Xm/Separator.h>
#if 0
#include <X11/xpm.h>
#endif

#ifdef USE_EDITRES
#include <X11/Xmu/Editres.h>
#endif

#include <Xlt.h>
#include <Stroke.h>
#include <Sound.h>
#include <SlideC.h>



char *Program;

Widget AppShell;
XtAppContext    Context;
static char rcsid[]="$Id: test1.c,v 1.6 2001/06/09 16:34:18 amai Exp $";
typedef struct {
	Boolean Sounds;
	String StartupSound;
	String CancelSound;
	int debug;
	Boolean Fallback;
	Boolean RedirectErrors;
       } AppResources_t, *AppResourcesPtr;

AppResources_t AppResources;

static XtResource resources[] = {
	{"Fallback","Fallback",XtRBoolean,sizeof(Boolean),XtOffset(AppResourcesPtr,Fallback),XtRImmediate, (void *)False},
	{"RedirectErrors","RedirectErrors",XtRBoolean,sizeof(Boolean),XtOffset(AppResourcesPtr,RedirectErrors),XtRImmediate, (void *)True},
	{"Sounds","Sounds",XtRBoolean,sizeof(Boolean),XtOffset(AppResourcesPtr,Sounds),XtRImmediate, False},
	{"StartupSound","StartupSound",XtRString,sizeof(String),XtOffset(AppResourcesPtr,StartupSound),XtRImmediate, NULL},
	{"CancelSound","CancelSound",XtRString,sizeof(String),XtOffset(AppResourcesPtr,CancelSound),XtRImmediate, NULL},
	{"debug","debug",XtRInt,sizeof(int),XtOffset(AppResourcesPtr,debug),XtRImmediate, (void *)0},
};

static XrmOptionDescRec opTable[] = {
	{"-Fallback",".Fallback",XrmoptionNoArg, "True"},
	{"-RedirectErrors",".RedirectErrors",XrmoptionNoArg, "False"},
	{"+RedirectErrors",".RedirectErrors",XrmoptionNoArg, "True"},
	{"-Sounds",".Sounds",XrmoptionNoArg, "False"},
	{"+Sounds",".Sounds",XrmoptionNoArg, "True"},
	{"-SoundCommand",".SoundCommand",XrmoptionSepArg, NULL},
	{"-StartupSound",".StartupSound",XrmoptionSepArg, NULL},
	{"-debug",".debug",XrmoptionSepArg, NULL},
};

static void ClearDA(void);

static void DisplayVersion(void);
static void DisplayCopyright(void);
static void DisplayLicense(void);

static XtActionsRec GlobalActions[] = {
	{"DisplayVersion", (XtActionProc)DisplayVersion},
	{"DisplayCopyright", (XtActionProc)DisplayCopyright},
	{"DisplayLicense", (XtActionProc)DisplayLicense},
	{"Exit", (XtActionProc)exit},
	{"ClearDA", (XtActionProc)ClearDA},
};

static char *FallBack[] = {
	"*StdErrText.rows: 10",
	"*StdErrText.columns: 40",
#ifdef linux
	"*SoundCommand: auplay",
	"*StartupSound: /usr/local/sounds/ding.wav",
	"*strokeSound: /usr/local/sounds/chord.wav",
#else /* not linux */
	"*SoundCommand: send_sound",
	"*StartupSound: /usr/local/sounds/ding.wav",
	"*strokeSound: /usr/local/sounds/chord.wav",
#endif /* linux */
	"*strokes: 456 ParentActivate, \
		654 ParentCancel, \
		123658 ManagerGadgetHelp, \
		14789 DisplayLicense, \
		3214789 DisplayCopyright, \
		123658 DisplayVersion",
	"Stroke*LicenseText.columns: 80",
	"Stroke*LicenseText.rows: 24",
	"Stroke*LicenseText.editMode: MULTI_LINE_EDIT",
	"Stroke*mainDA.width: 500",
	"Stroke*mainDA.height: 500",
	"Stroke*mainDA.strokes: 654 ClearDA \
			 14789 DisplayLicense, \
			 3214789 DisplayCopyright, \
			 123658 DisplayVersion, \
			 4563214789 Exit",
	"*strokeDebug: True",
	"*mainDA*translations: #override " DEFAULT_STROKE_TRANSLATION,
	"*XmDialogShell*translations: #override" DEFAULT_STROKE_TRANSLATION,
	NULL
};

/* ******************** */

void debug(int no, ...)
{
va_list args;
char    *fmt;
int     level;

        va_start(args, no);
        level = va_arg(args,int);
        if (level <= AppResources.debug)
        {
                fmt = va_arg(args,char *);
                vfprintf(stderr,fmt,args);
        }
        va_end(args);
}

/* ******************** */

static void WorkingDialogCB(Widget W, Boolean *Abort)
{
	*Abort = True;
	XtCallActionProc(AppShell, "PlaySound", NULL, &AppResources.CancelSound, 1);
}

/* ******************** */

Widget WorkingDialog(String Msg, Boolean *Abort)
{
static Widget Dialog = NULL;
XmString string;

	if (Dialog == NULL)
	{
		Dialog = XmCreateWorkingDialog(AppShell ,"WorkDialog",NULL,0);
                XtUnmanageChild(XmMessageBoxGetChild(Dialog,XmDIALOG_OK_BUTTON));
		XtAddCallback(Dialog,XmNcancelCallback,(XtCallbackProc)WorkingDialogCB,Abort);
	}
	string = XmStringCreateSimple(Msg);
	XtVaSetValues(Dialog,
		XmNmessageString, string,
		NULL);
	XmStringFree(string);
	XtManageChild(Dialog);
	return(Dialog);
}

/* ******************** */

void UpdateResources(String string)
{
FILE *xrdb;

	debug(5,"UpdateResources(%s)\n",string);
	xrdb = popen("xrdb -merge","w");
	fprintf(xrdb,"%s\n",string);
	pclose(xrdb);
}

/* ******************** */

static void DoNothing(void)
{
	/*
		This is necessary to ensure not more than 250 mS delay from
		when a SIGINT is delivered. Without it the Xlib select will
		be re-started and the WorkProc will not be noticed until
		another Xevent is processed!
	*/
	XtAppAddTimeOut(Context,250,(XtTimerCallbackProc)DoNothing,NULL);
}

/* ******************** */

void Info(char *msg)
{
static Widget   InfoDialog = NULL;
XmString        string;

        debug(5,"%s: Info(%s)\n",Program,msg);
        if (InfoDialog == NULL)
        {
                InfoDialog = XmCreateInformationDialog(AppShell,"Info",NULL,0);
                XtVaSetValues(InfoDialog,
                	XmNmessageAlignment,XmALIGNMENT_CENTER,
                	NULL);
                XtUnmanageChild(XmMessageBoxGetChild(InfoDialog,XmDIALOG_HELP_BUTTON));
        }
	if (msg != NULL)
	{
		string = XmStringCreateLtoR(msg,XmSTRING_DEFAULT_CHARSET);
		XtVaSetValues(InfoDialog,
			XmNmessageString,string,
			NULL);
		XmStringFree(string);
		XtManageChild(InfoDialog);
	}
	else
	{
		XtUnmanageChild(InfoDialog);
	}
}

/* ******************** */

#if 0
static void ToggleBoolean(Widget W, Boolean *state)
{
String ResourceString;
String Name;
String Class;

        debug(5,"ToggleBoolean(%s)\n",XtName(W));
        *state = XmToggleButtonGetState(W);
        XtGetApplicationNameAndClass(XtDisplay(W), &Name, &Class);
        ResourceString = XtMalloc(strlen(Class) + strlen(XtName(W)) + 6);
        sprintf(ResourceString,"%s.%s: %i",Class,XtName(W),*state);
        UpdateResources(ResourceString);
        XtFree(ResourceString);
}
#endif

/* ******************** */

static void ToggleStrokeDebug(Widget W, Boolean *state)
{
	StrokeSetDebug(XtNameToWidget(AppShell,"*mainDA"), XmToggleButtonGetState(W));
}

/* ******************** */

static Widget CreateOptionPulldown(Widget parent, Arg *args_in, int n_in)
{
Widget  PulldownMenu,
        Button;

        PulldownMenu = XmCreatePulldownMenu(parent,"OptionPullDown",args_in,n_in);
        Button = XmCreateCascadeButton(parent,"Options",NULL,0);
        XtVaSetValues(Button,
        	XmNsubMenuId,PulldownMenu,
        	NULL);
        XtManageChild(Button);

        Button = XmCreatePushButton(PulldownMenu,"DebugLevel",NULL,0);
        XtManageChild(Button);
        XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)XltSelectDebugLevel,&AppResources.debug);

	{
	Boolean *Flag;

	Flag = XltSoundInitialize(parent),
	XltSoundEnable(AppResources.Sounds);
        /*
        Button = XmCreateToggleButton(PulldownMenu,"Sounds",NULL,0);
        */
        Button = XmCreatePushButton(PulldownMenu,"SetupSounds",NULL,0);
        XtManageChild(Button);
        /*
        XmToggleButtonSetState(Button,
        	*Flag,
        	False);
        XtAddCallback(Button,XmNvalueChangedCallback,(XtCallbackProc)ToggleBoolean,Flag);
        */
        XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)XltSoundSetup,Flag);
        }

        Button = XmCreateToggleButton(PulldownMenu,"DebugStrokes",NULL,0);
        XtManageChild(Button);
        XtAddCallback(Button,XmNvalueChangedCallback,(XtCallbackProc)ToggleStrokeDebug,NULL);

        return(PulldownMenu);
}

/* ******************** */

static void DisplayLicense(void)
{
static Widget Dialog;
String msg = "\n\
		    GNU GENERAL PUBLIC LICENSE\n\
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION\n\
\n\
  0. This License applies to any program or other work which contains\n\
a notice placed by the copyright holder saying it may be distributed\n\
under the terms of this General Public License.  The \"Program\", below,\n\
refers to any such program or work, and a \"work based on the Program\"\n\
means either the Program or any derivative work under copyright law:\n\
that is to say, a work containing the Program or a portion of it,\n\
either verbatim or with modifications and/or translated into another\n\
language.  (Hereinafter, translation is included without limitation in\n\
the term \"modification\".)  Each licensee is addressed as \"you\".\n\
\n\
Activities other than copying, distribution and modification are not\n\
covered by this License; they are outside its scope.  The act of\n\
running the Program is not restricted, and the output from the Program\n\
is covered only if its contents constitute a work based on the\n\
Program (independent of having been made by running the Program).\n\
Whether that is true depends on what the Program does.\n\
\n\
  1. You may copy and distribute verbatim copies of the Program's\n\
source code as you receive it, in any medium, provided that you\n\
conspicuously and appropriately publish on each copy an appropriate\n\
copyright notice and disclaimer of warranty; keep intact all the\n\
notices that refer to this License and to the absence of any warranty;\n\
and give any other recipients of the Program a copy of this License\n\
along with the Program.\n\
\n\
You may charge a fee for the physical act of transferring a copy, and\n\
you may at your option offer warranty protection in exchange for a fee.\n\
\n\
  2. You may modify your copy or copies of the Program or any portion\n\
of it, thus forming a work based on the Program, and copy and\n\
distribute such modifications or work under the terms of Section 1\n\
above, provided that you also meet all of these conditions:\n\
\n\
    a) You must cause the modified files to carry prominent notices\n\
    stating that you changed the files and the date of any change.\n\
\n\
    b) You must cause any work that you distribute or publish, that in\n\
    whole or in part contains or is derived from the Program or any\n\
    part thereof, to be licensed as a whole at no charge to all third\n\
    parties under the terms of this License.\n\
\n\
    c) If the modified program normally reads commands interactively\n\
    when run, you must cause it, when started running for such\n\
    interactive use in the most ordinary way, to print or display an\n\
    announcement including an appropriate copyright notice and a\n\
    notice that there is no warranty (or else, saying that you provide\n\
    a warranty) and that users may redistribute the program under\n\
    these conditions, and telling the user how to view a copy of this\n\
    License.  (Exception: if the Program itself is interactive but\n\
    does not normally print such an announcement, your work based on\n\
    the Program is not required to print an announcement.)\n\
\n\
These requirements apply to the modified work as a whole.  If\n\
identifiable sections of that work are not derived from the Program,\n\
and can be reasonably considered independent and separate works in\n\
themselves, then this License, and its terms, do not apply to those\n\
sections when you distribute them as separate works.  But when you\n\
distribute the same sections as part of a whole which is a work based\n\
on the Program, the distribution of the whole must be on the terms of\n\
this License, whose permissions for other licensees extend to the\n\
entire whole, and thus to each and every part regardless of who wrote it.\n\
\n\
Thus, it is not the intent of this section to claim rights or contest\n\
your rights to work written entirely by you; rather, the intent is to\n\
exercise the right to control the distribution of derivative or\n\
collective works based on the Program.\n\
\n\
In addition, mere aggregation of another work not based on the Program\n\
with the Program (or with a work based on the Program) on a volume of\n\
a storage or distribution medium does not bring the other work under\n\
the scope of this License.\n\
\n\
  3. You may copy and distribute the Program (or a work based on it,\n\
under Section 2) in object code or executable form under the terms of\n\
Sections 1 and 2 above provided that you also do one of the following:\n\
\n\
    a) Accompany it with the complete corresponding machine-readable\n\
    source code, which must be distributed under the terms of Sections\n\
    1 and 2 above on a medium customarily used for software interchange; or,\n\
\n\
    b) Accompany it with a written offer, valid for at least three\n\
    years, to give any third party, for a charge no more than your\n\
    cost of physically performing source distribution, a complete\n\
    machine-readable copy of the corresponding source code, to be\n\
    distributed under the terms of Sections 1 and 2 above on a medium\n\
    customarily used for software interchange; or,\n\
\n\
    c) Accompany it with the information you received as to the offer\n\
    to distribute corresponding source code.  (This alternative is\n\
    allowed only for noncommercial distribution and only if you\n\
    received the program in object code or executable form with such\n\
    an offer, in accord with Subsection b above.)\n\
\n\
The source code for a work means the preferred form of the work for\n\
making modifications to it.  For an executable work, complete source\n\
code means all the source code for all modules it contains, plus any\n\
associated interface definition files, plus the scripts used to\n\
control compilation and installation of the executable.  However, as a\n\
special exception, the source code distributed need not include\n\
anything that is normally distributed (in either source or binary\n\
form) with the major components (compiler, kernel, and so on) of the\n\
operating system on which the executable runs, unless that component\n\
itself accompanies the executable.\n\
\n\
If distribution of executable or object code is made by offering\n\
access to copy from a designated place, then offering equivalent\n\
access to copy the source code from the same place counts as\n\
distribution of the source code, even though third parties are not\n\
compelled to copy the source along with the object code.\n\
\n\
  4. You may not copy, modify, sublicense, or distribute the Program\n\
except as expressly provided under this License.  Any attempt\n\
otherwise to copy, modify, sublicense or distribute the Program is\n\
void, and will automatically terminate your rights under this License.\n\
However, parties who have received copies, or rights, from you under\n\
this License will not have their licenses terminated so long as such\n\
parties remain in full compliance.\n\
\n\
  5. You are not required to accept this License, since you have not\n\
signed it.  However, nothing else grants you permission to modify or\n\
distribute the Program or its derivative works.  These actions are\n\
prohibited by law if you do not accept this License.  Therefore, by\n\
modifying or distributing the Program (or any work based on the\n\
Program), you indicate your acceptance of this License to do so, and\n\
all its terms and conditions for copying, distributing or modifying\n\
the Program or works based on it.\n\
\n\
  6. Each time you redistribute the Program (or any work based on the\n\
Program), the recipient automatically receives a license from the\n\
original licensor to copy, distribute or modify the Program subject to\n\
these terms and conditions.  You may not impose any further\n\
restrictions on the recipients' exercise of the rights granted herein.\n\
You are not responsible for enforcing compliance by third parties to\n\
this License.\n\
\n\
  7. If, as a consequence of a court judgment or allegation of patent\n\
infringement or for any other reason (not limited to patent issues),\n\
conditions are imposed on you (whether by court order, agreement or\n\
otherwise) that contradict the conditions of this License, they do not\n\
excuse you from the conditions of this License.  If you cannot\n\
distribute so as to satisfy simultaneously your obligations under this\n\
License and any other pertinent obligations, then as a consequence you\n\
may not distribute the Program at all.  For example, if a patent\n\
license would not permit royalty-free redistribution of the Program by\n\
all those who receive copies directly or indirectly through you, then\n\
the only way you could satisfy both it and this License would be to\n\
refrain entirely from distribution of the Program.\n\
\n\
If any portion of this section is held invalid or unenforceable under\n\
any particular circumstance, the balance of the section is intended to\n\
apply and the section as a whole is intended to apply in other\n\
circumstances.\n\
\n\
It is not the purpose of this section to induce you to infringe any\n\
patents or other property right claims or to contest validity of any\n\
such claims; this section has the sole purpose of protecting the\n\
integrity of the free software distribution system, which is\n\
implemented by public license practices.  Many people have made\n\
generous contributions to the wide range of software distributed\n\
through that system in reliance on consistent application of that\n\
system; it is up to the author/donor to decide if he or she is willing\n\
to distribute software through any other system and a licensee cannot\n\
impose that choice.\n\
\n\
This section is intended to make thoroughly clear what is believed to\n\
be a consequence of the rest of this License.\n\
\n\
  8. If the distribution and/or use of the Program is restricted in\n\
certain countries either by patents or by copyrighted interfaces, the\n\
original copyright holder who places the Program under this License\n\
may add an explicit geographical distribution limitation excluding\n\
those countries, so that distribution is permitted only in or among\n\
countries not thus excluded.  In such case, this License incorporates\n\
the limitation as if written in the body of this License.\n\
\n\
  9. The Free Software Foundation may publish revised and/or new versions\n\
of the General Public License from time to time.  Such new versions will\n\
be similar in spirit to the present version, but may differ in detail to\n\
address new problems or concerns.\n\
\n\
Each version is given a distinguishing version number.  If the Program\n\
specifies a version number of this License which applies to it and \"any\n\
later version\", you have the option of following the terms and conditions\n\
either of that version or of any later version published by the Free\n\
Software Foundation.  If the Program does not specify a version number of\n\
this License, you may choose any version ever published by the Free Software\n\
Foundation.\n\
\n\
  10. If you wish to incorporate parts of the Program into other free\n\
programs whose distribution conditions are different, write to the author\n\
to ask for permission.  For software which is copyrighted by the Free\n\
Software Foundation, write to the Free Software Foundation; we sometimes\n\
make exceptions for this.  Our decision will be guided by the two goals\n\
of preserving the free status of all derivatives of our free software and\n\
of promoting the sharing and reuse of software generally.\n\
\n\
			    NO WARRANTY\n\
\n\
  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY\n\
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN\n\
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES\n\
PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED\n\
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n\
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS\n\
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE\n\
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,\n\
REPAIR OR CORRECTION.\n\
\n\
  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n\
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR\n\
REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,\n\
INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING\n\
OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED\n\
TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY\n\
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER\n\
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE\n\
POSSIBILITY OF SUCH DAMAGES.\n\
";

	if (Dialog == NULL)
	{
	Widget Text;

		Dialog = XmCreateMessageDialog(AppShell, "LicenseDialog", NULL, 0);
		XtVaSetValues(XtParent(Dialog),
			XmNtitle, "GNU GENERAL PUBLIC LICENSE",
			NULL);
                XtUnmanageChild(XmMessageBoxGetChild(Dialog,XmDIALOG_HELP_BUTTON));
		Text = XmCreateScrolledText(Dialog, "LicenseText", NULL, 0);
		XtVaSetValues(Text,
			XmNeditable, False,
			XmNvalue, msg,
			NULL);
		XtManageChild(Text);
	}
	XtManageChild(Dialog);
}


/* ******************** */

static void DisplayCopyright(void)
{
String msg = "stroke $Revision: 1.6 $\n\
\n\
Copyright (C) 1997 Richard Scott\n\
\n\
stroke comes with ABSOLUTELY NO WARRANTY\n\
for details click `Help->License'.\n\
This is free software, and you are welcome to\n\
redistribute it under certain conditions\n\
click `Help->License' for details.\n\
";

	debug(5,"DisplayCopyright(%s)\n",rcsid);
        Info(msg);
}

/* ******************** */

static void DisplayVersion(void)
{
String msg = "stroke\n\
A stroke test program for the X11\n\
window system and Motif\n\
\n\
by Rick Scott <rwscott@magi.com>\n\
$Revision: 1.6 $";

	debug(5,"DisplayVersion(%s)\n",rcsid);
        Info(msg);
}

/* ******************** */

static Widget CreateFilePulldown(Widget  parent, Arg *args_in, int n_in)
{
Widget  PulldownMenu,
        Button;

        PulldownMenu = XmCreatePulldownMenu(parent,"FilePullDown",args_in,n_in);
        Button = XmCreateCascadeButton(parent,"File",NULL,0);
        XtVaSetValues(Button,
        	XmNsubMenuId,PulldownMenu,
        	NULL);
        XtManageChild(Button);

        XtManageChild(XmCreateSeparator(PulldownMenu,"Separator",NULL,0));

        Button = XmCreatePushButton(PulldownMenu,"Quit",NULL,0);
        XtManageChild(Button);
        XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)exit,NULL);

        return(PulldownMenu);
}

/* ******************** */

static void ClearDA(void)
{
Widget mainDA = XtNameToWidget(AppShell, "*mainDA");

	XClearArea(XtDisplay(mainDA),
		XtWindow(mainDA),
		0,0,0,0,False);
}

/* ******************** */

static Widget CreateEditPulldown(Widget  parent, Arg *args_in, int n_in)
{
Widget  PulldownMenu,
        Button;

        PulldownMenu = XmCreatePulldownMenu(parent,"ViewPullDown",args_in,n_in);
        Button = XmCreateCascadeButton(parent,"View",NULL,0);
        XtVaSetValues(Button,
        	XmNsubMenuId,PulldownMenu,
        	NULL);
        XtManageChild(Button);

        Button = XmCreatePushButton(PulldownMenu,"ClearDA",NULL,0);
        XtManageChild(Button);
        XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)ClearDA,NULL);

        return(PulldownMenu);
}

/* ******************** */

static Widget CreateHelpPulldown(Widget  parent, Arg *args_in, int n_in)
{
Widget  HelpMenu,
        Button;

        HelpMenu = XmCreatePulldownMenu(parent,"HelpPullDown",args_in,n_in);
        Button = XmCreateCascadeButton(parent,"Help",NULL,0);
        XtVaSetValues(Button,
        	XmNsubMenuId,HelpMenu,
        	NULL);
        XtManageChild(Button);

        XtVaSetValues(parent,
        	XmNmenuHelpWidget,Button,
        	NULL);
        Button = XmCreatePushButton(HelpMenu,"Version",NULL,0);
        XtManageChild(Button);
        XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)DisplayVersion,NULL);

        Button = XmCreatePushButton(HelpMenu,"Copyright",NULL,0);
        XtManageChild(Button);
        XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)DisplayCopyright,NULL);

        Button = XmCreatePushButton(HelpMenu,"License",NULL,0);
        XtManageChild(Button);
        XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)DisplayLicense,NULL);

        return(HelpMenu);
}

/* ******************** */

static Widget CreateMenus(Widget parent, Arg *args_in, int n_in)
{
Widget  MenuBar;

        MenuBar = XmCreateMenuBar(parent,"MenuBar",args_in,n_in);
        CreateFilePulldown(MenuBar,NULL,0);
        CreateEditPulldown(MenuBar,NULL,0);
        CreateOptionPulldown(MenuBar,NULL,0);
        CreateHelpPulldown(MenuBar,NULL,0);
        return(MenuBar);
}

/* ******************** */

static void GetApplicationResources(Widget W)
{
	debug(9,"GetApplicationResources(%s)\n",XtName(W));
	XtGetApplicationResources(W,
		&AppResources,
		resources, XtNumber(resources),
		NULL, 0);
}

/* ******************** */

static void ExposeDA(void)
{
}

/* ******************** */

static void ResizeDA(void)
{
}

/* ******************** */

static void InputDA(void)
{
}

/* ******************** */

int main(int	argc, char *argv[])
{
Widget		Form,
		Menus;
#ifdef USE_EDITRES
void _EditResCheckMessages();
#endif

	Program = strrchr(argv[0],'/');
	if (Program == NULL)
	{
		Program = argv[0];
	}
	else
	{
		Program++;
	}

	AppShell = XtAppInitialize(&Context,
		"Stroke",
		opTable,XtNumber(opTable),
		&argc,argv,
		FallBack,
		NULL,0);
	StrokeInitialize(AppShell);
#ifdef USE_EDITRES
	XtAddEventHandler(AppShell,(EventMask) 0, True, _EditResCheckMessages,NULL);
#endif
	
	GetApplicationResources(AppShell);
	if (AppResources.Fallback)
	{
		XltDisplayFallbackResources(FallBack);
		exit(0);
	}
	XtAppAddActions(Context,
		GlobalActions, XtNumber(GlobalActions));
#if 0
	{
	XpmAttributes attrib;
	Pixmap pixmap,mask;
	GC gc;
	Pixel bg;

		attrib.valuemask = 0;
		XpmCreatePixmapFromData(XtDisplay(AppShell),
			XRootWindowOfScreen(XtScreen(AppShell)),
			icon,
			&pixmap,
			&mask,
			&attrib);
		if (mask != (Pixmap)NULL)
		{
			gc = XDefaultGCOfScreen(XtScreen(AppShell));
			XtVaGetValues(AppShell,
				XmNbackground, &bg,
				NULL);
			XSetBackground(XtDisplay(AppShell),gc,bg);
			XSetFunction(XtDisplay(AppShell),gc,GXor);
			XCopyPlane(XtDisplay(AppShell),
				mask,pixmap,
				gc,
				0,0,attrib.width,attrib.height,
				0,0,
				1);
			XFreePixmap(XtDisplay(AppShell),mask);
		}
		XpmFreeAttributes(&attrib);

		XtVaSetValues(AppShell,
			XmNiconPixmap,pixmap,
			NULL);
	}	
#endif

	switch (argc)
	{
	default:
		break;
	}
	Form = XmCreateForm(AppShell,"TopLevelForm",NULL,0);
	XtVaSetValues(Form,
		XmNresizePolicy,XmRESIZE_ANY,
		NULL);

	Menus = CreateMenus(Form,NULL,0);
	XtVaSetValues(Menus,
		XmNtopAttachment,XmATTACH_FORM,
		XmNleftAttachment,XmATTACH_FORM,
		XmNrightAttachment,XmATTACH_FORM,
		NULL);
	XtManageChild(Menus);

#ifdef HAS_TOOLBAR
	{
        Widget Button;

		ToolBar = CreateToolBar(Form,"ToolBar",NULL,0);
		XtVaSetValues(ToolBar,
			XmNtopAttachment,XmATTACH_WIDGET,
			XmNtopWidget,Menus,
			XmNleftAttachment,XmATTACH_FORM,
			XmNrightAttachment,XmATTACH_FORM,
			NULL);
        
		Button = ToolBarAddItem(ToolBar,"Save",  floppy3);
		XtManageChild(Button);
		/*
		XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)SavePortfolioCallback,&Portfolio);
		*/

		Button = ToolBarAddItem(ToolBar,"Open",  open);
		XtManageChild(Button);
		XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)SelectArtworkFile, NULL);

		Button = ToolBarAddItem(ToolBar,"ZoomAll",  full_mag);
		XtManageChild(Button);
		XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)ViewAll, 0);

		Button = ToolBarAddItem(ToolBar,"ZoomOut",  minus_mag);
		XtManageChild(Button);
		XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)ZoomOut, NULL);

		Button = ToolBarAddItem(ToolBar,"ZoomIn",  plus_mag);
		XtManageChild(Button);
		XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)ZoomIn, NULL);

		Button = ToolBarAddItem(ToolBar,"EditAper",  aperture);
		XtManageChild(Button);
		XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)ViewAperture, (XtPointer)&Apertures);

		Button = ToolBarAddItem(ToolBar,"EditArt",  layers);
		XtManageChild(Button);
		XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)ViewLayers,(XtPointer)&Layers);

		Button = ToolBarAddItem(ToolBar,"Print",  print);
		XtManageChild(Button);
		XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)Print,&Layers);

		Button = ToolBarAddItem(ToolBar,"Exit", stopsign);
		XtManageChild(Button);
		XtAddCallback(Button,XmNactivateCallback,(XtCallbackProc)exit,NULL);
		XtVaSetValues(ToolBar,
			XmNmenuHelpWidget,Button,
			NULL);

		XtManageChild(ToolBar);
	}
#endif
	{
	Widget DrawingArea;

		DrawingArea = XmCreateDrawingArea(Form, "mainDA", NULL, 0);
		XtVaSetValues(DrawingArea,
			XmNtopAttachment, XmATTACH_WIDGET,
#ifdef HAS_TOOLBAR
			XmNtopWidget, ToolBar,
#else
			XmNtopWidget, Menus,
#endif
			NULL);
		XtAddCallback(DrawingArea,XmNexposeCallback,(XtCallbackProc)ExposeDA, NULL);
		XtAddCallback(DrawingArea,XmNresizeCallback,(XtCallbackProc)ResizeDA, NULL);
		XtAddCallback(DrawingArea,XmNinputCallback,(XtCallbackProc)InputDA, NULL);
		XtVaSetValues(DrawingArea,
			NULL);

		XtManageChild(DrawingArea);
	}
	
	XtManageChild(Form);
	XtRealizeWidget(AppShell);
	if (AppResources.RedirectErrors)
	{
	    XltRedirectStdErr(AppShell);
	}
	XtCallActionProc(AppShell, "PlaySound", NULL, &AppResources.StartupSound, 1);
	XtAppMainLoop(Context);
	return(0);
}
