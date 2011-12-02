/*
** ElecdispP.h for XmtdsElecdisp widget in .
** 
** Made by MaxTheDogSays (dubray_f@epita.fr && rancur_v@epita.fr
** Login   <vr@epita.fr>
** 
** Started on  Mon May 16 22:12:13 1994 vr
** Last update Mon Nov 21 10:57:12 1994 hydrix
*/

#ifndef _XMTDSELECDISPP_H_
#define _XMTDSELECDISPP_H_      1

#include <X11/CoreP.h>
#include <bit_tab.h>
#include "parse.h"
#include "ElecDisp.h"

typedef struct
{
   int                  empty;
} XmtdsElecDispClassPart;

typedef struct                  _XmtdsElecDispClassRec
{
   CoreClassPart                core_class;
   XmtdsElecDispClassPart       xmtds_elec_disp_class;
} XmtdsElecDispClassRec;

extern XmtdsElecDispClassRec    xmtdsElecDispClassRec;

typedef struct
{
   /* resources */
   Pixel                foreground;
   Pixel                background;
   Pixel                shadow_color;
   String               program;
   int                  interval;
   int                  rast_width;
   int                  rast_height;
   int                  pix_width;
   int                  pix_height;
   /* private state */
   bit_type             *video_mem;
   int                  video_mem_size;
   XtIntervalId         xt_interval_id;
   parse_state          ps;
   GC                   on_gc;
   GC                   off_gc;
   GC                   copy_gc;
   GC                   black_gc;
   XFontStruct          *message_font;
   Boolean              stop;
   Pixmap               bgp;
   Boolean              no_intr;
} XmtdsElecDispPart;

typedef struct  _XmtdsElecDispRec
{
   CorePart             core;
   XmtdsElecDispPart    xmtds_elec_disp;
} XmtdsElecDispRec;

#define CORE                    ((w)->core)
#define NEW_CORE                ((new)->core)
#define REQUEST_CORE            ((request)->core)
#define CURRENT_CORE            ((current)->core)
#define ELECDISP                ((w)->xmtds_elec_disp)
#define NEW_ELECDISP            ((new)->xmtds_elec_disp)
#define REQUEST_ELECDISP        ((request)->xmtds_elec_disp)
#define CURRENT_ELECDISP        ((current)->xmtds_elec_disp)

#define XRECT_BUFFER            1024

#define DEFAULT_MESSAGE_FONT    "fixed"

#endif /* _XMTDSELECDISPP_H_ */
