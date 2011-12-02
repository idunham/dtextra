/* 
   Solaris-specifc definitions. 
   $Id: os.h,v 1.1 2002/12/02 15:25:45 rockyb Exp $
*/ 
#ifndef OS_H
#define OS_H
#define NPROCSTATES 8
#define NCPUSTATES 4

/* Culled from #include <sys/proc.h> */
#define	SSLEEP	1		/* awaiting an event */
#define	SRUN	2		/* running */
#define	SZOMB	3		/* process terminated but not waited for */
#define	SSTOP	4		/* process stopped by debugger */
#define	SIDL	5		/* intermediate state in process creation */
#define	SONPROC	6		/* process is being run on a processor */
#define	SSWAP	7	

/* Alternate definitions for above */
#define	SACTIVE SRUN 
#define	SWAIT	SIDL

/*
 * Process priority specifications to get/setpriority.
 */
#define	PRIO_MIN	-20
#define	PRIO_MAX	 20

/* What "ps" command should be used used to show status? */
#define PSCMD "/bin/ps -f -o user,pid,ppid,tty,nice,pcpu,pmem,vsz,rss,class -p %ld"

/* What "ps" command should be used to show command arguments? 
   The 'sed' removes the header line.
*/
#define PS_CMDLINE "/bin/ps -o args -p %ld | /bin/sed -n '2,$p'"

/* This program can show kernel procs */
#define XPS_HAVE_KERNEL_PROCS 

#ifdef SOLARIS_NEW_PROC
#if defined(NEED_STDC_DEFINE) && __STDC__ != 0
/* 
   Solaris 2.6 and egcs-2.90.23 980102 (egcs-1.0.1 release)
   you will get parse errors because sigset_t is not defined.
   Casper H.S. Dik reports the meaning of the values of __STDC_ is:
    - not defined           - K&R mode (not prototypes, const, etc)
    - defined as 0          - ANSI + extensions (long long)
    - defined as 1          - strict ANSI (no long long, but also
                              no non-ANSI symbols visible in ANSI
                              include files.
 */
#define __STDC__ 0
#endif
#define _STRUCTURED_PROC 1
#define prpsinfo psinfo
#endif
#include <sys/procfs.h>
typedef struct prpsinfo ProcInfo;

/* Should've been defined somewhere like in /usr/include/sys/types.h */
typedef int signal_t;

/* Maximum number entries in the process table */
#define NR_TASKS XPS_MAX_PROCS
#endif /*OS_H*/
