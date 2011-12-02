/* +++FHDR+++
 *
 *	Filename: TimeEntry.h
 *	Module  : Xg WIdget Set
 *	SCCS ID	: 1.1 25 Oct 1995
 *
 *	Description:
 *
 *
 *
 *	Changes
 *
 *	By	Date		Description
 *	------	--------------	----------------------------------------------
 *	messner	08/22/94	Original file
 *
 *
 * ---FHDR--- */

#ifndef _XgTimeEntry_h
#define _XgTimeEntry_h



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/*
 * Defines for new resources
 */
#ifndef XgNshowDate 
#  define XgNshowDate	"showDate"
#endif
#ifndef XgCShowDate
#  define XgCShowDate	"ShowDate"
#endif

#ifndef XgNshowTime
#  define XgNshowTime	"showTime"
#endif
#ifndef XgCShowTime
#  define XgCShowTime	"ShowTime"
#endif

#ifndef XgNshowSeconds
#  define XgNshowSeconds	"showSeconds"
#endif
#ifndef XgCShowSeconds
#  define XgCShowSeconds	"ShowSeconds"
#endif

#ifndef XgNuseTimeZone
#  define XgNuseTimeZone	"useTimeZone"
#endif
#ifndef XgCUseTimeZone
#  define XgCUseTimeZone	"UseTimeZone"
#endif


#ifndef XgNtextForeground
#  define XgNtextForeground "textForeground"
#endif

#ifndef XgNtextBackground
#  define XgNtextBackground "textBackground"
#endif



extern WidgetClass			xgTimeEntryWidgetClass;
typedef struct _XgTimeEntryClassRec	*XgTimeEntryWidgetClass;
typedef struct _XgTimeEntryRec		*XgTimeEntryWidget;



/*
 * Is TimeEntry macro
 */
#define XgIsTimeEntry(w)	(XtIsSubclass (w, xgTimeEntryWidgetClass))


/*
 * Convienience creation function
 */
#ifndef _NO_PROTO
  Widget XgCreateTimeEntry(Widget, char *, ArgList, Cardinal);
#else
  Widget XgCreateTimeEntry();
#endif



/*
 * Declare the XgTimeEntryGet/SetString functions
 */
#ifndef _NO_PROTO
  extern long XgTimeEntryGetValue(Widget w);
  extern int XgTimeEntrySetValue(Widget w, long value);
  extern void XgTimeEntrySetFocus(Widget w, int iWidgetDefine);
#else
  extern long XgTimeEntryGetValue();
  extern int XgTimeEntrySetValue();
  extern void XgTimeEntrySetFocus();
#endif


/*
 * Declare the children get function and constants
 */
#ifndef _NO_PROTO
  extern Widget XgTimeEntryGetChild(Widget w, int child);
#else
  extern Widget XgTimeEntryGetChild();
#endif

#define XgTIMEENTRY_MONTH		0
#define XgTIMEENTRY_DAY_SLASH		1
#define XgTIMEENTRY_DAY			2
#define XgTIMEENTRY_YEAR_SLASH		3
#define XgTIMEENTRY_YEAR		4

#define XgTIMEENTRY_HOURS		5
#define XgTIMEENTRY_MINUTES_COLON	6
#define XgTIMEENTRY_MINUTES		7
#define XgTIMEENTRY_SECONDS_COLON	8
#define XgTIMEENTRY_SECONDS		9

#define XgTIMEENTRY_UP_ARROW		10
#define XgTIMEENTRY_DOWN_ARROW		11



/*
 * xgTimeEntryCallbackStruct
 */
typedef struct {
	int reason;
	XEvent *event;
	long value;
} XgTimeEntryCallbackStruct;


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* _XgTimeEntry_h */
