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
 * FILE NAME:	magic.c
 * CREATED:	Mon Oct 25 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Using MAGIC numbers handle file types. Better than using the
 *              filename as a guide for the file type.
 *
 */

#include <fcntl.h>
#include <stdio.h>

#include "magic.h"

int magic_ftype(char *, int);

static unsigned char jpeg[2]       = { 0xff, 0xd8 };
static unsigned char gif[3]        = { 0x47, 0x49, 0x46 };
static unsigned char compress[3]   = { 0x1f, 0x9d, 0x90 };
static unsigned char gzip[3]       = { 0x1f, 0x8b, 0x08 };
static unsigned char troff[2]      = { 0x5c, 0x22 };
static unsigned char postscript[2] = { 0x25, 0x21 };
static unsigned char bzip2[3]      = { 0x42, 0x5a, 0x68 };
static unsigned char xz[3]         = { 0xfd, 0x37, 0x7a };


/* magic_ftype:*******************************************************/
/* magic_ftype: Using magic numbers is file of type suggested?       */
/* magic_ftype:*******************************************************/

int magic_ftype(char *filename, int type)
{
    unsigned char buffer[3];
    int fp;

    /* Open the file for reading */
    if(filename == NULL) {
        fp = 0;
    } else {
        if((fp = open(filename, O_RDONLY)) < 0)
            return(0);
    }

    /* Read first 3 bytes */
    if(read(fp, buffer, 3) != 3) {
        if(filename == NULL)
            rewind(stdin);
        else
            close(fp);
        return(0);
    }
    if(filename == NULL)
        rewind(stdin);
    else
        close(fp);

    /* Compare bytes with type we are after */
    switch(type) { 
        case MAGIC_JPEG:
            if(!memcmp(buffer, jpeg, 2))
                return(1); 
            break;
        case MAGIC_GIF:
            if(!memcmp(buffer, gif, 3))
                return(1);
            break;
        case MAGIC_COMPRESS:
            if(!memcmp(buffer, compress, 3))
                return(1);
            break;
        case MAGIC_GZIP:
            if(!memcmp(buffer, gzip, 3))
                return(1);
            break;
        case MAGIC_TROFF:
            if(!memcmp(buffer, troff, 2))
                return(1);
            break;
        case MAGIC_POSTSCRIPT:
            if(!memcmp(buffer, postscript, 2))
                return(1);
            break;
	case MAGIC_XZ:
	    if(!memcmp(buffer, xz, 3))
		return(1);
	    break;
	case MAGIC_BZIP2:
	    if(!memcmp(buffer, bzip2, 3))
		return(1);
	    break;
        default:
            return(0);
    }

    return(0);
}
