/* Created by makesig.pl
   for SunOS milton 5.6 Generic_105181-03 sun4m sparc 
   on Sun Feb 28 11:21:27 1999 using:
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
#ifdef SIGUSR1
  {  "USR1",         	SIGUSR1},
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
#ifdef SIGCANCEL
  {  "CANCEL",       	SIGCANCEL},
#endif
#ifdef SIGCLD
  {  "CLD",          	SIGCLD},
#endif
#ifdef SIGEMT
  {  "EMT",          	SIGEMT},
#endif
#ifdef SIGEXIT
  {  "EXIT",         	SIGEXIT},
#endif
#ifdef SIGFPE
  {  "FPE",          	SIGFPE},
#endif
#ifdef SIGFREEZE
  {  "FREEZE",       	SIGFREEZE},
#endif
#ifdef SIGILL
  {  "ILL",          	SIGILL},
#endif
#ifdef SIGINT
  {  "INT",          	SIGINT},
#endif
#ifdef SIGLOST
  {  "LOST",         	SIGLOST},
#endif
#ifdef SIGLWP
  {  "LWP",          	SIGLWP},
#endif
#ifdef SIGPIPE
  {  "PIPE",         	SIGPIPE},
#endif
#ifdef SIGPOLL
  {  "POLL",         	SIGPOLL},
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
#ifdef SIGTHAW
  {  "THAW",         	SIGTHAW},
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
#ifdef SIGUSR2
  {  "USR2",         	SIGUSR2},
#endif
#ifdef SIGVTALRM
  {  "VTALRM",       	SIGVTALRM},
#endif
#ifdef SIGWAITING
  {  "WAITING",      	SIGWAITING},
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
