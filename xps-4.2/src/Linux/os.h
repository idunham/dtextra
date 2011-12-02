/* 
   $Id: os.h,v 1.1 2002/12/02 15:34:14 rockyb Exp $
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

/* What "ps" command should be used used to show status? */
#define PSCMD "/bin/ps up %d"

/* What "ps" command should be used to show command arguments? */
#define PS_CMDLINE "/bin/ps w %d | /bin/sed -n '2,$p'"

/* This program can't show kernel procs, yet... */
#undef XPS_HAVE_KERNEL_PROCS 

#include <sys/procfs.h>
typedef prpsinfo_t ProcInfo;

/* Should've been defined somewhere like in /usr/include/sys/types.h */
typedef int signal_t;
#endif /*OS_H*/
