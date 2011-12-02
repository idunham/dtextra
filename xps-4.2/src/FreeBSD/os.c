/* 
    Draw unix process tree in an X window.
    $Id: os.c,v 1.1 2002/12/01 23:56:11 rockyb Exp $

    Copyright (C) 1999 Kazuki Sakamoto  for NetBSD
    Copyright (C) 1998,1999,2001 Rocky Bernstein

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
#include <fcntl.h>
#include <limits.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include "common.h"
#include "xstuff.h"
#include "xps.h"
#include "askquestion.h"

pid_t MyPid;           /* my process id */

#define NUMLABS 8
static char label_array[NUMLABS][16] = {
            "rxvt",
            "xterm",
            "rlogind",
            "rsh",
            "sshd",
            "rshd",
            "telnetd",
            "ftpd",
};

/* Fills in process table info into global Proc and sets NumProcs; */

/*  Bruce Momjian <root@candle.pha.pa.us> reports: 
    On BSDI 4.01 when you do a kvm_close() the kp_eproc structure
    becomes inaccessible for certain processes, perhaps ones that have
    been swapped out.  So kvm_open() on the first call to GetProcInfo,
    and keep that kernel descriptor in a static variable between
    calls; don't close it.  

    Kazuki Sakamoto says that he thinks this is relevant for NetBSD,
    too.  */

unsigned int 
GetProcInfo(void)
{
  static kvm_t *kd = NULL;
  struct kinfo_proc *kp;
  char errbuf[_POSIX2_LINE_MAX];
  int nentries;

  NumProcs = 0;
  
  if (kd == NULL)
	kd = kvm_openfiles(NULL, NULL, NULL, O_RDONLY, errbuf);
  if (kd == NULL) {
    char msg[_POSIX2_LINE_MAX * 2];

    sprintf(msg, "kvm_openfiles failed: %s\n", errbuf);
    ShowDialog (active.shell, XmDIALOG_ERROR, msg);
    exit(5);
  }

#ifdef OpenBSD
  kp = kvm_getprocs(kd, KERN_PROC_KTHREAD, 0, &nentries);
#else
  kp = kvm_getprocs(kd, KERN_PROC_ALL, 0, &nentries);
#endif
  if (kp == NULL) {
    char msg[_POSIX2_LINE_MAX * 2];

    sprintf(msg, "kvm_getprocs failed: %s\n", kvm_geterr(kd));
    ShowDialog (active.shell, XmDIALOG_ERROR, msg);
    (void)kvm_close(kd);
    exit(5);
  }
  if (nentries >= XPS_MAX_PROCS) {
    char msg[300];

    const char fmt[300] = 
	"Static-sized process table full.\n"
	"Rebuild program setting XPS_MAX_PROCS larger than %d.\n"
	"You are currently have %d processes.\n";
    sprintf(msg, fmt, XPS_MAX_PROCS, NumProcs);
    ShowDialog (active.shell, XmDIALOG_ERROR, msg);
    (void)kvm_close(kd);
    exit(5);
  }

  for (; --nentries >= 0;) {
    Proc[NumProcs++] = kp++;
  }

  return NumProcs;
}

/* Store node info: pid, ppid, color, ... */
Boolean
GetStatus(ProcInfo *p)
{
  ProcInfo proc = *p;
  char pstat;

  if (!active.allprocs) {
#ifdef HAVE_REGEX
    if (active.match_regexp) {
      int regmatch = regexec(&active.re_pat, 
			     IDtouser(proc->kp_eproc.e_pcred.p_ruid), 
			     (size_t) NULL, (regmatch_t *) NULL,
			     REG_NOTBOL|REG_NOTEOL);
      if (regmatch == REG_NOMATCH) {
	return False;
      }
    } else {
#endif
      if (active.uid_displayed != proc->kp_eproc.e_pcred.p_ruid)
	return False;
#ifdef HAVE_REGEX
    }
#endif
}

  /* root process */
  if (proc->kp_proc.p_pid == ROOT_PID && base.showKernel == False)
    return False;

  /* 
    Chris Torek <torek@elf.bsdi.com> suggests for BSDI 
    with confirmation by Kazuki Sakamoto:

    the P_SYSTEM flag is probably close enough to what you want.

    Alternatives:
    Pids 0 and 2 are the swapper and page daemon; pid 1 is init,  
    pid 3 is reaper. Could hard code these numbers.

    See also the comments in BSDI/xps.c.  */

  if (proc->kp_proc.p_flag & P_SYSTEM && base.showKernel == False)
    /* a "kernel" process */
    return False;
  
  EndNode->pid = proc->kp_proc.p_pid;
  EndNode->ppid = proc->kp_eproc.e_ppid;

  if (proc->kp_proc.p_pid == MyPid && !cmd_options.me_too)
    pstat = SSLEEP;
  else if ((proc->kp_proc.p_flag & P_INMEM) == 0)
    pstat = SSWAP;
  else
    pstat = proc->kp_proc.p_stat;

  EndNode->color = base.nodeColor[pstat];
  return True;
}

void 
GetProcName(ProcInfo *p, TREENODE *node)
{
  ProcInfo proc = *p;
  node->uid = proc->kp_eproc.e_pcred.p_ruid;

  if (proc->kp_proc.p_stat == SZOMB)
    strcpy(node->label, "*zombie*");
  else if (proc->kp_proc.p_pid == ROOT_PID)
    strcpy(node->label, "*root*");
  else if (proc->kp_proc.p_flag & SIDL)
    strcpy(node->label, "*kernel*");
  else  {
      char *username;
      int i;

      strncpy(node->label, proc->kp_proc.p_comm, 16);
      if ((proc->kp_proc.p_pid != MyPid) && (proc->kp_proc.p_stat != SSLEEP)) return;

      /* We've got a sleeping process. (Hopefully that's the majority of 
	 the processes. Assign a color based on userid. */
      node->color = GetUID_Color(proc->kp_eproc.e_pcred.p_ruid);
 
      if ( (username=IDtouser(proc->kp_eproc.e_pcred.p_ruid)) != NULL ) {
	strcpy(node->label1, username);
      } else {
	strcpy(node->label1, UNKNOWN_USER);
      }

      node->show_username = 0;
      
#ifdef XPS_RECOLOR_LOGIN_PROCESSES      
      for (i=0;i<NUMLABS ;i++ ) {
	if (strcmp(node->label, label_array[i]) == 0) {
	  strcpy(node->label, IDtouser(proc->kp_eproc.e_pcred.p_ruid));
	  strcpy(node->label1, label_array[i]);
	  node->color = base.xterm;
	  node->show_username = 1;
	  break;
	} /* endif */
      } /* endfor */
#endif

      if ((proc->kp_proc.p_pid == base.selected_pid)) {
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
  Proc  = (ProcInfo *) malloc(sizeof(ProcInfo) * XPS_MAX_PROCS);
}
