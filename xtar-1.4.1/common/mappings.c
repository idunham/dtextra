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
 * FILE NAME:	mappings.c
 * CREATED:	Fri Aug 12 1994
 * AUTHOR:	Rik Turnbull <rik@csc.liv.ac.uk>
 * DESCRIPTION:	Routines for handling filename -> command mappings. This
 *              enables the correct external program to be called to view the
 *              file.
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

/******************************************************************************/
/********************** INTERNAL STRUCTURE & VARIABLES  ***********************/
/******************************************************************************/
typedef struct _MAPPING {
    char *ending;
    char *command;
} MAPPING;

static MAPPING **map= NULL;



/******************************************************************************/
/*************************** EXTERNAL FUNCTION PROTOTYPES *********************/
/******************************************************************************/
void mappings_create(char *);
int mappings_execute(char *, char *, mappings_func);



/******************************************************************************/
/*************************** INTERNAL FUNCTION PROTOTYPES *********************/
/******************************************************************************/
static MAPPING *mappings_alloc(char *, char *);



/******************************************************************************/
/****************************** EXTERNAL FUNCTIONS ****************************/
/******************************************************************************/

/*----------------------------------------------------------------------------
  mappings_create()
  This is the initialization function which creates the appropriate data
  structures for mapping the filenames to commands.
  ----------------------------------------------------------------------------*/
void mappings_create(char *mappings)
{
    char *memory, *m, *ptr, *ending, *command;
    MAPPING *mapping;
    int i = 0, count;

    if(map != NULL)
        return;

    count = str_numchars(mappings, '\n');

    if(count == 0)
        return;

    if((map = (MAPPING **) malloc(sizeof(MAPPING *)*(count+2))) == NULL)
        return;

    m = memory = strdup(mappings);

    /* Parse all data. */
    while(m != NULL) {
        /* Ignore leading white space. */
        while(isspace(*m)) m++;

        /* The delimiter for each line is "\n" */
        ptr = strstr(m, "\n");
        if(ptr != NULL) {
            *ptr = 0;
            ptr++;
        }

        /* Split into ending/command - on white space. */
        ending = m;  command = m;
        while(!isspace(*command)) command++;
        *command = 0;  command++;
        while(isspace(*command))  command++;

        /* Allocate memory for a new mapping. */
        if(mapping = mappings_alloc(ending, command))
            map[i++] = mapping;

        /* Reinitialize pointer. */
        m = ptr;
    }
    map[i] = NULL;

    free(memory);
}

/*----------------------------------------------------------------------------
  int mappings_execute()
  Given the filename execute the appropriate program based on the file ending.
  ----------------------------------------------------------------------------*/
int mappings_execute(char *filename, char *tmpname, mappings_func mf)
{
    char command[256], fname[256], *ptr, *tmpnew;
    MAPPING *mapping;
    int i;

    /* Is the data stored in a temporary file? */
    if(tmpname == NULL) tmpname = filename;

    /* Get lower case version of filename. */
    strcpy(fname, filename);

    /* Search for appropriate ending. */
    for(i = 0; map && map[i]; i++) {
        mapping = map[i];
        if(str_ending(fname, mapping->ending)) {
            sprintf(command, mapping->command, tmpname);
            strcat(command, "&");
            system(command);
            return(1);
        }
    }

    /* If we got this far then maybe the file is compressed or gzipped */
    if(is_gzipped(tmpname) && ((ptr = str_ending(fname, ".gz")) != NULL)) {
       *ptr = 0;
       tmpnew = tmpfile_create();
       sprintf(command, "gzip -dc %s > %s", tmpname, tmpnew);
       system(command);
       return(mappings_execute(fname, tmpnew, mf));
    } else if(is_bzipped(tmpname) && ((ptr =str_ending(fname, "bz2")) != NULL)) {
       *ptr = 0;
       tmpnew = tmpfile_create();
       sprintf(command, "bzip2 -dc %s > %s", tmpname, tmpnew);
       system(command);
       return(mappings_execute(fname, tmpnew, mf));
    } else if(is_xzipped(tmpname) && ((ptr =str_ending(fname, "xz")) != NULL)) {
       *ptr = 0;
       tmpnew = tmpfile_create();
       sprintf(command, "xz -dc %s > %s", tmpname, tmpnew);
       system(command);
       return(mappings_execute(fname, tmpnew, mf));
    } else if(is_compressed(tmpname) && ((ptr = str_ending(fname, ".Z")) != NULL)) {
       *ptr = 0;
       tmpnew = tmpfile_create();
       sprintf(command, "compress -dc < %s > %s", tmpname, tmpnew);
       system(command);
       return(mappings_execute(fname, tmpnew, mf));
    }
    
    return(mf ? mf(filename, tmpname) : 0);
}



/******************************************************************************/
/****************************** INTERNAL FUNCTIONS ****************************/
/******************************************************************************/

/*----------------------------------------------------------------------------
  mappings_alloc()
  Allocate memory for a new MAPPING structure and intialize data structures
  with copies of "ending" and "command".
  ----------------------------------------------------------------------------*/
static MAPPING *mappings_alloc(char *ending, char *command)
{
    MAPPING *mapping;

    /* Allocate memory. */
    mapping = (MAPPING *) malloc(sizeof(MAPPING));
    if(mapping == NULL) return(NULL);

    /* Initialize structure. */
    mapping->ending = strdup(ending);
    mapping->command = strdup(command);

    /* DONE. */
    return(mapping);
}
