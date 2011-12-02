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
 * FILE NAME:	ascii.h
 * CREATED:	Mon Oct 25 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Header file for ascii.c
 *
 */

#ifndef	COMMON_ASCII_H
#define	COMMON_ASCII_H	1

#include <time.h>

extern char *ascii_size(int);	     /* Convert size into K,MBS            */
extern char *ascii_mins(float);      /* Convert seconds to hours:mins:secs */
extern char *ascii_date(time_t);     /* Convert seconds to Day Year        */

#endif
