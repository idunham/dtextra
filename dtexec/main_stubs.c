/*
**LIBS: -lXm -lXt -lX11
*/

#include <stdlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <Xm/PushB.h>

#include "main.h"


extern void XDmanage_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDunmanage_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDpopup_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDpopdown_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDmap_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDunmap_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDenable_link( Widget w, XtPointer client_data, XtPointer call_data);
extern void XDdisable_link( Widget w, XtPointer client_data, XtPointer call_data);


void clearInW_CB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmPushButtonCallbackStruct *call_data = (XmPushButtonCallbackStruct *) xt_call_data ;
	
	XmTextSetString(inW, (char*)0);
}

void clearOutW_CB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmPushButtonCallbackStruct *call_data = (XmPushButtonCallbackStruct *) xt_call_data ;

	XmTextSetString(outW, (char*)0);
}

void runCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmPushButtonCallbackStruct *call_data = (XmPushButtonCallbackStruct *) xt_call_data ;
	
	char* cmd;
	cmd = (char*)XmTextGetString(inW);
	
	FILE *fp;
	fp = popen(cmd, "r");
	
	char c[1024];
	XmTextPosition pos;
	pos = XmTextGetLastPosition(outW);
	
	while(fgets(c, 1024, fp)) {
		pos = XmTextGetLastPosition(outW);
		XmTextInsert(outW, pos, c);
	#ifdef DEBUG
		printf("fp: %s", c);
		printf("pos: %d\n", (int)pos);
	#endif
	}

	pclose(fp);
}

void quitCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	XmPushButtonCallbackStruct *call_data = (XmPushButtonCallbackStruct *) xt_call_data ;
	
	exit(0);
}

