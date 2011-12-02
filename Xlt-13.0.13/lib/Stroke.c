/*
   $Id: Stroke.c,v 1.7 2003/06/19 13:03:05 rwscott Exp $

   libStroke is a library to enable any X application to use Strokes.
   Copyright (C) 1997  Richard Scott <rwscott@magi.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "../config.h"

#include <stdio.h>
#include <string.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Xm/MessageB.h>

#include <Stroke.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif


typedef struct
{
    String DefaultHelpString;
}
AppResources_t, *AppResourcesPtr;

static char DefaultHelp[] = "\n\
Strokes are neat, strokes are cool, try man Strokes.\n\
If it does not turn up anything you are out of luck.\n\
";

static AppResources_t AppResources;
static void InputAction(Widget W, XEvent *event);
static void ParentActivate(Widget w, XEvent *event, String *params, Cardinal *num_params);
static void ParentCancel(Widget w, XEvent *event, String *params, Cardinal *num_params);

static XtActionsRec Actions[] =
{
	{"Input", (XtActionProc)InputAction},
	{"StrokeInput", (XtActionProc)InputAction},
	{"ParentActivate", ParentActivate},
	{"ParentCancel", ParentCancel},
};

static XtResource resources[] =
{
	{"strokes", "Strokes", XtRString, sizeof(String), XtOffset(StrokeStatePtr, translations), XtRString, ""},
	{"strokeSound", "StrokeSound", XtRString, sizeof(String), XtOffset(StrokeStatePtr, Sound), XtRImmediate, NULL},
	{"strokeSlop", "StrokeSlop", XtRInt, sizeof(int), XtOffset(StrokeStatePtr, slop), XtRImmediate, (XtPointer)20},
	{"strokeDebug", "StrokeDebug", XtRBoolean, sizeof(Boolean), XtOffset(StrokeStatePtr, Debug), XtRImmediate, (XtPointer)False},
};

static StrokeStatePtr StrokeStateList = NULL;
static StrokeStatePtr StrokeMapList = NULL;

/* ***** */

static void 
CompileTranslations(StrokeStatePtr State)
{
	StrokeMapPtr NewMap;
	String StrokeName;
	String ActionName;
	int BytesRead;
	char *tmp;
	int i;

	i = 0;
	tmp = State->translations;
	StrokeName = XtNewString(tmp);
	ActionName = XtNewString(tmp);
	while (strlen(tmp) > 0)
	{
		StrokeName[0] = '\0';
		ActionName[0] = '\0';
		BytesRead = 0;
		sscanf(tmp, "%s %[^, \t]%n%*[, \t\n]%n", StrokeName, ActionName, &BytesRead, &BytesRead);
		NewMap = XtNew(StrokeMap);
		NewMap->Stroke = XtNewString(StrokeName);
		NewMap->Action = XtNewString(ActionName);
		State->Map = (StrokeMapPtr *) XtRealloc((char *)State->Map, (i + 2) * sizeof(StrokeMapPtr));
		State->Map[i] = NewMap;
		State->Map[i + 1] = NULL;
		tmp += BytesRead;
		i++;
	}
	XtFree(StrokeName);
	XtFree(ActionName);
}

/* ***** */

static StrokeStatePtr 
StrokeGetMap(Widget W)
{
	StrokeStatePtr State;

	State = StrokeMapList;
	while (State != NULL)
	{
		if (State->widget == W)
		{
			break;
		}
		State = State->next;
	}
	if (State == NULL)
	{
		StrokeStatePtr NewState;

		NewState = XtNew(StrokeState_t);
		NewState->next = StrokeMapList;
		StrokeMapList = NewState;
		NewState->widget = W;
		NewState->InStroke = False;
		NewState->points = NULL;
		NewState->box = NULL;
		NewState->Map = NULL;
		NewState->npoints = 0;
		NewState->maxpoints = 0;
		NewState->gc = (GC)NULL;
		XtGetSubresources(XtParent(W),
				  NewState,
				  XtName(W),
				  XtClass(W)->core_class.class_name,
				  resources,
				  XtNumber(resources),
				  NULL,
				  0);
		CompileTranslations(NewState);
		State = NewState;
	}
	return (State);
}

/* ***** */

static void 
ReleaseTranslations(StrokeStatePtr State)
{
	StrokeMapPtr maps;

	maps = *State->Map;
	while (maps != NULL)
	{
		XtFree(maps->Stroke);
		XtFree(maps->Action);
		maps++;
	}
	XtFree((char *)State->Map);
	State->Map = NULL;
	XtFree((char *)State->translations);
	State->translations = NULL;
}

/* ***** */

static void 
AllocatePoints(StrokeStatePtr State)
{
	if (State->npoints == State->maxpoints)
	{
		State->points = (XPoint *)XtRealloc((char *)State->points,
				   (State->maxpoints + 10) * sizeof(XPoint *));
		State->box = (int *)XtRealloc((char *)State->box,
				      (State->maxpoints + 10) * sizeof(int *));
		State->maxpoints += 10;
	}
}

/* ***** */

static void 
DoStroke(XEvent *event, StrokeStatePtr State)
{
	int i;
	int xtol;
	int ytol;
	int LastBox;
	int BoxWidth;
	int BoxHeight;
	int x0, x1, x2, x3;
	int y0, y1, y2, y3;
	String action;
	StrokeMapPtr *tmp = State->Map;
	int FinalPoints;

	/*
	   fprintf(stderr, "DoStroke(%s) - %i %i %i %i\n",
	   XtName(State->widget),
	   State->xmin, State->ymin,
	   State->xmax, State->ymax);
	 */
	if (State->npoints < 2)
		return;

	BoxWidth = State->xmax - State->xmin;
	BoxHeight = State->ymax - State->ymin;
	x0 = State->xmin;
	x1 = State->xmin + (1 * BoxWidth / 3);
	x2 = State->xmin + (2 * BoxWidth / 3);
	x3 = State->xmin + (3 * BoxWidth / 3);
	xtol = BoxWidth / State->slop;
	y0 = State->ymin;
	y1 = State->ymin + (1 * BoxHeight / 3);
	y2 = State->ymin + (2 * BoxHeight / 3);
	y3 = State->ymin + (3 * BoxHeight / 3);
	ytol = BoxHeight / State->slop;
	if (x3 - x0 < 20)
	{
		x1 = x0 - 1;
		x2 = x3 + 1;
	}
	if (y3 - y0 < 20)
	{
		y1 = y0 - 1;
		y2 = y3 + 1;
	}

	if (State->Debug)
	{
		XDrawRectangle(XtDisplay(State->widget),
			       event->xbutton.root,
			       State->gc,
			       x0, y0,
			       BoxWidth,
			       BoxHeight);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x1, y0,
			  x1, y3);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x1 - xtol, y0,
			  x1 - xtol, y3);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x1 + xtol, y0,
			  x1 + xtol, y3);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x2, y0,
			  x2, y3);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x2 - xtol, y0,
			  x2 - xtol, y3);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x2 + xtol, y0,
			  x2 + xtol, y3);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x0, y1,
			  x3, y1);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x0, y1 - ytol,
			  x3, y1 - ytol);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x0, y1 + ytol,
			  x3, y1 + ytol);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x0, y2,
			  x3, y2);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x0, y2 - ytol,
			  x3, y2 - ytol);
		XDrawLine(XtDisplay(State->widget),
			  event->xbutton.root,
			  State->gc,
			  x0, y2 + ytol,
			  x3, y2 + ytol);
	}
	for (i = 1; i < State->npoints; i++)
	{
		int dx = State->points[i - 1].x - State->points[i].x;
		int dy = State->points[i - 1].y - State->points[i].y;
		int maxx = (x2 - xtol) - (x1 + xtol);
		int maxy = (y2 - ytol) - (y1 + ytol);

		if (((dx * dx) >= (maxx * maxx)) || ((dy * dy) >= (maxy * maxy)))
		{
			int j;

			AllocatePoints(State);
			for (j = State->npoints; j > i; j--)
			{
				State->points[j].x = State->points[j - 1].x;
				State->points[j].y = State->points[j - 1].y;
			}
			State->npoints++;
			State->points[i].x = State->points[i - 1].x + (State->points[i].x - State->points[i - 1].x) / 2;
			State->points[i].y = State->points[i - 1].y + (State->points[i].y - State->points[i - 1].y) / 2;
			/*
			   XDrawRectangle(XtDisplay(State->widget),
			   XtWindow(State->widget),
			   State->gc,
			   State->points[i].x - 4,
			   State->points[i].y - 4,
			   8,
			   8);
			 */
			i--;
		}
	}
	for (i = 0; i < State->npoints; i++)
	{
		if ((State->points[i].x >= x0 && State->points[i].x < x1 - xtol) &&
		  (State->points[i].y >= y0 && State->points[i].y < y1 - ytol))
		{
			State->box[i] = 1;
		}
		else if ((State->points[i].x >= x1 + xtol && State->points[i].x < x2 - xtol) &&
		  (State->points[i].y >= y0 && State->points[i].y < y1 - ytol))
		{
			State->box[i] = 2;
		}
		else if ((State->points[i].x >= x2 + xtol && State->points[i].x <= x3) &&
		  (State->points[i].y >= y0 && State->points[i].y < y1 - ytol))
		{
			State->box[i] = 3;
		}
		else if ((State->points[i].x >= x0 && State->points[i].x < x1 - xtol) &&
			 (State->points[i].y >= y1 + ytol && State->points[i].y < y2 - ytol))
		{
			State->box[i] = 4;
		}
		else if ((State->points[i].x >= x1 + xtol && State->points[i].x < x2 - xtol) &&
			 (State->points[i].y >= y1 + ytol && State->points[i].y < y2 - ytol))
		{
			State->box[i] = 5;
		}
		else if ((State->points[i].x >= x2 + xtol && State->points[i].x <= x3) &&
			 (State->points[i].y >= y1 + ytol && State->points[i].y < y2 - ytol))
		{
			State->box[i] = 6;
		}
		else if ((State->points[i].x >= x0 && State->points[i].x < x1 - xtol) &&
		 (State->points[i].y >= y2 + ytol && State->points[i].y <= y3))
		{
			State->box[i] = 7;
		}
		else if ((State->points[i].x >= x1 + xtol && State->points[i].x < x2 - xtol) &&
		 (State->points[i].y >= y2 + ytol && State->points[i].y <= y3))
		{
			State->box[i] = 8;
		}
		else if ((State->points[i].x >= x2 + xtol && State->points[i].x <= x3) &&
		 (State->points[i].y >= y2 + ytol && State->points[i].y <= y3))
		{
			State->box[i] = 9;
		}
		else
		{
			State->box[i] = 0;
		}
	}
	LastBox = 0;
	FinalPoints = 0;
	action = XtNewString("Stroke-");
	for (i = 0; i < State->npoints; i++)
	{
		if ((State->box[i] != 0) && (State->box[i] != LastBox))
		{
			action = XtRealloc(action, 2 + strlen(action));
			sprintf(&action[strlen(action)], "%i", State->box[i]);
			LastBox = State->box[i];
			FinalPoints++;
		}
	}
	{
		StrokeStatePtr WidgetState;

		WidgetState = StrokeGetMap(State->widget);
		tmp = WidgetState->Map;
	}
	while (tmp != NULL && (*tmp) != NULL && FinalPoints > 2)
	{
		if (strcmp(&action[7], (*tmp)->Stroke) == 0)
		{
			if (State->Debug)
			{
				fprintf(stderr, "%s(%i) - %s Calling action >%s<\n",
					__FILE__, __LINE__,
					XtName(State->widget),
					(*tmp)->Action);
			}
			if (State->Sound != NULL)
			{
				XtCallActionProc(State->widget, "PlaySound", NULL, &State->Sound, 1);
			}
			XtCallActionProc(State->widget, (*tmp)->Action, event, NULL, 0);
			break;
		}
		tmp++;
	}
	if (((tmp == NULL) || ((*tmp) == NULL)) && FinalPoints > 2)
	{
		if (State->Debug)
		{
			fprintf(stderr, "%s(%i) - %s Calling action >%s<\n",
				__FILE__,
				__LINE__,
				XtName(State->widget),
				action);
		}
		XtCallActionProc(State->widget, action, event, NULL, 0);
	}
	XtFree(action);
}

/* ***** */

static void Input (Widget W, StrokeStatePtr State, XEvent *event)
{
	/*
	   fprintf(stderr, "Input(%s,%i) - %s %i %i %i %i\n",
	   XtName(State->widget),
	   event->xany.type,
	   XtName(W),
	   event->xbutton.x,
	   event->xbutton.y,
	   event->xbutton.x_root,
	   event->xbutton.y_root);
	 */
	switch (event->xany.type)
	{
	case KeyRelease:
	case ButtonRelease:
		if (State->InStroke)
		{
			int i;

			if (!State->Debug)
			{
				for (i = 1; i < State->npoints; i++)
				{
					XDrawLine(XtDisplay(W),
						  event->xbutton.root,
						  State->gc,
						  State->points[i - 1].x,
						  State->points[i - 1].y,
						  State->points[i].x,
						  State->points[i].y);
					/*
					   XDrawRectangle(XtDisplay(State->widget),
					   XtWindow(State->widget),
					   State->gc,
					   State->points[State->npoints].x - 2,
					   State->points[State->npoints].y - 2,
					   4,
					   4);
					 */
				}
			}
			XtUngrabPointer(W, event->xbutton.time);
			XUngrabServer(XtDisplay(W));
			DoStroke(event, State);
			State->InStroke = False;
			State->npoints = 0;
		}
		break;
	case KeyPress:
	case ButtonPress:
		if (!State->InStroke)
		{
			State->widget = W;
			State->npoints = 0;
			AllocatePoints(State);
			/*
			   TranslateCoords(W, State->widget,
			   &((XButtonPressedEvent *)event)->x,
			   &((XButtonPressedEvent *)event)->y);
			 */
			State->points[State->npoints].x = event->xbutton.x_root;
			State->points[State->npoints].y = event->xbutton.y_root;
			State->xmin = State->points[State->npoints].x;
			State->ymin = State->points[State->npoints].y;
			State->xmax = State->points[State->npoints].x;
			State->ymax = State->points[State->npoints].y;
			State->npoints++;
			State->InStroke = True;
			XGrabServer(XtDisplay(W));
			XtGrabPointer(W, 
				False,
				ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
				GrabModeAsync, GrabModeAsync, None, None, event->xbutton.time);
		}
		break;
	case MotionNotify:
		if (State->gc == (GC)NULL)
		{
			Pixel Background;

			State->gc = XCreateGC(XtDisplay(State->widget),
					      XtWindow(State->widget),
					      0, NULL);
			XtVaGetValues(State->widget,
				      XtNbackground, &Background,
				      NULL);
			XSetForeground(XtDisplay(State->widget),
				       State->gc,
				       Background);
			XSetFunction(XtDisplay(State->widget), State->gc, GXxor);
			XSetSubwindowMode(XtDisplay(State->widget), State->gc, IncludeInferiors);
		}
		if (State->InStroke)
		{
			int dx = State->points[State->npoints - 1].x - event->xmotion.x_root;
			int dy = State->points[State->npoints - 1].y - event->xmotion.y_root;

			if (dx * dx + dy * dy > State->slop)
			{
				AllocatePoints(State);
				State->points[State->npoints].x = event->xmotion.x_root;
				State->points[State->npoints].y = event->xmotion.y_root;
				XDrawLine(XtDisplay(State->widget),
					  event->xmotion.root,
					  State->gc,
					  State->points[State->npoints - 1].x,
					  State->points[State->npoints - 1].y,
					  State->points[State->npoints].x,
					  State->points[State->npoints].y);
				State->xmin = State->xmin < State->points[State->npoints].x ? State->xmin : State->points[State->npoints].x;
				State->xmax = State->xmax > State->points[State->npoints].x ? State->xmax : State->points[State->npoints].x;
				State->ymin = State->ymin < State->points[State->npoints].y ? State->ymin : State->points[State->npoints].y;
				State->ymax = State->ymax > State->points[State->npoints].y ? State->ymax : State->points[State->npoints].y;
				State->npoints++;
			}
		}
		break;
	default:
		fprintf(stderr, "%s(%i): Input(%s) - %s Unknown event type %i\n",
			__FILE__,
			__LINE__,
			XtName(State->widget),
			XtName(W),
			event->type);
	}
}

/* ***** */

static void 
InputAction(Widget W, XEvent *event)
{
	StrokeStatePtr State = StrokeStateList;

	if (State)
	{
		Input (W, State, event);
	}
}

/* ***** */

static void 
_StrokeInitialize(Widget W)
{
	StrokeStatePtr NewState;

	if (StrokeStateList == NULL)
	{
		NewState = XtNew(StrokeState_t);
		NewState->next = StrokeStateList;
		StrokeStateList = NewState;
		NewState->widget = W;
		NewState->InStroke = False;
		NewState->points = NULL;
		NewState->box = NULL;
		NewState->Map = NULL;
		NewState->npoints = 0;
		NewState->maxpoints = 0;
		NewState->gc = (GC)NULL;
		XtGetApplicationResources(W,
					  NewState,
					  resources, XtNumber(resources),
					  NULL, 0);
		if (NewState->slop < 3)
		{
			XtWarning("Stroke slop cannot be less than 3");
			NewState->slop = 3;
		}
	}
	XtAppAddActions(XtWidgetToApplicationContext(W),
			Actions, XtNumber(Actions));
}

/* ***** */

void 
StrokeInitialize(Widget W)
{
	_StrokeInitialize(W);
}

/* ***** */

void 
StrokeSetDebug(Widget W, Boolean Debug)
{
	StrokeStatePtr State = StrokeStateList;

	if (State != NULL)
	{
		State->Debug = Debug;
	}
}

/* ***** */

Boolean 
StrokeGetDebug(Widget W)
{
	StrokeStatePtr State = StrokeStateList;

	if (State != NULL)
	{
		return (State->Debug);
	}
	else
	{
		return (False);
	}
}

/* ***** */

void 
StrokeSetMapping(Widget W, String Map)
{
	StrokeStatePtr State;

	State = StrokeGetMap(W);
	if (State != NULL)
	{
		ReleaseTranslations(State);
		State->translations = XtNewString(Map);
		CompileTranslations(State);
	}
}

/* ***** */

String 
StrokeGetMapping(Widget W)
{
	StrokeStatePtr State;

	State = StrokeGetMap(W);
	if (State != NULL)
	{
		return (XtNewString(State->translations));
	}
	else
	{
		return (NULL);
	}
}

static void ParentCancel(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
	if (XmIsManager(w))
	{
		XtCallActionProc(w, "ManagerParentCancel", event, params, *num_params);
	}
	else if (XmIsPrimitive(w))
	{
		XtCallActionProc(w, "PrimitiveParentCancel", event, params, *num_params);
	}
	else
	{
	}
}

/* ******************** */

static void ParentActivate(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
	if (XmIsManager(w))
	{
		XtCallActionProc(w, "ManagerParentActivate", event, params, *num_params);
	}
	else if (XmIsPrimitive(w))
	{
		XtCallActionProc(w, "PrimitiveParentActivate", event, params, *num_params);
	}
	else
	{
	}
}

/* ***** */

void
XltHelpOnStrokes(Widget W)
{
static Widget Dialog = NULL;
Widget Shell = W;

	while (!XtIsShell(Shell)) Shell = XtParent(Shell);
	if (Dialog == NULL)
	{
	XmString string;
	XtResource resources[] =
	{
	    {"messageString", "MessageString", XtRString, sizeof(String), XtOffset(AppResourcesPtr, DefaultHelpString), XtRImmediate, DefaultHelp},
	};
	    Dialog = XmCreateInformationDialog(Shell, "HelpOnStrokes", NULL, 0);
	    XtGetSubresources(W,
			      &AppResources,
			      XtName(Dialog),
			      XtClass(Dialog)->core_class.class_name,
			      resources,
			      XtNumber(resources),
			      NULL, 0);
	    string = XmStringCreateLtoR(AppResources.DefaultHelpString, XmFONTLIST_DEFAULT_TAG);
	    XtVaSetValues(Dialog,
		    XmNmessageString, string,
		    NULL);
	    XmStringFree(string);
	    XtUnmanageChild(XmMessageBoxGetChild(Dialog, XmDIALOG_HELP_BUTTON));
	}
	XtManageChild(Dialog);
}

