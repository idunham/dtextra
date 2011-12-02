/* Created by makesig.pl
   for AIX buster 2 4 000191924600 
   on Sun Feb 28 11:16:45 1999 using:
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
#ifdef SIGALRM
  {  "ALRM",         	SIGALRM},
#endif
#ifdef SIGBUS
  {  "BUS",          	SIGBUS},
#endif
#ifdef SIGCHLD
  {  "CHLD",         	SIGCHLD},
#endif
#ifdef SIGDANGER
  {  "DANGER",       	SIGDANGER},
#endif
#ifdef SIGEMT
  {  "EMT",          	SIGEMT},
#endif
#ifdef SIGFPE
  {  "FPE",          	SIGFPE},
#endif
#ifdef SIGGRANT
  {  "GRANT",        	SIGGRANT},
#endif
#ifdef SIGILL
  {  "ILL",          	SIGILL},
#endif
#ifdef SIGINT
  {  "INT",          	SIGINT},
#endif
#ifdef SIGIO
  {  "IO",           	SIGIO},
#endif
#ifdef SIGIOT
  {  "IOT",          	SIGIOT},
#endif
#ifdef SIGMIGRATE
  {  "MIGRATE",      	SIGMIGRATE},
#endif
#ifdef SIGMSG
  {  "MSG",          	SIGMSG},
#endif
#ifdef SIGNULL
  {  "NULL",         	SIGNULL},
#endif
#ifdef SIGPIPE
  {  "PIPE",         	SIGPIPE},
#endif
#ifdef SIGPRE
  {  "PRE",          	SIGPRE},
#endif
#ifdef SIGPROF
  {  "PROF",         	SIGPROF},
#endif
#ifdef SIGPWR
  {  "PWR",          	SIGPWR},
#endif
#ifdef SIGRETRACT
  {  "RETRACT",      	SIGRETRACT},
#endif
#ifdef SIGSAK
  {  "SAK",          	SIGSAK},
#endif
#ifdef SIGSEGV
  {  "SEGV",         	SIGSEGV},
#endif
#ifdef SIGSOUND
  {  "SOUND",        	SIGSOUND},
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
