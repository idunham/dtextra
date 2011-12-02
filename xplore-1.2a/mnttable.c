
/* Derived from Robert Gasch's xfsm utility (available from ftp.x.org), which
   is also distributed under the GPL (see copyright notice below). */

/* ************************************************************************* *
   xfsm - (C) Copyright 1993-1996 Robert Gasch (Robert_Gasch@peoplesoft.com)

   Permission to use, copy, modify and distribute this software for any 
   purpose and without fee is hereby granted, provided that this copyright
   notice appear in all copies as well as supporting documentation. All
   work developed as a consequence of the use of this program should duly
   acknowledge such use.

   No representations are made about the suitability of this software for
   any purpose. This software is provided "as is" without express or implied 
   warranty.

   See the GNU General Public Licence for more information.
 * ************************************************************************* */

#include "mnttable.h"

/* I simply copied the following from the xfsm header file. Probably there are
   many things in it which aren't actually needed in this module, but I
   wouldn't dare to touch these definitions, since I cannot check whether it
   still compiles on anything but Linux. */

#ifndef DYNIX
# include <stdlib.h>
#endif
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>

#if defined (SUNOS) || defined (LINUX) || defined (HPUX) || \
    defined (CONVEXOS) || defined (MACH) || defined (AUX)
#include <sys/vfs.h>
#include <mntent.h>
#define fs_ignore	MNTTYPE_IGNORE		/* ignore FS flag */
# else
# if defined (SVR4) || defined (SOLARIS)
# include <sys/statvfs.h>
# include <sys/mnttab.h>
# include <sys/mntent.h>
# define statfs		statvfs			/* FS stats structure */
# define mntent		mnttab			/* mount structure */
# define mnt_fsname	mnt_special		/* device name */
# define mnt_dir	mnt_mountp		/* mount point */
# define mnt_type	mnt_fstype		/* mount type */
# define mnt_opts	mnt_mntopts		/* mount option */
# define f_bsize	f_frsize		/* block size */
#  ifdef SOLARIS
#   define fs_ignore    MNTOPT_IGNORE		/* ignore FS flag */
#  else  
#   define fs_ignore	MNTTYPE_IGNORE		/* ignore FS flag */
#  endif /* SOLARIS */
#  else
#  ifdef DYNIX
#  include <mntent.h>
#  include <sys/vfs.h>
#  include <sys/select.h>
#  include <sys/statfs.h>
#  define fs_ignore	MNTTYPE_IGNORE	 	/* ignore FS flag */ 
#   else
#   ifdef AIX
#   include <sys/statfs.h>
#   include <sys/select.h>			/* required for msleep */
#   include <sys/mntctl.h>
#   include <sys/vmount.h>
#   include <fstab.h>
#   define mntent	vmount			/* mount structure */
#   define BUFFERSIZE	128*1024		/* 128 Kb */
#    else
#    ifdef OSF1
#    include <sys/mount.h>
#    define f_bsize	f_fsize			/* block size */
#    define fs_ignore	"xx"			/* ignore FS flag */
#     else
#     ifdef ULTRIX
#     include <sys/param.h>
#     include <sys/mount.h>
#     define GT_UNKWN	0		/* Taken from <sys/fs_types.h> */
#     define mntent	fs_data			/* FS stucture for getmnt */
#     define statfs	fs_data			/* FS stats structure */
#     define mnt_dir	fd_req.path		/* mount path */
#     define mnt_fsname	fd_req.devname		/* device name */
#     define mnt_opts	fd_req.flags		/* mount options */
#     define mnt_type	fd_req.fstype		/* mount type */
#     define f_bsize 	fd_req.bsize		/* block size */
#     define f_blocks 	fd_req.btot		/* number of blocks */
#     define f_bfree  	fd_req.bfree		/* free blocks */
#     define f_bavail 	fd_req.bfreen		/* available blocks */
#     define f_files 	fd_req.gtot		/* number of inodes (gnodes) */
#     define f_ffree 	fd_req.gfree		/* free indodes (gnodes) */
#     define fs_ignore	GT_UNKWN		/* ignore FS flag */
#      else
#      ifdef SCO
#      include <sys/statfs.h> 
#      define MAXPATHLEN 1024
#	else
#	ifdef FreeBSD
#	include <sys/mount.h>
#        ifdef SGI
#	  include <sys/statvfs.h>
#	  include <mntent.h>
#	  define statfs		statvfs
#         define fs_ignore	MNTTYPE_IGNORE	/* ignore FS flag */
#        endif /* SGI */
#	endif /* Free BSD */
#      endif /* SCO */
#     endif /* ULTRIX */
#    endif /* OSF1 */
#   endif /* AIX */
#  endif /* DYNIX */
# endif /* SVR4 || SOLARIS */
#endif /* SUNOS || LINUX || HPUX || CONVEXOS || MACH || AUX */


#if defined (SUNOS) || defined (LINUX) || defined (DYNIX) || \
    defined (CONVEXOS) || defined (AUX) || defined (SGI)
#define MOUNT_FILE	"/etc/mtab"
# else 
# if defined (SVR4) || defined (HPUX) || defined (MACH) || \
     defined (SOLARIS) || defined (SCO) 
# define MOUNT_FILE	"/etc/mnttab"
# endif /* SVR4 || HPUX || MACH || SOLARIS || SCO */
#endif /* SUNOS || LINUX || DYNIX || CONVEXOS || AUX */

#ifdef AIX
static char	vmount_buf[BUFFERSIZE];
#endif

static int size = 0;
static char **fsnames = NULL, **devnames = NULL;

static void init_mnt_table(void)
{
  int i;
  for (i = 0; i < size; i++) {
    if (fsnames[i]) free(fsnames[i]);
    if (devnames[i]) free(devnames[i]);
  }
  if (fsnames) free(fsnames);
  if (devnames) free(devnames);
  size = 0;
  fsnames = devnames = NULL;
}

static int add_mnt_entry(char *fsname, char *devname)
{
  char **fsnames1 = (char**)realloc(fsnames, (size+1)*sizeof(char*));
  char **devnames1 = (char**)realloc(devnames, (size+1)*sizeof(char*));
  if (fsnames1 && devnames1) {
    fsnames = fsnames1; devnames = devnames1;
    fsnames[size] = fsname?strdup(fsname):NULL;
    devnames[size] = devname?strdup(devname):NULL;
    size++;
    return 1;
  } else
    return 0;
}

static time_t	last_mod_time=0;

/* This code is highly system-dependent. I snapped it from xfsm and massaged
   it a bit for use with xplore. I have no means to check whether the
   following works on any other system than Linux, though, so I'd appreciate
   any bug reports (or, preferably, fixes :). -AG */

int get_mnt_table(void)
{
	int		ignore;
	char		fs_name[MAXPATHLEN+1], dev_name[MAXPATHLEN+1];
#if defined (SVR4) || defined (SOLARIS) || defined (SCO) || \
    defined (SUNOS) || defined (LINUX) || defined (HPUX) || \
    defined (CONVEXOS) || defined (MACH) || defined (DYNIX) || \
    defined (AUX) || defined (SGI)
	FILE		*fp;
#endif
#ifdef ULTRIX
	int		which_fs=0;
#endif
#ifdef AIX
	int		i=0, retcode;
#endif
#ifdef SCO
#define BUFSZ 1024
	char		buf[BUFSZ];
#endif

#if defined (SUNOS) || defined (LINUX) || defined (DYNIX) || \
    defined (HPUX) || defined (AIX) || defined (CONVEXOS) || \
    defined (AUX) || defined (SGI)
	struct mntent 	*mnt_info;
# else
# if defined (SVR4) || defined (SOLARIS) || defined (ULTRIX) || defined (MACH) 
	struct mntent 	mnt_info;
#  else
#  if defined (OSF1) || defined (FreeBSD)
	struct statfs	*mnt_info;
	int		i, nstatfs;
#  endif /* OSF1 || FreeBSD */
# endif /* SVR4 || SOLARIS || ULTRIX || MACH */
#endif /* SUNOS || LINUX || DYNIX || HPUX || AIX || CONVEXOS || AUX || SGI */

	struct stat	stat_info;
#if !defined (AIX) && !defined (ULTRIX) && !defined (OSF1) && \
    !defined (FreeBSD)
	if (!(stat (MOUNT_FILE, &stat_info)))
		last_mod_time = stat_info.st_mtime;
#endif /* !AIX && !ULTRIX && !OSF1 && !FreeBSD */

	init_mnt_table();

#if defined (SUNOS) || defined (LINUX) || defined (HPUX) || \
    defined (CONVEXOS) || defined (MACH) || defined (DYNIX) || \
    defined (AUX) || defined (SGI)
	if ((fp=setmntent(MOUNT_FILE,"r"))==NULL) return 0;
# else
# if defined (SVR4) || defined (SOLARIS)
	if ((fp=fopen(MOUNT_FILE,"r"))==NULL) return 0;
#  else
#  ifdef OSF1
	nstatfs = getfsstat((struct statfs *)0, 0, MNT_NOWAIT);
	if (nstatfs < 0) return 0;
	mnt_info = (struct statfs *)malloc(nstatfs * sizeof (struct statfs));
	if (mnt_info == (struct statfs *)0) return 0;
	nstatfs = getfsstat(mnt_info,nstatfs*sizeof(struct statfs),MNT_NOWAIT);
	if (nstatfs < 0) return 0;
#   else
#   ifdef FreeBSD
	nstatfs = getmntinfo(&mnt_info,MNT_NOWAIT);
	if (nstatfs < 0) return 0;
#    else
#    ifdef SCO
	if ((fp = popen ("/etc/mount", "r")) == NULL) return 0;
#    endif /* SCO */
#   endif /* FreeBSD */
#  endif /* OSF1 */
# endif /* SVR4 || SOLARIS */
#endif /* SUNOS || LINUX || HPUX || CONVEXOS || MACH || DYNIX || AUX || SGI */


#if defined (SUNOS) || defined (LINUX) || defined (HPUX) || \
    defined (CONVEXOS) || defined (MACH) || defined(DYNIX) || \
    defined (AUX) || defined (SGI)
	while ((mnt_info=getmntent(fp)))
# else
# if defined (SVR4) || defined (SOLARIS)
	while (!(getmntent (fp, &mnt_info)))
#  else
#  if defined (OSF1) || defined (FreeBSD)
	i=0;
	while (i < nstatfs)
#   else
#   ifdef AIX
	retcode=mntctl(MCTL_QUERY, sizeof(vmount_buf), vmount_buf);
	if (retcode==0) return 0;
	mnt_info=(struct vmount *)vmount_buf;
	while (i<retcode)
#    else
#    ifdef ULTRIX
	while (getmnt (&which_fs, &mnt_info, sizeof (struct mntent), 
			 NOSTAT_MANY, "no_path") > 0)
#     else
#     ifdef SCO
	while ((fgets (buf, BUFSZ , fp)) != NULL)
#     endif /* SCO */
#    endif /* ULTRIX */
#   endif /* AIX */
#  endif /* OSF1 || FreeBSD */
# endif /* SVR4 || SOLARIS */
#endif /* SUNOS || LINUX || HPUX || CONVEXOS || MACH || AUX || SGI */


		{
		/* *** here the actual while loop starts *** */
#if defined (SUNOS) || defined (LINUX) || defined (DYNIX) || \
    defined (HPUX) || defined (CONVEXOS) || defined (MACH) || \
    defined (AUX) || defined (SGI)
                strcpy(fs_name, mnt_info->mnt_dir);
		strcpy(dev_name, mnt_info->mnt_fsname);
# else
# if defined (SVR4) || defined (SOLARIS)
		strcpy(fs_name, mnt_info.mnt_dir);
		strcpy(dev_name, mnt_info.mnt_fsname);
#  else
#  if defined (OSF1) || defined (FreeBSD)
		strcpy(fs_name, mnt_info[i].f_mntonname);
		strcpy(dev_name, mnt_info[i].f_mntfromname);
		i++;
#   else
#   ifdef AIX
		strcpy(fs_name, vmt2dataptr(mnt_info,VMT_STUB));
		strcpy(dev_name,vmt2dataptr(mnt_info,VMT_OBJECT));
		i++;
		mnt_info=(struct vmount *)((char *) 
			mnt_info+mnt_info->vmt_length);
#    else
#    ifdef ULTRIX
		strcpy(fs_name, mnt_info.mnt_dir);
		strcpy(dev_name, mnt_info.mnt_fsname);
#     else
#     ifdef SCO
		sscanf (buf, "%s%*s%s", fs_name, dev_name);
#     endif /* SCO */
#    endif /* ULTRIX */
#   endif /* AIX */
#  endif /* OSF1 */
# endif /* SVR4 || SOLARIS */
#endif /* SUNOS || LINUX || DYNIX || HPUX || ULTRIX || CONVEXOS || MACH || 
          AUX || SGI */


		ignore = 0;
#if !defined (MACH) && !defined (AIX) && !defined (OSF1) && \
    !defined (SCO) && !defined (FreeBSD)
# ifdef ULTRIX
		if (mnt_info.mnt_type == fs_ignore)
			ignore = 1;
# else
#  if defined (SOLARIS) || defined (SVR4)
		if (strstr(mnt_info.mnt_opts, fs_ignore))
			ignore = 1;
#   else  
		if (!strcmp(mnt_info->mnt_type, fs_ignore))
			ignore = 1;
#  endif /* SOLARIS || SVR4 */
# endif /* ULTRIX */
#endif /* !MACH && !AIX && !OSF1 && !SCO && !FreeBSD */

		if (!ignore)
		  if (!add_mnt_entry(fs_name, dev_name))
		    return 0;
		/* *** end of while loop *** */
		}

#if defined (SUNOS) || defined (LINUX) || defined (HPUX) || \
    defined (CONVEXOS) || defined (MACH) || defined (AUX) || defined (SGI)
	endmntent(fp);
# else
# if defined (SVR4) || defined (SOLARIS) || defined (SCO)
	fclose(fp);
#  else
#  ifdef AIX
	free(vmount_buf);
#   else
#   ifdef OSF1
	free(mnt_info);
#   endif /* OSF1 */
#  endif /* AIX */
# endif /* SVR4 || SOLARIS */
#endif /* SUNOS || LINUX || HPUX || CONVEXOS || MACH || DYNIX || AUX || SGI */
}

int check_mnt_table(void)
{
	struct stat	stat_info;
#if !defined (AIX) && !defined (ULTRIX) && !defined (OSF1) && \
    !defined (FreeBSD)
	if (!(stat (MOUNT_FILE, &stat_info)))
	        return stat_info.st_mtime > last_mod_time;
	else
		return 1;
#else
                return 1;
#endif /* !AIX && !ULTRIX && !OSF1 && !FreeBSD */
}

int n_mnt_entries(void)
{
  return size;
}

char *mnt_fsname(int i)
{
  if (0 <= i && i < size)
    return fsnames[i];
  else
    return NULL;
}

char *mnt_devname(int i)
{
  if (0 <= i && i < size)
    return devnames[i];
  else
    return NULL;
}

int search_mnt_table(char *fsname)
{
  if (!fsname)
    return -1;
  else {
    int i, l = strlen(fsname);
    for (i = 0; i < size; i++)
      if (fsnames[i] && !strncmp(fsnames[i], fsname, l) &&
	  (fsnames[i][l] == 0 || fsnames[i][l] == '/'))
	return i;
    return -1;
  }
}
