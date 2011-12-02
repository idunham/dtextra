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
 * FILE NAME:	magic.h
 * CREATED:	Mon Oct 25 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Header file for magic.c
 *
 */

#ifndef COMMON_MAGIC_H
#define COMMON_MAGIC_H	1

#define	MAGIC_JPEG		1
#define	MAGIC_GIF		2
#define	MAGIC_COMPRESS		3
#define	MAGIC_GZIP		4
#define	MAGIC_TROFF		5
#define	MAGIC_POSTSCRIPT	6
#define MAGIC_BZIP2		7
#define MAGIC_XZ		8

extern int magic_ftype(char *, int);

#define	is_jpeg(filename)	magic_ftype(filename, MAGIC_JPEG)
#define	is_gif(filename)	magic_ftype(filename, MAGIC_GIF)
#define	is_compressed(filename)	magic_ftype(filename, MAGIC_COMPRESS)
#define	is_gzipped(filename)	magic_ftype(filename, MAGIC_GZIP)
#define	is_troff(filename)	magic_ftype(filename, MAGIC_TROFF)
#define	is_postscript(filename)	magic_ftype(filename, MAGIC_POSTSCRIPT)
#define is_bzipped(filename)	magic_ftype(filename, MAGIC_BZIP2)
#define is_xzipped(filename)	magic_ftype(filename, MAGIC_XZ)

#endif
