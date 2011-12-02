/*
 * Copyright 1993 University of Liverpool Computer Science Department
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of L.U.C.S. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. L.U.C.S. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * FILE NAME:	ascii.c
 * CREATED:	Mon Oct 25 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Converts numbers into a more readable form.
 *
 */

/*
 * Tue Aug  9 12:16:17 1994
 * Added ascii_date() function and header files.
 */

#include <stdio.h>  
#include <time.h>

#include "cdefs.h"

char *ascii_size(int);	      /* Convert size into K,MBS            */
char *ascii_mins(float);      /* Convert seconds to hours:mins:secs */
char *ascii_date(time_t);     /* Convert time in secs to Date Year  */
char *ascii_time(time_t);     /* Convert time similar to ctime()    */

/* ascii_size:*******************************************************/
/* ascii_size: Routine to convert bytes into more readable bytes,   */
/* ascii_size: kilobytes or megabytes.                              */
/* ascii_size:*******************************************************/

char *ascii_size(int size)
{
    static char size_buf[25];

    if(size < 1024)
       sprintf(size_buf, "%.2f K", ((float)size)/1025);
    else if(size < 1048576)
        sprintf(size_buf, "%d K", size/1024);
    else
        sprintf(size_buf, "%.2f MB", ((float)size)/1048576);

    return(size_buf);
}

/* ascii_mins: *******************************************************/
/* ascii_mins: Given the time in seconds put it into a mins/secs     */
/* ascii_mins: format.                                               */
/* ascii_mins: *******************************************************/

char *ascii_mins(float seconds)
{
    static char mins[25];
    int secs = (int) seconds;

    sprintf(mins, "%.2d:%.2d:%.2d", secs/3600, secs/60, secs%60);

    return(mins);
}

/*----------------------------------------------------------------------------
  ascii_date()
  Given the time in seconds puts it into a more readable form - for example,
  May 3rd 1994
  ----------------------------------------------------------------------------*/
char *ascii_date(time_t seconds)
{
    static char retval[26];
    char date[26];

    strcpy(date, ctime(&seconds));
    strip_nl(date);
    date[10] = 0;
    sprintf(retval, "%s %s", &date[4], &date[20]);

    return(retval);
} 

/*----------------------------------------------------------------------------
  ascii_time()
  Given the time in seconds put it into the form, Sat Oct  1 00:50:07 1994.
  
  time_t seconds	The time to convert.
  ----------------------------------------------------------------------------*/
char *ascii_time(time_t seconds)
{
   static char retval[35];
   
   strcpy(retval, ctime(&seconds));
   strip_nl(retval);
   
   return(retval);
}
