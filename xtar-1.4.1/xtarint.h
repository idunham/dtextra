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
 * FILE NAME:	xtarint.h
 * CREATED:	Mon Nov  1 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Internal header file for xtar.c.
 *
 */

#ifndef	XTAR_XTARINT_H
#define	XTAR_XTARINT_H		1

#include <sys/time.h>

/* File types in tar archive                              */
#define TYPE_UNIX       0x0     /* Unix file              */
#define TYPE_NORMAL     0x30    /* Normal disk file       */
#define TYPE_HARDLINK   0x31    /* Hard link              */
#define TYPE_SOFTLINK   0x32    /* Soft link              */
#define TYPE_CHARACTER  0x33    /* Character special file */
#define TYPE_BLOCK      0x34    /* Block special file     */
#define TYPE_DIRECTORY  0x35    /* Directory              */

/* Sizes of fields in tar header                          */
#define NAMSIZ          100
#define MODE_SZ         8
#define UID_SZ          8
#define GID_SZ          8
#define SIZE_SZ         12
#define MTIME_SZ        12
#define CHKSUM_SZ       8
#define MAGIC_SZ        6
#define VERSION_SZ      2
#define UNAME_SZ        32
#define GNAME_SZ        32
#define DEV_SZ          8
#define PREFIX_SZ       155
#define PADDING_SZ      12

/* Actual tar header - 512 bytes long */
typedef struct {
    char name[NAMSIZ];		/* Name of file		*/
    char mode[MODE_SZ];		/* Mode on it		*/
    char uid[UID_SZ];		/* UID of owner		*/
    char gid[GID_SZ];		/* GID of owner		*/
    char size[SIZE_SZ];		/* Size in bytes	*/
    char mtime[MTIME_SZ];	/* Mod time		*/
    char chksum[CHKSUM_SZ];	/* Header checksum	*/
    char typeflag;		/* File type (as above) */
    char linkname[NAMSIZ];	/* Name of soft link 	*/
    char magic[MAGIC_SZ];	/* TAR magic number	*/
    char version[VERSION_SZ];	/* TAR version		*/
    char uname[UNAME_SZ];	/* Username of owner	*/
    char gname[GNAME_SZ];	/* Group name of owner	*/
    char devmajor[DEV_SZ];	/* Major device number	*/
    char devminor[DEV_SZ];	/* Minor device number	*/
    char prefix[PREFIX_SZ];	/* Filename prefix	*/
    char padding[PADDING_SZ];	/* To make up to 512b	*/
} FILE_HEADER;
#define FILE_HEADER_SIZE        sizeof(FILE_HEADER)

/* Just the info we want from the header */
typedef struct tar_file {
    char name[256];             /* File name                      */
    char linkname[256];         /* Name if a soft link            */
    char permissions[11];       /* Permissions eg. drwxr--r--     */
    char mtime[24];             /* Mod time eg. Jun  4 18:06 1992 */
    time_t time;		/* Time in seconds                */
    int uid;                    /* User id of owner               */
    int gid;                    /* Group id of owner              */
    int size;                   /* Size of file                   */
    int file_offset;            /* Offset in tar archive          */
    int file_type;              /* Type of file                   */
    mode_t mode;                /* File mode - so can use macros  */
    int is_ascii;		/* Is this file ascii?            */
    struct tar_file *nextdir;   /* Used to for chmod dirs         */
    struct tar_file *next;      /* Next file in list              */
} TAR_FILE;

#endif
