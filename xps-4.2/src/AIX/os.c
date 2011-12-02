/* 
    Draw unix process tree in an X window.
    $Id: os.c,v 1.1 2002/12/02 14:55:18 rockyb Exp $

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
#include <ctype.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/param.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include "common.h"
#include "xstuff.h"
#include "xps.h"
#include "loadavg.h"
#include "askquestion.h"

#ifdef HAVE_USERSEC_H
#include <usersec.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <errno.h>

pid_t MyPid;           /* my process id */

#ifdef XPS_RECOLOR_LOGIN_PROCESSES      
#define NUMLABS 8
static char label_array[NUMLABS][16] = {
            "rxvt",
            "aixterm",
            "dtterm",
            "xterm",
            "rlogind",
            "rshd",
            "telnetd",
            "ftpd",
};
#endif

#define min(a,b) ((a<b)?a:b)

/***************************************************************************
 Process specific routines for AIX RS/6000
****************************************************************************/

const int multiplier = 1.5;
static unsigned int max_procs=0;

/* Fills in process table info into global Proc and sets NumProcs; */
unsigned int
GetProcInfo(void)
{

  NumProcs = 0;

  while (((NumProcs = getproc(Proc, NumProcs, sizeof(ProcInfo))) == -1) &&
         errno == ENOSPC)
  {
    NumProcs = *(long *) Proc;        /* num of active proc structures */
    if (NumProcs > max_procs) {
      if (NumProcs > XPS_MAX_PROCS) {
	char msg[300];
	const char fmt[300] = 
	  "Static-sized process table full.\n"
	  "Rebuild program setting XPS_MAX_PROCS larger than %d.\n"
	  "You are currently have %d processes.\n";
	sprintf(msg, fmt, XPS_MAX_PROCS, NumProcs);
	ShowDialog (active.shell, XmDIALOG_ERROR, msg);
	fprintf(stderr, "%s: %s", base.program, msg);
	exit(5);
      }
      
      /* get a little extra in case busy system */
      max_procs *= 1.5;
      Proc = (ProcInfo *) realloc(Proc, sizeof(ProcInfo) * max_procs);
      if (Proc == NULL) {
	char msg[300] = "Ran out of memory loading process table.\n";
	fprintf(stderr, "%s: %s", base.program, msg);
	ShowDialog (active.shell, XmDIALOG_ERROR, msg);
	exit(1);
      }
      NumProcs = max_procs;
    }

  }
  return NumProcs;
}

/* Store node info: pid, ppid, color, ... */
Boolean
GetStatus(ProcInfo *proc)
{
  if (proc->pi_stat == SNONE)    /* empty table entry */
    return False;

  if (!active.allprocs) {
#ifdef HAVE_REGEX
    if (active.match_regexp) {
      int regmatch = regexec(&active.re_pat, IDtouser(proc->pi_uid), 
			     (size_t) NULL, (regmatch_t *) NULL,
			     REG_NOTBOL|REG_NOTEOL);
      if (regmatch == REG_NOMATCH) {
	return False;
      }
    } else {
#endif
      if (active.uid_displayed != proc->pi_uid)
	return False;
#ifdef HAVE_REGEX
    }
#endif
  }
  
  /* root process */
  if (proc->pi_pid == ROOT_PID && base.showKernel == False)
    return False;

   /* kernel process */
  if (proc->pi_pid != INIT_PID && base.showKernel == False
          && (proc->pi_flag & SKPROC))
    return False;
  
  EndNode->pid = proc->pi_pid;
  EndNode->ppid = proc->pi_ppid;

  /* If my processes id fake my status */
  if (proc->pi_pid == MyPid && !cmd_options.me_too) {
    proc->pi_stat = SSLEEP;
  }
  EndNode->color = base.nodeColor[proc->pi_stat];
  return True;
}

void 
GetProcName(ProcInfo *proc, TREENODE *node)
{
  node->uid = proc->pi_uid;
  
  if (proc->pi_stat == SZOMB)
    strcpy(node->label, "*zombie*");
  else if (proc->pi_pid == ROOT_PID)
    strcpy(node->label, "*root*");
  else if (proc->pi_flag & SKPROC)
    strcpy(node->label, "*kernel*");
  else  {
      char argbuf[NCARGS];    /* = 24576.  Used to be literal 256 */
      char *username;
      int i;
      
      if (getargs(proc, sizeof(ProcInfo), argbuf, sizeof(argbuf)) != 0)
	strcpy(node->label, "*noname*");
      else {
	for (i=strlen(argbuf); i>0; i--)
	  if (argbuf[i] == '/') { i++; break; }
            strncpy(node->label, &argbuf[i], 16);
      }
      if (proc->pi_stat != SSLEEP) return;

      /* We've got a sleeping process. (Hopefully that's the majority of 
	 the processes. Assign a color based on userid. */
      node->color = GetUID_Color(proc->pi_uid);
      
      if ( (username=IDtouser(proc->pi_uid)) != NULL ) {
	strcpy(node->label1, username);
      } else {
	strcpy(node->label1, UNKNOWN_USER);
      }
      
      node->show_username = 0;

#ifdef XPS_RECOLOR_LOGIN_PROCESSES      
      for (i=0;i<NUMLABS ;i++ ) {
	if (strcmp(node->label, label_array[i]) == 0) {
	  strcpy(node->label, node->label1);
	  strcpy(node->label1, label_array[i]);
	  node->color = base.xterm;
	  node->show_username = 1;
	  break;
	} /* endif */
      } /* endfor */
#endif

      if ((proc->pi_pid == base.selected_pid)) {
	node->color = base.selected_color;
      }

      return;
      
    }
}

/* Initializations that might be specific to a given platform. 
   See also common_init() for cross-the-board initializations. */
void 
machine_specific_init(void)
{
  loadavg_init();
  /* The ProcInfo structure is platform specific. */
  Proc  = (ProcInfo *) malloc(sizeof(ProcInfo) * NR_TASKS);
  max_procs = min(NR_TASKS, XPS_MAX_PROCS);
}
