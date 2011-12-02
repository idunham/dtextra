/* 
   BSDI-specifc definitions. 
   $Id: os.h,v 1.1 2002/12/02 15:45:51 rockyb Exp $
*/ 
#ifndef OS_H
#define OS_H

#define	NPROCSTATES 7

/* Culled from #include <sys/proc.h> */
#define SIDL    1               /* Process being created by fork. */
#define SRUN    2               /* Currently runnable. */
#define SSLEEP  3               /* Sleeping on an address. */  
#define SSTOP   4               /* Process debugging or suspension. */
#define SZOMB   5               /* Awaiting collection by parent. */
/* swap */
#define	SSWAP	6

/* Alternate definitions for above */
#define	SACTIVE SRUN 
#define	SWAIT	SIDL

/*
 * Process priority specifications to get/setpriority.
 */
#define	PRIO_MIN	-20
#define	PRIO_MAX	 20

/* What "ps" command should be used used to show status? */
#define PSCMD "/bin/ps -w -w -o ucomm,user,nice,pcpu,pmem,vsz,rss,pri,ppid,pid -p %ld"

/* What "ps" command should be used to show command arguments? 
   The 'sed' removes the header line.
*/
#define PS_CMDLINE "/bin/ps -w -w -o command -p %ld | /usr/bin/sed -n '2,$p'"

/* This program can show kernel procs */
#define XPS_HAVE_KERNEL_PROCS 

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <sys/types.h>
#include <sys/time.h>
#include "kvm.h"
typedef struct kinfo_proc *ProcInfo;

typedef int signal_t;

#endif /*OS_H*/
