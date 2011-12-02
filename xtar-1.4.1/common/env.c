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
 * FILE NAME:	env.c
 * CREATED:	Wed Oct 27 1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Contains routines concerned with the user's environment.
 *
 */

#include <netdb.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/utsname.h>

#include "env.h"

int env_osrelease();
int env_inpath(char *);
char *env_realname();
char *env_hostname();
char *env_username();
char *env_email();

/* env_osrelease:******************************************************/
/* env_osrelease: Returns the major release number of HP-UX that is   */
/* env_osrelease: running. (Not very portable :-)                     */
/* env_osrelease:******************************************************/

int env_osrelease()
{
#ifdef __hpux
    struct utsname name;
    char release[_SYS_NMLN], *ptr;
    int retval;

   /* If you're interested this is what the uname struct looks like:

      struct utsname {
        char    sysname[_SYS_NMLN];
        char    nodename[_SYS_NMLN];
        char    release[_SYS_NMLN];
        char    version[_SYS_NMLN];
        char    machine[_SYS_NMLN];
        char    __idnumber[_SNLEN];
      };

    */

    /* If uname call fails then game over! */
    if(uname(&name) < 0)
        return(-1);

    /* Make a copy */
    strcpy(release, name.release);

    /* Get rid of the minor release number */
    if((ptr = strchr(release, '.')) == NULL)
        return(-1);
    *ptr = 0;

    /* Return the major release version as an integer */
    if((retval = atoi(release)) > 0)
        return(retval);
    else
        return(-1);
#else
    return(-1);
#endif
}

/* env_inpath:********************************************************/
/* env_inpath: Search the users path for the given command.          */
/* env_inpath:********************************************************/

int env_inpath(char *command)
{
    char *ptr, filename[BUFSIZ], path[BUFSIZ];

    /* Make a copy of the user's path */
    strcpy(path, getenv("PATH"));
    if(path == NULL)
        return(0);

    /* Split it up into component parts */
    if((ptr = strtok(path, ":")) == NULL)
        return(0);

    /* See if the file exists with each component */
    do {
        sprintf(filename, "%s/%s", ptr, command);
        if(file_exists(filename))
            return(1);
    } while ((ptr = strtok(NULL, ":")) != NULL);

    return(0);
}

/* env_realname:**************************************************************/
/* env_realname: The real name of the person currently running this process. */
/* env_realname:**************************************************************/

char *env_realname()
{
    struct passwd  *passwd;
    static char buffer[MAXGCOSLEN];
    char *ptr;

    /* Try and find out their real name */
    if((passwd = getpwuid(getuid())) != NULL) {
        strcpy(buffer, passwd->pw_gecos);
        if((ptr = strchr(buffer, ',')) != NULL)
            *ptr = 0;
        return(buffer);
    }

    return(NULL);
}

/* env_hostname:**************************************************************/
/* env_hostname: Return the name of the current host.                        */
/* env_hostname:**************************************************************/

char *env_hostname()
{
    struct hostent *hostent;
    static char buffer[MAXHOSTNAMELEN];
    char hostname[MAXHOSTNAMELEN];

     /* Find their hostname */
    if(!gethostname(hostname, MAXHOSTNAMELEN)) {
        if((hostent = gethostbyname(hostname)) != NULL) {
            strcpy(buffer, hostent->h_name);
            return(buffer);
        }
    }

    return(NULL);
}

/* env_username:**************************************************************/
/* env_username: The username of the person running the program.             */
/* env_username:**************************************************************/

char *env_username()
{
    struct passwd  *passwd;
    static char buffer[MAXUSERNAMELEN];

    if((passwd = getpwuid(getuid())) != NULL)
        strcpy(buffer, passwd->pw_name);
    else
        strcpy(buffer, getlogin());

    return(buffer);
}

/* env_email:*****************************************************************/
/* env_email: EMail addres of user running program.                          */
/* env_email:*****************************************************************/

char *env_email()
{
    static char buffer[MAXEMAILLEN];
    char *hostname = env_hostname(), *ptr;
    
    if(hostname && ((ptr = strchr(hostname, '.'))!=NULL) && (strlen(ptr)>1))
        hostname = ptr + 1;

    sprintf(buffer, "%s@%s", env_username(), hostname);

    return(buffer);
}
