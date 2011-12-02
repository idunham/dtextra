/* 
   Draw unix process tree in an X window.
   $Id: os.c,v 1.2 2003/04/24 00:21:29 rockyb Exp $

    Copyright (C) 1998-2001 Rocky Bernstein

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

#include "user_config.h"
#include "os.h"
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/vfs.h>
#include <sys/stat.h>

#ifdef HAVE_LINUX_TASKS_H
#include <linux/tasks.h>	/* for NR_TASKS */
#else
#define NR_TASKS 5000
#endif /* HAVE_LINUX_TASKS_H */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include "common.h"
#include "xstuff.h"
#include "xps.h"
#include "askquestion.h"

#include <dirent.h>
#include <fcntl.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

/* Where we go to get process table info. */
#define PROCFS "/proc"

pid_t MyPid;           /* my process id */

#ifdef XPS_RECOLOR_LOGIN_PROCESSES      
#define NUMLABS 6
static char label_array[NUMLABS][16] = {
            "rxvt",
            "xterm",
            "rlogind",
            "rshd",
            "telnetd",
            "ftpd",
};
#endif

#define min(a,b) ((a<b)?a:b)

/***************************************************************************
 Process specific routines for Linux
****************************************************************************/

/*
 * proc_owner(pid) - returns the uid that owns process "pid", or -1 if
 *              the process does not exist.
 *              It is EXTREMLY IMPORTANT that this function work correctly.
 *              If top runs setuid root (as in SVR4), then this function
 *              is the only thing that stands in the way of a serious
 *              security problem.  It validates requests for the "kill"
 *              and "renice" commands.
 */

uid_t
proc_owner(const pid_t pid)
{
    struct stat sb;
    char buffer[32];
    sprintf(buffer, "%s/%d", PROCFS, pid);

    if (stat(buffer, &sb) < 0)
	return -1;
    else
	return sb.st_uid;
}

static Boolean
read_one_proc_stat(const char *pid, ProcInfo *proc)
{
  char  buffer[4096];

  /* grab the proc stat info in one go */
  {
    int fd, len;
    
    sprintf(buffer, "%s/stat", pid);
    
    fd = open(buffer, O_RDONLY);
    if (fd == -1) {
      fprintf(stderr, "%s: Couldn't open process status file %s: %s\n", 
	      base.program, buffer, strerror(errno));
      return False;
    }
    if (-1 == (len=read(fd, buffer, sizeof(buffer)-1))) {
      fprintf(stderr, "%s: Couldn't read process status file %s: %s\n", 
	      base.program, buffer, strerror(errno));
      return False;
    }
    close(fd);
    
    buffer[len] = '\0';
  }
  
  /* split into "PID (cmd" and "<rest>" */    
  if ( sscanf(buffer, "%d (%s", &proc->pr_pid, proc->pr_fname) < 2) {
    fprintf(stderr, "%s: Couldn't parse pid and name\n", base.program);
    return False;
  }

  /* Remove trailing ) from process named. sscanf is funny with respect
     to having it do this. */
  {
    char *pname=proc->pr_fname;
    size_t l=strlen(pname)-1;
    if (pname[l]==')') {
      pname[l]=0;
    }
  }
  
  {
    char *rest;
    int junk;     /* Place to store garbage from sscanf */
    long idjunk;  /* Place to store garbage from sscanf */
    rest = strrchr(buffer, ')'); 
    if ( sscanf(rest+2,  /* Part after ') ' */ 
	   "%c %d %d %d %d %lu %lu %lu ",
	   &proc->pr_sname, 
	   &proc->pr_ppid, 
	   &proc->pr_pgrp, 
	   &proc->pr_sid, 
	   &junk, /*tty*/ 
	   &idjunk, /*tpuid? */
	   &idjunk, /*tpgid */
	   &proc->pr_flag
	   ) < 8 ) {
      fprintf(stderr, "%s: Couldn't parse process %s (%ld) status info\n", 
	      base.program, proc->pr_fname, (long) proc->pr_pid );
      return False;
    }
    
    switch (proc->pr_sname)
      {
      case 'R': proc->pr_state = SRUN;  break;
      case 'S': proc->pr_state = SWAIT; break;
      case 'D': proc->pr_state = SIDL;  break;
      case 'Z': proc->pr_state = SZOMB; break;
      case 'T': proc->pr_state = SSTOP; break;
      case 'W': proc->pr_state = SSWAP; break;
      }
    proc->pr_uid = proc_owner(proc->pr_pid);
  }
  return True;
}

/* Fills in process table info into global Proc and sets NumProcs; */
unsigned int 
GetProcInfo(void)
{
  DIR *dirfp = opendir(".");
  struct dirent *dp;

  NumProcs = 0;
  
  if (dirfp == NULL) {
    fprintf(stderr, "%s: Can't open %s: %s\n", base.program, 
	    PROCFS, strerror(errno));
    exit(10);
  }
  
  while ((dp = readdir(dirfp)) != NULL) {
    if (!isdigit(dp->d_name[0]))
      continue;
    
    if (NumProcs >= NR_TASKS) {
      fprintf(stderr, "%s: static-sized process table full.\n", 
	      base.program);
      exit(5);
    }
    if (read_one_proc_stat(dp->d_name, &(Proc[NumProcs]))) {
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
      NumProcs++;
    }
    
  }
  (void) closedir(dirfp);
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
  
  EndNode->pid = proc->pr_pid;
  EndNode->ppid = proc->pr_ppid;

  /* I find the running light on me annoying; others prefer it.
     If my process id, fake my status or don't. */
  if (proc->pr_pid==MyPid && !cmd_options.me_too) {
    proc->pr_state = SSLEEP;
  }
  EndNode->color = base.nodeColor[proc->pr_state];
  return True;
}

void 
GetProcName(ProcInfo *proc, TREENODE *node)
{
  node->uid = proc->pr_uid;
  
  if (proc->pr_state == SZOMB)
    strcpy(node->label, "*zombie*");
  else if (proc->pr_pid == ROOT_PID)
    strcpy(node->label, "*root*");
  else  {
    char *username;
    int i;

    strncpy(node->label, proc->pr_fname, 16);
    if (proc->pr_state != SSLEEP) return;
    
    /* We've got a sleeping process. (Hopefully that's the majority of 
       the processes. Assign a color based on userid. */
    node->color = GetUID_Color(proc->pr_uid);
    
    if ( (username=IDtouser(proc->pr_uid)) != NULL ) {
      strcpy(node->label1, username);
    } else {
      strcpy(node->label1, UNKNOWN_USER);
    }

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

#define PROC_SUPER_MAGIC 0x9fa0

/* Initializations that might be specific to a given platform. 
   See also common_init() for cross-the-board initializations. */
void 
machine_specific_init(void)
{
  /* The ProcInfo structure is platform specific. */
  Proc  = (ProcInfo *) malloc(sizeof(ProcInfo) * NR_TASKS);
  
  /* make sure the proc filesystem is mounted */
  {
    struct statfs sb;
    if (statfs(PROCFS, &sb) < 0 || sb.f_type != PROC_SUPER_MAGIC)
      {
	fprintf(stderr, "%s: proc filesystem not mounted on %s\n",
		 PROCFS, base.program);
	exit(10);
      }
  }
  
  /* chdir to the proc filesystem to simplify reading process directory. */
  chdir(PROCFS);
}
