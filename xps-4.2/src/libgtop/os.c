/* 
   Draw unix process tree in an X window.
   $Id: os.c,v 1.1 2002/12/01 23:52:55 rockyb Exp $

    Copyright (C) 1998-2002 Rocky Bernstein

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

/* global.h should be first include -- contains gnome.h, config.h, etc... */
#include "global.h"

#include "common.h"
#include "gnopstree.h"

#include <glibtop.h>
#include <glibtop/xmalloc.h>
#include <glibtop/union.h>

#ifdef HAVE_LINUX_TASKS_H
#include <linux/tasks.h>	/* for NR_TASKS */
#else
/*
  RH7 doesn't have /usr/include/linux/tasks.h.
*/
#define NR_TASKS 4096
#endif /* HAVE_LINUX_TASKS_H */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

pid_t MyPid;           /* my process id */

#ifdef GNOPSTREE_RECOLOR_LOGIN_PROCESSES      
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

#ifdef STANDALONE
/* Do things to simplify dependencies of this program on others. */
#define error_dialog(msg, rc) g_error(msg); exit(rc);
#undef HAVE_REGEXEC
#undef GNOPSTREE_RECOLOR_LOGIN_PROCESSES
GdkColor  selected_color;
#define GetUID_Color(pid) selected_color
#define allprocs 1
#define me_too 0
#define showKernel 0
#define IDtouser(pid) "root"
#define selected_pid -1
GdkColor  nodeColor[NPROCSTATES+1];
#endif /*STANDALONE*/

/***************************************************************************
 Process-specific routines for Linux
****************************************************************************/

/* Fill in process structure proc for pid */
static gboolean
read_one_proc_stat(unsigned pid, ProcInfo *proc)
{
  glibtop_union data;

  glibtop_get_proc_state (&data.proc_state, pid);
  proc->pr_pid   = pid;
  strncpy(proc->pr_fname, data.proc_state.cmd, sizeof(proc->pr_fname));

  proc->pr_sname = data.proc_state.state;
  proc->pr_uid   = data.proc_state.uid;

  glibtop_get_proc_uid (&data.proc_uid, pid);
  proc->pr_ppid = data.proc_uid.ppid;
  proc->pr_pgrp = data.proc_uid.pgrp;
  proc->pr_flag = data.proc_uid.flags;
  
  switch (proc->pr_sname)
    {
    case 'R': proc->pr_state = SRUN;  break;
    case 'S': proc->pr_state = SWAIT; break;
    case 'D': proc->pr_state = SIDL;  break;
    case 'Z': proc->pr_state = SZOMB; break;
    case 'T': proc->pr_state = SSTOP; break;
    case 'W': proc->pr_state = SSWAP; break;
    }
  return TRUE;
}

/* Fills in process table info into global Proc and sets NumProcs; */
unsigned int 
GetProcInfo(void)
{
  char msg[300];

  glibtop_proclist proclist;
  unsigned *ptr, pid, i;

  NumProcs = 0;
  ptr = glibtop_get_proclist (&proclist, 0, 0);
  if (!ptr) return(0);

  NumProcs = (unsigned long) proclist.number;

  for (i = 0; i < proclist.number; i++) {
    pid = ptr [i];
    if (read_one_proc_stat(pid, &(Proc[NumProcs]))) {
      if (NumProcs >= GNOPSTREE_MAX_PROCS) {
	const char fmt[300] = 
	  "Static-sized process table full.\n"
	  "Rebuild program setting GNOPSTREE_MAX_PROCS larger than %d.\n"
	  "You are currently have %d processes.\n";
	sprintf(msg, fmt, GNOPSTREE_MAX_PROCS, NumProcs);
	error_dialog(msg, 5);
      }
      NumProcs++;
    }
  }

  glibtop_free (ptr);
  return NumProcs;
}

/* Store node info: pid, ppid, color, ... */
gboolean
GetStatus(ProcInfo *proc)
{
  if (!allprocs) {
#ifdef HAVE_REGEXEC
    if (match_regexp) {
      int regmatch = regexec(re_pat, IDtouser(proc->pr_uid), 
			     (size_t) NULL, (regmatch_t *) NULL,
			     REG_NOTBOL|REG_NOTEOL);
      if (regmatch == REG_NOMATCH) {
	return FALSE;
      }
    } else {
#endif
      if (uid_displayed != proc->pr_uid)
	return FALSE;
#ifdef HAVE_REGEXEC
    }
#endif
  }

  /* root process */
  if (proc->pr_pid == ROOT_PID && showKernel == FALSE)
    return FALSE;
  
  EndNode->pid = proc->pr_pid;
  EndNode->ppid = proc->pr_ppid;

  /* I find the running light on me annoying; others prefer it.
     If my process id, fake my status or don't. */
  if (proc->pr_pid==MyPid && !me_too) {
    proc->pr_state = SSLEEP;
  }
  EndNode->color = nodeColor[(short int) proc->pr_state];
  return TRUE;
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

#ifdef GNOPSTREE_RECOLOR_LOGIN_PROCESSES      
    { 
      int i;
      for (i=0;i<NUMLABS ;i++ ) {
	if (strcmp(node->label, label_array[i]) == 0) {
	  strcpy(node->label, IDtouser(proc->pr_uid));
	  strcpy(node->label1, label_array[i]);
	  node->show_username = 1;
	  break;
	} /* endif */
      } /* endfor */
    }
#endif
    
    if ((proc->pr_pid == selected_pid)) {
      node->color = selected_color;
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
}

#ifdef STANDALONE

/* Number of processes we are working with. */
guint NumProcs;
ProcInfo *Proc;               /* Points to array of process information */

/* See gnopstree.h for a description of what these arrays do. */
TREENODE         Node[GNOPSTREE_MAX_PROCS];
TREENODE        *EndNode;             /* end marker */

int 
main() 
{
  guint NumProcs;
  ProcInfo *proc;
  guint cnt;
  
  machine_specific_init();

  NumProcs = GetProcInfo();
  EndNode = Node;

  if (NumProcs==0) {
    /* Nothing to print. Could be something wrong. */
    return 0;
  }
    
  /* extract live process table entries */
  memset(Node, 0, sizeof(struct node)*NumProcs);

  printf("S    UID    GID    PID  PPID  NI CMD ARGS\n");
  
  for (cnt = 0, proc = Proc; cnt < NumProcs; proc++, cnt++) {
    if (!GetStatus(proc))
      continue;
    GetProcName(proc, EndNode);
    printf("%1d %6d %6d %6d %5d %3d %s %s\n", proc->pr_state, proc->pr_uid, 
	   proc->pr_gid, proc->pr_pid, proc->pr_ppid, proc->pr_nice,
	   proc->pr_fname, proc->pr_psargs);
	   
    ++EndNode;
  }

  return 0;
}
#endif  /* STANDALONE */
