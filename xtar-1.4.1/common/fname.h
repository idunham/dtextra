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
 * FILE NAME:	fname.h
 * CREATED:	Mon Oct 25 GMT 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Header file for fname.c
 *
 */

#ifndef	COMMON_FNAME_H
#define	COMMON_FNAME_H	1

extern void fname_change(char *, char *, char *);
extern char *fname_basename(char *);
extern char *fname_dirname(char *);
extern char *fname_resolve(char *, ...);

#define	fname_c2g(filename)	fname_change(filename, ".Z", ".gz")
#define	fname_g2c(filename)	fname_change(filename, ".gz", ".Z")

#define	MAXFNAMELEN		256

#endif
