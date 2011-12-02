/* Created by makesig.pl
   for Linux muffin.peccary.sty 2.4.7-10smp #1 SMP Thu Sep 6 16:16:16 EDT 2001 i686 unknown 
   on Sat Jan 12 10:12:49 2002 using:
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
  {  "NO ACTION",     0},
#ifdef SIGHUP
  {  "HUP"      , SIGHUP },
#endif
#ifdef SIGTERM
  {  "TERM"     , SIGTERM },
#endif
#ifdef SIGKILL
  {  "KILL"     , SIGKILL },
#endif
#ifdef SIGSTOP
  {  "STOP"     , SIGSTOP },
#endif
#ifdef SIGCONT
  {  "CONT"     , SIGCONT },
#endif
#ifdef SIGQUIT
  {  "QUIT"     , SIGQUIT },
#endif
#ifdef SIGUSR1
  {  "USR1"     , SIGUSR1 },
#endif
#ifdef SIGABRT
  {  "ABRT"     , SIGABRT },
#endif
#ifdef SIGALRM
  {  "ALRM"     , SIGALRM },
#endif
#ifdef SIGBUS
  {  "BUS"      , SIGBUS },
#endif
#ifdef SIGCHLD
  {  "CHLD"     , SIGCHLD },
#endif
#ifdef SIGCLD
  {  "CLD"      , SIGCLD },
#endif
#ifdef SIGFPE
  {  "FPE"      , SIGFPE },
#endif
#ifdef SIGILL
  {  "ILL"      , SIGILL },
#endif
#ifdef SIGINT
  {  "INT"      , SIGINT },
#endif
#ifdef SIGIO
  {  "IO"       , SIGIO },
#endif
#ifdef SIGIOT
  {  "IOT"      , SIGIOT },
#endif
#ifdef SIGPIPE
  {  "PIPE"     , SIGPIPE },
#endif
#ifdef SIGPOLL
  {  "POLL"     , SIGPOLL },
#endif
#ifdef SIGPROF
  {  "PROF"     , SIGPROF },
#endif
#ifdef SIGPWR
  {  "PWR"      , SIGPWR },
#endif
#ifdef SIGSEGV
  {  "SEGV"     , SIGSEGV },
#endif
#ifdef SIGSTKFLT
  {  "STKFLT"   , SIGSTKFLT },
#endif
#ifdef SIGSYS
  {  "SYS"      , SIGSYS },
#endif
#ifdef SIGTRAP
  {  "TRAP"     , SIGTRAP },
#endif
#ifdef SIGTSTP
  {  "TSTP"     , SIGTSTP },
#endif
#ifdef SIGTTIN
  {  "TTIN"     , SIGTTIN },
#endif
#ifdef SIGTTOU
  {  "TTOU"     , SIGTTOU },
#endif
#ifdef SIGUNUSED
  {  "UNUSED"   , SIGUNUSED },
#endif
#ifdef SIGURG
  {  "URG"      , SIGURG },
#endif
#ifdef SIGUSR2
  {  "USR2"     , SIGUSR2 },
#endif
#ifdef SIGVTALRM
  {  "VTALRM"   , SIGVTALRM },
#endif
#ifdef SIGWINCH
  {  "WINCH"    , SIGWINCH },
#endif
#ifdef SIGXCPU
  {  "XCPU"     , SIGXCPU },
#endif
#ifdef SIGXFSZ
  {  "XFSZ"     , SIGXFSZ },
#endif
};

static gchar *signal_strings[][MAX_SIGNAL_NAME] = {
  {  "NO ACTION" },
#ifdef SIGHUP
  {  "HUP" },
#endif
#ifdef SIGTERM
  {  "TERM" },
#endif
#ifdef SIGKILL
  {  "KILL" },
#endif
#ifdef SIGSTOP
  {  "STOP" },
#endif
#ifdef SIGCONT
  {  "CONT" },
#endif
#ifdef SIGQUIT
  {  "QUIT" },
#endif
#ifdef SIGUSR1
  {  "USR1" },
#endif
#ifdef SIGABRT
  {  "ABRT" },
#endif
#ifdef SIGALRM
  {  "ALRM" },
#endif
#ifdef SIGBUS
  {  "BUS" },
#endif
#ifdef SIGCHLD
  {  "CHLD" },
#endif
#ifdef SIGCLD
  {  "CLD" },
#endif
#ifdef SIGFPE
  {  "FPE" },
#endif
#ifdef SIGILL
  {  "ILL" },
#endif
#ifdef SIGINT
  {  "INT" },
#endif
#ifdef SIGIO
  {  "IO" },
#endif
#ifdef SIGIOT
  {  "IOT" },
#endif
#ifdef SIGPIPE
  {  "PIPE" },
#endif
#ifdef SIGPOLL
  {  "POLL" },
#endif
#ifdef SIGPROF
  {  "PROF" },
#endif
#ifdef SIGPWR
  {  "PWR" },
#endif
#ifdef SIGSEGV
  {  "SEGV" },
#endif
#ifdef SIGSTKFLT
  {  "STKFLT" },
#endif
#ifdef SIGSYS
  {  "SYS" },
#endif
#ifdef SIGTRAP
  {  "TRAP" },
#endif
#ifdef SIGTSTP
  {  "TSTP" },
#endif
#ifdef SIGTTIN
  {  "TTIN" },
#endif
#ifdef SIGTTOU
  {  "TTOU" },
#endif
#ifdef SIGUNUSED
  {  "UNUSED" },
#endif
#ifdef SIGURG
  {  "URG" },
#endif
#ifdef SIGUSR2
  {  "USR2" },
#endif
#ifdef SIGVTALRM
  {  "VTALRM" },
#endif
#ifdef SIGWINCH
  {  "WINCH" },
#endif
#ifdef SIGXCPU
  {  "XCPU" },
#endif
#ifdef SIGXFSZ
  {  "XFSZ" },
#endif
};
