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
 * FILE NAME:	cdefs.h
 * CREATED:	Wed Oct 27 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Common defs.h file. Contains some #defines that are sometimes
 *              quite useful. Mebee.
 *
 */

#ifndef COMMON_CDEFS_H
#define COMMON_CDEFS_H		1

/* Change case of character */
#define locase(c)		(((c)<='Z')&&((c)>='A')?((c)+32):(c))
#define upcase(c)		(((c)<='z')&&((c)>='a')?((c)-32):(c))

/* Strip last character of string if it matches */
#define	strip_char(string, c)	if(strlen(string) > 0 && \
				string[strlen(string)-1] == c) \
				string[strlen(string)-1] = 0

/* Strip of a new line */
#define strip_nl(string)	strip_char(string, '\n')

/* Number of seconds in a day/week/month */
#define	SECS_DAY	604800
#define	SECS_WEEK	4233600
#define	SECS_MONTH	18748800

#endif
