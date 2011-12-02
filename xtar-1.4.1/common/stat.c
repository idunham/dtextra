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
 * FILE NAME:	stat.c
 * CREATED:	Mon Oct 25 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION: Simple file routines based on stat() and the stat structure.
 *
 */

#include <sys/stat.h>		/* For struct stat        */
#include <time.h>		/* For time_t             */

#ifndef	S_ISDIR
#define	S_ISDIR(m)		((m)&0040000)
#endif

int stat_size(char *);		/* File size in bytes     */
time_t stat_atime(char *);	/* Last access time       */
time_t stat_mtime(char *);	/* Last modification time */
time_t stat_ctime(char *);	/* Last status change     */
unsigned short stat_mode(char *);	/* Mode of file           */
uid_t stat_uid(char *);		/* UID of owner           */
gid_t stat_gid(char *);		/* GID of owner           */
int stat_isdir(char *);         /* Is this a directory    */
int stat_islink(char *);        /* Is this a soft-link    */

/* stat_mtime:********************************************************/
/* stat_mtime: Modification time of file.                            */
/* stat_mtime:********************************************************/

time_t stat_mtime(char *filename)
{
    struct stat statbuf;

    if(stat(filename, &statbuf) < 0)
        return(0);
    else 
        return(statbuf.st_mtime);
}

/* stat_atime:*******************************************************/
/* stat_atime: Last access time of file.                            */
/* stat_atime:*******************************************************/

time_t stat_atime(char *filename)
{
    struct stat statbuf;

    if(stat(filename, &statbuf) < 0)
        return(0);
    else 
        return(statbuf.st_atime);
}

/* stat_ctime:*******************************************************/
/* stat_ctime: Last time file status changed.                       */
/* stat_ctime:*******************************************************/

time_t stat_ctime(char *filename)
{
    struct stat statbuf;

    if(stat(filename, &statbuf) < 0)
        return(0);
    else 
        return(statbuf.st_ctime);
}

/* stat_size:********************************************************/
/* stat_size: Stat the given file and return its size in bytes      */
/* stat_size:********************************************************/

int stat_size(char *filename)
{
    struct stat statbuf;

    if(stat(filename, &statbuf) < 0)
        return(0);
    else    
        return((int)statbuf.st_size);
}

/* stat_mode:********************************************************/
/* stat_mode: Mode of file - chmod thingies.                        */
/* stat_mode:********************************************************/

unsigned short stat_mode(char *filename)
{
    struct stat statbuf;

    if(stat(filename, &statbuf) < 0)
        return(0);
    else
        return(statbuf.st_mode);
}

/* stat_uid:*********************************************************/
/* stat_uid: UID of owner of file.                                  */
/* stat_uid:*********************************************************/

uid_t stat_uid(char *filename)
{
    struct stat statbuf;

    if(stat(filename, &statbuf) < 0)
        return(0);
    else
        return(statbuf.st_uid);
}

/* stat_gid:*********************************************************/
/* stat_gid: GID of owner of file.                                  */
/* stat_gid:*********************************************************/

gid_t stat_gid(char *filename)
{
    struct stat statbuf;

    if(stat(filename, &statbuf) < 0)
        return(0);
    else
        return(statbuf.st_gid);
}

/* stat_isdir:****************************************************************/
/* stat_isdir: Stat the file and see if it is a directory.                   */
/* stat_isdir:****************************************************************/

int stat_isdir(char *filename)
{
    struct stat statbuf;

    if(stat(filename, &statbuf) < 0)
        return(0);
    else
        return(S_ISDIR(statbuf.st_mode));
}

/* stat_islink:***************************************************************/
/* stat_islink: Stat the file and see if it is a link.                       */
/* stat_islink:***************************************************************/

int stat_islink(char *filename)
{
    struct stat statbuf;

    if(lstat(filename, &statbuf) < 0)
        return(0);
    else
        return((statbuf.st_mode&S_IFMT)==S_IFLNK);
}
