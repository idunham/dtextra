/*
 * $Id: ReqMach.c,v 1.4 1994/05/13 14:14:26 mallet Exp $
 * 
 * Copyright (c) 1991-1994  Lionel MALLET
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL Lionel MALLET	BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of Lionel MALLET shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization
 * from Lionel MALLET.
 *
 * Author:  Lionel Mallet, SIMULOG
 */

/*
 * $XConsortium: ReqMach.c,v 1.1 90/06/09 20:20:41 dmatic Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Davor Matic, MIT X Consortium
 */



#include <X11/IntrinsicP.h>
#include <X11/Xmu/Converters.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/Xfuncs.h>
#include "PixmapP.h"
    
#include <stdio.h>
#include <math.h>

#define XtStrlen(s)                   ((s) ? strlen(s) : 0)
#define abs(x)                        (((x) > 0) ? (x) : -(x))
#define min(x, y)                     (((x) < (y)) ? (x) : (y))
#define max(x, y)                     (((x) > (y)) ? (x) : (y))


/*****************************************************************************\
 * Request Machine: stacks up and handles requests from application calls.   * 
\*****************************************************************************/

/*
 * Searches for a request record of a request specified by its name.
 * Returns a pointer to the record or NULL if the request was not found.
 */
PWRequestRec *FindRequest(name)
    PWRequest name;
{
    int i;

    for (i = 0; i < pixmapClassRec.pixmap_class.num_requests; i++)
	if (!strcmp(name, pixmapClassRec.pixmap_class.requests[i].name))
	    return &pixmapClassRec.pixmap_class.requests[i];
    
    return NULL;
}

/*
 * Adds a request to the request stack and does proper initializations.
 * Returns TRUE if the request was found and FALSE otherwise.
 */
#if NeedFunctionPrototypes
Boolean PWAddRequest(Widget w, PWRequest name, Boolean trap, 
		     XtPointer call_data, Cardinal call_data_size)
#else
Boolean PWAddRequest(w, name, trap, call_data, call_data_size)
    Widget    w;
    PWRequest name;
    Boolean   trap;
    XtPointer   call_data;
    Cardinal  call_data_size;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    PWRequestRec *request;
    
    if((request = FindRequest(name)) != (PWRequestRec *)0) {
	if (_PWDEBUG)
	  fprintf(stderr, "Adding... Cardinal: %d\n", PW->pixmap.cardinal + 1);

	PW->pixmap.request_stack = (PWRequestStack *)
	    XtRealloc((char *)PW->pixmap.request_stack,
	     (Cardinal)((++PW->pixmap.cardinal + 1) * sizeof(PWRequestStack)));
	
	PW->pixmap.request_stack[PW->pixmap.cardinal].request = request;
	PW->pixmap.request_stack[PW->pixmap.cardinal].status = 
	    XtMalloc(request->status_size);
	PW->pixmap.request_stack[PW->pixmap.cardinal].trap = trap;
	PW->pixmap.request_stack[PW->pixmap.cardinal].call_data = 
	    XtMalloc(call_data_size);
	bcopy(call_data, 
	      PW->pixmap.request_stack[PW->pixmap.cardinal].call_data,
	      call_data_size);

	return True;
    }
    else {
	XtAppWarning(XtWidgetToApplicationContext(w),
		  "bad request name.  PixmapWidget");
	return False;
    }
}

/*
 * Engages the request designated by the current parameter.
 * Returnes TRUE if the request has an engage function and FALSE otherwise.
 */
Boolean Engage(PW, current)
    PixmapWidget PW;
    Cardinal current;
{
    PW->pixmap.current = current;
    
    if (_PWDEBUG)
	fprintf(stderr, "Request: %s\n", 
		PW->pixmap.request_stack[current].request->name);
  
    if (PW->pixmap.request_stack[current].request->engage) {
	(*PW->pixmap.request_stack[current].request->engage)
	    ((Widget) PW,
	     PW->pixmap.request_stack[current].status,
	     PW->pixmap.request_stack[current].request->engage_client_data,
	     PW->pixmap.request_stack[current].call_data);
	return True;
    }
    else
	return False;
}


/*
 * Scans down the request stack removing all requests untill it finds 
 * one to be trapped.
 */
void TrappingLoop(PW)
    PixmapWidget PW;
{

    if (_PWDEBUG)
	fprintf(stderr, "Scanning... Current: %d\n", PW->pixmap.current);
    if ((PW->pixmap.current > 0) 
	&& 
	(!PW->pixmap.request_stack[PW->pixmap.current--].trap)) {
	PWRemoveRequest((Widget) PW);
	TrappingLoop(PW);
    }
    else
	if (PW->pixmap.cardinal > 0) {
	    if (_PWDEBUG)
		fprintf(stderr, "Trapping... Current: %d\n", PW->pixmap.current+1);
	    if(!Engage(PW, ++PW->pixmap.current))
		PWTerminateRequest((Widget) PW, True);
	}
}
/*
 * Terimantes the current request and continues with next request if con = TRUE
 * Returnes TRUE if there is any number of requests left on the stack.
 */
#if NeedFunctionPrototypes
Boolean PWTerminateRequest(Widget w, Boolean cont)
#else
Boolean PWTerminateRequest(w, cont)
    Widget w;
    Boolean cont;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    if (PW->pixmap.current > 0) {
	if (_PWDEBUG)
	    fprintf(stderr, "Terminating... Current: %d\n", PW->pixmap.current);
    	if (PW->pixmap.request_stack[PW->pixmap.current].request->terminate)
	    (*PW->pixmap.request_stack[PW->pixmap.current].request->terminate)
		(w,
		 PW->pixmap.request_stack[PW->pixmap.current].status,
		 PW->pixmap.request_stack[PW->pixmap.current].request->terminate_client_data,
		 PW->pixmap.request_stack[PW->pixmap.current].call_data);
	
	if (cont) {
	    if (PW->pixmap.current == PW->pixmap.cardinal)
		TrappingLoop(PW);
	    else {
		if (_PWDEBUG)
		    fprintf(stderr, "Continuing... Current: %d\n", PW->pixmap.current+1);
		if (!Engage(PW, ++PW->pixmap.current))
		    PWTerminateRequest(w, True);
	    }
	}
	else
	    PW->pixmap.current = 0;
    }
    
    return PW->pixmap.current;
}

/*
 * Simple interface to PWTerminateRequest that takes only a widget.
 */
#if NeedFunctionPrototypes
void PWTerminate(Widget w)
#else
void PWTerminate(w)
    Widget w;
#endif
{
    PWTerminateRequest(w, True);
}

/*
 * Removes the top request from the request stack. If the request is active
 * it will terminate it.
 * Returns TRUE if the number of requests left on the stack != 0.
 */
#if NeedFunctionPrototypes
Boolean PWRemoveRequest(Widget w)
#else
Boolean PWRemoveRequest(w)
    Widget w;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    if (PW->pixmap.cardinal > 0) {
	if (_PWDEBUG)
	    fprintf(stderr, "Removing... Cardinal: %d\n", PW->pixmap.cardinal);
	if (PW->pixmap.current == PW->pixmap.cardinal)
	    PWTerminateRequest(w, False);
	
	if (PW->pixmap.request_stack[PW->pixmap.cardinal].request->remove)
	    (*PW->pixmap.request_stack[PW->pixmap.cardinal].request->remove)
		(w,
		 PW->pixmap.request_stack[PW->pixmap.cardinal].status,
		 PW->pixmap.request_stack[PW->pixmap.cardinal].request->remove_client_data,
		 PW->pixmap.request_stack[PW->pixmap.cardinal].call_data);
	
	XtFree(PW->pixmap.request_stack[PW->pixmap.cardinal].status);
	XtFree(PW->pixmap.request_stack[PW->pixmap.cardinal].call_data);
	PW->pixmap.request_stack = (PWRequestStack *)
	    XtRealloc((char *)PW->pixmap.request_stack,
	     (Cardinal)((--PW->pixmap.cardinal + 1) * sizeof(PWRequestStack)));
	
	return True;
    }
    else 
	return False;
}

#if NeedFunctionPrototypes
void PWRemoveAllRequests(Widget w)
#else
void PWRemoveAllRequests(w)
    Widget w;
#endif
{
    while (PWRemoveRequest(w)) {/* removes all requests from the stack */}
}

/*
 * Adds the request to the stack and performs engaging ritual.
 * Returns TRUE if the request was found, FALSE otherwise.
 */
#if NeedFunctionPrototypes
Boolean PWEngageRequest(Widget w, PWRequest name, Boolean trap, 
			XtPointer call_data, Cardinal call_data_size)
#else
Boolean PWEngageRequest(w, name, trap, call_data, call_data_size)
    Widget w;
    PWRequest name;
    Boolean trap;
    XtPointer call_data;
    Cardinal call_data_size;
#endif
{
    PixmapWidget PW = (PixmapWidget) w;
    
    if (PWAddRequest(w, name, trap, call_data, call_data_size)) {
	PWTerminateRequest(w, False);
	if (_PWDEBUG)
	    fprintf(stderr, "Engaging... Cardinal: %d\n", PW->pixmap.cardinal);
	if (!Engage(PW, PW->pixmap.cardinal))
	    PWTerminateRequest(w, True);
	
	return True;
    }
    else
	return False;
}

/************************* End of the Request Machine ************************/




