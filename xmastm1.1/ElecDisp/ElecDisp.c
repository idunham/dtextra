/*
** ElecDisp.c for XmtdsElecDisp widget in .
** 
** Made by MaxTheDogSays (dubray_f@epita.fr && rancur_v@epita.fr
** Login   <vr@epita.fr>
** 
** Started on  Mon May 16 22:24:08 1994 vr
** Last update Mon Nov 21 11:38:21 1994 hydrix
*/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <sysdep.h>
#include <bit_tab.h>
#include <Xstd.h>
#include "parse.h"
#include "ElecDispP.h"

static void                     Initialize();
static void                     SetValues();
static void                     Redisplay();
static void                     Destroy();
static void                     Resize();

static XtTimerCallbackProc      Timer();

static void                     DrawAction();
static void                     AskAction();
static void                     GrExpAction();

static Pixmap                   create_bg_pixmap();

static cmd_def                  cmd_defs[] =
{
{"message",                     XmtdsEDMessageFunc},
{"spray",                       XmtdsEDSprayFunc},
{"put-pixel",                   XmtdsEDPutPixelFunc},
{"stop",                        XmtdsEDStopFunc},
{"scroll-right",                XmtdsEDScrollRightFunc},
{"scroll-left",                 XmtdsEDScrollLeftFunc},
{"scroll-up",                   XmtdsEDScrollUpFunc},
{"scroll-down",                 XmtdsEDScrollDownFunc},
{"set-font",                    XmtdsEDSetFontFunc},
{"clear",                       XmtdsEDClearFunc},
{"speed",                       XmtdsEDSpeedFunc}
};

static char     defaultTranslations[] =
"<Btn1Down>:    draw()\n\
<GrExp>:        grexp()";

static XtActionsRec     actionsList[] =
{
{"draw",        (XtActionProc)DrawAction},
{"ask",         (XtActionProc)AskAction},
{"grexp",       (XtActionProc)GrExpAction}
};

static XtResource       resources[] =
{
{XtNforeground,         XtCForeground,  XtRPixel,       sizeof (Pixel),
    XtOffsetOf(XmtdsElecDispRec,        xmtds_elec_disp.foreground),
    XtRString,          "yellow"},
{XtNbackground,         XtCBackground,  XtRPixel,       sizeof (Pixel),
    XtOffsetOf(XmtdsElecDispRec,        xmtds_elec_disp.background),
    XtRString,          "black"},
{XtNshadowColor,        XtCShadowColor, XtRPixel,       sizeof (Pixel),
    XtOffsetOf(XmtdsElecDispRec,        xmtds_elec_disp.shadow_color),
    XtRString,          "gray50"},
{XtNprogram,            XtCProgram,     XtRString,      sizeof (char *),
    XtOffsetOf(XmtdsElecDispRec,        xmtds_elec_disp.program),
    XtRString,          ""},
{XtNinterval,           XtCInterval,    XtRInt,         sizeof (int),
    XtOffsetOf(XmtdsElecDispRec,        xmtds_elec_disp.interval),
    XtRString,          "2000"},
{XtNrasterWidth,        XtCRasterWidth, XtRInt,         sizeof (int),
    XtOffsetOf(XmtdsElecDispRec,        xmtds_elec_disp.rast_width),
    XtRString,          "100"},
{XtNrasterHeight,       XtCRasterHeight,XtRInt,         sizeof (int),
    XtOffsetOf(XmtdsElecDispRec,        xmtds_elec_disp.rast_height),
    XtRString,          "16"},
{XtNpixWidth,           XtCPixWidth,    XtRInt,         sizeof (int),
    XtOffsetOf(XmtdsElecDispRec,        xmtds_elec_disp.pix_width),
    XtRString,          "2"},
{XtNpixHeight,          XtCPixHeight,XtRInt,            sizeof (int),
    XtOffsetOf(XmtdsElecDispRec,        xmtds_elec_disp.pix_height),
    XtRString,          "2"}
};

XmtdsElecDispClassRec   xmtdsElecDispClassRec = {
{
   /* core */
   (WidgetClass)&widgetClassRec,        /* superclass           */
   "XmtdsElecDisp",                     /* class name           */
   sizeof(XmtdsElecDispRec),            /* size                 */   
   NULL,                                /* class initialize     */   
   NULL,                                /* class_part_init      */   
   FALSE,                               /* class_inited         */   
   Initialize,                          /* initialize           */   
   NULL,                                /* initialize_hook      */   
   XtInheritRealize,                    /* realize              */   
   actionsList,                         /* actions              */   
   XtNumber(actionsList),               /* num_actions          */   
   resources,                           /* resources            */   
   XtNumber(resources),                 /* resource_count       */   
   NULLQUARK,                           /* xrm_class            */   
   TRUE,                                /* compress_motion      */   
   TRUE,                                /* compress_exposure    */   
   TRUE,                                /* compress_enterleave  */   
   FALSE,                               /* visible_interest     */   
   Destroy,                             /* destroy              */   
   Resize,                              /* resize               */   
   Redisplay,                           /* expose               */   
   NULL,                                /* set_values           */   
   NULL,                                /* set_values_hook      */   
   XtInheritSetValuesAlmost,            /* set_values_almost    */   
   NULL,                                /* get_values_hook      */   
   NULL,                                /* accept_focus         */   
   XtVersion,                           /* version              */   
   NULL,                                /* callback_private     */   
   defaultTranslations,                 /* tm_table             */   
   XtInheritQueryGeometry,              /* query_geometry       */   
   XtInheritDisplayAccelerator,         /* display_accelerator  */   
   NULL,                                /* extension            */   
},
/* xmtds_elec_disp */
{
   0,                                   /* empty                */
}
};

WidgetClass     xmtdsElecDispWidgetClass = (WidgetClass)&xmtdsElecDispClassRec;

static Pixmap           create_bg_pixmap(w)
XmtdsElecDispWidget     w;
{
   Pixmap               pix;
   
   pix = 
      XCreatePixmap(XtDisplay(w),
                    W_ROOT(w),
                    ELECDISP.pix_width*2,
                    ELECDISP.pix_height*2,
                    W_DEPTH(w));
   XFillRectangle(XtDisplay(w),
                  pix,
                  ELECDISP.black_gc,
                  0,
                  0,
                  ELECDISP.pix_width*2,
                  ELECDISP.pix_height*2);
   XFillRectangle(XtDisplay(w),
                  pix,
                  ELECDISP.off_gc,
                  ELECDISP.pix_width,
                  ELECDISP.pix_height,
                  ELECDISP.pix_width,
                  ELECDISP.pix_height);
   return pix;
}

static void             Initialize(request,new,args,num_args)
XmtdsElecDispWidget     request;
XmtdsElecDispWidget     new;
ArgList                 args;
Cardinal                *num_args;
{
   XGCValues            xgcv;
   
   xgcv.foreground = NEW_ELECDISP.foreground;
   NEW_ELECDISP.on_gc =
      XtGetGC(new,
              GCForeground,
              &xgcv);
   xgcv.foreground = NEW_ELECDISP.shadow_color;
   NEW_ELECDISP.off_gc =
      XtGetGC(new,
              GCForeground,
              &xgcv);
   xgcv.function = GXcopy;
   xgcv.foreground = W_BPX(new);
   NEW_ELECDISP.black_gc =
      XtGetGC(new,
              GCForeground,
              &xgcv);
   xgcv.function = GXcopy;
   NEW_ELECDISP.copy_gc =
      XtGetGC(new,
              GCFunction,
              &xgcv);
   NEW_ELECDISP.bgp = create_bg_pixmap(new);
   NEW_CORE.background_pixmap = NEW_ELECDISP.bgp;
   NEW_ELECDISP.xt_interval_id =
      XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)new),
                      NEW_ELECDISP.interval,
                      Timer,
                      new);
   bit_tab_create(&(NEW_ELECDISP.video_mem),
                  NEW_ELECDISP.rast_width,
                  NEW_ELECDISP.rast_height,
                  &(NEW_ELECDISP.video_mem_size));
   bzero(NEW_ELECDISP.video_mem,
         NEW_ELECDISP.video_mem_size);
   NEW_CORE.width =
      NEW_ELECDISP.rast_width*(NEW_ELECDISP.pix_width*2);
   NEW_CORE.height =
      NEW_ELECDISP.rast_height*(NEW_ELECDISP.pix_height*2);
   parse_compile(&(NEW_ELECDISP.ps),
                 cmd_defs,
                 XtNumber(cmd_defs),
                 NEW_ELECDISP.program,
                 strlen(NEW_ELECDISP.program),
                 new);
   parse_rewind(&(NEW_ELECDISP.ps));
   if ((NEW_ELECDISP.message_font = 
        XLoadQueryFont(XtDisplay(new),DEFAULT_MESSAGE_FONT)) == NULL)
   {
      XtWarning("Unknown font");
      exit(-1);
   }
   NEW_ELECDISP.stop = FALSE;
   NEW_ELECDISP.no_intr = FALSE;
}

static void             SetValues(current,request,new,args,num_args)
XmtdsElecDispWidget     current;
XmtdsElecDispWidget     request;
XmtdsElecDispWidget     new;
ArgList                 args;
Cardinal                *num_args;
{
   if (REQUEST_ELECDISP.program != CURRENT_ELECDISP.program)
   {
      parse_destroy(&(NEW_ELECDISP.ps)); 
      free(CURRENT_ELECDISP.program);
      parse_compile(&(NEW_ELECDISP.ps),
                    cmd_defs,
                    XtNumber(cmd_defs),
                    NEW_ELECDISP.program,
                    strlen(NEW_ELECDISP.program),
                    new);
      parse_rewind(&(NEW_ELECDISP.ps));  
   }
   if ((REQUEST_ELECDISP.rast_width !=
        CURRENT_ELECDISP.rast_width) ||
       (REQUEST_ELECDISP.rast_height !=
        CURRENT_ELECDISP.rast_height))
   {
      free(CURRENT_ELECDISP.video_mem);
      bit_tab_create(&(NEW_ELECDISP.video_mem),
                     NEW_ELECDISP.rast_width,
                     NEW_ELECDISP.rast_height,
                     &(NEW_ELECDISP.video_mem_size));
      bzero(NEW_ELECDISP.video_mem,
            NEW_ELECDISP.video_mem_size);
   }
   if ((REQUEST_ELECDISP.pix_width !=
        CURRENT_ELECDISP.pix_width) ||
       (REQUEST_ELECDISP.pix_height !=
        CURRENT_ELECDISP.pix_height))
   {
      XFreePixmap(XtDisplay(new),NEW_ELECDISP.bgp);
      NEW_ELECDISP.bgp = create_bg_pixmap(new);
      NEW_CORE.background_pixmap = NEW_ELECDISP.bgp;
   }
}

void                    refresh_raster(w,x,y,width,height)
XmtdsElecDispWidget     w;
int                     x;
int                     y;
int                     width;
int                     height;
{
   XRectangle           on_xrects[XRECT_BUFFER];
   XRectangle           off_xrects[XRECT_BUFFER];
   int                  num_on_xrects;
   int                  num_off_xrects;
   int                  i;
   int                  dest_i;
   int                  j;
   int                  dest_j;
   
   num_on_xrects = 0;
   num_off_xrects = 0;
   j = y;
   dest_i = x+width;
   dest_j = y+height;
   if (dest_i > ELECDISP.rast_width)
      dest_i = ELECDISP.rast_width;
   if (dest_j > ELECDISP.rast_height)
      dest_j = ELECDISP.rast_height;
   while (j < dest_j)
   {
      i = x;
      while (i < dest_i)
      {
         if (BIT_TEST(ELECDISP.video_mem,
                      i,
                      j,
                      ELECDISP.rast_width))
         {
            if (num_on_xrects == XRECT_BUFFER)
            {
               XFillRectangles(XtDisplay(w),
                               XtWindow(w),
                               ELECDISP.on_gc,
                               on_xrects,
                               num_on_xrects);
               num_on_xrects = 0;
            }
            on_xrects[num_on_xrects].x =
               i*(ELECDISP.pix_width*2)+ELECDISP.pix_width;
            on_xrects[num_on_xrects].y =
               j*(ELECDISP.pix_height*2)+ELECDISP.pix_height;
            on_xrects[num_on_xrects].width = ELECDISP.pix_width;
            on_xrects[num_on_xrects].height = ELECDISP.pix_height;
            num_on_xrects++;
         }
         else
         {
            if (num_off_xrects == XRECT_BUFFER)
            {
               XFillRectangles(XtDisplay(w),
                               XtWindow(w),
                               ELECDISP.off_gc,
                               off_xrects,
                               num_off_xrects);
               num_off_xrects = 0;
            }
            off_xrects[num_off_xrects].x =
               i*(ELECDISP.pix_width*2)+ELECDISP.pix_width;
            off_xrects[num_off_xrects].y =
               j*(ELECDISP.pix_height*2)+ELECDISP.pix_height;
            off_xrects[num_off_xrects].width = ELECDISP.pix_width;
            off_xrects[num_off_xrects].height = ELECDISP.pix_height;
            num_off_xrects++;
         }
         i++;
      }
      j++;
   }
   XFillRectangles(XtDisplay(w),
                   XtWindow(w),
                   ELECDISP.on_gc,
                   on_xrects,
                   num_on_xrects);
   XFillRectangles(XtDisplay(w),
                   XtWindow(w),
                   ELECDISP.off_gc,
                   off_xrects,
                   num_off_xrects);
}

static void             Redisplay(w,event,region)
XmtdsElecDispWidget     w;
XExposeEvent            *event;
Region                  region;
{
   refresh_raster(w,
                  event->x/(ELECDISP.pix_width*2),
                  event->y/(ELECDISP.pix_height*2),
                  event->width/(ELECDISP.pix_width*2)+2,
                  event->height/(ELECDISP.pix_height*2)+2);
}

static void             Destroy(w)
XmtdsElecDispWidget     w;
{
   XtReleaseGC(w,ELECDISP.on_gc);
   XtReleaseGC(w,ELECDISP.off_gc);
   XtReleaseGC(w,ELECDISP.copy_gc);
   XtReleaseGC(w,ELECDISP.black_gc);
   XtRemoveTimeOut(ELECDISP.xt_interval_id);
   XFreePixmap(XtDisplay(w),ELECDISP.bgp);
   XFreeFont(XtDisplay(w),ELECDISP.message_font);
   free(ELECDISP.video_mem);
   parse_destroy(&(ELECDISP.ps));  
}

static void             Resize(w)
XmtdsElecDispWidget     w;
{
   ELECDISP.rast_width = CORE.width/(ELECDISP.pix_width*2);
   ELECDISP.rast_height = CORE.height/(ELECDISP.pix_height*2);
   free(ELECDISP.video_mem);
   bit_tab_create(&(ELECDISP.video_mem),
                  ELECDISP.rast_width,
                  ELECDISP.rast_height,
                  &(ELECDISP.video_mem_size));
   bzero(ELECDISP.video_mem,
         ELECDISP.video_mem_size);
}

static XtTimerCallbackProc      Timer(w,id)
XmtdsElecDispWidget             w;
XtIntervalId                    *id;
{
   if (ELECDISP.no_intr)
      return ;
   if (ELECDISP.stop)
   {
      XtRemoveTimeOut(ELECDISP.xt_interval_id);
      return ;
   }
   if (parse_next(&(ELECDISP.ps)) == PARSE_EOP)
   {
      parse_rewind(&(ELECDISP.ps));
      parse_next(&(ELECDISP.ps));
   }
   ELECDISP.xt_interval_id =
      XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)w),
                      ELECDISP.interval,
                      Timer,
                      w);  
}

static void             DrawAction(w,event,args,n_args)
XmtdsElecDispWidget     w;
XButtonEvent            *event;
String                  *args;
int                     *n_args;
{
   int                  x;
   int                  y;
   
   x = event->x/(ELECDISP.pix_width*2);
   y = event->y/(ELECDISP.pix_height*2);
   if ((x > ELECDISP.rast_width) ||
       (y > ELECDISP.rast_height))
      return ;
   if (event->type != ButtonPress)
      return ;
   BIT_SET(ELECDISP.video_mem,
           x,
           y,
           ELECDISP.rast_width);
   XFillRectangle(XtDisplay(w),
                  XtWindow(w),
                  ELECDISP.on_gc,
                  x*(ELECDISP.pix_width*2)+
                  ELECDISP.pix_width,
                  y*(ELECDISP.pix_height*2)+
                  ELECDISP.pix_height,
                  ELECDISP.pix_width,
                  ELECDISP.pix_height);
}

static void             AskAction(w,event,args,n_args)
XmtdsElecDispWidget     w;
XButtonEvent            *event;
String                  *args;
int                     *n_args;
{
   int                  x;
   int                  y;
   
   x = event->x/(ELECDISP.pix_width*2);
   y = event->y/(ELECDISP.pix_height*2);
   if ((x > ELECDISP.rast_width) ||
       (y > ELECDISP.rast_height))
      return ;
   if (event->type != ButtonPress)
      return ;
   printf("x = %d y = %d offset: %d mask: %d bit = %d\n",
          x,
          y,
          BIT_OFFSET(x,
                     y,
                     ELECDISP.rast_width),
          BIT_MASK(x),
          BIT_TEST(ELECDISP.video_mem,
                   x,
                   y,
                   ELECDISP.rast_width));
}

static void             GrExpAction(w,event,args,n_args)
XmtdsElecDispWidget     w;
XGraphicsExposeEvent    *event;
String                  *args;
int                     *n_args;
{
   if (event->type != GraphicsExpose)
      return ;
   ELECDISP.no_intr = TRUE;
   refresh_raster(w,
                  event->x/(ELECDISP.pix_width*2),
                  event->y/(ELECDISP.pix_height*2),
                  event->width/(ELECDISP.pix_width*2)+2,
                  event->height/(ELECDISP.pix_height*2)+2);
   ELECDISP.no_intr = FALSE;
}

void                    XmtdsEDMessageFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   int                  x;
   int                  y;
   int                  width;
   int                  height;
   int                  len;
   GC                   gc;
   XGCValues            xgcv;
   XImage               *image;
   int                  i;
   int                  j;
   XRectangle           on_xrects[XRECT_BUFFER];
   XRectangle           off_xrects[XRECT_BUFFER];
   int                  num_on_xrects;
   int                  num_off_xrects;
   Pixmap               pix;
   
   num_on_xrects = 0;
   num_off_xrects = 0;
   if (num_args != 3)
   {
      XtWarning("syntax: message(x,y,str)");
      exit(-1);
   }
   x = atoi(args[0]);
   y = atoi(args[1]);
   len = strlen(args[2]);
   width = XTextWidth(ELECDISP.message_font,args[2],len);
   height = XFS_HEIGHT(ELECDISP.message_font);
   if ((x+width) > ELECDISP.rast_width)
      width = ELECDISP.rast_width-x;
   if ((y+height) > ELECDISP.rast_height)
      height = ELECDISP.rast_height-y;
   pix = XCreatePixmap(XtDisplay(w),
                       W_ROOT(w),
                       width,
                       height,
                       1);
   xgcv.foreground = W_WPX(w);
   xgcv.font = XFS_FID(ELECDISP.message_font);
   gc = XCreateGC(XtDisplay(w),
                  pix,
                  GCForeground|GCFont,
                  &xgcv);
   XFillRectangle(XtDisplay(w),
                  pix,
                  gc,
                  0,
                  0,
                  width,
                  height);
   XSetForeground(XtDisplay(w),
                  gc,
                  W_BPX(w));
   XDrawString(XtDisplay(w),
               pix,
               gc,
               0,
               XFS_HEIGHT(ELECDISP.message_font)-
               XFS_DESCENT(ELECDISP.message_font),
               args[2],
               len);
   image = XGetImage(XtDisplay(w),
                     pix,
                     0,
                     0,
                     width,
                     height,
                     -1,
                     ZPixmap);
   j = 0;
   while (j < height)
   {
      i = 0;
      while (i < width)
      {
         if (XGetPixel(image,i,j) == W_BPX(w))
         {
            BIT_SET(ELECDISP.video_mem,
                    x+i,
                    y+j,
                    ELECDISP.rast_width);
            if (num_on_xrects == XRECT_BUFFER)
            {
               XFillRectangles(XtDisplay(w),
                               XtWindow(w),
                               ELECDISP.on_gc,
                               on_xrects,
                               num_on_xrects);
               num_on_xrects = 0;
            }
            on_xrects[num_on_xrects].x =
               (x+i)*(ELECDISP.pix_width*2)+
                  ELECDISP.pix_width;
            on_xrects[num_on_xrects].y =
               (y+j)*(ELECDISP.pix_height*2)+
                  ELECDISP.pix_height;
            on_xrects[num_on_xrects].width = ELECDISP.pix_width;
            on_xrects[num_on_xrects].height = ELECDISP.pix_height;
            num_on_xrects++;
         }
         else
         {
            BIT_UNSET(ELECDISP.video_mem,
                      x+i,
                      y+j,
                      ELECDISP.rast_width);
            if (num_off_xrects == XRECT_BUFFER)
            {
               XFillRectangles(XtDisplay(w),
                               XtWindow(w),
                               ELECDISP.off_gc,
                               off_xrects,
                               num_off_xrects);
               num_off_xrects = 0;
            }
            off_xrects[num_off_xrects].x =
               (x+i)*(ELECDISP.pix_width*2)+
                  ELECDISP.pix_width;
            off_xrects[num_off_xrects].y =
               (y+j)*(ELECDISP.pix_height*2)+
                  ELECDISP.pix_height;
            off_xrects[num_off_xrects].width = ELECDISP.pix_width;
            off_xrects[num_off_xrects].height = ELECDISP.pix_height;
            num_off_xrects++;
         }
         i++;
      }
      j++;
   }
   XFillRectangles(XtDisplay(w),
                   XtWindow(w),
                   ELECDISP.on_gc,
                   on_xrects,
                   num_on_xrects);
   XFillRectangles(XtDisplay(w),
                   XtWindow(w),
                   ELECDISP.off_gc,
                   off_xrects,
                   num_off_xrects);
   XFreeGC(XtDisplay(w),gc);
   XFreePixmap(XtDisplay(w),pix);
   XDestroyImage(image);
}

static int              myrand(n)
int                     n;
{
   return (rand()%n);
}

static void             spray(w,xr,num_xr)
XmtdsElecDispWidget     w;
XRectangle              *xr;
int                     num_xr;
{
   Boolean              spray[XRECT_BUFFER];
   int                  i;
   int                  j;
   int                  ran;
   int                  found;
   
   i = 0;
   while (i < XRECT_BUFFER)
      spray[i++] = 0;
   i = 0;
   while (i < num_xr)
   {
      ran = myrand(XRECT_BUFFER);
      found = 0;
      j = ran;
      while (j < num_xr)
      {
         if (!spray[j])
         {
            ran = j;
            spray[ran] = 1;
            XFillRectangle(XtDisplay(w),
                           XtWindow(w),
                           ELECDISP.on_gc,
                           xr[ran].x,
                           xr[ran].y,
                           xr[ran].width,
                           xr[ran].height);
            XFlush(XtDisplay(w));
            found = 1;
            break ;
         }
         j++;
      }
      if (found)
         i++;
   }
}

void                    XmtdsEDSprayFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   int                  x;
   int                  y;
   int                  width;
   int                  height;
   int                  len;
   GC                   gc;
   XGCValues            xgcv;
   XImage               *image;
   int                  i;
   int                  j;
   XRectangle           on_xrects[XRECT_BUFFER];
   XRectangle           off_xrects[XRECT_BUFFER];
   int                  num_on_xrects;
   int                  num_off_xrects;
   Pixmap               pix;
   
   num_on_xrects = 0;
   num_off_xrects = 0;
   if (num_args != 3)
   {
      XtWarning("syntax: message(x,y,str)");
      exit(-1);
   }
   x = atoi(args[0]);
   y = atoi(args[1]);
   len = strlen(args[2]);
   width = XTextWidth(ELECDISP.message_font,args[2],len);
   height = XFS_HEIGHT(ELECDISP.message_font);
   if ((x+width) > ELECDISP.rast_width)
      width = ELECDISP.rast_width-x;
   if ((y+height) > ELECDISP.rast_height)
      height = ELECDISP.rast_height-y;
   pix = XCreatePixmap(XtDisplay(w),
                       W_ROOT(w),
                       width,
                       height,
                       1);
   xgcv.foreground = W_WPX(w);
   xgcv.font = XFS_FID(ELECDISP.message_font);
   gc = XCreateGC(XtDisplay(w),
                  pix,
                  GCForeground|GCFont,
                  &xgcv);
   XFillRectangle(XtDisplay(w),
                  pix,
                  gc,
                  0,
                  0,
                  width,
                  height);
   XSetForeground(XtDisplay(w),
                  gc,
                  W_BPX(w));
   XDrawString(XtDisplay(w),
               pix,
               gc,
               0,
               XFS_HEIGHT(ELECDISP.message_font)-
               XFS_DESCENT(ELECDISP.message_font),
               args[2],
               len);
   image = XGetImage(XtDisplay(w),
                     pix,
                     0,
                     0,
                     width,
                     height,
                     -1,
                     ZPixmap);
   j = 0;
   while (j < height)
   {
      i = 0;
      while (i < width)
      {
         if (XGetPixel(image,i,j) == W_BPX(w))
         {
            BIT_SET(ELECDISP.video_mem,
                    x+i,
                    y+j,
                    ELECDISP.rast_width);
            if (num_on_xrects == XRECT_BUFFER)
            {
               spray(w,on_xrects,num_on_xrects);
               num_on_xrects = 0;
            }
            on_xrects[num_on_xrects].x =
               (x+i)*(ELECDISP.pix_width*2)+
                  ELECDISP.pix_width;
            on_xrects[num_on_xrects].y =
               (y+j)*(ELECDISP.pix_height*2)+
                  ELECDISP.pix_height;
            on_xrects[num_on_xrects].width = ELECDISP.pix_width;
            on_xrects[num_on_xrects].height = ELECDISP.pix_height;
            num_on_xrects++;
         }
         else
         {
            BIT_UNSET(ELECDISP.video_mem,
                      x+i,
                      y+j,
                      ELECDISP.rast_width);
            if (num_off_xrects == XRECT_BUFFER)
            {
               XFillRectangles(XtDisplay(w),
                               XtWindow(w),
                               ELECDISP.off_gc,
                               off_xrects,
                               num_off_xrects);
               num_off_xrects = 0;
            }
            off_xrects[num_off_xrects].x =
               (x+i)*(ELECDISP.pix_width*2)+
                  ELECDISP.pix_width;
            off_xrects[num_off_xrects].y =
               (y+j)*(ELECDISP.pix_height*2)+
                  ELECDISP.pix_height;
            off_xrects[num_off_xrects].width = ELECDISP.pix_width;
            off_xrects[num_off_xrects].height = ELECDISP.pix_height;
            num_off_xrects++;
         }
         i++;
      }
      j++;
   }
   spray(w,on_xrects,num_on_xrects);
   XFillRectangles(XtDisplay(w),
                   XtWindow(w),
                   ELECDISP.off_gc,
                   off_xrects,
                   num_off_xrects);
   XFreeGC(XtDisplay(w),gc);
   XFreePixmap(XtDisplay(w),pix);
   XDestroyImage(image);
}

void                    XmtdsEDPutPixelFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   int                  x;
   int                  y;
   int                  lit;
   GC                   gc;
   
   if (num_args != 3)
   {
      XtWarning("syntax: put-pixel(x,y,0/1)");
      exit(-1);
   }
   x = atoi(args[0]);
   y = atoi(args[1]);
   lit = atoi(args[2]);
   if (lit)
   {
      BIT_SET(ELECDISP.video_mem,
              x,
              y,
              ELECDISP.rast_width);
      gc = ELECDISP.on_gc;
   }
   else
   {
      BIT_UNSET(ELECDISP.video_mem,
                x,
                y,
                ELECDISP.rast_width);
      gc = ELECDISP.off_gc;  
   }
   XFillRectangle(XtDisplay(w),
                  XtWindow(w),
                  gc,
                  x*(ELECDISP.pix_width*2)+
                  ELECDISP.pix_width,
                  y*(ELECDISP.pix_height*2)+
                  ELECDISP.pix_height,
                  ELECDISP.pix_width,
                  ELECDISP.pix_height);
}

void                    XmtdsEDStopFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   ELECDISP.stop = TRUE;
}

void                    XmtdsEDRestartFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   ELECDISP.stop = FALSE;
   ELECDISP.xt_interval_id =
      XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)w),
                      ELECDISP.interval,
                      Timer,
                      w);
}

void                    XmtdsEDScrollRightFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   XCopyArea(XtDisplay(w),
             XtWindow(w),
             XtWindow(w),
             ELECDISP.copy_gc,
             0,
             0,
             ELECDISP.rast_width*ELECDISP.pix_width*2,
             ELECDISP.rast_height*ELECDISP.pix_height*2,
             (ELECDISP.pix_width*2),
             0);
   bit_tab_scroll_right(ELECDISP.video_mem,
                        ELECDISP.rast_width,
                        ELECDISP.rast_height);
}

void                    XmtdsEDScrollLeftFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   XCopyArea(XtDisplay(w),
             XtWindow(w),
             XtWindow(w),
             ELECDISP.copy_gc,
             (ELECDISP.pix_width*2),
             0,
             ELECDISP.rast_width*ELECDISP.pix_width*2,
             ELECDISP.rast_height*ELECDISP.pix_height*2,
             0,
             0);
   bit_tab_scroll_left(ELECDISP.video_mem,
                       ELECDISP.rast_width,
                       ELECDISP.rast_height);
}

void                    XmtdsEDScrollUpFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   XCopyArea(XtDisplay(w),
             XtWindow(w),
             XtWindow(w),
             ELECDISP.copy_gc,
             0,
             (ELECDISP.pix_height*2),
             ELECDISP.rast_width*ELECDISP.pix_width*2,
             ELECDISP.rast_height*ELECDISP.pix_height*2,
             0,
             0);
   bit_tab_scroll_up(ELECDISP.video_mem,
                     ELECDISP.rast_width,
                     ELECDISP.rast_height);
}

void                    XmtdsEDScrollDownFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   XCopyArea(XtDisplay(w),
             XtWindow(w),
             XtWindow(w),
             ELECDISP.copy_gc,
             0,
             0,
             ELECDISP.rast_width*ELECDISP.pix_width*2,
             ELECDISP.rast_height*ELECDISP.pix_height*2,
             0,
             ELECDISP.pix_height*2);
   bit_tab_scroll_down(ELECDISP.video_mem,
                       ELECDISP.rast_width,
                       ELECDISP.rast_height);
}

void                    XmtdsEDSetFontFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   if (num_args != 1)
   {
      XtWarning("syntax: set-font(font name)");
      exit(-1);
   }
   XFreeFont(XtDisplay(w),
             ELECDISP.message_font);
   if ((ELECDISP.message_font = 
        XLoadQueryFont(XtDisplay(w),args[0])) == NULL)
   {
      XtWarning("Unknown font");
      exit(-1);
   }  
}

void                    XmtdsEDClearFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   XRectangle           off_xrects[XRECT_BUFFER];
   int                  num_off_xrects;
   int                  i;
   int                  j;
   
   num_off_xrects = 0;
   j = 0;
   while (j < ELECDISP.rast_height)
   {
      i = 0;
      while (i < ELECDISP.rast_width)
      {
         if (BIT_TEST(ELECDISP.video_mem,
                      i,
                      j,
                      ELECDISP.rast_width))
         {
            if (num_off_xrects == XRECT_BUFFER)
            {
               XFillRectangles(XtDisplay(w),
                               XtWindow(w),
                               ELECDISP.off_gc,
                               off_xrects,
                               num_off_xrects);
               num_off_xrects = 0;
            }
            off_xrects[num_off_xrects].x =
               i*(ELECDISP.pix_width*2)+ELECDISP.pix_width;
            off_xrects[num_off_xrects].y =
               j*(ELECDISP.pix_height*2)+ELECDISP.pix_height;
            off_xrects[num_off_xrects].width = ELECDISP.pix_width;
            off_xrects[num_off_xrects].height = ELECDISP.pix_height;
            num_off_xrects++;
         }
         i++;
      }
      j++;
   }
   XFillRectangles(XtDisplay(w),
                   XtWindow(w),
                   ELECDISP.off_gc,
                   off_xrects,
                   num_off_xrects);
   bzero(ELECDISP.video_mem,
         ELECDISP.video_mem_size);  
}

void                    XmtdsEDSpeedFunc(w,args,num_args)
XmtdsElecDispWidget     w;
char                    **args;
int                     num_args;
{
   int                  time;
   
   if (num_args != 1)
   {
      XtWarning("syntax: speed(time)");
      exit(-1);
   }
   time = atoi(args[0]);
   ELECDISP.interval = time;  
}
