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
 * FILE NAME:	mkdirhier.c
 * CREATED:	Mon Feb  7 1994
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION: This is an implementation of the mkdirhier script which creates
 *              a directory and if its parents do not exist it creates them as
 *              well.
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "fname.h"

int mkdirhier(char *, mode_t);

/* mkdirhier:*****************************************************************/
/* mkdirhier: This an implementation of the mkdirhier script which creates a */
/* mkdirhier: directory and if its parents do not exist it creates them as   */
/* mkdirhier: well.                                                          */
/* mkdirhier:*****************************************************************/

int mkdirhier(char *dirname, mode_t mode)
{
    char name[MAXFNAMELEN], create[MAXFNAMELEN], *ptr;

    /* Make a copy of the directory name */
    strcpy(name, dirname);
    
    /* Split it up into tokens with / as the split character */
    ptr = strtok(name, "/");
    if(dirname[0] == '/')
        sprintf(create, "/%s", ptr);
    else
        strcpy(create, ptr);

    while(ptr) {
        /* If the file exists do nothing */
        if(file_exists(create)) {
            /* If it is not a directory then return */
            if(!stat_isdir(create))
                return(0);
        } else {
            if(mkdir(create, mode) != 0)
                return(0);
        }
        ptr = strtok(NULL, "/");
        /* Better not strcat a NULL pointer */
        if(ptr != (char *)NULL) {
            strcat(create, "/");
            strcat(create, ptr);
        }
    }

    return(1);
}
