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
 * DESCRIPTION:	Temporary file handling routines
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Data structure describing a temporary file */
typedef struct tmp_struct {
    char *name;
    struct tmp_struct *next;
} TMP_FILE;

/* Private functions */
static TMP_FILE *tmpfile_alloc(char *);
static int  tmpfile_search(char *);

/* Functions for the outside world */
void tmpfile_free();
char *tmpfile_create();

/* First file in linked list */
static TMP_FILE *first_tmpfile = NULL;

/* tmpfile_alloc:*************************************************************/
/* tmpfile_alloc: Allocate memory for a new TMP_FILE.                        */
/* tmpfile_alloc:*************************************************************/

static TMP_FILE *tmpfile_alloc(char *name)
{
    TMP_FILE *tf;

    /* Allocate memory for the structure */
    if((tf = (TMP_FILE *) malloc(sizeof(TMP_FILE))) == NULL)
        return(NULL);

    /* Copy in the file name */
    tf->name = name;

    /* Initialize next ptr */
    tf->next = NULL;

    /* OK. */
    return(tf);
}

/* tmpfile_free:**************************************************************/
/* tmpfile_free: Free up the memory for the list.                            */
/* tmpfile_free:**************************************************************/

void tmpfile_free()
{
    TMP_FILE *tf, *free_tmpfile = first_tmpfile;

    /* Follow linked list and free each node as we go */
    while(free_tmpfile != NULL) {
        /* Delete file */
        unlink(free_tmpfile->name);
        tf = free_tmpfile->next;
        if(free_tmpfile->name) free(free_tmpfile->name);
        free(free_tmpfile);
        free_tmpfile = tf;
    }

    first_tmpfile = NULL;
}

/* tmpfile_create:************************************************************/
/* tmpfile_create: Add package to linked list - return name of file created  */
/* tmpfile_create: or NULL if it failed.
/* tmpfile_create:*************************************************************/

char *tmpfile_create()
{
    TMP_FILE *ptr, *tf;
    char *filename = strdup(tmpnam(NULL));

    if(filename == NULL)
        return(NULL);

    /* Create a TMP_FILE structure */
    if((tf = tmpfile_alloc(filename)) == NULL)
        return(NULL);

    /* First element in list */
    if(!first_tmpfile) {
        first_tmpfile = tf;
    }
    else {
        /* Find last element in list */
        for(ptr = first_tmpfile; ptr->next != NULL; ptr = ptr->next) {
            /* If it is already there then don't add it */
            if(!strcmp(tf->name, ptr->name)) {
                free(tf->name);
                return(NULL);
            }
        }
        ptr->next = tf;
    }

    return(filename);
}

/* tmpfile_search:************************************************************/
/* tmpfile_search: Have a look in the linked list for the temporary file.    */
/* tmpfile_search:************************************************************/

static int tmpfile_search(char *name)
{
    TMP_FILE *tf;

    /* Follow linked list and have a look for the filename */
    for(tf = first_tmpfile; tf != NULL; tf = tf->next) {
        if(!strcmp(name, tf->name)) {
            /* Found it's name in the list, but does it exist? */
            if(!file_exists(name))
                return(0);
            else
                return(1);
        }
    }

    /* Can't find it */
    return(0);
}
