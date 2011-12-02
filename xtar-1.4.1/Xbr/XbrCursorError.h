/* 
 * Copyright 1993, 1994 Liverpool University Computer Science Department
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
 * FILE NAME: 	XbrCursorError.h
 * CREATED:	Thu Oct 28 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION: Error cursor bitmap.
 *
 */

#ifndef	XBRLIB_XBRCURSORERROR_H
#define	XBRLIB_XBRCURSORERROR_H		1

#define err_width 20
#define err_height 20
#define err_xhot 9
#define err_yhot 8
static unsigned char err_bits[] = {
   0x00, 0x00, 0x00, 0xc0, 0x0f, 0x00, 0xf0, 0x3a, 0x00, 0x58, 0x55, 0x00,
   0x2c, 0xa0, 0x00, 0x56, 0x40, 0x01, 0xaa, 0x80, 0x02, 0x46, 0x81, 0x01,
   0x8a, 0x82, 0x02, 0x06, 0x85, 0x01, 0x0a, 0x8a, 0x02, 0x06, 0x94, 0x01,
   0x0a, 0xe8, 0x02, 0x14, 0x50, 0x01, 0x28, 0xb0, 0x00, 0xd0, 0x5f, 0x00,
   0xa0, 0x2a, 0x00, 0x40, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#endif
