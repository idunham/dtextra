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
 * FILE NAME:	tmpfile.c
 * CREATED:	Thu Nov 11 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Header file for tmpfile.c.
 *
 */

#ifndef COMMON_TMPFILE_H
#define COMMON_TMPFILE_H

extern char *tmpfile_create();
extern void tmpfile_free();

#endif
