
#include "xplore.h"
#include "error.h"

#include "dialogs.h"

static String errmsg[] = {
  /* ERR_NONE */	   "No error",
  /* ERR_ABORT */	   "Operation aborted",
  /* ERR_CHDIR */	   "Error changing to directory\n%s",
  /* ERR_CHMOD */	   "Error changing permissions for\n%s",
  /* ERR_CHOWN */	   "Error changing owner of\n%s",
  /* ERR_COPY */	   "Error copying %s\nto %s",
  /* ERR_CREATE_DIR */	   "Error creating directory\n%s",
  /* ERR_CREATE_FILE */	   "Error creating file\n%s",
  /* ERR_CREATE_RESFILE */ "Error creating resource file",
  /* ERR_DELETE */	   "Error deleting\n%s",
  /* ERR_DOTDOT */	   "Cannot %s parent directory",
  /* ERR_EMPTY_PATTERN */  "No pattern specified\nPlease try again.",
  /* ERR_EXEC */	   "Error executing command %s\nin directory %s",
  /* ERR_FIND_DIR */	   "Couldn't find directory\n%s",
  /* ERR_IDENTICAL */	   "Source and target are identical\n%s\n%s",
  /* ERR_INVALID */	   "Unrecognized file type\n%s",
  /* ERR_INVALID_USR */	   "Invalid user id\n%s",
  /* ERR_INVALID_GRP */	   "Invalid group id\n%s",
  /* ERR_LINK */	   "Error linking %s\nto %s",
  /* ERR_MOVE */	   "Error moving %s\nto %s",
  /* ERR_MOUNT */	   "Error mounting device on\n%s",
  /* ERR_PARSE_TYPE */	   "Error parsing type\n(%s)\nPlease try again.",
  /* ERR_READ_DIR */	   "Error reading directory\n%s",
  /* ERR_READ_ICON */	   "Error reading icon\n%s",
  /* ERR_RENAME */	   "Error renaming %s\nto %s",
  /* ERR_TARGET_IS_DIR */  "Target is a directory\n%s",
  /* ERR_TARGET_NOT_DIR */ "Target is not a directory\n%s",
  /* ERR_SYS */		   "%s\n(%s)",
  /* other */		   "Unknown error code (this shouldn't happen)"
};

#define code(err) ((0<=(err)&&(err)<N_ERRS)?err:N_ERRS)

static void pxplerr(String s)
{
    error_dlg(NULL, s);
}

void xplerr(int err)
{
    pxplerr(errmsg[code(err)]);
}

void xplerr1(int err, String s)
{
    String msg = (String) alloca(strlen(errmsg[code(err)])+strlen(s)+1);

    sprintf(msg, errmsg[code(err)], s);
    pxplerr(msg);
}

void xplerr2(int err, String s1, String s2)
{
    String msg = (String)
	alloca(strlen(errmsg[code(err)])+strlen(s1)+strlen(s2)+1);

    sprintf(msg, errmsg[code(err)], s1, s2);
    pxplerr(msg);
}

extern const char *const sys_errlist[];

static void psyserr(String s)
{
  if (errno) {
    String msg = (String) alloca(strlen(errmsg[ERR_SYS])+strlen(s)+
				 strlen(sys_errlist[errno]));
    sprintf(msg, errmsg[ERR_SYS], s, sys_errlist[errno]);
    pxplerr(msg);
  } else
    pxplerr(s);
}

void syserr(int err)
{
    psyserr(errmsg[code(err)]);
}

void syserr1(int err, String s)
{
    String msg = (String) alloca(strlen(errmsg[code(err)])+strlen(s)+1);

    sprintf(msg, errmsg[code(err)], s);
    psyserr(msg);
}

void syserr2(int err, String s1, String s2)
{
    String msg = (String)
	alloca(strlen(errmsg[code(err)])+strlen(s1)+strlen(s2)+1);

    sprintf(msg, errmsg[code(err)], s1, s2);
    psyserr(msg);
}
