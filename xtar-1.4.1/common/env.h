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
 * FILE NAME:	env.h
 * CREATED:	Wed Oct 27 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Header file for env.c
 *
 */

#ifndef COMMON_ENV_H
#define COMMON_ENV_H		1

#include <limits.h>

extern int env_osrelease();
extern int env_inpath(char *);
extern char *env_realname();
extern char *env_hostname();
extern char *env_username();
extern char *env_email();

#define	MAXUSERNAMELEN	10
#define	MAXGCOSLEN	256
#define	MAXEMAILLEN	MAXUSERNAMELEN+MAXHOSTNAMELEN+1

#define	running9()	(env_osrelease() == 9)
#define	env_isroot()	(!strcmp("root", env_username()))

#ifdef	_USE_GETWD
#define	env_cwd(buffer, length)	getwd(buffer)
#else
#define	env_cwd(buffer, length)	getcwd(buffer, length)
#endif

#endif
