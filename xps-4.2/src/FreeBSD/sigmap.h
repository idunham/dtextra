/* Created by makesig.pl
   for NetBSD saturn 1.3I NetBSD 1.3I (GENERIC) #1: Tue Feb  9 10:44:01 JST 1999     root@saturn:/opt/src/sys/arch/i386/compile/GENERIC i386 
   on Mon Feb 22 08:43:36 1999 using:
   kill -l 
*/

#include <sys/signal.h>


#define MAX_SIGNAL_NAME 9
typedef
  struct {
    char     signal_name[MAX_SIGNAL_NAME];
    signal_t sig;
  } sigmap_el;

static sigmap_el signal_map[] =
{
  {  "NO ACTION",    	0},
#ifdef SIGHUP
  {  "HUP",          	SIGHUP},
#endif
#ifdef SIGTERM
  {  "TERM",         	SIGTERM},
#endif
#ifdef SIGKILL
  {  "KILL",         	SIGKILL},
#endif
#ifdef SIGSTOP
  {  "STOP",         	SIGSTOP},
#endif
#ifdef SIGCONT
  {  "CONT",         	SIGCONT},
#endif
#ifdef SIGQUIT
  {  "QUIT",         	SIGQUIT},
#endif
#ifdef SIGABRT
  {  "ABRT",         	SIGABRT},
#endif
#ifdef SIGALRM
  {  "ALRM",         	SIGALRM},
#endif
#ifdef SIGBUS
  {  "BUS",          	SIGBUS},
#endif
#ifdef SIGCHLD
  {  "CHLD",         	SIGCHLD},
#endif
#ifdef SIGEMT
  {  "EMT",          	SIGEMT},
#endif
#ifdef SIGFPE
  {  "FPE",          	SIGFPE},
#endif
#ifdef SIGILL
  {  "ILL",          	SIGILL},
#endif
#ifdef SIGINFO
  {  "INFO",         	SIGINFO},
#endif
#ifdef SIGINT
  {  "INT",          	SIGINT},
#endif
#ifdef SIGIO
  {  "IO",           	SIGIO},
#endif
#ifdef SIGPIPE
  {  "PIPE",         	SIGPIPE},
#endif
#ifdef SIGPROF
  {  "PROF",         	SIGPROF},
#endif
#ifdef SIGPWR
  {  "PWR",          	SIGPWR},
#endif
#ifdef SIGSEGV
  {  "SEGV",         	SIGSEGV},
#endif
#ifdef SIGSYS
  {  "SYS",          	SIGSYS},
#endif
#ifdef SIGTRAP
  {  "TRAP",         	SIGTRAP},
#endif
#ifdef SIGTSTP
  {  "TSTP",         	SIGTSTP},
#endif
#ifdef SIGTTIN
  {  "TTIN",         	SIGTTIN},
#endif
#ifdef SIGTTOU
  {  "TTOU",         	SIGTTOU},
#endif
#ifdef SIGURG
  {  "URG",          	SIGURG},
#endif
#ifdef SIGUSR1
  {  "USR1",         	SIGUSR1},
#endif
#ifdef SIGUSR2
  {  "USR2",         	SIGUSR2},
#endif
#ifdef SIGVTALRM
  {  "VTALRM",       	SIGVTALRM},
#endif
#ifdef SIGWINCH
  {  "WINCH",        	SIGWINCH},
#endif
#ifdef SIGXCPU
  {  "XCPU",         	SIGXCPU},
#endif
#ifdef SIGXFSZ
  {  "XFSZ",         	SIGXFSZ},
#endif
};
