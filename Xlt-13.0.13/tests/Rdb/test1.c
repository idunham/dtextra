#include <stdio.h>

#include <X11/Intrinsic.h>

#include "Xlt.h"

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
    XltRdbPutString(shell, "test", "test\nvalue 13");
    XtDestroyWidget(shell);
    return(0);
}
