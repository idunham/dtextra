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
 * FILE NAME:	stat.h
 * CREATED:	Mon Oct 25 11:16:39 GMT 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION: Header file for file.c
 *
 */

#ifndef	COMMON_FILE_H
#define	COMMON_FILE_H

#include <time.h>          /* For time_t */

extern int stat_size(char *);		/* File size in bytes     */
extern time_t stat_atime(char *);	/* Last access time       */
extern time_t stat_mtime(char *);	/* Last modification time */
extern time_t stat_ctime(char *);	/* Last status change     */
extern ushort stat_mode(char *);	/* Mode of file           */
extern uid_t stat_uid(char *);		/* UID of owner           */
extern gid_t stat_gid(char *);		/* GID of owner           */
extern int stat_isdir(char *);          /* Is file a directory?   */
extern int stat_islink(char *);         /* Is file a link?        */

#endif
