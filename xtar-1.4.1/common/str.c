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
 * FILE NAME:	str.c
 * CREATED:	Wed Oct 27 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Some extra string handling routines not in the clib.
 *
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "cdefs.h"

int str_numchars(char *, char);
int str_numdelims(char *, char);
char *str_token(char **, char);
char *str_lower(char *);
char *str_ending(char *, char *);
char *str_casestr(char *, char *);
int str_sub(char *, char, char);
char *str_word(char **);
char *str_error(int);

/* str_numchars:******************************************************/
/* str_numchars: Counts how many times the given character occurs in */
/* str_numchars: a string.                                           */
/* str_numchars:******************************************************/

int str_numchars(char *string, char c)
{
    int i = 0, count = 0;

    while(string[i])
        if(string[i++] == c)
            count++;

    return(count);
}

/* str_numdelims:*************************************************************/
/* str_numdelims: Similar to above except if the character occurs twice in a */
/* str_numdelims: row it is ignored.                                         */
/* str_numdelims:*************************************************************/

int str_numdelims(char *string, char c)
{
    char *ptr = string;
    int count = 0;

    while(*ptr) {
        if(*ptr == c) {
            count++; ptr++;
            /* Skip. extra ones. */
            while(*ptr == c) ptr++;
        } else {
            ptr++;
        }
    }

    return(count);
}

/* str_token:*****************************************************************/
/* str_token: Functionally similar to strtok except it does not use static   */
/* str_token: memory and can be called by a function anywhere on the stack.  */
/* str_token:*****************************************************************/

char *str_token(char **ptr, char c)
{
        char *newptr, *mem;
        
        if((newptr = strchr(*ptr, c)) != NULL) {
            *newptr = 0;
            newptr++;
        }

        mem = strdup(*ptr);

        if((newptr != NULL) && (*newptr == 0))
            *ptr = NULL;
        else    
            *ptr = newptr;
        
        return(mem);
}

/* str_lower:*****************************************************************/
/* str_lower: Convert a whole string to lower case.                          */
/* str_lower:*****************************************************************/

char *str_lower(char *string)
{
    char *ptr = string;

    while(*ptr) { *ptr = tolower(*ptr); ptr++; }

    return(ptr);
}

/*----------------------------------------------------------------------------
  str_ending()
  Determine if a substring is in fact the last few characters of a string.
  Useful for checking file endings.
  ----------------------------------------------------------------------------*/
char *str_ending(char *string, char *substring)
{
   char *ptr;
   
   if(((ptr = str_casestr(string, substring)) != NULL) &&
      (strlen(ptr) == strlen(substring)))
      return(ptr);
   
   return(NULL);
}

/* 
 * strstr.c --
 *
 *	Source code for the "strstr" library routine.
 *
 * Copyright (c) 1988-1993 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *----------------------------------------------------------------------
 *
 * strstr --
 *
 *	Locate the first instance of a substring in a string.
 *
 * Results:
 *	If string contains substring, the return value is the
 *	location of the first matching instance of substring
 *	in string.  If string doesn't contain substring, the
 *	return value is 0.  Matching is done on an exact
 *	character-for-character basis with no wildcards or special
 *	characters.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
  str_casestr()
  Same as the strstr() library function except it ignores case. Adapted
  from the original strstr() routine (C)as above.
  ---------------------------------------------------------------------------*/
char *str_casestr(char *string, char *substring)
{
    register char *a, *b;

    /* First scan quickly through the two strings looking for a
     * single-character match.  When it's found, then compare the
     * rest of the substring.
     */

    b = substring;
    if (*b == 0) {
	return string;
    }
    for ( ; *string != 0; string += 1) {
	if (locase(*string) != locase(*b)) {
	    continue;
	}
	a = string;
	while (1) {
	    if (*b == 0) {
		return string;
	    }
	    if (locase(*a) != locase(*b)) {
                a++; b++;
		break;
	    }
            a++; b++;
	}
	b = substring;
    }
    return (char *) 0;
}

/*----------------------------------------------------------------------------
  str_sub()
  Given a string substitute one character for another.
  
  char *string	Working string
  char c	Character to search for
  char sub	Character to use as the substitute
  ----------------------------------------------------------------------------*/
int str_sub(char *string, char c, char sub)
{
    char *ptr = string;
    int count = 0;
    
    while(*ptr) {
	if(*ptr == c) {
	    count++;
	    *ptr = sub;
	}
	ptr++;
    }

    return(count);
}

/*---------------------------------------------------------------------------
  str_word()
  Read the next word from the string and advance he pointer.

  char **string		Pointer to string to parse
  ---------------------------------------------------------------------------*/
char *str_word(char **string)
{
    static char word[512];
    char *ptr = *string, *wptr = word;

    while(isspace(*ptr) && *ptr != 0) ptr++;
    while(!isspace(*ptr) && *ptr != 0) *wptr++ = *ptr++;
    *wptr = 0;
    *string = ptr;

    if(word[0] == 0)
        return(NULL);
    else
        return(word);
}

/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*----------------------------------------------------------------------------
  str_error()
  Function to do the same as strerror() but for systems without this function.
  It returns a static string decribing a given error number.

  int errnum	Number of system error to find
  ---------------------------------------------------------------------------*/
char *str_error(int errnum)
{
        extern int sys_nerr;
/*        extern char *sys_errlist[];	Let's see if stdio.h is enough */
        static char ebuf[40];           /* 64-bit number + slop */

        if ((unsigned int)errnum < sys_nerr)
                return(sys_errlist[errnum]);
        (void)sprintf(ebuf, "Unknown error: %d", errnum);
        return(ebuf);
}
