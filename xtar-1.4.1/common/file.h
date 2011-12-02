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
 * FILE NAME:	file.h
 * CREATED:	Tue Oct 26 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Header file for file.c.
 *
 */

#ifndef	COMMON_FILE_H
#define	COMMON_FILE_H		1

#include <time.h>

extern int file_exists(char *);        /* Does file exist?             */  
extern int file_copy(char *, char *);  /* UNIX cp command              */
extern char *file_load(char *, int *); /* Load whole file into memory  */
extern char *file_uncompress(char *);  /* Uncompress a file            */
extern char *file_stdin();             /* Load stdin to memory         */
extern int file_stamp(char *, time_t); /* Stamp a file with data       */

#endif
