#include <stdlib.h>
#include <stdio.h>
#include <Xm/Label.h>
#include <../../lib/AppShell.h>

static XrmOptionDescRec options[] = {
	XltAppShellXrmOptions
};
#define N_OPTIONS (sizeof(options)/sizeof(options[0]))

static char *fallbacks[] = {
	"*label.labelString:	This is a label\\n\
with a couple lines\\n\
for space.\\n\
Trying running with -install\\n\
for a private colormap",
	NULL,
};

int
main(int argc, char **argv)
{
	Widget       top;
	XtAppContext ac;

	XtSetLanguageProc(NULL, NULL, NULL);

	top = XltAppInitialize(&ac, "AppShell", options, N_OPTIONS,
					&argc, argv, fallbacks, NULL, 0);

	XtCreateManagedWidget("label", xmLabelWidgetClass, top, NULL, 0);

	XtRealizeWidget(top);
	XtAppMainLoop(ac);

	return 0;
}
