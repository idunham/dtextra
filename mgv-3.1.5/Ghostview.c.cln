/*
 * Ghostview.c -- Ghostview widget.
 * Copyright (C) 1992  Timothy O. Theisen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Author: Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 *
 * The GhostviewPSBBox() was added by mu@trends.net who also did some
 * code reformatting and wrapped an sprintf() in setlocale()'s to
 * make sure floating point values were written with decimal points.
 * The gsLocale resource was added to control this.  The widget was
 * also modified to make its own copy of the GvNfilename and
 * GvNarguments resources to avoid free memory reads (thank you purify!).
 * The anti-aliasing support was also added.  I also reformatted the code
 * to make it easier on my eyes.
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/Ghostview.c 1.75 1998/09/02 02:49:39 $")

#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <locale.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xproto.h>

#include <wlib/stdlib.h>
#include <wlib/sanity.h>
#include <mine/GhostviewP.h>
#include <mine/util.h>

/*
 * EAGAIN and EWOULDBLOCK, AFAIK, mean the same thing.
 */
#if !defined(EWOULDBLOCK)
#	define EWOULDBLOCK EAGAIN
#endif

/*
 * GV_BUFSIZ is set to the minimum POSIX PIPE_BUF to ensure that
 * nonblocking writes to ghostscript will work properly.
 */
#define	GV_BUFSIZ	512

/*
 * Misc. convenience macros.
 */
#define	core_width(w) \
	(((GhostviewWidget)(w))->core.width)
#define	core_height(w) \
	(((GhostviewWidget)(w))->core.height)
#define	core_bpixmap(w) \
	(((GhostviewWidget)(w))->core.background_pixmap)
#define	core_depth(w) \
	(((GhostviewWidget)(w))->core.depth)
#define	core_background_pixel(w) \
	(((GhostviewWidget)(w))->core.background_pixel)

#define	msg_callback(w) \
	(((GhostviewWidget)(w))->ghostview.message_callback)
#define	notify_callback(w) \
	(((GhostviewWidget)(w))->ghostview.notify_callback)
#define	output_callback(w) \
	(((GhostviewWidget)(w))->ghostview.output_callback)
#define	done_atom(w) \
	(((GhostviewWidget)(w))->ghostview.done)
#define	page_atom(w) \
	(((GhostviewWidget)(w))->ghostview.page)
#define	ghostview_atom(w) \
	(((GhostviewWidget)(w))->ghostview.ghostview)
#define	gv_colors_atom(w) \
	(((GhostviewWidget)(w))->ghostview.gv_colors)
#define	next_atom(w) \
	(((GhostviewWidget)(w))->ghostview.next)
#define	gv_cursor(w) \
	(((GhostviewWidget)(w))->ghostview.cursor)
#define	gv_busy(w) \
	(((GhostviewWidget)(w))->ghostview.busy)
#define	gv_mwin(w) \
	(((GhostviewWidget)(w))->ghostview.mwin)
#define	llx(w) \
	(((GhostviewWidget)(w))->ghostview.llx)
#define	lly(w) \
	(((GhostviewWidget)(w))->ghostview.lly)
#define	urx(w) \
	(((GhostviewWidget)(w))->ghostview.urx)
#define	ury(w) \
	(((GhostviewWidget)(w))->ghostview.ury)
#define	xdpi(w) \
	(((GhostviewWidget)(w))->ghostview.xdpi)
#define	ydpi(w) \
	(((GhostviewWidget)(w))->ghostview.ydpi)
#define	orientation(w) \
	(((GhostviewWidget)(w))->ghostview.orientation)
#define	buffer_bytes_left(w) \
	(((GhostviewWidget)(w))->ghostview.buffer_bytes_left)
#define	ps_input(w) \
	(((GhostviewWidget)(w))->ghostview.ps_input)
#define	bytes_left(w) \
	(((GhostviewWidget)(w))->ghostview.bytes_left)
#define	input_buffer(w) \
	(((GhostviewWidget)(w))->ghostview.input_buffer)
#define	input_buffer_ptr(w) \
	(((GhostviewWidget)(w))->ghostview.input_buffer_ptr)
#define	interpreter_input(w) \
	(((GhostviewWidget)(w))->ghostview.interpreter_input)
#define	interpreter_input_id(w) \
	(((GhostviewWidget)(w))->ghostview.interpreter_input_id)
#define	changed(w) \
	(((GhostviewWidget)(w))->ghostview.changed)
#define	gs_width(w) \
	(((GhostviewWidget)(w))->ghostview.gs_width)
#define	gs_height(w) \
	(((GhostviewWidget)(w))->ghostview.gs_height)
#define	use_bpixmap(w) \
	(((GhostviewWidget)(w))->ghostview.use_bpixmap)
#define	left_margin(w) \
	(((GhostviewWidget)(w))->ghostview.left_margin)
#define	right_margin(w) \
	(((GhostviewWidget)(w))->ghostview.right_margin)
#define	top_margin(w) \
	(((GhostviewWidget)(w))->ghostview.top_margin)
#define	bottom_margin(w) \
	(((GhostviewWidget)(w))->ghostview.bottom_margin)
#define	palette(w) \
	(((GhostviewWidget)(w))->ghostview.palette)
#define	foreground(w) \
	(((GhostviewWidget)(w))->ghostview.foreground)
#define	gs_locale(w) \
	(((GhostviewWidget)(w))->ghostview.gs_locale)
#define	disable_start(w) \
	(((GhostviewWidget)(w))->ghostview.disable_start)
#define	interpreter_pid(w) \
	(((GhostviewWidget)(w))->ghostview.interpreter_pid)
#define	gv_filename(w) \
	(((GhostviewWidget)(w))->ghostview.filename)
#define	interpreter_output(w) \
	(((GhostviewWidget)(w))->ghostview.interpreter_output)
#define	interpreter_output_id(w) \
	(((GhostviewWidget)(w))->ghostview.interpreter_output_id)
#define	interpreter_error(w) \
	(((GhostviewWidget)(w))->ghostview.interpreter_error)
#define	interpreter_error_id(w) \
	(((GhostviewWidget)(w))->ghostview.interpreter_error_id)
#define	arguments(w) \
	(((GhostviewWidget)(w))->ghostview.arguments)
#define	interpreter(w) \
	(((GhostviewWidget)(w))->ghostview.interpreter)
#define	gv_gc(w) \
	(((GhostviewWidget)(w))->ghostview.gc)
#define	antialias(w) \
	(((GhostviewWidget)(w))->ghostview.antialias)
#define	quiet(w) \
	(((GhostviewWidget)(w))->ghostview.quiet)
#define	safer(w) \
	(((GhostviewWidget)(w))->ghostview.safer)
#define	busy_cursor(w) \
	(((GhostviewWidget)(w))->ghostview.busy_cursor)

static void	ComputeXdpi(Widget, int, XrmValue *);
static void	ComputeYdpi(Widget, int, XrmValue *);

#define	OFFSET(field)	XtOffsetOf(GhostviewRec, ghostview.field)
static XtResource resources[] =
{
	{
		GvNarguments, GvCArguments,
		XtRString, sizeof(String), OFFSET(arguments),
		XtRString, NULL
	}, {
		GvNbottomMargin, XtCMargin,
		XtRInt, sizeof(int), OFFSET(bottom_margin),
		XtRImmediate, (XtPointer)0
	}, {
		GvNbusyCursor, XtCCursor,
		XtRCursor, sizeof(XtPointer), OFFSET(busy_cursor),
		XtRString, "watch"
	}, {
		GvNnotifyCallback, XtCCallback,
		XtRCallback, sizeof(XtPointer), OFFSET(notify_callback),
		XtRCallback, NULL
	}, {
		GvNcursor, XtCCursor,
		XtRCursor, sizeof(XtPointer), OFFSET(cursor),
		XtRString, "crosshair"
	}, {
		GvNfilename, GvCFilename,
		XtRString, sizeof(String), OFFSET(filename),
		XtRString, NULL
	}, {
		XtNforeground, XtCForeground,
		XtRPixel, sizeof(Pixel), OFFSET(foreground),
		XtRString, XtDefaultForeground
	}, {
		GvNinterpreter, GvCInterpreter,
		XtRString, sizeof(String), OFFSET(interpreter),
		XtRString, "gs"
	}, {
		GvNleftMargin, XtCMargin,
		XtRInt, sizeof(int), OFFSET(left_margin),
		XtRImmediate, (XtPointer)0
	}, {
		GvNllx, GvCBoundingBox,
		XtRInt, sizeof(int), OFFSET(llx),
		XtRImmediate, (XtPointer)0
	}, {
		GvNlly, GvCBoundingBox,
		XtRInt, sizeof(int), OFFSET(lly),
		XtRImmediate, (XtPointer)0
	}, {
		GvNmessageCallback, XtCCallback,
		XtRCallback, sizeof(XtPointer), OFFSET(message_callback),
		XtRCallback, NULL
	}, {
		GvNorientation, GvCOrientation,
		GvRPageOrientation, sizeof(GvPageOrientation), OFFSET(orientation),
		XtRImmediate, (XtPointer)GvPageOrientationPortrait
	}, {
		GvNoutputCallback, XtCCallback,
		XtRCallback, sizeof(XtPointer), OFFSET(output_callback),
		XtRCallback, NULL
	}, {
		GvNpalette, GvCPalette,
		GvRPalette, sizeof(GvPalette), OFFSET(palette),
		XtRImmediate, (XtPointer)GvPaletteColor
	}, {
		GvNquiet, GvCQuiet,
		XtRBoolean, sizeof(Boolean), OFFSET(quiet),
		XtRImmediate, (XtPointer)True
	}, {
		GvNrightMargin, XtCMargin,
		XtRInt, sizeof(int), OFFSET(right_margin),
		XtRImmediate, (XtPointer)0
	}, {
		GvNsafer, GvCSafer,
		XtRBoolean, sizeof(Boolean), OFFSET(safer),
		XtRImmediate, (XtPointer)True
	}, {
		GvNtopMargin, XtCMargin,
		XtRInt, sizeof(int), OFFSET(top_margin),
		XtRImmediate, (XtPointer)0
	}, {
		GvNuseBackingPixmap, GvCUseBackingPixmap,
		XtRBoolean, sizeof(Boolean), OFFSET(use_bpixmap),
		XtRImmediate, (XtPointer)True
	}, {
		GvNurx, GvCBoundingBox,
		XtRInt, sizeof(int), OFFSET(urx),
		XtRImmediate, (XtPointer)612
	}, {
		GvNury, GvCBoundingBox,
		XtRInt, sizeof(int), OFFSET(ury),
		XtRImmediate, (XtPointer)792
	}, {
		GvNxdpi, GvCResolution,
		XtRInt, sizeof(int), OFFSET(xdpi),
		XtRCallProc, (XtPointer)ComputeXdpi
	}, {
		GvNydpi, GvCResolution,
		XtRInt, sizeof(int), OFFSET(ydpi),
		XtRCallProc, (XtPointer)ComputeYdpi
	}, {
		GvNgsLocale, GvCGsLocale,
		XtRString, sizeof(String), OFFSET(gs_locale),
		XtRString, (XtPointer)"POSIX"
	}, {
		GvNantialias, GvCAntialias,
		XtRBoolean, sizeof(Boolean), OFFSET(antialias),
		XtRImmediate, (XtPointer)True
	}
};
#undef	offset

static void	Message(Widget, XEvent *, String *, Cardinal *);
static void	Notify(Widget, XEvent *, String *, Cardinal *);
static void	Input(XtPointer, int *, XtInputId *);
static void	Output(XtPointer, int *, XtInputId *);

static void	ClassInitialize(void);
static void	Initialize(Widget, Widget, ArgList, Cardinal *);
static void	Realize(Widget, Mask *, XSetWindowAttributes *);
static void	Destroy(Widget);
static void	Resize(Widget);
static Boolean	SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult	QueryGeometry(Widget,
				XtWidgetGeometry *, XtWidgetGeometry *);

static void	Layout(Widget, Boolean, Boolean);
static Boolean	ComputeSize(Widget, Boolean, Boolean, Dimension *, Dimension *);
static void	ChangeSize(Widget, Dimension, Dimension);
static Boolean	Setup(Widget);
static void	StartInterpreter(Widget);
static void	StopInterpreter(Widget);
static void	InterpreterFailed(Widget);

static XtActionsRec actions[] = {
	{"gvMessage",	Message},
	{"gvNotify",	Notify},
};

static char translations[] = "<Message>: gvMessage()";

GhostviewClassRec ghostviewClassRec = {
	{	/** Core **/
	/* superclass		*/	(WidgetClass) &coreClassRec,
	/* class_name		*/	"Ghostview",
	/* widget_size		*/	sizeof(GhostviewRec),
	/* class_initialize	*/	ClassInitialize,
	/* class_part_initialize*/	NULL,
	/* class_inited		*/	FALSE,
	/* initialize		*/	Initialize,
	/* initialize_hook	*/	NULL,
	/* realize		*/	Realize,
	/* actions		*/	actions,
	/* num_actions		*/	XtNumber(actions),
	/* resources		*/	resources,
	/* num_resources	*/	XtNumber(resources),
	/* xrm_class		*/	NULLQUARK,
	/* compress_motion	*/	TRUE,
	/* compress_exposure	*/	TRUE,
	/* compress_enterleave	*/	TRUE,
	/* visible_interest	*/	FALSE,
	/* destroy		*/	Destroy,
	/* resize		*/	Resize,
	/* expose		*/	NULL,
	/* set_values		*/	SetValues,
	/* set_values_hook	*/	NULL,
	/* set_values_almost	*/	XtInheritSetValuesAlmost,
	/* get_values_hook	*/	NULL,
	/* accept_focus		*/	NULL,
	/* version		*/	XtVersion,
	/* callback_private	*/	NULL,
	/* tm_table		*/	translations,
	/* query_geometry	*/	QueryGeometry,
	/* display_accelerator	*/	XtInheritDisplayAccelerator,
	/* extension		*/	NULL
	},

	{	/** Ghostview **/
	/* extension		*/	NULL,
	}
};

WidgetClass ghostviewWidgetClass = (WidgetClass)&ghostviewClassRec;

/*
 * Procedures that compute the default xdpi and ydpi from display parameters
 */
static void
ComputeXdpi(Widget w, int offset, XrmValue *value)
{
static	int	xdpi;
	USEUP(offset);
	xdpi  = 25.4 * WidthOfScreen(XtScreen(w));
	xdpi /= WidthMMOfScreen(XtScreen(w));
	value->addr = (XtPointer)&xdpi;
}

static void
ComputeYdpi(Widget w, int offset, XrmValue *value)
{
static	int	ydpi;
	USEUP(offset);
	ydpi  = 25.4 * HeightOfScreen(XtScreen(w));
	ydpi /= HeightMMOfScreen(XtScreen(w));
	value->addr = (XtPointer)&ydpi;
}

/*
 * Message action routine.
 * Passes ghostscript message events back to application via
 * the message callback.  It also marks the interpreter as
 * being not busy at the end of page, and stops the interpreter
 * when it send a "done" message.
 */
static void
Message(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
	String	callback;
	USEUP(params); USEUP(num_params);

	callback = NULL;
	gv_mwin(w) = event->xclient.data.l[0];
	if(event->xclient.message_type == page_atom(w)) {
		gv_busy(w) = False;
		XDefineCursor(XtDisplay(w), XtWindow(w), gv_cursor(w));
		callback = "Page";
	}
	else if(event->xclient.message_type == done_atom(w)) {
		StopInterpreter(w);
		callback = "Done";
	}
	else {
		return;
	}
	assert(callback != NULL);
	XtCallCallbackList(w, msg_callback(w), callback);
}

/*
 * Notify action routine.
 */
static void
Notify(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
	GhostviewReturnStruct ret_val;
	USEUP(params); USEUP(num_params);

	memset((void *)&ret_val, '\0', sizeof(ret_val));

	switch(orientation(w)) {
	case GvPageOrientationPortrait:
		ret_val.psx = llx(w) + event->xbutton.x * 72.0 / xdpi(w);
		ret_val.psy = ury(w) - event->xbutton.y * 72.0 / ydpi(w);
		break;
	case GvPageOrientationLandscape:
		ret_val.psx = llx(w) + event->xbutton.y * 72.0 / ydpi(w);
		ret_val.psy = lly(w) + event->xbutton.x * 72.0 / xdpi(w);
		break;
	case GvPageOrientationUpsideDown:
		ret_val.psx = urx(w) - event->xbutton.x * 72.0 / xdpi(w);
		ret_val.psy = lly(w) + event->xbutton.y * 72.0 / ydpi(w);
		break;
	case GvPageOrientationSeascape:
		ret_val.psx = urx(w) - event->xbutton.y * 72.0 / ydpi(w);
		ret_val.psy = ury(w) - event->xbutton.x * 72.0 / xdpi(w);
		break;
	}
	ret_val.event = event;
	XtCallCallbackList(w, notify_callback(w), (XtPointer)&ret_val);
}

static Boolean broken_pipe = False;

static void
CatchPipe(int i)
{
	USEUP(i);
	broken_pipe = True;
}

/*
 * Input - Feed data to ghostscript's stdin.
 * Write bytes to ghostscript using non-blocking I/O.
 * Also, pipe signals are caught during writing.  The return
 * values are checked and the appropriate action is taken.  I do
 * this at this low level, because it may not be appropriate for
 * SIGPIPE to be caught for the overall application.
 */
static void
Input(XtPointer client_data, int *source, XtInputId *id)
{
	Widget		w   = (Widget)client_data;
	int		bytes_written;
	struct sigaction	sa, osa;

	USEUP(id); USEUP(source);

	memset((void *)&sa,  '\0', sizeof(sa));
	memset((void *)&osa, '\0', sizeof(osa));
	sa.sa_handler = CatchPipe;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGPIPE, &sa, &osa);
	do {
		if(buffer_bytes_left(w) == 0) {
			/*
			 * Get a new section if required
			 */
			if(ps_input(w)   != NULL
			&& bytes_left(w) == 0) {
				GVFile	*ps_old = ps_input(w);
				ps_input(w) = ps_old->next;
				if(ps_old->close)
					fclose(ps_old->fp);
				XtFree((XtPointer)ps_old);
			}

			/*
			 * Have to seek at the beginning of each section
			 */
			if(ps_input(w) != NULL
			&& ps_input(w)->seek_needed) {
				if(ps_input(w)->len > 0)
					fseek(ps_input(w)->fp,
						ps_input(w)->begin, SEEK_SET);
				ps_input(w)->seek_needed = False;
				bytes_left(w) = ps_input(w)->len;
			}

			if(bytes_left(w) > GV_BUFSIZ) {
				buffer_bytes_left(w) = fread(input_buffer(w),
						sizeof(char), GV_BUFSIZ,
						ps_input(w)->fp);
			}
			else if(bytes_left(w) > 0) {
				buffer_bytes_left(w) = fread(input_buffer(w),
						sizeof(char), bytes_left(w),
						ps_input(w)->fp);
			}
			else {
				buffer_bytes_left(w) = 0;
			}

			if(bytes_left(w) > 0
			&& buffer_bytes_left(w) == 0) {
				/*
				 * Error occured
				 */
				InterpreterFailed(w);
			}
			input_buffer_ptr(w) = input_buffer(w);
			bytes_left(w)      -= buffer_bytes_left(w);
		}

		if(buffer_bytes_left(w) > 0) {
			bytes_written = write(interpreter_input(w),
				input_buffer_ptr(w),
				buffer_bytes_left(w));

			if(broken_pipe) {
				/*
				 * Something bad happened
				 */
				broken_pipe = False;
				InterpreterFailed(w);
			}
			else if (bytes_written == -1) {
				if((errno != EWOULDBLOCK)
				&& (errno != EAGAIN)) {
					/*
					 * Something bad happened
					 */
					InterpreterFailed(w);
				}
			}
			else {
				buffer_bytes_left(w) -= bytes_written;
				input_buffer_ptr(w)  += bytes_written;
			}
		}
	} while(ps_input(w) != NULL && buffer_bytes_left(w) == 0);

	sigaction(SIGPIPE, &osa, &sa);
	if(ps_input(w)             == NULL
	&& buffer_bytes_left(w)    == 0
	&& interpreter_input_id(w) != None) {
		XtRemoveInput(interpreter_input_id(w));
		interpreter_input_id(w) = None;
	}
}

/*
 * Output - receive I/O from ghostscript's stdout and stderr.
 * Pass this to the application via the output_callback.
 */
static void
Output(XtPointer client_data, int *source, XtInputId *id)
{
	Widget		w     = (Widget)client_data;
	int		bytes = 0;
	char		buf[GV_BUFSIZ+1];
	int		*gvw_src;
	XtInputId	gvw_id;

	USEUP(id);

	gvw_src = NULL;
	gvw_id  = (XtInputId)0;
	if(*source == interpreter_output(w)) {
		gvw_src = &interpreter_output(w);
		gvw_id  = interpreter_output_id(w);
	}
	else if(*source == interpreter_error(w)) {
		gvw_src = &interpreter_error(w);
		gvw_id  = interpreter_error_id(w);
	}
	if(gvw_src != NULL) {
		bytes = read(*gvw_src, buf, GV_BUFSIZ);
		if(bytes == 0) {
			/*
			 * EOF
			 */
			close(*gvw_src);
			*gvw_src = -1;
			XtRemoveInput(gvw_id);
			return;
		}
		else if(bytes == -1) {
			/*
			 * badness
			 */
			InterpreterFailed(w);
			return;
		}
	}

	buf[bytes] = '\0';
	if(bytes > 0)
		XtCallCallbackList(w, output_callback(w), (XtPointer)buf);
}

#define	DONE(type, value)					\
	{							\
		if(toVal->addr != NULL) {			\
			if(toVal->size < sizeof(type)) {	\
				toVal->size = sizeof(type);	\
				return False;			\
			}					\
			*(type*)(toVal->addr) = (value);	\
		}						\
		else {						\
			static type static_val;			\
			static_val  = (value);			\
			toVal->addr = (XPointer)&static_val;	\
		}						\
		toVal->size = sizeof(type);			\
		return True;					\
	}

/*
 * PageOrienation Conversion Routine.
 * Returns True if Conversion is successful.
 */
static Boolean
GvCvtStringToPageOrientation(Display *dpy, XrmValue *args, Cardinal *num_args,
			XrmValue *fromVal, XrmValue *toVal, XtPointer *data)
{
static	XrmQuark GvQEportrait, GvQElandscape, GvQEupsideDown, GvQEseascape;
static	Boolean  haveQuarks = False;
	XrmQuark q;
	char    *str = (char *)fromVal->addr;
	char    *lower;
	USEUP(args); USEUP(num_args); USEUP(data);

	if(str == NULL)
		return False;

	if(!haveQuarks) {
		GvQEportrait   = XrmStringToQuark(GvEportrait);
		GvQElandscape  = XrmStringToQuark(GvElandscape);
		GvQEupsideDown = XrmStringToQuark(GvEupsideDown);
		GvQEseascape   = XrmStringToQuark(GvEseascape);
		haveQuarks     = True;
	}

	q = XrmStringToQuark(lower = lowdup(str));
	XtFree(lower);

	if(q == GvQEportrait)
		DONE(GvPageOrientation, GvPageOrientationPortrait);
	if(q == GvQElandscape)
		DONE(GvPageOrientation, GvPageOrientationLandscape);
	if(q == GvQEupsideDown)
		DONE(GvPageOrientation, GvPageOrientationUpsideDown);
	if(q == GvQEseascape)
		DONE(GvPageOrientation, GvPageOrientationSeascape);

	XtDisplayStringConversionWarning(dpy, str, GvRPageOrientation);
	return False;
}

/*
 * Palette Conversion Routine.
 * Returns True if Conversion is successful.
 */
static Boolean
GvCvtStringToPalette(Display *dpy, XrmValue *args, Cardinal *num_args,
			XrmValue *fromVal, XrmValue *toVal, XtPointer *data)
{
static	XrmQuark GvQEmonochrome, GvQEgrayscale, GvQEcolor;
static	Boolean  haveQuarks = False;
	XrmQuark q;
	char    *str = (XPointer)fromVal->addr;
	char    *lower;
	USEUP(args); USEUP(num_args); USEUP(data);

	if(str == NULL)
		return False;

	if(!haveQuarks) {
		GvQEmonochrome = XrmStringToQuark(GvEmonochrome);
		GvQEgrayscale  = XrmStringToQuark(GvEgrayscale);
		GvQEcolor      = XrmStringToQuark(GvEcolor);
		haveQuarks      = True;
	}

	q = XrmStringToQuark(lower = lowdup(str));
	XtFree(lower);

	if(q == GvQEmonochrome)
		DONE(GvPalette, GvPaletteMonochrome);
	if(q == GvQEgrayscale)
		DONE(GvPalette, GvPaletteGrayscale);
	if(q == GvQEcolor)
		DONE(GvPalette, GvPaletteColor);

	XtDisplayStringConversionWarning(dpy, str, GvRPalette);
	return False;
}

/*
 * Register the type converter required for the PageOrientation.
 * Register the type converter required for the Palette.
 * This routine is called exactly once.
 */
static void
ClassInitialize(void)
{
	XtSetTypeConverter(XtRString, GvRPageOrientation,
		GvCvtStringToPageOrientation, NULL, 0, XtCacheAll, NULL);
	XtSetTypeConverter(XtRString, GvRPalette, GvCvtStringToPalette,
		NULL, 0, XtCacheAll, NULL);
}

/*
 * Initialize private state.
 */
static void
Initialize(Widget request, Widget new, ArgList args, Cardinal *num_args)
{
	XGCValues	values;
	XtGCMask	mask;
	GhostviewWidget	ngvw = (GhostviewWidget)new;
	GhostviewWidget	rgvw = (GhostviewWidget)request;
	Display		*dpy = XtDisplay(new);
	USEUP(args); USEUP(num_args);

	values.foreground = new->core.background_pixel;
	mask              = GCForeground;

	ngvw->ghostview.gc                    = XtGetGC(new, mask, &values);
	ngvw->ghostview.mwin                  = None;
	ngvw->ghostview.disable_start         = False;
	ngvw->ghostview.interpreter_pid       = -1;
	ngvw->ghostview.input_buffer          = NULL;
	ngvw->ghostview.bytes_left            = 0;
	ngvw->ghostview.input_buffer_ptr      = NULL;
	ngvw->ghostview.buffer_bytes_left     = 0;
	ngvw->ghostview.ps_input              = NULL;
	ngvw->ghostview.interpreter_input     = -1;
	ngvw->ghostview.interpreter_output    = -1;
	ngvw->ghostview.interpreter_error     = -1;
	ngvw->ghostview.interpreter_input_id  = None;
	ngvw->ghostview.interpreter_output_id = None;
	ngvw->ghostview.interpreter_error_id  = None;
	ngvw->ghostview.gs_width              = 0;
	ngvw->ghostview.gs_height             = 0;
	ngvw->ghostview.changed               = False;
	ngvw->ghostview.busy                  = False;

	ngvw->ghostview.ghostview = XInternAtom(dpy, "GHOSTVIEW", False);
	ngvw->ghostview.gv_colors = XInternAtom(dpy, "GHOSTVIEW_COLORS", False);
	ngvw->ghostview.next      = XInternAtom(dpy, "NEXT", False);
	ngvw->ghostview.page      = XInternAtom(dpy, "PAGE", False);
	ngvw->ghostview.done      = XInternAtom(dpy, "DONE", False);

	if(rgvw->ghostview.interpreter != NULL)
		ngvw->ghostview.interpreter = XtNewString(rgvw->ghostview.interpreter);
	if(rgvw->ghostview.arguments != NULL)
		ngvw->ghostview.arguments = XtNewString(rgvw->ghostview.arguments);
	if(rgvw->ghostview.filename != NULL)
		ngvw->ghostview.filename = XtNewString(rgvw->ghostview.filename);

	/*
	 * Compute window size
	 */
	Layout(new, (Boolean)(rgvw->core.width == 0),
					(Boolean)(rgvw->core.height == 0));
}

/*
 * Create Window and start interpreter if needed
 */
static void
Realize(Widget w, Mask *valueMask, XSetWindowAttributes *attributes)
{
	if(gv_cursor(w) != None) {
		attributes->cursor = gv_cursor(w);
		*valueMask |= CWCursor;
	}

	XtCreateWindow(w, (unsigned)InputOutput, (Visual *)CopyFromParent,
							*valueMask, attributes);

	Setup(w);
}

/*
 * Destroy routine: kill the interpreter and release the GC
 */
static void
Destroy(Widget w)
{
	StopInterpreter(w);
	XtReleaseGC(w, gv_gc(w));
	XtFree((XtPointer)input_buffer(w));
	XtFree((XtPointer)gv_filename(w));
	XtFree((XtPointer)arguments(w));
	XtFree((XtPointer)interpreter(w));
	if(core_bpixmap(w) != XtUnspecifiedPixmap)
		XFreePixmap(XtDisplay(w), core_bpixmap(w));
}

/*
 * Process resize request.  Requested size cannot be changed.
 * NOTE: This routine may be called before the widget is realized.
 * (It was a surprise to me.)
 * If the widget is realized, start a new interpreter by calling Setup().
 * If Setup() actually started a new interpreter and it is taking input
 * from stdin, send a refresh message to the application.  This is the
 * only way that the application can be notified that it needs to resend
 * the input because someone forced a new window size on the widget.
 */
static void
Resize(Widget w)
{
	Layout(w, False, False);
	if(!XtIsRealized(w))
		return;
	if(Setup(w) && gv_filename(w) == NULL)
		XtCallCallbackList(w, msg_callback(w), "Refresh");
}

/*
 * SetValues routine.  Set new private state, based on changed values
 * in the widget.  Always returns False, because redisplay is never needed.
 */
static Boolean
SetValues(Widget current, Widget request, Widget new, ArgList args,
						Cardinal *num_args)
{
	GhostviewWidget	cgvw = (GhostviewWidget)current;
	GhostviewWidget	rgvw = (GhostviewWidget)request;
	GhostviewWidget	ngvw = (GhostviewWidget)new;
	String		cfilename, carguments, cinterpreter;
	String		rfilename, rarguments, rinterpreter;

	USEUP(args); USEUP(num_args);

	if(XtIsRealized(new)
	&& !ngvw->ghostview.busy
	&& cgvw->ghostview.cursor != ngvw->ghostview.cursor)
		XDefineCursor(XtDisplay(new), XtWindow(new),
						ngvw->ghostview.cursor);
	if(XtIsRealized(new)
	&& ngvw->ghostview.busy
	&& cgvw->ghostview.busy_cursor != ngvw->ghostview.busy_cursor)
		XDefineCursor(XtDisplay(new), XtWindow(new),
						ngvw->ghostview.busy_cursor);

	if(cgvw->core.background_pixel != rgvw->core.background_pixel) {
		XGCValues	values;
		XtGCMask	mask;

		XtReleaseGC(current, cgvw->ghostview.gc);
		values.foreground = new->core.background_pixel;
		mask = GCForeground;
		ngvw->ghostview.gc = XtGetGC(new, mask, &values);
	}
	if(cgvw->ghostview.filename != rgvw->ghostview.filename) {
		if(rgvw->ghostview.filename != NULL)
			ngvw->ghostview.filename =
					XtNewString(rgvw->ghostview.filename);
		else
			ngvw->ghostview.filename = NULL;
		XtFree((XtPointer)cgvw->ghostview.filename);
		cgvw->ghostview.filename = NULL;
	}
	if(cgvw->ghostview.arguments != rgvw->ghostview.arguments) {
		if(rgvw->ghostview.arguments != NULL)
			ngvw->ghostview.arguments =
					XtNewString(rgvw->ghostview.arguments);
		else
			ngvw->ghostview.arguments = NULL;
		XtFree((XtPointer)cgvw->ghostview.arguments);
		cgvw->ghostview.arguments = NULL;
	}
	if(cgvw->ghostview.interpreter != rgvw->ghostview.interpreter) {
		if(rgvw->ghostview.interpreter != NULL)
			ngvw->ghostview.interpreter =
				XtNewString(rgvw->ghostview.interpreter);
		else
			ngvw->ghostview.interpreter = NULL;
		XtFree((XtPointer)cgvw->ghostview.interpreter);
		cgvw->ghostview.interpreter = NULL;
	}

	if(cgvw->ghostview.antialias != rgvw->ghostview.antialias)
		cgvw->ghostview.antialias = rgvw->ghostview.antialias;

	cfilename    = (cgvw->ghostview.filename == NULL)
			? "(null)"
			: cgvw->ghostview.filename;
	carguments   = (cgvw->ghostview.arguments == NULL)
			? "(null)"
			: cgvw->ghostview.arguments;
	cinterpreter = (cgvw->ghostview.interpreter == NULL)
			? "(null)"
			: cgvw->ghostview.interpreter;
	rfilename    = (rgvw->ghostview.filename == NULL)
			? "(null)"
			: rgvw->ghostview.filename;
	rarguments   = (rgvw->ghostview.arguments == NULL)
			? "(null)"
			: rgvw->ghostview.arguments;
	rinterpreter = (rgvw->ghostview.interpreter == NULL)
			? "(null)"
			: rgvw->ghostview.interpreter;

	if(cgvw->core.width != rgvw->core.width
	|| cgvw->core.height != rgvw->core.height
	|| cgvw->core.background_pixel != rgvw->core.background_pixel
	|| cgvw->ghostview.foreground != rgvw->ghostview.foreground
	|| cgvw->ghostview.palette != rgvw->ghostview.palette
	|| strcmp(cinterpreter, rinterpreter) != 0
	|| strcmp(carguments, rarguments) != 0
	|| cgvw->ghostview.quiet != rgvw->ghostview.quiet
	|| cgvw->ghostview.safer != rgvw->ghostview.safer
	|| strcmp(cfilename, rfilename) != 0
	|| cgvw->ghostview.orientation != rgvw->ghostview.orientation
	|| cgvw->ghostview.use_bpixmap != rgvw->ghostview.use_bpixmap
	|| cgvw->ghostview.xdpi != rgvw->ghostview.xdpi
	|| cgvw->ghostview.ydpi != rgvw->ghostview.ydpi
	|| cgvw->ghostview.bottom_margin != rgvw->ghostview.bottom_margin
	|| cgvw->ghostview.left_margin != rgvw->ghostview.left_margin
	|| cgvw->ghostview.right_margin != rgvw->ghostview.right_margin
	|| cgvw->ghostview.top_margin != rgvw->ghostview.top_margin
	|| cgvw->ghostview.llx != rgvw->ghostview.llx
	|| cgvw->ghostview.lly != rgvw->ghostview.lly
	|| cgvw->ghostview.urx != rgvw->ghostview.urx
	|| cgvw->ghostview.ury != rgvw->ghostview.ury
	|| cgvw->ghostview.antialias != rgvw->ghostview.antialias) {
		ngvw->ghostview.changed = True;
		Layout(new, True, True);
	}

	if(ngvw->ghostview.changed && XtIsRealized(current))
		Setup(new);
	return False;
}

/*
 * This tells the parent what size we would like to be given certain
 * constraints.
 */
static XtGeometryResult 
QueryGeometry(Widget w, XtWidgetGeometry *intended, XtWidgetGeometry *requested)
{
	Dimension	new_width, new_height;
	Boolean		change;
	XtGeometryMask	width_req, height_req;
 
	width_req  = intended->request_mode & CWWidth;
	height_req = intended->request_mode & CWHeight;

	if(width_req != 0)
		new_width = intended->width;
	else
		new_width = w->core.width;

	if(height_req != 0)
		new_height = intended->height;
	else
		new_height = w->core.height;

	requested->request_mode = 0;

	/*
	 * We only care about our height and width.
	 */
	if(width_req == 0 && height_req == 0)
		return XtGeometryYes;

	change = ComputeSize(w, width_req == 0, height_req == 0, &new_width,
								&new_height);

	requested->request_mode |= CWWidth;
	requested->width         = new_width;
	requested->request_mode |= CWHeight;
	requested->height        = new_height;

	return (change) ? XtGeometryAlmost : XtGeometryYes;
}

/*
 * Layout the widget.
 */
static void
Layout(Widget w, Boolean xfree, Boolean yfree)
{
	Dimension	width = w->core.width;
	Dimension	height = w->core.height;

	if(ComputeSize(w, xfree, yfree, &width, &height))
		ChangeSize(w, width, height);
}

/*
 * Compute new size of window, sets xdpi and ydpi if necessary.
 * returns True if new window size is different
 */
static Boolean
ComputeSize(Widget w, Boolean xfree, Boolean yfree, Dimension *width,
							Dimension *height)
{
	Dimension	new_width  = *width;
	Dimension	new_height = *height;
	int		newxdpi, newydpi;
	Boolean		change;

	if(xfree && yfree) {
		/*
		 * width and height can be changed, calculate window size
		 * according to xpdi and ydpi
		 */
		switch(orientation(w)) {
		case GvPageOrientationPortrait:
		case GvPageOrientationUpsideDown:
			new_width  = (urx(w) - llx(w)) / 72.0 * xdpi(w) + 0.5;
			new_height = (ury(w) - lly(w)) / 72.0 * ydpi(w) + 0.5;
			break;
		case GvPageOrientationLandscape:
		case GvPageOrientationSeascape:
			new_width  = (ury(w) - lly(w)) / 72.0 * xdpi(w) + 0.5;
			new_height = (urx(w) - llx(w)) / 72.0 * ydpi(w) + 0.5;
			break;
		}
	}
	else if(xfree) {
		/*
		 * height is fixed, preserve aspect ratio by
		 * recomputing ydpi and xdpi
		 */
		switch(orientation(w)) {
		case GvPageOrientationPortrait:
		case GvPageOrientationUpsideDown:
			newydpi = core_height(w) * 72.0 / (ury(w) - lly(w));
			newxdpi = newydpi * xdpi(w) / ydpi(w);
			xdpi(w) = newxdpi;
			ydpi(w) = newydpi;
			new_width = (urx(w) - llx(w)) / 72.0 * xdpi(w) + 0.5;
			break;
		case GvPageOrientationLandscape:
		case GvPageOrientationSeascape:
			newydpi = core_height(w) * 72.0 / (urx(w) - llx(w));
			newxdpi = newydpi * xdpi(w) / ydpi(w);
			xdpi(w) = newxdpi;
			ydpi(w) = newydpi;
			new_width = (ury(w) - lly(w)) / 72.0 * xdpi(w) + 0.5;
			break;
		}
	}
	else if(yfree) {
		/*
		 * width is fixed, preserve aspect ratio by recomputing
		 * xdpi and ydpi
		 */
		switch(orientation(w)) {
		case GvPageOrientationPortrait:
		case GvPageOrientationUpsideDown:
			newxdpi = core_width(w) * 72.0 /(urx(w) - llx(w));
			newydpi = newxdpi * ydpi(w) / xdpi(w);
			xdpi(w) = newxdpi;
			ydpi(w) = newydpi;
			new_height = (ury(w) - lly(w)) / 72.0 * ydpi(w) + 0.5;
			break;
		case GvPageOrientationLandscape:
		case GvPageOrientationSeascape:
			newxdpi = core_width(w) * 72.0 / (ury(w) - lly(w));
			newydpi = newxdpi * ydpi(w) / xdpi(w);
			xdpi(w) = newxdpi;
			ydpi(w) = newydpi;
			new_height = (urx(w) - llx(w)) / 72.0 * ydpi(w) + 0.5;
			break;
		}
	}
	else {
		/*
		 * height and width are fixed.  Just have to live with it.
		 */
		switch(orientation(w)) {
		case GvPageOrientationPortrait:
		case GvPageOrientationUpsideDown:
			xdpi(w) = core_width(w)  * 72.0 / (urx(w) - llx(w));
			ydpi(w) = core_height(w) * 72.0 / (ury(w) - lly(w));
			break;
		case GvPageOrientationLandscape:
		case GvPageOrientationSeascape:
			xdpi(w) = core_width(w)  * 72.0 / (ury(w) - lly(w));
			ydpi(w) = core_height(w) * 72.0 / (urx(w) - llx(w));
			break;
		}
	}

	change  = (new_width != *width) || (new_height != *height);
	*width  = new_width;
	*height = new_height;
	return change;
}

/*
 * Request a size change.
 */
static void
ChangeSize(Widget w, Dimension width, Dimension height)
{
	XtWidgetGeometry	request, reply;
	Boolean			changed = False;
	Boolean			canx, cany;

	request.request_mode = CWWidth | CWHeight;
	request.width        = width;
	request.height       = height;

	switch(XtMakeGeometryRequest(w, &request, &reply)) {
	case XtGeometryYes:
		changed = True;
		break;
	case XtGeometryNo:
	case XtGeometryDone:
		break;
	case XtGeometryAlmost:
		canx = (Boolean)(request.height != reply.height);
		cany = (Boolean)(request.width  != reply.width);
		ComputeSize(w, canx, cany, &reply.width, &reply.height);
		request = reply;
		switch(XtMakeGeometryRequest(w, &request, &reply)) {
		case XtGeometryYes:
			changed = True;
			break;
		case XtGeometryNo:
		case XtGeometryDone:
			break;
		case XtGeometryAlmost:
			request = reply;
			ComputeSize(w, False, False, &request.width,
							&request.height);
			request.request_mode = CWWidth | CWHeight;
			XtMakeGeometryRequest(w, &request, &reply);
			changed = True;
			break;
		}
		break;
	}

	/*
	 * If success, setup the widet for the new size.
	 */
	if(changed && XtIsRealized(w))
		Setup(w);
}

/*
 * Catch the alloc error when there is not enough resources for the
 * backing pixmap.  Automatically shut off backing pixmap and let the
 * user know when this happens.
 */
static Boolean alloc_error;
static XErrorHandler oldhandler;

static int
catch_alloc(Display *dpy, XErrorEvent *err)
{
	if(err->error_code == BadAlloc)
		alloc_error = True;
	if(alloc_error)
		return 0;
	return oldhandler(dpy, err);
}

/*
 * Setup - sets up the backing pixmap, and GHOSTVIEW property and
 * starts interpreter if needed.
 * NOTE: the widget must be realized before calling Setup().
 * Returns True if a new interpreter was started, False otherwise.
 */
static Boolean
Setup(Widget w)
{
	char			buf[GV_BUFSIZ];
	Pixmap			bpixmap;
	XSetWindowAttributes	xswa;
	char			*olocale;

	if(!changed(w)
	&& core_width(w)  == gs_width(w)
	&& core_height(w) == gs_height(w))
		return False;

	StopInterpreter(w);

	if((core_width(w) != gs_width(w)
	|| core_height(w) != gs_height(w)
	|| !use_bpixmap(w))
	&& core_bpixmap(w) != XtUnspecifiedPixmap) {
		XFreePixmap(XtDisplay(w), core_bpixmap(w));
		core_bpixmap(w) = XtUnspecifiedPixmap;
		XSetWindowBackgroundPixmap(XtDisplay(w), XtWindow(w), None);
	}

	if(use_bpixmap(w)) {
		if(core_bpixmap(w) == XtUnspecifiedPixmap) {
			/*
			 * Get a Backing Pixmap, but be ready for the
			 * BadAlloc.
			 */
			XSync(XtDisplay(w), False);
			oldhandler  = XSetErrorHandler(catch_alloc);
			alloc_error = False;
			bpixmap     = XCreatePixmap(XtDisplay(w), XtWindow(w),
						core_width(w), core_height(w),
						core_depth(w));
			/*
			 * force the error
			 */
			XSync(XtDisplay(w), False);
			if(alloc_error) {
				XtCallCallbackList(w, msg_callback(w),
								"BadAlloc");
				if(bpixmap != None) {
					XFreePixmap(XtDisplay(w), bpixmap);
					/*
					 * force the error
					 */
					XSync(XtDisplay(w), False);
					bpixmap = None;
				}
			}
			oldhandler = XSetErrorHandler(oldhandler);
			if(bpixmap != None) {
				core_bpixmap(w) = bpixmap;
				XSetWindowBackgroundPixmap(XtDisplay(w),
						XtWindow(w), core_bpixmap(w));
			}
		}
		else {
			bpixmap = core_bpixmap(w);
		}
	}
	else {
		if(core_bpixmap(w) != XtUnspecifiedPixmap) {
			XFreePixmap(XtDisplay(w), core_bpixmap(w));
			core_bpixmap(w) = XtUnspecifiedPixmap;
			XSetWindowBackgroundPixmap(XtDisplay(w), XtWindow(w),
									None);
		}
		bpixmap = None;
	}

	if(bpixmap != None)
		xswa.backing_store = NotUseful;
	else
		xswa.backing_store = Always;
	XChangeWindowAttributes(XtDisplay(w), XtWindow(w),CWBackingStore,&xswa);

	gs_width(w)  = core_width(w);
	gs_height(w) = core_height(w);

	/*
	 * This presents a minor localization problem, the %g
	 * format will use commas in some locales but ghostscript
	 * will not always understand this.  Hence, we need to do
	 * the setlocale() dance.
	 *		--mu@trends.net.
	 */
	olocale = setlocale(LC_ALL, NULL);
	setlocale(LC_NUMERIC, gs_locale(w));
	sprintf(buf, "%ld %d %d %d %d %d %g %g %d %d %d %d",
		bpixmap, orientation(w),
		llx(w), lly(w), urx(w), ury(w),
		(double)xdpi(w), (double)ydpi(w),
		left_margin(w), bottom_margin(w),
		right_margin(w), top_margin(w));
	setlocale(LC_NUMERIC, olocale);
	XChangeProperty(XtDisplay(w), XtWindow(w),
		ghostview_atom(w),
		XA_STRING, 8, PropModeReplace,
		(unsigned char *)buf, (int)strlen(buf));

	sprintf(buf, "%s %ld %ld",
		palette(w) == GvPaletteMonochrome
			? "Monochrome"
			: palette(w) == GvPaletteGrayscale
			? "Grayscale"
			: palette(w) == GvPaletteColor
			? "Color" : "?",
		foreground(w), core_background_pixel(w));
	XChangeProperty(XtDisplay(w), XtWindow(w),
		gv_colors_atom(w),
		XA_STRING, 8, PropModeReplace,
		(unsigned char *)buf, (int)strlen(buf));

	/*
	 * make sure the properties are updated
	 */
	XSync(XtDisplay(w), False);
	StartInterpreter(w);

	return True;
}

/*
 * This routine starts the interpreter.  It sets the DISPLAY and 
 * GHOSTVIEW environment variables.  The GHOSTVIEW environment variable
 * contains the Window that ghostscript should write on.
 *
 * This routine also opens pipes for stdout and stderr and initializes
 * application input events for them.  If input to ghostscript is not
 * from a file, a pipe for stdin is created.  This pipe is setup for
 * non-blocking I/O so that the user interface never "hangs" because of
 * a write to ghostscript.
 */
static void
StartInterpreter(Widget w)
{
	int	std_in[2];
	int	std_out[2];
	int	std_err[2];
	char	*argv[100];
	char	*cptr;
	int	argc;
	char	*args;

	argc = 0;
	args = NULL;

	StopInterpreter(w);

	/*
	 * Clear the window before starting a new interpreter.
	 */
	if(core_bpixmap(w) != XtUnspecifiedPixmap) {
		XFillRectangle(XtDisplay(w), core_bpixmap(w),
				gv_gc(w), 0, 0, core_width(w), core_height(w));
	}
	XClearArea(XtDisplay(w), XtWindow(w),
				0, 0, core_width(w), core_height(w), False);

	if(disable_start(w))
		return;

	argv[argc++] = interpreter(w);
	if(antialias(w)) {
		argv[argc++] = "-dNOPLATFONTS";
		argv[argc++] = "-sDEVICE=x11alpha";
	}
	else {
		argv[argc++] = "-sDEVICE=x11";
	}
	argv[argc++] = "-dNOPAUSE";
	if(quiet(w))
		argv[argc++] = "-dQUIET";
	if(safer(w))
		argv[argc++] = "-dSAFER";
	if(arguments(w) != NULL) {
		cptr = args = XtNewString(arguments(w));
		while(isspace((int)*cptr & 0xff))
			++cptr;
		while(*cptr != '\0') {
			argv[argc++] = cptr;
			while(*cptr != '\0' && !isspace((int)*cptr & 0xff))
				++cptr;
			if(*cptr != '\0')
				*cptr++ = '\0';
			if(argc + 2 >= (int)(sizeof(argv)/sizeof(argv[0]))) {
				fprintf(stderr,
					"Too many arguments to interpreter.\n");
				exit(EXIT_FAILURE);
			}
			while(isspace((int)*cptr & 0xff))
				++cptr;
		}
	}

	if(gv_filename(w) != NULL
	&& strcmp(gv_filename(w), "-") != 0) {
		argv[argc++] = gv_filename(w);
		argv[argc++] = "-c";
		argv[argc++] = "quit";
	}
	else {
		argv[argc++] = "-";
	}
	argv[argc++] = NULL;
	ARGSANITY(argv, argc);

	if(gv_filename(w) == NULL
	|| strcmp(gv_filename(w), "-") != 0) {
		if(pipe(std_in) == -1) {
			perror("Could not create pipe");
			exit(EXIT_FAILURE);
		}
	}
	else {
		std_in[0] = open(gv_filename(w), O_RDONLY, 0);
	}

	if(pipe(std_out) == -1) {
		perror("Could not create pipe");
		exit(EXIT_FAILURE);
	}
	if(pipe(std_err) == -1) {
		perror("Could not create pipe");
		exit(EXIT_FAILURE);
	}

	changed(w) = False;
	gv_busy(w) = True;
	XDefineCursor(XtDisplay(w), XtWindow(w), busy_cursor(w));

	interpreter_pid(w) = fork();
	if(interpreter_pid(w) == 0) {
		/*
		 * child
		 *
		 * We are safe leaving buf0 and buf1 on the stack
		 * (as opposed to static or dynamically allocated)
		 * since we're doing an execvp() before they go out
		 * of scope and that exec will do the necessary copying.
		 * Normally putenv() just does a plain pointer assignment
		 * so you can't use automatic arrays for setting environment
		 * variables.
		 */
		char buf0[GV_BUFSIZ];
		char buf1[GV_BUFSIZ];

		close(std_out[0]);
		dup2(std_out[1], 1);
		close(std_out[1]);

		close(std_err[0]);
		dup2(std_err[1], 2);
		close(std_err[1]);

		sprintf(buf0, "GHOSTVIEW=%ld", (long)XtWindow(w));
		sprintf(buf1, "DISPLAY=%s", XDisplayString(XtDisplay(w)));
		putenv(&buf0[0]);
		putenv(&buf1[0]);
		if(gv_filename(w) == NULL
		|| strcmp(gv_filename(w), "-") != 0) {
			close(std_in[1]);
			dup2(std_in[0], 0);
			close(std_in[0]);
		}
		execvp(argv[0], argv);
		sprintf(buf0, "Exec of %s failed", argv[0]);
		perror(buf0);
		_exit(EXIT_FAILURE);
	}
	else {
		if(gv_filename(w) == NULL) {
			int result;
			close(std_in[0]);

			result = fcntl(std_in[1], F_GETFL, 0);
			result = result | O_NONBLOCK;
			result = fcntl(std_in[1], F_SETFL, result);
			interpreter_input(w)    = std_in[1];
			interpreter_input_id(w) = None;
		}
		else if(strcmp(gv_filename(w), "-")) {
			close(std_in[0]);
		}
		close(std_out[1]);
		interpreter_output(w)    = std_out[0];
		interpreter_output_id(w) = 
				XtAppAddInput(XtWidgetToApplicationContext(w),
					std_out[0], (XtPointer)XtInputReadMask,
					Output, (XtPointer)w);
		close(std_err[1]);
		interpreter_error(w)    = std_err[0];
		interpreter_error_id(w) = 
				XtAppAddInput(XtWidgetToApplicationContext(w),
					std_err[0], (XtPointer)XtInputReadMask,
					Output, (XtPointer)w);
	}
	XtFree((XtPointer)args);
}

/*
 * Stop the interperter, if present, and remove any Input sources.
 * Also reset the busy state.
 */
static void
StopInterpreter(Widget w)
{
	if(interpreter_pid(w) >= 0) {
		struct sigaction sa, osa;
		int	status;

		/*
		 * Make sure SIGCHLD isn't being ignored, if it is, the
		 * waitpid() might not return at all.
		 * This section of code used to use wait(0) but that really
		 * isn't what we want since it assumes that the process
		 * that dies is the one we tried to kill.  The chance of
		 * confusion is pretty minimal but if we have waitpid(),
		 * which does what we really want, we should use it.
		 *	--mu@trends.net.
		 */
		memset((void *)&sa,  '\0', sizeof(sa));
		memset((void *)&osa, '\0', sizeof(osa));
		sa.sa_handler = SIG_DFL;
		sigemptyset(&sa.sa_mask);
		sigaction(SIGCHLD, &sa, &osa);
		kill(interpreter_pid(w), SIGTERM);
		waitpid(interpreter_pid(w), &status, 0);
		interpreter_pid(w) = -1;
		sigaction(SIGCHLD, &osa, NULL);
	}

	if(interpreter_input(w) >= 0) {
		GVFile	*ps_old;

		close(interpreter_input(w));
		interpreter_input(w) = -1;
		if(interpreter_input_id(w) != None) {
			XtRemoveInput(interpreter_input_id(w));
			interpreter_input_id(w) = None;
		}
		while(ps_input(w) != NULL) {
			ps_old = ps_input(w);
			ps_input(w) = ps_old->next;
			if(ps_old->close)
				fclose(ps_old->fp);
			XtFree((XtPointer)ps_old);
		}
	}

	if(interpreter_output(w) >= 0) {
		close(interpreter_output(w));
		interpreter_output(w) = -1;
		XtRemoveInput(interpreter_output_id(w));
	}
	if(interpreter_error(w) >= 0) {
		close(interpreter_error(w));
		interpreter_error(w) = -1;
		XtRemoveInput(interpreter_error_id(w));
	}
	gv_busy(w) = False;
	XDefineCursor(XtDisplay(w), XtWindow(w), gv_cursor(w));
}

/*
 * The interpeter failed, Stop what's left and notify application
 */
static void
InterpreterFailed(Widget w)
{
	StopInterpreter(w);
	XtCallCallbackList(w, msg_callback(w), "Failed");
}


	/**
	 ** Public Routines
	 **/


/*
 * Stop any interpreter and disable new ones from starting.
 */
void
GhostviewDisableInterpreter(Widget w)
{
	disable_start(w) = True;
	if(XtIsRealized(w))
		StopInterpreter(w);
}

/*
 * Allow an interpreter to start and start one if the widget is
 * currently realized.
 */
void
GhostviewEnableInterpreter(Widget w)
{
	disable_start(w) = False;
	if(XtIsRealized(w))
		StartInterpreter(w);
}

/*
 * Returns True if the interpreter is ready for new input.
 */
Boolean
GhostviewIsInterpreterReady(Widget w)
{
	return interpreter_pid(w) != -1
		&& !gv_busy(w)
		&& ps_input(w) == NULL;
}

/*
 * Returns True if the interpreter is running.
 */
Boolean
GhostviewIsInterpreterRunning(Widget w)
{
	return interpreter_pid(w) != -1;
}

/*
 * Returns the current backing pixmap.
 */
Pixmap
GhostviewGetBackingPixmap(Widget w)
{
	return (core_bpixmap(w) != XtUnspecifiedPixmap)
		? core_bpixmap(w)
		: None;
}

/*
 * Queue a portion of a PostScript file for output to ghostscript.
 * fp: FILE * of the file in question.  NOTE: if you have several
 * Ghostview widgets reading from the same file.  You must open
 * a unique FILE * for each widget.
 * SendPS does not actually send the PostScript, it merely queues it
 * for output.
 * begin: position in file (returned from ftell()) to start.
 * len:   number of bytes to write.
 *
 * If an interpreter is not running, nothing is queued and
 * False is returned.
 */
Boolean
GhostviewSendPS(Widget w, FILE *fp, off_t begin, unsigned len, Boolean close)
{
	GVFile	*ps_new;

	if(interpreter_input(w) < 0
	|| (ps_new = (GVFile *)XtMalloc(sizeof(GVFile))) == NULL)
		return False;
	ps_new->fp          = fp;
	ps_new->begin       = begin;
	ps_new->len         = len;
	ps_new->seek_needed = True;
	ps_new->close       = close;
	ps_new->next        = NULL;

	if(input_buffer(w) == NULL
	&& (input_buffer(w) = (char *)XtMalloc(GV_BUFSIZ)) == NULL) {
		XtFree((XtPointer)ps_new);
		return False;
	}

	if(ps_input(w) == NULL) {
		input_buffer_ptr(w)  = input_buffer(w);
		bytes_left(w)        = len;
		buffer_bytes_left(w) = 0;
		ps_input(w)          = ps_new;
		interpreter_input_id(w) =
				XtAppAddInput(XtWidgetToApplicationContext(w),
					interpreter_input(w),
					(XtPointer)XtInputWriteMask, Input,
					(XtPointer)w);
	}
	else {
		GVFile	*p;
		for(p = ps_input(w); p->next != NULL; p = p->next)
			;
		p->next = ps_new;
	}

	return True;
}

/*
 * Tell ghostscript to start the next page.
 * Returns False if ghostscript is not running, or not ready to start
 * another page.
 * If another page is started.  Sets the busy flag and cursor.
 */
Boolean
GhostviewNextPage(Widget w)
{
	XEvent	event;

	if(interpreter_pid(w) < 0
	|| gv_mwin(w) == None
	|| gv_busy(w))
		return False;

	gv_busy(w) = True;
	XDefineCursor(XtDisplay(w), XtWindow(w), busy_cursor(w));

	event.xclient.type         = ClientMessage;
	event.xclient.display      = XtDisplay(w);
	event.xclient.window       = gv_mwin(w);
	event.xclient.format       = 32;
	event.xclient.message_type = next_atom(w);
	XSendEvent(XtDisplay(w), gv_mwin(w), False, 0, &event);

	/*
	 * force delivery
	 */
	XSync(XtDisplay(w), False);

	return True;
}

/*
 * convert a screen bounding-box to a PostScript bounding-box
 */
void
GhostviewPSBBox(Widget w, int x1, int y1, int x2, int y2,
						GhostviewReturnStruct *gs)
{
#	define	GVABS(x)	(((x) >= 0) ? (x) : -(x))
	Position	x, y;

	x        = (x1 + x2) / 2;
	y        = (y1 + y2) / 2;
	gs->xdpi = xdpi(w);
	gs->ydpi = ydpi(w);

	switch(orientation(w)) {
	default:
	case GvPageOrientationPortrait:
		gs->psx    = llx(w) + (x * 72.0) / gs->xdpi;
		gs->psy    = ury(w) - (y * 72.0) / gs->ydpi;
		gs->width  = (GVABS(x2 - x1) * 72.0) / gs->xdpi;
		gs->height = (GVABS(y2 - y1) * 72.0) / gs->ydpi;
		break;
	case GvPageOrientationLandscape:
		gs->psx    = llx(w) + (y * 72.0) / gs->ydpi;
		gs->psy    = lly(w) + (x * 72.0) / gs->xdpi;
		gs->width  = (GVABS(y2 - y1) * 72.0) / gs->xdpi;
		gs->height = (GVABS(x2 - x1) * 72.0) / gs->xdpi;
		break;
	case GvPageOrientationUpsideDown:
		gs->psx    = urx(w) - (x * 72.0) / gs->xdpi;
		gs->psy    = lly(w) + (y * 72.0) / gs->ydpi;
		gs->width  = (GVABS(x2 - x1) * 72.0) / gs->xdpi;
		gs->height = (GVABS(y2 - y1) * 72.0) / gs->ydpi;
		break;
	case GvPageOrientationSeascape:
		gs->psx    = urx(w) - (y * 72.0) / gs->ydpi;
		gs->psy    = ury(w) - (x * 72.0) / gs->xdpi;
		gs->width  = (GVABS(y2 - y1) * 72.0) / gs->ydpi;
		gs->height = (GVABS(x2 - x1) * 72.0) / gs->xdpi;
		break;
	}
#	undef	GVABS
}

/*
 * initialize a ghostview widget
 */
void
GhostviewInitialize(Widget w)
{
	disable_start(w) = True;
	if(XtIsRealized(w))
		Setup(w);
}

GvPalette
GhostviewGetPalette(Widget w)
{
	return palette(w);
}

Boolean
GhostviewGetAntiAlias(Widget w)
{
	return antialias(w);
}

GvPageOrientation
GhostviewGetOrientation(Widget w)
{
	return orientation(w);
}

int
GhostviewGetXDPI(Widget w)
{
	return xdpi(w);
}

int
GhostviewGetYDPI(Widget w)
{
	return ydpi(w);
}
