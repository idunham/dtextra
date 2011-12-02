/* 
    Draw unix process tree in an X window.
    $Id: os.c,v 1.1 2002/12/02 15:25:45 rockyb Exp $

    Copyright (C) 1998,99 Rocky Bernstein

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include "config.h"
#include "os.h"
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/param.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include "common.h"
#include "xstuff.h"
#include "xps.h"
#include "askquestion.h"

#include <sys/proc.h>
#include <dirent.h>
#include <pwd.h>
#include <fcntl.h>
#define PROCFS "/proc"

static char  nodename[];      /* name of the local system */
pid_t MyPid;           /* my process id */

#ifdef XPS_RECOLOR_LOGIN_PROCESSES      
#define NUMLABS 7
static char label_array[NUMLABS][16] = {
            "rxvt",
            "xterm",
            "rlogind",
            "dtterm",
            "rshd",
            "telnetd",
            "ftpd",
};
#endif

#define min(a,b) ((a<b)?a:b)

/****************************************************************************
 Process specific routines for Sun Solaris
*****************************************************************************/

/*
 * GetProc()
 *
 * Fill in and return a `struct ProcInfo' with information about the
 * next process.  If no processes are left, return NULL.
 */
struct prpsinfo *
GetProc(void)
{
  extern char *SyErr();
  static DIR *dirfp = NULL;
  static struct prpsinfo pinfo;
  struct dirent *dp;
  char flnm[FILENAME_MAX];
  int fd;
  
  /*
   * If this is our first time here, open the proc directory,...
   */
  if (dirfp == NULL && (dirfp=opendir(".")) == NULL) {
    fprintf(stderr, "%s: %s: %s\n", base.program, 
	    PROCFS, strerror(errno));
    exit(10);
  }
  
  while ((dp = readdir(dirfp)) != NULL) {
    if (strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0)
      continue;
#ifdef SOLARIS_NEW_PROC
    (void) sprintf(flnm,"./%s/psinfo", dp->d_name);
#else        
    (void) sprintf(flnm, "./%s", dp->d_name);
#endif
    if ((fd = open (flnm, O_RDONLY)) < 0)
      /* Sorry, couldn't get any info. */
      continue;

#ifdef SOLARIS_NEW_PROC
    if (read(fd, &pinfo, sizeof(psinfo_t)) != sizeof(psinfo_t))
      {
	(void) close (fd);
	continue;
      }
#else 
    ioctl(fd, PIOCPSINFO, &pinfo);
#endif
    (void) close(fd);
    return(&pinfo);
  }
  
  (void) closedir(dirfp);
  dirfp = NULL;
  return((struct prpsinfo *)NULL);
}

/* Fills in process table info into global Proc and sets NumProcs; */
unsigned int 
GetProcInfo(void)
{
  struct prpsinfo *p;

  NumProcs = 0;

  while ((p=GetProc()) != NULL)
  {
    if (NumProcs >= XPS_MAX_PROCS) {
      char msg[300];
      const char fmt[300] = 
	"Static-sized process table full.\n"
	"Rebuild program setting XPS_MAX_PROCS larger than %d.\n"
	"You are currently have %d processes.\n";
      sprintf(msg, fmt, XPS_MAX_PROCS, NumProcs);
      ShowDialog (active.shell, XmDIALOG_ERROR, msg);
      exit(5);
    }
    Proc[NumProcs] = *p;
    NumProcs++;
  }
  return NumProcs;
}

/* Store node info: pid, ppid, color, ... */
Boolean
GetStatus(ProcInfo *proc)
{
  if (!active.allprocs) {
#ifdef HAVE_REGEX
    if (active.match_regexp) {
      int regmatch = regexec(&active.re_pat, IDtouser(proc->pr_uid), 
			     (size_t) NULL, (regmatch_t *) NULL,
			     REG_NOTBOL|REG_NOTEOL);
      if (regmatch == REG_NOMATCH) {
	return False;
      }
    } else {
#endif
      if (active.uid_displayed != proc->pr_uid)
	return False;
#ifdef HAVE_REGEX
    }
#endif
  }

  /* root process */
  if (proc->pr_pid == ROOT_PID && base.showKernel == False)
    return False;

  if (proc->pr_pid != INIT_PID && base.showKernel == False   /* kernel process */
          && (proc->pr_flag & SIDL))
    return False;
  
  EndNode->pid = proc->pr_pid;
  EndNode->ppid = proc->pr_ppid;
#ifdef SOLARIS_NEW_PROC
  EndNode->color = base.nodeColor[proc->pr_pid == MyPid && !cmd_options.me_too
				  ? SSLEEP : proc->pr_lwp.pr_state];
#else 
  EndNode->color = base.nodeColor[proc->pr_pid == MyPid 
				  ? SSLEEP : proc->pr_state];
#endif
  return True;
}


void 
GetProcName(ProcInfo *proc, TREENODE *node)
{
  node->uid = proc->pr_uid;
  
#ifdef SOLARIS_NEW_PROC
  if (proc->pr_lwp.pr_state == SZOMB)
#else
  if (proc->pr_state == SZOMB)
#endif
    strcpy(node->label, "*zombie*");
  else if (proc->pr_pid == ROOT_PID)
    strcpy(node->label, "*root*");
  else if (proc->pr_flag & SIDL)
    strcpy(node->label, "*kernel*");
  else  {
      char *username;
      int i;
      
      strncpy(node->label, proc->pr_fname, 16);
#ifdef SOLARIS_NEW_PROC
      if ((proc->pr_pid != MyPid) && (proc->pr_lwp.pr_state != SSLEEP)) return;
#else
      if ((proc->pr_pid != MyPid) && (proc->pr_state != SSLEEP)) return;
#endif

      /* We've got a sleeping process. (Hopefully that's the majority of 
	 the processes. Assign a color based on userid. */
      node->color = GetUID_Color(proc->pr_uid);
      
      if ( (username=IDtouser(proc->pr_uid)) != NULL ) {
	strcpy(node->label1, username);
      } else {
	strcpy(node->label1, UNKNOWN_USER);
      }

      node->show_username = 0;
      
#ifdef XPS_RECOLOR_LOGIN_PROCESSES      
      for (i=0;i<NUMLABS ;i++ ) {
	if (strcmp(node->label, label_array[i]) == 0) {
	  strcpy(node->label, IDtouser(proc->pr_uid));
	  strcpy(node->label1, label_array[i]);
	  node->color = base.xterm;
	  node->show_username = 1;
	  break;
	} /* endif */
      } /* endfor */
#endif

      if ((proc->pr_pid == base.selected_pid)) {
	node->color = base.selected_color;
      }

    }
}

/* Initializations that might be specific to a given platform. 
   See also common_init() for cross-the-board initializations. */
void 
machine_specific_init(void)
{
  /* The ProcInfo structure is platform specific. */
  Proc  = (ProcInfo *) malloc(sizeof(ProcInfo) * NR_TASKS);

  /* chdir to the proc filesystem to simplify reading process directory. */
  chdir(PROCFS);

}
