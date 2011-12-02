/* 
   $Id: os.h,v 1.1 2002/12/02 14:55:18 rockyb Exp $ 
   AIX-specific definitions.
*/ 
#ifndef OS_H
#define OS_H
#include <sys/proc.h>
#define NPROCSTATES 7
#define SWAIT 2  
#ifndef SRUN
#define SRUN  3
#endif

/* What "ps" command should be used to show status? */
#define PSCMD "/bin/ps -F 'user pid ppid group tty st time etime pcpu pmem pricp vsz scount' -p %d"

/* What "ps" command should be used to show command arguments? 
   The 'sed' removes the header line.
*/
#define PS_CMDLINE "/bin/ps -F args -p %d | /bin/sed -n '2,$p'"

#include <procinfo.h>
typedef struct procinfo ProcInfo;

/* Maximum number entries in the process table */
#define NR_TASKS 1000

/* This program can show kernel procs */
#define XPS_HAVE_KERNEL_PROCS 
#endif /*OS_H*/
