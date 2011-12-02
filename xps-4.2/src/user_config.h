/* $Id: user_config.h,v 1.1 2003/04/24 00:21:29 rockyb Exp $ 

    Copyright (C) 1999-2001,2003 Rocky Bernstein

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

#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include "config.h"

/* Define to the maximimum number of processes you'll allow this
   program to display. The program display algorithm is not linear,
   so as the number of actual processes displayed increases this program
   will slow down and take up more CPU resources. 
*/
#define XPS_MAX_PROCS 1024

/* Put here a userid that you know will not be used on your system. */
#define BOGUS_UID 9999

/* Put here a bogus process id system. Negative process id's dont'
   generally exist, however we still stay way from -1 since if something
   goes wrong this could mean the process group for 1 = init.
*/
#define BOGUS_PID -5

/* There is an internal table which associates userids to colors.
   These size of the table determines how many different userids can
   be assigned in the user color dialog. After this, the last userid
   assigned will be reused.
   
   Note however that no matter what the size of the table, the program
   assigns based on a hash of the userid for those userids not
   assigned in the table.  */

#define NUM_USER_COLORS 10

/* Define the largest value for the refresh rate that's allowed.
   There are implications in the code that need to be dealt with
   like sizes of integers, if you define above 64. 
   This number is multiplied by XPS_TICK to give a value in milliseconds.
*/
#define XPS_MAX_REFRESH_RATE 64

/* Define the smallest value for the refresh rate that's allowed.
   There are implications in the code that need to be dealt with
   if you define below 1. 
   This number is multiplied by XPS_TICK to give a value in milliseconds.
*/
#define XPS_MIN_REFRESH_RATE 1

/* Define the default refresh rate to use when no valid parameters 
   or X resources are specified. Needs to be a string
*/
#define XPS_DEFAULT_REFRESH_RATE "1"

/* The units of millisecond increments that XPS_MIN/MAX_REFRESH_RATE
   above represent. Be careful with this parameter as it can 
   affect CPU performance if too small. And you may have to change
   existing overflow tests too.
*/
#define XPS_TICK 1000

/* Normally, processes like xterm, ftp, or rlogin, list the user
   name, not the process name. If you don't like this behaviour,
   comment out the below.
*/
#define XPS_RECOLOR_LOGIN_PROCESSES 


/* What's the default signal to send when you *first* select "quick kill."
   See sigmap.h for the order of the list. 
   Generally 1 is HUP, and 2 is TERM, and 3 is KILL.
*/
#define XPS_DEFAULT_QUICK_KILL_SIGNAL 1


#if defined(HAVE_XEDITRESCHECKMESSAGES) && defined(HAVE_X11_XMU_EDITRES_H)
#define DO_XEDITRESCHECKMESSAGES 1
#endif

#if defined(NEED_SUID_ROOT) && !defined(SOLARIS_NEW_PROC)
#define RUN_SUID_ROOT 1
#endif

#if defined(HAVE_REGCOMP) && defined(HAVE_REGEXEC)
#define HAVE_REGEX 1
#endif

#endif /*USER_CONFIG_H*/
