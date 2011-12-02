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
 * FILE NAME:	pipe.c
 * CREATED:	Mon Nov  1 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Commands similar to reading files except we read from a pipe.
 *              Useful for reading compressed files. Note SIGCLD must be
 *              ignored for this to work.
 *
 *
 */

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

#include "magic.h"
#include "str.h"

int pipe_open(char *);
int pipe_seek(int, int);
int pipe_read(int, char *, int);

#define F_DUPFAILED     3
#define F_EXECFAILED    4

/* pipe_open:*****************************************************************/
/* pipe_open: Open a file for reading - if it is a normal file then just     */
/* pipe_open: open it and return. Otherwise fork a process to uncompress it. */
/* pipe_open: At the moment this handles files in compress or gzipped        */
/* pipe_open: format.                                                        */
/* pipe_open:*****************************************************************/

int pipe_open(char *filename)
{
    int outfd[2];
    pid_t pid;
    char name[12], args[4];

    /* Set up command and arguments depending on compression program. */
    if(is_gzipped(filename)) {
        strcpy(name, "gzip");
        strcpy(args, "-dc");
    }
    else if(is_bzipped(filename)) {
	strcpy(name, "bzip2");
	strcpy(args, "-dc");
    }
    else if(is_xzipped(filename)) {
	strcpy(name, "xz");
	strcpy(args, "-dc");
    }
    else if(is_compressed(filename)) {
        strcpy(name, "compress");
        strcpy(args, "-dc");
    }
    else if(filename == NULL) {
        return(0);
    } else {
        return(open(filename, O_RDONLY));
    }

    /* Create a pipe to read from when the fork is run */
    if(pipe(outfd) == -1)
        return(-1);

    /* Fork another process to perform the decompression */
    if((pid = fork()) == 0) {
        /*CHILD*/
        close(outfd[0]);

        if(dup2(outfd[1], 1) < 0) {
            close(outfd[1]);
            fprintf(stderr, "dup2() failed: %s\n", str_error(errno));
            exit(F_DUPFAILED);
        }

        if(execlp(name, name, args, filename, NULL) == -1) {
            close(outfd[1]);
            fprintf(stderr, "Failed to execlp(): %s\n", str_error(errno));
            exit(F_EXECFAILED);
        }
    }
    else if(pid > 0) {
        /*PARENT*/
        close(outfd[1]);
        return(outfd[0]);
    }
    else
        return(-1);
}

/* pipe_seek:*****************************************************************/
/* pipe_seek: This is not as flexible as fseek. You can only seek forward    */
/* pipe_seek: from the present position.                                     */
/* pipe_seek:*****************************************************************/

int pipe_seek(int fp, int offset)
{
    char buffer[BUFSIZ];
    int readbytes = 0, val;

    /* We must actually perform a read and discard the data, rather than do
       an acutual seek - apparently.
    */
    while(offset > 0) {
        val = offset > BUFSIZ ? BUFSIZ : offset;
        while(((readbytes = read(fp, buffer, val)) > 0) && (val > 0))
            val -= readbytes;
        if(readbytes == -1)
            return(-1);
        offset -= BUFSIZ;
    }

    return(1);
}

/* pipe_read:*****************************************************************/
/* pipe_read: Read a block from the pipe.                                    */
/* pipe_read:*****************************************************************/

int pipe_read(int fp, char *buffer, int size)
{
    int readbytes = 0, ind = 0;
    char *ptr;

    /* Our starting point */
    ptr = buffer;

    /* Fill up the buffer. */
    while((size > 0) && ((readbytes = read(fp, ptr, size)) > 0)) {
            size -= readbytes;
            ind += readbytes;
            ptr = &buffer[ind];
    }

    /* Was there an error? */
    if(readbytes == -1)
        return(-1);

    /* Return number of bytes read */
    return(ind);
}
