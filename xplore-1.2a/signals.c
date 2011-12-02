
/* This stuff has mostly been adopted from xfm/moxfm. */

#include "xplore.h"
#include "signals.h"

static struct sigaction sigchld, sigterm, sigint;

#ifndef hpux
static struct sigaction sigsegv, sigbus;
#endif

/* clear up zombie processes */

static void sigchldHandler(int sig)
{
    waitpid(-1, NULL, WNOHANG);
}

#ifndef hpux

/* write message on SEGV or BUS signal */

static void sigfatalHandler(int sig)
{
    static const String msg =
    "\n*** xplore version %s ***\n"
    "You have come across a fatal bug in xplore -- something which shouldn't\n"
    "happen. ;-) Please consider submitting a bug report to the author:\n"
    "\tag@muwiinfa.geschichte.uni-mainz.de\n";

    fprintf(stderr_dup, msg, version);
    signal(sig, (void (*)(int))SIG_DFL);
    kill(getpid(), sig);
}

#endif

/* clean up on HUP and TERM signals */

static void sigtermHandler(int sig)
{
    quit();
}

void install_signal_handlers(void)
{
    sigchld.sa_handler = sigchldHandler;
    sigemptyset(&sigchld.sa_mask);
    sigchld.sa_flags = 0;
    sigaction(SIGCHLD, &sigchld, NULL);
    sigterm.sa_handler = sigtermHandler;
    sigemptyset(&sigterm.sa_mask);
    sigterm.sa_flags = 0;
    sigaction(SIGHUP, &sigterm, NULL);
    sigaction(SIGTERM, &sigterm, NULL);
#ifndef hpux
    sigsegv.sa_handler = sigfatalHandler;
    sigemptyset(&sigsegv.sa_mask);
    sigsegv.sa_flags = 0;
    sigaction(SIGSEGV, &sigsegv, NULL);
    sigbus.sa_handler = sigfatalHandler;
    sigemptyset(&sigbus.sa_mask);
    sigbus.sa_flags = 0;
    sigaction(SIGBUS, &sigbus, NULL);
#endif
}
