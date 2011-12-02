/*@
 * Copyright(c) 1995-1997 Gregory M. Messner
 * All rights reserved
 *
 * Permission to use, copy, modify and distribute this material for
 * non-commercial personal and educational use without fee is hereby
 * granted, provided that the above copyright notice and this permission 
 * notice appear in all copies, and that the name of Gregory M. Messner
 * not be used in advertising or publicity pertaining to this material
 * without the specific, prior written permission of Gregory M. Messner 
 * or an authorized representative.
 *
 * GREGORY M. MESSNER MAKES NO REPRESENTATIONS AND EXTENDS NO WARRANTIES, 
 * EXPRESS OR IMPLIED, WITH RESPECT TO THE SOFTWARE, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * ANY PARTICULAR PURPOSE, AND THE WARRANTY AGAINST INFRINGEMENT OF PATENTS
 * OR OTHER INTELLECTUAL PROPERTY RIGHTS. THE SOFTWARE IS PROVIDED "AS IS",
 * AND IN NO EVENT SHALL GREGORY M. MESSNER BE LIABLE FOR ANY DAMAGES,
 * INCLUDING ANY LOST PROFITS OR OTHER INCIDENTAL OR CONSEQUENTIAL DAMAGES
 * RELATING TO THE SOFTWARE.
 *
 */


/* +++FHDR+++
 *
 *	Filename: ClockLabel.c
 *	Module  : Xg Widget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *		This is the source file for a Motif Clock Label.
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	GMM	08/27/93	Original file
 *	GMM	09/02/93	General cleanup
 *
 *
 * ---FHDR--- */



#include	<time.h>
#include        <setjmp.h>

#include	<Xm/XmP.h>


#include	"ClockLabelP.h"


/*
 * Declaration of methods
 */
static void Initialize();
static Boolean SetValues();
static void Destroy();
static void clock_timer();


static XtResource resources[] = {

#define XgOffset(field) XtOffsetOf(XgClockLabelRec, clocklabel.field)

    { XgNupdateWidget, XmCWidget, XmRWidget, sizeof(Widget),
         XgOffset(update_widget), XmRImmediate, (XtPointer)NULL},

    { XgNformatString, XgCFormatString, XmRString, sizeof(String),
	 XgOffset(format_string), XmRImmediate, (XtPointer)NULL},

    { XgNtimeProc, XgCTimeProc, XmRFunction, sizeof(long (*)()),
	XgOffset(time_proc), XmRImmediate, (XtPointer) NULL},

    { XgNupdateRate, XgCUpdateRate, XmRInt, sizeof(int),
	XgOffset(update_rate), XmRImmediate, (XtPointer) 1000},

    { XgNuseTimeZone, XgCUseTimeZone, XmRBoolean, sizeof(Boolean),
	XgOffset(use_timezone), XmRImmediate, (XtPointer)True}
#undef XgOffset
};

XgClockLabelClassRec XgclockLabelClassRec = {
    {
    /* core_class fields */
	/* superclass		*/ (WidgetClass) &xgFastLabelClassRec,
	/* class_name		*/ "XgClockLabel",
	/* widget_size		*/ sizeof(XgClockLabelRec),
	/* class_initialize	*/ NULL,
	/* class_part_initialize*/ NULL,
	/* class_inited		*/ FALSE,
	/* initialize		*/ Initialize,
	/* initialize_hook	*/ NULL,
	/* realize		*/ XtInheritRealize,
	/* actions		*/ NULL,
	/* num_actions		*/ 0,
	/* resources		*/ resources,
	/* num_resources	*/ XtNumber(resources),
	/* xrm_class		*/ NULLQUARK,
	/* compress_motion	*/ TRUE,
	/* compress_exposure	*/ TRUE,
	/* compress_enterleave	*/ TRUE,
	/* visible_interest	*/ TRUE,
	/* destroy		*/ Destroy,
	/* resize		*/ XtInheritResize,
	/* expose		*/ XtInheritExpose,
	/* set_values		*/ SetValues,
	/* set_values_hook	*/ NULL,
	/* set_values_almost	*/ XtInheritSetValuesAlmost,
	/* get_values_hook	*/ NULL,
	/* accept_focus		*/ NULL,
	/* version		*/ XtVersion,
	/* callback_private	*/ NULL,
	/* tm_table		*/ NULL,
	/* query_geometry	*/ ((XtGeometryResult (*)())_XtInherit),
	/* display_accelerator	*/ NULL,
	/* extension		*/ NULL
    },

    /* Primitive Class Part */
    {
        ((XtWidgetProc) _XtInherit),           /* border highlight        */
        ((XtWidgetProc) _XtInherit),           /* border_unhighlight      */
        NULL		   ,                   /* translations            */
        NULL,                                  /* arm and activate        */
        NULL,                                  /* synthetic resources     */
        0,                                     /* number of syn res       */
        NULL                                   /* extension               */
    },

    /* FastLabel Fields */
    {
	0,
    },


    /* ClockLabel fields */
    {
	0,
    }
};


WidgetClass xgClockLabelWidgetClass = (WidgetClass) &XgclockLabelClassRec;


/* +++PHDR+++
 *
 * Function:	Initialize()
 *
 * Scope:	static
 *
 * Description:
 *	This function is the Initialize() method for the XgClockLabel widget.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * request	XgClockLabelWidget	Widget filled in from resources
 * new		XgClockLabelWidget	Copy of request widget that has
 *					  been potentialy altered by 
 *					  XgClockLabel's superclasses
 *
 *
 * Returns:	void
 *
 *
 * ---PHDR--- */

static void
Initialize(request, new)
XgClockLabelWidget	request, new;
{
	/*
	 * 
	 */
	if ( request->clocklabel.format_string != NULL )
	{
		new->clocklabel.my_fmt_string = 
			new->clocklabel.format_string =
				XtNewString(request->clocklabel.format_string);
	}
	else
		new->clocklabel.format_string = 
			new->clocklabel.my_fmt_string = NULL;

	new->clocklabel.old_time = -1;
	clock_timer(new, NULL);
}


/* +++PHDR+++
 *
 * Function:	clock_timer()
 *
 * Scope:	static
 *
 * Description:
 *	This function is 
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:	void
 *
 * ---PHDR--- */

static void
clock_timer(w, id)
XgClockLabelWidget	w;
XtIntervalId *		id;
{
time_t		t;
struct tm       *tptr;
char		time_str[128];
char		*fmt_str = (w->clocklabel.my_fmt_string == NULL ?
	"%D %T" : w->clocklabel.my_fmt_string);

	if ( w->clocklabel.time_proc == NULL )
		time(&t);
	else
		t = (time_t)w->clocklabel.time_proc();

	if ( w->clocklabel.old_time == t && w != NULL )
	{
		if ( w->clocklabel.update_rate > 0 )
			w->clocklabel.timer_id = XtAppAddTimeOut(
				XtWidgetToApplicationContext((Widget)w),
				w->clocklabel.update_rate, 
				(XtTimerCallbackProc)clock_timer, w);

		return;
	}

	w->clocklabel.old_time = t;

	if ( w->clocklabel.use_timezone == True )
		tptr = localtime(&t);
	else
		tptr = gmtime(&t);

	my_strftime(time_str, 127, fmt_str, tptr);

	if ( w->clocklabel.update_rate > 0 )
		w->clocklabel.timer_id = XtAppAddTimeOut(
		    XtWidgetToApplicationContext((Widget)w), 
			w->clocklabel.update_rate, 
			(XtTimerCallbackProc)clock_timer, w);

	if ( w->fastlabel.my_string != NULL )
		if ( strcmp(time_str, w->fastlabel.my_string) == 0 )
			return;
	

	/*
	 * If a widget to update has been specified and we are right
	 * around a minute boundry, force an update on it
	 */
	if ( w->clocklabel.update_widget != NULL )
	{
	int	leftover = t % 60;

		if ( leftover < 2 || leftover > 58 )
			XgClockLabelForceUpdate(w->clocklabel.update_widget);			
	}

	XtVaSetValues((Widget) w, XmNstring, time_str, NULL);
}


/* +++PHDR+++
 *
 * Function:	SetValues()
 *
 * Scope:	static	
 *
 * Description:
 *	SetValues() method for the XgClockLabel widget. 
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * currnet	Widget
 * request	Widget
 * new		Widget
 *
 *
 * Returns:	Boolean which indicates wether the widget needs to be redrawn.
 *
 *
 * ---PHDR--- */

static Boolean SetValues(current, request, new)
Widget	current, request, new;
{
XgClockLabelWidget	cur = (XgClockLabelWidget)current,
		        req = (XgClockLabelWidget)request,
		        neww = (XgClockLabelWidget)new;
Boolean			redraw = False;

	/* 
	 * Check for changed label string
	 */
	if ( cur->clocklabel.format_string != neww->clocklabel.format_string )
	{
	char	*ptr = neww->clocklabel.format_string;

		if ( neww->clocklabel.my_fmt_string != NULL )
		{
			XtFree(neww->clocklabel.my_fmt_string);
			neww->clocklabel.my_fmt_string = NULL;
		}

		if ( ptr != NULL )
			neww->clocklabel.my_fmt_string = XtNewString(ptr);
		else
			neww->clocklabel.my_fmt_string = NULL;

		neww->clocklabel.format_string = neww->clocklabel.my_fmt_string;

		redraw = True;
	}


	/*
	 * Check for changed update rate
	 */
	if ( cur->clocklabel.update_rate != neww->clocklabel.update_rate )
		redraw = True;


	if ( redraw == True )
	{
		if ( neww->clocklabel.timer_id != None )
		{
			XtRemoveTimeOut(neww->clocklabel.timer_id);
			neww->clocklabel.timer_id = None;
		}

		clock_timer(neww, NULL);
	}


	return redraw;
}


/* +++PHDR+++
 *
 * Function:	Destroy()
 *
 * Scope:	static
 *
 * Description:
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 *
 *
 * Returns:	void
 *
 *
 * ---PHDR--- */

static void 
Destroy(w)
XgClockLabelWidget	w;
{
	if ( w->clocklabel.timer_id != None )
		XtRemoveTimeOut(w->clocklabel.timer_id);

	if ( w->clocklabel.my_fmt_string != NULL )
		XtFree(w->clocklabel.my_fmt_string);
}


/* +++PHDR+++
 *
 * Function:	XgClockLabelGetTime()
 *
 * Scope:	extern
 *
 * Description:
 *	This function returns a long that represents the time since Epoch.
 *
 *
 * Argument	Type			Description
 * ------------	-----------------------	-------------------------------------
 * w		Widget			Widget id of XgClockLabel
 *
 *
 * Returns: 	long
 *
 *
 * ---PHDR--- */


long
XgClockLabelGetTime(w)
Widget w; 
{
	/*
	 * Make sure this is a ClockLabel Widget
	 */
	if ( !XgIsClockLabel(w) )
		return 0;

	return(((XgClockLabelWidget)w)->clocklabel.old_time);
}



/* 
 *
 *	Description:
 *	The strftime function places characters into the array pointed to
 *	by 's' as controlled by the string pointed to by 'format'.  The
 *	format shall be a multibyte character sequence, beginning and
 *	ending in its initial shift state.  The format string consists
 *	of zero or more conversion specifiers and ordinary multibyte
 *	characters.  A conversion specifier consists of a % character
 *	followed by a character that determines the behavior of the
 *	conversion character.  All ordinary multibyte characters
 *	(including the terminating null character) are copied unchanged
 *	into the array.  If copying takes place between objects that
 *	overlap, the behavior is undefined.  No more than 'maxsize'
 *	characters are placed into the array.  Each conversion specifier
 *	is replaced by appropriate characters as described in the following
 *	list.  The appropriate characters are determined by the LC_TIME
 *	category of the current locale and by the values contained in
 *	the structure pointed to by timeptr.
 *
 *	%a	is replaced by the locale's abbreviated weekday name
 *	%A	is replaced by the locale's full weekday name
 *	%b	is replaced by the locale's abbreviated month name
 *	%B	is replaced by the locale's full month name
 *	%c	is replaced by the locale's appropriate date and
 *		  time representation
 *	%d	is replaced by the day of the month as a decimal number (01-31)
 *	%H	is replaced by the hour (24-hour clock) as a decimal # (00-23)
 *	%I	is replaced by the hour (12-hour clock) as a decimal # (01-12)
 *	%j	is replaced by the day of the year as a decimal # (001-366)
 *	%m	is replaced by the day of the year as a decimal # (01-12)
 *	%M	is replaced by the minute as a decimal # (00-59)
 *	%p	is replaced by the locale's equivalent of the AM/PM
 *		  designations associated with a 12-hour clock
 *	%S	is replaced by the second as a decimal number (00-61)
 *	%U	is replaced by the week number of the year (the first
 *		  Sunday as the first day of week 1) as a decimal # (00-53).
 *	%w	is replaced by the the weekday as a decimal number [0(Sun) - 6]
 *	%W	is replaced by the week number of the year (the first
 *		  Monday as the first day of week 1) as a decimal # (00-53).
 *	%x	is replaced by the locale's appropriate date representation
 *	%X	is replaced by the locale's appropriate time representation
 *	%y	is replaced by the year without century as a decimal # (00-99)
 *	%Y	is replaced by the the year with century as decimal number.
 *	%Z	is replaced by the time zone name or abbreviation, or by
 *		  no characters if no time zone information is determinable.
 *	%%	is replaced by %.
 *
 *	If a conversion specifier is not one of the above, the behavior
 *	is undefined.
 *
 *	Returns:
 *
 *	If the total number of resulting characters including the
 *	terminating null character is not more than maxsize, the
 *	strftime function returns the number of character placed in
 *	the array pointed to by s not including the terminating null
 *	character.  Otherwise zero is returned and the contents of
 *	the array are indeterminant.
 *
 *	---
 *
 *	The SVID Issue 3 defines a handful of additional conversions:
 *
 *	%D	date as %m/%d/%y
 *	%e	day of month (1-31; single digits are preceded by a blank)
 *	%h	locale's appropriate date and time representation [?]
 *	%n	same as \n
 *	%r	time as %I:%M:%S %p
 *	%R	time as %H:%M
 *	%t	insert a tab
 *	%T	time as %H:%M:%S
 *
 *	These are enabled by defining ENABLE_SVID3
 *
 *	---
 *
 *	Implementation notes: this function knows nothing about locales
 *	and assumes English everywhere.
 *
 *	Character output is done by the single function "outchar", and
 *	it always makes sure that there is sufficient room in the output
 *	for the character being written, and it is where we detect overflow.
 *
 *	This outchar() function could be called several functions deep,
 *	and it quickly became apparent that trying to check the return
 *	value at every point would be maddening and confusing.  So, rather
 *	than try to hack this together, the program uses setjmp to mark
 *	a return place at setup time and then processes normally.  If
 *	outchar() determines that it has run out of room, it does a
 *	longjmp() to the saved place and returns a failed indicator.
 *
 *	---
 *
 *	Unknown formatting requests are included in the output string
 *	literally (i.e., %q on input is copied as "%q" to output).
 *
 *	---
 *
 *	This function is currently NOT reentrant, but could be made so
 *	if we tried hard enough.  Maybe later.
 * ---FHDR--- */

#define		ENABLE_SVID3	TRUE		/* sure, why not	*/

/*------------------------------------------------------------------------
 * these two tables are used for printing the current day and month
 * names.  They should take their information out of the current
 * locale, but we don't know how to do that just yet.
 */
static struct {
	char	*abbr;		/* abbreviated name		*/
	char	*full;		/* full name			*/
}
	wday_name[] = {
		{ "Sun",	"Sunday"	},
		{ "Mon",	"Monday"	},
		{ "Tue",	"Tuesday"	},
		{ "Wed",	"Wednesday"	},
		{ "Thu",	"Thursday"	},
		{ "Fri",	"Friday"	},
		{ "Sat",	"Saturday"	}
	},
	month_name[] = {
		{ "Jan",	"January"	},
		{ "Feb",	"February"	},
		{ "Mar",	"March"		},
		{ "Apr",	"April"		},
		{ "May",	"May"		},
		{ "Jun",	"June"		},
		{ "Jul",	"July"		},
		{ "Aug",	"August"	},
		{ "Sep",	"September"	},
		{ "Oct",	"October"	},
		{ "Nov",	"November"	},
		{ "Dec",	"December"	},
	};

/*------------------------------------------------------------------------
 * these are the variables for our output buffering.  "outbuf" points to
 * the user's output buffer, and it is stored and incremented each time
 * the outchar() function is called.
 *
 */
static char	*outbuf;		/* output buffer		*/
static int	n_out,			/* # of bytes written		*/
		max_out;		/* output buffer size		*/

struct tm	*tm;			/* broken-down time		*/
static jmp_buf	env;			/* placemarker for error hdl	*/

static void do_format();


int
my_strftime(s, maxsize, format, timeptr)
char		*s;		/* output buffer		*/
int		maxsize;	/* max size of output buffer	*/
char	*format;	/* format string		*/
struct tm	*timeptr;	/* broken-down time string	*/
{
	/*----------------------------------------------------------------
	 * set up the output buffer pointers.  These are all used by the
	 * outchar() routine and friends.
	 */
	outbuf  = s;			/* user's output buffer		*/
	max_out = maxsize;		/* max chars to output		*/
	n_out   = 0;			/* # of chars output already	*/

	tm	= timeptr;		/* used by do_format		*/

	/*----------------------------------------------------------------
	 * plant our longjmp marker right here.  If we ever get to this
	 * point, it means we hit the end of our buffer so we return 0
	 * and have an indeterminate buffer.
	 */
	if (setjmp(env))
		return 0;

	/*----------------------------------------------------------------
	 * go do the real work!
	 */
	do_format(format);

	/*----------------------------------------------------------------
	 * at this point we have written all the format-related characters
	 * to the output buffer, but we finally have to place the final
	 * NUL character.  We only do this if we have room...
	 *
	 * Also notice that we don't increment the output count because
	 * the return value is not supposed to include it.
	 */
	if (n_out >= max_out)		/* no room for the NUL?		*/
		return 0;		/* ... return error		*/

	*outbuf = '\0';			/* plant the final NUL		*/

	return n_out;
}

/*
 * outchar()
 *
 *	This is the central place for placing characters into the user's
 *	output buffer.  Before it places the character in the output
 *	buffer, it makes sure that it will assuredly not overwrite
 *	the space provided, and if it does, it longjmps to the main
 *	calling routine.  This way we can completely ignore the overflow
 *	issue in the mainline code.
 */
static void
outchar(c)
int	c;
{
	if (n_out >= max_out)		/* overflow?		*/
		longjmp(env, 1);	/* ... abort!		*/
	*outbuf++ = c;			/* store the char	*/
	n_out++;			/* update output count	*/
}

/*
 * outstring()
 *
 *	Given a string, output it one at a time to the user's buffer.
 */
static void
outstring(s)
register char	*s;
{
	while (*s)
		outchar(*s++);
}

#define		out1digit(n)	outchar('0' + (n))

/*
 * out3digits()
 *
 *	Given a number, output it as a three-digit number with
 *	leading zeros if necessary.  If the number is out of
 *	range, output three ? chars.
 */
static void
out3digits(n)
int	n;
{
	if (n < 0 || n > 999)			/* out of range?	*/
		outstring("???");
	else
	{
		out1digit(  n / 100       );
		out1digit( (n /  10) % 10 );
		out1digit(  n %  10       );
	}
}

/*
 * out2digits()
 *
 *	Given a number, output it as a two-digit number with
 *	leading zeros if necessary.  If the number is out of
 *	range, output two ? chars.
 */
static void
out2digits(n)
int	n;
{
	if (n < 0 || n > 99)			/* out of range?	*/
		outstring("??");
	else
	{
		out1digit(n / 10);
		out1digit(n % 10);
	}
}

/*
 * valid_range()
 *
 *	Given a value, min+max ranges, and a number of chars,
 *	see if the input value is within range.  If not, output
 *	the given number of ? chars and return FALSE, else
 *	return TRUE.
 */
static int
valid_range(n, minval, maxval, nchars)
int	n, minval, maxval, nchars;
{
	if (n >= minval && n <= maxval)
		return 1;
	while (nchars-- > 0)
		outchar('?');
	return 0;
}

/*
 * do_format()
 *
 *	This is the workhorse function of the strftime() package.  It
 *	takes a format string, runs through it, and "outputs" the
 *	resultant time information.  Here, "output" means "save in
 *	user's output buffer", and it is all done eventually through
 *	the "outchar" function or indirectly through related functions.
 *
 *	This function does not place the NUL at the end of the string,
 *	but the output buffer pointer is left pointing there.
 *
 *	Note: this function can be called recursively by composite
 *	format strings (i.e., %D yields %m/%d/%y) so be careful when
 *	making any modifications that would get in the way of doing
 *	this.
 */
static void
do_format(format)
register char	*format;
{
int	tmp;		/* local random temp variable	*/

	for ( ; *format; format++)
	{
		/*--------------------------------------------------------
		 * if the current char is not a %, then it is a regular char
		 * to be put down without change.  Just do it and continue.
		 */
		if (*format != '%')
		{
			outchar(*format);
			continue;
		}

		/*--------------------------------------------------------
		 * At this point we just saw a % sign, so we have to now
		 * interpret it.  Note that we also increment the format
		 * pointer to get past the % sign we just saw.
		 */
		switch (*++format)
		{
		/*--------------------------------------------------------
		 * %% - just a regular % sign, include literally
		 */
		  case '%':
			outchar( '%' );
			break;

		/*--------------------------------------------------------
		 * %a - locale's abbreviated weekday name.  We first make
		 *      sure the value is in range, and if not we output
		 *	??? as a place filler.
		 */
		  case 'a':
			if (valid_range(tm->tm_wday, 0, 6, 3))
				outstring( wday_name[tm->tm_wday].abbr );
			break;

		/*--------------------------------------------------------
		 * %A - locale's full weekday name.  We first make sure
		 *	the value is in range, and if not we output ??????
		 *	as a place filler.
		 */
		  case 'A':
			if (valid_range(tm->tm_wday, 0, 6, 6))
				outstring( wday_name[tm->tm_wday].full );
			break;

		/*--------------------------------------------------------
		 * %b - locale's abbreviated month name.  We first make sure
		 *	the value is in range, and if not we output ??? as a
		 *	place filler.
		 */
		  case 'b':
			if (valid_range(tm->tm_mon, 0, 11, 3))
				outstring( month_name[tm->tm_mon].abbr );
			break;

		/*--------------------------------------------------------
		 * %B - locale's full month name.  We first make sure
		 *	the value is in range, and if not we output ??????
		 *	as a place filler.
		 */
		  case 'B':
			if (valid_range(tm->tm_mon, 0, 11, 6))
				outstring( month_name[tm->tm_mon].full );
			break;

		/*--------------------------------------------------------
		 * %c - locale's appropriate date and time representation
		 *
		 *	example:	%a  %b  %d %H %M %S %Z  %Y
		 *			Sun Aug 12 15:39:49 PDT 1990
		 */
		  case 'c':
			do_format("%a %b %d %H:%M:%S %Z %Y");
			break;

		/*--------------------------------------------------------
		 * %d - day of month as a decimal number		01-31
		 */
		  case 'd':
			out2digits( tm->tm_mday );
			break;

		/*--------------------------------------------------------
		 * %H - hour (24-hour clock) as a decimal number	00-23
		 */
		  case 'H':
			out2digits( tm->tm_hour );
			break;

		/*--------------------------------------------------------
		 * %I - hour (12-hour clock) as a decimal number	01-12
		 */
		  case 'I':
			if (tmp = tm->tm_hour % 12, tmp == 0)
				tmp = 12;
			out2digits( tmp );
			break;

		/*-------------------------------------------------------------
		 * %j - day of the year as a decimal number		001-366
		 */
		  case 'j':
			out3digits( tm->tm_yday + 1 );
			break;

		/*------------------------------------------------------------
		 * %m - month as decimal number				01-12
		 */
		  case 'm':
			out2digits( tm->tm_mon + 1 );
			break;

		/*-----------------------------------------------------------
		 * %M - minute as decimal number			00-59
		 */
		  case 'M':
			out2digits( tm->tm_min );
			break;

		/*-----------------------------------------------------------
		 * %p - locale's equivalent of the AM/PM designations
		 *      associated with a 12-hour clock.
		 */
		  case 'p':
			outstring( ( tm->tm_hour < 12 )  ? "AM" : "PM");
			break;

		/*-----------------------------------------------------------
		 * %S - seconds as a decimal number			00-61
		 *	(with room for a couple of leap seconds)
		 */
		  case 'S':
			out2digits( tm->tm_sec );
			break;

		/*--------------------------------------------------------
		 * %U - week number of the year (first Sunday as first day
		 *	of week 1) as decimal number (00-53)
		 * %W - week number of the year (first Monday as first day
		 *	of week 1) as decimal number (00-53)
 		 *
		 *	In both of these, week 0 is in the days before the
		 *	first week starts.
		 *
		 * Note: the formulas to derive the week numbers were
		 * really ugly to derive and it is hard to describe.
		 *
		 * The general idea is to take the day-of-year and add
		 * some factor to it so division by seven yields the
		 * proper value -- this factor is "tmp".  Once we have
		 * this number, calculating the appropriate week number
		 * is easy.  Getting the number was the hard part.
		 *
		 * A note on the 32767 business.  Part of the formula
		 * is something of the form
		 *
		 *		(yday - wday) % 7
		 *
		 * the difference could be negative, the result of the
		 * modulo operator could be undefined.  Since we are doing
		 * mod-7 arithmetic, we can add any multiple of seven to
		 * yday to keep it greater than wday but not affect the
		 * result.  While any multiple of seven is allowed, we
		 * picked a big one (32767) because it's big and it is
		 * wonderfully magic.  We contemplated not documenting
		 * this to really confuse you but decided against it.
		 *
		 * Anyway, the 'W' case has an additional +1 factor in
		 * it so we add it directly in.  To figure out why the
		 * tmp variable is calculated the way it is, just try
		 * all seven cases and see for yourself.
		 */
		  case 'U':
			tmp = 7 - (32767 + tm->tm_yday - tm->tm_wday) % 7;
			out2digits((tm->tm_yday + tmp) / 7);
			break;

		  case 'W':
			tmp = 7 - (32768 + tm->tm_yday - tm->tm_wday) % 7;
			out2digits((tm->tm_yday + tmp) / 7);
			break;

		/*-----------------------------------------------------------
		 * %w - weekday as a decimal number (0=Sun)		0-6
		 */
		  case 'w':
			out1digit( tm->tm_wday );
			break;

		/*--------------------------------------------------------
		 * %x - locale's appropriate date representation
		 * %D - same as %x (SVID 3 only)
		 *
		 *	example:		%m/%d/%y
		 *				08/07/90
		 */
#ifdef	ENABLE_SVID3
		  case 'D': /*FALLTHROUGH*/
#endif
		  case 'x':
			do_format( "%m/%d/%y" );
			break;

		/*--------------------------------------------------------
		 * %X - locale's appropriate time representation
		 * %T - same as %X (SVID 3 only)
		 *
		 *	example:	%H:%M:%S
		 *			12:43:50
		 */
#ifdef	ENABLE_SVID3
		  case 'T': /*FALLTHROUGH*/
#endif
		  case 'X':
			do_format( "%H:%M:%S" );
			break;

		/*-----------------------------------------------------------
		 * %Y - year with century as decimal number		19YY
		 */
		  case 'Y':
			out2digits( (1900 + tm->tm_year) / 100 );

			/*FALLTHROUGH*/

		/*-----------------------------------------------------------
		 * %y - year without century as decimal number		00-99
		 */
		  case 'y':
			out2digits( tm->tm_year % 100 );
			break;

		/*--------------------------------------------------------
		 * %Z - time zone name or abbreviation, or by no characters
		 *      if no time zone is determinable.
		 */
		  case 'Z':
			if (tm->tm_isdst >= 0)
#if	V_USE_TM_ZONE
				outstring( tm->tm_zone );
#else
				outstring( tzname[ !! tm->tm_isdst ] );
#endif
			else
				; /* unavailable, do nothing */
			break;

#ifdef	ENABLE_SVID3
		/*--------------------------------------------------------
		 * %e - day of month (1-31); single digits preceded by blank
 		 *					(SVID 3 only)
		 */
		  case 'e':
			if (tm->tm_mday >= 0 && tm->tm_mday < 10)
			{
				outchar(' ');
				out1digit( tm->tm_mday );
			}
			else
				out2digits( tm->tm_mday );
			break;

		/*--------------------------------------------------------
		 * %n - same as \n			(SVID 3 only)
		 */
		  case 'n':
			outchar('\n');
			break;

		/*--------------------------------------------------------
		 * %r - time as %I:%M:%S %p		(SVID 3 only)
		 *
		 *	example:	%I %M %S %p
		 *			12:43:43 AM
		 */
		  case 'r':
			do_format("%I:%M:%S %p");
			break;

		/*--------------------------------------------------------
		 * %R - time as %H:%M			(SVID 3 only)
 		 *
		 *	example:	%H %M
 		 *			17 43
		 */
		  case 'R':
			do_format("%H:%M");
			break;

		/*--------------------------------------------------------
		 * %t - insert a regular tab		(SVID 3 only)
		 */
		  case 't':
			outchar('\t');
			break;

#endif	/* ENABLE_SVID3 */

		/*--------------------------------------------------------
		 * oops!  The user's format string with a % as the char
		 * before the NUL, so we stuff a % in the output buffer
		 * and terminate the loop right here.
		 */
		  case '\0':
			outchar('%');
			return;

		/*--------------------------------------------------------
		 * this is called for unknown format indicators.  This is
		 * our current way to deal with them until we find another
		 * better way...
		 */
		  default:
			outchar('%');
			outchar(*format);
			break;
		}
	}
}


/*
 * For the widget to update
 */
void 
XgClockLabelForceUpdate(w)
Widget w;
{
        ((XgClockLabelWidget)w)->clocklabel.old_time = -1;
        clock_timer((XgClockLabelWidget)w, NULL);
}

