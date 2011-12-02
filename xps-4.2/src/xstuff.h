/* X Windows-ish definitions for xps 
   $Id: xstuff.h,v 1.1 2002/12/01 22:00:57 rockyb Exp $
   Copyright (C) 1999 Rocky Bernstein
   
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
#ifndef XSTUFF_H
#define XSTUFF_H

#include <X11/Intrinsic.h>

#include "xpstypes.h"

extern char  nodename[];       /* name of the local system */
extern pid_t MyPid;            /* my process id */

#ifdef RUN_SUID_ROOT
extern uid_t real_uid;        /* who originally was I */
#endif

/* Set the visible name of a widget (i.e of a button or status line) */
extern void set_label (char *name, Widget w);

/**************************************************************************
Forces a recalculation of everything and update of the display.
Useful when we know something has changed.
***************************************************************************/
extern void ForceRedisplay(void);

#endif /*ifndef XSTUFF_H*/
