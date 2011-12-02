/*$Id: xpstypes.h,v 1.1 2002/12/01 22:00:56 rockyb Exp $

 Typedefs, structures, and common #defines for xps.

 Copyright (C) 1998,1999,2000 Rocky Bernstein

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/* Bits for debugging values */
#ifndef XPSTYPES_H
#define XPSTYPES_H

#include <X11/Intrinsic.h>

#ifdef HAVE_REGEX_H
#include <regex.h>
#endif

/* Debugging bit masks */
#define DBG_CALLBACK  1  /* For callbacks *and* events handling */
#define DBG_DISPLAY   2
#define DBG_MENU      4
#define DBG_VIRTUAL   8
#define DBG_TREESORT 16
#define DBG_DRAW     32

/* 
   There are a couple of Window "shells": a zoomed and 
   normal window. This the below structure contains global variables 
   relevant to each window/shell. A structure is needed because it
   is set via XGetApplicationResources().
 */
typedef
  struct {
    Widget      
      shell,                     /* widget for AppShell */
      scroll,                    /* widget for scrollbar */
      canvas,                    /* widget for drawing area */
      message,                   /* message/status area, line 1 */
      message2,                  /* message/status area, line 2 */
      zoom_button,               /* zoom/unzoom toggle button */
      beep_button,               /* beep on/off toggle button */
      menu_bar;                  /* widget for top menu/button bar */
#ifdef HAVE_KILL
      Widget kill_button;        /* kill on/off toggle button */
      Boolean quick_kill;        /* are we set point and kill? */
#endif
#ifdef XPS_HAVE_KERNEL_PROCS
      Widget      kernel_button; /* Kernel toggle button */
#endif
      Widget      procs_button;  /* Kernel/All/User radio button */
      Widget      user_button;   /* user toggle button */
      Boolean     allprocs;      /* are we showing all processes? */
      Boolean     beep;          /* beep on change? */
      uid_t       uid_displayed; /* user ID graphed (if not allprocs)  */
      char        *user_name;
      Dimension   width, height; /* dimensions of the drawing area in 
  				    Pixel sizes. */
      Pixel       foreground;    /* foreground color of drawing area  */
      XFontStruct *font;         /* font to draw text with            */
      unsigned int fontHeight;   /* height of font in tree. */
#ifdef HAVE_REGEX
    Boolean      match_regexp;   /* Should we match on a regular expression?*/
    regex_t      re_pat;         /* If so, then the compiled pattern */
#endif
  } ShellInfo;

/* typedef an index for accessing into the userColor and uid arrays.  */
typedef unsigned int user_color_index;

/* 
   The the below structure contains general global variables of this
   program. A structure is needed because it is set via
   XGetApplicationResources().
*/
typedef
  struct {
    unsigned long int interval;         /* update interval, in milliseconds  */
    Boolean      visible;               /* Are we visible?                   */
    Boolean      showKernel;            /* Show kernel processes as well?    */
    Boolean      zoomed;                /* Are we in zoomed mode?            */
    int          debugging;             /* Are we in debugging?              */
    Boolean      regexp;                /* Are we using regular expressions? */
    char         *beep_sound_up;        /* If so what to play when more or   */
    char         *beep_sound_down;      /* less processes are added.         */
    char         *ps_cmd_info;          /* ps command to get ps info         */
    char         *ps_cmdline;           /* ps command to command invocation  */
    String       program;               /* name of the program for messages  */
    ShellInfo    info[2];               /* info for normal/zoomed modes      */
    GC           lineStyle;             /* gc for use in drawing lines       */
    GC           nodeStyle;             /* gc for use in drawing node text   */
    Pixel        nodeColor[NPROCSTATES+1]; /* colors for various process 
					      states  */
    uid_t        uid[NUM_USER_COLORS];  /* uid's for various users */
    Pixel        userColor[NUM_USER_COLORS];/* colors for various users */
    Pixel        xterm;                 /* color for xterm process */
    pid_t        selected_pid;          /* pid selected or -5 */
    Pixel        selected_color;        /* color of selected pid */
#ifdef HAVE_KILL
    signal_t     sig;                   /* default signal to send. */
#endif
    XtAppContext app_con;
  } ApplInfo;

extern ApplInfo base;

typedef unsigned int nodeIndex;
#define UNKNOWN_USER "*unknown*"

#define active  base.info[(short int) base.zoomed]

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 50
#endif

#endif /*ifdef #XPSTYPES_H*/
