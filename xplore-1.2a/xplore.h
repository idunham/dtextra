
/*************************************************************************/
/* xplore.h: global definitions                                          */
/*************************************************************************/

#ifndef XPLORE_H
#define XPLORE_H

#define VERSION "1.2a"

/* general blurb *********************************************************/

/* some systems define SVR4 but not SYSV */

#ifdef SVR4
#ifndef SYSV
#define SYSV
#endif
#endif

/* alloca */

#ifdef _AIX
#pragma alloca
#else
#ifdef hpux
#pragma alloca
#else
#if defined(__NetBSD__) || defined(__OpenBSD__)
#pragma alloca
#else
#include <alloca.h>
#endif	/* __NetBSD__ || __OpenBSD__ */
#endif	/* hpux */
#endif	/* _AIX */

/* includes from the system libraries */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/wait.h>

/* statfs includes */

#if defined(SVR4) || defined(SOLARIS) || defined(SGI)
#ifndef USE_STATVFS
#define USE_STATVFS
#endif
#endif	/* SVR4 || SOLARIS || SGI */

#ifdef USE_STATVFS
#include <sys/statvfs.h>
#else
#if defined(__NetBSD__) || defined(__OpenBSD__)
#include <sys/param.h>
#include <sys/mount.h>
#else
#ifdef OSF1
#include <sys/vfs_proto.h>
#else
#include <sys/vfs.h>
#endif	/* OSF1 */
#endif	/* __NetBSD__ || __OpenBSD__ */
#endif	/* USE_STATVFS */

#ifdef _AIX
#include <sys/resource.h>
#include <sys/statfs.h>
#endif

/* basic X11 and Motif stuff */

#include <Xm/Xm.h>

#if XmVERSION < 2
#error "Sorry, this program requires Motif 2.x"
#endif

#include <X11/Xresource.h>
#include <X11/xpm.h>

/* ULTRIX apparently doesn't define these */

#ifdef ultrix
#define S_ISLNK(mode) (mode & S_IFMT) == S_IFLNK
#define S_ISSOCK(mode) (mode & S_IFMT) == S_IFSOCK
#endif

/* for compatibility with BSDI */

#define fnmatch xfnmatch

/* global application-specific stuff ***************************************/

extern String version;         /* program version */
extern char basedir[];         /* base directory */
extern char libdir[];	       /* name of the library directory */
extern char libconfig[];       /* name of the system-wide xplorerc file */
extern char libmagic[];	       /* name of the system-wide magic file */
extern char libsetup[];	       /* name of setup script */
extern char libstartup[];      /* name of startup script */
extern char libicons[];	       /* default icons dir */
extern char libschemes[];      /* default color schemes dir */

extern FILE *stdout_dup;       /* stdout duplicate */
extern FILE *stderr_dup;       /* stderr duplicate */
extern int *pipefd;	       /* pipe for stdout/err redirection */

/* default xplore lib dir: */

#ifndef XPLORELIBDIR
#define XPLORELIBDIR "/usr/lib/X11/xplore"
#endif

/* standard file names: */

#ifndef XPLORE_ICONS
#define XPLORE_ICONS "icons"
#endif

#ifndef XPLORE_SCHEMES
#define XPLORE_SCHEMES "schemes"
#endif

#ifndef XPLORE_SETUP
#define XPLORE_SETUP "setup"
#endif

#ifndef XPLORE_STARTUP
#define XPLORE_STARTUP "startup"
#endif

#ifndef XPLORE_RC
#define XPLORE_RC "system.xplorerc"
#endif

#ifndef XPLORE_MAGIC
#define XPLORE_MAGIC "magic"
#endif

/* how to invoke the C preprocessor: */

#ifndef CPP
#define CPP "/lib/cpp"
#endif

/* memory allocation routines: */

#define NEWSTRING(s)  (s?XtNewString(s):NULL)
#define MALLOC(n)     XtMalloc(n)
#define REALLOC(p,n)  XtRealloc((void *)(p),(n))
#define FREE(p)       XtFree((void *)(p))

/* symbolic parameter values: */

enum { ALL = -3, NONE = -1, ANY = -2, DFLT = 0 };

/* switch cursors */

extern Boolean WaitFlag;

void Wait(void);
void WaitMsg(String msg);
void WaitMsg2(String msg, String arg);
void Done(void);
Boolean CheckBreak(void);

/* print an error message on stderr: */

void error(String msg);

/* perform the necessary cleanup for termination: */

void cleanup(void);

/* write a fatal error message, clean up and exit with nonzero return value: */

void fatal(String msg);

/* clean up and exit: */

void quit(void);

#endif /* XPLORE_H */
