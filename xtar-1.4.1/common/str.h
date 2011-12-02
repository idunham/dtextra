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
 * FILE NAME:	str.h
 * CREATED:	Wed Oct 27 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Header file for str.c
 *
 */

#ifndef COMMON_STR_H
#define COMMON_STR_H		1

extern int str_numdelims(char *, char);
extern int str_numchars(char *, char);
extern char *str_token(char **, char);
extern char *str_lower(char *);
extern char *str_ending(char *, char *);
extern char *str_casestr(char *, char *);
extern char *str_word(char **);
extern char *str_error(int);

#endif
