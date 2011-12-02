/* $Header: /cvsroot/xlt/Xlt/tests/Host/test1.c,v 1.9 2002/01/09 22:34:25 rwscott Exp $ */

#include <stdlib.h>
#include <stdio.h>

#include <Xm/Form.h>

#include <BubbleButton.h>
#include <Host.h>
#include <Stroke.h>
#include <Sound.h>


static char *Fallback[] = {
	"*XmDialogShell*translations: #override" DEFAULT_STROKE_TRANSLATION,
	"*strokes: 456 ParentActivate, \
		   654 ParentCancel",
  	"*HostTranscript*columns: 80",
  	"*HostTranscript*rows: 24",
	NULL
};

static void
InputCallback(Widget W, String client_data, XltHostCallbackStruct *call_data)
{
	printf("InputCallback(%s,%s,%i)\n", XtName(W),client_data,call_data->len);
}

static void
BinaryInputCallback(Widget W, String client_data, XltHostCallbackStruct *call_data)
{
	printf("BinaryInputCallback(%s,%s,%i)\n", XtName(W),client_data,call_data->len);
}

static void
AsciiInputCallback(Widget W, Widget client_data, XltHostCallbackStruct *call_data)
{
	printf("AsciiInputCallback(%s,%s,%s)\n", XtName(W),XtName(client_data),call_data->data);
}

static void
Callback(Widget W)
{
	printf("ConnectCallback(%s)\n", XtName(W));
}

int
main(int argc, char **argv)
{
  Widget toplevel, one, form, Connect, Connect1;
  Widget Host, Host1;
  XtAppContext app;

  XtSetLanguageProc(NULL, NULL, NULL);

  toplevel = XtVaAppInitialize(&app, 
  	"Label", 
  	NULL, 0, 
  	&argc, argv, 
  	Fallback, 
  	NULL);
  StrokeInitialize(toplevel);
  XltSoundInitialize(toplevel);
  Host = XltCreateHost(toplevel, "Host", NULL, 0);
  XtVaSetValues(Host,
  	NULL);
  Host1 = XltCreateHost(toplevel, "Host1", NULL, 0);

  form = XmCreateForm(toplevel, "Form", NULL, 0);
  Connect = XltCreateBubbleButton(form, "Connect", NULL, 0);
  Connect1 = XltCreateBubbleButton(form, "Connect1", NULL, 0);

  one = XltHostCreateTranscript(form, Host, NULL, 0);
  XtVaSetValues(XtParent(one),
  	XmNtopAttachment, XmATTACH_WIDGET,
  	XmNtopWidget, Connect,
  	XmNbottomAttachment, XmATTACH_FORM,
  	XmNleftAttachment, XmATTACH_FORM,
  	XmNrightAttachment, XmATTACH_FORM,
  	NULL);
  XtVaSetValues(Connect,
  	XmNtopAttachment, XmATTACH_FORM,
  	XmNleftAttachment, XmATTACH_FORM,
  	NULL);
  XtVaSetValues(Connect1,
  	XmNtopAttachment, XmATTACH_FORM,
  	XmNleftAttachment, XmATTACH_WIDGET,
  	XmNleftWidget, Connect,
  	NULL);
  XtManageChild(one);

  XtManageChild(Connect);
  XtManageChild(Connect1);
  XtManageChild(form);
  XtAddCallback(Host, XltNconnectCallback, (XtCallbackProc)Callback, NULL);
  XtAddCallback(Host, XltNasciiInputCallback, (XtCallbackProc)AsciiInputCallback, one);
  XtAddCallback(Host, XltNbinaryInputCallback, (XtCallbackProc)BinaryInputCallback, NULL);
  XtAddCallback(Host, XltNinputCallback, (XtCallbackProc)InputCallback, NULL);
  XtAddCallback(Connect, XmNactivateCallback, (XtCallbackProc)XltHostSelect, Host);
  XtAddCallback(Connect1, XmNactivateCallback, (XtCallbackProc)XltHostSelect, Host1);
  XtRealizeWidget(toplevel);
  {
  String port;

  	XtVaGetValues(Host,
  		XltNport, &port,
  		NULL);
  	printf("PORT >%s<\n",port);
  }
  XtAppMainLoop(app);

  exit(0);
}
