#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/time.h>

#include "Timer.h"

typedef struct
{
    XtIntervalId id;
    struct timeval start_time;
    unsigned long interval;
    XltTimerCallbackProc proc;
    XtPointer client_data;
} XltInterval_t;

static void
_timeout(XtPointer client_data, XtIntervalId *id)
{
XltInterval_t *data = (XltInterval_t *) client_data;

    data->id = (XtIntervalId)NULL;
    (*data->proc) (data->client_data, data);
    XtFree((char *)data);
}

XltIntervalId
XltAppAddTimeOut(XtAppContext app_context, unsigned long interval,
		  XltTimerCallbackProc proc, XtPointer client_data)
{
XltInterval_t *data;

    data = (XltInterval_t *) XtMalloc(sizeof(XltInterval_t));
    data->interval = interval;
    data->proc = proc;
    data->client_data = client_data;
    gettimeofday(&data->start_time, NULL);
    data->id = XtAppAddTimeOut(app_context, data->interval, _timeout, data);
    return (data);
}

unsigned long
XltRemoveTimeOut(XltIntervalId client_data)
{
XltInterval_t *data = (XltInterval_t *) client_data;
unsigned long remaining;

    XtRemoveTimeOut(data->id);
    remaining = XltTimeOutQuery(client_data);
    XtFree((char *)data);
    return(remaining);
}

unsigned long
XltTimeOutQuery(XltIntervalId client_data)
{
    XltInterval_t *data = (XltInterval_t *) client_data;
    unsigned long elapsed;
    unsigned long remaining;
    struct timeval now;

    gettimeofday(&now, NULL);
    elapsed = (now.tv_sec - data->start_time.tv_sec) * 1000;
    elapsed += (now.tv_usec - data->start_time.tv_usec) / 1000;
    if (data->interval > elapsed)
    {
	remaining = data->interval - elapsed;
    }
    else
    {
	remaining = 0;
    }
    return (remaining);
}
