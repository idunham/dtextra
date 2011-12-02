#ifndef XLTTIMER_H
#define XLTTIMER_H

#include <X11/Intrinsic.h>

typedef void *XltIntervalId;
typedef void (*XltTimerCallbackProc)(XtPointer client_data, XltIntervalId id);

#ifdef __cplusplus
extern "C"
{
#endif

    XltIntervalId XltAppAddTimeOut(XtAppContext app_context,
				     unsigned long interval,
				     XltTimerCallbackProc proc,
				     XtPointer client_data);
    unsigned long XltRemoveTimeOut(XltIntervalId data);
    unsigned long XltTimeOutQuery(XltIntervalId data);

#ifdef __cplusplus
}				/* Close scope of 'extern "C"' declaration which encloses file. */
#endif


#endif
