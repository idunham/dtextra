/*
** Logo.c for XmtdsLogo widget in .
** 
** Made by MaxTheDogSays (dubray_f@epita.fr && rancur_v@epita.fr
** Login   <vr@epita.fr>
** 
** Started on  Mon May 16 22:24:08 1994 vr
** Last update Mon Nov 21 12:42:20 1994 hydrix
*/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <std/sysdep.h>
#include <std/algeb.h>
#include <std/Xstd.h>
#include "LogoP.h"

static void			Initialize();
static void			SetValues();
static void			Redisplay();
static void			Destroy();
static void			Resize();

static XtResource	resources[] =
{
{XtNforeground,		XtCForeground,	XtRPixel,	sizeof (Pixel),
    XtOffset(XmtdsLogoWidget,	xmtds_logo.foreground),
    XtRString,		"black"},
{XtNbackground,		XtCBackground,	XtRPixel,	sizeof (Pixel),
    XtOffset(XmtdsLogoWidget,	xmtds_logo.background),
    XtRString,		"white"}
};

XmtdsLogoClassRec	xmtdsLogoClassRec = {
{
   /* core */
   (WidgetClass)&widgetClassRec,	/* superclass		*/
   "XmtdsLogo",				/* class name		*/
   sizeof(XmtdsLogoRec),		/* size			*/   
   NULL,				/* class initialize	*/   
   NULL,				/* class_part_init	*/   
   FALSE,				/* class_inited		*/   
   Initialize,				/* initialize		*/   
   NULL,				/* initialize_hook	*/   
   XtInheritRealize,			/* realize		*/   
   NULL,				/* actions		*/   
   0,					/* num_actions		*/   
   resources,				/* resources		*/   
   XtNumber(resources),			/* resource_count	*/   
   NULLQUARK,				/* xrm_class		*/   
   TRUE,				/* compress_motion	*/   
   TRUE,				/* compress_exposure	*/   
   TRUE,				/* compress_enterleave	*/   
   FALSE,				/* visible_interest	*/   
   Destroy,				/* destroy		*/   
   Resize,				/* resize		*/   
   Redisplay,				/* expose		*/   
   NULL,				/* set_values		*/   
   NULL,				/* set_values_hook	*/   
   XtInheritSetValuesAlmost,		/* set_values_almost	*/   
   NULL,				/* get_values_hook	*/   
   NULL,				/* accept_focus		*/   
   XtVersion,				/* version		*/   
   NULL,				/* callback_private	*/   
   NULL,				/* tm_table		*/   
   XtInheritQueryGeometry,		/* query_geometry	*/   
   XtInheritDisplayAccelerator,		/* display_accelerator	*/   
   NULL,				/* extension		*/   
},
/* xmtds_logo */
{
   0,					/* empty		*/
}
};

WidgetClass	xmtdsLogoWidgetClass = (WidgetClass)&xmtdsLogoClassRec;

static void		Initialize(request,w,args,num_args)
XmtdsLogoWidget		request;
XmtdsLogoWidget		w;
ArgList			args;
Cardinal		*num_args;
{
   XGCValues		xgcv;
   
   xgcv.foreground = LOGO.foreground;;
   LOGO.fg_gc =
      XtGetGC(w,GCForeground,&xgcv);
   xgcv.background = LOGO.background;;
   LOGO.bg_gc =
      XtGetGC(w,GCBackground,&xgcv);
   CORE.background_pixel = LOGO.background;
   CORE.width = 100;
   CORE.height = 100;
}

static void		SetValues(old,request,new,args,num_args)
XmtdsLogoWidget	old;
XmtdsLogoWidget	request;
XmtdsLogoWidget	new;
ArgList			args;
Cardinal		*num_args;
{
}

static void		Redisplay(w,event,region)
XmtdsLogoWidget		w;
XExposeEvent		*event;
Region			region;
{
   Resize(w);
}

static void		Destroy(w)
XmtdsLogoWidget	w;
{
   XtReleaseGC(w,LOGO.fg_gc);
   XtReleaseGC(w,LOGO.bg_gc);
}

static void		Resize(w)
XmtdsLogoWidget	w;
{
   XmuDrawLogo(XtDisplay(w),
	       XtWindow(w),
	       LOGO.fg_gc,
	       LOGO.bg_gc,
	       0,
	       0,
	       CORE.width,
	       CORE.height);
}
