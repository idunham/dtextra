
/***************************************************************************/
/* error.h: error handling routines                                        */
/***************************************************************************/

#ifndef ERROR_H
#define ERROR_H

#include "xplore.h"

/* error values: */

enum {
    ERR_NONE,             /* no error */
    ERR_ABORT,            /* operation aborted */
    ERR_CHDIR,            /* error changing to a directory */
    ERR_CHMOD,            /* error changing permissions */
    ERR_CHOWN,            /* error changing owner/group */
    ERR_COPY,             /* error copying file */
    ERR_CREATE_DIR,       /* error creating a directory */
    ERR_CREATE_FILE,      /* error creating a file */
    ERR_CREATE_RESFILE,   /* error creating resource file */
    ERR_DELETE,           /* error deleting file */
    ERR_DOTDOT,           /* move/copy/link/delete operation on .. */
    ERR_EMPTY_PATTERN,    /* empty pattern in type */
    ERR_EXEC,             /* error executing program */
    ERR_FIND_DIR,	  /* error locating directory */
    ERR_IDENTICAL,        /* source and target are identical */
    ERR_INVALID,          /* invalid file type (shouldn't happen) */
    ERR_INVALID_USR,      /* invalid user id */
    ERR_INVALID_GRP,      /* invalid group id */
    ERR_LINK,             /* error linking file */
    ERR_MOVE,             /* error moving file */
    ERR_MOUNT,            /* error mounting device */
    ERR_PARSE_TYPE,       /* error parsing type */
    ERR_READ_DIR,         /* error reading directory */
    ERR_READ_ICON,        /* error reading icon */
    ERR_RENAME,           /* error renaming file */
    ERR_TARGET_IS_DIR,    /* target for rename is a directory */
    ERR_TARGET_NOT_DIR,   /* target for multiple files is not a directory
			     (move/copy/link) */
    ERR_SYS,              /* system error (used internally) */
    N_ERRS                /* size of error table */
};

void xplerr(int err);
/* handle xplore error */

void xplerr1(int err, String s);
void xplerr2(int err, String s1, String s2);
/* like xplerr, but interpolate s resp. s1, s2 into error message */

void syserr(int err);
/* handle system error (additional error code in errno) */

void syserr1(int err, String s);
void syserr2(int err, String s1, String s2);
/* like syserr, but interpolate s resp. s1, s2 into error message */

#endif /* ERROR_H */
