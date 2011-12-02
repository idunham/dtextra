/*
** Tetris.c for XmtdsTetris widget in .
** 
** Made by MaxTheDogSays (dubray_f@epita.fr && rancur_v@epita.fr
** Login   <vr@epita.fr>
** 
** Started on  Mon May 16 22:24:08 1994 vr
** Last update Mon Nov 21 12:37:46 1994 hydrix
*/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <std/sysdep.h>
#include <std/algeb.h>
#include <std/Xstd.h>
#include "TetrisP.h"

static void			Initialize();
static void			SetValues();
static void			Redisplay();
static void			Destroy();
static void			Resize();

static XtTimerCallbackProc	Timer();

static void			MoveLeftAction();
static void			MoveRightAction();
static void			MoveDownAction();
static void			RotateLeftAction();
static void			RotateRightAction();
static void			DropAction();
static void			PauseAction();
static void			StartAction();
static void			GrExpAction();

static void			draw_block();
static void			erase_block();
static void			update_color_list();
static void			destroy_color_list();
static void			refresh_video();
static void			test_vm();
static void			draw_shape();
static int			test_shape();
static void			video_scroll();
static void			grfx_scroll();
static void			lookup_lines();
static void			tetris_send();

/*(c) 1994 BriX Corporation...*/
static  shape_def sd_tab[]=
{
{57856,	51328,	36352,	8800},
{59392,	35008,	736,	25120},
{52224,	52224,	52224,	52224},
{61440,	17476,	61440,	8738},
{50688,	19584,	50688,	19584},
{27648,	35904,	27648,	35904},
{58368,	17984,	19968,	19520}
};

static int		num_shapes = XtNumber(sd_tab);

static char	defaultTranslations[] =
"<Btn1Down>:	move-left()\n\
<Btn2Down>:	rotate-right()\n\
<Btn3Down>:	move-right()\n\
<Key>Left:	move-left()\n\
<Key>Right:	move-right()\n\
<Key>space:	drop()\n\
<Key>Up:	rotate-right()\n\
<Key>Down:	move-down()\n\
<Key>P:		pause()\n\
<Key>S:		start()\n\
<GrExp>:	grexp()";

static XtActionsRec	actionsList[] =
{
{"move-left",	(XtActionProc)MoveLeftAction},
{"move-right",	(XtActionProc)MoveRightAction},
{"move-down",	(XtActionProc)MoveDownAction},
{"rotate-left",	(XtActionProc)RotateLeftAction},
{"rotate-right",(XtActionProc)RotateRightAction},
{"drop",	(XtActionProc)DropAction},
{"pause",	(XtActionProc)PauseAction},
{"start",	(XtActionProc)StartAction},
{"grexp",	(XtActionProc)GrExpAction}
};

static XtResource	resources[] =
{
{XtNbackground,		XtCBackground,	XtRPixel,	sizeof (Pixel),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.background),
    XtRString,		"black"},
{XtNinterval,		XtCInterval,	XtRInt,		sizeof (int),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.interval),
    XtRString,		"300"},
{XtNblockWidth,		XtCBlockWidth,	XtRInt,		sizeof (int),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.block_width),
    XtRString,		"20"},
{XtNblockHeight,	XtCBlockHeight,	XtRInt,		sizeof (int),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.block_height),
    XtRString,		"20"},
{XtNgameWidth,		XtCGameWidth,	XtRInt,		sizeof (int),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.game_width),
    XtRString,		"9"},
{XtNgameHeight,	XtCGameHeight,		XtRInt,		sizeof (int),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.game_height),
    XtRString,		"20"},
{XtNcolorList,	XtCColorList,		XtRString,	sizeof (char *),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.color_list),
    XtRString,		"white"},
{XtNscoreCallback,	XtCCallback,	XtRCallback,	sizeof(caddr_t),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.score_callback),
    XtRCallback,	NULL},
{XtNcomm,		XtCComm,	XtRBoolean,	sizeof (Boolean),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.comm),
    XtRString,		"FALSE"},
{XtNfd,			XtCFd,		XtRInt,		sizeof (int),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.fd),
    XtRString,		"0"},
{XtNuserData,		XtCUserData,	XtRPointer,	sizeof (caddr_t),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.user_data),
    XtRString,		NULL},
{XtNslave,		XtCSlave,	XtRBoolean,	sizeof (Boolean),
    XtOffset(XmtdsTetrisWidget,	xmtds_tetris.slave),
    XtRString,		"FALSE"}
};

XmtdsTetrisClassRec	xmtdsTetrisClassRec = {
{
   /* core */
   (WidgetClass)&widgetClassRec,	/* superclass		*/
   "XmtdsTetris",			/* class name		*/
   sizeof(XmtdsTetrisRec),		/* size			*/   
   NULL,				/* class initialize	*/   
   NULL,				/* class_part_init	*/   
   FALSE,				/* class_inited		*/   
   Initialize,				/* initialize		*/   
   NULL,				/* initialize_hook	*/   
   XtInheritRealize,			/* realize		*/   
   actionsList,				/* actions		*/   
   XtNumber(actionsList),		/* num_actions		*/   
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
 defaultTranslations,			/* tm_table		*/   
   XtInheritQueryGeometry,		/* query_geometry	*/   
   XtInheritDisplayAccelerator,		/* display_accelerator	*/   
   NULL,				/* extension		*/   
},
/* xmtds_tetris */
{
   0,					/* empty		*/
}
};

WidgetClass	xmtdsTetrisWidgetClass = (WidgetClass)&xmtdsTetrisClassRec;

static void		Initialize(request,new,args,num_args)
XmtdsTetrisWidget	request;
XmtdsTetrisWidget	new;
ArgList			args;
Cardinal		*num_args;
{
   XGCValues		xgcv;
   
   if (REQUEST_TETRIS.game_width < 4)
   {
      XtWarning("too little gameWidth");
      NEW_TETRIS.game_width = 4;
   }
   if (REQUEST_TETRIS.game_height < 4)
   {
      XtWarning("too little gameHeight");
      NEW_TETRIS.game_height = 4;
   }
   xgcv.foreground = W_BPX(new);
   NEW_TETRIS.black_gc = XtGetGC(new,
				 GCForeground,
				 &xgcv);
   xgcv.foreground = W_WPX(new);
   NEW_TETRIS.white_gc = XtGetGC(new,
				 GCForeground,
				 &xgcv);
   xgcv.foreground = NEW_TETRIS.background;
   NEW_TETRIS.bg_gc = XtGetGC(new,
			      GCForeground,
			      &xgcv);
   xgcv.fill_style = FillTiled;
   xgcv.tile = XmuCreateStippledPixmap(XtScreen(new),
				       W_BPX(new),
				       W_WPX(new),
				       W_DEPTH(new));   
   NEW_TETRIS.gray_gc = XtGetGC(new,
				GCTile|GCFillStyle,
				&xgcv);
   NEW_TETRIS.pixel_list = NULL;
   update_color_list(new,NEW_TETRIS.color_list);
   MALLOC(NEW_TETRIS.video_mem,
	  NEW_TETRIS.game_width*NEW_TETRIS.game_height*
	  sizeof (tm_attr_union),
	  "XmtdsTetrisWidget");
   bzero(NEW_TETRIS.video_mem,
	 NEW_TETRIS.game_width*NEW_TETRIS.game_height*
	 sizeof (tm_attr_union));
   NEW_TETRIS.start = TRUE;
   NEW_CORE.background_pixel = NEW_TETRIS.background;
   NEW_CORE.width =
      NEW_TETRIS.game_width*NEW_TETRIS.block_width;
   NEW_CORE.height =
      NEW_TETRIS.game_height*NEW_TETRIS.block_height;
   NEW_TETRIS.next_shape = RAND(num_shapes);  
   NEW_TETRIS.xt_interval_id = 0;
   NEW_TETRIS.pause = FALSE;
}

static void		SetValues(old,request,new,args,num_args)
XmtdsTetrisWidget	old;
XmtdsTetrisWidget	request;
XmtdsTetrisWidget	new;
ArgList			args;
Cardinal		*num_args;
{
}

static void		Redisplay(w,event,region)
XmtdsTetrisWidget	w;
XExposeEvent		*event;
Region			region;
{
   refresh_video(w,
		 event->x/TETRIS.block_width,
		 event->y/TETRIS.block_height,
		 event->width/TETRIS.block_width+2,
		 event->height/TETRIS.block_height+2);
}

static void		Destroy(w)
XmtdsTetrisWidget	w;
{
   XtReleaseGC(w,TETRIS.black_gc);
   XtReleaseGC(w,TETRIS.white_gc);
   XtReleaseGC(w,TETRIS.bg_gc);
   if (!TETRIS.slave)
      XtRemoveTimeOut(TETRIS.xt_interval_id);
   destroy_color_list(w);
}

static void		Resize(w)
XmtdsTetrisWidget	w;
{
   TETRIS.block_width = 
      w->core.width/TETRIS.game_width;
   TETRIS.block_height = 
      w->core.height/TETRIS.game_height;
}

static XtTimerCallbackProc	Timer(w,id)
XmtdsTetrisWidget		w;
XtIntervalId			*id;
{
   XmtdsTetrisGoDown(w);
   TETRIS.xt_interval_id =
      XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)w),
		      TETRIS.interval,
		      Timer,
		      w);
} 

static void		MoveLeftAction(w,event,args,n_args)
XmtdsTetrisWidget	w;
XEvent			*event;
String			*args;
int			*n_args;
{
   int			i;
   int			j;
   int			val;
   Boolean		ok;
   
   if (TETRIS.start)
      return ;
   ok = TRUE;
   val = sd_tab[TETRIS.shape][TETRIS.rotation];
   j = 0;
   while (j < 4)
   {
      i = 0;
      while (i < 4)
      {
	 if (VAL_TEST(val,i,j))
	    if ((TETRIS.x+i == 0) ||
		(VM_STATE(w,TETRIS.x+i-1,TETRIS.y+j) ==
		 STATE_FIXED))
	    {
	       ok = FALSE;
	       break ;
	    }
	 i++;
      }
      if (ok)
	 j++;
      else
	 break ;
   }
   if (ok)
   {
      j = 0;
      while (j < 4)
      {
	 i = 0;
	 while (i < 4)
	 {
	    if (VM_STATE(w,
			 TETRIS.x+i,
			 TETRIS.y+j) == STATE_MOVING)
	    {
	       erase_block(w,TETRIS.x+i,TETRIS.y+j);
	       VM_STATE(w,TETRIS.x+i,TETRIS.y+j) =
		  STATE_NOTHING;
	    }
	    i++;
	 }
	 j++;
      }
      TETRIS.x--;
      draw_shape(w,
		 TETRIS.x,
		 TETRIS.y,
		 val,
		 TETRIS.color);
      if (TETRIS.comm)
	 tetris_send(w,PROTO_MOVE_LEFT);
   }
}

static void		MoveRightAction(w,event,args,n_args)
XmtdsTetrisWidget	w;
XEvent			*event;
String			*args;
int			*n_args;
{
   int			i;
   int			j;
   int			val;
   Boolean		ok;
   
   if (TETRIS.start)
      return ;
   ok = TRUE;
   val = sd_tab[TETRIS.shape][TETRIS.rotation];
   j = 0;
   while (j < 4)
   {
      i = 0;
      while (i < 4)
      {
	 if (VAL_TEST(val,i,j))
	    if ((TETRIS.x+i+1 == TETRIS.game_width) ||
		(VM_STATE(w,TETRIS.x+i+1,TETRIS.y+j) ==
		 STATE_FIXED))
	    {
	       ok = FALSE;
	       break ;
	    }
	 i++;
      }
      if (ok)
	 j++;
      else
	 break ;
   }
   if (ok)
   {
      j = 0;
      while (j < 4)
      {
	 i = 0;
	 while (i < 4)
	 {
	    if (VM_STATE(w,
			 TETRIS.x+i,
			 TETRIS.y+j) == STATE_MOVING)
	    {
	       erase_block(w,TETRIS.x+i,TETRIS.y+j);
	       VM_STATE(w,TETRIS.x+i,TETRIS.y+j) =
		  STATE_NOTHING;
	    }
	    i++;
	 }
	 j++;
      }
      TETRIS.x++;
      draw_shape(w,
		 TETRIS.x,
		 TETRIS.y,
		 val,
		 TETRIS.color);
      if (TETRIS.comm)
	 tetris_send(w,PROTO_MOVE_RIGHT);  
   }
}

static void		MoveDownAction(w,event,args,n_args)
XmtdsTetrisWidget	w;
XEvent			*event;
String			*args;
int			*n_args;
{
   int			i;
   int			j;
   int			val;
   Boolean		ok;
   
   if (TETRIS.start)
      return ;
   ok = TRUE;
   val = sd_tab[TETRIS.shape][TETRIS.rotation];
   j = 0;
   while (j < 4)
   {
      i = 0;
      while (i < 4)
      {
	 if (VAL_TEST(val,i,j))
	    if ((TETRIS.y+j+1 == TETRIS.game_height) ||
		(VM_STATE(w,TETRIS.x+i,TETRIS.y+j+1) ==
		 STATE_FIXED))
	    {
	       ok = FALSE;
	       break ;
	    }
	 i++;
      }
      if (ok)
	 j++;
      else
	 break ;
   }
   if (ok)
   {
      j = 0;
      while (j < 4)
      {
	 i = 0;
	 while (i < 4)
	 {
	    if (VM_STATE(w,
			 TETRIS.x+i,
			 TETRIS.y+j) == STATE_MOVING)
	    {
	       erase_block(w,TETRIS.x+i,TETRIS.y+j);
	       VM_STATE(w,TETRIS.x+i,TETRIS.y+j) =
		  STATE_NOTHING;
	    }
	    i++;
	 }
	 j++;
      }
      TETRIS.y++;
      draw_shape(w,
		 TETRIS.x,
		 TETRIS.y,
		 val,
		 TETRIS.color);
      if (TETRIS.comm)
	 tetris_send(w,PROTO_MOVE_DOWN);
   }
}

static void		RotateLeftAction(w,event,args,n_args)
XmtdsTetrisWidget	w;
XEvent			*event;
String			*args;
int			*n_args;
{
   int			i;
   int			j;
   int			val;
   int			rotation;
   Boolean		ok;
   
   if (TETRIS.start)
      return ;
   rotation = (TETRIS.rotation+1)%4;
   val = sd_tab[TETRIS.shape][rotation];
   ok = TRUE;
   j = 0;
   while (j < 4)
   {
      i = 0;
      while (i < 4)
      {
	 if (VAL_TEST(val,i,j))
	    if ((TETRIS.x+i < 0) ||
		(TETRIS.x+i >= TETRIS.game_width) ||
		(TETRIS.y+j < 0) ||
		(TETRIS.y+j >= TETRIS.game_height) ||
		(VM_STATE(w,TETRIS.x+i,TETRIS.y+j) ==
		 STATE_FIXED))
	    {
	       ok = FALSE;
	       break ;
	    }
	 i++;
      }
      if (ok)
	 j++;
      else
	 break ;
   }
   if (ok)
   {
      j = 0;
      while (j < 4)
      {
	 i = 0;
	 while (i < 4)
	 {
	    if (VM_STATE(w,
			 TETRIS.x+i,
			 TETRIS.y+j) == STATE_MOVING)
	    {
	       erase_block(w,TETRIS.x+i,TETRIS.y+j);
	       VM_STATE(w,TETRIS.x+i,TETRIS.y+j) =
		  STATE_NOTHING;
	    }
	    i++;
	 }
	 j++;
      }
      TETRIS.rotation = rotation;
      draw_shape(w,
		 TETRIS.x,
		 TETRIS.y,
		 val,
		 TETRIS.color);
      if (TETRIS.comm)
	 tetris_send(w,PROTO_ROTATE_LEFT);
   }
}

static void		RotateRightAction(w,event,args,n_args)
XmtdsTetrisWidget	w;
XEvent			*event;
String			*args;
int			*n_args;
{
   int			i;
   int			j;
   int			val;
   int			rotation;
   Boolean		ok;
   
   if (TETRIS.start)
      return ;
   rotation = TETRIS.rotation-1;
   if (rotation == -1)
      rotation = 3;
   val = sd_tab[TETRIS.shape][rotation];
   ok = TRUE;
   j = 0;
   while (j < 4)
   {
      i = 0;
      while (i < 4)
      {
	 if (VAL_TEST(val,i,j))
	    if ((TETRIS.x+i < 0) ||
		(TETRIS.x+i >= TETRIS.game_width) ||
		(TETRIS.y+j < 0) ||
		(TETRIS.y+j >= TETRIS.game_height) ||
		(VM_STATE(w,TETRIS.x+i,TETRIS.y+j) ==
		 STATE_FIXED))
	    {
	       ok = FALSE;
	       break ;
	    }
	 i++;
      }
      if (ok)
	 j++;
      else
	 break ;
   }
   if (ok)
   {
      j = 0;
      while (j < 4)
      {
	 i = 0;
	 while (i < 4)
	 {
	    if (VM_STATE(w,
			 TETRIS.x+i,
			 TETRIS.y+j) == STATE_MOVING)
	    {
	       erase_block(w,TETRIS.x+i,TETRIS.y+j);
	       VM_STATE(w,TETRIS.x+i,TETRIS.y+j) =
		  STATE_NOTHING;
	    }
	    i++;
	 }
	 j++;
      }
      TETRIS.rotation = rotation;
      draw_shape(w,
		 TETRIS.x,
		 TETRIS.y,
		 val,
		 TETRIS.color);
      if (TETRIS.comm)
	 tetris_send(w,PROTO_ROTATE_RIGHT);
   }
}

static void		DropAction(w,event,args,n_args)
XmtdsTetrisWidget	w;
XEvent			*event;
String			*args;
int			*n_args;
{
   int			i;
   int			j;
   int			val;
   Boolean		ok;
   int			y;
   
   y = TETRIS.y;
   ok = TRUE;
   val = sd_tab[TETRIS.shape][TETRIS.rotation];
   while (ok)
   {
      j = 0;
      while (j < 4)
      {
	 i = 0;
	 while (i < 4)
	 {
	    if (VAL_TEST(val,i,j))
	       if ((y+j+1 == TETRIS.game_height) ||
		   (VM_STATE(w,TETRIS.x+i,y+j+1) ==
		    STATE_FIXED))
	       {
		  ok = FALSE;
		  break ;
	       }
	    i++;
	 }
	 if (ok)
	    j++;
	 else
	    break ;
      }
      y++;
   }
   j = 0;
   while (j < 4)
   {
      i = 0;
      while (i < 4)
      {
	 if (VM_STATE(w,
		      TETRIS.x+i,
		      TETRIS.y+j) == STATE_MOVING)
	 {
	    erase_block(w,TETRIS.x+i,TETRIS.y+j);
	    VM_STATE(w,TETRIS.x+i,TETRIS.y+j) =
	       STATE_NOTHING;
	 }
	 i++;
      }
      j++;
   }
   TETRIS.y = --y;
   draw_shape(w,
	      TETRIS.x,
	      TETRIS.y,
	      val,
	      TETRIS.color);
   j = 0;
   while (j < 4)
   {
      i = 0;
      while (i < 4)
      {
	 if (VM_STATE(w,
		      TETRIS.x+i,
		      TETRIS.y+j) == STATE_MOVING)
	    VM_STATE(w,TETRIS.x+i,TETRIS.y+j) =
	       STATE_FIXED;
	 i++;
      }
      j++;
   }
   lookup_lines(w);
   TETRIS.start = TRUE;
   if (TETRIS.comm)
      tetris_send(w,PROTO_DROP);
}

static void		PauseAction(w,event,args,n_args)
XmtdsTetrisWidget	w;
XEvent			*event;
String			*args;
int			*n_args;
{
   if (TETRIS.slave)
      return ;
   if (TETRIS.pause)
      TETRIS.pause = FALSE;
   else
      TETRIS.pause = TRUE;
   if (TETRIS.pause)
   {
      if (TETRIS.xt_interval_id)
	 XtRemoveTimeOut(TETRIS.xt_interval_id);
   }
   else
   {
      TETRIS.xt_interval_id =
	 XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)w),
			 TETRIS.interval,
			 Timer,
			 w);
   }
}

static void		StartAction(w,event,args,n_args)
XmtdsTetrisWidget	w;
XEvent			*event;
String			*args;
int			*n_args;
{
   XmtdsTetrisClear(w);
   XmtdsTetrisStart(w);
}

static void		GrExpAction(w,event,args,n_args)
XmtdsTetrisWidget	w;
XGraphicsExposeEvent	*event;
String			*args;
int			*n_args;
{
   if (event->type != GraphicsExpose)
      return ;
   refresh_video(w,
		 event->x/TETRIS.block_width,
		 event->y/TETRIS.block_height,
		 event->width/TETRIS.block_width+2,
		 event->height/TETRIS.block_height+2);  
}

static void		draw_block(w,x,y,color)
XmtdsTetrisWidget	w;
int			x;
int			y;
int			color;
{
   GC			gc;
   
   if (W_DEPTH(w) == 1)
      gc = TETRIS.gray_gc;
   else
      gc = TETRIS.gc_list[color];
   XFillRectangle(XtDisplay(w),
		  XtWindow(w),
		  gc,
		  x*TETRIS.block_width,
		  y*TETRIS.block_height,
		  TETRIS.block_width,
		  TETRIS.block_height);
   XDrawLine(XtDisplay(w),
	     XtWindow(w),
	     TETRIS.white_gc,
	     x*TETRIS.block_width,
	     y*TETRIS.block_height,
	     (x+1)*TETRIS.block_width-1,
	     y*TETRIS.block_height);
   XDrawLine(XtDisplay(w),
	     XtWindow(w),
	     TETRIS.white_gc,
	     x*TETRIS.block_width,
	     y*TETRIS.block_height,
	     x*TETRIS.block_width,
	     (y+1)*TETRIS.block_height-1);
   XDrawLine(XtDisplay(w),
	     XtWindow(w),
	     TETRIS.black_gc,
	     (x+1)*TETRIS.block_width-1,
	     y*TETRIS.block_height,
	     (x+1)*TETRIS.block_width-1,
	     (y+1)*TETRIS.block_height-1);
   XDrawLine(XtDisplay(w),
	     XtWindow(w),
	     TETRIS.black_gc,
	     x*TETRIS.block_width,
	     (y+1)*TETRIS.block_height-1,
	     (x+1)*TETRIS.block_width-1,
	     (y+1)*TETRIS.block_height-1);
}

static void		erase_block(w,x,y)
XmtdsTetrisWidget	w;
int			x;
int			y;
{
   XFillRectangle(XtDisplay(w),
		  XtWindow(w),
		  TETRIS.bg_gc,
		  x*TETRIS.block_width,
		  y*TETRIS.block_height,
		  TETRIS.block_width,
		  TETRIS.block_height);
}

static void		update_color_list(w,str)
XmtdsTetrisWidget	w;
char			*str;
{
   char			*ptr;
   char			buf[BUFSIZ];
   int			count;
   XColor		rgb_def;
   XColor		colorcell_def;
   XGCValues		xgcv;
   
   if (W_DEPTH(w) == 1)
   {
      TETRIS.num_colors = 1;
      return ;
   }
   destroy_color_list(w);
   count = 0;
   ptr = str;
   while (sscanf(ptr,"%s",buf) > 0)
   {
      ptr += strlen(buf)+1;
      count++;
      if (count == MAX_COLORS)
      {
	 XtWarning("too many colors in colorList");
	 break ;
      }
   }
   MALLOC(TETRIS.pixel_list,count*sizeof (Pixel),"XmtdsTetrisWidget");
   MALLOC(TETRIS.gc_list,count*sizeof (GC),"XmtdsTetrisWidget");
   TETRIS.num_colors = count;
   count = 0;
   ptr = str;
   while (sscanf(ptr,"%s",buf) > 0)
   {
      if (XAllocNamedColor(XtDisplay(w),
			   W_CMAP(w),
			   buf,
			   &colorcell_def,
			   &rgb_def) == 0)
      {
	 XtWarning("can't find color in colorList");
	 exit(-1);
      }
      TETRIS.pixel_list[count] = colorcell_def.pixel;
      xgcv.foreground = TETRIS.pixel_list[count];
      TETRIS.gc_list[count] = XtGetGC(w,
					       GCForeground,
					       &xgcv);
      ptr += strlen(buf)+1;
      count++;
      if (count == MAX_COLORS)
      	 break ;
   }
}

static void		destroy_color_list(w)
XmtdsTetrisWidget	w;
{
   int			i;
   
   if (TETRIS.pixel_list == NULL)
      return ;
   XFreeColors(XtDisplay(w),
	       W_CMAP(w),
	       TETRIS.pixel_list,
	       TETRIS.num_colors,
	       W_DEPTH(w));
   free(TETRIS.pixel_list);
   i = 0;
   while (i < TETRIS.num_colors)
      XtReleaseGC(w,TETRIS.gc_list[i++]);
}

static void		refresh_video(w,x,y,width,height)
XmtdsTetrisWidget	w;
int			x;
int			y;
int			width;
int			height;
{
   int			i;
   int			j;
   int			dest_i;
   int			dest_j;
   
   dest_i = x+width;
   dest_j = y+height;
   if (dest_i > TETRIS.game_width)
      dest_i = TETRIS.game_width;
   if (dest_j > TETRIS.game_height)
      dest_j = TETRIS.game_height;
   j = y;
   while (j < dest_j) 
   {
      i = x;
      while (i < dest_i)
      {
	 if (VM_STATE(w,i,j) != STATE_NOTHING)
	    draw_block(w,i,j,VM_COLOR(w,i,j));
	 i++;
      }
      j++;
   }
}

static void		test_vm(w)
XmtdsTetrisWidget	w;
{
   int			i;
   int			x;
   int			y;
   int			c;
   
   i = 0;
   while (i < 27)
   {
      x = RAND(TETRIS.game_width-5)+5;
      y = RAND(TETRIS.game_height);
      c = RAND(TETRIS.num_colors);
      VM_STATE(w,x,y) = STATE_FIXED;
      VM_COLOR(w,x,y) = c;
      i++;
   }
}

static void		draw_shape(w,x,y,val,color)
XmtdsTetrisWidget	w;
int			x;
int			y;
int			val;
int			color;
{
   int			i;
   int			j;
   
   j = 0;
   while (j < 4)
   {
      i = 0;
      while (i < 4)
      {
	 if (VAL_TEST(val,i,j))
	 {
	    VM_COLOR(w,x+i,y+j) = color; 
	    VM_STATE(w,x+i,y+j) = STATE_MOVING;
	    draw_block(w,x+i,y+j,color);
	 }
	 i++;
      }
      j++;
   }
}

static int		test_shape(w,x,y,val)
XmtdsTetrisWidget	w;
int			x;
int			y;
int			val;
{
   int			i;
   int			j;
   
   j = 0;
   while (j < 4)
   {
      i = 0;
      while (i < 4)
      {
	 if (VAL_TEST(val,i,j))
	    if (VM_STATE(w,x+i,y+j) == STATE_FIXED)
	       return (-1);
	 i++;
      }
      j++;
   }
   return 0;
}

static void		video_scroll(w,y,n)
XmtdsTetrisWidget	w;
int			y;
int			n;
{
   bcopy(&VM_ATTR(w,0,0),
         &VM_ATTR(w,0,n),
         y*TETRIS.game_width*sizeof (tm_attr_union));
}

static void		grfx_scroll(w,y,n)
XmtdsTetrisWidget	w;
int			y;
int			n;
{
   XCopyArea(XtDisplay(w),
             XtWindow(w),
             XtWindow(w),
             TETRIS.black_gc,
             0,
             0,
	     TETRIS.block_width*TETRIS.game_width,
             y*TETRIS.block_height,
	     0,
             n*TETRIS.block_height);
}

static void		lookup_lines(w)
XmtdsTetrisWidget	w;
{
   int			num_lines;
   int			i;
   int			j;
   Boolean		ok;
   
   num_lines = 0;
   j = 0;
   while (j < TETRIS.game_height)
   {
      ok = TRUE;
      i = 0;
      while (i < TETRIS.game_width)
      {
	 if (VM_STATE(w,i,j) != STATE_FIXED)
	    ok = FALSE;
	 i++;
      }
      if (ok)
      {
	 video_scroll(w,j,1);
	 grfx_scroll(w,j,1);
	 num_lines++;
      }
      j++;
   }
   if (num_lines > 0)
   {
      xmtdsscoreCallbackStruct	cbs;
      
      cbs.reason = SCORE_LINE;
      cbs.detail = num_lines;
      XtCallCallbacks(w,XtNscoreCallback,&cbs);
   }
}

static void		tetris_send(w,proto)
XmtdsTetrisWidget	w;
unsigned char		proto;
{
   if (write(TETRIS.fd,&proto,1) < 0)
   {
      perror("write");
      exit(-1);
   }
}

void			XmtdsTetrisStart(w)
XmtdsTetrisWidget	w;
{
   if (!TETRIS.slave)
   {
      if (TETRIS.xt_interval_id)
	 XtRemoveTimeOut(TETRIS.xt_interval_id);
      TETRIS.xt_interval_id =
	 XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)w),
			 TETRIS.interval,
			 Timer,
			 w);
   }
   TETRIS.start = TRUE;
}

void			XmtdsTetrisDrawShape(w,x,y,shape)
XmtdsTetrisWidget	w;
int			x;
int			y;
int			shape;
{
   int			val;
   
   val = sd_tab[shape][0];
   draw_shape(w,
	      x,
	      y,
	      val,
	      shape%TETRIS.num_colors);
}

void			XmtdsTetrisClear(w)
XmtdsTetrisWidget	w;
{
   XClearWindow(XtDisplay(w),XtWindow(w));
   bzero(TETRIS.video_mem,
	 TETRIS.game_width*TETRIS.game_height*
	 sizeof (tm_attr_union));  
}

void			XmtdsTetrisSet(w,x,y)
XmtdsTetrisWidget	w;
int			x;
int			y;
{
   int			color;
   
   color = RAND(TETRIS.num_colors);
   if ((x < TETRIS.game_width) ||
       (x >= 0) ||
       (y < TETRIS.game_height) ||
       (y >= 0))
   {
      VM_COLOR(w,x,y) = color; 
      VM_STATE(w,x,y) = STATE_FIXED;
      draw_block(w,x,y,color);  
   }
   else
      XtWarning("XmtdsTetrisSet");
}

void			XmtdsTetrisInsertLine(w)
XmtdsTetrisWidget	w;
{
   int			i;
   int			j;
   int			color;
   
   j = 1;
   while (j < TETRIS.game_height)
   {
      i = 0;
      while (i < TETRIS.game_width)
      {
	 if (VM_STATE(w,i,j) == STATE_FIXED)
	 {
	    color = VM_COLOR(w,i,j);
	    erase_block(w,i,j);
	    draw_block(w,i,j-1,color);
	    VM_STATE(w,i,j-1) = STATE_FIXED;
	    VM_COLOR(w,i,j-1) = color;
	    VM_STATE(w,i,j) = STATE_NOTHING;
	 }
	 i++;
      }
      j++;
   }
}

void			XmtdsTetrisSetShape(w,shape)
XmtdsTetrisWidget	w;
int			shape;
{
   TETRIS.shape = shape;
   TETRIS.rotation = 0;
   TETRIS.x = (TETRIS.game_width-4)/2;
   TETRIS.y = 0;
   TETRIS.color =
      TETRIS.shape%TETRIS.num_colors;
}

void			XmtdsTetrisGoDown(w)
XmtdsTetrisWidget	w;
{
   if (TETRIS.start)
   {
      if (!TETRIS.slave)
      {
	 TETRIS.shape = TETRIS.next_shape;
	 TETRIS.next_shape = RAND(num_shapes);
	 TETRIS.rotation = 0;
	 TETRIS.x = (TETRIS.game_width-4)/2;
	 TETRIS.y = 0;
	 TETRIS.color =
	    TETRIS.shape%TETRIS.num_colors;
      }
      if (test_shape(w,
		     TETRIS.x,
		     TETRIS.y,
		     sd_tab[TETRIS.shape][TETRIS.rotation])
	  < 0)
      {
	 xmtdsscoreCallbackStruct	cbs;
	 
	 cbs.reason = SCORE_END;
	 XtCallCallbacks(w,XtNscoreCallback,&cbs);
	 if (!TETRIS.slave)
	    XtRemoveTimeOut(TETRIS.xt_interval_id);
	 return ;
      }
      else
      {
	 xmtdsscoreCallbackStruct	cbs;
	 
	 cbs.reason = SCORE_NORMAL;
	 cbs.detail = TETRIS.next_shape;
	 XtCallCallbacks(w,XtNscoreCallback,&cbs);
      }
      draw_shape(w,
		 TETRIS.x,
		 TETRIS.y,
		 sd_tab[TETRIS.shape][TETRIS.rotation],
		 TETRIS.color);
      if (TETRIS.comm)
	 tetris_send(w,PROTO_SHAPE+TETRIS.shape);
      TETRIS.start = FALSE;
   }
   else
   {
      int		i;
      int		j;
      int		val;
      Boolean		ok;
      
      ok = TRUE;
      val = sd_tab[TETRIS.shape][TETRIS.rotation];
      j = 0;
      while (j < 4)
      {
	 i = 0;
	 while (i < 4)
	 {
	    if (VAL_TEST(val,i,j))
	       if ((TETRIS.y+j+1 == TETRIS.game_height) ||
		   (VM_STATE(w,TETRIS.x+i,TETRIS.y+j+1) ==
		    STATE_FIXED))
	       {
		  ok = FALSE;
		  break ;
	       }
	    i++;
	 }
	 if (ok)
	    j++;
	 else
	    break ;
      }
      if (ok)
      {
	 j = 0;
	 while (j < 4)
	 {
	    i = 0;
	    while (i < 4)
	    {
	       if (VM_STATE(w,
			    TETRIS.x+i,
			    TETRIS.y+j) == STATE_MOVING)
	       {
		  erase_block(w,TETRIS.x+i,TETRIS.y+j);
		  VM_STATE(w,TETRIS.x+i,TETRIS.y+j) =
		     STATE_NOTHING;
	       }
	       i++;
	    }
	    j++;
	 }
	 TETRIS.y++;
	 draw_shape(w,
		    TETRIS.x,
		    TETRIS.y,
		    val,
		    TETRIS.color);
	 if (TETRIS.comm)
	    tetris_send(w,PROTO_GO_DOWN); 
      }
      else
      {
	 j = 0;
	 while (j < 4)
	 {
	    i = 0;
	    while (i < 4)
	    {
	       if (VM_STATE(w,
			    TETRIS.x+i,
			    TETRIS.y+j) == STATE_MOVING)
		  VM_STATE(w,TETRIS.x+i,TETRIS.y+j) =
		     STATE_FIXED;
	       i++;
	    }
	    j++;
	 }
	 lookup_lines(w);
	 TETRIS.start = TRUE;
      }
   }  
}
