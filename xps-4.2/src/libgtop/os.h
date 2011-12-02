/* 
   $Id: os.h,v 1.1 2002/12/01 23:52:55 rockyb Exp $
   Linux-specifc stuff. 
*/ 
#ifndef OS_H
#define OS_H
#define NPROCSTATES 7
#define NCPUSTATES 4
#define SRUN  1  /* running */
#define SWAIT 2  
#define SIDL  3  
#define SZOMB 4  /* terminated process whose parent has seen via "wait" */
#define SSTOP 5  /* process stopped by debugger */  
#define SSWAP 6    

/* Alternate definitions for above */
#define	SACTIVE SRUN 
#define	SSLEEP  SWAIT

/* What status (or "ps") command should be used used to show status? */
#define PS_CMDINFO "/bin/ps up %d"

/* What status (or "ps") command should be used to show command arguments? 
   The 'sed' removes the header line.
*/
#define PS_CMDLINE "/bin/ps w %d | /bin/sed -n '2,$p'"

/* What action command (or "kill") should be run for quick action? */
#define QUICK_CMDLINE "/bin/kill %d"

/* This program can't show kernel procs, yet... */
#undef GNOPSTREE_HAVE_KERNEL_PROCS 

#include <sys/procfs.h>
typedef prpsinfo_t ProcInfo;

/* Should've been defined somewhere like in /usr/include/sys/types.h */
typedef int signal_t;
#endif /*OS_H*/
