
#include "xplore.h"
#include "callbacks.h"
#include "dialogs.h"
#include "fileops.h"
#include "icons.h"
#include "interface.h"
#include "mnttable.h"
#include "parse.h"
#include "signals.h"
#include "util.h"

String version = VERSION;
char basedir[MAXPATHLEN+1];
char libdir[MAXPATHLEN+1];
char libconfig[MAXPATHLEN+1];
char libmagic[MAXPATHLEN+1];
char libsetup[MAXPATHLEN+1];
char libstartup[MAXPATHLEN+1];
char libicons[MAXPATHLEN+1];
char libschemes[MAXPATHLEN+1];

FILE *stdout_dup = NULL, *stderr_dup = NULL;
int *pipefd = NULL;

Boolean WaitFlag = False, MsgFlag = False;
static Boolean init_flag = False;

void Wait()
{
  if (init_flag && !WaitFlag) {
    XDefineCursor(display, XtWindow(app_shell), wait_cursor);
    XmUpdateDisplay(app_shell);
    WaitFlag = True;
  }
}

void WaitMsg(String msg)
{
  if (init_flag) {
    StatusMessage(msg);
    if (!WaitFlag)
      Wait();
    else
      XmUpdateDisplay(app_shell);
    MsgFlag = True;
  }
}

void WaitMsg2(String msg, String arg)
{
  char s[MAXPATHLEN+100];

  sprintf(s, msg, arg);
  WaitMsg(s);
}

void Done()
{
  int serrno = errno;
  if (init_flag && WaitFlag) {
    XUndefineCursor(display, XtWindow(app_shell));
    WaitFlag = False;
    if (MsgFlag) {
      RestoreStatusLine();
      MsgFlag = False;
    }
  }
  errno = serrno;
}

#define KEY_BREAK '\3' /* C-C */

Boolean CheckBreak(void)
{
  Boolean lock_save = lock;
  Window w = XtWindow(main_window);
  XEvent event;

  lock = True;
  while (XCheckTypedWindowEvent(display, w, KeyPress, &event)) {
    KeySym keysym;
    unsigned char c;
    int res = XLookupString(&(event.xkey), &c, 1, &keysym, NULL);

    if (res && c == KEY_BREAK) {
      lock = lock_save;
      return True;
    }
  }
  lock = lock_save;
  return False;
}

void cleanup(void)
{
    int d;

    /* save defaults */

    if (autosave)
      SaveDefaultsCB(app_shell, NULL, NULL);

    /* try to unmount all mounted devices */

    for (d = 0; d < ndevs(); d++)
	while (devcount(d) && devumount(d)) ;

    /* free up allocated resources */

    init_file_types();
    free_icons();

    /* close the pipe */

    if (pipefd) close(pipefd[0]);
}

void error(String msg)
{
    fprintf(stderr, "%s: %s\n", progname, msg);
    fflush(stderr);
}

void fatal(String msg)
{
    fprintf(stderr, "%s: %s\n", progname, msg);
    ClearSession();
    cleanup();
    exit_app(1);
}

void quit(void)
{
    ClearSession();
    cleanup();
    exit_app(0);
}

/* drain remaining log output to saved stderr (pilfered from a yet unreleased
   xfm version, thanks to Till Straumann!) */

static void log_drain(void)
{
  int n;
  char buf[BUFSIZ];

  /* flush the buffers (to the pipe) */
  fflush(stdout); fflush(stderr);

  /* now drain the pipe */
  while ( 0 < (n=read(pipefd[0],buf,BUFSIZ)) )
    fwrite(buf,sizeof(char),n,stderr_dup);

  fflush(stderr_dup);
}

/* set up stdout and stderr for capturing in console window or log pane */

static void redirect(void)
{
  int oldflags, stdout_dup_fd, stderr_dup_fd;

  /* create duplicates of the standard output streams */
  if ((stdout_dup_fd = dup(fileno(stdout))) == -1 ||
      !(stdout_dup = fdopen(stdout_dup_fd, "w")))
    fatal("cannot dup stdout");
  else if ((stderr_dup_fd = dup(fileno(stderr))) == -1 ||
	   !(stderr_dup = fdopen(stderr_dup_fd, "w")))
    fatal("cannot dup stderr");
  oldflags = fcntl(stdout_dup_fd, F_GETFD, 0);
  fcntl(stdout_dup_fd, F_SETFD, oldflags|FD_CLOEXEC);
  oldflags = fcntl(stderr_dup_fd, F_GETFD, 0);
  fcntl(stderr_dup_fd, F_SETFD, oldflags|FD_CLOEXEC);
  
  if (console) {
    freopen("/dev/console", "w", stdout);
    dup2(fileno(stdout), fileno(stderr));
  } else {
    if (!(pipefd = (int*)malloc(2*sizeof(int))) || pipe(pipefd) < 0)
      fatal("error opening pipe");
    dup2(pipefd[1], fileno(stdout));
    dup2(fileno(stdout), fileno(stderr));
    close(pipefd[1]);
    /* don't let our side of the pipe be inherited by child processes */
    oldflags = fcntl(pipefd[0], F_GETFD, 0);
    fcntl(pipefd[0], F_SETFD, oldflags|FD_CLOEXEC);
    /* set to nonblocking read */
    oldflags = fcntl(pipefd[0], F_GETFL, 0);
    fcntl(pipefd[0], F_SETFL, oldflags|O_NONBLOCK);
    /* drain the pipe at exit */
    atexit(log_drain);
  }
}

int main(int argc, char **argv)
{
    String s, progname = argv[0];

    stdout_dup = stdout; stderr_dup = stderr;

    /* initialize the application: */

    getcwd(basedir, MAXPATHLEN);
    if ((s = getenv("XPLORELIBDIR")))
      abspath(libdir, basedir, s);
    else {
      strcpy(libdir, XPLORELIBDIR);
      mysetenv("XPLORELIBDIR", XPLORELIBDIR, True);
    }
    sprintf(libconfig, "%s/%s", libdir, XPLORE_RC);
    sprintf(libmagic, "%s/%s", libdir, XPLORE_MAGIC);
    sprintf(libsetup, "%s/%s", libdir, XPLORE_SETUP);
    sprintf(libstartup, "%s/%s", libdir, XPLORE_STARTUP);
    sprintf(libicons, "%s/%s", libdir, XPLORE_ICONS);
    sprintf(libschemes, "%s/%s", libdir, XPLORE_SCHEMES);

    user_umask = umask(0);
    umask(user_umask);
    user_umask = 0777777 ^ user_umask;

    init_app(argc, argv);

    /* set the XPLORECONFIGDIR variable */
    if (*configdir)
      mysetenv("XPLORECONFIGDIR", configdir, True);

    /* process -help and -version options */
    if (showhelp) {
      printf(
"Usage: %s [options ...]\n\
Options:\n\
-C		redirect output streams to /dev/console\n\
-c config-dir	specify the name of the configuration directory\n\
-clone id	clone the given session\n\
-help		print this message and exit\n\
-nosm		disable X11R6 session management\n\
-o cpp-options	specify options to pass to the C preprocessor\n\
-p dir		specify the name of the initial directory\n\
-s shelf	specify the name of the initial shelf\n\
-session id	start or continue the given session\n\
-tree -file -shelf -log\n\
		specify the panes to be shown at startup\n\
-version	print version number and exit\n", progname);
      exit(0);
    } else if (showver) {
      printf("xplore version %s\n", VERSION);
      exit(0);
    }

    /* other initializations: */

    /* install signal handlers first, to avoid zombie processes */
    install_signal_handlers();
    /* perform shell test */
    shellTest(sh_list, shell);
    /* get the mount table */
    get_mnt_table();
    /* initialize the icons table */
    init_icons(display, ANY, iconpath);
    /* show splash screen */
    if (splash) {
      splash_on(xplore_logo());
      sleep(2);
    }
    /* parse magic and config files */
    if (exists(magicfile))
	magic_parse_file(magicfile);
    else
	magic_parse_file(libmagic);
    initdevs(); init_file_types(); init_menus();
    if (exists(configfile))
	parse_config_file(configfile, CPP, cpp_options);
    else
	parse_config_file(libconfig, CPP, cpp_options);
    parse_config_string(usertypes);
    /* provide reasonable defaults for command menu and shelf tabs if not
       set */
    if (!n_cmd_actions())
      default_cmd_menu();
    if (!n_shelves())
      default_shelf_menu(curshelf);

    /* create the application widgets: */

    splash_off();
    create_app();
    init_flag = True;

    /* run the application: */

    redirect();
    run_app();

}
